// Dialupass.cpp: implementation of the CDialupass class.

#include "Dialupass.h"
#include <Tlhelp32.h>
#include <Wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "shell32.lib")
#ifndef WINVER
#define WINVER 0x500
#endif
#include <userenv.h>
#pragma comment(lib,"userenv.lib")

LPCSTR PBDIR=" Data\\Microsoft\\Network\\Connections\\pbk\\rasphone.pbk";
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DWORD GetProcessID(LPCTSTR lpProcessName)
{
	DWORD RetProcessID = 0;
	HANDLE handle=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32* info=new PROCESSENTRY32;
	info->dwSize=sizeof(PROCESSENTRY32);
	
	if(Process32First(handle,info))
	{
		if (stricmp(info->szExeFile,lpProcessName) == 0)
		{
			RetProcessID = info->th32ProcessID;
			delete info;
			return RetProcessID;
		}
		while(Process32Next(handle,info) != FALSE)
		{
			if (lstrcmpi(info->szExeFile,lpProcessName) == 0)
			{
				RetProcessID = info->th32ProcessID;
				delete info;
				return RetProcessID;
			}
		}
	}
	
	CloseHandle(handle);//ÐÂÐÞ¸Ä
	delete info;
	return RetProcessID;
}

char *GetLogUserXP()
{
	TCHAR	*szLogName = NULL;
	DWORD	dwSize = 0;
	if (WTSQuerySessionInformationA(WTS_CURRENT_SERVER_HANDLE, WTS_CURRENT_SESSION, WTSUserName, &szLogName, &dwSize))
	{
		char	*lpUser = new char[256];
		lstrcpy(lpUser, szLogName);
		WTSFreeMemory(szLogName);
		return lpUser;
	}
	else
		return NULL;
}

char *GetLogUser2K()
{
	DWORD	dwProcessID = GetProcessID("explorer.exe");
	if (dwProcessID == 0)
		return NULL;
	
	BOOL fResult  = FALSE;
    HANDLE hProc  = NULL;
	HANDLE hToken = NULL;
	TOKEN_USER *pTokenUser = NULL;
	char	*lpUserName = NULL;
	
	__try
	{
        // Open the process with PROCESS_QUERY_INFORMATION access
        hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessID);
        if (hProc == NULL)
		{
			__leave;
		}
        fResult = OpenProcessToken(hProc, TOKEN_QUERY, &hToken);
        if(!fResult)  
		{
			__leave;
		}
		
		DWORD dwNeedLen = 0;		
		fResult = GetTokenInformation(hToken,TokenUser, NULL, 0, &dwNeedLen);
		if (dwNeedLen > 0)
		{
			pTokenUser = (TOKEN_USER*)new BYTE[dwNeedLen];
			fResult = GetTokenInformation(hToken,TokenUser, pTokenUser, dwNeedLen, &dwNeedLen);
			if (!fResult)
			{
				__leave;
			}
		}
		else
		{
			__leave;
		}
		
		SID_NAME_USE sn;
		TCHAR szDomainName[MAX_PATH];
		DWORD dwDmLen = MAX_PATH;
		
		DWORD	nNameLen = 256;
		lpUserName = new char[256];
		fResult = LookupAccountSid(NULL, pTokenUser->User.Sid, lpUserName, &nNameLen,
			szDomainName, &dwDmLen, &sn);
	}
	__finally
	{
		if (hProc)
			::CloseHandle(hProc);

		if (hToken)
			::CloseHandle(hToken);

		if (pTokenUser)
			delete[] (char*)pTokenUser;
		
	}
    return lpUserName;
}

char *GetCurrentLoginUser()
{
	OSVERSIONINFOEX    OsVerInfo;
	ZeroMemory(&OsVerInfo, sizeof(OSVERSIONINFOEX));
	OsVerInfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
	if(!GetVersionExA((OSVERSIONINFO *)&OsVerInfo))
	{
		OsVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!GetVersionEx((OSVERSIONINFO *)&OsVerInfo))
			return NULL;
	}
	
	if(OsVerInfo.dwMajorVersion == 5 && OsVerInfo.dwMinorVersion == 0)
		return GetLogUser2K();
	else
		return GetLogUserXP();
	
}

