#if !defined(AFX_PRANKDLG_H__B3053F16_2504_4B22_BAA8_4B9BD6DB0C95__INCLUDED_)
#define AFX_PRANKDLG_H__B3053F16_2504_4B22_BAA8_4B9BD6DB0C95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrankDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrankDlg dialog

class CPrankDlg : public CDialog
{
// Construction
public:
	CPrankDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPrankDlg)
	enum { IDD = IDD_DIALOG_PRANK };
	CComboBox	m_combo_prank_type;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrankDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrankDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRANKDLG_H__B3053F16_2504_4B22_BAA8_4B9BD6DB0C95__INCLUDED_)
