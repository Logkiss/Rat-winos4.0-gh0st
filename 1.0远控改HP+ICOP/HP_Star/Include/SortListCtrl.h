#if !defined(AFX_SORTLISTCTRL_H__C1111AF4_B95F_4E2D_9D87_26D3EF5A34E6__INCLUDED_)
#define AFX_SORTLISTCTRL_H__C1111AF4_B95F_4E2D_9D87_26D3EF5A34E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SortListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSortListCtrl window

class CSortListCtrl : public CListCtrl
{
// Construction
public:
	CSortListCtrl();
	BOOL m_fAsc;//是否顺序排序
	int m_nSortedCol;//当前排序的列

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSortListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSortListCtrl)
	//手工定义消息映射，处理顶部按钮被按下的消息
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	//手工定义消息映射，处理顶部按钮被按下的消息

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SORTLISTCTRL_H__C1111AF4_B95F_4E2D_9D87_26D3EF5A34E6__INCLUDED_)
