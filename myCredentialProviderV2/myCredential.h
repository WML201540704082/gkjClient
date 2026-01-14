#pragma once

#include <windows.h>
#include <strsafe.h>
#include <shlguid.h>
#include <propkey.h>
#include <thread>
#include <string>
#include "common.h"
#include "dll.h"
#include "resource.h"

class myProvider;

class myCredential : public ICredentialProviderCredential2, ICredentialProviderCredentialWithFieldOptions
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
		if (!cRef)
		{
			delete this;
		}
		return cRef;
	}

	IFACEMETHODIMP QueryInterface(_In_ REFIID riid, _COM_Outptr_ void **ppv)
	{
		static const QITAB qit[] =
		{
			QITABENT(myCredential, ICredentialProviderCredential), // IID_ICredentialProviderCredential
			QITABENT(myCredential, ICredentialProviderCredential2), // IID_ICredentialProviderCredential2
			QITABENT(myCredential, ICredentialProviderCredentialWithFieldOptions), //IID_ICredentialProviderCredentialWithFieldOptions
			{0},
		};
		return QISearch(this, qit, riid, ppv);
	}
public:
	// ICredentialProviderCredential
	IFACEMETHODIMP Advise(_In_ ICredentialProviderCredentialEvents *pcpce);
	IFACEMETHODIMP UnAdvise();

	IFACEMETHODIMP SetSelected(_Out_ BOOL *pbAutoLogon);
	IFACEMETHODIMP SetDeselected();

	IFACEMETHODIMP GetFieldState(DWORD dwFieldID,
		_Out_ CREDENTIAL_PROVIDER_FIELD_STATE *pcpfs,
		_Out_ CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE *pcpfis);

	IFACEMETHODIMP GetStringValue(DWORD dwFieldID, _Outptr_result_nullonfailure_ PWSTR *ppwsz);
	IFACEMETHODIMP GetBitmapValue(DWORD dwFieldID, _Outptr_result_nullonfailure_ HBITMAP *phbmp);
	IFACEMETHODIMP GetCheckboxValue(DWORD dwFieldID, _Out_ BOOL *pbChecked, _Outptr_result_nullonfailure_ PWSTR *ppwszLabel);
	IFACEMETHODIMP GetComboBoxValueCount(DWORD dwFieldID, _Out_ DWORD *pcItems, _Deref_out_range_(< , *pcItems) _Out_ DWORD *pdwSelectedItem);
	IFACEMETHODIMP GetComboBoxValueAt(DWORD dwFieldID, DWORD dwItem, _Outptr_result_nullonfailure_ PWSTR *ppwszItem);
	IFACEMETHODIMP GetSubmitButtonValue(DWORD dwFieldID, _Out_ DWORD *pdwAdjacentTo);

	IFACEMETHODIMP SetStringValue(DWORD dwFieldID, _In_ PCWSTR pwz);
	IFACEMETHODIMP SetCheckboxValue(DWORD dwFieldID, BOOL bChecked);
	IFACEMETHODIMP SetComboBoxSelectedValue(DWORD dwFieldID, DWORD dwSelectedItem);
	IFACEMETHODIMP CommandLinkClicked(DWORD dwFieldID);

	IFACEMETHODIMP GetSerialization(_Out_ CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE *pcpgsr,
		_Out_ CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *pcpcs,
		_Outptr_result_maybenull_ PWSTR *ppwszOptionalStatusText,
		_Out_ CREDENTIAL_PROVIDER_STATUS_ICON *pcpsiOptionalStatusIcon);
	IFACEMETHODIMP ReportResult(NTSTATUS ntsStatus,
		NTSTATUS ntsSubstatus,
		_Outptr_result_maybenull_ PWSTR *ppwszOptionalStatusText,
		_Out_ CREDENTIAL_PROVIDER_STATUS_ICON *pcpsiOptionalStatusIcon);


	// ICredentialProviderCredential2
	IFACEMETHODIMP GetUserSid(_Outptr_result_nullonfailure_ PWSTR *ppszSid);

	// ICredentialProviderCredentialWithFieldOptions
	IFACEMETHODIMP GetFieldOptions(DWORD dwFieldID,
		_Out_ CREDENTIAL_PROVIDER_CREDENTIAL_FIELD_OPTIONS *pcpcfo);

public:
	HRESULT Initialize(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
		_In_ CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR const *rgcpfd,
		_In_ FIELD_STATE_PAIR const *rgfsp,
		_In_ ICredentialProviderUser *pcpUser);
	myCredential(myProvider* pProvider);

	ICredentialProviderCredentialEvents2*    _pCredProvCredentialEvents;                    // Used to update fields.


private:

	virtual ~myCredential();
	long                                    _cRef;
	CREDENTIAL_PROVIDER_USAGE_SCENARIO      _cpus;                                          // The usage scenario for which we were enumerated.
	CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR    _rgCredProvFieldDescriptors[SFI_NUM_FIELDS];    // An array holding the type and name of each field in the tile.
	FIELD_STATE_PAIR                        _rgFieldStatePairs[SFI_NUM_FIELDS];             // An array holding the state of each field in the tile.
	PWSTR                                   _rgFieldStrings[SFI_NUM_FIELDS];                // An array holding the string value of each field. This is different from the name of the field held in _rgCredProvFieldDescriptors.
	PWSTR                                   _pszUserSid;
	PWSTR                                   _pszQualifiedUserName;                          // The user name that's used to pack the authentication buffer
	//ICredentialProviderCredentialEvents2*    _pCredProvCredentialEvents;                    // Used to update fields.
																							// CredentialEvents2 for Begin and EndFieldUpdates.
	BOOL                                    _fChecked;                                      // Tracks the state of our checkbox.
	DWORD                                   _dwComboIndex;                                  // Tracks the current index of our combobox.
	bool                                    _fShowControls;                                 // Tracks the state of our show/hide controls link.
	bool                                    _fIsLocalUser;                                  // If the cred prov is assosiating with a local user tile

	//std::thread _pipeThread;
	//static void ListenForPassword(myCredential* pCredential);
	//myProvider* _pProvider; // 新增成员变量，指向关联的 Provider

	std::wstring unlockScreenPath;
	std::wstring GetUnlockScreenPathFromRegistry();

	//下面是新增
	//void LaunchInActiveSession(const std::wstring& exePath);
	//bool _bProcessLaunched;      // 是否已启动进程
	//DWORD _dwLaunchTime;         // 启动时间

};