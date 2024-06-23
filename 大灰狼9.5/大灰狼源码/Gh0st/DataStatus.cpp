// DataStatus.cpp : implementation file
//
#include "stdafx.h"
#include "DataStatus.h"
#include <afxinet.h>
#include <afxwin.h>
#include "Gh0st.h"
#include <conio.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





#define SystemBasicInformation 0
#define SystemPerformanceInformation 2
#define SystemTimeInformation 3

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

typedef struct
{
	DWORD dwUnknown1;
	ULONG uKeMaximumIncrement;
	ULONG uPageSize;
	ULONG uMmNumberOfPhysicalPages;
	ULONG uMmLowestPhysicalPage;
	ULONG uMmHighestPhysicalPage;
	ULONG uAllocationGranularity;
	PVOID pLowestUserAddress;
	PVOID pMmHighestUserAddress;
	ULONG uKeActiveProcessors;
	BYTE bKeNumberProcessors;
	BYTE bUnknown2;
	WORD wUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct
{
	LARGE_INTEGER liIdleTime;
	DWORD dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
	LARGE_INTEGER liKeBootTime;
	LARGE_INTEGER liKeSystemTime;
	LARGE_INTEGER liExpTimeZoneBias;
	ULONG uCurrentTimeZoneId;
	DWORD dwReserved;
} SYSTEM_TIME_INFORMATION;

typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);

PROCNTQSI NtQuerySystemInformation;

/////////////////////////////////////////////////////////////////////////////
// CDataStatus dialog


CDataStatus::CDataStatus(CWnd* /*pParent /*=NULL*/)

{
	//{{AFX_DATA_INIT(CDataStatus)
	//}}AFX_DATA_INIT
}


void CDataStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataStatus)
	DDX_Control(pDX, IDC_GUNDONGWEB, m_gundonweb);
	DDX_Control(pDX, IDC_GUNDON, m_gundon);
	DDX_Control(pDX, IDC_DAY, m_day);
	DDX_Control(pDX, IDC_NIGHT, m_night);
	DDX_Text(pDX, IDC_Men, m_Men);
		DDX_Text(pDX, IDC_cpuu, m_Cpu);
//	DDX_Control(pDX, IDC_GIFFIRST, m_Picture);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT1, m_edt1);	//LED变量
}


BEGIN_MESSAGE_MAP(CDataStatus, CDialogBar)
//{{AFX_MSG_MAP(CDataStatus)
ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
ON_WM_PAINT()
ON_WM_TIMER()
ON_WM_CTLCOLOR()

// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void TransParentDC(CRect rect,CDC * pDC)
{
	CDC m_MemDC;
	m_MemDC.CreateCompatibleDC(pDC);
	CBitmap m_Bitmap;
	m_Bitmap.CreateCompatibleBitmap
		(pDC,rect.Width(),rect.Height());
	CBitmap *pOldBitmap = 
		m_MemDC.SelectObject(&m_Bitmap);
	m_MemDC.FillSolidRect(0,0,rect.Width(),
		rect.Height(),GetSysColor(COLOR_MENU));
	
	COLORREF cor =  pDC->GetPixel(0,0);
	for(int y = 0; y < rect.Height();y++)
	{
		for(int x = 0; x < rect.Width(); x++)
		{
			COLORREF ch = pDC->GetPixel(x,y);
			if(ch != cor)
				m_MemDC.SetPixelV(x,y,ch);
		}
	}
	pDC->BitBlt(0,0,rect.Width(),
		rect.Height(),&m_MemDC,0,0,SRCCOPY);
	m_MemDC.SelectObject(pOldBitmap);
	m_Bitmap.DeleteObject();
}

CSize CDataStatus::LoadMyBitmap(UINT nID)
{
	CDC * pDC = GetDC();
	CDC m_MemDC;
	m_MemDC.CreateCompatibleDC(pDC);
	CSize m_Size = pDC->GetTextExtent("刷新");
	ReleaseDC(pDC);
	CRect rect(0,0,60,32);
	CBitmap *pBitmap,*pOldBitmap;
	pBitmap = new CBitmap;
	pBitmap->LoadBitmap(nID);
	pOldBitmap = m_MemDC.SelectObject(pBitmap);
	TransParentDC(rect,&m_MemDC);
	m_MemDC.SelectObject(pOldBitmap);
	m_ToolBarList.Add(pBitmap,GetSysColor(COLOR_MENU));
	pBitmap->DeleteObject();
	delete pBitmap;
	return m_Size;
}
// CDataStatus message handlers
void CDataStatus::OnPaint() 
{
	
	
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CFont	*pFont = new CFont();
	pFont->CreateFont(0,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,_T("Arial"));
	
	GetDlgItem(IDC_CITY)->SetFont(pFont,TRUE);
	
	delete pFont;
	
	
	
	// Do not call CDialog::OnPaint() for painting messages
}

