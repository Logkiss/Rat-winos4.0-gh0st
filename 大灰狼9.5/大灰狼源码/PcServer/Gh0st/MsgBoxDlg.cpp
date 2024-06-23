// MsgBoxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "MsgBoxDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgBoxDlg dialog


CMsgBoxDlg::CMsgBoxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMsgBoxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMsgBoxDlg)
	m_edit_caption = _T("±êÌâ");
	m_edit_text = _T("ÄÚÈÝ");
	//}}AFX_DATA_INIT
}


void CMsgBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMsgBoxDlg)
	DDX_Control(pDX, IDC_COMBO_BUTTON_TYPE, m_combo_button_type);
	DDX_Control(pDX, IDC_COMBO_MSG_TYPE, m_combo_msg_type);
	DDX_Text(pDX, IDC_EDIT_CAPTION, m_edit_caption);
	DDX_Text(pDX, IDC_EDIT_TEXT, m_edit_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMsgBoxDlg, CDialog)
	//{{AFX_MSG_MAP(CMsgBoxDlg)
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, OnButtonPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgBoxDlg message handlers

BOOL CMsgBoxDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText("MessageBox");

	m_combo_msg_type.SetCurSel(0);
	m_combo_button_type.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CMsgBoxDlg::UpdateDlg()
{
	UpdateData(TRUE);

	switch(m_combo_msg_type.GetCurSel())
	{
	case 0:
		m_msgIcon = MB_ICONINFORMATION;
		break;
	case 1:
		m_msgIcon = MB_ICONQUESTION;
		break;
	case 2:
		m_msgIcon = MB_ICONWARNING;
		break;
	case 3:
		m_msgIcon = MB_ICONERROR;
		break;
	}

	switch(m_combo_button_type.GetCurSel())
	{
	case 0:
		m_msgButton = MB_OK;
		break;
	case 1:
		m_msgButton = MB_HELP;
		break;
	case 2:	
		m_msgButton = MB_YESNO;
		break;
	case 3:	
		m_msgButton = MB_OKCANCEL;
		break;
	case 4:	
		m_msgButton = MB_CANCELTRYCONTINUE;
		break;
	case 5:	
		m_msgButton = MB_YESNOCANCEL;
		break;
	case 6:	
		m_msgButton = MB_ABORTRETRYIGNORE;
		break;
	}
}
void CMsgBoxDlg::OnButtonPreview() 
{
	// TODO: Add your control notification handler code here
	UpdateDlg();
	MessageBox(m_edit_text.GetBuffer(0),m_edit_caption.GetBuffer(0),m_msgIcon|m_msgButton);
}

void CMsgBoxDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateDlg();

	CDialog::OnOK();
}
