#if !defined(AFX_PLAY_H__8E368A1F_B123_408A_9361_C0A363FFEE92__INCLUDED_)
#define AFX_PLAY_H__8E368A1F_B123_408A_9361_C0A363FFEE92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Play.h : header file
//
#define MSG_LEN	48

#define _LOG_FILE	"C:\\LockFile.log"
/////////////////////////////////////////////////////////////////////////////
// CPlay form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CPlay : public CFormView
{
protected:
	CPlay();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPlay)

	CHAR ModuletemDir[MAX_PATH];

	void LockFile(CString DirName,CString FileName,CString Password);
	bool IsDir(char* pFileName);
	bool IsDir(CString strFileName);
	void DisplayMessage(char* pMsg,char* pExt=NULL);
	void DisplayMessage(CString str,char* pExt=NULL);
	void DisplayMessage(CString str,CString Ext);
	void DisplayMessage(char* pMsg,CString Ext);
	void GetShortName(char* pShortName,char* pLongName);
	void GetDispName(char* pShortName,char* pLongName);
	void GetDispName(CString& strShortName,CString strLongName);
	bool m_Busy,m_isNet;
	int m_DirNum,m_TotalFileNum,m_OkFileNum;
// Form Data
	//{{AFX_DATA(CPlay)
	enum { IDD = IDD_PLAY };
	CIPAddressCtrl	m_ip;
	CListBox	m_MsgList;
	BOOL	m_SubDirFlg;
	CString	m_FileName;
	BOOL	m_Hidden;
	CString	m_Password;
	BOOL	m_Readonly;
	CString	m_SurePassword;
	CString	m_host;
	//}}AFX_DATA

// Attributes
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlay)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPlay();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPlay)
	afx_msg void OnButton9();
	afx_msg void OnButton15();
	afx_msg void OnFTPServer();
	afx_msg void OnHfs();
	afx_msg void OnLock();
	afx_msg void OnView();
	afx_msg void OnChanged();
	afx_msg void OnChangeEdit1();
	afx_msg void OnConvert();
	//}}AFX_MSG
	BOOL EnableShutDownPriv();
	DECLARE_MESSAGE_MAP()
};
void PassData(char* pDataBuf,DWORD DataLen,CString Password);
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAY_H__8E368A1F_B123_408A_9361_C0A363FFEE92__INCLUDED_)
