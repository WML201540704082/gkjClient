#ifndef WIN32_NO_STATUS
#include <ntstatus.h>
#define WIN32_NO_STATUS
#endif
#include <unknwn.h>
#include <userenv.h>
#include <WtsApi32.h>
#include <sddl.h> 
#include <tlhelp32.h>
#include <string>
#include "myCredential.h"
#include "guid.h"
#include "myProvider.h"

#include "sm4/sm4.h"
#include "sm4/hex.h"
#include "sm4/rand.h"
#include "sm4/error.h"

#include "logger.h"

#pragma comment(lib, "Wtsapi32.lib")

HRESULT myCredential::Initialize(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR const * rgcpfd, FIELD_STATE_PAIR const * rgfsp, ICredentialProviderUser * pcpUser)
{
	Logger::Log(L"myCredential::Initialize");

	HRESULT hr = S_OK;
	_cpus = cpus;

	GUID guidProvider;
	pcpUser->GetProviderID(&guidProvider);
	_fIsLocalUser = (guidProvider == Identity_LocalUserProvider);

	// Copy the field descriptors for each field. This is useful if you want to vary the field
	// descriptors based on what Usage scenario the credential was created for.
	for (DWORD i = 0; SUCCEEDED(hr) && i < ARRAYSIZE(_rgCredProvFieldDescriptors); i++)
	{
		_rgFieldStatePairs[i] = rgfsp[i];
		hr = FieldDescriptorCopy(rgcpfd[i], &_rgCredProvFieldDescriptors[i]);
	}

	// Initialize the String value of all the fields.
	if (SUCCEEDED(hr))
	{
		//hr = SHStrDupW(L"Sample Credential", &_rgFieldStrings[SFI_LABEL]); 
		hr = SHStrDupW(L"办公终端人脸识别", &_rgFieldStrings[SFI_LABEL]);
	}
	if (SUCCEEDED(hr))
	{
		hr = SHStrDupW(L"Sample Credential Provider", &_rgFieldStrings[SFI_LARGE_TEXT]);
	}
	if (SUCCEEDED(hr))
	{
		hr = SHStrDupW(L"Edit Text", &_rgFieldStrings[SFI_EDIT_TEXT]);
	}
	if (SUCCEEDED(hr))
	{
		hr = SHStrDupW(L"", &_rgFieldStrings[SFI_PASSWORD]);
	}
	if (SUCCEEDED(hr))
	{
		hr = SHStrDupW(L"Submit", &_rgFieldStrings[SFI_SUBMIT_BUTTON]);
	}
	if (SUCCEEDED(hr))
	{
		hr = SHStrDupW(L"Checkbox", &_rgFieldStrings[SFI_CHECKBOX]);
	}
	if (SUCCEEDED(hr))
	{
		hr = SHStrDupW(L"Combobox", &_rgFieldStrings[SFI_COMBOBOX]);
	}
	if (SUCCEEDED(hr))
	{
		//hr = SHStrDupW(L"Launch helper window", &_rgFieldStrings[SFI_LAUNCHWINDOW_LINK]);
		hr = SHStrDupW(L"办公终端人脸识别", &_rgFieldStrings[SFI_LAUNCHWINDOW_LINK]);
	}
	if (SUCCEEDED(hr))
	{
		hr = SHStrDupW(L"Hide additional controls", &_rgFieldStrings[SFI_HIDECONTROLS_LINK]);
	}
	if (SUCCEEDED(hr))
	{
		hr = pcpUser->GetStringValue(PKEY_Identity_QualifiedUserName, &_pszQualifiedUserName);
	}
	if (SUCCEEDED(hr))
	{
		PWSTR pszUserName;
		pcpUser->GetStringValue(PKEY_Identity_UserName, &pszUserName);
		if (pszUserName != nullptr)
		{
			wchar_t szString[256];
			StringCchPrintf(szString, ARRAYSIZE(szString), L"User Name: %s", pszUserName);
			hr = SHStrDupW(szString, &_rgFieldStrings[SFI_FULLNAME_TEXT]);
			CoTaskMemFree(pszUserName);
		}
		else
		{
			hr = SHStrDupW(L"User Name is NULL", &_rgFieldStrings[SFI_FULLNAME_TEXT]);
		}
	}
	if (SUCCEEDED(hr))
	{
		PWSTR pszDisplayName;
		pcpUser->GetStringValue(PKEY_Identity_DisplayName, &pszDisplayName);
		if (pszDisplayName != nullptr)
		{
			wchar_t szString[256];
			StringCchPrintf(szString, ARRAYSIZE(szString), L"Display Name: %s", pszDisplayName);
			hr = SHStrDupW(szString, &_rgFieldStrings[SFI_DISPLAYNAME_TEXT]);
			CoTaskMemFree(pszDisplayName);
		}
		else
		{
			hr = SHStrDupW(L"Display Name is NULL", &_rgFieldStrings[SFI_DISPLAYNAME_TEXT]);
		}
	}
	if (SUCCEEDED(hr))
	{
		PWSTR pszLogonStatus;
		pcpUser->GetStringValue(PKEY_Identity_LogonStatusString, &pszLogonStatus);
		if (pszLogonStatus != nullptr)
		{
			wchar_t szString[256];
			StringCchPrintf(szString, ARRAYSIZE(szString), L"Logon Status: %s", pszLogonStatus);
			hr = SHStrDupW(szString, &_rgFieldStrings[SFI_LOGONSTATUS_TEXT]);
			CoTaskMemFree(pszLogonStatus);
		}
		else
		{
			hr = SHStrDupW(L"Logon Status is NULL", &_rgFieldStrings[SFI_LOGONSTATUS_TEXT]);
		}
	}

	if (SUCCEEDED(hr))
	{
		hr = pcpUser->GetSid(&_pszUserSid);
	}

	//ShellExecuteW(nullptr, L"open", L"C:\\test\\release\\myUnlockDemo.exe", nullptr, nullptr, SW_SHOWNORMAL);
	//ShellExecuteW(nullptr, L"open", L"C:\\test\\lnsoft-otfr\\unlockScreen.exe", nullptr, nullptr, SW_SHOWNORMAL);

	return hr;
}

