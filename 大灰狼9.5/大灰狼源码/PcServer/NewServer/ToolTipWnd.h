#if !defined(AFX_TOOLTIPWND_H__2C52D3E4_2F5B_11D2_8FC9_000000000000__INCLUDED_)
#define AFX_TOOLTIPWND_H__2C52D3E4_2F5B_11D2_8FC9_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ToolTipWnd.h : header file
//
// 作者：易强亮
// Email:kuge3907@sina.com
// 完成日期:2003-3-22
// Copyright (c) 2003.
//本程序可以实现六种样式:
//1.普通矩形样式
//2.圆角矩形样式
//3.椭圆样式
//4.气球样式
//5.吊篮样式
//6.心脏样式
/////////////////////////////////////////////////////////////////////////////
// CToolTipWnd window
struct BTOOLINFO {

	HWND	hwndTool;
	CString strToolText;
	COLORREF clrToolTextClr;
};
#define TTWS_RECT      0x00000001 //普通矩形样式
#define TTWS_ROUNDRECT 0x00000002 //圆角矩形样式
#define TTWS_ELLIPSE   0x00000003 //椭圆样式
#define TTWS_BALLOON   0x00000004 //气球样式
#define TTWS_NACELLE   0x00000005 //吊篮样式
#define TTWS_HEART     0x00000006 //心脏样式
class CToolTipWnd : public CWnd
{

private:
// Construction
	LPCTSTR lpWndCls;
public:
	CToolTipWnd();
	HWND pCurrwnd;//拥有当前ToolTipWnd的窗体的窗口句柄
	// Attributes
public:
	void RelayEvent(LPMSG);//事件延迟
	BOOL Create(CWnd*,DWORD);//窗体创建
	bool m_bStuck;         //是否显示tooltip窗口

    BOOL SetTTWStyle(DWORD nStyle);//设置窗口样式
	DWORD GetCurrentTTWStyle(){return nCurntTTWStyle;}//得到当前样式
	void SetCurrentTTWStyle(DWORD nStyle){nCurntTTWStyle=nStyle;}//设置当前样式

	void AddTool(CWnd *pWnd, CString strText, COLORREF clrTextColor=NULL);
	void SetWidth(int iWidth) { m_iWidth = iWidth; }
	void SetHeight(int iHeight) { m_iHeight = iHeight; }
	void SetBkColor(COLORREF clrRef) { m_clrBkColor = clrRef; }
	void SetFrameColor(COLORREF clrRef) { m_clrFrameColor = clrRef; }
	void SetDefTextColor(COLORREF clrRef) { m_clrTextColor = clrRef; }
	void SetFontHeight(int iHeight) { m_iFontHeight = iHeight; }
	void SetFontName(CString strFontName) { m_strFontName = strFontName; }

    BYTE GetWndAlpha()const{return m_hWndAlpha;}//得到窗口透明度
	void SetWndAlpha(BYTE nWndAlpha){m_hWndAlpha=nWndAlpha;}//设置透明度 
public:
	CRgn rgn;
	CRgn rgnComb;           //合并体区域
	CRgn rgnTri;
	CRect m_RectText;       //文本窗体
	CFont m_fontText;       //要显示的文本字体
	BYTE m_hWndAlpha;       //窗体透明度(<=255)
	CString m_strText;      //要显示的文本
	bool m_bMouseIn;        //鼠标是否在某个控件内
	COLORREF m_clrTextColor;//文本颜色
	COLORREF m_clrBkColor;  //背景颜色
	COLORREF m_clrFrameColor;//边框颜色
	CMapPtrToPtr m_ToolPtr;  //映射对象 
	int m_iWidth;            //窗口宽度
	int m_iHeight;           //窗口高度
	int m_iFontHeight;       //字体高度
	CString m_strFontName;   //字体名称
    static DWORD nCurntTTWStyle; //当前使用的样式,其值只能是定义的那些
	HWND m_hParentWnd;       //父窗口句柄
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolTipWnd)
	//}}AFX_VIRTUAL
	

// Implementation
public:
	
	virtual ~CToolTipWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolTipWnd)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTIPWND_H__2C52D3E4_2F5B_11D2_8FC9_000000000000__INCLUDED_)