///////////////////////////////////////////////////////////////////////////////////////////////
CDialupass::CDialupass()
{
    m_nMax = 0;
	m_lpCurrentUser = GetCurrentLoginUser();
	if (m_lpCurrentUser==NULL)
	{
		m_lpCurrentUser=new char [20];
		strcpy(m_lpCurrentUser,"Administrator");
	}
	m_nRasCount = GetRasEntryCount();//+20;
	m_PassWords = new PASSWORDS[m_nRasCount];
	OneInfo = new COneInfo* [m_nRasCount];
	m_nUsed = 0;
	m_nCount = 0; 
    GetRasEntries();
}

CDialupass::~CDialupass()
{
	if (OneInfo)
	{
		for(int i=0; i<m_nMax; i++)
			delete OneInfo[i];
		delete [] OneInfo;
	}
	delete [] m_PassWords;
	if (m_lpCurrentUser)
		delete [] m_lpCurrentUser;
}

bool FileExists(char * file)
{
	return (GetFileAttributes(file) != INVALID_FILE_ATTRIBUTES);
}


HANDLE GetCurrentLoginToken()
{
	DWORD        dwProcessID = GetProcessID("explorer.exe");
	if (dwProcessID == 0)
		return NULL;
	
	BOOL fResult = FALSE;
	HANDLE hProc = NULL;
	HANDLE hToken = NULL;
	
	__try
	{
		// Open the process with PROCESS_QUERY_INFORMATION access
		hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessID);
		if (hProc == NULL)
		{
			__leave;
		}
		fResult = OpenProcessToken(hProc, TOKEN_QUERY, &hToken);
		if(!fResult) 
		{
			__leave;
		}
	}
	__finally
	{
		if (hProc) CloseHandle(hProc);
		//		return hToken;
	}
	return hToken;
}

DWORD CDialupass::GetRasEntryCount()
{
	int   nCount = 0;
	int   nDirCount=2;
	char *lpPhoneBook[3];
    char szPhoneBook1[MAX_PATH+1], szPhoneBook2[MAX_PATH+1],szPhoneBook3[MAX_PATH+1];
	
	HANDLE hToken=GetCurrentLoginToken();
	DWORD dwSize=sizeof(szPhoneBook1);
	GetProfilesDirectory(szPhoneBook1,&dwSize);
	
	lstrcat(szPhoneBook1, "\\");
	lstrcat(szPhoneBook1, m_lpCurrentUser);
	lstrcat(szPhoneBook1, PBDIR);
	
	if (FileExists(szPhoneBook1)==false)
	{
		ExpandEnvironmentStrings("%USERPROFILE%\\Application Data\\Microsoft\\Network\\Connections\\pbk\\rasphone.pbk",
			szPhoneBook1,sizeof(szPhoneBook1));
	}
	SHGetSpecialFolderPath(NULL,szPhoneBook2, 0x23, 0);
	wsprintf(szPhoneBook2, "%s\\%s", szPhoneBook2, "Microsoft\\Network\\Connections\\pbk\\rasphone.pbk");
	
	dwSize=sizeof(szPhoneBook3);
	GetUserProfileDirectory(hToken,szPhoneBook3,&dwSize);
	lstrcat(szPhoneBook3,PBDIR);
	
	if (hToken) 
		CloseHandle(hToken);
	
	lpPhoneBook[0] = szPhoneBook1;
	lpPhoneBook[1] = szPhoneBook2;
	if (FileExists(szPhoneBook3) && stricmp(szPhoneBook1,szPhoneBook3)!=0) 
	{
		nDirCount++;
		lpPhoneBook[2] = szPhoneBook3;
	}
	
	DWORD nSize = 1024 * 8;
	char *lpszReturnBuffer = new char[nSize];
	
	nCount=0;
	for (int i = 0; i < nDirCount; i++)
	{
		memset(lpszReturnBuffer, 0, nSize);
		GetPrivateProfileSectionNames(lpszReturnBuffer, nSize, lpPhoneBook[i]);
		for(char *lpSection = lpszReturnBuffer; *lpSection != '\0'; lpSection += lstrlen(lpSection) + 1)
		{
			nCount++;
		}
	}
	delete [] lpszReturnBuffer;
	return nCount;
}