myCredential::myCredential(myProvider * pProvider) :
	_cRef(1),
	_pCredProvCredentialEvents(nullptr),
	_pszUserSid(nullptr),
	_pszQualifiedUserName(nullptr),
	_fIsLocalUser(false),
	_fChecked(false),
	_fShowControls(false),
	_dwComboIndex(0)
	//_bProcessLaunched(false),    // 新增
	//_dwLaunchTime(0)             // 新增
{
	Logger::Log(L"myCredential::myCredential");
	DllAddRef();

	ZeroMemory(_rgCredProvFieldDescriptors, sizeof(_rgCredProvFieldDescriptors));
	ZeroMemory(_rgFieldStatePairs, sizeof(_rgFieldStatePairs));
	ZeroMemory(_rgFieldStrings, sizeof(_rgFieldStrings));

	unlockScreenPath = GetUnlockScreenPathFromRegistry();
	//_pipeThread = std::thread(ListenForPassword, this);
	//_pipeThread.detach();
}

myCredential::~myCredential()
{
	Logger::Log(L"myCredential::~myCredential");

	if (_rgFieldStrings[SFI_PASSWORD])
	{
		size_t lenPassword = wcslen(_rgFieldStrings[SFI_PASSWORD]);
		SecureZeroMemory(_rgFieldStrings[SFI_PASSWORD], lenPassword * sizeof(*_rgFieldStrings[SFI_PASSWORD]));
	}
	for (int i = 0; i < ARRAYSIZE(_rgFieldStrings); i++)
	{
		CoTaskMemFree(_rgFieldStrings[i]);
		CoTaskMemFree(_rgCredProvFieldDescriptors[i].pszLabel);
	}
	CoTaskMemFree(_pszUserSid);
	CoTaskMemFree(_pszQualifiedUserName);
	DllRelease();
}

//void myCredential::ListenForPassword(myCredential * pCredential)
//{
//	const uint8_t user_key[16] = { 0x35, 0x13, 0x41, 0x69, 0x09, 0xA1, 0xAD, 0x8E,
//								  0x27, 0x85, 0x57, 0x76, 0x98, 0xBA, 0xDC, 0xFE };
//	const uint8_t iv[16] = { 0xAD, 0x01, 0xC2, 0xB3, 0x64, 0x21, 0xCC, 0x77,
//							0xFF, 0x12, 0x0A, 0x0D, 0xEC, 0x7B, 0x0E, 0x0F };
//	SM4_KEY decrypt_key;
//	sm4_set_decrypt_key(&decrypt_key, user_key);
//
//	HANDLE hPipe;
//	wchar_t buffer[256];
//	DWORD bytesRead;
//
//	hPipe = CreateNamedPipeW(
//		L"\\\\.\\pipe\\MyCredentialPipe",
//		PIPE_ACCESS_INBOUND,
//		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
//		1,
//		256 * sizeof(wchar_t),
//		256 * sizeof(wchar_t),
//		0,
//		nullptr
//	);
//
//	if (hPipe == INVALID_HANDLE_VALUE) return;
//
//	while (true) {
//		ConnectNamedPipe(hPipe, nullptr);
//
//		if (ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, nullptr)) {
//			uint8_t encrypted[256];  // 存储加密后的数据
//			size_t encryptedLen = bytesRead;
//			memcpy(encrypted, buffer, encryptedLen);
//
//			uint8_t decrypted[256];  // 解密后的数据
//			size_t decryptedLen = 0;
//
//			if (sm4_cbc_padding_decrypt(&decrypt_key, iv, encrypted, encryptedLen, decrypted, &decryptedLen) != 1) {
//				CloseHandle(hPipe);
//				return;
//			}
//			// 将解密后的密码转换为 wchar_t 字符串
//			wchar_t decryptedPassword[256];
//			memcpy(decryptedPassword, decrypted, decryptedLen);
//			decryptedPassword[decryptedLen / sizeof(wchar_t)] = L'\0';  // 确保以 '\0' 结尾
//
//
//			//buffer[bytesRead / sizeof(wchar_t)] = L'\0';
//
//			// 1. 更新密码字段
//			//pCredential->SetStringValue(SFI_PASSWORD, buffer);
//			pCredential->SetStringValue(SFI_PASSWORD, decryptedPassword);
//
//			// 2. 通知 LogonUI 密码字段已更新
//			if (pCredential->_pCredProvCredentialEvents) {
//
//				pCredential->_pCredProvCredentialEvents->SetFieldInteractiveState(
//					pCredential,
//					SFI_PASSWORD,
//					CPFIS_FOCUSED
//				);
//				pCredential->_pCredProvCredentialEvents->BeginFieldUpdates();
//				pCredential->_pCredProvCredentialEvents->SetFieldString(
//					pCredential,
//					SFI_PASSWORD,
//					pCredential->_rgFieldStrings[SFI_PASSWORD]
//				);
//				pCredential->_pCredProvCredentialEvents->EndFieldUpdates();
//			}
//
//			// 3. 通过 _pProvider 触发 CredentialsChanged 事件
//			if (pCredential->_pProvider && pCredential->_pProvider->_pCredProviderEvents)
//			{
//				// 进入临界区（确保线程安全）
//				EnterCriticalSection(&pCredential->_pProvider->_csEvents);
//				pCredential->_pProvider->_pCredProviderEvents->CredentialsChanged(pCredential->_pProvider->_upAdviseContext);
//				LeaveCriticalSection(&pCredential->_pProvider->_csEvents);
//			}
//
//			// 4. 触发自动提交
//			/*BOOL bAutoLogon;
//			pCredential->SetSelected(&bAutoLogon);*/
//		}
//		DisconnectNamedPipe(hPipe);
//	}
//	CloseHandle(hPipe);
//}

