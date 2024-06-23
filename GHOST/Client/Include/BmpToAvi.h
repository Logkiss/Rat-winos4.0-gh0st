// BmpToAvi.h: interface for the CBmpToAvi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMPTOAVI_H__002A4942_72F6_4CD9_B035_55492220CE74__INCLUDED_)
#define AFX_BMPTOAVI_H__002A4942_72F6_4CD9_B035_55492220CE74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <windows.h>
#include <vfw.h>
#pragma comment(lib, "vfw32.lib")
class CBmpToAvi
{
public:
	CBmpToAvi();
	virtual ~CBmpToAvi();
	bool Open(LPCTSTR szFile, LPBITMAPINFO lpbmi);
	bool Write(LPVOID lpBuffer);
	void Close();
private:
	PAVIFILE m_pfile;
	PAVISTREAM m_pavi;
	int m_nFrames;
	static AVISTREAMINFO m_si; // 这个参数需要是静态的
};

#endif // !defined(AFX_BMPTOAVI_H__002A4942_72F6_4CD9_B035_55492220CE74__INCLUDED_)
