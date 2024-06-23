// DecryptDataDlg.h : header file
//

#if !defined(AFX_DECRYPTDATADLG_H__16731EB6_7152_4AF7_87DF_ED099379C699__INCLUDED_)
#define AFX_DECRYPTDATADLG_H__16731EB6_7152_4AF7_87DF_ED099379C699__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDecryptDataDlg dialog

class CDecryptDataDlg : public CDialog
{
// Construction
public:
	CDecryptDataDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDecryptDataDlg)
	enum { IDD = IDD_DECRYPTDATA_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CEdit	m_edit_log;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDecryptDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void LogOut(LPCSTR outtext);
	// Generated message map functions
	//{{AFX_MSG(CDecryptDataDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOk1();
	afx_msg void OnOk2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECRYPTDATADLG_H__16731EB6_7152_4AF7_87DF_ED099379C699__INCLUDED_)
