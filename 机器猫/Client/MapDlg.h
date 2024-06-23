#if !defined(AFX_MAPDLG_H__24E8C025_73BA_4C9B_AC7F_E2D6F50E325C__INCLUDED_)
#define AFX_MAPDLG_H__24E8C025_73BA_4C9B_AC7F_E2D6F50E325C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapDlg dialog

#include "HtmlCtrl.h"
class CMyHtmlCtrl : public CHtmlCtrl 
{
};

/////////////////
// Handle "app:ok" link by closing dialog
//
 
class CMapDlg : public CDialog
{
// Construction
public:
	CMapDlg(CWnd* pParent = NULL,ClientContext *pContext = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMapDlg)
	enum { IDD = IDD_MAP_DIALOG };
	CString	m_1;
	CString	m_3;
	//}}AFX_DATA
	ClientContext *m_pContext;
	CString m_IPAddress;
	CString m_x,m_y;
	CString htmlmap();
	CMyHtmlCtrl m_page;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMapDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	HICON m_hIcon;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPDLG_H__24E8C025_73BA_4C9B_AC7F_E2D6F50E325C__INCLUDED_)
