#if !defined(AFX_PROXYMAPDLG_H__807A97FE_989D_41FF_A9E1_723D80504637__INCLUDED_)
#define AFX_PROXYMAPDLG_H__807A97FE_989D_41FF_A9E1_723D80504637__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProxyMapDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProxyMapDlg dialog

class CProxyMapDlg : public CDialog
{
// Construction
public:
	CProxyMapDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProxyMapDlg)
	enum { IDD = IDD_DIALOG_PROXY_MAP };
	CEdit	m_edit;
	//}}AFX_DATA

	static void CALLBACK NotifyProc( ClientContext *pContext, UINT nCode);
	ClientContext * pContexts[10000];
	void OnReceiveComplete();
	void AddLog(TCHAR * lpText);
	LRESULT OnNotifyProc(WPARAM wParam, LPARAM lParam);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProxyMapDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProxyMapDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;
	CIOCPLOCAL* m_iocpLocal;
	CString m_IPAddress;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROXYMAPDLG_H__807A97FE_989D_41FF_A9E1_723D80504637__INCLUDED_)
