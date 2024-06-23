#if !defined(AFX_UPDATEDLG_H__4B1865D2_D586_4E97_B1AE_E41DED8DA2D3__INCLUDED_)
#define AFX_UPDATEDLG_H__4B1865D2_D586_4E97_B1AE_E41DED8DA2D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UpdateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUpdateDlg dialog

class CUpdateDlg : public CDialog
{
// Construction
public:
	int ReplaceStr(char* sSrc, char* sMatchStr, char* sReplaceStr);
	virtual int memfind(const char *mem, const char *str, int sizem, int sizes);
	virtual char* substr(char   *str,int   istar,int   iend);
	CUpdateDlg(CWnd* pParent = NULL);   // standard constructor
	virtual CString Base64Encode(LPCTSTR lpszSrc);   //加密函数
	virtual CString Base64Decode(LPCTSTR lpszSrc);   //解密函数
	void rc4_init(unsigned char *s, unsigned char *key, unsigned long Len);
	void rc4_crypt(unsigned char *s, unsigned char *Data, unsigned long Len);
	virtual CString GetIPAddress();    //外网IP获取
// Dialog Data
	//{{AFX_DATA(CUpdateDlg)
	enum { IDD = IDD_UPDATE_DLG };
	CButton	m_getwan;
	CXTPHyperLink	m_SignNOIP;
	CXTPHyperLink	m_SignNOIP2;
	CXTPHyperLink	m_SignNOIP3;
	CComboBox	m_Combo_DnsIP;
	CString	m_DnsUser;
	CString	m_DnsPass;
	CString	m_DnsDomain;
	BOOL	m_3322;
	BOOL	m_oicp;	
	BOOL	m_noip;	
	CString	m_EDIT_DnsIP;
	CString	m_EDIT_DnsIPjiami;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

protected:
	// Generated message map functions
	//{{AFX_MSG(CUpdateDlg)
	afx_msg void OnBtnDnsupdate();
	virtual BOOL OnInitDialog();
	afx_msg void OnGetwan();
	afx_msg void On3322();
	afx_msg void OnOicp();
	afx_msg void OnNoip();
	afx_msg void OnDnsipjiami();
	afx_msg void OnDnsipsjfz();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATEDLG_H__4B1865D2_D586_4E97_B1AE_E41DED8DA2D3__INCLUDED_)
