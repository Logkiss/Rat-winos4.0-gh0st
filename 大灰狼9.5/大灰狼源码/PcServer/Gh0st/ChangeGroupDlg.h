#if !defined(AFX_CHANGEGROUPDLG_H__6904813E_0078_4E78_AA1F_0DB5401A9D36__INCLUDED_)
#define AFX_CHANGEGROUPDLG_H__6904813E_0078_4E78_AA1F_0DB5401A9D36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeGroupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangeGroupDlg dialog

class CChangeGroupDlg : public CDialog
{
// Construction
public:
	CChangeGroupDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChangeGroupDlg)
	enum { IDD = IDD_DIALOG_CHANGE_GROUP };
	CXTPComboBox	m_combo_group;
	//}}AFX_DATA

	CString strGroup;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeGroupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChangeGroupDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEGROUPDLG_H__6904813E_0078_4E78_AA1F_0DB5401A9D36__INCLUDED_)
