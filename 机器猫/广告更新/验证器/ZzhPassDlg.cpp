// ZzhPassDlg.cpp : implementation file
//
#include "StdAfx.h"
#include "ZzhPass.h"
#include "ZzhPassDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZzhPassDlg dialog

#include "WINSOCK.H"

#define Request_DOWN			0x9
#define File_Buffer				0x10
#define File_Buffer_Finish		0x11

typedef struct
{
	BYTE	Flags;
	DWORD	Buffer_Size;
	BYTE	Buffer[1024];
	BYTE	Info[256];
}NET_DATA, *LPNET_DATA;

char *DAT_NAME = "Cache\\Install.dat";
char *INI_NAME = "公告.ini";
char DAT_PATH[MAX_PATH] = {0}, INI_PATH[MAX_PATH] = {0};

SOCKET m_hSocket;
HANDLE hLinstenThread = NULL;
CZzhPassDlg::CZzhPassDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZzhPassDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CZzhPassDlg)
	m_port = 0;
	m_username = _T("123");
	m_password = _T("123");
	m_dns = _T("127.0.0.1");
	m_date = _T("20300101");
	m_build = _T("100");
	m_gengx = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_port = 8688;
	GetCurrentDirectory( sizeof(DAT_PATH), DAT_PATH );
	lstrcpy( INI_PATH, DAT_PATH );
	lstrcat( DAT_PATH, "\\" );
	lstrcat( INI_PATH, "\\" );
	lstrcat( DAT_PATH, DAT_NAME );
	lstrcat( INI_PATH, INI_NAME );
}

void CZzhPassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZzhPassDlg)
	DDX_Text(pDX, IDC_EDIT1, m_port);
	DDV_MinMaxInt(pDX, m_port, 1, 65535);
	DDX_Text(pDX, IDC_USERNEMA, m_username);
	DDX_Text(pDX, IDC_PASSWORD, m_password);
	DDX_Text(pDX, IDC_DNS, m_dns);
	DDX_Text(pDX, IDC_DATA, m_date);
	DDX_Text(pDX, IDC_CLIENPORT, m_clienport);
	DDX_Text(pDX, IDC_BUILD, m_build);
	DDX_Text(pDX, IDC_GENGX, m_gengx);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CZzhPassDlg, CDialog)
	//{{AFX_MSG_MAP(CZzhPassDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZzhPassDlg message handlers

BOOL CZzhPassDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
OnCheck();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CZzhPassDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CZzhPassDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CZzhPassDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void My_GetTime(char Rstr[])
{
	time_t curtime=time(0);
	tm tim = *localtime(&curtime);
	int day,mon,year,hour,minute,seconds;
	day = tim.tm_mday;
	mon = tim.tm_mon;
	year = tim.tm_year;
	hour = tim.tm_hour;
	minute = tim.tm_min;
	seconds = tim.tm_sec;
	year += 1900;
	mon += 1;
	sprintf( Rstr, "%04d%02d%02d", year, mon, day );
}

