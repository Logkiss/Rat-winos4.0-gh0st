#if !defined(AFX_SERVICEINFODLG_H__26B58579_BE8F_407C_989D_CA66687F21BC__INCLUDED_)
#define AFX_SERVICEINFODLG_H__26B58579_BE8F_407C_989D_CA66687F21BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServiceInfoDlg.h : header file
//
#include "ServiceDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CServiceInfoDlg dialog
typedef struct 
{
	CString strSerName;
	CString strSerDisPlayname;
	CString strSerDescription;
	CString strFilePath;
	CString strSerRunway;
	CString strSerState;
}SERVICEINFO;

class CServiceInfoDlg : public CDialog
{
// Construction
public:
	CServiceInfoDlg(CWnd* pParent = NULL);   // standard constructor

	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;
// Dialog Data
	//{{AFX_DATA(CServiceInfoDlg)
	enum { IDD = IDD_DIALOG_SERVICE_INFO };
	CComboBox	m_combox_runway;
	//}}AFX_DATA

	SERVICEINFO m_ServiceInfo;
	CServiceDlg *m_ServiceDLg;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServiceInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:
	HICON m_hIcon;
	void SendToken(BYTE bToken);

	// Generated message map functions
	//{{AFX_MSG(CServiceInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboRunway();
	afx_msg void OnButtonStart();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonPause();
	afx_msg void OnButtonContinue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVICEINFODLG_H__26B58579_BE8F_407C_989D_CA66687F21BC__INCLUDED_)
