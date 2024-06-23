// ChangeGroupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "ChangeGroupDlg.h"
#include "Gh0stView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CGh0stView* g_pTabView;
/////////////////////////////////////////////////////////////////////////////
// CChangeGroupDlg dialog


CChangeGroupDlg::CChangeGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangeGroupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeGroupDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CChangeGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeGroupDlg)
	DDX_Control(pDX, IDC_COMBO_GROUP, m_combo_group);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeGroupDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeGroupDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeGroupDlg message handlers

BOOL CChangeGroupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strGroupName, strTemp;
	int nTabs = g_pTabView->m_wndTabControl.GetItemCount();
	for ( int i = 0; i < nTabs; i++ )
	{
		strTemp = g_pTabView->m_wndTabControl.GetItem(i)->GetCaption();
		int n = strTemp.ReverseFind('(');
		if ( n > 0 )
		{
			strGroupName = strTemp.Left(n);
		}
		else
		{
			strGroupName = strTemp;
		}
		m_combo_group.AddString(strGroupName);
	}
	m_combo_group.SetCurSel(0);
	
	m_combo_group.SetTheme(xtpControlThemeOfficeXP);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangeGroupDlg::OnOK() 
{
	// TODO: Add extra validation here
	((CEdit*)GetDlgItem(IDC_COMBO_GROUP))->GetWindowText(strGroup);
	if (strGroup == "" )
	{
		AfxMessageBox("输入不能为空!");
		return;
	}
	CString strTemp, Temp;
	strTemp = g_pTabView->m_wndTabControl.GetSelectedItem()->GetCaption();
	
	int n = strTemp.ReverseFind('(');
	if ( n > 0 )
	{
		Temp = strTemp.Left(n);
	}
	else
	{
		Temp = strTemp;
	}
	if ( strGroup == Temp )
	{
		AfxMessageBox("分组没有改变!");
		return;
	}

	CDialog::OnOK();
}
