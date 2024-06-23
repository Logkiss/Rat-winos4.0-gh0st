// ZcDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ZcDlg.h"
#include "afxdialogex.h"


// CZcDlg 对话框

IMPLEMENT_DYNAMIC(CZcDlg, CDialogEx)

CZcDlg::CZcDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CZcDlg::IDD, pParent)
{

}

CZcDlg::~CZcDlg()
{
}

void CZcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CZcDlg, CDialogEx)
END_MESSAGE_MAP()


// CZcDlg 消息处理程序
