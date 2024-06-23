#include "afxwin.h"
#if !defined(AFX_PROXY_H__B7BD8810_F8BF_4B9D_B5E6_F238DF4F7825__INCLUDED_)
#define AFX_PROXY_H__B7BD8810_F8BF_4B9D_B5E6_F238DF4F7825__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Proxy.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProxy form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CProxy : public CFormView
{
protected:
	CProxy();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CProxy)

// Form Data
public:
	//{{AFX_DATA(CProxy)
	enum { IDD = IDD_DAILI };
	CString	m_ftpserwer;
	CString	m_ftpnema;
	CString	m_ftpset;
	CString	m_longip;
	CString	m_longnema;
	CString	m_longpass;
	CString	m_longport;

	CComboBox	m_long_nema;
	CButton m_Ftp_ip;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProxy)
public:
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void ftpnets();
	//}}AFX_VIRTUAL


// Implementation
protected:
	virtual ~CProxy();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CProxy)
	afx_msg void OnFTPPrese();
	afx_msg void OnChangeFtpSet();
	afx_msg void OnChangeFtpNema();
	afx_msg void OnFTPFilecopy();
//	afx_msg void OnChangeLongip();
//	afx_msg void OnChangeLongnema();
//	afx_msg void OnChangeLongpass();
	afx_msg void OnLongnect();
//	afx_msg void OnChangeLongport();
	afx_msg void OnFtpIp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Cproxy form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class Cproxy : public CFormView
{
protected:
	Cproxy();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(Cproxy)

// Form Data
public:
	//{{AFX_DATA(Cproxy)
	enum { IDD = IDD_DAILI };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cproxy)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~Cproxy();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(Cproxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeLongnema();
	afx_msg void OnCbnSelchangeComboboxex1();

};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROXY_H__B7BD8810_F8BF_4B9D_B5E6_F238DF4F7825__INCLUDED_)
