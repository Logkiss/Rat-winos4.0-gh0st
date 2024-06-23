#if !defined(AFX_SERVICEDLG_H__A67B9CCF_B564_402D_A6FC_10876C98AF05__INCLUDED_)
#define AFX_SERVICEDLG_H__A67B9CCF_B564_402D_A6FC_10876C98AF05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServiceDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CServiceDlg dialog

class CServiceDlg : public CDialog
{
// Construction
public:
	CServiceDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor
	void SendToken(BYTE bToken);

// Dialog Data
	//{{AFX_DATA(CServiceDlg)
	enum { IDD = IDD_DIALOG_SERVICE };
	CXTPListCtrl	m_list;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	CXTHeaderCtrl   m_heades;

	void OnReceiveComplete();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServiceDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	CString strMsgShow;
// Implementation
protected:
	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;
	HICON m_hIcon;
	CImageList     m_ImgList;
	
	CXTPStatusBar m_wndStatusBar;
	CXTPComboBox     *m_wndComBox_search;
	CXTPComboBox     *m_wndComBox_type;
	
	UINT nServiceType;
	void AdjustList();
	void ShowServiceList();
	void OpenInfoDlg();

	// Generated message map functions
	//{{AFX_MSG(CServiceDlg)
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboServiceType();
	afx_msg void OnMenuitemRefreshService();
	afx_msg void OnMenuitemDelService();
	afx_msg void OnMenuitemStartService();
	afx_msg void OnMenuitemStopService();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuitemPropertiesService();
	afx_msg void OnMenuitemPauseService();
	afx_msg void OnMenuitemRegainService();
	afx_msg void OnMenuitemRebootService();
	afx_msg void OnShowMessage(WPARAM wParam,LPARAM lParam); // 自定义消息

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVICEDLG_H__A67B9CCF_B564_402D_A6FC_10876C98AF05__INCLUDED_)
