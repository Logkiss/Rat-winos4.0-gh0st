#if !defined(AFX_WEBCAMDLG_H__C03CAC35_55A8_4998_B6BD_73D20EFC108A__INCLUDED_)
#define AFX_WEBCAMDLG_H__C03CAC35_55A8_4998_B6BD_73D20EFC108A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WebCamDlg.h : header file
//
#include <list>
#include <vector>
#include <Vfw.h>
#include "BmpToAvi.h"
#include "XvidDec.h"
/////////////////////////////////////////////////////////////////////////////
// CWebCamDlg dialog
class ResolutionInfo
{
public:
	int m_DeviceIndex;
	int m_iWidth;
	int m_iHeight;
};
class CWebCamDlg : public CDialog
{
// Construction
public:
	CWebCamDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWebCamDlg)
	enum { IDD = IDD_DIALOG_WEBCAM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	void OnReceive();
	void OnReceiveComplete();

	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;
	HICON m_hIcon;
	CString m_IPAddress;
	UINT m_nCount; // 第几帧
	char* m_deviceList;
	int m_nWebcamSelected; // 选择视频设备的id
	int m_nDeviceNums; // 设备数量
	int m_iResNum;
	std::vector<ResolutionInfo> m_resInfo;// 不知道啥意思
	LPBITMAPINFO m_lpbmi;
	MINMAXINFO m_MMI;

	HDRAWDIB	m_hDD;
	HDC			m_hDC;
	LPVOID m_lpScreenDIB;

	CString m_aviFile; // 录像文件名。如果文件名不为空就写入
	CBmpToAvi	m_aviStream;

	int	m_nOldWidth; // OnSize时判断是高或宽发生变化，宽优先

	CXvidDec *	m_dec ; 

	BOOL m_bOnClose;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebCamDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DrawDIB();

	void ResetScreen();
	void InitMMI();
	LRESULT OnGetMiniMaxInfo(WPARAM, LPARAM);
	bool SendResetScreen(int nWidth, int nHeight); // 发送命令改变视频大小

	bool SaveSnapshot();
	void SaveAvi();

	// Generated message map functions
	//{{AFX_MSG(CWebCamDlg)
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBCAMDLG_H__C03CAC35_55A8_4998_B6BD_73D20EFC108A__INCLUDED_)
