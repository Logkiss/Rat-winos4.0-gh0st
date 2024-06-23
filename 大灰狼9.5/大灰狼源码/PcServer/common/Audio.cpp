// Audio.cpp: implementation of the CAudio class.
//
//////////////////////////////////////////////////////////////////////

#include "Audio.h"

// 引入G729包
extern "C" 
{
     #include "va_g729a.h" //QQ1746473722
}
#pragma comment(lib,"va_g729a.lib")


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
	m_nBufferLength		= SIZE_AUDIO_FRAME; // m_GSMWavefmt.wfx.nSamplesPerSec / 8(bit)
	
	m_bIsWaveInUsed		= false;
	m_bIsWaveOutUsed	= false;
	//QQ1746473722
	for (int i = 0; i < 2; i++)
	{
		m_lpInAudioData[i] = new BYTE[m_nBufferLength];
		m_lpInAudioHdr[i] = new WAVEHDR;
		
		ZeroMemory(m_lpInAudioData[i],m_nBufferLength);

		m_lpOutAudioData[i] = new BYTE[m_nBufferLength];
		m_lpOutAudioHdr[i] = new WAVEHDR;
		ZeroMemory(m_lpOutAudioData[i],m_nBufferLength);
	}
	m_SelectedDevice = 0;
	//QQ1746473722
	va_g729a_init_encoder();
	va_g729a_init_decoder();
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
	{
		return NULL;
	}
	if (lpdwBytes == NULL)
	{
		return NULL;
	}
	SetEvent(m_hStartRecord);
	WaitForSingleObject(m_hEventWaveIn, INFINITE);
	*lpdwBytes = m_nBufferLength;
	return	m_lpInAudioData[m_nWaveInIndex];
}

bool CAudio::playBuffer(LPBYTE lpWaveBuffer, DWORD dwBytes)
{
	if (!m_bIsWaveOutUsed && !InitializeWaveOut())
		return NULL;
	
	
	for (DWORD i = 0; i < dwBytes; i += m_nBufferLength)
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
	
    WAVEFORMATEX waveform; //采集音频的格式，结构体
	SetDeviceType(&waveform);
	
	MMRESULT	mmResult;
	DWORD		dwThreadID = 0;
	m_hThreadCallBack = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)waveInCallBack, (LPVOID)this, CREATE_SUSPENDED, &dwThreadID);
	// UINT_PTR uDeviceID 乃所用波形输入设备之ID。若此为 WAVE_MAPPER，系统则会自动寻找合适设备
	mmResult = waveInOpen(&m_hWaveIn, m_SelectedDevice, &waveform, (LONG)dwThreadID, (LONG)0, CALLBACK_THREAD);
	
	if (mmResult != MMSYSERR_NOERROR)
	{
		return false;
	}
	for (int i = 0; i < 2; i++)
	{
		m_lpInAudioHdr[i]->lpData = (LPSTR)m_lpInAudioData[i];
		m_lpInAudioHdr[i]->dwBufferLength = m_nBufferLength;
		m_lpInAudioHdr[i]->dwFlags = 0;
		m_lpInAudioHdr[i]->dwLoops = 0;
		//波形声音输入设备准备一个缓冲区
		waveInPrepareHeader(m_hWaveIn, m_lpInAudioHdr[i], sizeof(WAVEHDR));
	}
	//向波形声音输入设备添加缓冲区
	waveInAddBuffer(m_hWaveIn, m_lpInAudioHdr[m_nWaveInIndex], sizeof(WAVEHDR));

	ResumeThread(m_hThreadCallBack);
	//开始录音
	waveInStart(m_hWaveIn);
	
	m_bIsWaveInUsed = true;
	
	return true;
	
}

