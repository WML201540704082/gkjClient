#include <initguid.h>
#include <userenv.h>
#include <WtsApi32.h>
#include <sddl.h> 
#include "myCredential.h"
#include "guid.h"
#include "myProvider.h"
#include "logger.h"

#include <tlhelp32.h>

#include <thread>
#include "sm4/sm4.h"
#include "sm4/hex.h"
#include "sm4/rand.h"
#include "sm4/error.h"

#include <vector>
#include <algorithm>

#include <UIAutomation.h>
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "OleAut32.lib")

// ============ 全局管道管理器（修改版）============
class GlobalPipeManager
{
private:
	static GlobalPipeManager* s_instance;
	static CRITICAL_SECTION s_cs;
	static bool s_csInitialized;

	std::thread _thread;
	HANDLE _hStopEvent;

	// 【修改】使用集合管理多个 Provider
	std::vector<myProvider*> _providers;
	CRITICAL_SECTION _csProviders;
	bool _bRunning;

	GlobalPipeManager()
	{
		InitializeCriticalSection(&_csProviders);
		_bRunning = true;
		_hStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
		_thread = std::thread(&GlobalPipeManager::PipeListenLoop, this);
		Logger::Log(L"[GlobalPipeManager] Created and started");
	}

	~GlobalPipeManager()
	{
		_bRunning = false;
		SetEvent(_hStopEvent);

		if (_thread.joinable())
		{
			_thread.join();
		}

		CloseHandle(_hStopEvent);
		DeleteCriticalSection(&_csProviders);
		Logger::Log(L"[GlobalPipeManager] Destroyed");
	}

	void PipeListenLoop()
	{
		Logger::Log(L"[GlobalPipeManager] Listen loop started");

		const uint8_t user_key[16] = { 0x35, 0x13, 0x41, 0x69, 0x09, 0xA1, 0xAD, 0x8E,
									  0x27, 0x85, 0x57, 0x76, 0x98, 0xBA, 0xDC, 0xFE };
		const uint8_t iv[16] = { 0xAD, 0x01, 0xC2, 0xB3, 0x64, 0x21, 0xCC, 0x77,
								0xFF, 0x12, 0x0A, 0x0D, 0xEC, 0x7B, 0x0E, 0x0F };
		SM4_KEY decrypt_key;
		sm4_set_decrypt_key(&decrypt_key, user_key);

		while (_bRunning)
		{
			HANDLE hPipe = CreateNamedPipeW(
				L"\\\\.\\pipe\\MyCredentialPipe",
				PIPE_ACCESS_INBOUND,
				PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
				PIPE_UNLIMITED_INSTANCES,
				256 * sizeof(wchar_t),
				256 * sizeof(wchar_t),
				1000,
				nullptr
			);

			if (hPipe == INVALID_HANDLE_VALUE)
			{
				Logger::Log(L"[GlobalPipeManager] ERROR: Failed to create pipe");
				Sleep(1000);
				continue;
			}

			Logger::Log(L"[GlobalPipeManager] Waiting for connection...");

			if (!ConnectNamedPipe(hPipe, nullptr))
			{
				DWORD err = GetLastError();
				if (err != ERROR_PIPE_CONNECTED)
				{
					Logger::Log(L"[GlobalPipeManager] ConnectNamedPipe failed: " + std::to_wstring(err));
					CloseHandle(hPipe);
					Sleep(100);
					continue;
				}
			}

			Logger::Log(L"[GlobalPipeManager] Pipe connected");

			wchar_t buffer[256];
			DWORD bytesRead;

			if (ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, nullptr))
			{
				uint8_t encrypted[256];
				memcpy(encrypted, buffer, bytesRead);

				uint8_t decrypted[256];
				size_t decryptedLen = 0;

				if (sm4_cbc_padding_decrypt(&decrypt_key, iv, encrypted,
					bytesRead, decrypted, &decryptedLen) == 1)
				{
					wchar_t decryptedPassword[256];
					memcpy(decryptedPassword, decrypted, decryptedLen);
					decryptedPassword[decryptedLen / sizeof(wchar_t)] = L'\0';

					Logger::Log(L"[GlobalPipeManager] Password decrypted successfully");
					ProcessPassword(decryptedPassword);
				}
				else
				{
					Logger::Log(L"[GlobalPipeManager] ERROR: Decryption failed");
				}
			}
			else
			{
				Logger::Log(L"[GlobalPipeManager] ERROR: ReadFile failed");
			}

			DisconnectNamedPipe(hPipe);
			CloseHandle(hPipe);
		}

