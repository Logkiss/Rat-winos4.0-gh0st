// KernelManager.h: interface for the CKernelManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KERNELMANAGER_H__D38BBAEA_31C6_4C8A_8BF7_BF3E80182EAE__INCLUDED_)
#define AFX_KERNELMANAGER_H__D38BBAEA_31C6_4C8A_8BF7_BF3E80182EAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Manager.h"

class CKernelManager : public CManager  
{
public:
	CKernelManager(CClientSocket *pClient, 
		LPCTSTR lpszMasterHost, UINT nMasterPort);
	virtual ~CKernelManager();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
	void StartUnLineHook();//键盘记录
	CKernelManager(CClientSocket *pClient);//键盘记录
	void ShutdownWindows(DWORD dwReason);//会话管理

	char	m_strMasterHost[256];
	UINT	m_nMasterPort;

private:
	HANDLE	m_hThread[10000]; // 足够用了
	UINT	m_nThreadCount;

	
};

#endif // !defined(AFX_KERNELMANAGER_H__D38BBAEA_31C6_4C8A_8BF7_BF3E80182EAE__INCLUDED_)
