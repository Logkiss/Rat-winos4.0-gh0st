// BuildDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "BuildDlg.h"
#include "MainFrm.h"

extern CMainFrame	*g_pFrame;

// CBuildDlg

IMPLEMENT_DYNCREATE(CBuildDlg, CFormView)

CBuildDlg::CBuildDlg()
	: CFormView(CBuildDlg::IDD)
{

}

CBuildDlg::~CBuildDlg()
{
}

void CBuildDlg::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_BUILD, m_btnbuild);
	DDX_Control(pDX, IDC_RADIO1, m_ck1);
	DDX_Control(pDX, IDC_RADIO2, m_ck2);
}

BEGIN_MESSAGE_MAP(CBuildDlg, CFormView)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_BUILD, &CBuildDlg::OnBnClickedBtnBuild)
END_MESSAGE_MAP()


// CBuildDlg 诊断

#ifdef _DEBUG
void CBuildDlg::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CBuildDlg::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CBuildDlg 消息处理程序
BOOL CBuildDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类
    cs.style &= ~WS_BORDER;
	return CFormView::PreCreateWindow(cs);
}

void CBuildDlg::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	m_brush.CreateSolidBrush(RGB(255,255,255));

	//设置字体
	CFont m_font1,m_font2;
	m_font1.CreatePointFont(110,_T("微软雅黑"));
	GetDlgItem(IDC_STATIC_BUILD)->SetFont(&m_font1);
	m_font1.Detach();

	m_font2.CreatePointFont(90,_T("微软雅黑"));
	GetDlgItem(IDC_STATIC_BUILD2)->SetFont(&m_font2);
	m_font2.Detach();

	// 设置图标按钮
	m_btnbuild.EnableMarkup();
	m_btnbuild.SetBitmap(0, IDB_PNG_BUILD);
//	m_btnbuild.SetFlatStyle(TRUE);
	m_btnbuild.SetUseVisualStyle(TRUE);	

	m_ck1.SetFlatStyle(TRUE);;	
	m_ck2.SetFlatStyle(TRUE);
	m_ck1.SetUseVisualStyle(TRUE);
	m_ck2.SetUseVisualStyle(TRUE);

	CButton* radio=(CButton*)GetDlgItem(IDC_RADIO1);
	radio->SetCheck(1);
}

void CBuildDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CFormView::OnPaint()

	CRect   rect;  
	GetClientRect(rect);  
	dc.FillSolidRect(rect,RGB(255, 255, 255)); //设置为白色背景
}


/*
CTLCOLOR_BTN 按钮控件
CTLCOLOR_DLG 对话框
CTLCOLOR_EDIT 编辑框
CTLCOLOR_LISTBOX 列表框
CTLCOLOR_MSGBOX 消息框
CTLCOLOR_SCROLLBAR 滚动条
CTLCOLOR_STATIC 静态文本 
*/
HBRUSH CBuildDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	switch(nCtlColor)  
	{  
	case CTLCOLOR_STATIC: //对所有静态文本控件的设置  
		{  
			//设置背景为透明  
			pDC->SetBkMode(TRANSPARENT); 
			if(pWnd->GetDlgCtrlID() == IDC_STATIC_BUILD)
                pDC->SetTextColor(RGB(65,105,225)); //设置字体颜色 
			else
				pDC->SetTextColor(RGB(110,110,110)); //设置字体颜色 
			hbr=(HBRUSH)m_brush; 
			break;  
		}  
	case CTLCOLOR_DLG:  
		{  
			return (HBRUSH) m_brush .GetSafeHandle();   
			break;  
		}  
	case CTLCOLOR_BTN:  
		{  
			//设置背景为透明  
		//	pDC->SetBkMode(TRANSPARENT);   
		//	pDC->SetTextColor(RGB(120,120,120)); //设置字体颜色 
			hbr=(HBRUSH)m_brush; 
			break;  
		}  
	default:  
		break;  
	}  
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