		Logger::Log(L"[GlobalPipeManager] Listen loop ended");
	}

	void ProcessPassword(const wchar_t* password)
	{
		EnterCriticalSection(&_csProviders);

		myProvider* pValidProvider = nullptr;

		for (auto it = _providers.begin(); it != _providers.end(); ++it)
		{
			myProvider* p = *it;
			if (p != nullptr && p->_pCredential != nullptr)
			{
				pValidProvider = p;
				Logger::Log(L"[GlobalPipeManager] Found valid provider with credential");
				break;
			}
		}

		if (pValidProvider == nullptr)
		{
			Logger::Log(L"[GlobalPipeManager] WARNING: No valid provider found");
			Logger::Log(L"[GlobalPipeManager] Total providers: " + std::to_wstring(_providers.size()));
			LeaveCriticalSection(&_csProviders);
			return;
		}

		// 更新密码
		pValidProvider->_pCredential->SetStringValue(SFI_PASSWORD, password);
		Logger::Log(L"[GlobalPipeManager] Password set to credential");

		// 更新UI
		if (pValidProvider->_pCredential->_pCredProvCredentialEvents)
		{
			pValidProvider->_pCredential->_pCredProvCredentialEvents->BeginFieldUpdates();
			pValidProvider->_pCredential->_pCredProvCredentialEvents->SetFieldString(
				pValidProvider->_pCredential,
				SFI_PASSWORD,
				password
			);
			pValidProvider->_pCredential->_pCredProvCredentialEvents->SetFieldInteractiveState(
				pValidProvider->_pCredential,
				SFI_SUBMIT_BUTTON,
				CPFIS_FOCUSED
			);
			pValidProvider->_pCredential->_pCredProvCredentialEvents->EndFieldUpdates();
			Logger::Log(L"[GlobalPipeManager] UI updated");
		}

		LeaveCriticalSection(&_csProviders);

		// 启动智能确认线程
		std::thread([this]() {
			Logger::Log(L"[ConfirmationThread] Started with smart detection");

			// 第一次提交（填充密码后的初始提交）
			Sleep(200);  // 缩短到 200ms
			SendEnterKey("Initial submission");

			// 智能等待确认界面并点击
			if (WaitAndClickLoginButton(3000))  // 最多等待 3 秒
			{
				Logger::Log(L"[ConfirmationThread] Successfully clicked login button");
			}
			else
			{
				Logger::Log(L"[ConfirmationThread] Timeout, sending fallback ENTER");
				// 如果检测超时，发送键盘事件作为后备
				SendEnterKey("Fallback");
			}

			Logger::Log(L"[ConfirmationThread] Completed");
		}).detach();
	}

	// 【新增】智能等待并点击登录按钮
	bool WaitAndClickLoginButton(DWORD timeoutMs)
	{
		Logger::Log(L"[WaitAndClick] Starting, timeout: " + std::to_wstring(timeoutMs) + L"ms");

		DWORD startTime = GetTickCount();
		const DWORD checkInterval = 50;  // 每 50ms 检查一次

		while (GetTickCount() - startTime < timeoutMs)
		{
			// 尝试使用 UI Automation 查找并点击按钮
			if (TryClickLoginButtonUIA())
			{
				Logger::Log(L"[WaitAndClick] Button clicked successfully");
				return true;
			}

			Sleep(checkInterval);
		}

		Logger::Log(L"[WaitAndClick] Timeout reached");
		return false;
	}

	// 【优化】尝试点击登录按钮（单次尝试）
	bool TryClickLoginButtonUIA()
	{
		static bool s_comInitialized = false;
		if (!s_comInitialized)
		{
			HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
			s_comInitialized = SUCCEEDED(hr);
		}

		IUIAutomation* pAutomation = nullptr;
		HRESULT hr = CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IUIAutomation), (void**)&pAutomation);

		if (FAILED(hr) || pAutomation == nullptr)
		{
			return false;
		}

		bool success = false;
		IUIAutomationElement* pRoot = nullptr;
		hr = pAutomation->GetRootElement(&pRoot);

		if (SUCCEEDED(hr) && pRoot != nullptr)
		{
			// 创建条件：查找按钮
			VARIANT varProp;
			varProp.vt = VT_I4;
			varProp.lVal = UIA_ButtonControlTypeId;

			IUIAutomationCondition* pCondition = nullptr;
			hr = pAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId,
				varProp, &pCondition);

			if (SUCCEEDED(hr) && pCondition != nullptr)
			{
				IUIAutomationElementArray* pButtons = nullptr;
				hr = pRoot->FindAll(TreeScope_Descendants, pCondition, &pButtons);

				if (SUCCEEDED(hr) && pButtons != nullptr)
				{
					int buttonCount = 0;
					pButtons->get_Length(&buttonCount);

					// 遍历按钮
					for (int i = 0; i < buttonCount && !success; i++)
					{
						IUIAutomationElement* pButton = nullptr;
						hr = pButtons->GetElement(i, &pButton);

						if (SUCCEEDED(hr) && pButton != nullptr)
						{
							BSTR buttonName = nullptr;
							pButton->get_CurrentName(&buttonName);

							if (buttonName != nullptr)
							{
								std::wstring name(buttonName);

								// 匹配登录按钮（扩展匹配规则）
								if (name.find(L"登录") != std::wstring::npos ||
									name.find(L"Sign in") != std::wstring::npos ||
									name.find(L"Submit") != std::wstring::npos ||
									name.find(L"确定") != std::wstring::npos ||
									name.find(L"OK") != std::wstring::npos ||
									name.find(L"Go") != std::wstring::npos ||
									name.empty())  // 有些按钮可能没有名称
								{
									// 检查按钮是否可用
									BOOL isEnabled = FALSE;
									pButton->get_CurrentIsEnabled(&isEnabled);

									if (isEnabled)
									{
										Logger::Log(L"[TryClick] Found enabled button: " +
											(name.empty() ? L"<unnamed>" : name));

										// 点击按钮
										IUIAutomationInvokePattern* pInvoke = nullptr;
										hr = pButton->GetCurrentPatternAs(UIA_InvokePatternId,
											__uuidof(IUIAutomationInvokePattern),
											(void**)&pInvoke);

										if (SUCCEEDED(hr) && pInvoke != nullptr)
										{
											pInvoke->Invoke();
											success = true;
											pInvoke->Release();
										}
									}
								}

								SysFreeString(buttonName);
							}

							pButton->Release();
						}
					}

					pButtons->Release();
				}

				pCondition->Release();
			}

			pRoot->Release();
		}

		pAutomation->Release();
		return success;
	}

	// 【辅助】发送回车键
	void SendEnterKey(const char* stage)
	{
		Logger::Log(L"[SendEnter] " + std::wstring(stage, stage + strlen(stage)));

		// 激活窗口
		HWND hwnd = FindLogonUIWindow();
		if (hwnd != NULL)
		{
			SetForegroundWindow(hwnd);
			Sleep(30);  // 缩短延迟
		}

		// 发送回车
		INPUT inputs[2] = {};
		inputs[0].type = INPUT_KEYBOARD;
		inputs[0].ki.wVk = VK_RETURN;
		inputs[0].ki.dwFlags = 0;

		inputs[1].type = INPUT_KEYBOARD;
		inputs[1].ki.wVk = VK_RETURN;
		inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

		UINT sent = SendInput(2, inputs, sizeof(INPUT));
		Logger::Log(L"[SendEnter] SendInput result: " + std::to_wstring(sent));
	}

	// 【新增】查找 LogonUI 窗口
	HWND FindLogonUIWindow()
	{
		// 尝试多个可能的窗口类名和标题
		const wchar_t* classNames[] = {
			L"Credential Dialog Xaml Host",  // Win10 1809+
			L"Windows.UI.Core.CoreWindow",   // Win10
			L"LogonUI Logon Window",         // 旧版本
			NULL
		};

		for (int i = 0; classNames[i] != NULL; i++)
		{
			HWND hwnd = FindWindowW(classNames[i], NULL);
			if (hwnd != NULL)
			{
				Logger::Log(L"[GlobalPipeManager] Found window class: " + std::wstring(classNames[i]));
				return hwnd;
			}
		}

		// 如果找不到，枚举所有顶层窗口
		HWND hwnd = GetForegroundWindow();
		if (hwnd != NULL)
		{
			wchar_t className[256];
			GetClassNameW(hwnd, className, 256);
			Logger::Log(L"[GlobalPipeManager] Foreground window class: " + std::wstring(className));
			return hwnd;
		}

		return NULL;
	}

	//// 【新增】尝试点击提交按钮
	//void TryClickSubmitButton(HWND hwndParent)
	//{
	//	// 枚举子窗口，查找提交按钮
	//	HWND hwndButton = FindWindowExW(hwndParent, NULL, L"Button", NULL);
	//	if (hwndButton != NULL)
	//	{
	//		Logger::Log(L"[GlobalPipeManager] Found submit button, sending click");

	//		// 发送点击消息
	//		SendMessageW(hwndButton, BM_CLICK, 0, 0);

	//		// 或者使用 PostMessage
	//		// PostMessageW(hwndButton, WM_LBUTTONDOWN, MK_LBUTTON, 0);
	//		// PostMessageW(hwndButton, WM_LBUTTONUP, 0, 0);
	//	}
	//	else
	//	{
	//		Logger::Log(L"[GlobalPipeManager] Submit button not found");
	//	}
	//}


