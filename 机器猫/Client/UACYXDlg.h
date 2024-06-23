// UACYXDlg.h : header file
//

#if !defined(AFX_UACYXDLG_H__62B887F2_53B4_417C_8FB0_3AC1ECE253AE__INCLUDED_)
#define AFX_UACYXDLG_H__62B887F2_53B4_417C_8FB0_3AC1ECE253AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CUACYXDlg dialog

class CUACYXDlg : public CDialog
{
// Construction
public:
	CUACYXDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CUACYXDlg)
	enum { IDD = IDD_UACYX_DIALOG };
	CString	m_edit_path1;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUACYXDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CUACYXDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonPaths();
	virtual void OnOK();
	afx_msg void UAC();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UACYXDLG_H__62B887F2_53B4_417C_8FB0_3AC1ECE253AE__INCLUDED_)