LPTSTR CDialupass::GetLocalSid()
{
	union
	{
		SID s;
		char c[256];
	}Sid;
	
	DWORD sizeSid=sizeof(Sid);
	char DomainName[256];
	DWORD sizeDomainName=sizeof(DomainName);
	SID_NAME_USE peUse;
	
	LPSTR pSid;
	if (m_lpCurrentUser == NULL)
		return NULL;
	
	if(!LookupAccountName(NULL,m_lpCurrentUser,(SID*)&Sid,&sizeSid,DomainName,&sizeDomainName,&peUse))return NULL;
	if(!IsValidSid(&Sid))return NULL;
	
	
	typedef BOOL (WINAPI *ConvertSid2StringSid)(PSID , LPTSTR *);
	ConvertSid2StringSid proc;
	HINSTANCE hLibrary = LoadLibrary("advapi32.dll");
	proc = (ConvertSid2StringSid) GetProcAddress(hLibrary, "ConvertSidToStringSidA");
	if(proc)   proc((SID*)&Sid.s,&pSid);
	FreeLibrary(hLibrary);
	return pSid;
}


void CDialupass::AnsiStringToLsaStr(LPSTR AValue,PLSA_UNICODE_STRING lsa)
{
	lsa->Length=lstrlen(AValue)*2;
	lsa->MaximumLength=lsa->Length+2;
	//lsa->Buffer=(PWSTR)LocalAlloc(LPTR,lsa->MaximumLength);
	lsa->Buffer=(PWSTR)new char[lsa->MaximumLength];
	MultiByteToWideChar(NULL,NULL,(LPCSTR)AValue,lstrlen(AValue),lsa->Buffer,lsa->MaximumLength);
}


PLSA_UNICODE_STRING CDialupass::GetLsaData(LPSTR KeyName)
{
	LSA_OBJECT_ATTRIBUTES LsaObjectAttribs;
	LSA_HANDLE LsaHandle;
	LSA_UNICODE_STRING LsaKeyName,OutData;
	NTSTATUS nts;
	PLSA_UNICODE_STRING pOutData=&OutData;
	
	ZeroMemory(&LsaObjectAttribs,sizeof(LsaObjectAttribs));
	nts=LsaOpenPolicy(NULL,&LsaObjectAttribs,POLICY_GET_PRIVATE_INFORMATION,&LsaHandle);
	if(nts!=0) return NULL;
	AnsiStringToLsaStr(KeyName, &LsaKeyName);
	
    pOutData->Length = 0x500;
	pOutData->MaximumLength = 0x500;
	//OutData->Buffer = new WCHAR[500];
	
	nts=LsaRetrievePrivateData(LsaHandle, &LsaKeyName,&pOutData);
	delete [] (char *)LsaKeyName.Buffer;
	//LocalFree(LsaKeyName.Buffer);
	nts=LsaClose(LsaHandle);
	if(nts!=0) return NULL;
	return pOutData;
}

/////////
void CDialupass::ParseLsaBuffer(LPCWSTR Buffer,USHORT Length)
{
	char AnsiPsw[1024];
	char chr,PswStr[256];
	PswStr[0]=0;
	WideCharToMultiByte(0,NULL,Buffer,Length,AnsiPsw,1024,0,0);
	
	for(int i=0,SpacePos=0,TXT=0;i<Length/2-2;i++)
	{
		chr=AnsiPsw[i];
		if(chr==0)
		{
			SpacePos++;
			switch(SpacePos)
			{
			case 1:
				PswStr[TXT]=chr;
				strcpy(m_PassWords[m_nUsed].UID,PswStr);
				break;
			case 6:
				PswStr[TXT]=chr;
				strcpy(m_PassWords[m_nUsed].login,PswStr);
				break;
			case 7:
				PswStr[TXT]=chr;
				strcpy(m_PassWords[m_nUsed].pass,PswStr);
				m_PassWords[m_nUsed].used=false;
				m_nUsed++;
				if(m_nUsed>=m_nRasCount) return;
				break;
			}
			ZeroMemory(PswStr,256);
			TXT=0;
		}
		else 
		{
			PswStr[TXT]=chr;
			TXT++;
		}
		if(SpacePos==9)SpacePos=0;
	}
}

