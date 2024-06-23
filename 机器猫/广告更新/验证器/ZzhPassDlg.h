// ZzhPassDlg.h : header file
//

#if !defined(AFX_ZzhPassDLG_H__08EB067A_1E59_4BA4_A05E_4122201CBA83__INCLUDED_)
#define AFX_ZzhPassDLG_H__08EB067A_1E59_4BA4_A05E_4122201CBA83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CZzhPassDlg dialog
#include "IniFile.h"
class CZzhPassDlg : public CDialog
{
// Construction
public:
	CZzhPassDlg(CWnd* pParent = NULL);	// standard constructor
    CIniFile m_inifile;
// Dialog Data
	//{{AFX_DATA(CZzhPassDlg)
	enum { IDD = ZzhPass };
	int		m_port;
	CString	m_username;
	CString	m_password;
	CString	m_dns;
	CString	m_date;
	CString	m_clienport;
	CString	m_build;
	CString	m_gengx;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZzhPassDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CZzhPassDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	afx_msg void OnExit();
	afx_msg void OnCheck();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZzhPassDLG_H__08EB067A_1E59_4BA4_A05E_4122201CBA83__INCLUDED_)