std::wstring myCredential::GetUnlockScreenPathFromRegistry()
{
	HKEY hKey;
	wchar_t path[MAX_PATH];
	DWORD pathLen = sizeof(path);
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\LnsoftFaceLogin", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		if (RegQueryValueEx(hKey, L"UnlockScreenPath", nullptr, nullptr, (LPBYTE)path, &pathLen) == ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return path;
		}
		RegCloseKey(hKey);
	}
	return L"";
}


IFACEMETHODIMP myCredential::Advise(ICredentialProviderCredentialEvents * pcpce)
{
	Logger::Log(L"myCredential::Advise");

	if (_pCredProvCredentialEvents != nullptr)
	{
		_pCredProvCredentialEvents->Release();
	}
	return pcpce->QueryInterface(IID_PPV_ARGS(&_pCredProvCredentialEvents));
}

IFACEMETHODIMP myCredential::UnAdvise()
{
	Logger::Log(L"myCredential::UnAdvise");
	if (_pCredProvCredentialEvents)
	{
		_pCredProvCredentialEvents->Release();
	}
	_pCredProvCredentialEvents = nullptr;
	return S_OK;
}


//// 检查指定进程是否正在运行
//bool IsProcessRunning(const wchar_t* processName)
//{
//	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	if (hSnapshot == INVALID_HANDLE_VALUE)
//	{
//		return false;
//	}
//
//	PROCESSENTRY32W pe32;
//	pe32.dwSize = sizeof(PROCESSENTRY32W);
//
//	if (!Process32FirstW(hSnapshot, &pe32)) 
//	{
//		CloseHandle(hSnapshot);
//		return false;
//	}
//
//	bool found = false;
//	do
//	{
//		if (_wcsicmp(pe32.szExeFile, processName) == 0)
//		{
//			found = true;
//			break;
//		}
//	} while (Process32NextW(hSnapshot, &pe32));
//
//	CloseHandle(hSnapshot);
//	return found;
//}
//
//// 终止指定名称的进程
//void TerminateProcessByName(const wchar_t* processName)
//{
//	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	if (hSnapshot == INVALID_HANDLE_VALUE)
//	{
//		return;
//	}
//
//	PROCESSENTRY32W pe32;
//	pe32.dwSize = sizeof(PROCESSENTRY32W);
//
//	if (!Process32FirstW(hSnapshot, &pe32))
//	{
//		CloseHandle(hSnapshot);
//		return;
//	}
//
//	do 
//	{
//		if (_wcsicmp(pe32.szExeFile, processName) == 0)
//		{
//			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
//			if (hProcess != nullptr)
//			{
//				TerminateProcess(hProcess, 0);
//				CloseHandle(hProcess);
//			}
//		}
//	} while (Process32NextW(hSnapshot, &pe32));
//
//	CloseHandle(hSnapshot);
//}

