//#include "PipeManager.h"
//#include "myProvider.h"
//#include "myCredential.h"
//#include "logger.h"
//#include "common.h"
//#include "sm4/sm4.h"
//
//PipeManager* PipeManager::s_instance = nullptr;
//std::mutex PipeManager::s_instanceMutex;
//
//PipeManager& PipeManager::GetInstance()
//{
//	std::lock_guard<std::mutex> lock(s_instanceMutex);
//	if (s_instance == nullptr)
//	{
//		s_instance = new PipeManager();
//	}
//	return *s_instance;
//}
//
//PipeManager::PipeManager()
//	: _pProvider(nullptr)
//	, _bRunning(false)
//	, _bStopRequested(false)
//{
//	Logger::Log(L"PipeManager created");
//}
//
//PipeManager::~PipeManager()
//{
//	Stop();
//	Logger::Log(L"PipeManager destroyed");
//}
//
//void PipeManager::Start(myProvider* pProvider)
//{
//	std::lock_guard<std::mutex> lock(_mutex);
//
//	if (_bRunning)
//	{
//		Logger::Log(L"PipeManager already running, updating provider reference");
//		_pProvider = pProvider;
//		return;
//	}
//
//	_pProvider = pProvider;
//	_bStopRequested = false;
//	_pipeThread = std::thread(ListenForPassword, this);
//	_pipeThread.detach();
//	_bRunning = true;
//
//	Logger::Log(L"PipeManager started");
//}
//
//void PipeManager::Stop()
//{
//	std::lock_guard<std::mutex> lock(_mutex);
//
//	if (!_bRunning)
//		return;
//
//	_bStopRequested = true;
//	_bRunning = false;
//	_pProvider = nullptr;
//
//	Logger::Log(L"PipeManager stopped");
//}
//
//void PipeManager::ListenForPassword(PipeManager* pManager)
//{
//	Logger::Log(L"PipeManager ListenForPassword thread started");
//
//	const uint8_t user_key[16] = { 0x35, 0x13, 0x41, 0x69, 0x09, 0xA1, 0xAD, 0x8E,
//								  0x27, 0x85, 0x57, 0x76, 0x98, 0xBA, 0xDC, 0xFE };
//	const uint8_t iv[16] = { 0xAD, 0x01, 0xC2, 0xB3, 0x64, 0x21, 0xCC, 0x77,
//							0xFF, 0x12, 0x0A, 0x0D, 0xEC, 0x7B, 0x0E, 0x0F };
//	SM4_KEY decrypt_key;
//	sm4_set_decrypt_key(&decrypt_key, user_key);
//
//	HANDLE hPipe = CreateNamedPipeW(
//		L"\\\\.\\pipe\\MyCredentialPipe",
//		PIPE_ACCESS_INBOUND,
//		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
//		1,
//		256 * sizeof(wchar_t),
//		256 * sizeof(wchar_t),
//		1000,
//		nullptr
//	);
//
//	if (hPipe == INVALID_HANDLE_VALUE)
//	{
//		DWORD error = GetLastError();
//		Logger::Log(L"ERROR: Failed to create named pipe. Error: " + std::to_wstring(error));
//		return;
//	}
//
//	Logger::Log(L"Named pipe created successfully");
//
//	while (!pManager->_bStopRequested)
//	{
//		BOOL connected = ConnectNamedPipe(hPipe, nullptr);
//		if (!connected && GetLastError() != ERROR_PIPE_CONNECTED)
//		{
//			Sleep(100);
//			continue;
//		}
//
//		wchar_t buffer[256];
//		DWORD bytesRead;
//
//		if (ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, nullptr))
//		{
//			uint8_t encrypted[256];
//			size_t encryptedLen = bytesRead;
//			memcpy(encrypted, buffer, encryptedLen);
//
//			uint8_t decrypted[256];
//			size_t decryptedLen = 0;
//
//			if (sm4_cbc_padding_decrypt(&decrypt_key, iv, encrypted,
//				encryptedLen, decrypted, &decryptedLen) != 1)
//			{
//				Logger::Log(L"ERROR: Decryption failed");
//				DisconnectNamedPipe(hPipe);
//				continue;
//			}
//
//			wchar_t decryptedPassword[256];
//			memcpy(decryptedPassword, decrypted, decryptedLen);
//			decryptedPassword[decryptedLen / sizeof(wchar_t)] = L'\0';
//
//			Logger::Log(L"Password received via pipe");
//			Logger::Log(L"Password length: " + std::to_wstring(wcslen(decryptedPassword)));
//
//			// ===== 简化且安全的更新方式 =====
//			{
//				std::lock_guard<std::mutex> lock(pManager->_mutex);
//
//				Logger::Log(L"Lock acquired, checking provider state...");
//
//				if (!pManager->_pProvider)
//				{
//					Logger::Log(L"ERROR: Provider reference is NULL!");
//					DisconnectNamedPipe(hPipe);
//					continue;
//				}
//
//				myProvider* provider = pManager->_pProvider;
//
//				if (!provider->_pCredential)
//				{
//					Logger::Log(L"ERROR: Credential not created yet!");
//					DisconnectNamedPipe(hPipe);
//					continue;
//				}
//
//				myCredential* credential = provider->_pCredential;
//				Logger::Log(L"Provider and Credential are valid");
//
//				try
//				{
//					// ===== 方法1：使用 SetStringValue（线程安全）=====
//					Logger::Log(L"Calling SetStringValue...");
//					HRESULT hrSet = credential->SetStringValue(SFI_PASSWORD, decryptedPassword);
//					Logger::Log(L"SetStringValue result: " + std::to_wstring(hrSet));
//
//					if (FAILED(hrSet))
//					{
//						Logger::Log(L"ERROR: SetStringValue failed!");
//						DisconnectNamedPipe(hPipe);
//						continue;
//					}
//
//					Logger::Log(L"Password field updated successfully");
//
//					// ===== 仅在有事件接口时更新 UI =====
//					if (credential->_pCredProvCredentialEvents)
//					{
//						Logger::Log(L"Updating UI...");
//
//						credential->_pCredProvCredentialEvents->BeginFieldUpdates();
//
//						credential->_pCredProvCredentialEvents->SetFieldString(
//							credential,
//							SFI_PASSWORD,
//							L"●●●●●●●●" // 显示掩码而不是实际密码
//						);
//
//						credential->_pCredProvCredentialEvents->EndFieldUpdates();
//
//						Logger::Log(L"UI updated");
//					}
//					else
//					{
//						Logger::Log(L"WARNING: No credential events interface");
//					}
//
//					// ===== 触发凭据变更 =====
//					if (provider->_pCredProviderEvents)
//					{
//						Logger::Log(L"Triggering CredentialsChanged...");
//
//						EnterCriticalSection(&provider->_csEvents);
//						HRESULT hrChange = provider->_pCredProviderEvents->CredentialsChanged(
//							provider->_upAdviseContext);
//						LeaveCriticalSection(&provider->_csEvents);
//
//						Logger::Log(L"CredentialsChanged result: " + std::to_wstring(hrChange));
//					}
//					else
//					{
//						Logger::Log(L"WARNING: No provider events interface");
//					}
//
//					Logger::Log(L"Password update completed successfully");
//				}
//				catch (const std::exception& e)
//				{
//					Logger::Log(L"EXCEPTION: std::exception caught");
//				}
//				catch (...)
//				{
//					Logger::Log(L"EXCEPTION: Unknown exception caught");
//				}
//			}
//
//			Logger::Log(L"Lock released");
//		}
//
//		DisconnectNamedPipe(hPipe);
//	}
//
//	CloseHandle(hPipe);
//	Logger::Log(L"PipeManager ListenForPassword thread stopped");
//}
