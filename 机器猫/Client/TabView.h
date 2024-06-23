#if !defined(AFX_TABVIEW_H__1B58ECF1_635C_4F25_8CD6_147D0C6A1C6F__INCLUDED_)
#define AFX_TABVIEW_H__1B58ECF1_635C_4F25_8CD6_147D0C6A1C6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabView.h : header file
//
#include "ClientDoc.h"
/////////////////////////////////////////////////////////////////////////////
// CTabView view

class CTabView : public CView
{
protected:
	CTabView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTabView)

// Attributes
public:
	CClientDoc* GetDocument();
// Operations
public:
	CXTPTabControl m_wndTabControl;
	BOOL UpDateNumber();
	void UpdateDocTitle();
	BOOL AddGroup( LPCTSTR lpszTitle );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTabView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTabView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnAddGroup();
	afx_msg void OnDelGroup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelectedChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
private:
	BOOL AddView(CRuntimeClass* pViewClass, LPCTSTR lpszTitle);
	afx_msg LRESULT OnAddFindGroup(WPARAM, LPARAM);
};

#ifndef _DEBUG  // debug version in ClientView.cpp
inline CClientDoc* CTabView::GetDocument()
{ return (CClientDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABVIEW_H__1B58ECF1_635C_4F25_8CD6_147D0C6A1C6F__INCLUDED_)
