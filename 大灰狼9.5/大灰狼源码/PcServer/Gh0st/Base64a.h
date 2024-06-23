#if !defined(AFX_BASE64A_H__2187948A_1D51_4A77_9EB8_B4A804CAA2B2__INCLUDED_)
#define AFX_BASE64A_H__2187948A_1D51_4A77_9EB8_B4A804CAA2B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// base64a.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Base64a window

class Base64a
{
// Construction
public:
	Base64a();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Base64a)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~Base64a();

	// Generated message map functions
protected:
	//{{AFX_MSG(Base64a)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	CString Base64Encode(LPCTSTR lpszSrc);   //加密函数
	CString Base64Decode(LPCTSTR lpszSrc);  //解密函数

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASE64A_H__2187948A_1D51_4A77_9EB8_B4A804CAA2B2__INCLUDED_)
