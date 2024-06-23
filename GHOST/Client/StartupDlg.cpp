// StartupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "StartupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStartupDlg dialog


CStartupDlg::CStartupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStartupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStartupDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_TimeCount = 1;
}


void CStartupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStartupDlg)
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStartupDlg, CDialog)
	//{{AFX_MSG_MAP(CStartupDlg)
		// NOTE: the ClassWizard will add message map macros here
		ON_WM_TIMER()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStartupDlg message handlers
BOOL CStartupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	m_Progress.SetRange(1,50);
	
	m_Progress.SetPos(1);
	
	SetTimer(0x100,100,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CStartupDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if(pMsg->message == WM_KEYDOWN)
		return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}
void CStartupDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==0x100)
	{
		if(m_TimeCount >= 50)
		{
			KillTimer(nIDEvent);
			PostMessage(WM_CLOSE,NULL,NULL);
		}
		m_Progress.SetPos(++m_TimeCount);
		
		CClientDC cdc(this);
		CString str;
		cdc.SetBkMode(TRANSPARENT);
		//cdc.SetTextColor(RGB(128,138,135));
        cdc.SetTextColor(RGB(0,255,255));
		
		CFont   font;   
		VERIFY(font.CreateFont(16,0,0,0,
			FW_NORMAL,FALSE,FALSE,0,ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,"黑体")); 
		CFont *OldFont=cdc.SelectObject(&font);
		
		CRect rc;
		rc.bottom = 265;
		rc.right  = 500;
		rc.top    = 190;
		rc.left   = 265;
		
		switch(m_TimeCount)
		{
		case 2:
			str=_T("正在加载窗体..请稍后");
			cdc.TextOut(265,190,str);
			break;
		case 7:
			InvalidateRect(rc,TRUE);
			break;
		case 12:
			str=_T("正在从注册表恢复默认值");
			cdc.TextOut(265,190,str);
			break;
		case 17:
			InvalidateRect(rc,TRUE);
			break;
		case 22:
			str=_T("正在检查软件是否更新版本");
			cdc.TextOut(265,190,str);
			break;
		case 27:
			InvalidateRect(rc,TRUE);
			break;
		case 32:
			str=_T("正在启用IOCP端口监听程序");
			cdc.TextOut(265,190,str);
			break;
		case 37:
			InvalidateRect(rc,TRUE);
			break;
		case 42:
			str=_T("程序准备完毕,欢迎使用本软件");
			cdc.TextOut(265,190,str);
			break;
		case 47:
			InvalidateRect(rc,TRUE);
			break;
		default: break;
		}
		
	}
	
	CDialog::OnTimer(nIDEvent);
}


void CStartupDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	//	m_TimeCount++;
	
	
	
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
}
