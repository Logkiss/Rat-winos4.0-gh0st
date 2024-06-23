// DecryptData.h : main header file for the DECRYPTDATA application
//

#if !defined(AFX_DECRYPTDATA_H__3D85003B_D2D5_4BBB_96E1_2AE98A81EAED__INCLUDED_)
#define AFX_DECRYPTDATA_H__3D85003B_D2D5_4BBB_96E1_2AE98A81EAED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDecryptDataApp:
// See DecryptData.cpp for the implementation of this class
//

class CDecryptDataApp : public CWinApp
{
public:
	CDecryptDataApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDecryptDataApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDecryptDataApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECRYPTDATA_H__3D85003B_D2D5_4BBB_96E1_2AE98A81EAED__INCLUDED_)
