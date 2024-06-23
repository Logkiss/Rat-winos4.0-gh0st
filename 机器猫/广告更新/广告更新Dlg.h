// 广告更新Dlg.h : header file
//

#if !defined(AFX_DLG_H__C262D882_0562_4798_B3E9_A7296B968942__INCLUDED_)
#define AFX_DLG_H__C262D882_0562_4798_B3E9_A7296B968942__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMyDlg dialog
class CMyDlg : public CDialog
{
// Construction
public:
	CMyDlg(CWnd* pParent = NULL);	// standard constructor

	DWORD dLogin;
// Dialog Data
	//{{AFX_DATA(CMyDlg)
	enum { IDD = IDD_DOWN_DIALOG };
	CProgressCtrl	m_Prog;
	CEdit	m_edit_log;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnStart1();
	afx_msg void OnBtnStart2();
	afx_msg void OnBtnStart3();
	afx_msg void OnBtnStart4();
	afx_msg void OnBtnStop();
	afx_msg void LogOut(LPCSTR outtext);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
//	ClientContext* m_pContext;
	UINT m_nCurSel;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_H__C262D882_0562_4798_B3E9_A7296B968942__INCLUDED_)