/*
BUILD_INFO m_BuildInfo = 
{
	_T("127.0.0.1"),    // 上线地址
	2019,               // 上线端口
	FALSE               // 是否静默安装，默认显示安装
};

int MemFindStr(const char *strMem, const char *strSub, int iSizeMem, int isizeSub)   
{   
	int   da,i,j;   
	if (isizeSub == 0) 
		da = strlen(strSub);   
	else 
		da = isizeSub;   
	for (i = 0; i < iSizeMem; i++)   
	{   
		for (j = 0; j < da; j ++)   
			if (strMem[i+j] != strSub[j])	
				break;   
		if (j == da) 
			return i;   
	}  

	return -1;   
}


BOOL CBuildDlg::CreateServer(BUILD_INFO *pBuildInfo,LPSTR szPath)
{

	HANDLE m_Handle =CreateFile(szPath,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_WRITE|FILE_SHARE_READ,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(m_Handle!=INVALID_HANDLE_VALUE)
	{	
		DWORD Size = GetFileSize(m_Handle,NULL);
		char * pBuffer = new char[Size];
		if(pBuffer == NULL)
		{
			delete pBuffer;
			CloseHandle(m_Handle);
			return FALSE;
		}
		DWORD YtesRead=0;
		DWORD iCount=0;
		do
		{
			ReadFile(m_Handle,&pBuffer[iCount],Size-iCount,&YtesRead,NULL);
			iCount+=YtesRead;
		}while(iCount<Size);

		DWORD SizePoint = MemFindStr(pBuffer,_T("127.0.0.1"),Size,0);
		if(SizePoint != 0)
		{
			SetFilePointer(m_Handle,SizePoint,0,FILE_BEGIN);
			DWORD Written=0;
			WriteFile(m_Handle,pBuildInfo,sizeof(BUILD_INFO),&Written,NULL);
		}
		if (pBuffer)
			delete pBuffer;

		CloseHandle(m_Handle);
	}

	return TRUE;
}*/


// 获取本机外网IP
CString GetInternetIP()
{
	//查询网站
	CString checkWeb=_T("http://www.ip138.com/ips1388.asp");
	CString tagStr=_T("您的IP地址是：["); 
	CString endChar=_T("]");

	HRESULT hr=URLDownloadToFileA(0, _T(checkWeb),_T("ipFile.txt"), 0,NULL);    
	if (hr!=S_OK){
		return "";
	}

	CFile file;
	file.Open(_T("ipFile.txt"),CFile::modeRead);
	if (!file)
	{
		return "";
	}
	ULONGLONG len=file.GetLength();
	char* buf=new char[len+1];
	ZeroMemory(buf,len+1);
	file.Read(buf,len);
	buf[len]=0;
	CString ans=buf;
	CString ip=_T("");

	int ix=ans.Find(tagStr);
	int endn=-1;
	if (ix>-1)
	{
		len=tagStr.GetLength();
		endn=ans.Find(endChar,ix+len);
		if (endn>-1)
		{
			ip=ans.Mid(ix+len,endn-ix-len);
		}
	}
	delete buf;
	file.Close();
	return ip;
}

void CBuildDlg::OnBnClickedBtnBuild()
{
	// TODO: 在此添加控件通知处理程序代码

	char DatPath[MAX_PATH];
	GetModuleFileName( NULL, DatPath, sizeof(DatPath) );
	*strrchr( DatPath, '\\' ) = '\0';
	lstrcat( DatPath, _T("\\Dat\\Install.dat") );

	if ( GetFileAttributes(DatPath) == -1  )
	{
		MessageBox(_T("没有找到dat文件，无法生成"),_T("提示"), MB_ICONERROR);
		return;
	}

	CString strIP;
	strIP.Format(_T("%s"),GetInternetIP());

	if (strIP.GetLength() == 0)
	{
		MessageBox(_T("外网IP获取失败,无法生成"),_T("提示"), MB_ICONWARNING);
		return;
	}

	// 	ZeroMemory(&m_BuildInfo,sizeof(BUILD_INFO));
	// 	m_BuildInfo.LoginPort = 2019;


	TCHAR szFileName[MAX_PATH];

	CButton* radio=(CButton*)GetDlgItem(IDC_RADIO1);

	if(radio->GetCheck())
		wsprintf(szFileName, _T("Install_%s_v1.exe"),strIP);//带界面
	else
		wsprintf(szFileName, _T("Install_%s_v2.exe"),strIP);//静默安装

 	CopyFile(DatPath,szFileName,FALSE);
 //	CreateServer(&m_BuildInfo,szFileName);

	MessageBox(_T("服务端生成完毕,返回主界面"),_T("提示"), MB_OK);
	Sleep(1000);

	g_pFrame->OnFileUser();


}