int FindMixernIndex(char *szPname)
{
	MIXERCAPS mxcaps        = {0};
    HMIXER hMixer            = NULL;
	
    for ( UINT i = 0; i < mixerGetNumDevs (); i++ )
	{
        if ( MMSYSERR_NOERROR != mixerGetDevCaps ( i, &mxcaps, sizeof ( MIXERCAPS ) ) ){ continue; }   
        if ( MMSYSERR_NOERROR != mixerOpen ( &hMixer, i, NULL, NULL, 0 ) ){ continue; }
		
        MIXERLINE mxl            = { sizeof ( MIXERLINE ), 0, 0, 0, 0, 0, MIXERLINE_COMPONENTTYPE_DST_WAVEIN, 0};
        if ( MMSYSERR_NOERROR != mixerGetLineInfo ( (HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE ) ){ 
            mixerClose ( hMixer );
            continue; 
        }
		
		if (strcmp(mxcaps.szPname,szPname) == 0)
		{
			mixerClose ( hMixer );

			return i;
		}
	}
	return -1;
}

bool CAudio::ActiveMixerInputChannel(char * szMixerDevice, int nMixerLineIndex)
{
	int nMixerDeviceIndex = FindMixernIndex(szMixerDevice);
	if (nMixerDeviceIndex == -1)
		return false;
    bool result                = false;
    HMIXER hMixer            = NULL;
    MIXERCONTROL mxc        = { sizeof ( MIXERCONTROL ), 0 }; 
    MIXERLINE mxl            = { sizeof ( MIXERLINE ), 0, 0, 0, 0, 0, MIXERLINE_COMPONENTTYPE_DST_WAVEIN, 0};
    if ( MMSYSERR_NOERROR != mixerOpen ( &hMixer, nMixerDeviceIndex, NULL, NULL, 0 ) )
	 return false; 

    if ( MMSYSERR_NOERROR != mixerGetLineInfo ( (HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE ) )
	{ 
        mixerClose ( hMixer ); 
        return false; 
    }

    MIXERLINECONTROLS mxlc    = { sizeof ( MIXERLINECONTROLS ), mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_MIXER, 1, mxc.cbStruct, &mxc }; 
    mxl.dwSource            = nMixerLineIndex;
    if ( MMSYSERR_NOERROR != mixerGetLineInfo ( (HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_DESTINATION | MIXER_GETLINEINFOF_SOURCE ) )
	{ 
        mixerClose ( hMixer ); 
        return false; 
    }
    if ( MMSYSERR_NOERROR != mixerGetLineControls ( (HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE ) )
	{ 
        mxlc.dwControlType    = MIXERCONTROL_CONTROLTYPE_MUX;
        if ( MMSYSERR_NOERROR != mixerGetLineControls ( (HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE ) )
		{ 
            mixerClose ( hMixer ); 
            return false; 
        }
    }
    if ( mxc.cMultipleItems == 0 )
	{ 
		mixerClose ( hMixer ); 
		return false; 
	}
    PMIXERCONTROLDETAILS_BOOLEAN mxcdbls    = (PMIXERCONTROLDETAILS_BOOLEAN)malloc ( sizeof ( MIXERCONTROLDETAILS_BOOLEAN ) * mxc.cMultipleItems );
    PMIXERCONTROLDETAILS_LISTTEXT mxcdlts    = (PMIXERCONTROLDETAILS_LISTTEXT)malloc ( sizeof ( MIXERCONTROLDETAILS_LISTTEXT ) * mxc.cMultipleItems );
    MIXERCONTROLDETAILS mxcd                = { sizeof ( MIXERCONTROLDETAILS ), mxc.dwControlID, 1, (HWND)mxc.cMultipleItems, sizeof ( MIXERCONTROLDETAILS_LISTTEXT ), mxcdlts };       
    if ( MMSYSERR_NOERROR == mixerGetControlDetails ( (HMIXEROBJ)hMixer, &mxcd, MIXER_GETCONTROLDETAILSF_LISTTEXT ) )
	{ 
        memset ( mxcdbls, 0, sizeof ( MIXERCONTROLDETAILS_BOOLEAN ) * mxc.cMultipleItems );
        for ( DWORD i = 0; i < mxc.cMultipleItems; i++ )
		{ 
            if ( mxcdlts[i].dwParam1 == mxl.dwLineID )
			{
                mxcdbls[i].fValue        = 1;   
                mxcd.paDetails            = mxcdbls;
                mxcd.cbDetails            = sizeof ( MIXERCONTROLDETAILS_BOOLEAN );
                result    = (MMSYSERR_NOERROR == mixerSetControlDetails ( (HMIXEROBJ)hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE ) ); break;
            }
        }
    }
	
    free ( mxcdbls );
    free ( mxcdlts ); 
    mixerClose ( hMixer );
	
    return result;
}

int CAudio::EnumerateInputLines( char * szPname,  char * str )
{
	if(str == NULL ||szPname == NULL )
		return -1;

	MIXERCAPS mxcaps        = {0};
    HMIXER hMixer            = NULL;
	int nRet  = -1;
	
    for ( UINT i = 0; i < mixerGetNumDevs (); i++ )
	{
        if ( MMSYSERR_NOERROR != mixerGetDevCaps ( i, &mxcaps, sizeof ( MIXERCAPS ) ) )
			continue;    
        if ( MMSYSERR_NOERROR != mixerOpen ( &hMixer, i, NULL, NULL, 0 ) )
		    continue; 
		
        MIXERLINE mxl            = { sizeof ( MIXERLINE ), 0, 0, 0, 0, 0, MIXERLINE_COMPONENTTYPE_DST_WAVEIN, 0};
        if ( MMSYSERR_NOERROR != mixerGetLineInfo ( (HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE ) )
		{ 
            mixerClose ( hMixer );
            continue; 
        }

		if (strcmp(mxcaps.szPname,szPname) != 0)
		{
			mixerClose ( hMixer );
			continue;
		}
		
        MIXERCONTROL mxc        = { sizeof ( MIXERCONTROL ), 0 }; 
        MIXERLINECONTROLS mxlc    = { sizeof ( MIXERLINECONTROLS ), mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_MIXER, 1, mxc.cbStruct, &mxc }; 
        if ( MMSYSERR_NOERROR != mixerGetLineControls ( (HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE ) )
		{ 
            mxlc.dwControlType    = MIXERCONTROL_CONTROLTYPE_MUX;
            if ( MMSYSERR_NOERROR != mixerGetLineControls ( (HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE ) )
			{ 
                mixerClose ( hMixer );
                continue; 
            }
        }
        if ( mxc.cMultipleItems == 0 )
		{
            mixerClose ( hMixer );
            continue;
        }
		
        MIXERCONTROLDETAILS mxcd    = { sizeof ( MIXERCONTROLDETAILS ), mxc.dwControlID, 1, 0 };       
        PMIXERCONTROLDETAILS_LISTTEXT mxcdlts    = (PMIXERCONTROLDETAILS_LISTTEXT)malloc ( sizeof ( MIXERCONTROLDETAILS_LISTTEXT ) * mxc.cMultipleItems );
        mxcd.cbDetails            = sizeof ( MIXERCONTROLDETAILS_LISTTEXT );
        mxcd.cMultipleItems        = mxc.cMultipleItems;
        mxcd.paDetails            = mxcdlts;
        if ( MMSYSERR_NOERROR != mixerGetControlDetails ( (HMIXEROBJ)hMixer, &mxcd, MIXER_GETCONTROLDETAILSF_LISTTEXT ) )
		{ 
            free ( mxcdlts );
            mixerClose ( hMixer );
            continue; 
        }
		
        PMIXERCONTROLDETAILS_BOOLEAN mxcdbls    = (PMIXERCONTROLDETAILS_BOOLEAN)malloc ( sizeof ( MIXERCONTROLDETAILS_BOOLEAN ) * mxc.cMultipleItems );
        mxcd.cbDetails            = sizeof ( MIXERCONTROLDETAILS_BOOLEAN );
        mxcd.cMultipleItems        = mxc.cMultipleItems;
        mxcd.paDetails            = mxcdbls;
        if ( MMSYSERR_NOERROR != mixerGetControlDetails ( (HMIXEROBJ)hMixer, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE ) )
		{ 
            free ( mxcdbls );
            mixerClose ( hMixer );
            continue; 
        }
		
        DWORD dwConnections        = mxl.cConnections;
        DWORD dwDestination        = mxl.dwDestination;
		
        for ( DWORD j = 0; j < dwConnections; j++ )
		{ 
            mxl.dwDestination    = dwDestination;
            mxl.dwSource        = j;
            if ( MMSYSERR_NOERROR != mixerGetLineInfo ( (HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_DESTINATION | MIXER_GETLINEINFOF_SOURCE ) ){ continue; }
            for ( DWORD k = 0; k < mxc.cMultipleItems; k++ )
			{
                if ( mxcdlts[k].dwParam1 == mxl.dwLineID )
				{
					strcat(str,mxl.szName);
					strcat(str,"@");

					if (mxcdbls[k].fValue)
						nRet = j;
					
                    break;
                }
            }
        }
		
        free ( mxcdbls );
        free ( mxcdlts ); 
        mixerClose ( hMixer );
    }
	return nRet;
}


void CAudio::SetDeviceType(WAVEFORMATEX* pwfe)
{
	memset( pwfe, 0, sizeof(WAVEFORMATEX) );
	pwfe->wFormatTag = WAVE_FORMAT_PCM;//声音格式为PCM
	pwfe->nSamplesPerSec = 8000;//采样率，16000次/秒
	pwfe->wBitsPerSample = 16;//采样比特，16bits/次
	pwfe->nChannels = 1;//采样声道数，2声道
	pwfe->nBlockAlign = pwfe->wBitsPerSample*pwfe->nChannels/8;//一个块的大小，采样bit的字节数乘以声道数
	pwfe->nAvgBytesPerSec = pwfe->nBlockAlign*pwfe->nSamplesPerSec ;//每秒的数据率，就是每秒能采集多少字节的数据
    pwfe->cbSize = 0;//一般为0}
}

bool CAudio::InitializeWaveOut()
{
	if (!waveOutGetNumDevs())
		return false;
	
	for (int i = 0; i < 2; i++)
		memset(m_lpOutAudioData[i], 0, m_nBufferLength);
	
    WAVEFORMATEX waveform; //采集音频的格式，结构体
	SetDeviceType(&waveform);
	
	MMRESULT	mmResult;
	mmResult = waveOutOpen(&m_hWaveOut, (WORD)WAVE_MAPPER, &waveform, (LONG)0, (LONG)0, CALLBACK_NULL);
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
			{
				return -1;		
			}
		}
		// Why never happend this
		if (Msg.message == MM_WIM_CLOSE)
			break;
		
		TranslateMessage(&Msg); 
		DispatchMessage(&Msg);
	}
	
	return 0;	
}

////////////////////////////////////////////////////////////////////////// 
// 音频压缩  解压缩部分

int CAudio::Compress(char *pBuf, char *pDstBuf, int nLen)
{
	short* pSpBuf = (short*)pBuf;
	BYTE*   pBDstBuf = (BYTE*)pDstBuf;
	int nRet=0;
	nLen/=sizeof(short);
	while(nLen>0)
	{
		va_g729a_encoder(pSpBuf,pBDstBuf);
		
		pSpBuf+=L_FRAME;
		nLen-=L_FRAME;
		nRet+=L_FRAME_COMPRESSED;
		pBDstBuf+=L_FRAME_COMPRESSED;
	}
	return nRet;
}

int CAudio::DeCompress(char *pBuf, char *pDstBuf, int nLen)
{
	int bfi = 0;
	BYTE* pSpBuf = (BYTE*)pBuf;
	short*   pBDstBuf = (short*)pDstBuf;
	
	int nRet=0;
	while(nLen>0)
	{
		va_g729a_decoder(pSpBuf,pBDstBuf,bfi);
		
		pSpBuf+=L_FRAME_COMPRESSED;
		nLen-=L_FRAME_COMPRESSED;
		nRet+=L_FRAME;
		pBDstBuf+=L_FRAME;
	}
	return nRet*sizeof(short);
}
