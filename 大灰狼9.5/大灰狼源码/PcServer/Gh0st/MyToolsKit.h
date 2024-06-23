#if !defined(AFX_MYTOOLSKIT_H__629E0060_A724_4B03_80D8_E87293440312__INCLUDED_)
#define AFX_MYTOOLSKIT_H__629E0060_A724_4B03_80D8_E87293440312__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyToolsKit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyToolsKit dialog
#include "Icons.h"

class CMyToolsKit : public CDialog
{
// Construction
public:
	CMyToolsKit(CWnd* pParent = NULL);

	LPICONRESOURCE lpIR;  // ICON×ÊÔ´Ö¸Õë
    EXEDLLICONINFO EDII;
    CString szOpenFileName,szOpenFileExtName,szSaveFileName,szSaveFileExtName;
// Dialog Data
	//{{AFX_DATA(CMyToolsKit)
	enum { IDD = IDD_TOOLS_DIALOG };
	CSliderCtrl	m_Sllider;
	CListBox	m_List;
	CString	m_Path;
	UINT	m_time;
	//}}AFX_DATA


// Overrides
	virtual BOOL ChangeExeIcon(LPSTR lpIconFile, LPSTR lpExeName);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyToolsKit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMyToolsKit)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelecticon();
	afx_msg void OnSelchangeList2();
	afx_msg void OnPaint();
	afx_msg void OnButtonSaveas();
	afx_msg void OnButton1();
	afx_msg void OnButton3();
	afx_msg void OnButton2();
	afx_msg void OnSelectfile();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnButton4();
	afx_msg void OnCheck3();
	afx_msg void OnFilePath();
	afx_msg void OnCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButton5();
	afx_msg void GetRandString(LPSTR szStr,DWORD len);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYTOOLSKIT_H__629E0060_A724_4B03_80D8_E87293440312__INCLUDED_)
