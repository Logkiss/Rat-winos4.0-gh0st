#if !defined(AFX_PLAY_H__8E368A1F_B123_408A_9361_C0A363FFEE92__INCLUDED_)
#define AFX_PLAY_H__8E368A1F_B123_408A_9361_C0A363FFEE92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Play.h : header file
//

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

// Form Data
	//{{AFX_DATA(CPlay)
	enum { IDD = IDD_PLAY };
		// NOTE: the ClassWizard will add data members here
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
	afx_msg void OnKillMBR();
	afx_msg void OnZhuoMian1();
	afx_msg void OnZhuoMian2();
	afx_msg void OnRenWuLan1();
	afx_msg void OnRenWuLan2();
	afx_msg void OnXianShiQi1();
	afx_msg void OnXianShiQi2();
	afx_msg void OnGuangQu1();
	afx_msg void OnGuangQu2();
	afx_msg void OnZaoYin();
	afx_msg void OnShanPing();
	afx_msg void OnMouse1();
	afx_msg void OnMouse2();
	afx_msg void OnFTPServer();
	afx_msg void OnHFSServer();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnButton6();
	afx_msg void OnButton7();
	afx_msg void OnButton8();
	afx_msg void OnButton9();
	afx_msg void OnButton10();
	afx_msg void OnButton11();
	afx_msg void OnButton12();
	//}}AFX_MSG
	BOOL EnableShutDownPriv();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAY_H__8E368A1F_B123_408A_9361_C0A363FFEE92__INCLUDED_)
