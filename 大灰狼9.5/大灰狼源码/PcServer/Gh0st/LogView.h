#if !defined(AFX_LOGVIEW_H__49B30949_8048_4827_BDCA_DE41D46C3401__INCLUDED_)
#define AFX_LOGVIEW_H__49B30949_8048_4827_BDCA_DE41D46C3401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLogView view

class CLogView : public CListView
{
protected:
	CLogView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CLogView)

// Attributes
public:

// Operations
public:
	void InsertLogItem(LPCTSTR Text,int Mode, int Flag);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CXTHeaderCtrl   m_heades;
	virtual ~CLogView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CLogView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEventDelete();
	afx_msg void OnAllDelete();
	afx_msg void OnEventSave();
	afx_msg void OnEventCopy();
	afx_msg void g_ColumnWidth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CListCtrl* m_pLogList;
	CImageList I_LogList;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGVIEW_H__49B30949_8048_4827_BDCA_DE41D46C3401__INCLUDED_)
