#if !defined(AFX_STARTUPDLG_H__7C81C012_B68D_4637_A6C9_7D94DF503ACF__INCLUDED_)
#define AFX_STARTUPDLG_H__7C81C012_B68D_4637_A6C9_7D94DF503ACF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StartupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStartupDlg dialog

class CStartupDlg : public CDialog
{
	// Construction
public:
	CStartupDlg(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CStartupDlg)
	enum { IDD = IDD_LOGIN1 };
	CProgressCtrl	m_Progress;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStartupDlg)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	// Generated message map functions
	//{{AFX_MSG(CStartupDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	WORD m_TimeCount;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STARTUPDLG_H__7C81C012_B68D_4637_A6C9_7D94DF503ACF__INCLUDED_)
