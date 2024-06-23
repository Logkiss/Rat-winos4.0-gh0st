// PcServerView.h : interface of the CPcServerView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCSERVERVIEW_H__0D456C06_2F6F_4E5F_B49C_80A009D4E17C__INCLUDED_)
#define AFX_PCSERVERVIEW_H__0D456C06_2F6F_4E5F_B49C_80A009D4E17C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPcServerView : public CView
{
protected: // create from serialization only
	CPcServerView();
	DECLARE_DYNCREATE(CPcServerView)

// Attributes
public:
	CPcServerDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPcServerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPcServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPcServerView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in PcServerView.cpp
inline CPcServerDoc* CPcServerView::GetDocument()
   { return (CPcServerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCSERVERVIEW_H__0D456C06_2F6F_4E5F_B49C_80A009D4E17C__INCLUDED_)
