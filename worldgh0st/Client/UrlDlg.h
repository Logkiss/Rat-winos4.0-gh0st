#pragma once
#include "afxcmn.h"


// CUrlDlg 对话框

class CUrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CUrlDlg)

public:
	CUrlDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor
	void OnReceiveComplete();


// 对话框数据
	enum { IDD = IDD_DLG_URLHISTORY };
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void PostNcDestroy();
protected:

	HICON m_hIcon;
	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;

	DECLARE_MESSAGE_MAP()
	BOOL m_bOnClose;
	void ShowUrlList();
	void GetUrlList();
public:
	CHeaderCtrl   m_header;
	CListCtrl     m_list_url;
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
