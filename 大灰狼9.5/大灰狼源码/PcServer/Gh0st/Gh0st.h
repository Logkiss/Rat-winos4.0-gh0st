// Gh0st.h : main header file for the GH0ST application
//

#if !defined(AFX_GH0ST_H__3B33D150_C1FD_45C6_A2C7_C03DEC66EC1D__INCLUDED_)
#define AFX_GH0ST_H__3B33D150_C1FD_45C6_A2C7_C03DEC66EC1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "IniFile.h"
#include "PictureEx.h"
/////////////////////////////////////////////////////////////////////////////
// CGh0stApp:
// See Gh0st.cpp for the implementation of this class
//

class CGh0stApp : public CWinApp
{
public:
	CGh0stApp();

	CIniFile	m_IniFile;
	bool m_bIsQQwryExist;
	bool m_bIsDisablePopTips;
	bool m_bIsWarning_Tone;
	bool m_bIsList_Draw;
bool m_national;
	CView*		m_pConnectView; // 主连接视图

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGh0stApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CGh0stApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GH0ST_H__3B33D150_C1FD_45C6_A2C7_C03DEC66EC1D__INCLUDED_)
