// ShellManager.h: interface for the CShellManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHELLMANAGER_H__5BE31746_111D_46D6_B423_AB0D6E9BA64C__INCLUDED_)
#define AFX_SHELLMANAGER_H__5BE31746_111D_46D6_B423_AB0D6E9BA64C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Manager.h"

class CShellManager : public CManager  
{
public:
	CShellManager(CClientSocket *pClient);
	virtual ~CShellManager();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
private:
    HANDLE m_hReadPipeHandle;   
    HANDLE m_hWritePipeHandle; 
	HANDLE m_hReadPipeShell;
    HANDLE m_hWritePipeShell;
	
    HANDLE m_hProcessHandle;
	HANDLE m_hThreadHandle;
    HANDLE m_hThreadRead;
	HANDLE m_hThreadMonitor;
	
	static DWORD WINAPI ReadPipeThread(LPVOID lparam);
	static DWORD WINAPI MonitorThread(LPVOID lparam);
};


#endif // !defined(AFX_SHELLMANAGER_H__5BE31746_111D_46D6_B423_AB0D6E9BA64C__INCLUDED_)
