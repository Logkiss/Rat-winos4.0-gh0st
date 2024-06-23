// Graphic.h : main header file for the GRAPHIC application
//

#if !defined(AFX_GRAPHIC_H__368B9355_C43F_434E_B438_25420F53AB2A__INCLUDED_)
#define AFX_GRAPHIC_H__368B9355_C43F_434E_B438_25420F53AB2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CGraphicApp:
// See Graphic.cpp for the implementation of this class
//

class CGraphicApp : public CWinApp
{
public:
	CGraphicApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphicApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CGraphicApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHIC_H__368B9355_C43F_434E_B438_25420F53AB2A__INCLUDED_)
