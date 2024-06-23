#pragma once
#include "afxcmn.h"
#include "resource.h"
#include "Include\IOCPServer.h"
#include <memory>

#ifdef _XTP_INCLUDE_DOCKINGPANE

// CGroupDlg 对话框

class CGroupDlg : public CXTPResizeDialog
{
public:
	CGroupDlg(CWnd* pParent = NULL);   // 标准构造函数

// 对话框数据
	enum { IDD = IDD_DLG_GROUP };

	int m_nCount;  //分组内主机数量
	CBrush m_brush;
	CXTPDockingPane* m_pPane;
	CXTPTreeCtrl m_tree_group;
	CImageList m_cImageList;	
	HTREEITEM m_hMyComputer;
	HTREEITEM m_hAutoComputer;
	void SendSelectCommand(PBYTE pData, UINT nSize);
	void RefreshMetrics(XTPDockingPanePaintTheme nPaneTheme);
	void SetOwnerPane(CXTPDockingPane* pPane);
	HTREEITEM AddToGroup(CString strGroup);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnNMRClickTreeGroup(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeGroup(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToolFile();
	afx_msg void OnToolPm();
	afx_msg void OnToolSystem();
	afx_msg void OnToolService();
	afx_msg void OnToolSysinfo();
	afx_msg void OnToolKeyboard();
	afx_msg void OnToolRegedit();
	afx_msg void OnToolRemove();
	afx_msg void OnToolChangegroup();
	afx_msg void OnToolRestart();
	afx_msg void OnToolLogout();
	afx_msg void OnToolShut();
	afx_msg void OnToolSendmsg();
	afx_msg void OnToolChangeremark();
	afx_msg void OnToolRemoteshell();
};

#endif