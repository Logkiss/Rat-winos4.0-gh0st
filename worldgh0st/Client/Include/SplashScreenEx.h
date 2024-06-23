// SplashScreenEx.cpp : header file
// by John O'Byrne 01/10/2002

#pragma once

#define CSS_FADEIN		0x0001
#define CSS_FADEOUT		0x0002
#define CSS_FADE		CSS_FADEIN | CSS_FADEOUT
#define CSS_SHADOW		0x0004
#define CSS_CENTERSCREEN	0x0008
#define CSS_CENTERAPP		0x0010
#define CSS_HIDEONCLICK		0x0020

#define CSS_TEXT_NORMAL		0x0000
#define CSS_TEXT_BOLD		0x0001
#define CSS_TEXT_ITALIC		0x0002
#define CSS_TEXT_UNDERLINE	0x0004

typedef BOOL (WINAPI* FN_ANIMATE_WINDOW)(HWND,DWORD,DWORD);

// CSplashScreenEx

class CSplashScreenEx : public CWnd
{
	DECLARE_DYNAMIC(CSplashScreenEx)

public:
	CSplashScreenEx();
	virtual ~CSplashScreenEx();

	BOOL Create(CWnd *pWndParent,LPCTSTR szText=NULL,DWORD dwTimeout=2000,DWORD dwStyle=CSS_FADE | CSS_CENTERSCREEN | CSS_SHADOW);
	BOOL SetBitmap(UINT nBitmapID,short red=-1,short green=-1,short blue=-1);
	BOOL SetBitmap(LPCTSTR szFileName,short red=-1,short green=-1,short blue=-1);

	void Show();
	void Hide();

	void SetText(LPCTSTR szText);
	void SetTextFont(LPCTSTR szFont,int nSize,int nStyle);
	void SetTextDefaultFont();
	void SetTextColor(COLORREF crTextColor);
	void SetTextRect(CRect& rcText);
	void SetTextFormat(UINT uTextFormat);
	
protected:	
	FN_ANIMATE_WINDOW m_fnAnimateWindow;
	CWnd *m_pWndParent;
	CBitmap m_bitmap;
	CFont m_myFont;
	HRGN m_hRegion;
	
	DWORD m_dwStyle;
	DWORD m_dwTimeout;
	CString m_strText;
	CRect m_rcText;
	UINT m_uTextFormat;
	COLORREF m_crTextColor;

	int m_nBitmapWidth;
	int m_nBitmapHeight;
	int m_nxPos;
	int m_nyPos;
		
	HRGN CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color);
	void DrawWindow(CDC *pDC);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	LRESULT OnPrintClient(WPARAM wParam, LPARAM lParam);
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
};


