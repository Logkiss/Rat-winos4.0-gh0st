// Client.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Client.h"
#include "MainFrm.h"
#include "InfoDlg.h"
#include "ClientDoc.h"
#include "ClientView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientApp

BEGIN_MESSAGE_MAP(CClientApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CClientApp::OnAppAbout)
END_MESSAGE_MAP()

// void dbg_dump(struct _EXCEPTION_POINTERS* ExceptionInfo) {
// 	TCHAR buff[1024];
// 	memset(buff, 0, sizeof(buff));
// 	wsprintf
// 		(buff, 
// 		_T("CRASH CODE:0x%.8x ADDR=0x%.8x FLAGS=0x%.8x PARAMS=0x%.8x\n")
// 		_T("eax=%.8x ebx=%.8x ecx=%.8x\nedx=%.8x esi=%.8x edi=%.8x\neip=%.8x esp=%.8x ebp=%.8x\n"),
// 		ExceptionInfo->ExceptionRecord->ExceptionCode,
// 		ExceptionInfo->ExceptionRecord->ExceptionAddress,
// 		ExceptionInfo->ExceptionRecord->ExceptionFlags,
// 		ExceptionInfo->ExceptionRecord->NumberParameters,
// 		ExceptionInfo->ContextRecord->Eax,
// 		ExceptionInfo->ContextRecord->Ebx,
// 		ExceptionInfo->ContextRecord->Ecx,
// 		ExceptionInfo->ContextRecord->Edx,
// 		ExceptionInfo->ContextRecord->Esi,
// 		ExceptionInfo->ContextRecord->Edi,
// 		ExceptionInfo->ContextRecord->Eip,
// 		ExceptionInfo->ContextRecord->Esp,
// 		ExceptionInfo->ContextRecord->Ebp
// 		);
// 
// 	MessageBox(NULL, buff, _T("LiteX程序出错！"), MB_OK);
// }

// LONG WINAPI bad_exception(struct _EXCEPTION_POINTERS* ExceptionInfo) {
// 	dbg_dump(ExceptionInfo);
// 	// 不退出
// 	return true;
// 	/*ExitProcess(0);*/
// }
// CClientApp construction

CClientApp::CClientApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	// 加载风格控件
	TCHAR szAppPath[_MAX_PATH];
	VERIFY(::GetModuleFileName(m_hInstance, szAppPath, _MAX_PATH));	

	CString strAppPath(szAppPath);
	int nIndex  = strAppPath.ReverseFind(_T('\\'));
	if (nIndex > 0)
	{
		strAppPath = strAppPath.Left(nIndex);
	}
	else
	{
		strAppPath.Empty();
	}

	CString strStylesPath;
	strStylesPath += strAppPath + _T("\\Styles\\");

	m_hModule2010.LoadLibrary(strStylesPath + _T("Office2010.dll"));
	m_hModule2013.LoadLibrary(strStylesPath + _T("Office2013.dll"));
	m_hModule2016.LoadLibrary(strStylesPath + _T("Office2016.dll"));
	m_hModuleWin7.LoadLibrary(strStylesPath + _T("Windows7.dll"));

	m_theme = themeOffice2013Word;

	// 加载系统图标,后面在Filemanager要用到
	typedef BOOL (WINAPI * pfn_FileIconInit) (BOOL fFullInit);
	HMODULE hModule_shell32 = LoadLibrary("shell32.dll");
	if(hModule_shell32)
	{
		pfn_FileIconInit FileIconInit = (pfn_FileIconInit) GetProcAddress(hModule_shell32, (LPCSTR)660);
		FileIconInit(TRUE);
	}
	else
	{
		AfxMessageBox("LoadLibray shell32 error!");
	}

	// 读取公司名称
	m_CompanyName = ((CClientApp *)AfxGetApp())->m_IniFile.GetString(_T("Setting"), _T("CompanyName"), _T("V-Eye终端安全管理软件"));
}


// The one and only CClientApp object

CClientApp theApp;


// CClientApp initialization
BOOL CClientApp::InitInstance()
{
//	SetUnhandledExceptionFilter(bad_exception);



	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CClientDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CInfoDlg));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// 启动IOCP服务器
	nPort = m_IniFile.GetInt(_T("Settings"), _T("ListenPort"));
	if (nPort == 0)	nPort = 2019;

	((CMainFrame*) m_pMainWnd)->Activate(nPort);
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CClientApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

