// Gh0stView.h : interface of the CGh0stView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GH0STVIEW_H__7C4A27DF_BBAF_4BB0_9FB9_F53C9933BD9D__INCLUDED_)
#define AFX_GH0STVIEW_H__7C4A27DF_BBAF_4BB0_9FB9_F53C9933BD9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Gh0stDoc.h"

class CGh0stView : public CXTPListView
{
protected: // create from serialization only
	CGh0stView();
	DECLARE_DYNCREATE(CGh0stView)

// Attributes
public:
	CGh0stDoc* GetDocument();
	void UpdateDocTitle();
	BOOL AddView(CRuntimeClass* pViewClass, LPCTSTR lpszTitle);
	BOOL AddGroup( LPCTSTR lpszTitle );
	BOOL UpDateNumber();

// Operations
public:
	CXTPTabControl m_wndTabControl;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGh0stView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGh0stView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGh0stView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMenuitemGroupAdd();
	afx_msg void OnMenuitemGroupDel();
	afx_msg void OnMenuitemChange();
	afx_msg void OnMenuitemSelectNum();
	//}}AFX_MSG
	afx_msg void OnSelectedChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnAddFindGroup(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in Gh0stView.cpp
inline CGh0stDoc* CGh0stView::GetDocument()
   { return (CGh0stDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GH0STVIEW_H__7C4A27DF_BBAF_4BB0_9FB9_F53C9933BD9D__INCLUDED_)
