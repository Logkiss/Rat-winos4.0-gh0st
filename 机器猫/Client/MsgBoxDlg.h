#if !defined(AFX_MSGBOXDLG_H__42F5227B_8513_4259_B6A0_9420CF1D50E9__INCLUDED_)
#define AFX_MSGBOXDLG_H__42F5227B_8513_4259_B6A0_9420CF1D50E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgBoxDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgBoxDlg dialog

class CMsgBoxDlg : public CDialog
{
// Construction
public:
	CMsgBoxDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMsgBoxDlg)
	enum { IDD = IDD_DIALOG_MESSAGEBOX };
	CComboBox	m_combo_button_type;
	CComboBox	m_combo_msg_type;
	CString	m_edit_caption;
	CString	m_edit_text;
	//}}AFX_DATA

	UINT m_msgIcon,m_msgButton;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgBoxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateDlg();
	// Generated message map functions
	//{{AFX_MSG(CMsgBoxDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonPreview();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGBOXDLG_H__42F5227B_8513_4259_B6A0_9420CF1D50E9__INCLUDED_)