public:
	static GlobalPipeManager* GetInstance()
	{
		if (!s_csInitialized)
		{
			InitializeCriticalSection(&s_cs);
			s_csInitialized = true;
		}

		EnterCriticalSection(&s_cs);
		if (s_instance == nullptr)
		{
			s_instance = new GlobalPipeManager();
		}
		LeaveCriticalSection(&s_cs);
		return s_instance;
	}

	void RegisterProvider(myProvider* pProvider)
	{
		EnterCriticalSection(&_csProviders);

		// 避免重复注册
		auto it = std::find(_providers.begin(), _providers.end(), pProvider);
		if (it == _providers.end())
		{
			_providers.push_back(pProvider);
			Logger::Log(L"[GlobalPipeManager] Provider registered (total: " +
				std::to_wstring(_providers.size()) + L")");
		}

		LeaveCriticalSection(&_csProviders);
	}

	void UnregisterProvider(myProvider* pProvider)
	{
		EnterCriticalSection(&_csProviders);

		auto it = std::find(_providers.begin(), _providers.end(), pProvider);
		if (it != _providers.end())
		{
			_providers.erase(it);
			Logger::Log(L"[GlobalPipeManager] Provider unregistered (remaining: " +
				std::to_wstring(_providers.size()) + L")");
		}

		LeaveCriticalSection(&_csProviders);
	}
};