//IFACEMETHODIMP myCredential::SetSelected(BOOL * pbAutoLogon)
//{
//	static int s_callCount = 0;
//	s_callCount++;
//
//	Logger::Log(L"SetSelected called. Count: " + std::to_wstring(s_callCount) +
//		L", Scenario: " + std::to_wstring(_cpus));
//
//	// 防止5秒内重复启动
//	DWORD currentTime = GetTickCount();
//	if (_bProcessLaunched)
//	{
//		DWORD elapsed = currentTime - _dwLaunchTime;
//		if (elapsed < 5000)
//		{
//			Logger::Log(L"Process already launched " + std::to_wstring(elapsed) +
//				L"ms ago, skipping...");
//			*pbAutoLogon = FALSE;
//			return S_OK;
//		}
//		else
//		{
//			Logger::Log(L"Previous launch expired, relaunching...");
//			_bProcessLaunched = false;
//		}
//	}
//
//	// 检查并终止已有进程
//	const wchar_t* targetProcess = L"unlockScreen.exe";
//	if (IsProcessRunning(targetProcess))
//	{
//		Logger::Log(L"Terminating existing process...");
//		TerminateProcessByName(targetProcess);
//		Sleep(500);
//	}
//
//	// 根据场景选择启动方式
//	if (_cpus == CPUS_UNLOCK_WORKSTATION)
//	{
//		// 锁屏场景：使用当前会话（用户已登录）
//		Logger::Log(L"UNLOCK_WORKSTATION scenario - Launching in current session");
//
//		HWND hwndOwner = nullptr;
//		if (_pCredProvCredentialEvents)
//		{
//			_pCredProvCredentialEvents->OnCreatingWindow(&hwndOwner);
//
//			HINSTANCE hResult = ShellExecuteW(
//				hwndOwner,
//				L"open",
//				unlockScreenPath.c_str(),
//				nullptr,
//				nullptr,
//				SW_SHOWNORMAL
//			);
//
//			if ((INT_PTR)hResult > 32)
//			{
//				Logger::Log(L"ShellExecuteW succeeded");
//			}
//			else
//			{
//				Logger::Log(L"ShellExecuteW failed. Error: " + std::to_wstring((INT_PTR)hResult));
//			}
//		}
//	}
//	else if (_cpus == CPUS_LOGON)
//	{
//		// 登录场景：使用活动会话（用户未登录）
//		Logger::Log(L"LOGON scenario - Launching in active session");
//		LaunchInActiveSession(unlockScreenPath);
//	}
//	else
//	{
//		Logger::Log(L"Unknown scenario: " + std::to_wstring(_cpus));
//	}
//
//	// 标记已启动
//	_bProcessLaunched = true;
//	_dwLaunchTime = currentTime;
//
//	// 检查密码
//	if (_rgFieldStrings[SFI_PASSWORD] && wcslen(_rgFieldStrings[SFI_PASSWORD]) > 0)
//	{
//		Logger::Log(L"Password detected, enabling auto-logon");
//		*pbAutoLogon = TRUE;
//	}
//	else
//	{
//		Logger::Log(L"No password, disabling auto-logon");
//		*pbAutoLogon = FALSE;
//	}
//
//	return S_OK;
//}
//
//
//// 在活动会话中启动进程（用于开机登录场景）
//void myCredential::LaunchInActiveSession(const std::wstring& exePath)
//{
//	Logger::Log(L"LaunchInActiveSession: " + exePath);
//
//	// ===== 新增：启用 SE_TCB_NAME 权限 =====
//	HANDLE hToken = nullptr;
//	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
//	{
//		Logger::Log(L"OpenProcessToken failed. Error: " + std::to_wstring(GetLastError()));
//		return;
//	}
//
//	TOKEN_PRIVILEGES tp;
//	LUID luid;
//
//	if (!LookupPrivilegeValue(nullptr, SE_TCB_NAME, &luid))
//	{
//		Logger::Log(L"LookupPrivilegeValue failed. Error: " + std::to_wstring(GetLastError()));
//		CloseHandle(hToken);
//		return;
//	}
//
//	tp.PrivilegeCount = 1;
//	tp.Privileges[0].Luid = luid;
//	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
//
//	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr))
//	{
//		Logger::Log(L"AdjustTokenPrivileges failed. Error: " + std::to_wstring(GetLastError()));
//		CloseHandle(hToken);
//		return;
//	}
//
//	DWORD lastError = GetLastError();
//	if (lastError == ERROR_NOT_ALL_ASSIGNED)
//	{
//		Logger::Log(L"WARNING: SE_TCB_NAME privilege not assigned to process");
//		CloseHandle(hToken);
//		return;
//	}
//
//	CloseHandle(hToken);
//	Logger::Log(L"SE_TCB_NAME privilege enabled successfully");
//	// ===== 权限提升结束 =====
//
//	// 获取活动控制台会话ID
//	DWORD sessionId = WTSGetActiveConsoleSessionId();
//	if (sessionId == 0xFFFFFFFF)
//	{
//		Logger::Log(L"ERROR: No active console session");
//		return;
//	}
//
//	Logger::Log(L"Active session ID: " + std::to_wstring(sessionId));
//
//	// 获取该会话的用户令牌
//	HANDLE hUserToken = nullptr;
//	if (!WTSQueryUserToken(sessionId, &hUserToken))
//	{
//		DWORD error = GetLastError();
//		Logger::Log(L"WTSQueryUserToken failed. Error: " + std::to_wstring(error));
//		return;
//	}
//
//	Logger::Log(L"User token obtained successfully");
//
//	// 创建进程信息结构
//	STARTUPINFOW si = { 0 };
//	si.cb = sizeof(si);
//	si.lpDesktop = (LPWSTR)L"winsta0\\default";
//	si.dwFlags = STARTF_USESHOWWINDOW;
//	si.wShowWindow = SW_SHOWNORMAL;
//
//	PROCESS_INFORMATION pi = { 0 };
//
//	// 构造命令行
//	wchar_t cmdLine[MAX_PATH];
//	wcscpy_s(cmdLine, exePath.c_str());
//
//	// 在用户会话中创建进程
//	BOOL result = CreateProcessAsUserW(
//		hUserToken,
//		nullptr,
//		cmdLine,
//		nullptr,
//		nullptr,
//		FALSE,
//		CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS,
//		nullptr,
//		nullptr,
//		&si,
//		&pi
//	);
//
//	if (result)
//	{
//		Logger::Log(L"Process created successfully in session " + std::to_wstring(sessionId));
//		CloseHandle(pi.hProcess);
//		CloseHandle(pi.hThread);
//	}
//	else
//	{
//		DWORD error = GetLastError();
//		Logger::Log(L"CreateProcessAsUserW failed. Error: " + std::to_wstring(error));
//	}
//
//	CloseHandle(hUserToken);
//}

