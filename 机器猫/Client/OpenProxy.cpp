// OpenProxy.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "OpenProxy.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenProxy dialog


COpenProxy::COpenProxy(CWnd* pParent /*=NULL*/)
	: CDialog(COpenProxy::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpenProxy)
	m_check_verify = FALSE;
	m_edit_port = 1080;
	m_edit_user = _T("");
	m_edit_pass = _T("");
	//}}AFX_DATA_INIT
}


void COpenProxy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenProxy)
	DDX_Check(pDX, IDC_CHECK_VERIFY, m_check_verify);
	DDX_Text(pDX, IDC_EDIT_PORT, m_edit_port);
	DDV_MinMaxUInt(pDX, m_edit_port, 1, 65535);
	DDX_Text(pDX, IDC_EDIT_USER, m_edit_user);
	DDX_Text(pDX, IDC_EDIT_PASS, m_edit_pass);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenProxy, CDialog)
	//{{AFX_MSG_MAP(COpenProxy)
	ON_BN_CLICKED(IDC_CHECK_VERIFY, OnCheckVerify)
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(BN_CLICKED, IDOK, IDNO, OnEndDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenProxy message handlers


void COpenProxy::OnCheckVerify() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	GetDlgItem(IDC_EDIT_USER)->EnableWindow(m_check_verify);
	GetDlgItem(IDC_EDIT_PASS)->EnableWindow(m_check_verify);
}


void COpenProxy::OnEndDialog(UINT id)
{
	// TODO: Add your control notification handler code here
	EndDialog(id);
}
