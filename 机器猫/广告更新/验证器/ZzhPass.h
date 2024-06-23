// ZzhPass.h : main header file for the ZZHPASS application
//

#if !defined(AFX_ZZHPASS_H__2D8C858A_B015_4B6E_9D49_C764616C71BF__INCLUDED_)
#define AFX_ZZHPASS_H__2D8C858A_B015_4B6E_9D49_C764616C71BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CZzhPassApp:
// See ZzhPass.cpp for the implementation of this class
//

class CZzhPassApp : public CWinApp
{
public:
	CZzhPassApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZzhPassApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CZzhPassApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZZHPASS_H__2D8C858A_B015_4B6E_9D49_C764616C71BF__INCLUDED_)
