#if !defined(AFX_MYCHAT_H__6AA5739D_2C35_4B30_B032_B394C602B85E__INCLUDED_)
#define AFX_MYCHAT_H__6AA5739D_2C35_4B30_B032_B394C602B85E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyChat.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyChat dialog

class CMyChat : public CDialog
{
// Construction
public:
	void OnReceiveComplete();
	CMyChat(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CMyChat)
	enum { IDD = IDD_CHAT };
	CEdit	m_newtext;
	CEdit	m_chat;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyChat)
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
	//{{AFX_MSG(CMyChat)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSend();
	afx_msg void OnButtonEnd();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYCHAT_H__6AA5739D_2C35_4B30_B032_B394C602B85E__INCLUDED_)
