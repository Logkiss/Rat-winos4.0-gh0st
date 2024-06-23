// InfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "InfoDlg.h"
#include "MainFrm.h"
#include <atlbase.h>
#include "GroupDlg.h"
#include  <Iphlpapi.h> 
#pragma  comment(lib,"Iphlpapi.lib") 

extern CGroupDlg	*g_pGroupDlg;

#define WM_SHOW_MSG_IN (WM_USER+1111)

// CInfoDlg

typedef struct
{
	char	*title;
	int		nWidth;
}COLUMNSTRUCT;
COLUMNSTRUCT g_Column_Data[] = 
{
	{_T("序号"),	        	50	},//0
	{_T("WAN"),		        	100	},//1
	{_T("LAN"),		        	100	},//2
	{_T("计算机名/备注"),       100	},//3
	{_T("系统版本"),			80	},//4
	{_T("CPU"),		        	70  },//5
	{_T("内存"),	        	50  },//6
	{_T("登录用户"),	        50  } //7
};

int g_Column_Width = 0;
int	g_Column_Count = (sizeof(g_Column_Data) / 8);

CInfoDlg	*g_pInfoDlg;

IMPLEMENT_DYNCREATE(CInfoDlg, CFormView)

CInfoDlg::CInfoDlg()
	: CFormView(CInfoDlg::IDD)
{
	g_pInfoDlg=this;
}

CInfoDlg::~CInfoDlg()
{
}

void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHARTCONTRO, m_ChartControl);
	DDX_Control(pDX, IDC_BTN_PM, m_btnpm);
	DDX_Control(pDX, IDC_BTN_MSG, m_btnmsg);
	DDX_Control(pDX, IDC_BTN_FILE, m_btnfile);
	DDX_Control(pDX, IDC_BTN_SHUT, m_btnshut);
	DDX_Control(pDX, IDC_LIST_INFO, m_list_info);
}

BEGIN_MESSAGE_MAP(CInfoDlg, CFormView)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_PM, &CInfoDlg::OnBnClickedBtnPm)
	ON_BN_CLICKED(IDC_BTN_MSG, &CInfoDlg::OnBnClickedBtnMsg)
	ON_BN_CLICKED(IDC_BTN_FILE, &CInfoDlg::OnBnClickedBtnFile)
	ON_BN_CLICKED(IDC_BTN_SHUT, &CInfoDlg::OnBnClickedBtnShut)
	ON_MESSAGE(WM_SHOW_MSG_IN,&OnShowMessage)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CInfoDlg 诊断

#ifdef _DEBUG
void CInfoDlg::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CInfoDlg::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CInfoDlg 消息处理程序

BOOL CInfoDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类
	cs.style &= ~WS_BORDER;
	return CFormView::PreCreateWindow(cs);
}

// 是否为64位系统
BOOL IsWow64() 
{ 
	HINSTANCE kernel32 = LoadLibrary(_T("kernel32.dll")); 

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL); 
	LPFN_ISWOW64PROCESS fnIsWow64Process; 
	BOOL bIsWow64 = FALSE; 
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( kernel32,"IsWow64Process"); 

	typedef HANDLE (WINAPI *TGetCurrentProcess)(VOID); 
	TGetCurrentProcess myGetCurrentProcess = (TGetCurrentProcess)GetProcAddress(kernel32, "GetCurrentProcess"); 

	if (NULL != fnIsWow64Process) 
	{ 
		fnIsWow64Process(myGetCurrentProcess(),&bIsWow64);
	}

	if(kernel32)
		FreeLibrary(kernel32);
	return bIsWow64; 
}