LONG CDataStatus::OnInitDialog(UINT wParam, LONG lParam)
{
	
	BOOL bRet = HandleInitDialog(wParam, lParam);
	if(!UpdateData(FALSE))
	{
		TRACE0("Warning, Unalbe to init update.\n");
	}
	
	GetWeather();
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP //CBRS_LEFT
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR_MAIN))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	//下面是允许工具栏可以托动
	// 	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY); //允许工具栏在工具栏区域中移动
	// 	EnableDocking(CBRS_ALIGN_ANY);  //此框架任何地方允许可拖动，如果已经为别的ToolBar写过可以不用再写
	//	DockControlBar(&m_wndToolBar); //允许此工具栏在整个window窗口中拖动
	
   	m_ToolBarList.Create(60,36,ILC_COLOR24|ILC_MASK,0,0);
	CSize m_Size = 
	LoadMyBitmap(IDB_BITMAP_XTSZ);
	LoadMyBitmap(IDB_BITMAP_FWSC);
	LoadMyBitmap(IDB_BITMAP_SYGJ);
	LoadMyBitmap(IDB_BITMAP_GXYM);
	LoadMyBitmap(IDB_BITMAP_GYCX);
	m_wndToolBar.SetButtonText(0,_T("系统设置"));
	m_wndToolBar.SetButtonText(1,_T("服务生成"));
	m_wndToolBar.SetButtonText(2,_T("实用工具"));
	m_wndToolBar.SetButtonText(3,_T("关于程序"));
	m_wndToolBar.SetButtonText(4,_T("退出程序"));

	m_wndToolBar.GetToolBarCtrl().SetImageList(&m_ToolBarList);
	m_wndToolBar.GetToolBarCtrl().SetDisabledImageList(&m_ToolBarList);
	m_wndToolBar.GetToolBarCtrl().SetButtonSize(CSize(60,34 + m_Size.cy + 5));
	m_wndToolBar.GetToolBarCtrl().SetBitmapSize(CSize(60,33));
	
	// 	if (m_Picture.Load(MAKEINTRESOURCE(IDR_TUTU),_T("GIF")))   //显示GIF图片
	// 	m_Picture.Draw();
	
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); //显示状态栏
	m_gundon.GetClientRect(m_rect_parent);
	m_gundonweb.GetClientRect(m_rect_web);
    m_gundonweb.SetParent(&m_gundon);
    CurRect.DeflateRect(0,1,m_rect_parent.Width()-m_rect_web.Width(),1);
	m_gundonweb.MoveWindow(m_rect_parent);
	SetTimer(1,30,NULL);//天气提醒字符速度
	// Add Code here	
	OnTimer( 30);
	m_gundonweb.MoveWindow(m_rect_web);
	// Add Code here

//	CWinThread * hThread = AfxBeginThread(OnCPU, this);
    UpdateData(FALSE);
	return bRet;   
}

//cpu使用获取线程
UINT __cdecl CDataStatus::OnCPU(LPVOID pParam)
{
	CDataStatus *pItem = (CDataStatus*)pParam;
	
	pItem->OnInitCPU();
	
	return 0;
}
//cpu使用获取
void CDataStatus::OnInitCPU()
{
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
	SYSTEM_TIME_INFORMATION SysTimeInfo;
	SYSTEM_BASIC_INFORMATION SysBaseInfo;
	double dbIdleTime;
	double dbSystemTime;
	LONG status;
	LARGE_INTEGER liOldIdleTime = {0,0};
	LARGE_INTEGER liOldSystemTime = {0,0};
	
	NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle(_T("ntdll")),"NtQuerySystemInformation");
	
	if (!NtQuerySystemInformation)
		return;
	
	// get number of processors in the system
	status = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL);
	if (status != NO_ERROR)
		return;
	
	//printf("\nCPU Usage (press any key to exit):    ");
	while(!_kbhit())
	{
		// get new system time
		status = NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0);
		if (status!=NO_ERROR)
			return;
		
		// get new CPU's idle time
		status =NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL);
		if (status != NO_ERROR)
			return;
		
		// if it's a first call - skip it
		if (liOldIdleTime.QuadPart != 0)
		{
			// CurrentValue = NewValue - OldValue
			dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime);
			dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) -
				
				Li2Double(liOldSystemTime);
			
			// CurrentCpuIdle = IdleTime / SystemTime
			dbIdleTime = dbIdleTime / dbSystemTime;
			
			// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
			dbIdleTime = 100.0 - dbIdleTime * 100.0 /
				
				(double)SysBaseInfo.bKeNumberProcessors + 0.5;
			
			CString strCPU;
			strCPU.Format(_T("%3d%% ,"), (UINT)dbIdleTime);
			SetDlgItemText(IDC_cpuu, strCPU);
			//	RedrawWindow();
		}
		
		// store new CPU's idle and system time
		liOldIdleTime = SysPerfInfo.liIdleTime;
		liOldSystemTime = SysTimeInfo.liKeSystemTime;
		
		// wait one second
		Sleep(800);
	}
	//printf("\n");
}
HBRUSH CDataStatus::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)        // 消息响应函数实现  
{  
	
	if ((pWnd->GetDlgCtrlID() == IDC_CITY))
	{
		COLORREF clr = RGB(242, 0, 6);
		pDC->SetTextColor(clr);   //设置白色的文本
		clr = RGB(240,240,240);
		pDC->SetBkColor(clr);     //设置黑色的背景
		return CreateSolidBrush(clr);  //作为约定，返回背景色对应的刷子句柄
	}
	
	if ((pWnd->GetDlgCtrlID() == IDC_WENDU))
	{
		COLORREF clr = RGB(250, 0, 0);
		pDC->SetTextColor(clr);   //设置白色的文本
		clr = RGB(240,240,240);
		pDC->SetBkColor(clr);     //设置黑色的背景
		return CreateSolidBrush(clr);  //作为约定，返回背景色对应的刷子句柄
	}
	if ((pWnd->GetDlgCtrlID() == IDC_TIANQI))
	{
		COLORREF clr = RGB(37, 0, 251);
		pDC->SetTextColor(clr);   //设置白色的文本
		clr = RGB(240,240,240);
		pDC->SetBkColor(clr);     //设置黑色的背景
		return CreateSolidBrush(clr);  //作为约定，返回背景色对应的刷子句柄
	}
	
	if ((pWnd->GetDlgCtrlID() == IDC_FENGSU))
	{
		COLORREF clr = RGB(37, 0, 251);
		pDC->SetTextColor(clr);   //设置白色的文本
		clr = RGB(240,240,240);
		pDC->SetBkColor(clr);     //设置黑色的背景
		return CreateSolidBrush(clr);  //作为约定，返回背景色对应的刷子句柄
	}
	
// 	if ((pWnd->GetDlgCtrlID() == IDC_GUNDON))
// 	{
// 		COLORREF clr = RGB(37, 0, 251);
// 		pDC->SetTextColor(clr);   //设置白色的文本
// 		clr = RGB(240,240,240);
// 		pDC->SetBkColor(clr);     //设置黑色的背景
// 		return CreateSolidBrush(clr);  //作为约定，返回背景色对应的刷子句柄
// 	}
	return 0;
}  

