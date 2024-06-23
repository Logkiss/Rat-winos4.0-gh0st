// Start.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "Start.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStart dialog


CStart::CStart(CWnd* pParent /*=NULL*/)
	: CDialog(CStart::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStart)
	m_StartName = ((CClientApp *)AfxGetApp())->m_IniFile.GetString("Start", "StartName", "SVP7"); 
	m_Enlarge = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Start", "Enlarge", 0); 
	
	//}}AFX_DATA_INIT
}


void CStart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStart)
	DDX_Control(pDX, IDC_FILEATTRIBUTE, m_FileAttribute);
	DDX_Text(pDX, IDC_STARTNAME, m_StartName);
	DDX_Text(pDX, IDC_ENLARGE, m_Enlarge);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStart, CDialog)
	//{{AFX_MSG_MAP(CStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStart message handlers

BOOL CStart::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	
	m_FileAttribute.SetCurSel(((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Start","FileAttribute",0));
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CStart::OnOK() 
{
	// TODO: Add extra validation here

	m_FileAttribute.GetLBText(m_FileAttribute.GetCurSel(),m_strCur);
	

	((CClientApp *)AfxGetApp())->m_IniFile.SetString("Start", "StartName", m_StartName);  //安装名称
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Start","FileAttribute",m_FileAttribute.GetCurSel());
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Start","Enlarge",m_Enlarge);     //安装增大

	char ShowText[200];
	wsprintf(ShowText,"%s %s","文件名称:",m_StartName);

	::MessageBox(0, ShowText,"成功写入启动", MB_ICONINFORMATION);//弹出提示框

	CDialog::OnOK();
}