CString strIn,strOut;
DWORD WINAPI GetFlowProc(LPVOID p)
{
	CInfoDlg* pFrame = (CInfoDlg*) p;	

	PMIB_IFTABLE m_pTable   =   NULL; 
	DWORD m_dwAdapters   =   0; 
	ULONG uRetCode   =   GetIfTable(m_pTable,   &m_dwAdapters,   TRUE); 
	if   (uRetCode   ==   ERROR_NOT_SUPPORTED) 
	{ 
		return   (-1); 
	} 

	if   (uRetCode   ==   ERROR_INSUFFICIENT_BUFFER) 
	{ 
		m_pTable   =   (PMIB_IFTABLE)new   BYTE[65535];//MIB_IFTABLE[m_dwAdapters]; 
	} 

	DWORD dwLastIn   =   0; 
	DWORD dwLastOut   =   0; 
	DWORD dwBandIn   =   0; 
	DWORD dwBandOut   =   0; 

	do   
	{ 
		GetIfTable(m_pTable,   &m_dwAdapters,   TRUE); 
		DWORD dwInOctets   =   0; 
		DWORD dwOutOctets   =   0; 
		for   (UINT   i   =   0;   i   <   m_pTable-> dwNumEntries;   i++) 
		{ 
			MIB_IFROW Row   =   m_pTable-> table[i]; 
			dwInOctets   +=   Row.dwInOctets; 
			dwOutOctets   +=   Row.dwOutOctets; 
		} 

		dwBandIn   =   dwInOctets   -   dwLastIn; 
		dwBandOut   =   dwOutOctets   -   dwLastOut; 
		if   (dwLastIn   <=   0) 
		{ 
			dwBandIn   =   0; 
		} 
		else 
		{ 
			dwBandIn   =   dwBandIn   /   1024; 
		} 

		if   (dwLastOut   <=   0) 
		{ 
			dwBandOut   =   0; 
		} 
		else 
		{ 
			dwBandOut   =   dwBandOut   /   1024; 
		} 

		dwLastIn   =   dwInOctets; 
		dwLastOut   =   dwOutOctets; 

		strIn.Format(_T("下载:%uKB/s"),dwBandIn);
		strOut.Format(_T("上传:%uKB/s"),dwBandOut);

		HWND hWnd = pFrame->GetSafeHwnd();
		::PostMessage(hWnd,WM_SHOW_MSG_IN, 0, 0); 

// 		printf( "收到字节:   %u   bytes\n ",   dwLastIn); 
// 		printf( "发送字节:   %u   bytes\n ",   dwLastOut); 
// 		printf( "下行速度:   %u   KB\n ",   dwBandIn); 
// 		printf( "上行速度:   %u   KB\n ",   dwBandOut); 
		Sleep(3000); 
	}while   (TRUE); 

	delete []  m_pTable; 

	return 0;
}

// 显示消息
LRESULT CInfoDlg::OnShowMessage(WPARAM wParam,LPARAM lParam)
{
	SetDlgItemText(IDC_STATIC_SEND,strIn);
	SetDlgItemText(IDC_STATIC_RECV,strOut);

	return TRUE;
}

