//#if !defined(AFX_FADERWND_H__4663A0FE_1B90_4E99_A5FD_960B3CF66B5C__INCLUDED_)
//#define AFX_FADERWND_H__4663A0FE_1B90_4E99_A5FD_960B3CF66B5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FaderWnd.h : header file
//
#pragma once
#ifndef WS_EX_LAYERED 
#define WS_EX_LAYERED 0x00080000
#define FaderWndTimer 0
#endif
typedef void(WINAPI* lpUpdateLayeredWindow)(HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags);
/////////////////////////////////////////////////////////////////////////////
// CFaderWnd window

class CFaderWnd : public CWnd
{
// Construction
public:
	CFaderWnd(CWnd*pWndToFader,UINT nFaderTime=2000,BYTE ALPHA=255);

// Attributes
public:

// Operations
public:
  
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFaderWnd)
	protected:
	virtual void PostNcDestroy(){delete this;}
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFaderWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFaderWnd)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
CWnd*    m_pWndToFader; //关联窗口指针
BLENDFUNCTION m_Blend;  //混合结构体变量
 lpUpdateLayeredWindow  m_lpfunUpdateLayeredWnd;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//#endif // !defined(AFX_FADERWND_H__4663A0FE_1B90_4E99_A5FD_960B3CF66B5C__INCLUDED_)