IFACEMETHODIMP myCredential::SetSelected(BOOL * pbAutoLogon)
{
	static int s_callCount = 0; // 静态变量记录调用次数
	s_callCount++;
	// 记录日志
	std::wstring logMessage = L"SetSelected called. Count: " + std::to_wstring(s_callCount);
	Logger::Log(logMessage);

	OutputDebugStringW(L"[INFO] SetSelected(BOOL * pbAutoLogon)");

	//// 检查并终止已有进程
	//const wchar_t* targetProcess = L"unlockScreen.exe";
	//if (IsProcessRunning(targetProcess))
	//{
	//	TerminateProcessByName(targetProcess);
	//	// 稍等片刻确保进程退出
	//	Sleep(500);
	//}

	//// 启动新进程
	//HWND hwndOwner = nullptr;
	//if (_pCredProvCredentialEvents) {
	//	_pCredProvCredentialEvents->OnCreatingWindow(&hwndOwner);
	//	//ShellExecuteW(hwndOwner, L"open", L"C:\\test\\lnsoft-otfr\\unlockScreen.exe",nullptr, nullptr, SW_SHOWNORMAL);
	//	ShellExecuteW(hwndOwner, L"open", unlockScreenPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
	//}

	//HWND hwndOwner = nullptr;
	//if (_pCredProvCredentialEvents) {
	//	static std::once_flag s_flag;
	//	_pCredProvCredentialEvents->OnCreatingWindow(&hwndOwner);
	//	std::call_once(s_flag, [hwndOwner]()
	//	{
	//		ShellExecuteW(hwndOwner, L"open", L"C:\\test\\lnsoft-otfr\\unlockScreen.exe", nullptr, nullptr, SW_SHOWNORMAL);
	//	});
	//	//ShellExecuteW(hwndOwner, L"open", L"C:\\test\\lnsoft-otfr\\unlockScreen.exe", nullptr, nullptr, SW_SHOWNORMAL);
	//}



	// 检查密码是否已填充
	if (_rgFieldStrings[SFI_PASSWORD] && wcslen(_rgFieldStrings[SFI_PASSWORD]) > 0) {
		*pbAutoLogon = TRUE; // 触发自动提交
	}
	else {
		*pbAutoLogon = FALSE;
	}
	return S_OK;
}

IFACEMETHODIMP myCredential::SetDeselected()
{
	Logger::Log(L"myCredential::SetDeselected");
	HRESULT hr = S_OK;
	if (_rgFieldStrings[SFI_PASSWORD])
	{
		size_t lenPassword = wcslen(_rgFieldStrings[SFI_PASSWORD]);
		SecureZeroMemory(_rgFieldStrings[SFI_PASSWORD], lenPassword * sizeof(*_rgFieldStrings[SFI_PASSWORD]));

		CoTaskMemFree(_rgFieldStrings[SFI_PASSWORD]);
		hr = SHStrDupW(L"", &_rgFieldStrings[SFI_PASSWORD]);

		if (SUCCEEDED(hr) && _pCredProvCredentialEvents)
		{
			_pCredProvCredentialEvents->SetFieldString(this, SFI_PASSWORD, _rgFieldStrings[SFI_PASSWORD]);
		}
	}

	return hr;
}

