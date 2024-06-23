#include "afxwin.h"
#if !defined(AFX_UPDATEIP_H__89D145DD_E4BB_4515_89F4_5508299872A1__INCLUDED_)
#define AFX_UPDATEIP_H__89D145DD_E4BB_4515_89F4_5508299872A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UPDATEIP.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUPDATEIP form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CUPDATEIP : public CFormView
{
//protected:
//	CUPDATEIP();           // protected constructor used by dynamic creation
//	DECLARE_DYNCREATE(CUPDATEIP)

// Form Data
public:
	CUPDATEIP();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CUPDATEIP)

	//{{AFX_DATA(CUPDATEIP)
	enum { IDD = IDD_UPDATEIP };
	CComboBox	m_Combo_DnsIP;
	CComboBox	m_Combo_FtpIP;
	CString	m_DnsUser;
	CString	m_DnsPass;
	CString	m_DnsDomain;
	CString	m_DnsUser1;
	CString	m_DnsPass1;
	CString	m_DnsDomain1;
	CString	m_Combo_DnsIP2;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	virtual CString Base64Encode(LPCTSTR lpszSrc);   //加密函数
	virtual CString Base64Decode(LPCTSTR lpszSrc);   //解密函数
	virtual CString GetIPAddress();    //外网IP获取
	virtual VOID MyEncryptFunctionForServer(LPSTR szData,WORD Size);
	virtual int ReplaceStr(char* sSrc, char* sMatchStr, char* sReplaceStr);
	virtual int memfind(const char *mem, const char *str, int sizem, int sizes);
	virtual char* substr(char   *str,int   istar,int   iend);

	void rc4_init(unsigned char *s, unsigned char *key, unsigned long Len);
	void rc4_crypt(unsigned char *s, unsigned char *Data, unsigned long Len);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUPDATEIP)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnBtnDnsupdate();
	//}}AFX_VIRTUAL

// Implementation
protected:
//	virtual ~CUPDATEIP();

	CIniFile m_Ini;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;

#endif

	// Generated message map functions
	//{{AFX_MSG(CUPDATEIP)
	afx_msg void OnEditchangeComboFtpip();
	afx_msg void OnBtnFtpupdate();
	afx_msg void OnB_IP();
	afx_msg void OnFTPServer();
	afx_msg void OnFTPServer2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CButton m_Button2;
	CButton m_Bin_Dnsuodate;
	CButton m_Btn_Ftpupdate;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATEIP_H__89D145DD_E4BB_4515_89F4_5508299872A1__INCLUDED_)
