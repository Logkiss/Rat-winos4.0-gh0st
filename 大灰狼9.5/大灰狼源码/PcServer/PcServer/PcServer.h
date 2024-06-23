// PcServer.h : main header file for the PCSERVER application
//

#if !defined(AFX_PCSERVER_H__3B7FD5B4_54D5_4B68_85E8_674B93A2281D__INCLUDED_)
#define AFX_PCSERVER_H__3B7FD5B4_54D5_4B68_85E8_674B93A2281D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CPcServerApp:
// See PcServer.cpp for the implementation of this class
//

class CPcServerApp : public CWinApp
{
public:
	CPcServerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPcServerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CPcServerApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCSERVER_H__3B7FD5B4_54D5_4B68_85E8_674B93A2281D__INCLUDED_)