void CDataStatus::OnTimer(UINT nIDEvent) 
{
// 	TCHAR szBuffer[256];
// 	DWORD dwSize=256;
// 	GetComputerName(szBuffer,&dwSize);
// 	m_Men=szBuffer;
// 	MEMORYSTATUS mem_stat;
// 	GlobalMemoryStatus(&mem_stat);
// 	m_Men.Format("%d %%",mem_stat.dwMemoryLoad);
	//显示时钟
	CString ww;
	CTime time=CTime::GetCurrentTime();
	ww=time.Format("%Y-%m-%d %H:%M:%S");//转换时间格式
	
	//m_edt1.SetColor(RGB(255,255,255),RGB(0,0,255));	//文字颜色(文字背景色,前景色)
	//m_edt1.SetBkColor(RGB(255,255,0));	//控件背景色
	m_edt1.SetColor(RGB(255,255,255),RGB(0,0,0));	//文字颜色(文字背景色,前景色)
	m_edt1.SetBkColor(RGB(255,255,255));	//控件背景色
	m_edt1.SetText(ww);
	
	CWnd::OnTimer(nIDEvent);
	///
	if (m_rect_web.right<=m_rect_parent.left)
	{
		m_rect_web.left = m_rect_parent.right;
		m_rect_web.right = m_rect_parent.right-m_rect_web.Width();
		
		
	}
	else
	{
		m_rect_web.left-=2;
		m_rect_web.right-=2;
		
	}	
	//调整控件位置
	m_gundonweb.MoveWindow(m_rect_web);


	
}

int URLEncode(LPCTSTR pszUrl, LPTSTR pszEncode, int nEncodeLen)
{
	if (pszUrl == NULL)
		return 0;
	
	if (pszEncode == NULL || nEncodeLen == 0)
		return 0;
	
	//定义变量  
	int nLength = 0;
	WCHAR* pWString = NULL;
	TCHAR* pString = NULL;
	
	//先将字符串由多字节转换成UTF-8编码  
	nLength = MultiByteToWideChar(CP_ACP, 0, pszUrl, -1, NULL, 0);
	
	//分配Unicode空间  
	pWString = new WCHAR[nLength];
	
	//先转换成Unicode  
	MultiByteToWideChar(CP_ACP, 0, pszUrl, -1, pWString, nLength);
	
	//分配UTF-8空间  
	nLength = WideCharToMultiByte(CP_UTF8, 0, pWString, -1, NULL, 0, NULL, NULL);
	pString = new TCHAR[nLength];
	
	//Unicode转到UTF-8  
	nLength = WideCharToMultiByte(CP_UTF8, 0, pWString, -1, pString, nLength, NULL, NULL);
	
	static char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	memset(pszEncode, 0, nEncodeLen / sizeof(TCHAR));
	
	for (int i = 0; i < nLength - 1; i++)
	{
		unsigned char c = pString[i];
		if (c > 0x20 && c < 0x7f)    // 数字或字母  
		{
			*pszEncode++ = c;
		}
		else if (c == 0x20)        // 包含空格  
		{
			*pszEncode++ = '+';
		}
		else                        // 进行编码  
		{
			*pszEncode++ = '%';
			*pszEncode++ = hex[c / 16];
			*pszEncode++ = hex[c % 16];
		}
	}
	
	//删除内存  
	delete pWString;
	delete pString;
	
	return nLength;
}

