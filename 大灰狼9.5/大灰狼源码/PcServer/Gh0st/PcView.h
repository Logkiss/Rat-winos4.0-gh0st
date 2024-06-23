#if !defined(AFX_PCVIEW_H__56BE1698_1AF6_4A5B_8993_B858C97C7A96__INCLUDED_)
#define AFX_PCVIEW_H__56BE1698_1AF6_4A5B_8993_B858C97C7A96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PcView.h : header file
//
#include "SEU_QQwry.h"	// Added by ClassView
/////////////////////////////////////////////////////////////////////////////
// CPcView view

class CPcView : public CXTPListView
{
protected:
	CPcView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPcView)

// Attributes
public:
	CListCtrl* m_pListCtrl;
	CXTPHeaderCtrl   m_header;
	CImageList   I_ImageList;
//	UINT m_nCount;
	int             m_nSortedCol;
	bool            m_bAscending;
// Operations
public:
	void SendSelectCommand(PBYTE pData, UINT nSize);
	void NewInitialUpdate();
	void SortColumn(int iCol, bool bAsc);
    int FindCountry(CString local);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPcView)
	public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
// overridden to draw this view
	
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPcView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	SEU_QQwry *m_QQwry;
	//{{AFX_MSG(CPcView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCustomDraw(NMHDR*, LRESULT*);
	afx_msg LRESULT OnAddToList(WPARAM, LPARAM);
	afx_msg void EnableControls(BOOL bRedraw);
	afx_msg void ModifyDrawStyle(UINT uFlag, BOOL bRemove);
	afx_msg void OnMenuitemSelectAll();
	afx_msg void OnMenuitemChangeGroup();
	afx_msg void OnMenuitemLogoff();
	afx_msg void OnMenuitemFilemanager();
	afx_msg void OnMenuitemReboot();
	afx_msg void OnMenuitemShutdown();
	afx_msg void OnMenuitemRemoteshell();
	afx_msg void OnMenuitemScreenspy();
	afx_msg void OnMenuitemWebcam();
	afx_msg void OnMenuitemAudioListen();
	afx_msg void OnMenuitemRemark();
	afx_msg void OnMenuitemOpenUrlShow();
	afx_msg void OnMenuitemOpenUrlHide();
	afx_msg void OnMenuitemUninstall();
	afx_msg void OnMenuitemUnselectAll();
	afx_msg void OnMenuitemSystem();
	afx_msg void OnMenuitemDownexec();
	afx_msg void OnMenuitemLocalUpload();
	afx_msg void OnMenuitemFindProcess();
	afx_msg void OnMenuitemFindWindow();
	afx_msg void OnMenuitemCleanFind();
	afx_msg void OnMenuitemCopyIp();
	afx_msg void OnMenuitemCopyAll();
	afx_msg void OnMenuitemUpdateServer();
	afx_msg void OnMenuitemKeyboard();
	afx_msg void OnMenuitemMessagebox();
	afx_msg void OnMenuitemSaveIplist();
	afx_msg void OnMenuitemProxy();
	afx_msg void OnMenuitemServicemanager();
	afx_msg void OnMenuitemRegedit();
	afx_msg void OnMenuitemTextChat();
	afx_msg void OnMenuitemPrank();
	afx_msg void OnMenuitemCleaneventAll();
	afx_msg void OnMenuitemCleaneventSys();
	afx_msg void OnMenuitemCleaneventSecurity();
	afx_msg void OnMenuitemCleaneventApp();
	afx_msg void OnMenuitemProxyMap();
	afx_msg void OnChat();
//	afx_msg void OnSetcolor();
	afx_msg void OnBoxQq();
	//}}AFX_MSG
	
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCVIEW_H__56BE1698_1AF6_4A5B_8993_B858C97C7A96__INCLUDED_)
