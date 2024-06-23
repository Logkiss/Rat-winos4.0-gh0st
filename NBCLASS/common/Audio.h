// Audio.h: interface for the CAudio class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIO_H__4DD286A3_85E9_4492_A1A5_C3B2D860BD1A__INCLUDED_)
#define AFX_AUDIO_H__4DD286A3_85E9_4492_A1A5_C3B2D860BD1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#pragma comment(lib, "Winmm.lib")

class CAudio
{
public:
	CAudio();
	virtual ~CAudio();
	int m_nBufferLength;

	LPBYTE m_lpInAudioData[2]; // 保持声音的连继
	LPBYTE m_lpOutAudioData[2];

	HWAVEIN m_hWaveIn;
	int m_nWaveInIndex;
	int m_nWaveOutIndex;

	HANDLE	m_hEventWaveIn; // MM_WIM_DATA触发
	HANDLE	m_hStartRecord; // getRecordBuffer 返回后触发

	LPBYTE getRecordBuffer(LPDWORD lpdwBytes);
	bool playBuffer(LPBYTE lpWaveBuffer, DWORD dwBytes);
private:
	HANDLE	m_hThreadCallBack;

	LPWAVEHDR m_lpInAudioHdr[2];
	LPWAVEHDR m_lpOutAudioHdr[2];

	HWAVEOUT m_hWaveOut;
	
	bool	m_bIsWaveInUsed;
	bool	m_bIsWaveOutUsed;
	GSM610WAVEFORMAT m_GSMWavefmt;

	bool InitializeWaveIn();
	bool InitializeWaveOut();

	static DWORD WINAPI waveInCallBack(LPVOID lparam);
};

#endif // !defined(AFX_AUDIO_H__4DD286A3_85E9_4492_A1A5_C3B2D860BD1A__INCLUDED_)