// 静态成员初始化
GlobalPipeManager* GlobalPipeManager::s_instance = nullptr;
CRITICAL_SECTION GlobalPipeManager::s_cs;
bool GlobalPipeManager::s_csInitialized = false;


myProvider::myProvider() :
	_cRef(1),
	_pCredential(nullptr),
	_pCredProviderUserArray(nullptr),
	_pCredProviderEvents(nullptr),
	_upAdviseContext(0)
{
	Logger::Log(L"myProvider::myProvider()");
	DllAddRef();

	InitializeCriticalSection(&_csEvents);

	// 【新增】注册到全局管道管理器
	GlobalPipeManager::GetInstance()->RegisterProvider(this);

	_unlockScreenPath = GetUnlockScreenPathFromRegistry();
}

myProvider::~myProvider()
{
	Logger::Log(L"myProvider::~myProvider()");

	// 【新增】从全局管道管理器注销
	GlobalPipeManager::GetInstance()->UnregisterProvider(this);

	if (_pCredential != nullptr)
	{
		_pCredential->Release();
		_pCredential = nullptr;
	}

	if (_pCredProviderUserArray != nullptr)
	{
		_pCredProviderUserArray->Release();
		_pCredProviderUserArray = nullptr;
	}

	if (_pCredProviderEvents != nullptr)
	{
		_pCredProviderEvents->Release();
		_pCredProviderEvents = nullptr;
	}

	DeleteCriticalSection(&_csEvents);
	DllRelease();
}

