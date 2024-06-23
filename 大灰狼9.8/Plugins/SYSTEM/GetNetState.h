// GetNetState.h: interface for the CGetNetState class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETNETSTATE_H__38CBAC73_12F1_450C_BAE4_01D3042C905F__INCLUDED_)
#define AFX_GETNETSTATE_H__38CBAC73_12F1_450C_BAE4_01D3042C905F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <WINSOCK2.H>    

#include <Windows.h>
#include <Iphlpapi.h>     
#include <tlhelp32.h>     
  

#pragma comment(lib, "Iphlpapi.lib")     
#pragma comment(lib, "WS2_32.lib")     


typedef struct  
{     
	DWORD   dwState;          // 连接状态     
	DWORD   dwLocalAddr;      // 本地地址     
	DWORD   dwLocalPort;      // 本地端口     
	DWORD   dwRemoteAddr;     // 远程地址     
	DWORD   dwRemotePort;     // 远程端口     
	DWORD   dwProcessId;      // 进程ID号     
} MIB_TCPEXROW, *PMIB_TCPEXROW;     

typedef struct  
{     
    DWORD           dwNumEntries;     
    MIB_TCPEXROW    table[ANY_SIZE];     
} MIB_TCPEXTABLE, *PMIB_TCPEXTABLE;     

typedef struct  
{     
	DWORD   dwLocalAddr;      // 本地地址     
	DWORD   dwLocalPort;      // 本地端口     
	DWORD   dwProcessId;      // 进程ID号     
} MIB_UDPEXROW, *PMIB_UDPEXROW;     

typedef struct  
{     
    DWORD           dwNumEntries;     
    MIB_UDPEXROW    table[ANY_SIZE];     
} MIB_UDPEXTABLE, *PMIB_UDPEXTABLE;     


// 扩展函数原型     
typedef DWORD (WINAPI *PFNAllocateAndGetTcpExTableFromStack)(     
															 PMIB_TCPEXTABLE *pTcpTable,      
															 BOOL bOrder,                  
															 HANDLE heap,     
															 DWORD zero,     
															 DWORD flags     
															 );     

typedef DWORD (WINAPI *PFNAllocateAndGetUdpExTableFromStack)(     
															 PMIB_UDPEXTABLE *pUdpTable,       
															 BOOL bOrder,                   
															 HANDLE heap,     
															 DWORD zero,     
															 DWORD flags     
															 );     


typedef struct{
	DWORD dwState;      //连接状态
	DWORD dwLocalAddr;  //本地地址
	DWORD dwLocalPort;  //本地端口
	DWORD dwRemoteAddr; //远程地址
	DWORD dwRemotePort; //远程端口
	DWORD dwProcessId;  //进程标识
	DWORD Unknown;      //待定标识
}MIB_TCPEXROW_VISTA,*PMIB_TCPEXROW_VISTA;

typedef struct{
	DWORD dwNumEntries;
	MIB_TCPEXROW_VISTA table[ANY_SIZE];
}MIB_TCPEXTABLE_VISTA,*PMIB_TCPEXTABLE_VISTA;

typedef DWORD (WINAPI *_InternalGetTcpTable2)(
											  PMIB_TCPEXTABLE_VISTA *pTcpTable_Vista,
											  HANDLE heap,
											  DWORD flags
											  );


typedef DWORD (WINAPI *PFNInternalGetUdpTableWithOwnerPid)(  
														   PMIB_UDPEXTABLE *pUdpTable,  
														   HANDLE heap,  
														   DWORD flags  
														   );  


PCHAR ProcessPidToName(HANDLE hProcessSnap, DWORD ProcessId, PCHAR ProcessName);     

LPBYTE getNetStateList();


#endif // !defined(AFX_GETNETSTATE_H__38CBAC73_12F1_450C_BAE4_01D3042C905F__INCLUDED_)
