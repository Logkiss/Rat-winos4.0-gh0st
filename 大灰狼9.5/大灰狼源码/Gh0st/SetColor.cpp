// SetColor.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "SetColor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetColor dialog


CSetColor::CSetColor(CWnd* pParent /*=NULL*/)
	: CDialog(CSetColor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetColor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSetColor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetColor)
	DDX_Control(pDX, IDC_BSend, m_BsendXtp);
	DDX_Control(pDX, IDC_BCANCEL, m_Canxtp);
	DDX_Control(pDX, IDC_COMBO_Color, m_ComBox_Color);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetColor, CDialog)
	//{{AFX_MSG_MAP(CSetColor)
	ON_BN_CLICKED(IDC_BSend, OnBSend)
	ON_BN_CLICKED(IDC_BCANCEL, OnBcancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetColor message handlers

void CSetColor::OnBSend() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	nSecletIndex = m_ComBox_Color.GetCurSel();

	CDialog::OnOK();
}

void CSetColor::OnBcancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

BOOL CSetColor::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
			return true;
		if (pMsg->wParam == VK_RETURN)
		{
			OnBSend();
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CSetColor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  	m_Canxtp.SetTheme(xtpButtonThemeOfficeXP);
    m_BsendXtp.SetTheme(xtpButtonThemeOfficeXP);
    m_ComBox_Color.SetTheme(xtpControlThemeOfficeXP);
	// TODO: Add extra initialization here
	m_ComBox_Color.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
