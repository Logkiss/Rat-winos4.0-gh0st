#if !defined(AFX_START_H__91A4EA60_BB0A_43AD_98CE_838A3F3FAC9F__INCLUDED_)
#define AFX_START_H__91A4EA60_BB0A_43AD_98CE_838A3F3FAC9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Start.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStart dialog

class CStart : public CDialog
{
// Construction
public:
	CStart(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStart)
	enum { IDD = IDD_START };
	CComboBox	m_FileAttribute;
	CString	m_StartName;
	DWORD	m_Enlarge;
	//}}AFX_DATA

	CString m_strCur;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStart)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStart)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_START_H__91A4EA60_BB0A_43AD_98CE_838A3F3FAC9F__INCLUDED_)
