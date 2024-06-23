// MainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "MainDlg.h"


// CMainDlg 对话框

IMPLEMENT_DYNAMIC(CMainDlg, CDialog)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
{

}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CMainDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMainDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMainDlg 消息处理程序


void CMainDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	SendMessage(WM_CLOSE,0,0);

}

void CMainDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}
