// BmpToAvi.h: interface for the CBmpToAvi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMPTOAVI_H__0EC4F645_0647_4BD0_ABE7_235E93573CB0__INCLUDED_)
#define AFX_BMPTOAVI_H__0EC4F645_0647_4BD0_ABE7_235E93573CB0__INCLUDED_
#include <vfw.h>
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBmpToAvi  
{
public:
	CBmpToAvi();
	virtual ~CBmpToAvi();
	bool Open(HWND m_hWnd,LPCTSTR szFile, LPBITMAPINFO lpbmi,BOOL bIsWebCam = FALSE);
	bool Write(LPVOID lpBuffer);
	void Close();
private:
	PAVIFILE m_pfile;
	PAVISTREAM m_pavi;
	PAVISTREAM pCompressedStream;
	int m_nFrames;
	static AVISTREAMINFO m_si; // 这个参数需要是静态的
};

#endif // !defined(AFX_BMPTOAVI_H__0EC4F645_0647_4BD0_ABE7_235E93573CB0__INCLUDED_)