IFACEMETHODIMP myCredential::GetFieldState(DWORD dwFieldID,
	CREDENTIAL_PROVIDER_FIELD_STATE * pcpfs,
	CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE * pcpfis)
{
	Logger::Log(L"myCredential::GetFieldState");
	HRESULT hr;

	// Validate our parameters.
	if ((dwFieldID < ARRAYSIZE(_rgFieldStatePairs)))
	{
		*pcpfs = _rgFieldStatePairs[dwFieldID].cpfs;
		*pcpfis = _rgFieldStatePairs[dwFieldID].cpfis;
		hr = S_OK;
	}
	else
	{
		hr = E_INVALIDARG;
	}
	return hr;
}

IFACEMETHODIMP myCredential::GetStringValue(DWORD dwFieldID, PWSTR * ppwsz)
{
	Logger::Log(L"myCredential::GetStringValue");
	HRESULT hr;
	*ppwsz = nullptr;

	// Check to make sure dwFieldID is a legitimate index
	if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors))
	{
		// Make a copy of the string and return that. The caller
		// is responsible for freeing it.
		hr = SHStrDupW(_rgFieldStrings[dwFieldID], ppwsz);
	}
	else
	{
		hr = E_INVALIDARG;
	}

	return hr;
}

IFACEMETHODIMP myCredential::GetBitmapValue(DWORD dwFieldID, HBITMAP * phbmp)
{
	Logger::Log(L"myCredential::GetBitmapValue");
	HRESULT hr;
	*phbmp = nullptr;

	if ((SFI_TILEIMAGE == dwFieldID))
	{
		HBITMAP hbmp = LoadBitmap(HINST_THISDLL, MAKEINTRESOURCE(IDB_TILE_IMAGE));
		if (hbmp != nullptr)
		{
			hr = S_OK;
			*phbmp = hbmp;
		}
		else
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}
	hr = S_OK;
	return hr;
}

IFACEMETHODIMP myCredential::GetCheckboxValue(DWORD dwFieldID, BOOL * pbChecked, PWSTR * ppwszLabel)
{
	Logger::Log(L"myCredential::GetCheckboxValue");
	HRESULT hr;
	*ppwszLabel = nullptr;

	// Validate parameters.
	if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) &&
		(CPFT_CHECKBOX == _rgCredProvFieldDescriptors[dwFieldID].cpft))
	{
		*pbChecked = _fChecked;
		hr = SHStrDupW(_rgFieldStrings[SFI_CHECKBOX], ppwszLabel);
	}
	else
	{
		hr = E_INVALIDARG;
	}

	return hr;
}

IFACEMETHODIMP myCredential::GetComboBoxValueCount(DWORD dwFieldID, DWORD * pcItems, DWORD * pdwSelectedItem)
{
	Logger::Log(L"myCredential::GetComboBoxValueCount");
	HRESULT hr;
	*pcItems = 0;
	*pdwSelectedItem = 0;

	// Validate parameters.
	if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) &&
		(CPFT_COMBOBOX == _rgCredProvFieldDescriptors[dwFieldID].cpft))
	{
		*pcItems = ARRAYSIZE(s_rgComboBoxStrings);
		*pdwSelectedItem = 0;
		hr = S_OK;
	}
	else
	{
		hr = E_INVALIDARG;
	}

	return hr;
}

IFACEMETHODIMP myCredential::GetComboBoxValueAt(DWORD dwFieldID, DWORD dwItem, PWSTR * ppwszItem)
{
	Logger::Log(L"myCredential::GetComboBoxValueAt");
	HRESULT hr;
	*ppwszItem = nullptr;

	// Validate parameters.
	if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) &&
		(CPFT_COMBOBOX == _rgCredProvFieldDescriptors[dwFieldID].cpft))
	{
		hr = SHStrDupW(s_rgComboBoxStrings[dwItem], ppwszItem);
	}
	else
	{
		hr = E_INVALIDARG;
	}

	return hr;
}

IFACEMETHODIMP myCredential::GetSubmitButtonValue(DWORD dwFieldID, DWORD * pdwAdjacentTo)
{
	Logger::Log(L"myCredential::GetSubmitButtonValue");
	HRESULT hr;

	if (SFI_SUBMIT_BUTTON == dwFieldID)
	{
		// pdwAdjacentTo is a pointer to the fieldID you want the submit button to
		// appear next to.
		*pdwAdjacentTo = SFI_PASSWORD;
		hr = S_OK;
	}
	else
	{
		hr = E_INVALIDARG;
	}
	return hr;
}

IFACEMETHODIMP myCredential::SetStringValue(DWORD dwFieldID, PCWSTR pwz)
{
	Logger::Log(L"myCredential::SetStringValue");
	HRESULT hr;

	// Validate parameters.
	if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) &&
		(CPFT_EDIT_TEXT == _rgCredProvFieldDescriptors[dwFieldID].cpft ||
			CPFT_PASSWORD_TEXT == _rgCredProvFieldDescriptors[dwFieldID].cpft))
	{
		PWSTR *ppwszStored = &_rgFieldStrings[dwFieldID];
		CoTaskMemFree(*ppwszStored);
		hr = SHStrDupW(pwz, ppwszStored);
	}
	else
	{
		hr = E_INVALIDARG;
	}

	return hr;
}