void SendFile( SOCKET sock, char *username, char *ConnPass )//SOCKET,用户名,服务的配置信息
{
	char Buffer[128] = {0};
	HANDLE hFile = CreateFile( DAT_PATH, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox("DAT文件不存在!");
		return;
	}
	
	//验证使用次数
	char read_build[256];
	char new_build[256];
	memset( read_build, 0, sizeof(read_build) );
	GetPrivateProfileString( username, "build", NULL, read_build, sizeof(read_build), INI_PATH );
	if ( atoi(read_build) <= 0 )
	{
		CloseHandle(hFile);
		return;
	}

	//得到域名及其端口信息
	char read_dns[256] = {0};
	memset( read_dns, 0, sizeof(read_dns) );
	GetPrivateProfileString( username, "DNS", NULL, read_dns, sizeof(read_dns), INI_PATH );
	if ( lstrlen(read_dns) == 0 )
	{
		CloseHandle(hFile);
		return;//获取错误，直接返回
	}
	char finally_info[256];
	lstrcpy( finally_info, read_dns );
	NET_DATA MyData;
	while(1)
	{	
		memset( &MyData, 0, sizeof(NET_DATA) );
		MyData.Flags = File_Buffer;
		ReadFile(hFile, MyData.Buffer, 1024, &MyData.Buffer_Size, NULL); // 读取数据, 后面发送
		if ( MyData.Buffer_Size == 0 )//读取完了，文件发送完成，继续发送配置信息
		{
			CloseHandle(hFile);
			memset( new_build, 0, sizeof(new_build) );
			wsprintf( new_build, "%d", atoi(read_build) - 1 );
			WritePrivateProfileString( username, "build", new_build, INI_PATH ); // 使用次数减1

			memset( &MyData, 0, sizeof(NET_DATA) );
			MyData.Flags = File_Buffer_Finish;

			lstrcpy( (char*)&MyData.Info, finally_info );
			MyData.Buffer_Size = lstrlen(finally_info) + 1; // 发送配置信息 端口和域名
			
			if ( send( sock, (char*)&MyData, sizeof(MyData), NULL ) == SOCKET_ERROR ) break;
			recv( sock, Buffer, sizeof(Buffer), NULL );
			break;
		}
		if (send( sock, (char*)&MyData, sizeof(MyData), NULL ) == SOCKET_ERROR ) break; // 发送上面读取的数据
		recv( sock, Buffer, sizeof(Buffer), NULL );
	}
	CloseHandle(hFile);

	return;
}
char vipname[30];
BOOL CheckUser( char *buff, char *username )
{
//Login:用户名@密码

	char read_pass[256];
	char read_date[256];
	char bendi_date[256];
	if ( strncmp( buff, "Login:", 6 ) == 0 )//如果前面几个验证字符相等
	{	
		char *pos1 = strchr( buff, ':' );
		pos1++;
		char *pos2 = strchr( pos1, '@' );
		*pos2 = '\0';
		pos2++;
		lstrcpy(vipname,pos1);
		memset( read_pass, 0, sizeof(read_pass) );
		GetPrivateProfileString( pos1, "userpass", NULL, read_pass, sizeof(read_pass), INI_PATH );
		if ( lstrcmpi( pos2, read_pass ) == 0 )//完全相等，忽略大小写
		{	
			memset( read_date, 0, sizeof(read_date) );
			GetPrivateProfileString( pos1, "date", NULL, read_date, sizeof(read_date), INI_PATH );
			My_GetTime(bendi_date);
			if ( atoi(bendi_date) > atoi(read_date) ) return FALSE;
			lstrcpy( username, pos1 );
			return TRUE;
		}
		else
		{	
			return FALSE;
		}
	}
	else//连验证字符都不相等，直接PASS
	{	
		return FALSE;
	}
}


//多线程处理事件
DWORD WINAPI AnswerThread(LPVOID lparam)
{
	SOCKET ClientSocket = (SOCKET)lparam;
	char Buffer[1024] = {0};
	char UserName[30] = {0};
	int Ret = 0;
	BOOL isLogined = FALSE;
	char *strYes = "Pass";
	char *strNO = " ";
	char read_build[256];
	char radiostr[256];
	char sendstr[256];
	char read_date[30];
	while(1)
	{
		memset( Buffer, 0, sizeof(Buffer) );
		Ret = recv( ClientSocket, Buffer, sizeof(Buffer), NULL );
		if ( Ret == 0 || Ret == SOCKET_ERROR )
		{
			break;
		}

		if ( !isLogined )//没有登录
		{
			if ( CheckUser( Buffer, UserName ) )
			{
				GetPrivateProfileString( vipname, "date", NULL, read_date, sizeof(read_date), INI_PATH );
				GetPrivateProfileString( vipname, "build", NULL, read_build, sizeof(read_build), INI_PATH );
				GetPrivateProfileString(vipname, "radio", NULL,radiostr, sizeof(radiostr), INI_PATH );
	            wsprintf(sendstr,
					"%s\r\n%s\r\n\r\n%s",
					strYes,radiostr,read_build);
				isLogined = TRUE;
				send( ClientSocket, sendstr, lstrlen(sendstr) + 1, NULL );//发送字符，告诉客户端已经通过验证
				continue;
			}
			else//没有登录还不登录的，直接断开
			{
				break;
			}
		}
		else//已经登录
		{
			if ( Buffer[0] == Request_DOWN )//如果是获取配置文件
			{
				SendFile( ClientSocket, UserName, Buffer );//发送文件，然后断开
				break;
			}
			else//其他无用信息，直接断开连接，节省服务器资源
			{
				break;
			}
		}
	}
	closesocket(ClientSocket);
	return 0;
}

