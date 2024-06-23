#if !defined(AFX_SIGNATURE_H__0E222394_69A4_4235_85CF_3C5E5063F221__INCLUDED_)
#define AFX_SIGNATURE_H__0E222394_69A4_4235_85CF_3C5E5063F221__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Signature.h : header file
//
#include "DropEdit.h"
/////////////////////////////////////////////////////////////////////////////
// CSignature form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CSignature : public CFormView
{
protected:
	CSignature();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSignature)

// Form Data
public:
	//{{AFX_DATA(CSignature)
	enum { IDD = IDD_SIGNATURE };
	CEdit	m_Log;
	CString	m_path;
	//}}AFX_DATA
	int		m_key;
	BOOL	m_bak;

// Attributes
public:
	virtual void OnInitialUpdate();
	bool AddSection1();

// Operations
public:
	void AsmCode(char *lpBuffer,DWORD *codelen);
	bool CheckSec(PIMAGE_SECTION_HEADER sectiontable,PIMAGE_NT_HEADERS peheader,int num);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSignature)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSignature();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CSignature)
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnDropFiles( HDROP hDropInfo );
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void Onxuanwu();
	afx_msg void Onweishi();
	afx_msg void Onkugou();
	afx_msg void Onliulanqi();
	afx_msg void Onxunlei();
	afx_msg void Ontengxun();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CDropEdit m_dropEdit;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIGNATURE_H__0E222394_69A4_4235_85CF_3C5E5063F221__INCLUDED_)