///////////
void CDialupass::GetLsaPasswords()
{
	PLSA_UNICODE_STRING PrivateData;
	char Win2k[]="RasDialParams!%s#0";
	char WinXP[]="L$_RasDefaultCredentials#0";
	char temp[256];
	
	wsprintf(temp,Win2k,GetLocalSid());
	
	PrivateData=GetLsaData(temp);
	if(PrivateData!=NULL)
	{
		ParseLsaBuffer(PrivateData->Buffer,PrivateData->Length);
		// 		if(PrivateData->Buffer)
		// 		    LsaFreeMemory(PrivateData->Buffer); 
	}
	
	PrivateData=GetLsaData(WinXP);
	if(PrivateData!=NULL)
	{ 
		ParseLsaBuffer(PrivateData->Buffer,PrivateData->Length);
		// 		if(PrivateData->Buffer)
		// 			LsaFreeMemory(PrivateData->Buffer); 
	}
}


bool CDialupass::GetRasEntries()
{
	
    int   nCount = 0;
    int   nDirCount=2;
    char *lpPhoneBook[3];
    char szPhoneBook1[MAX_PATH+1], szPhoneBook2[MAX_PATH+1],szPhoneBook3[MAX_PATH+1];
    HANDLE hToken=GetCurrentLoginToken();
    DWORD dwSize=sizeof(szPhoneBook1);
    GetProfilesDirectory(szPhoneBook1,&dwSize);
    lstrcat(szPhoneBook1, "\\");
    lstrcat(szPhoneBook1, m_lpCurrentUser);
    lstrcat(szPhoneBook1, PBDIR);
    if (FileExists(szPhoneBook1)==false)
	{
        ExpandEnvironmentStrings("%USERPROFILE%\\Application Data\\Microsoft\\Network\\Connections\\pbk\\rasphone.pbk",
			szPhoneBook1,sizeof(szPhoneBook1));
	}
    SHGetSpecialFolderPath(NULL,szPhoneBook2, 0x23, 0);
	wsprintf(szPhoneBook2, "%s\\%s", szPhoneBook2, "Microsoft\\Network\\Connections\\pbk\\rasphone.pbk");
	
	dwSize=sizeof(szPhoneBook3);
	GetUserProfileDirectory(hToken,szPhoneBook3,&dwSize);
	lstrcat(szPhoneBook3,PBDIR);
	if (hToken) 
		CloseHandle(hToken);
	
	
	lpPhoneBook[0] = szPhoneBook1;
	lpPhoneBook[1] = szPhoneBook2;
	if (FileExists(szPhoneBook3) && stricmp(szPhoneBook1,szPhoneBook3)!=0) 
	{
		nDirCount++;
		lpPhoneBook[2] = szPhoneBook3;
	}
	OSVERSIONINFO osi;
	osi.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&osi);
	
	if(osi.dwPlatformId == VER_PLATFORM_WIN32_NT && osi.dwMajorVersion >= 5)
	{
		GetLsaPasswords();
	} 
	
	DWORD nSize = 1024 * 4;
	char *lpszReturnBuffer = new char[nSize];
	int i;
	for (i = 0; i < nDirCount; i++)
	{
		memset(lpszReturnBuffer, 0, nSize);
		GetPrivateProfileSectionNames(lpszReturnBuffer, nSize, lpPhoneBook[i]);
		for(char *lpSection = lpszReturnBuffer; *lpSection != '\0'; lpSection += lstrlen(lpSection) + 1)
		{ 
			char *lpRealSection = (char *)UTF8ToGB2312(lpSection);
			char strDialParamsUID[256];
			char strUserName[256];
			char strPassWord[256];
			char strPhoneNumber[256];
			char strDevice[256];
			memset(strDialParamsUID, 0, sizeof(strDialParamsUID));
			memset(strUserName, 0, sizeof(strUserName));
			memset(strPassWord, 0, sizeof(strPassWord));
			memset(strPhoneNumber, 0, sizeof(strPhoneNumber));
			memset(strDevice, 0, sizeof(strDevice));
			
			int nBufferLen = GetPrivateProfileString(lpSection, "DialParamsUID", 0, 
				strDialParamsUID, sizeof(strDialParamsUID), lpPhoneBook[i]);
			
			if (nBufferLen > 0)//DialParamsUID=4326020    198064
			{
				for(int j=0; j< m_nUsed; j++)
				{
					if(lstrcmp(strDialParamsUID, m_PassWords[j].UID)==0)
					{
						lstrcpy(strUserName, m_PassWords[j].login); 
						lstrcpy(strPassWord, m_PassWords[j].pass); 
						m_PassWords[j].used=true;
						//m_nUsed++;
						break;
					}
				}
			}
			
			GetPrivateProfileString(lpSection, "PhoneNumber", 0, 
				strPhoneNumber, sizeof(strDialParamsUID), lpPhoneBook[i]);
			GetPrivateProfileString(lpSection, "Device", 0, 
				strDevice, sizeof(strDialParamsUID), lpPhoneBook[i]);
			char *lpRealDevice = (char *)UTF8ToGB2312(strDevice);
			char *lpRealUserName = (char *)UTF8ToGB2312(strUserName);
			Set(strDialParamsUID, lpRealSection, lpRealUserName, strPassWord,
				strPhoneNumber, lpRealDevice);
			if (lpRealSection)
			{
				delete [] lpRealSection;
				lpRealSection=NULL;
			}
			if (lpRealUserName) 
			{
				delete [] lpRealUserName;
				lpRealUserName=NULL;
			}
			if (lpRealDevice)
			{
				delete [] lpRealDevice;
				lpRealDevice=NULL;
			}
		}
	}
	for (i=0;i<m_nUsed;i++)
	{
		if(m_PassWords[i].used ==false && (m_PassWords[i].login[0] || m_PassWords[i].pass[0]))
			Set(m_PassWords[i].UID,"",m_PassWords[i].login,m_PassWords[i].pass,"","");
	}
	
	delete [] lpszReturnBuffer;
	return true;
}