void CInfoDlg::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	m_brush.CreateSolidBrush(RGB(255,255,255));

	// 暂时用list来储存下数据
	m_list_info.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	for (int i = 0; i < g_Column_Count; i++)
	{
		m_list_info.InsertColumn(i, g_Column_Data[i].title);
		m_list_info.SetColumnWidth(i, g_Column_Data[i].nWidth);
		g_Column_Width += g_Column_Data[i].nWidth; // 总宽度
	}


	//设置字体
	CFont m_font1,m_font2,m_font3,m_font4;
	m_font1.CreatePointFont(160,_T("微软雅黑"));
	GetDlgItem(IDC_STATIC_HOST)->SetFont(&m_font1);
	GetDlgItem(IDC_STATIC_NET)->SetFont(&m_font1);
	m_font1.Detach();

	m_font2.CreatePointFont(100,_T("微软雅黑"));
	GetDlgItem(IDC_STATIC_NAME)->SetFont(&m_font2);
	GetDlgItem(IDC_STATIC_NAME2)->SetFont(&m_font2);
	GetDlgItem(IDC_STATIC_CPU)->SetFont(&m_font2);
	GetDlgItem(IDC_STATIC_CPU2)->SetFont(&m_font2);
	GetDlgItem(IDC_STATIC_MEM)->SetFont(&m_font2);
	GetDlgItem(IDC_STATIC_MEM2)->SetFont(&m_font2);
	GetDlgItem(IDC_STATIC_OS)->SetFont(&m_font2);
	GetDlgItem(IDC_STATIC_OS2)->SetFont(&m_font2);
	GetDlgItem(IDC_STATIC_USER)->SetFont(&m_font2);
	GetDlgItem(IDC_STATIC_USER2)->SetFont(&m_font2);
	GetDlgItem(IDC_STATIC_IP)->SetFont(&m_font2);
	GetDlgItem(IDC_STATIC_IP2)->SetFont(&m_font2);
	m_font2.Detach();

	m_font4.CreatePointFont(80,_T("微软雅黑"));
	GetDlgItem(IDC_STATIC_RECV)->SetFont(&m_font4);
	GetDlgItem(IDC_STATIC_SEND)->SetFont(&m_font4);
	m_font4.Detach();

// 设置图标按钮
	m_btnpm.EnableMarkup();
	m_btnpm.SetBitmap(0, IDB_PNG_PM);
	m_btnpm.SetFlatStyle(TRUE);
	m_btnpm.SetUseVisualStyle(TRUE);	

	m_btnmsg.EnableMarkup();
	m_btnmsg.SetBitmap(0, IDB_PNG_MSG);
	m_btnmsg.SetFlatStyle(TRUE);
	m_btnmsg.SetUseVisualStyle(TRUE);	

	m_btnfile.EnableMarkup();
	m_btnfile.SetBitmap(0, IDB_PNG_FILE);
	m_btnfile.SetFlatStyle(TRUE);
	m_btnfile.SetUseVisualStyle(TRUE);	

	m_btnshut.EnableMarkup();
	m_btnshut.SetBitmap(0, IDB_PNG_SHUT);
	m_btnshut.SetFlatStyle(TRUE);
	m_btnshut.SetUseVisualStyle(TRUE);

	// 初始化主机信息
	TCHAR ComputerName[100]={0};
	DWORD dwSize=100;
	GetComputerName(ComputerName,&dwSize);
	SetDlgItemText(IDC_STATIC_NAME2,ComputerName);
	SetDlgItemText(IDC_STATIC_HOST,ComputerName);

	//获取CPU信息
	DWORD dwSize2 = 100;
	LONG  ReturnValue;
	HKEY  hKey;
	DWORD type;
	TCHAR  strBuf[100];

	ReturnValue = RegOpenKeyEx (HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"),0,KEY_ALL_ACCESS, &hKey);
	if(ReturnValue != ERROR_SUCCESS) return;

	LONG lReturn = RegQueryValueEx (hKey, _T("ProcessorNameString"), NULL,&type, (BYTE *)strBuf, &dwSize2);
	if(lReturn != ERROR_SUCCESS) return;

	RegCloseKey (hKey);
	hKey=NULL;	

	CString strValue;
	strValue.Format(_T("%s"),strBuf);
	strValue.TrimLeft();
	strValue.TrimRight();
	SetDlgItemText(IDC_STATIC_CPU2,strBuf);

	//内存信息
	MEMORYSTATUSEX	MemInfo; //用GlobalMemoryStatusEx可显示2G以上内存
	MemInfo.dwLength=sizeof(MemInfo); 
	GlobalMemoryStatusEx(&MemInfo);
	DWORDLONG dwMem = MemInfo.ullTotalPhys/1024/1024;
	DWORD   dwMemSize = (unsigned long)dwMem;
	CString strMem;
	strMem.Format(_T("%.2fGB"),(float)dwMemSize/1024);
	SetDlgItemText(IDC_STATIC_MEM2,strMem);

	//当前登录用户名
	TCHAR Name[100]={0};
	DWORD dwSize1=300;
	GetUserName(Name,&dwSize1);
	SetDlgItemText(IDC_STATIC_USER2,Name);

	//操作系统
	CString strPath=_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");//注册 表子键路径
	CRegKey regkey;
	LONG lResult;
	TCHAR chProductName[150];
	DWORD dwSize3=50; 

	lResult=regkey.Open(HKEY_LOCAL_MACHINE,LPCTSTR(strPath),KEY_ALL_ACCESS); //打开 注册表键 
	if (lResult!=ERROR_SUCCESS)
	{
		return;
	}
	regkey.QueryValue(chProductName,_T("ProductName"),&dwSize3);

	CString strOS;
	strOS.Format(_T("%s"),chProductName);
	if(IsWow64()==TRUE)	
		strOS +=_T("（64位操作系统）");
	else
		strOS +=_T("（32位操作系统）");

	SetDlgItemText(IDC_STATIC_OS2,strOS);
	regkey.Close();

	// 登录IP
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD( 2, 0 );
	CString str;
	if ( WSAStartup( wVersionRequested, &wsaData ) == 0 ) //这个没加取不到IP
	{

		char hostname[256]={0}; 
		gethostname(hostname, sizeof(hostname));
		HOSTENT *host = gethostbyname(hostname);
		if (host != NULL)
		{ 
			for ( int i=0; ; i++ )
			{ 
				str += inet_ntoa(*(IN_ADDR*)host->h_addr_list[i]);
				if ( host->h_addr_list[i] + host->h_length >= host->h_name )
					break;
				str += "/";
			}
		}
		WSACleanup( );
	}
	SetDlgItemText(IDC_STATIC_IP2,str);


	CreateChart();

	SetTimer(1, 3000, NULL);  //开启定时器1

	HANDLE m_pThread = AfxBeginThread((AFX_THREADPROC)GetFlowProc, this); 

}

