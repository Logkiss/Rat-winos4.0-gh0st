#pragma once
//#include "c:\program files (x86)\codejock software\mfc\xtreme toolkitpro v18.5.0\source\chart\xtpchartcontrol.h"
#include "afxcmn.h"
#include "afxwin.h"



// CInfoDlg 窗体视图

class CInfoDlg : public CFormView
{
	DECLARE_DYNCREATE(CInfoDlg)

public:
	CInfoDlg();           // 动态创建所使用的受保护的构造函数
	virtual ~CInfoDlg();

public:
	enum { IDD = IDD_DLG_INFO };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	CXTPChartControl m_ChartControl;
	void CreateChart();
	CBrush  m_brush;
	CXTPButton m_btnpm;
	CXTPButton m_btnmsg;
	CXTPButton m_btnfile;
	CXTPButton m_btnshut;
	CListCtrl m_list_info;
	void AddPoint();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtnPm();
	afx_msg void OnBnClickedBtnMsg();
	afx_msg void OnBnClickedBtnFile();
	afx_msg void OnBnClickedBtnShut();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnShowMessage(WPARAM wParam,LPARAM lParam); // 自定义消息
protected:
	HANDLE hThread;
	static DWORD WINAPI GetFlowPro( LPVOID p); 
};


