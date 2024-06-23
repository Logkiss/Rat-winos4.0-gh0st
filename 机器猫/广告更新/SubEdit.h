#if !defined(AFX_SUBEDIT_H__56C17DCC_C8EA_11D3_9E0B_00A0C9395AB0__INCLUDED_)
#define AFX_SUBEDIT_H__56C17DCC_C8EA_11D3_9E0B_00A0C9395AB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SubEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SubEdit window

class SubEdit : public CEdit
{
// Construction
public:
	SubEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SubEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SubEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(SubEdit)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

    CFont    m_font;
	COLORREF m_Color;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBEDIT_H__56C17DCC_C8EA_11D3_9E0B_00A0C9395AB0__INCLUDED_)