void CInfoDlg::CreateChart()
{
	CXTPChartContent* pContent = m_ChartControl.GetContent();


	CXTPChartSeriesCollection* pCollection = pContent->GetSeries();
	pCollection->RemoveAll();

	if (pCollection)
	{
		for (int s = 0; s < 1; s++)
		{
			CXTPChartSeries* pSeries = pCollection->Add(new CXTPChartSeries());
			if (pSeries)
			{
				pSeries->SetName(_T("Series"));				

				CXTPChartFastLineSeriesStyle*pStyle = new CXTPChartFastLineSeriesStyle();
				pSeries->SetStyle(pStyle);

				pStyle->SetAntialiasing(TRUE);				

				pSeries->SetArgumentScaleType(xtpChartScaleNumerical);
			}
		}
	}

	// Set the X and Y Axis title for the series.
	CXTPChartDiagram2D* pDiagram = DYNAMIC_DOWNCAST(CXTPChartDiagram2D, pCollection->GetAt(0)->GetDiagram());
	ASSERT (pDiagram);

	pDiagram->SetAllowZoom(TRUE);


	pDiagram->GetAxisY()->GetRange()->SetMaxValue(50.1);
	pDiagram->GetAxisY()->GetRange()->SetAutoRange(FALSE);
	pDiagram->GetAxisY()->SetAllowZoom(FALSE);

	pDiagram->GetAxisX()->GetRange()->SetMaxValue(100.1);
	pDiagram->GetAxisX()->GetRange()->SetAutoRange(FALSE);
	pDiagram->GetAxisX()->GetRange()->SetZoomLimit(10);

	pDiagram->GetAxisX()->SetInterlaced(FALSE);
	pDiagram->GetAxisY()->SetInterlaced(FALSE);	

	pDiagram->GetPane()->GetFillStyle()->SetFillMode(xtpChartFillSolid);

}
void CInfoDlg::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
// 	CXTPWindowRect rcChart(this);
// 	ScreenToClient(&rcChart);
// 	if (m_ChartControl.GetSafeHwnd())
// 	{
// 	m_ChartControl.SetWindowPos(NULL, rcChart.left, rcChart.top, rcChart.Width(), rcChart.Height(), SWP_NOZORDER);
// 	}

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
HBRUSH CInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	switch(nCtlColor)  
	{  
	case CTLCOLOR_STATIC: //对所有静态文本控件的设置  
		{  
			//设置背景为透明  
			pDC->SetBkMode(TRANSPARENT); 
			if(pWnd->GetDlgCtrlID() != IDC_STATIC_HOST&&pWnd->GetDlgCtrlID() != IDC_STATIC_NET
				&&pWnd->GetDlgCtrlID() != IDC_STATIC_RECV&&pWnd->GetDlgCtrlID() != IDC_STATIC_SEND)
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
			pDC->SetBkMode(TRANSPARENT);   
			pDC->SetTextColor(RGB(120,120,120)); //设置字体颜色 
			hbr=(HBRUSH)m_brush; 
			break;  
		}  
	default:  
		break;  
	}  
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CInfoDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CFormView::OnPaint()
	CRect   rect;  
	GetClientRect(rect);  
	dc.FillSolidRect(rect,RGB(255, 255, 255)); //设置为白色背景
}