//寻找字符串函数 yesNO是否要删除找到以前的所有字符
CString CDataStatus::FindStr(CString &str,CString strFind1,CString strFind2,bool yesNo)
{
	int nFirst;
	int nEnd;
	CString strTemp;
	nFirst = str.Find(strFind1);
	nEnd = str.Find(strFind2);
	strTemp = str.Mid(nFirst+_tcslen(strFind1),nEnd-nFirst-_tcslen(strFind1));
	if (yesNo)
	{
		str = str.Right(_tcslen(str)-(nEnd + _tcslen(strFind2)));
	}
	
	return strTemp;
}



//获取网络资源  感谢 (彬)泪闯天涯提供的参考代码
// void CDataStatus::SavePng(CString strUrl,CString strPath)
// {
// 	CInternetSession Session;
// 	CString strServerName,strObject;
// 	DWORD dwSvrType;
// 	INTERNET_PORT nPort;
// 	AfxParseURL(strUrl,dwSvrType,strServerName,strObject,nPort);
// 	const TCHAR szHeaders[] = _T("application/x-shockwave-flash, image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/xaml+xml, application/vnd.ms-xpsdocument, application/x-ms-xbap, application/x-ms-application, */*");
// 	CHttpConnection *pConnection = Session.GetHttpConnection(strServerName,nPort);
// 	if (!pConnection)
// 	{
// 		MessageBox(_T("连接服务器失败"));
// 	}
// 	CHttpFile *pHttpFile;
// 	try
// 	{
// 		pHttpFile = (CHttpFile *)pConnection->OpenRequest(
// 			CHttpConnection::HTTP_VERB_GET,
// 			strObject,
// 			NULL,1,NULL,NULL,
// 			INTERNET_FLAG_NO_COOKIES|INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_NO_AUTO_REDIRECT
// 			);
// 	}
// 	catch(CException *e)
// 	{
// 		pHttpFile = NULL;
// 		throw;
// 	}
// 	if (pHttpFile)
// 	{
// 		DWORD dwStatus = 0;
// 		BOOL bSuccess;
// 		try
// 		{
// 			pHttpFile->AddRequestHeaders(szHeaders);
// 			pHttpFile->SendRequest();
// 			bSuccess = pHttpFile->QueryInfoStatusCode(dwStatus);
// 		}
// 		catch(...)
// 		{
// 			delete pHttpFile;
// 			delete pConnection;
// 			return;
// 		}
// 		char strarray[1024];
// 		if (bSuccess && (dwStatus >= 200 && dwStatus <=300))
// 		{
// 			//CString sFileName = GetUrlName
// 			CString strContentType = _T("");
// 			//text/html
// 			pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_TYPE,strContentType);
// 			CFile NetFile;
// 			if (NetFile.Open(strPath,CFile::modeWrite|CFile::modeCreate))
// 			{
// 				int nCount = 0;
// 				nCount = pHttpFile->Read(strarray,1023);
// 				while (nCount>0)
// 				{
// 					NetFile.Write(strarray,nCount);
// 					nCount = pHttpFile->Read(strarray,1023);
// 				}
// 				NetFile.Close();
// 			}
// 
// 		}
// 
// 	}
// 	pHttpFile->Close();
// 	delete pHttpFile;
// 	
// }


////////////////////////////////////////////////////////////////////

