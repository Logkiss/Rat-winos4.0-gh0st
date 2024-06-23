// ProgressManager.h: interface for the CProgressManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRESSMANAGER_H__DDB9BCDF_DA32_4C83_B4C9_F9818EF5EDF4__INCLUDED_)
#define AFX_PROGRESSMANAGER_H__DDB9BCDF_DA32_4C83_B4C9_F9818EF5EDF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Contain/Manager.h"
#include <tlhelp32.h>

class CProgressManager  : public CManager  
{
public:
	CProgressManager(CClientSocket *pClient);
	virtual ~CProgressManager();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
	void KillProcess(LPBYTE lpBuffer, UINT nSize);

	LPBYTE GetProcessList();
	void SendProcessList();
};

#endif // !defined(AFX_PROGRESSMANAGER_H__DDB9BCDF_DA32_4C83_B4C9_F9818EF5EDF4__INCLUDED_)
