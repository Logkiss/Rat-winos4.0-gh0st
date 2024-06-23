// SelectQQ.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "SelectQQ.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectQQ dialog


CSelectQQ::CSelectQQ(CString strQQNum,CWnd* pParent /*=NULL*/)
	: CDialog(CSelectQQ::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectQQ)
	//}}AFX_DATA_INIT
	if (strQQNum != "-/-")
	{
		m_strQQ = strQQNum;
	}
}


void CSelectQQ::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectQQ)
	DDX_Control(pDX, IDC_COMBO_QQ, m_combo_qq);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectQQ, CDialog)
	//{{AFX_MSG_MAP(CSelectQQ)
	ON_CBN_SELCHANGE(IDC_COMBO_QQ, OnSelchangeComboQq)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectQQ message handlers

void CSelectQQ::OnSelchangeComboQq() 
{
	// TODO: Add your control notification handler code here
	char szText[260];
	GetDlgItem(IDC_COMBO_QQ)->GetWindowText(szText, 260);
	m_strSelect = szText;
}

BOOL CSelectQQ::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString str;
	int i = 0;
	while (AfxExtractSubString(str, m_strQQ, i++, '|'))
	{	
		if (i == 1)
		{
			m_strSelect = str;
		}
		m_combo_qq.AddString(str);
	}
	m_combo_qq.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