void myProvider::_ReleaseEnumeratedCredentials()
{
	Logger::Log(L"myProvider::_ReleaseEnumeratedCredentials()");
	if (_pCredential != nullptr)
	{
		_pCredential->Release();
		_pCredential = nullptr;
	}
}

void myProvider::_CreateEnumeratedCredentials()
{
	Logger::Log(L"myProvider::_CreateEnumeratedCredentials()");
	switch (_cpus)
	{
	case CPUS_LOGON:
	case CPUS_UNLOCK_WORKSTATION:
	{
		_EnumerateCredentials();
		break;
	}
	default:
		break;
	}
}

HRESULT myProvider::_EnumerateCredentials()
{
	Logger::Log(L"_EnumerateCredentials - Enter");
	HRESULT hr = E_UNEXPECTED;

	if (_pCredProviderUserArray != nullptr)
	{
		DWORD dwUserCount;
		_pCredProviderUserArray->GetCount(&dwUserCount);
		if (dwUserCount > 0)
		{
			ICredentialProviderUser *pCredUser;
			hr = _pCredProviderUserArray->GetAt(0, &pCredUser);
			if (SUCCEEDED(hr))
			{
				_pCredential = new(std::nothrow) myCredential(this);
				if (_pCredential != nullptr)
				{
					hr = _pCredential->Initialize(_cpus, s_rgCredProvFieldDescriptors, s_rgFieldStatePairs, pCredUser);
					if (FAILED(hr))
					{
						_pCredential->Release();
						_pCredential = nullptr;
					}
				}
				else
				{
					hr = E_OUTOFMEMORY;
				}
				pCredUser->Release();
			}
		}
	}
	else
	{
		Logger::Log(L"ERROR: _pCredProviderUserArray is NULL!");
	}
	return hr;
}

// 检查指定进程是否正在运行
bool IsProcessRunning(const wchar_t* processName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	PROCESSENTRY32W pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32W);

	if (!Process32FirstW(hSnapshot, &pe32))
	{
		CloseHandle(hSnapshot);
		return false;
	}

	bool found = false;
	do
	{
		if (_wcsicmp(pe32.szExeFile, processName) == 0)
		{
			found = true;
			break;
		}
	} while (Process32NextW(hSnapshot, &pe32));

	CloseHandle(hSnapshot);
	return found;
}

// 终止指定名称的进程
void TerminateProcessByName(const wchar_t* processName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return;
	}

	PROCESSENTRY32W pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32W);

	if (!Process32FirstW(hSnapshot, &pe32))
	{
		CloseHandle(hSnapshot);
		return;
	}

	do
	{
		if (_wcsicmp(pe32.szExeFile, processName) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
			if (hProcess != nullptr)
			{
				TerminateProcess(hProcess, 0);
				CloseHandle(hProcess);
			}
		}
	} while (Process32NextW(hSnapshot, &pe32));

	CloseHandle(hSnapshot);
}

std::wstring myProvider::GetUnlockScreenPathFromRegistry()
{
	Logger::Log(L"myProvider::GetUnlockScreenPathFromRegistry()");
	HKEY hKey;
	wchar_t path[MAX_PATH];
	DWORD pathLen = sizeof(path);
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\LnsoftFaceLogin",
		0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey, L"UnlockScreenPath", nullptr, nullptr,
			(LPBYTE)path, &pathLen) == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return path;
		}
		RegCloseKey(hKey);
	}
	return L"";
}

