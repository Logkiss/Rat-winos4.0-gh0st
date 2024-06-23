// VideoCap.h: interface for the CVideoCap class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_VIDEOCAP_H__5D857DCE_D889_45B0_8A98_33E1DF64CDE3__INCLUDED_)
#define AFX_VIDEOCAP_H__5D857DCE_D889_45B0_8A98_33E1DF64CDE3__INCLUDED_
#include <windows.h>
#include <vfw.h>
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVideoCap
{
public:
	HANDLE	m_hCaptureEvent;
	LPBITMAPINFO m_lpbmi;
	static bool IsWebCam();
	bool Initialize(int nWidth = 0, int nHeight = 0);
	LPBYTE GetDIB();
	static LRESULT CALLBACK capErrorCallback(HWND hWnd,	int nID, LPCSTR lpsz);
	static LRESULT CALLBACK FrameCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr);
	CVideoCap();
	virtual ~CVideoCap();
private:
	LPBYTE m_lpDIB;
	HWND m_hWnd;
	HWND m_hWndCap;
	static bool m_bIsConnected;
};

#endif // !defined(AFX_VIDEOCAP_H__5D857DCE_D889_45B0_8A98_33E1DF64CDE3__INCLUDED_)
