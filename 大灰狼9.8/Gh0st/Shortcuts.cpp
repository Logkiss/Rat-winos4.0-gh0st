// Shortcuts.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "Shortcuts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShortcuts

IMPLEMENT_DYNCREATE(CShortcuts, CFormView)

CShortcuts::CShortcuts()
	: CFormView(CShortcuts::IDD)
{
	//{{AFX_DATA_INIT(CShortcuts)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CShortcuts::~CShortcuts()
{
}

void CShortcuts::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShortcuts)
	DDX_Control(pDX, IDC_TASKMGR, m_taskmgr);
	DDX_Control(pDX, IDC_SERVICES, m_services);
	DDX_Control(pDX, IDC_REGEDIT, m_regedit);
	DDX_Control(pDX, IDC_NOTEPAD, m_notepad);
	DDX_Control(pDX, IDC_MSTSC, m_mstsc);
	DDX_Control(pDX, IDC_MSPAINT, m_mspaint);
	DDX_Control(pDX, IDC_IEXPRESS, m_iexpress);
	DDX_Control(pDX, IDC_GPEDIT, m_gpedit);
	DDX_Control(pDX, IDC_DIRECTX, m_directx);
	DDX_Control(pDX, IDC_COMPMGMT, m_compmgmt);
	DDX_Control(pDX, IDC_CMD, m_cmd);
	DDX_Control(pDX, IDC_CLAC, m_clac);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShortcuts, CFormView)
	//{{AFX_MSG_MAP(CShortcuts)
	ON_BN_CLICKED(IDC_CLAC, OnClac)
	ON_BN_CLICKED(IDC_CMD, OnCmd)
	ON_BN_CLICKED(IDC_COMPMGMT, OnCompmgmt)
	ON_BN_CLICKED(IDC_DIRECTX, OnDirectx)
	ON_BN_CLICKED(IDC_GPEDIT, OnGpedit)
	ON_BN_CLICKED(IDC_IEXPRESS, OnIexpress)
	ON_BN_CLICKED(IDC_MSPAINT, OnMspaint)
	ON_BN_CLICKED(IDC_MSTSC, OnMstsc)
	ON_BN_CLICKED(IDC_NOTEPAD, OnNotepad)
	ON_BN_CLICKED(IDC_REGEDIT, OnRegedit)
	ON_BN_CLICKED(IDC_SERVICES, OnServices)
	ON_BN_CLICKED(IDC_TASKMGR, OnTaskmgr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShortcuts diagnostics

#ifdef _DEBUG
void CShortcuts::AssertValid() const
{
	CFormView::AssertValid();
}

void CShortcuts::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CShortcuts message handlers

void CShortcuts::OnClac() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","calc.exe",NULL,NULL,SW_SHOW);
}

void CShortcuts::OnCmd() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","cmd.exe",NULL,NULL,SW_SHOW);
}

void CShortcuts::OnCompmgmt() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","compmgmt.msc",NULL,NULL,SW_SHOW);
}

void CShortcuts::OnDirectx() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","dxdiag.exe",NULL,NULL,SW_SHOW);
}

void CShortcuts::OnGpedit() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","Gpedit.msc",NULL,NULL,SW_SHOW);
}

void CShortcuts::OnIexpress() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","Iexpress.exe",NULL,NULL,SW_SHOW);
}

void CShortcuts::OnMspaint() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","Mspaint.exe",NULL,NULL,SW_SHOW);
}

void CShortcuts::OnMstsc() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","Mstsc.exe",NULL,NULL,SW_SHOW);
}

void CShortcuts::OnNotepad() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","notepad.exe",NULL,NULL,SW_SHOW);
}

void CShortcuts::OnRegedit() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","regedit.exe",NULL,NULL,SW_SHOW);
}

void CShortcuts::OnServices() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","services.msc",NULL,NULL,SW_SHOW);
}

void CShortcuts::OnTaskmgr() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","taskmgr.exe",NULL,NULL,SW_SHOW);
}

void CShortcuts::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
}