IFACEMETHODIMP myCredential::SetCheckboxValue(DWORD dwFieldID, BOOL bChecked)
{
	Logger::Log(L"myCredential::SetCheckboxValue");
	HRESULT hr;

	// Validate parameters.
	if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) &&
		(CPFT_CHECKBOX == _rgCredProvFieldDescriptors[dwFieldID].cpft))
	{
		_fChecked = bChecked;
		hr = S_OK;
	}
	else
	{
		hr = E_INVALIDARG;
	}

	return hr;
}

IFACEMETHODIMP myCredential::SetComboBoxSelectedValue(DWORD dwFieldID, DWORD dwSelectedItem)
{
	Logger::Log(L"myCredential::SetComboBoxSelectedValue");
	HRESULT hr;

	// Validate parameters.
	if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) &&
		(CPFT_COMBOBOX == _rgCredProvFieldDescriptors[dwFieldID].cpft))
	{
		_dwComboIndex = dwSelectedItem;
		hr = S_OK;
	}
	else
	{
		hr = E_INVALIDARG;
	}

	return hr;
}

IFACEMETHODIMP myCredential::CommandLinkClicked(DWORD dwFieldID)
{
	Logger::Log(L"myCredential::CommandLinkClicked");
	HRESULT hr = S_OK;

	CREDENTIAL_PROVIDER_FIELD_STATE cpfsShow = CPFS_HIDDEN;

	// Validate parameter.
	if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) &&
		(CPFT_COMMAND_LINK == _rgCredProvFieldDescriptors[dwFieldID].cpft))
	{
		HWND hwndOwner = nullptr;
		switch (dwFieldID)
		{
		case SFI_LAUNCHWINDOW_LINK:
			//if (_pCredProvCredentialEvents)
			//{
			//    _pCredProvCredentialEvents->OnCreatingWindow(&hwndOwner);
			//}

			//// Pop a messagebox indicating the click.
			//::MessageBox(hwndOwner, L"Command link clicked", L"Click!", 0);
			//break;
			if (_pCredProvCredentialEvents) {
				_pCredProvCredentialEvents->OnCreatingWindow(&hwndOwner);
			}
			// 启动Qt程序（替换为实际路径）
			//ShellExecuteW(hwndOwner, L"open", L"C:\\test\\release\\myUnlockDemo.exe", nullptr, nullptr, SW_SHOWNORMAL);
			ShellExecuteW(hwndOwner, L"open", unlockScreenPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
			break;

		case SFI_HIDECONTROLS_LINK:
			_pCredProvCredentialEvents->BeginFieldUpdates();
			cpfsShow = _fShowControls ? CPFS_DISPLAY_IN_SELECTED_TILE : CPFS_HIDDEN;
			_pCredProvCredentialEvents->SetFieldState(nullptr, SFI_FULLNAME_TEXT, cpfsShow);
			_pCredProvCredentialEvents->SetFieldState(nullptr, SFI_DISPLAYNAME_TEXT, cpfsShow);
			_pCredProvCredentialEvents->SetFieldState(nullptr, SFI_LOGONSTATUS_TEXT, cpfsShow);
			_pCredProvCredentialEvents->SetFieldState(nullptr, SFI_CHECKBOX, cpfsShow);
			_pCredProvCredentialEvents->SetFieldState(nullptr, SFI_EDIT_TEXT, cpfsShow);
			_pCredProvCredentialEvents->SetFieldState(nullptr, SFI_COMBOBOX, cpfsShow);
			_pCredProvCredentialEvents->SetFieldString(nullptr, SFI_HIDECONTROLS_LINK, _fShowControls ? L"Hide additional controls" : L"Show additional controls");
			_pCredProvCredentialEvents->EndFieldUpdates();
			_fShowControls = !_fShowControls;
			break;
		default:
			hr = E_INVALIDARG;
		}

	}
	else
	{
		hr = E_INVALIDARG;
	}

	return hr;
}

