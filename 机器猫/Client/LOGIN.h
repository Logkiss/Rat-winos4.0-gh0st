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
// Dialog Data
	//{{AFX_DATA(LOGIN)
	enum { IDD = IDD_LOGIN };
	CString	m_username;
	CString	m_userpass;
	CString	m_onlinepass;
	BOOL	m_jzmm;
	BOOL    m_bisDisablePifu;

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
	afx_msg void OnLogin();
	virtual BOOL OnInitDialog();
	afx_msg void OnDISABLEPifu();
	afx_msg void OnLXKF();
	afx_msg void OnGFWZ();
	afx_msg void OnSYJC();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	//位图进度条
	CPictureEx m_GifPic;

};
/////////网络信息获取////////////
#define Request_DOWN			0x9
typedef struct
{
	BYTE	Flags;
	DWORD	Buffer_Size;
	BYTE	Buffer[1024];
	BYTE	Info[256];
}NET_DATA, *LPNET_DATA;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGIN_H__3286D204_C760_48C5_ACC0_5C1D67E505AA__INCLUDED_)
