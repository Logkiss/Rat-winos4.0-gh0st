#if !defined(AFX_SETTINGDLG_H__8F21CBB2_F9C5_4382_8CDF_598D3360D927__INCLUDED_)
#define AFX_SETTINGDLG_H__8F21CBB2_F9C5_4382_8CDF_598D3360D927__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettingDlg dialog

class CSettingDlg : public CDialog
{
// Construction
public:
	CSettingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettingDlg)
	enum { IDD = IDD_DIALOG_SETTING };
	BOOL	m_disble_poptips;
	UINT	m_listen_port;
	BOOL	m_warning_tone;
	UINT	xintiao;
	BOOL	m_line;
	BOOL	m_national;
	BOOL    m_bisDisablePifu;
	CString	m_ConnetPass;
	CComboBox	m_ListColor;
	//}}AFX_DATA
	DWORD ListColor;			//上线列表颜色
	void InitColor(void);		//上线列表颜色
	CString Base64Encode(LPCTSTR lpszSrc);   //加密函数
	CString Base64Decode(LPCTSTR lpszSrc);  //解密函数
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	//}}AFX_VIRTUAL

// Implementation
protected:

	
	// Generated message map functions
	//{{AFX_MSG(CSettingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDISABLEPifu();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
		HICON m_hIcon;
public:
	BOOL	m_dawed_1;
	BOOL	m_dawed_2;
	BOOL	m_dawed_3;
	BOOL	m_dawed_4;
	BOOL	m_dawed_5;
	BOOL	m_dawed_6;
	BOOL	m_dawed_7;
	BOOL	m_dawed_8;
	BOOL	m_dawed_9;
	BOOL	m_dawed_10;
	BOOL	m_dawed_11;
	BOOL	m_dawed_12;
	BOOL	m_dawed_13;
	BOOL	m_dawed_14;
	BOOL	m_dawed_15;
	BOOL	m_dawed_16;
	BOOL	m_dawed_17;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGDLG_H__8F21CBB2_F9C5_4382_8CDF_598D3360D927__INCLUDED_)
