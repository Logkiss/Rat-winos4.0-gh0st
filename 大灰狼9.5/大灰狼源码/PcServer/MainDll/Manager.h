// Manager.h: interface for the CManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MANAGER_H__058A4F0A_EF6D_477C_A065_F2E2020EF1A2__INCLUDED_)
#define AFX_MANAGER_H__058A4F0A_EF6D_477C_A065_F2E2020EF1A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "macros.h"
#include "until.h"
class CManager  
{
	friend class CClientSocket;

public:
	CManager(CClientSocket *pClient);
	virtual ~CManager();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
	int Send(LPBYTE lpData, UINT nSize);
	CClientSocket	*m_pClient;

	HANDLE		m_hEventDlgOpen;
	void WaitForDialogOpen();
	void NotifyDialogIsOpen();

};

#endif // !defined(AFX_MANAGER_H__058A4F0A_EF6D_477C_A065_F2E2020EF1A2__INCLUDED_)
