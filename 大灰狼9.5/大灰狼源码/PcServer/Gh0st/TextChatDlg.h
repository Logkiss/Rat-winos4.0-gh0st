#if !defined(AFX_TEXTCHATDLG_H__7E6A3BEE_4B8B_4810_9248_30F20C7E7C18__INCLUDED_)
#define AFX_TEXTCHATDLG_H__7E6A3BEE_4B8B_4810_9248_30F20C7E7C18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextChatDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTextChatDlg dialog

class CTextChatDlg : public CDialog
{
// Construction
public:
	CTextChatDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor
	void OnReceiveComplete();
	CString strName;

// Dialog Data
	//{{AFX_DATA(CTextChatDlg)
	enum { IDD = IDD_DIALOG_TEXTCHAT };
	CEdit	m_edit_new_text;
	CEdit	m_edit_chat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextChatDlg)
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
	// Generated message map functions
	//{{AFX_MSG(CTextChatDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTCHATDLG_H__7E6A3BEE_4B8B_4810_9248_30F20C7E7C18__INCLUDED_)
