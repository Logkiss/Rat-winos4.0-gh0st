// GetSystemInfo.h: interface for the CGetSystemInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETSYSTEMINFO_H__AEAC9DF9_93F0_4FFC_A9B4_0E1AAC9B69F1__INCLUDED_)
#define AFX_GETSYSTEMINFO_H__AEAC9DF9_93F0_4FFC_A9B4_0E1AAC9B69F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Manager.h"

//信息列表结构体
struct tagSystemInfo
{
	char szCpuInfo[50];   //CPU信息
	char szActiveTime[50]; //活动时间
	char szAntiVirus[50]; //杀毒软件
	char szUserName[30];   //当前用户名
	char szPcName[30];  //计算机名
	char szRemotePort[10]; //远程端口
	DWORD szMemory;         //内存大小 
	DWORD szMemoryFree;     //可用内存 
	DWORD szDriveSize;      //硬盘大小
	DWORD szDriveFreeSize;  //可用硬盘大小
	char szOpenInfo[20];	// 是否双开(是否开启)
	char szDLLPath[128];    // 服务端路径
	char szScrxy[20];
	char szQQNum[100];
	char szLocalTime[50];
	BOOL bSysFirewall;
	WCHAR lpLCData[32];
};

class CGetSystemInfo  : public CManager  
{
public:
	CGetSystemInfo(CClientSocket *pClient);
	virtual ~CGetSystemInfo();

	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);

	void SendSysInfoList();
};

#endif // !defined(AFX_GETSYSTEMINFO_H__AEAC9DF9_93F0_4FFC_A9B4_0E1AAC9B69F1__INCLUDED_)