IFACEMETHODIMP myProvider::SetUsageScenario(
	CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
	DWORD dwFlags)
{
	Logger::Log(L"myProvider::SetUsageScenario");

	HRESULT hr;

	switch (cpus)
	{
	case CPUS_LOGON:
	case CPUS_UNLOCK_WORKSTATION:
		_cpus = cpus;
		_fRecreateEnumeratedCredentials = true;

		// 添加：如果已有用户数组，立即创建凭据
		if (_pCredProviderUserArray != nullptr)
		{
			_ReleaseEnumeratedCredentials();
			_CreateEnumeratedCredentials();
			_fRecreateEnumeratedCredentials = false;
		}

		// ===== 新增：启动 unlockScreen.exe =====
		{
			// 检查并终止已有进程
			const wchar_t* targetProcess = L"unlockScreen.exe";
			if (IsProcessRunning(targetProcess))
			{
				Logger::Log(L"Terminating existing unlockScreen.exe");
				TerminateProcessByName(targetProcess);
				Sleep(500);
			}

			// 从注册表读取路径
			std::wstring unlockScreenPath = GetUnlockScreenPathFromRegistry();
			if (!unlockScreenPath.empty())
			{
				Logger::Log(L"Launching unlockScreen.exe from SetUsageScenario");

				// 使用 ShellExecuteW 启动（适用于 UNLOCK 场景）
				if (cpus == CPUS_UNLOCK_WORKSTATION)
				{
					ShellExecuteW(nullptr, L"open", unlockScreenPath.c_str(),
						nullptr, nullptr, SW_SHOWNORMAL);
				}
				// LOGON 场景可能需要特殊处理（见方案二）
				else if (cpus == CPUS_LOGON)
				{
					// 开机登录场景，可以尝试直接启动
					ShellExecuteW(nullptr, L"open", unlockScreenPath.c_str(),
						nullptr, nullptr, SW_SHOWNORMAL);
				}
			}
			else
			{
				Logger::Log(L"ERROR: unlockScreenPath is empty");
			}
		}
		// ===== 启动逻辑结束 =====

		hr = S_OK;
		break;

	case CPUS_CHANGE_PASSWORD:
	case CPUS_CREDUI:
		hr = E_NOTIMPL;
		break;

	default:
		hr = E_INVALIDARG;
		break;
	}

	return hr;
}


// SetUsageScenario is the provider's cue that it's going to be asked for tiles
// in a subsequent call.
//IFACEMETHODIMP myProvider::SetUsageScenario(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD dwFlags)
//{
//	Logger::Log(L"myProvider::SetUsageScenario");
//
//	HRESULT hr;
//
//	// Decide which scenarios to support here. Returning E_NOTIMPL simply tells the caller
//	// that we're not designed for that scenario.
//	switch (cpus)
//	{
//	case CPUS_LOGON:
//	case CPUS_UNLOCK_WORKSTATION:
//		// The reason why we need _fRecreateEnumeratedCredentials is because ICredentialProviderSetUserArray::SetUserArray() is called after ICredentialProvider::SetUsageScenario(),
//		// while we need the ICredentialProviderUserArray during enumeration in ICredentialProvider::GetCredentialCount()
//		_cpus = cpus;
//		_fRecreateEnumeratedCredentials = true;
//		hr = S_OK;
//		break;
//
//	case CPUS_CHANGE_PASSWORD:
//	case CPUS_CREDUI:
//		hr = E_NOTIMPL;
//		break;
//
//	default:
//		hr = E_INVALIDARG;
//		break;
//	}
//
//	return hr;
//}

IFACEMETHODIMP myProvider::SetSerialization(CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION const * pcpcs)
{
	Logger::Log(L"myProvider::SetSerialization");
	return E_NOTIMPL;
}

IFACEMETHODIMP myProvider::Advise(ICredentialProviderEvents * pcpe, UINT_PTR upAdviseContext)
{
	Logger::Log(L"myProvider::Advise");

	EnterCriticalSection(&_csEvents);

	if (_pCredProviderEvents != nullptr)
	{
		_pCredProviderEvents->Release();
	}
	_pCredProviderEvents = pcpe;
	_pCredProviderEvents->AddRef();
	_upAdviseContext = upAdviseContext;
	LeaveCriticalSection(&_csEvents);
	return S_OK;
}

HRESULT myProvider::UnAdvise()
{
	Logger::Log(L"myProvider::UnAdvise");
	EnterCriticalSection(&_csEvents);
	if (_pCredProviderEvents != nullptr)
	{
		_pCredProviderEvents->Release();
		_pCredProviderEvents = nullptr;
	}
	_upAdviseContext = 0;
	LeaveCriticalSection(&_csEvents);
	return S_OK;
	//return E_NOTIMPL;
}