void CInfoDlg::OnBnClickedBtnPm()
{
	// TODO: 在此添加控件通知处理程序代码
	g_pGroupDlg->OnToolPm();
}

void CInfoDlg::OnBnClickedBtnMsg()
{
	// TODO: 在此添加控件通知处理程序代码
	g_pGroupDlg->OnToolSendmsg();
}

void CInfoDlg::OnBnClickedBtnFile()
{
	// TODO: 在此添加控件通知处理程序代码
	g_pGroupDlg->OnToolFile();
}

void CInfoDlg::OnBnClickedBtnShut()
{
	// TODO: 在此添加控件通知处理程序代码
	g_pGroupDlg->OnToolShut();
}

void CInfoDlg::AddPoint()
{
	CXTPChartContent* pContent = m_ChartControl.GetContent();

	CXTPChartSeriesCollection* pCollection = pContent->GetSeries();

	int nCount = 0;

	if (pCollection)
	{
		for (int s = 0; s < pCollection->GetCount(); s++)
		{
			CXTPChartSeries* pSeries = pCollection->GetAt(s);
			if (pSeries)
			{
				int nValue = 50;

				nCount = pSeries->GetPoints()->GetCount();

				if (nCount)
					nValue = (int)pSeries->GetPoints()->GetAt(nCount - 1)->GetValue(0);

				nValue = nValue + (rand() % 20) - 10;

				if (nValue < 0) nValue = 0;
				if (nValue > 100) nValue = 100;

				pSeries->GetPoints()->Add(new CXTPChartSeriesPoint(nCount, nValue));

			}
		}
	}

	CXTPChartDiagram2D* pDiagram = DYNAMIC_DOWNCAST(CXTPChartDiagram2D, 
		m_ChartControl.GetContent()->GetPanels()->GetAt(0));
	ASSERT (pDiagram);


	if (nCount > 100)
	{
		CXTPChartAxisRange* pRange = pDiagram->GetAxisX()->GetRange();

		BOOL bAutoScroll = pRange->GetViewMaxValue() == pRange->GetMaxValue();

		pRange->SetMaxValue(nCount);

		if (bAutoScroll)
		{
			double delta = pRange->GetViewMaxValue() - pRange->GetViewMinValue();

			pRange->SetViewAutoRange(FALSE);
			pRange->SetViewMaxValue(nCount);
			pRange->SetViewMinValue(nCount - delta);
		}

	}

}



void CInfoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nIDEvent == 1)
	{
		AddPoint();
	}
}

void CInfoDlg::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	KillTimer(1);
	KillTimer(2);
}