BOOL CDialupass::Set(char *DialParamsUID, char *Name,char *User,char *Password,char *PhoneNumber, char *Device)
{
	for(int i=0; i<m_nMax; i++){
		if(0==strcmp(OneInfo[i]->Get(STR_DialParamsUID), DialParamsUID)){
			
            if(Name!=NULL)
				OneInfo[i]->Set(STR_Name,Name);
			if(User!=NULL)
				OneInfo[i]->Set(STR_User,User);
			if(Password!=NULL)
				OneInfo[i]->Set(STR_Password,Password);
			if(PhoneNumber!=NULL)
				OneInfo[i]->Set(STR_PhoneNumber,PhoneNumber);
			if(Device!=NULL)
				OneInfo[i]->Set(STR_Device, Device);
			return TRUE;
		}
	}
	
    if(m_nMax < m_nRasCount){
		
        OneInfo[m_nMax] = new COneInfo;
		OneInfo[m_nMax]->Set(STR_DialParamsUID,DialParamsUID);
		OneInfo[m_nMax]->Set(STR_Name,Name);
		OneInfo[m_nMax]->Set(STR_User,User);
		OneInfo[m_nMax]->Set(STR_Password,Password);
		OneInfo[m_nMax]->Set(STR_PhoneNumber,PhoneNumber);
		OneInfo[m_nMax]->Set(STR_Device,Device);
		m_nMax ++;
		return TRUE;
	}
	return false;
}


LPCTSTR CDialupass::UTF8ToGB2312(char UTF8Str[])
{
	if (UTF8Str == NULL || lstrlen(UTF8Str) == 0)
		return new char(0);
	
	int nStrLen = lstrlen(UTF8Str) * 5;
	char *lpWideCharStr = new char[nStrLen];
	char *lpMultiByteStr = new char[nStrLen];
	
	MultiByteToWideChar(CP_UTF8, 0, UTF8Str, -1, (LPWSTR)lpWideCharStr, nStrLen);
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)lpWideCharStr, -1, lpMultiByteStr, nStrLen, 0, 0);
	
	delete [] lpWideCharStr;
	return lpMultiByteStr;
}

