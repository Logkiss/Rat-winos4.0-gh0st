// BmpToAvi.cpp: implementation of the CBmpToAvi class.
//
//////////////////////////////////////////////////////////////////////

#include "..\stdafx.h"
#include "..\gh0st.h"
#include "BmpToAvi.h"
#include "..\MainFrm.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AVISTREAMINFO CBmpToAvi::m_si;

CBmpToAvi::CBmpToAvi()
{
	m_pfile = NULL;
	m_pavi = NULL;
	AVIFileInit();
}

CBmpToAvi::~CBmpToAvi()
{

	AVIFileExit();
}

bool CBmpToAvi::Open( LPCTSTR szFile, LPBITMAPINFO lpbmi )
{
	if (szFile == NULL)
		return false;
	m_nFrames = 0;

	if (AVIFileOpen(&m_pfile, szFile, OF_WRITE | OF_CREATE, NULL))
		return false;
	UINT nFrameRate =  ((CGh0stApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "FrameRate", 4);
	m_si.fccType = streamtypeVIDEO;
	m_si.fccHandler = BI_RGB;
	m_si.dwScale = 1;
	m_si.dwRate = nFrameRate; // Ã¿Ãë5Ö¡
	SetRect(&m_si.rcFrame, 0, 0, lpbmi->bmiHeader.biWidth, lpbmi->bmiHeader.biHeight);
	m_si.dwSuggestedBufferSize = lpbmi->bmiHeader.biSizeImage;
	
	
	if (AVIFileCreateStream(m_pfile, &m_pavi, &m_si))
			return false;

	
	if (AVIStreamSetFormat(m_pavi, 0, lpbmi, sizeof(BITMAPINFO)) != AVIERR_OK)
		return false;

	return true;
}

bool CBmpToAvi::Write(LPVOID lpBuffer)
{
	if (m_pfile == NULL || m_pavi == NULL)
		return false;

	return AVIStreamWrite(m_pavi, m_nFrames++, 1, lpBuffer, m_si.dwSuggestedBufferSize, AVIIF_KEYFRAME, NULL, NULL) == AVIERR_OK;	
}


void CBmpToAvi::Close()
{
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