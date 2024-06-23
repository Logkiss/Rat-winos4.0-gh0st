// BmpToAvi.cpp: implementation of the CBmpToAvi class.
//
//////////////////////////////////////////////////////////////////////

#include "..\stdafx.h"
#include "..\Client.h"
#include "..\MainFrm.h"
#include "BmpToAvi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AVISTREAMINFO CBmpToAvi::m_si;

CBmpToAvi::CBmpToAvi()
{
	m_pfile = NULL;
	m_pavi = NULL;
	pCompressedStream =NULL;
	AVIFileInit();

}

CBmpToAvi::~CBmpToAvi()
{
	AVIFileExit();
}

bool CBmpToAvi::Open( HWND m_hWnd, LPCTSTR szFile, LPBITMAPINFO lpbmi,BOOL bIsWebCam )
{

	if (szFile == NULL)
		return false;

	m_nFrames = 0;
	
	if (AVIFileOpen(&m_pfile, szFile, OF_WRITE | OF_CREATE, NULL))
		return false;


	m_si.fccType = streamtypeVIDEO;
	m_si.fccHandler = BI_RGB;
	m_si.dwScale = 1;
	m_si.dwRate = 4; // Ã¿Ãë4Ö¡

	SetRect(&m_si.rcFrame, 0, 0, lpbmi->bmiHeader.biWidth, lpbmi->bmiHeader.biHeight);
	m_si.dwSuggestedBufferSize = lpbmi->bmiHeader.biSizeImage;
	

	if (AVIFileCreateStream(m_pfile, &m_pavi, &m_si))
		return false;


	AVICOMPRESSOPTIONS  options; 
	AVICOMPRESSOPTIONS * aoptions[1] = {&options};
    memset(&options,0,sizeof(options));
	options.fccType = ICTYPE_VIDEO ;
	options.fccHandler = bIsWebCam ? 1668707181 : 1701605997; 
	options.dwQuality = 1*100;
	options.dwFlags = AVICOMPRESSF_KEYFRAMES;
	options.lpParms = 0;
	options.cbParms = 2056 ;
	options.dwKeyFrameEvery   = 15;
    AVIMakeCompressedStream(&pCompressedStream,m_pavi,aoptions[0],NULL);

	if (AVIStreamSetFormat(pCompressedStream, 0, lpbmi, sizeof(BITMAPINFO)) != AVIERR_OK)
		return false;


	return true;
}

bool CBmpToAvi::Write(LPVOID lpBuffer)
{
	if (m_pfile == NULL || pCompressedStream == NULL)
		return false;
	
	return AVIStreamWrite(pCompressedStream, m_nFrames++, 1, lpBuffer, m_si.dwSuggestedBufferSize, AVIIF_KEYFRAME, NULL, NULL) == AVIERR_OK;	
}


void CBmpToAvi::Close()
{
	if (pCompressedStream)
	{
		AVIStreamRelease(pCompressedStream);
		pCompressedStream = NULL;
	}

	if (m_pavi)
	{
		AVIStreamRelease(m_pavi);
		m_pavi = NULL;
	}
	if (m_pfile)
	{
		AVIFileRelease(m_pfile);
		m_pfile = NULL;
	}		
}