DWORD WINAPI ListenThread(LPVOID lparam)
{
	SOCKET AcceptSocket;
	DWORD dwThreadId;
	while (1)
	{
		AcceptSocket = SOCKET_ERROR;
		while(AcceptSocket == SOCKET_ERROR)
		{
			AcceptSocket = accept( m_hSocket, NULL, NULL );
		}
		CreateThread( NULL, NULL, AnswerThread, (LPVOID)AcceptSocket,0,&dwThreadId);
	}
	WSACleanup();
	return 0;
}

void CZzhPassDlg::OnStart() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if ( m_port <= 0 || m_port >65535 )
	{
		return;
	}

	WSADATA wsaData;
	SOCKADDR_IN m_addr;
	int iRet = 0;
	DWORD ThreadId;

	if ( m_hSocket != NULL )
	{
		closesocket(m_hSocket);
	}

	WSACleanup();
	iRet = WSAStartup(MAKEWORD(2,2),&wsaData);
	if( iRet != NO_ERROR ) AfxMessageBox("Error at WSAStartup()");

	memset( &m_addr, 0, sizeof(SOCKADDR) );
	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	m_addr.sin_port = htons(m_port);

	m_hSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if ( bind(m_hSocket,(LPSOCKADDR)&m_addr,sizeof(m_addr)) == SOCKET_ERROR )
	{
		AfxMessageBox("Bind失败,端口可能被占用!");
		WSACleanup();
		return;
	}
	if ( listen( m_hSocket, 10000 ) ==  SOCKET_ERROR )
	{
		AfxMessageBox("Listen失败!");
		WSACleanup();
		return;
	}
	hLinstenThread = CreateThread( NULL, NULL, ListenThread, NULL, 0, &ThreadId);
	((CEdit*)GetDlgItem(IDC_EDIT1))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_START))->EnableWindow(FALSE);	//禁用按钮
	return;
}

void CZzhPassDlg::OnExit()
{
	// TODO: Add your control notification handler code here
	if ( hLinstenThread != NULL )
	{
		CloseHandle(hLinstenThread);
	}
	if ( m_hSocket != NULL )
	{
		closesocket(m_hSocket);
	}
	WSACleanup();
	ExitProcess(0);
}


void CZzhPassDlg::OnCheck() 
{
	// TODO: Add your control notification handler code here
	static BOOL bShow = TRUE;
	RECT rct;
	GetWindowRect(&rct);
	bShow = !bShow;
	
	if	(bShow)
		rct.bottom = rct.bottom + 185;
	else
		rct.bottom = rct.bottom - 185;
	
	MoveWindow(&rct, TRUE);
	
	rct.top = rct.bottom - 20;
//	m_wndStatusBar.MoveWindow(&rct, TRUE);
	
	InvalidateRect(NULL);
	UpdateWindow();
}

void CZzhPassDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_inifile.SetString(m_username,"userpass",m_password);
	m_inifile.SetString(m_username,"dns",m_dns);
	m_inifile.SetString(m_username,"build",m_build);
	m_inifile.SetString(m_username,"date",m_date);
	m_inifile.SetString(m_username,"radio",m_gengx);
}
