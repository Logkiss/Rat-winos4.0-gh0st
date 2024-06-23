#pragma once


// CZcDlg 对话框

class CZcDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CZcDlg)

public:
	CZcDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CZcDlg();

// 对话框数据
	enum { IDD = IDD_DLG_ZC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
