#pragma once

#include "helpers.h"
#include <windows.h>
#include <strsafe.h>
#include <new>

#include "myCredential.h"
#include "logger.h"

class myProvider : public ICredentialProvider,
	public ICredentialProviderSetUserArray
{
public:
	// IUnknown
	IFACEMETHODIMP_(ULONG) AddRef()
	{
		return ++_cRef;
	}

	IFACEMETHODIMP_(ULONG) Release()
	{
		long cRef = --_cRef;
		Logger::Log(L"Release   cRef: " + std::to_wstring(cRef));
		if (!cRef)
		{
			Logger::Log(L"Release   delete this;");
			delete this;
		}
		return cRef;
	}

	IFACEMETHODIMP QueryInterface(_In_ REFIID riid, _COM_Outptr_ void **ppv)
	{
		static const QITAB qit[] =
		{
			QITABENT(myProvider, ICredentialProvider), // IID_ICredentialProvider
			QITABENT(myProvider, ICredentialProviderSetUserArray), // IID_ICredentialProviderSetUserArray
			{0},
		};
		return QISearch(this, qit, riid, ppv);
	}

public:
	IFACEMETHODIMP SetUsageScenario(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD dwFlags);
	IFACEMETHODIMP SetSerialization(_In_ CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION const *pcpcs);

	IFACEMETHODIMP Advise(_In_ ICredentialProviderEvents *pcpe, _In_ UINT_PTR upAdviseContext);
	IFACEMETHODIMP UnAdvise();

	IFACEMETHODIMP GetFieldDescriptorCount(_Out_ DWORD *pdwCount);
	IFACEMETHODIMP GetFieldDescriptorAt(DWORD dwIndex, _Outptr_result_nullonfailure_ CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR **ppcpfd);

	IFACEMETHODIMP GetCredentialCount(_Out_ DWORD *pdwCount,
		_Out_ DWORD *pdwDefault,
		_Out_ BOOL *pbAutoLogonWithDefault);
	IFACEMETHODIMP GetCredentialAt(DWORD dwIndex,
		_Outptr_result_nullonfailure_ ICredentialProviderCredential **ppcpc);

	IFACEMETHODIMP SetUserArray(_In_ ICredentialProviderUserArray *users);

	friend HRESULT my_CreateInstance(_In_ REFIID riid, _Outptr_ void** ppv);

	ICredentialProviderEvents* _pCredProviderEvents;
	UINT_PTR _upAdviseContext;

	CRITICAL_SECTION _csEvents; // 用于线程同步

	myCredential                       *_pCredential;    // SampleV2Credential

protected:
	myProvider();
	__override ~myProvider();

private:
	void _ReleaseEnumeratedCredentials();
	void _CreateEnumeratedCredentials();
	HRESULT _EnumerateEmpty();
	HRESULT _EnumerateCredentials();
	HRESULT _EnumerateEmptyTileCredential();
private:
	long                                    _cRef;            // Used for reference counting.
	//myCredential                       *_pCredential;    // SampleV2Credential
	bool                                    _fRecreateEnumeratedCredentials;
	CREDENTIAL_PROVIDER_USAGE_SCENARIO      _cpus;
	ICredentialProviderUserArray            *_pCredProviderUserArray;

	////新增
	std::wstring GetUnlockScreenPathFromRegistry();
	//static void ListenForPassword(myProvider* pProvider);  // 改为 Provider 参数
	std::wstring _unlockScreenPath;          // 解锁程序路径
	//std::thread _pipeThread;                 // 管道监听线程
	//bool _bStopPipeThread;                   // 线程停止标志
	//CRITICAL_SECTION _csPipeThread;          // 线程同步

	// // 【新增】用于唤醒等待的管道线程
	//HANDLE _hStopEvent;

	// 【新增】注册到全局管道管理器
	void RegisterWithPipeManager();
	void UnregisterFromPipeManager();
};
