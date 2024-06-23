#if !defined(AFX_FILETRANSFERMODEDLG_H__E29F0DEC_DACE_4A8C_AA2C_6337582736A0__INCLUDED_)
#define AFX_FILETRANSFERMODEDLG_H__E29F0DEC_DACE_4A8C_AA2C_6337582736A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileTransferModeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileTransferModeDlg dialog

class CFileTransferModeDlg : public CDialog
{
// Construction
public:
	CFileTransferModeDlg(CWnd* pParent = NULL);   // standard constructor
	CString m_strFileName;
// Dialog Data
	//{{AFX_DATA(CFileTransferModeDlg)
	enum { IDD = IDD_DIALOG_TRANSFERMODE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileTransferModeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileTransferModeDlg)
	afx_msg	void OnEndDialog(UINT id);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILETRANSFERMODEDLG_H__E29F0DEC_DACE_4A8C_AA2C_6337582736A0__INCLUDED_)
