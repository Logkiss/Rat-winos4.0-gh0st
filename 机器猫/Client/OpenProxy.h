#if !defined(AFX_OPENPROXY_H__D48F67B8_D225_4562_922F_755404A9987E__INCLUDED_)
#define AFX_OPENPROXY_H__D48F67B8_D225_4562_922F_755404A9987E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenProxy.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COpenProxy dialog

class COpenProxy : public CDialog
{
// Construction
public:
	COpenProxy(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COpenProxy)
	enum { IDD = IDD_DIALOG_PROXY };
	BOOL	m_check_verify;
	UINT	m_edit_port;
	CString	m_edit_user;
	CString	m_edit_pass;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenProxy)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COpenProxy)
	afx_msg void OnCheckVerify();
	afx_msg	void OnEndDialog(UINT id);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENPROXY_H__D48F67B8_D225_4562_922F_755404A9987E__INCLUDED_)
