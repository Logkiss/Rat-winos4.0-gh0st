#if !defined(AFX_LOCALUPLOAD_H__7FF95076_E80A_4A26_A3F4_3B504EEC478B__INCLUDED_)
#define AFX_LOCALUPLOAD_H__7FF95076_E80A_4A26_A3F4_3B504EEC478B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LocalUpload.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLocalUpload dialog

class CLocalUpload : public CDialog
{
// Construction
public:
	CLocalUpload(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLocalUpload)
	enum { IDD = IDD_DIALOG_UPLOAD };
	CComboBox	m_combo_type;
	CString	m_edit_path;
	CString	m_edit_cmd_line;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLocalUpload)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    UINT m_type;
	// Generated message map functions
	//{{AFX_MSG(CLocalUpload)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonPath();
	virtual void OnOK();
	afx_msg void OnSelchangeComboType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOCALUPLOAD_H__7FF95076_E80A_4A26_A3F4_3B504EEC478B__INCLUDED_)