IFACEMETHODIMP myProvider::GetFieldDescriptorCount(DWORD * pdwCount)
{
	Logger::Log(L"myProvider::GetFieldDescriptorCount");
	*pdwCount = SFI_NUM_FIELDS;
	return S_OK;
}

IFACEMETHODIMP myProvider::GetFieldDescriptorAt(
	DWORD dwIndex,
	_Outptr_result_nullonfailure_ CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR **ppcpfd)
{
	Logger::Log(L"myProvider::GetFieldDescriptorAt");
	HRESULT hr;
	*ppcpfd = nullptr;

	// Verify dwIndex is a valid field.
	if ((dwIndex < SFI_NUM_FIELDS) && ppcpfd)
	{
		hr = FieldDescriptorCoAllocCopy(s_rgCredProvFieldDescriptors[dwIndex], ppcpfd);
	}
	else
	{
		hr = E_INVALIDARG;
	}

	return hr;
}

//IFACEMETHODIMP myProvider::GetCredentialCount(DWORD * pdwCount, DWORD * pdwDefault, BOOL * pbAutoLogonWithDefault)
//{
//	Logger::Log(L"GetCredentialCount - Enter");
//
//	if (_fRecreateEnumeratedCredentials)
//	{
//		Logger::Log(L"Recreating credentials...");
//		_fRecreateEnumeratedCredentials = false;
//		_ReleaseEnumeratedCredentials();
//		_CreateEnumeratedCredentials();
//	}
//
//	*pdwCount = (_pCredential != nullptr) ? 1 : 0;
//
//	if (_pCredential != nullptr)
//	{
//		*pdwDefault = 0;
//		*pbAutoLogonWithDefault = FALSE;
//		Logger::Log(L"Credential ready, set as default");
//	}
//	else
//	{
//		*pdwDefault = CREDENTIAL_PROVIDER_NO_DEFAULT;
//		*pbAutoLogonWithDefault = FALSE;
//		Logger::Log(L"WARNING: Credential is NULL!");
//	}
//
//	return S_OK;
//}

IFACEMETHODIMP myProvider::GetCredentialCount(DWORD * pdwCount, DWORD * pdwDefault, BOOL * pbAutoLogonWithDefault)
{
	Logger::Log(L"GetCredentialCount called. Scenario: " + std::to_wstring(_cpus));

	

	if (_fRecreateEnumeratedCredentials)
	{
		_fRecreateEnumeratedCredentials = false;
		_ReleaseEnumeratedCredentials();
		_CreateEnumeratedCredentials();
	}

	*pdwCount = 1;
	*pdwDefault = 0/*CREDENTIAL_PROVIDER_NO_DEFAULT*/;
	//*pbAutoLogonWithDefault = TRUE;

	//*pdwDefault = CREDENTIAL_PROVIDER_NO_DEFAULT;
	*pbAutoLogonWithDefault = FALSE;

	return S_OK;
}

IFACEMETHODIMP myProvider::GetCredentialAt(
	DWORD dwIndex,
	_Outptr_result_nullonfailure_ ICredentialProviderCredential **ppcpc)
{
	Logger::Log(L"GetCredentialAt called. Index: " + std::to_wstring(dwIndex));
	HRESULT hr = E_INVALIDARG;
	*ppcpc = nullptr;

	if ((dwIndex == 0) && ppcpc)
	{
		hr = _pCredential->QueryInterface(IID_PPV_ARGS(ppcpc));
	}
	return hr;
}

IFACEMETHODIMP myProvider::SetUserArray(ICredentialProviderUserArray * users)
{
	Logger::Log(L"myProvider::SetUserArray");
	if (_pCredProviderUserArray)
	{
		_pCredProviderUserArray->Release();
	}
	_pCredProviderUserArray = users;
	_pCredProviderUserArray->AddRef();
	return S_OK;
}

HRESULT my_CreateInstance(REFIID riid, void ** ppv)
{
	Logger::Log(L"myProvider::my_CreateInstance");
	HRESULT hr;
	myProvider *pProvider = new(std::nothrow) myProvider();
	if (pProvider)
	{
		hr = pProvider->QueryInterface(riid, ppv);
		pProvider->Release();
	}
	else
	{
		hr = E_OUTOFMEMORY;
	}
	return hr;
}
