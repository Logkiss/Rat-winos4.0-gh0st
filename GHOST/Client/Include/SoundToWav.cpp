// SoundToWav.cpp: implementation of the CSoundToWav class.
//
//////////////////////////////////////////////////////////////////////

#include "..\stdafx.h"
#include "..\Client.h"
#include "SoundToWav.h"
#include <ASSERT.H>
#include <AVIRIFF.H>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const DWORD WAVE_FILE_HEAD_SIZE = sizeof(RIFFLIST) + sizeof(RIFFCHUNK) + sizeof(WAVEFORMATEX) + sizeof(RIFFCHUNK);
extern DWORD g_dwLastError;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoundToWav::CSoundToWav()
{
	m_hFile=INVALID_HANDLE_VALUE;
	
	m_dwTotalBytes = 0;
	ZeroMemory(&m_wfx,sizeof(WAVEFORMATEX));

	m_wfx.wFormatTag = WAVE_FORMAT_PCM;//声音格式为PCM
	m_wfx.nSamplesPerSec = 8000;//采样率，16000次/秒
	m_wfx.wBitsPerSample = 16;//采样比特，16bits/次
	m_wfx.nChannels = 1;//采样声道数，2声道
	m_wfx.nBlockAlign = m_wfx.wBitsPerSample*m_wfx.nChannels/8;//一个块的大小，采样bit的字节数乘以声道数
	m_wfx.nAvgBytesPerSec = m_wfx.nBlockAlign*m_wfx.nSamplesPerSec ;//每秒的数据率，就是每秒能采集多少字节的数据
    m_wfx.cbSize = 0;//一般为0
}

CSoundToWav::~CSoundToWav()
{
	if (m_hFile!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
	}
}

void  CSoundToWav::SetWaveFormat(WAVEFORMATEX *pwfx)
{
	CopyMemory(&m_wfx,pwfx,sizeof(WAVEFORMATEX));
}

DWORD CSoundToWav::WriteWavFileHeader()
{
	
	BYTE Buffer[WAVE_FILE_HEAD_SIZE];
	RIFFLIST  *pRiffWave = (RIFFLIST *)Buffer;
	RIFFCHUNK *pRiffFmt  = (RIFFCHUNK *)(pRiffWave + 1);
	RIFFCHUNK *pRiffData = (RIFFCHUNK *)(((BYTE *)(pRiffFmt + 1)) + sizeof(WAVEFORMATEX));
	
	
	pRiffData->fcc = FCC('data');
	pRiffData->cb = m_dwTotalBytes;
	
	pRiffFmt->fcc = FCC('fmt ');
	pRiffFmt->cb = sizeof(WAVEFORMATEX);
	
	CopyMemory(pRiffFmt + 1, &m_wfx, sizeof(WAVEFORMATEX));
	
	pRiffWave->fcc = FCC('RIFF');
	pRiffWave->cb = m_dwTotalBytes + WAVE_FILE_HEAD_SIZE - sizeof(RIFFCHUNK);
	pRiffWave->fccListType = FCC('WAVE');
	
	SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
	
	DWORD dwWriten = 0;
	BOOL bRet = WriteFile(m_hFile, Buffer, WAVE_FILE_HEAD_SIZE, &dwWriten, NULL);
	if ((bRet == FALSE) || (dwWriten != WAVE_FILE_HEAD_SIZE))
	{
		return -1;
	}
	return 0;
}

DWORD CSoundToWav::WriteWavData(LPBYTE pData,DWORD nSize)
{
	DWORD nByteWrite =  Write(pData,nSize);
	m_dwTotalBytes+=nByteWrite;
	return nByteWrite;
}

BOOL CSoundToWav::close( )
{
	if (m_hFile!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
	}
	return TRUE;
}

BOOL CSoundToWav::Open(LPCTSTR lpFileName,DWORD dwDesiredAccess)
{
	m_hFile = CreateFile(lpFileName,dwDesiredAccess,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	return m_hFile!=INVALID_HANDLE_VALUE ? TRUE:FALSE;
}

DWORD CSoundToWav::SeekToBegin( )
{
	assert(m_hFile!=INVALID_HANDLE_VALUE);
	return SetFilePointer(m_hFile,0,NULL,FILE_BEGIN);
}

DWORD CSoundToWav::SeekToEnd( )
{
	assert(m_hFile!=INVALID_HANDLE_VALUE);
	return SetFilePointer(m_hFile,0,NULL,FILE_END);
}

DWORD CSoundToWav::Write(LPVOID lpBuffer,DWORD nNumberOfBytesToWrite)
{
	assert(m_hFile!=INVALID_HANDLE_VALUE);
	DWORD dwNumberOfBytesWrite; 
	WriteFile(m_hFile,lpBuffer,nNumberOfBytesToWrite,&dwNumberOfBytesWrite,NULL);
	return dwNumberOfBytesWrite;
}
