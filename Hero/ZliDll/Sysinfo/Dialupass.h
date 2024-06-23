// Dialupass.h: interface for the CDialupass class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALUPASS_H__B0BBD3E2_526C_4B10_A877_95E6D12F31D2__INCLUDED_)
#define AFX_DIALUPASS_H__B0BBD3E2_526C_4B10_A877_95E6D12F31D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <windows.h>
#include <ras.h>
#include <raserror.h>
#include <shlobj.h>
#include <ntsecapi.h>
#pragma comment(lib, "rasapi32.lib")

#define STR_DialParamsUID	0
#define STR_Name			1
#define STR_User			2
#define STR_Password		3
#define STR_PhoneNumber		4
#define STR_Device			5
#define STR_MAX				6


typedef struct
{
	char	UID[256];
	char	pass[256];
	char	login[256];
	bool	used;
}PASSWORDS, *PPASSWORDS;

class COneInfo{
private:
	char *Str[STR_MAX];
	
public:
	COneInfo(void)
	{
		for(int i=0;i<STR_MAX;i++)
        Str[i]=NULL;
	}

	virtual ~COneInfo(void)
	{
		for(int i=0;i<STR_MAX;i++){
			if(Str[i]!=NULL)
				delete [] Str[i];
		}
	}
	
	void Set(int Kind,char *str)
	{
		if(str==NULL)
			return;
		if(Str[Kind]!=NULL){
			delete [] Str[Kind];
		}
		Str[Kind] = new char[strlen(str)+1];
		strcpy(Str[Kind],str);
	}
	char * Get(int Kind)
	{
		return Str[Kind];
	}
};

class CDialupass  
{
public:
	CDialupass();
	virtual ~CDialupass();
	int GetMax(void){ return m_nMax;}
    COneInfo * GetOneInfo(int n){return OneInfo[n];}
private:
	LPCTSTR UTF8ToGB2312(char UTF8Str[]);
	int m_nMax;
	int m_nUsed;
	int m_nCount;
	int m_nRasCount;
	char *m_lpCurrentUser;
	COneInfo **OneInfo;
	PASSWORDS *m_PassWords;
	BOOL Set(char *DialParamsUID, char *Name,char *User,char *Password,char *PhoneNumber, char *Device);
	DWORD GetRasEntryCount();
	void GetLsaPasswords();
	void ParseLsaBuffer(LPCWSTR Buffer,USHORT Length);
	PLSA_UNICODE_STRING GetLsaData(LPSTR KeyName);
	void AnsiStringToLsaStr(LPSTR AValue,PLSA_UNICODE_STRING lsa);
	LPTSTR GetLocalSid();
	bool GetRasEntries();
};

#endif // !defined(AFX_DIALUPASS_H__B0BBD3E2_526C_4B10_A877_95E6D12F31D2__INCLUDED_)
