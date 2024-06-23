#if !defined(AFX_LOGIN_H__3286D204_C760_48C5_ACC0_5C1D67E505AA__INCLUDED_)
#define AFX_LOGIN_H__3286D204_C760_48C5_ACC0_5C1D67E505AA__INCLUDED_
#include "PictureEx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LOGIN.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// LOGIN dialog

class LOGIN : public CDialog
{
// Construction
public:
	char* MI(char* mi);
	char* GetJQM();
	LOGIN(CWnd* pParent = NULL);   // standard constructor
	DWORD dLogin;
	CIniFile m_inifile;
	CPictureEx	m_Picture;
// Dialog Data
	//{{AFX_DATA(LOGIN)
	enum { IDD = IDD_LOGIN };
	CString	m_username;
	CString	m_userpass;
	CString	m_onlinepass;
	CString	m_hostname;
	BOOL	m_checkjz;
	BOOL	m_checkxs;
	BOOL	m_checkmf;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(LOGIN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(LOGIN)
    afx_msg void OnPaint();
	afx_msg void OnLogin();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckxs();
	afx_msg void OnCheckjz();
	afx_msg void OnCheckmf();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL iShow;		//表示是否收缩的标志位
	CRect rectSmall;  //收缩时显示最小化
	CRect rectLarge; //扩展时显示最大化
	afx_msg void OnBtnHide();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGIN_H__3286D204_C760_48C5_ACC0_5C1D67E505AA__INCLUDED_)
