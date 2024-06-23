// LOGIN.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "LOGIN.h"
#include "BuildDlg.h"
#include <afxinet.h>
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// LOGIN dialog
CString IP1;
LOGIN::LOGIN(CWnd* pParent /*=NULL*/)
: CDialog(LOGIN::IDD, pParent)
{
	//{{AFX_DATA_INIT(LOGIN)
	m_username = _T("");
	m_userpass = _T("");
	m_onlinepass = _T("");
	m_jzmm = m_inifile.GetInt("USER","JZMM",1);
	m_bisDisablePifu = m_inifile.GetInt("Settings", "SKIN", 1);//软件皮肤
	//}}AFX_DATA_INIT
}


void LOGIN::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LOGIN)
	DDX_Text(pDX, IDC_username, m_username);
	DDX_Text(pDX, IDC_userpass, m_userpass);
	DDX_Check(pDX, IDC_CHECK2,  m_jzmm);
	DDX_Check(pDX, IDC_DISABLE_Pifu,  m_bisDisablePifu);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_GIF, m_GifPic);

}

BEGIN_MESSAGE_MAP(LOGIN, CDialog)
//{{AFX_MSG_MAP(LOGIN)
ON_BN_CLICKED(IDC_LOGIN, OnLogin)
ON_BN_CLICKED(IDC_LXKF, OnLXKF)
ON_BN_CLICKED(IDC_GFWZ, OnGFWZ)
ON_BN_CLICKED(IDC_SYJC, OnSYJC)
ON_BN_CLICKED(IDC_DISABLE_Pifu,OnDISABLEPifu)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LOGIN message handlers
//加密
CString Encrypt(CString S, WORD K)
{
	CString Str,Str1,Result;
	int i,j;
	Str = S;
	for(i=0;i<S.GetLength();i++)
	{
		Str.SetAt(i,S.GetAt(i)+K);
	}
	S = Str;
	for(i=0;i<S.GetLength();i++)
	{
		j = (BYTE)S.GetAt(i);
		Str1 = "01";
		Str1.SetAt(0,65+j/26);
		Str1.SetAt(1,65+j%26);
		Result += Str1;
	}
	return Result;
}
//解密
CString Decrypt(CString v, WORD K)
{
	CString Result,Str;
	int i,j;
	for(i=0;i<v.GetLength()/2;i++)
	{
		j=((BYTE)v.GetAt(2*i)-65)*26;
		j+=(BYTE)v.GetAt(2*i+1)-65;
		Str = "0";
		Str.SetAt(0,j);
		Result += Str;
	}
	v = Result;
	for(i=0;i<v.GetLength();i++)
	{
		Result.SetAt(i,(BYTE)v.GetAt(i)-K);
	}
	return Result;
}
//获取本机外网IP
CString GetHtml()
{
	CString content;
	CString data;
	DWORD dwStatusCode;
	CInternetSession session("HttpClient");
	CHttpFile* pfile = (CHttpFile *)session.OpenURL("http://members.3322.org/dyndns/getip");
	pfile -> QueryInfoStatusCode(dwStatusCode);
	if(dwStatusCode == HTTP_STATUS_OK)
	{ 
		while (pfile -> ReadString(data))
		{
			content  += data;
		}
	}
	pfile -> Close();
	delete pfile;
	session.Close();
	return content;
}
//获取域名IP
CString GotHost(CString YM)
{
	WSAData wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);
	hostent *phst=gethostbyname(YM);
	in_addr * iddr=(in_addr*)phst->h_addr;
	unsigned long IPUL=iddr->s_addr;
	char *IP=inet_ntoa(*iddr);
	return IP;
}
void LOGIN::OnLogin() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_username.GetLength() == 0 || m_userpass.GetLength() == 0 )
	{
		AfxMessageBox("请填写用户名和密码.");
		return;
	}
	//---------------------------------------//
	DWORD recvsize = 0;
	SOCKET sockInt;
	struct sockaddr_in serverAddr;
	WORD sockVersion;
    WSADATA wsaData;
    sockVersion = MAKEWORD(2,2);
    WSAStartup(sockVersion, &wsaData);
	//创建SOCK
	sockInt = socket(AF_INET, SOCK_STREAM, 0);
	if(sockInt == INVALID_SOCKET)
	{
		AfxMessageBox("socket error!\n");
		WSACleanup();
		return;
	}
	//------获取服务器IP和端口----//
	serverAddr.sin_family = AF_INET;    
	char tgtIP[30] = {0};
	struct hostent *hp = NULL;
	if ((hp = gethostbyname("svp7.net")) != NULL)
	{
		in_addr in;
		memcpy(&in, hp->h_addr, hp->h_length);
		lstrcpy(tgtIP,inet_ntoa(in));
	}
	serverAddr.sin_addr.s_addr = inet_addr(tgtIP);
	serverAddr.sin_port = htons(8689);
	
	//连接服务器
	if(connect(sockInt, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		AfxMessageBox("连接服务器失败!");
		WSACleanup();
		return;
	}
	char USERIMFOR[256] = {0}, buff[256] = {0};
	wsprintf(USERIMFOR, "Login:%s@%s", m_username, m_userpass );
	if( send(sockInt, USERIMFOR, sizeof(USERIMFOR), 0) == SOCKET_ERROR )
	{
		AfxMessageBox("账号不存在!");
		WSACleanup();
		return;
	}
	Sleep(50);
	int Ret = recv( sockInt, buff, sizeof(buff), NULL );
	if ( Ret == 0 || Ret == SOCKET_ERROR )
	{
		AfxMessageBox("账号密码错误,或已到期!");
		closesocket(sockInt);
		WSACleanup();
		return;
	}
	//获取IP
	NET_DATA MyData;
	BYTE request[256] = {0};
	request[0] = Request_DOWN;
	send(sockInt,(char*)&request, sizeof(request),NULL);
	memset(&MyData,0,sizeof(NET_DATA));
	recv(sockInt,(char*)&MyData, sizeof(MyData),NULL);
	IP1 = MyData.Info;
	
	//判断IP还是域名
	if(inet_addr(IP1)==INADDR_NONE)
		IP1=GotHost(IP1);//域名转换IP

	//判断绑定IP和本机IP是否一致
	CString IP2=GetHtml();
	if (IP1!=IP2)
	{
		MessageBox("IP变动,无法登陆!!!\n\n请使用绑定的服务器登陆\n\n绑定的IP:"+IP1+"\n\n本机IP:"+IP2,"提醒");
		closesocket(sockInt);
		WSACleanup();
		return;
	}
	if (strstr( buff, "Pass" ) != NULL)//通过验证
	{
		closesocket(sockInt);
		WSACleanup();
		m_inifile.SetInt("USER","JZMM",m_jzmm);
		m_inifile.SetString( "USER", "username", m_username);
		if(m_jzmm==1)
		{
			CString userpass = Encrypt(m_userpass,1024);
			m_inifile.SetString( "USER", "userpass", userpass );
		}
		else
		{
			m_inifile.SetString( "USER", "userpass", "");
		}
		dLogin = GetTickCount();
		MessageBox(buff+6,"会员信息",NULL);
		OnCancel();
	}
}
void LOGIN::OnLXKF() 
{
	TCHAR szModuleFileName[MAX_PATH]; // 全路径名
	GetModuleFileName(NULL, szModuleFileName, MAX_PATH);
	PathRemoveFileSpec(szModuleFileName);
	strcat(szModuleFileName,"\\Plugins\\联系客服.url");
	ShellExecute(NULL, "Open",szModuleFileName, 0,  NULL, SW_MAXIMIZE);	
}
void LOGIN::OnGFWZ() 
{
	TCHAR szModuleFileName[MAX_PATH]; // 全路径名
	GetModuleFileName(NULL, szModuleFileName, MAX_PATH);
	PathRemoveFileSpec(szModuleFileName);
	strcat(szModuleFileName,"\\Plugins\\官方网站.url");
	ShellExecute(NULL, "Open",szModuleFileName, 0,  NULL, SW_MAXIMIZE);	
}
void LOGIN::OnSYJC() 
{
	TCHAR szModuleFileName[MAX_PATH]; // 全路径名
	GetModuleFileName(NULL, szModuleFileName, MAX_PATH);
	PathRemoveFileSpec(szModuleFileName);
	strcat(szModuleFileName,"\\Plugins\\使用教程.url");
	ShellExecute(NULL, "Open",szModuleFileName, 0,  NULL, SW_MAXIMIZE);	
}
void LOGIN::OnDISABLEPifu() //显示软件皮肤
{	
	UpdateData(true);
	if ( ((CButton *)GetDlgItem(IDC_DISABLE_Pifu))->GetCheck() == TRUE )
	{
		char Path[200]={0};
		GetModuleFileName(NULL,Path,200);
		*strrchr(Path,'\\')=0;
		strcat(Path,"\\Plugins\\SkinH.she");
		SkinH_AttachEx(Path,NULL);
	}
	else
	{
		SkinH_Detach();//关闭皮肤效果
	}
	m_bisDisablePifu=m_inifile.SetInt("Settings", "SKIN", m_bisDisablePifu);
}
BOOL LOGIN::OnInitDialog() 
{
	CDialog::OnInitDialog();
	HINSTANCE hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_MGICON_A),RT_GROUP_ICON);
	HICON m_hIcon = (HICON)::LoadImage(hInstResource,MAKEINTRESOURCE(IDI_MGICON_A),IMAGE_ICON,24, 24, 0);
	if( m_hIcon != 0 )
	{
		SetIcon(m_hIcon,TRUE);
	}
    m_GifPic.UnLoad(); // 释放资源初始化动画条
	if (m_GifPic.Load(MAKEINTRESOURCE(IDR_GIFROARING),_T("Gif")))
		m_GifPic.Draw();
	m_inifile.SetInt("Settings", "SKIN", m_bisDisablePifu);
	m_inifile.SetInt("USER","JZMM",m_jzmm);
	// TODO: Add extra initialization here
	SetDlgItemText( IDC_username, m_inifile.GetString( "USER", "username", "" ) );
	CString pass = m_inifile.GetString("USER","userpass","");
	if (pass.GetLength() != 0 )
	{ 
		CString userpass = Decrypt(pass,1024);
		SetDlgItemText(IDC_userpass,userpass);
	}
	UpdateData();
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