IFACEMETHODIMP myCredential::GetSerialization(CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE * pcpgsr, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION * pcpcs, PWSTR * ppwszOptionalStatusText, CREDENTIAL_PROVIDER_STATUS_ICON * pcpsiOptionalStatusIcon)
{
	Logger::Log(L"myCredential::GetSerialization");
	*pcpgsr = CPGSR_NO_CREDENTIAL_NOT_FINISHED;
	*ppwszOptionalStatusText = nullptr;
	*pcpsiOptionalStatusIcon = CPSI_NONE;
	ZeroMemory(pcpcs, sizeof(*pcpcs));

	// 检查密码是否已接收
	if (_rgFieldStrings[SFI_PASSWORD] == nullptr ||
		wcslen(_rgFieldStrings[SFI_PASSWORD]) == 0) {
		*pcpgsr = CPGSR_NO_CREDENTIAL_NOT_FINISHED;
		return S_OK;
	}

	// 生成身份验证数据
	HRESULT hr = E_FAIL;
	if (_fIsLocalUser) {
		PWSTR pwzProtectedPassword = nullptr;
		hr = ProtectIfNecessaryAndCopyPassword(_rgFieldStrings[SFI_PASSWORD], _cpus, &pwzProtectedPassword);
		if (SUCCEEDED(hr)) {
			PWSTR pszDomain, pszUsername;
			hr = SplitDomainAndUsername(_pszQualifiedUserName, &pszDomain, &pszUsername);
			if (SUCCEEDED(hr)) {
				KERB_INTERACTIVE_UNLOCK_LOGON kiul;
				hr = KerbInteractiveUnlockLogonInit(pszDomain, pszUsername, pwzProtectedPassword, _cpus, &kiul);
				if (SUCCEEDED(hr)) {
					hr = KerbInteractiveUnlockLogonPack(kiul, &pcpcs->rgbSerialization, &pcpcs->cbSerialization);
					if (SUCCEEDED(hr)) {
						ULONG ulAuthPackage;
						hr = RetrieveNegotiateAuthPackage(&ulAuthPackage);
						if (SUCCEEDED(hr)) {
							pcpcs->ulAuthenticationPackage = ulAuthPackage;
							pcpcs->clsidCredentialProvider = CLSID_CSample;
							*pcpgsr = CPGSR_RETURN_CREDENTIAL_FINISHED; // 标记为完成
						}
					}
				}
				CoTaskMemFree(pszDomain);
				CoTaskMemFree(pszUsername);
			}
			CoTaskMemFree(pwzProtectedPassword);
		}
	}
	else {
		// 远程用户处理（略）
		hr = E_NOTIMPL;
	}

	return hr;
}

struct REPORT_RESULT_STATUS_INFO
{
	NTSTATUS ntsStatus;
	NTSTATUS ntsSubstatus;
	PWSTR     pwzMessage;
	CREDENTIAL_PROVIDER_STATUS_ICON cpsi;
};

static const REPORT_RESULT_STATUS_INFO s_rgLogonStatusInfo[] =
{
	{ STATUS_LOGON_FAILURE, STATUS_SUCCESS, L"用户名或密码错误", CPSI_ERROR, },
	{ STATUS_ACCOUNT_RESTRICTION, STATUS_ACCOUNT_DISABLED, L"The account is disabled.", CPSI_WARNING },
};

IFACEMETHODIMP myCredential::ReportResult(NTSTATUS ntsStatus, NTSTATUS ntsSubstatus, PWSTR * ppwszOptionalStatusText, CREDENTIAL_PROVIDER_STATUS_ICON * pcpsiOptionalStatusIcon)
{
	Logger::Log(L"myCredential::ReportResult");
	*ppwszOptionalStatusText = nullptr;
	*pcpsiOptionalStatusIcon = CPSI_NONE;

	DWORD dwStatusInfo = (DWORD)-1;

	// Look for a match on status and substatus.
	for (DWORD i = 0; i < ARRAYSIZE(s_rgLogonStatusInfo); i++)
	{
		if (s_rgLogonStatusInfo[i].ntsStatus == ntsStatus && s_rgLogonStatusInfo[i].ntsSubstatus == ntsSubstatus)
		{
			dwStatusInfo = i;
			break;
		}
	}

	if ((DWORD)-1 != dwStatusInfo)
	{
		if (SUCCEEDED(SHStrDupW(s_rgLogonStatusInfo[dwStatusInfo].pwzMessage, ppwszOptionalStatusText)))
		{
			*pcpsiOptionalStatusIcon = s_rgLogonStatusInfo[dwStatusInfo].cpsi;
		}
	}

	// If we failed the logon, try to erase the password field.
	if (FAILED(HRESULT_FROM_NT(ntsStatus)))
	{
		if (_pCredProvCredentialEvents)
		{
			_pCredProvCredentialEvents->SetFieldString(this, SFI_PASSWORD, L"");
		}
	}

	// Since nullptr is a valid value for *ppwszOptionalStatusText and *pcpsiOptionalStatusIcon
	// this function can't fail.
	return S_OK;
}

IFACEMETHODIMP myCredential::GetUserSid(PWSTR * ppszSid)
{
	Logger::Log(L"myCredential::GetUserSid");
	*ppszSid = nullptr;
	HRESULT hr = E_UNEXPECTED;
	if (_pszUserSid != nullptr)
	{
		hr = SHStrDupW(_pszUserSid, ppszSid);
	}
	// Return S_FALSE with a null SID in ppszSid for the
	// credential to be associated with an empty user tile.

	return hr;
}

IFACEMETHODIMP myCredential::GetFieldOptions(DWORD dwFieldID, CREDENTIAL_PROVIDER_CREDENTIAL_FIELD_OPTIONS * pcpcfo)
{
	Logger::Log(L"myCredential::GetFieldOptions");
	*pcpcfo = CPCFO_NONE;

	if (dwFieldID == SFI_PASSWORD)
	{
		*pcpcfo = CPCFO_ENABLE_PASSWORD_REVEAL;
	}
	else if (dwFieldID == SFI_TILEIMAGE)
	{
		*pcpcfo = CPCFO_ENABLE_TOUCH_KEYBOARD_AUTO_INVOKE;
	}

	return S_OK;
}

