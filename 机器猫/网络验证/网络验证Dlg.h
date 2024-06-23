// ÍøÂçÑéÖ¤Dlg.h : header file
//

#if !defined(AFX_DLG_H__927DC5B1_E405_483E_B364_80942EB5E859__INCLUDED_)
#define AFX_DLG_H__927DC5B1_E405_483E_B364_80942EB5E859__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMyDlg dialog
#include "IniFile.h"
class CMyDlg : public CDialog
{
// Construction
public:
	CMyDlg(CWnd* pParent = NULL);	// standard constructor
	CIniFile m_inifile;
// Dialog Data
	//{{AFX_DATA(CMyDlg)
	enum { IDD = KPass };
	int		m_port;
	CString	m_username;
	CString	m_password;
	CString	m_dns;
	CString	m_date;
	CString	m_build;
	CString	m_gengx;
	CString strTime;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	afx_msg void OnExit();
	afx_msg void OnCheck();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_H__927DC5B1_E405_483E_B364_80942EB5E859__INCLUDED_)
