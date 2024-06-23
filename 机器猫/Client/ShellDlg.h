#if !defined(AFX_SHELLDLG_H__A55DD4F8_2B26_47BE_A349_9FA8128796D0__INCLUDED_)
#define AFX_SHELLDLG_H__A55DD4F8_2B26_47BE_A349_9FA8128796D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShellDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShellDlg dialog

class CShellDlg : public CDialog
{
// Construction
public:
	void OnReceiveComplete();
	CShellDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShellDlg)
	enum { IDD = IDD_DIALOG_SHELL };
	CEdit	m_edit;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShellDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChangeEdit();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	HICON m_hIcon;
	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;
	UINT m_nCurSel;
	UINT m_nReceiveLength;
	void AddKeyBoardData();
	void ResizeEdit();
	BOOL m_bOnClose;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHELLDLG_H__A55DD4F8_2B26_47BE_A349_9FA8128796D0__INCLUDED_)
