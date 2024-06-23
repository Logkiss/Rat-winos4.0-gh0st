// RegeditTextDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "RegeditTextDlg.h"
#include "RegeditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegeditTextDlg dialog


CRegeditTextDlg::CRegeditTextDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegeditTextDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegeditTextDlg)
	m_strName = _T("");
	m_strValue = _T("");
	//}}AFX_DATA_INIT
	EPath=false;
	EKey=false;
	isOK=false;
    isDWORD=false;
}


void CRegeditTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegeditTextDlg)
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_DATE, m_strValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegeditTextDlg, CDialog)
	//{{AFX_MSG_MAP(CRegeditTextDlg)
	ON_EN_CHANGE(IDC_EDIT_DATE, OnChangeEditDate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegeditTextDlg message handlers

void CRegeditTextDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);

	CString restr="数据不能为空！";
	if(m_strName==""&&!EPath){
		MessageBox(restr);
		isOK=false;
		return;
	}
	if(m_strValue==""&&!EKey)
	{
		MessageBox(restr);
		isOK=false;
		return;
	}
	isOK=true;

	CDialog::OnOK();
}

BOOL CRegeditTextDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(EPath==true)
	{
		EnablePath();
	}
	if(EKey==true)
	{
		EnableKey();
	}
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// 路径变灰
void CRegeditTextDlg::EnablePath()
{
	GetDlgItem(IDC_EDIT_NAME)->EnableWindow(false);
	UpdateData(false);
}

// key变灰
void CRegeditTextDlg::EnableKey()
{
	GetDlgItem(IDC_EDIT_DATE)->EnableWindow(false);
	UpdateData(false);
}

void CRegeditTextDlg::OnChangeEditDate() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if(!isDWORD)
		return;
	UpdateData(true);
	int length=m_strValue.GetLength();
	
	for(int i=0;i<length;i++)
	{
		char ch=m_strValue.GetAt(i);
		if(ch>57||ch<48)
		{
			m_strValue.Delete(i);
			UpdateData(false);
		}
	}
	length=m_strValue.GetLength();
	if(length>9)
	{                               //数据不能超范围
		m_strValue.Delete(length-1);
		UpdateData(false);
	}
}