//从网络上获取数据
BOOL CDataStatus::GetWeather()
{
	BOOL	bRetCode = FALSE;
	
	CInternetSession	CSession;
	CHttpConnection		*pHttpConn = NULL;
	CHttpFile			*pHttpFile = NULL;
	
	DWORD	dwStatusCode = 0;	//网络访问状态
	CString	strErrMsg;	//错误消息提示
	char	szRecvBuff[128];
	
	try
	{
		
		//http://whois.pconline.com.cn/ip.jsp
		pHttpConn = CSession.GetHttpConnection(_T("whois.pconline.com.cn"),(INTERNET_PORT)80);
		if (pHttpConn == NULL)
		{
			strErrMsg = _T("打开网络连接失败，无法获取更新信息！");
			goto EXIT0;
		}
		pHttpFile = pHttpConn->OpenRequest(CHttpConnection::HTTP_VERB_GET,_T("/ipJson.jsp"));
		if (pHttpFile == NULL)
		{
			strErrMsg = _T("服务器上更新数据丢失，无法获取更新信息！");
			goto EXIT0;
		}
		
		if (!pHttpFile->SendRequest())
		{
			strErrMsg = _T("请求服务器数据失败！无法获取更新信息。");
			goto EXIT0;
		}
		
		
		//查询网络访问状态
		pHttpFile->QueryInfoStatusCode(dwStatusCode);
		if (dwStatusCode != 200)
		{
			strErrMsg.Format(_T("获取服务器返回数据失败！状态码：%d，无法获取更新信息"),
				dwStatusCode);
			goto EXIT0;
		}
		
		//接受数据
		pHttpFile->Read(szRecvBuff,160);
		
		bRetCode = TRUE;
	}
	catch (CInternetException* e)
	{
		e->Delete();
	}
	
EXIT0:
	//关闭网络连接
	pHttpFile->Close();
	delete pHttpFile;
	pHttpConn->Close();
	delete pHttpConn;
	CSession.Close();
	
	
	//如果网络数据没有获取程序，要返回
	if (!bRetCode)
	{
		return bRetCode;
	}
	
	//处理数据，分离出版本号
	CString	strRecv(szRecvBuff,130);
	strRecv.Delete(0,strRecv.Find(_T("city\":\"")) + _tcslen(_T("city\":\"")));
	strRecv = strRecv.Left(strRecv.Find(_T("\",\"cityCode")));
	SetDlgItemText(IDC_CITY,strRecv);
	
	TCHAR szEncode[255];
	CString strCoding;
	
    URLEncode(strRecv, szEncode, sizeof(szEncode));
	
	strCoding = szEncode;
	
    CString strURL;
	
	strURL = _T("http://api.map.baidu.com/telematics/v3/weather?location=") + strCoding + _T("&&output=json&&ak=DC8048985ecf4db39f22016f18226a1c");
	
	CInternetSession mySession(NULL,0);
	CHttpFile *httpFile;
	httpFile = (CHttpFile *)mySession.OpenURL(strURL);
	if (!httpFile)
	{
		MessageBox(_T("读取天气信息错误"),_T(""),MB_OK);
		
	}
	DWORD dwStatus;
	DWORD dwBuffLen = sizeof(dwStatus);
	BOOL bSuccess;
	bSuccess = httpFile->QueryInfo(HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER,&dwStatus,&dwBuffLen);
	CString str;
	if (dwStatus >= 200 && dwStatus < 300)
	{
		CString strTemp;
		
		while (httpFile->ReadString(strTemp))
		{
			str += strTemp;	
			
		}
		
	}
	
   	httpFile->Close();
	delete httpFile;
	mySession.Close();
	
    //uft-8 转 unicode
	CString wData;
	WCHAR *wword;
	DWORD num = MultiByteToWideChar(CP_UTF8,0,(LPCSTR)str.GetBuffer(0),-1,NULL,0);
	wword = (WCHAR*)calloc(num,sizeof(WCHAR));  
	memset(wword, 0, num*sizeof(WCHAR)); 
	MultiByteToWideChar(CP_UTF8,0,(LPCSTR)str.GetBuffer(0),-1,wword,num);
	str.ReleaseBuffer();
	wData = wword;
	
	
    m_1= "★"+FindStr(wData,_T("des\":\""),_T("\"},{\"title"),true)+"★";
	m_2= "★"+FindStr(wData,_T("des\":\""),_T("\"},{\"title"),true)+"★";
	m_3= "★"+FindStr(wData,_T("des\":\""),_T("\"},{\"title"),true)+"★";
	m_strgundon = "★  "+FindStr(wData,_T("des\":\""),_T("\"},{\"title"),true);

	SetDlgItemText(IDC_GUNDONGWEB,m_strgundon);
	
	
	m_strwendu = FindStr(wData,_T("[{\"date\":\""),_T("\",\"dayPictureUrl"),true);
	SetDlgItemText(IDC_WENDU,m_strwendu);
	
	
	m_strtianqi = FindStr(wData,_T("weather\":\""),_T("\",\"wind"),true);
	SetDlgItemText(IDC_TIANQI,m_strtianqi);
	
	//  m_strNewUrl1 = FindStr(wData,_T("dayPictureUrl\":\""),_T("\",\"night"),true);
	// 	m_strNewUrl2 = FindStr(wData,_T("Url\":\""),_T("\",\"weather"),true);
	
	m_strfengsu = FindStr(wData,_T("wind\":\""),_T("\",\"temperature"),true) +" ( "+ FindStr(wData,_T("temperature\":\""),_T("\"},{\"date"),true) +" ) ";
	SetDlgItemText(IDC_FENGSU,m_strfengsu);
/*
		
	if (m_strtianqi=="多云")
	{
		CStatic* p1 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon1   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_duoyun),   IMAGE_ICON,   42,   30,   0); 
		p1->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p1->SetIcon(hIcon1);  
		
		CStatic* p2 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon2   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_duoyun),   IMAGE_ICON,   42,   30,   0); 
		p2->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p2->SetIcon(hIcon2);
		
	}
	
	if (m_strtianqi=="阴")
	{
		CStatic* p3 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon3   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_yin),   IMAGE_ICON,   42,   30,   0); 
		p3->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p3->SetIcon(hIcon3);  
		
		CStatic* p4 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon4   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_yin),   IMAGE_ICON,   42,   30,   0); 
		p4->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p4->SetIcon(hIcon4);
		
	}
	
	if (m_strtianqi=="大雨")
	{
		CStatic* p5 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon5   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_dayu),   IMAGE_ICON,   42,   30,   0); 
		p5->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p5->SetIcon(hIcon5);  
		
		CStatic* p6 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon6   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_dayu),   IMAGE_ICON,   42,   30,   0); 
		p6->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p6->SetIcon(hIcon6);
		
	}
	
	if (m_strtianqi=="小雨")
	{
		CStatic* p7 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon7   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_xiaoyu),   IMAGE_ICON,   42,   30,   0); 
		p7->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p7->SetIcon(hIcon7);  
		
		CStatic* p8 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon8   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_xiaoyu),   IMAGE_ICON,   42,   30,   0); 
		p8->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p8->SetIcon(hIcon8);
		
	}
	
	if (m_strtianqi=="大雪")
	{
		CStatic* p9 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon9   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_daxue),   IMAGE_ICON,   42,   30,   0); 
		p9->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p9->SetIcon(hIcon9);  
		
		CStatic* p10 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon10   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_daxue),   IMAGE_ICON,   42,   30,   0); 
		p10->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p10->SetIcon(hIcon10);
		
	}
	
	if (m_strtianqi=="小雪")
	{
		CStatic* p11 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon11   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_xiaoxue),   IMAGE_ICON,   42,   30,   0); 
		p11->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p11->SetIcon(hIcon11);  
		
		CStatic* p12 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon12   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_xiaoxue),   IMAGE_ICON,   42,   30,   0); 
		p12->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p12->SetIcon(hIcon12);
		
	}
	
	if (m_strtianqi=="晴")
	{
		CStatic* p13 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon13   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_qing),   IMAGE_ICON,   42,   30,   0); 
		p13->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p13->SetIcon(hIcon13);  
		
		CStatic* p14 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon14   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_qing),   IMAGE_ICON,   42,   30,   0); 
		p14->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p14->SetIcon(hIcon14);
		
	}
	
	if (m_strtianqi=="暴雨")
	{
		CStatic* p15 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon15   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_baoyu),   IMAGE_ICON,   42,   30,   0); 
		p15->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p15->SetIcon(hIcon15);  
		
		CStatic* p16 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon16   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_baoyu),   IMAGE_ICON,   42,   30,   0); 
		p16->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p16->SetIcon(hIcon16);
		
	}
	
	if (m_strtianqi=="中雨")
	{
		CStatic* p17 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon17   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_zhongyu),   IMAGE_ICON,   42,   30,   0); 
		p17->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p17->SetIcon(hIcon17);  
		
		CStatic* p18 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon18   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_zhongyu),   IMAGE_ICON,   42,   30,   0); 
		p18->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p18->SetIcon(hIcon18);
		
	}
	
	if (m_strtianqi=="中雨转大雨")
	{
		CStatic* p19 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon19   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_zhongyuzhuandayu),   IMAGE_ICON,   42,   30,   0); 
		p19->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p19->SetIcon(hIcon19);  
		
		CStatic* p20 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon20   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_zhongyuzhuandayu),   IMAGE_ICON,   42,   30,   0); 
		p20->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p20->SetIcon(hIcon20);
		
	}
	
	if (m_strtianqi=="大暴雨")
	{
		CStatic* p21 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon21   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_dabaoyu),   IMAGE_ICON,   42,   30,   0); 
		p21->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p21->SetIcon(hIcon21);  
		
		CStatic* p22 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon22   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_dabaoyu),   IMAGE_ICON,   42,   30,   0); 
		p22->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p22->SetIcon(hIcon22);
		
	}
	
	if (m_strtianqi=="特大暴雨")
	{
		CStatic* p23 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon23   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_tedabaoyu),   IMAGE_ICON,   42,   30,   0); 
		p23->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p23->SetIcon(hIcon23);  
		
		CStatic* p24 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon24   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_tedabaoyu),   IMAGE_ICON,   42,   30,   0); 
		p24->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p24->SetIcon(hIcon24);
		
	}
	
	if (m_strtianqi=="暴雨转大暴雨")
	{
		CStatic* p25 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon25   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_baoyuzhuandabaoyu),   IMAGE_ICON,   42,   30,   0); 
		p25->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p25->SetIcon(hIcon25);  
		
		CStatic* p26 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon26   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_baoyuzhuandabaoyu),   IMAGE_ICON,   42,   30,   0); 
		p26->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p26->SetIcon(hIcon26);
		
	}
	
	if (m_strtianqi=="大暴雨转特大暴雨")
	{
		CStatic* p27 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon27   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_dabaoyuzhuantedabaoyu),   IMAGE_ICON,   42,   30,   0); 
		p27->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p27->SetIcon(hIcon27);  
		
		CStatic* p28 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon28   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_dabaoyuzhuantedabaoyu),   IMAGE_ICON,   42,   30,   0); 
		p28->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p28->SetIcon(hIcon28);
		
	}
	
	if (m_strtianqi=="大雨转暴雨")
	{
		CStatic* p29 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon29   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_dayuzhuanbaoyu),   IMAGE_ICON,   42,   30,   0); 
		p29->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p29->SetIcon(hIcon29);  
		
		CStatic* p30 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon30   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_dayuzhuanbaoyu),   IMAGE_ICON,   42,   30,   0); 
		p30->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p30->SetIcon(hIcon30);
		
	}
	
	if (m_strtianqi=="阵雨")
	{
		CStatic* p31 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon31   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_zhenyu),   IMAGE_ICON,   42,   30,   0); 
		p31->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p31->SetIcon(hIcon31);  
		
		CStatic* p32 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon32   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_zhenyu),   IMAGE_ICON,   42,   30,   0); 
		p32->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p32->SetIcon(hIcon32);
		
	}
	
	if (m_strtianqi=="雷阵雨")
	{
		CStatic* p33 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon33   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_leizhenyu),   IMAGE_ICON,   42,   30,   0); 
		p33->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p33->SetIcon(hIcon33);  
		
		CStatic* p34 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon34   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_leizhenyu),   IMAGE_ICON,   42,   30,   0); 
		p34->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p34->SetIcon(hIcon34);
		
	}
	
	if (m_strtianqi=="冻雨")
	{
		CStatic* p35 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon35   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_dongyu),   IMAGE_ICON,   42,   30,   0); 
		p35->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p35->SetIcon(hIcon35);  
		
		CStatic* p36 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon36   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_dongyu),   IMAGE_ICON,   42,   30,   0); 
		p36->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p36->SetIcon(hIcon36);
		
	}
	
	if (m_strtianqi=="雷阵雨伴有冰雹")
	{
		CStatic* p37 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon37   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_leizhenyubanyoubingbao),   IMAGE_ICON,   42,   30,   0); 
		p37->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p37->SetIcon(hIcon37);  
		
		CStatic* p38 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon38   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_leizhenyubanyoubingbao),   IMAGE_ICON,   42,   30,   0); 
		p38->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p38->SetIcon(hIcon38);
		
	}
	
	if (m_strtianqi=="雨夹雪")
	{
		CStatic* p39 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon39   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_yujiaxue),   IMAGE_ICON,   42,   30,   0); 
		p39->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p39->SetIcon(hIcon39);  
		
		CStatic* p40 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon40   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_yujiaxue),   IMAGE_ICON,   42,   30,   0); 
		p40->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p40->SetIcon(hIcon40);
		
	}
	
	if (m_strtianqi=="雾")
	{
		CStatic* p41 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon41   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_wu),   IMAGE_ICON,   42,   30,   0); 
		p41->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p41->SetIcon(hIcon41);  
		
		CStatic* p42 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon42   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_wu),   IMAGE_ICON,   42,   30,   0); 
		p42->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p42->SetIcon(hIcon42);
		
	}
	
	if (m_strtianqi=="小雨转中雨")
	{
		CStatic* p43 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon43   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_xiaoyuzhuanzhongyu),   IMAGE_ICON,   42,   30,   0); 
		p43->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p43->SetIcon(hIcon43);  
		
		CStatic* p44 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon44   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_xiaoyuzhuanzhongyu),   IMAGE_ICON,   42,   30,   0); 
		p44->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p44->SetIcon(hIcon44);
		
	}
	
	if (m_strtianqi=="中雪")
	{
		CStatic* p45 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon45   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_zhongxue),   IMAGE_ICON,   42,   30,   0); 
		p45->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p45->SetIcon(hIcon45);  
		
		CStatic* p46 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon46   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_zhongxue),   IMAGE_ICON,   42,   30,   0); 
		p46->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p46->SetIcon(hIcon46);
		
	}
	
	if (m_strtianqi=="暴雪")
	{
		CStatic* p47 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon47   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_baoxue),   IMAGE_ICON,   42,   30,   0); 
		p47->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p47->SetIcon(hIcon47);  
		
		CStatic* p48 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon48   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_baoxue),   IMAGE_ICON,   42,   30,   0); 
		p48->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p48->SetIcon(hIcon48);
		
	}
	
	if (m_strtianqi=="沙尘暴")
	{
		CStatic* p49 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon49   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_shachenbao),   IMAGE_ICON,   42,   30,   0); 
		p49->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p49->SetIcon(hIcon49);  
		
		CStatic* p50 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon50   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_shachenbao),   IMAGE_ICON,   42,   30,   0); 
		p50->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p50->SetIcon(hIcon50);
		
	}
	
	if (m_strtianqi=="强沙尘暴")
	{
		CStatic* p51 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon51   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_qiangshachenbao),   IMAGE_ICON,   42,   30,   0); 
		p51->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p51->SetIcon(hIcon51);  
		
		CStatic* p52 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon52   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_qiangshachenbao),   IMAGE_ICON,   42,   30,   0); 
		p52->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p52->SetIcon(hIcon52);
		
	}
	
	if (m_strtianqi=="霾")
	{
		CStatic* p53 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon53   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_mai),   IMAGE_ICON,   42,   30,   0); 
		p53->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p53->SetIcon(hIcon53);  
		
		CStatic* p54 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon54   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_mai),   IMAGE_ICON,   42,   30,   0); 
		p54->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p54->SetIcon(hIcon54);
		
	}
	
	if (m_strtianqi=="扬沙")
	{
		CStatic* p55 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon55   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_yangsha),   IMAGE_ICON,   42,   30,   0); 
		p55->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p55->SetIcon(hIcon55);  
		
		CStatic* p56 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon56   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_yangsha),   IMAGE_ICON,   42,   30,   0); 
		p56->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p56->SetIcon(hIcon56);
		
	}
	
	if (m_strtianqi=="浮尘")
	{
		CStatic* p57 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon57   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_fuchen),   IMAGE_ICON,   42,   30,   0); 
		p57->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p57->SetIcon(hIcon57);  
		
		CStatic* p58 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon58   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_fuchen),   IMAGE_ICON,   42,   30,   0); 
		p58->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p58->SetIcon(hIcon58);
		
	}
	
	if (m_strtianqi=="大雪转暴雪")
	{
		CStatic* p59 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon59   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_daxuezhuanbaoxue),   IMAGE_ICON,   42,   30,   0); 
		p59->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p59->SetIcon(hIcon59);  
		
		CStatic* p60 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon60   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_daxuezhuanbaoxue),   IMAGE_ICON,   42,   30,   0); 
		p60->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p60->SetIcon(hIcon60);
		
	}
	
	if (m_strtianqi=="小雪转中雪")
	{
		CStatic* p61 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon61   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_xiaoxuezhuanzhongxue),   IMAGE_ICON,   42,   30,   0); 
		p61->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p61->SetIcon(hIcon61);  
		
		CStatic* p62 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon62   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_xiaoxuezhuanzhongxue),   IMAGE_ICON,   42,   30,   0); 
		p62->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p62->SetIcon(hIcon62);
		
	}
	
	if (m_strtianqi=="中雪转大雪")
	{
		CStatic* p63 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon63   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_zhongxuezhuandaxue),   IMAGE_ICON,   42,   30,   0); 
		p63->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p63->SetIcon(hIcon63);  
		
		CStatic* p64 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon64   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_zhongxuezhuandaxue),   IMAGE_ICON,   42,   30,   0); 
		p64->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p64->SetIcon(hIcon64);
		
	}
	
	if (m_strtianqi=="阵雪")
	{
		CStatic* p65 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon65   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_zhenxue),   IMAGE_ICON,   42,   30,   0); 
		p65->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p65->SetIcon(hIcon65);  
		
		CStatic* p66 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon66   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_zhenxue),   IMAGE_ICON,   42,   30,   0); 
		p66->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p66->SetIcon(hIcon66);
		
	}
	
	if (m_strtianqi=="阴转多云")
	{
		CStatic* p67 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon67   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_duoyun),   IMAGE_ICON,   42,   30,   0); 
		p67->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p67->SetIcon(hIcon67);  
		
		CStatic* p68 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon68   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_yin),   IMAGE_ICON,   42,   30,   0); 
		p68->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p68->SetIcon(hIcon68);
		
	}
	
	if (m_strtianqi=="多云转阴")
	{
		CStatic* p69 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon69   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_yin),   IMAGE_ICON,   42,   30,   0); 
		p69->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p69->SetIcon(hIcon69);  
		
		CStatic* p70 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon70   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_duoyun),   IMAGE_ICON,   42,   30,   0); 
		p70->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p70->SetIcon(hIcon70);
		
	}
	
	if (m_strtianqi=="小雨转多云")
	{
		CStatic* p71 = (CStatic*)GetDlgItem(IDC_NIGHT);
		HICON   hIcon71   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_NIGHT_duoyun),   IMAGE_ICON,   42,   30,   0); 
		p71->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p71->SetIcon(hIcon71);  
		
		CStatic* p72 = (CStatic*)GetDlgItem(IDC_DAY);
		HICON   hIcon72   =   (HICON)::LoadImage(::AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDI_DAY_xiaoyu),   IMAGE_ICON,   42,   30,   0); 
		p72->ModifyStyle(0x0,SS_ICON|SS_CENTERIMAGE);     
		p72->SetIcon(hIcon72);
		
	}
	*/

	//从网络保存图片到本地
	// 	SavePng(m_strNewUrl1,_T("c:\\day.png"));
	// 	SavePng(m_strNewUrl2,_T("c:\\night.png"));
	
	
	//-------------------------------------------------------------------------
	
	return bRetCode;	
	
}




