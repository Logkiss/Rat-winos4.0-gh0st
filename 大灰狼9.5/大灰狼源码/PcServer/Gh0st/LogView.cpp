// LogView.cpp : implementation file
//

#include "stdafx.h"
#include "Gh0st.h"
#include "LogView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLogView* g_pLogView;

typedef struct
{
	char	*title;
	int		nWidth;
}COLUMNSTRUCT;

COLUMNSTRUCT g_Log_Data[] = 
{
	{"发生时间",	    150	},
	{"事件记录",	    650	}
};

int g_Log_Width = 0;
int	g_Log_Count = (sizeof(g_Log_Data) / 8);
BOOL gLogUpdate = FALSE;
/////////////////////////////////////////////////////////////////////////////
// CLogView

IMPLEMENT_DYNCREATE(CLogView, CListView)

CLogView::CLogView()
{
	g_pLogView = this;
}

CLogView::~CLogView()
{
}


BEGIN_MESSAGE_MAP(CLogView, CListView)
	//{{AFX_MSG_MAP(CLogView)
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND(IDM_EVENT_DELETE, OnEventDelete)
	ON_COMMAND(IDM_ALL_DELETE, OnAllDelete)
	ON_COMMAND(IDM_EVENT_SAVE, OnEventSave)
	ON_COMMAND(IDM_EVENT_COPY, OnEventCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogView drawing

void CLogView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CLogView diagnostics

#ifdef _DEBUG
void CLogView::AssertValid() const
{
	CListView::AssertValid();
}

void CLogView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLogView message handlers

void CLogView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class

	m_pLogList = &GetListCtrl();
	m_pLogList->SetRedraw(FALSE);
	

		I_LogList.Create(16, 16, ILC_COLOR32|ILC_MASK,10, 0);
		HICON hIcon = NULL;//以下为加入3个图标资源
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_88), IMAGE_ICON, 16, 16, 0);
		I_LogList.Add(hIcon);
		DestroyIcon(hIcon); 
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_99), IMAGE_ICON, 16, 16, 0);
		I_LogList.Add(hIcon);
		DestroyIcon(hIcon); 
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_77), IMAGE_ICON, 16, 16, 0);
		I_LogList.Add(hIcon);
		DestroyIcon(hIcon); 
	
		ListView_SetImageList(m_pLogList->m_hWnd, I_LogList, LVSIL_SMALL);
	
	
	m_pLogList->SetExtendedStyle(/*LVIF_PARAM |*/ LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP|/*LVS_EX_FLATSB|*/
		LVS_EX_ONECLICKACTIVATE | /*LVS_EX_UNDERLINEHOT |*/LVS_EX_SUBITEMIMAGES /*|LVS_EX_GRIDLINES*/);
	
	for (int i = 0; i < g_Log_Count; i++)
	{	
		m_pLogList->InsertColumn(i, g_Log_Data[i].title);
//		m_pLogList->SetColumnWidth(i, g_Log_Data[i].nWidth);
		g_Log_Width += g_Log_Data[i].nWidth; // 总宽度
	}
	gLogUpdate = TRUE;

	HWND hWndHeader = m_pLogList->GetDlgItem(0)->GetSafeHwnd();
	m_heades.SubclassWindow(hWndHeader);
	m_heades.SetTheme(new CXTHeaderCtrlThemeOffice2003());
	m_pLogList->SetRedraw(TRUE);

	//主要调用OnSize函数
	CRect rect ;
	this->GetWindowRect( rect );
	ScreenToClient( rect ) ;
	this->OnSize(SIZE_RESTORED,rect.Width(),rect.Height());
}

BOOL CLogView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |=  LVS_REPORT;
	return CListView::PreCreateWindow(cs);
}

void CLogView::OnSize(UINT nType, int cx, int cy) 
{
	CListView::OnSize(nType, cx, cy);
/*
	if(gLogUpdate)
	{
		m_pLogList->LockWindowUpdate();
		double dcx=(double)(cx-5)/g_Log_Width; 
		if (m_pLogList != NULL)
		{
			for(int i=0;i<g_Log_Count;i++){           //遍历每一个列
				double dd=g_Log_Data[i].nWidth;          //得到当前列的宽度
				dd = dd*dcx;
				m_pLogList->SetColumnWidth(i,(int)dd);       //设置当前的宽度
			}	
		}
		m_pLogList->UnlockWindowUpdate();
	}
	*/

	if(gLogUpdate)
	{
		double dcx=cx-5;     //对话框的总宽度  g_Column_cx
		if (m_pLogList != NULL)
		{
			for(int i=0;i<g_Log_Count;i++){                   //遍历每一个列
				double dd=g_Log_Data[i].nWidth;               //得到当前列的宽度
				dd/=g_Log_Width;                              //看一看当前宽度占总长度的几分之几
				dd*=dcx;                                         //用原来的长度乘以所占的几分之几得到当前的宽度
				m_pLogList->SetColumnWidth(i,(int)dd);          //设置当前的宽度
			}

		}
	}

}

void CLogView::g_ColumnWidth()
{
	
}

void CLogView::InsertLogItem(LPCTSTR Text,int Mode, int Flag)
{
	char m_Text[512] = {0};
	CTime time = CTime::GetCurrentTime();		//构造CTime对象 

	CString strTime = time.Format(" %Y-%m-%d %H:%M:%S");
	m_pLogList->InsertItem(0, strTime, Flag);//int InsertItem( int nItem, LPCTSTR lpszItem, int nImage );
	m_pLogList->SetItemText(0, 1, Text);  //BOOL SetItemText( int nItem, int nSubItem, LPTSTR lpszText );
	switch(Mode)
	{
	    case 0:
		{
			m_pLogList->SetTextColor(RGB(0,100,250));//蓝色
		}
		break;	
		case 1:
		{
			m_pLogList->SetTextColor(RGB(255,0,0));//红色
		}
		break;
		case 2:
		{
			m_pLogList->SetTextColor(RGB(0,180,250));//蓝绿色
		}
		break;
		case 3:
		{
			m_pLogList->SetTextColor(RGB(200,0,200));//粉色
		}
		break;
		case 4:
		{
			m_pLogList->SetTextColor(RGB(0,20,100));//貌似黑色
		}
		break;
		
	default:
		m_pLogList->SetTextColor(RGB(0,100,255));//蓝色
		break;
	}
}

void CLogView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CMenu	popup;
	popup.LoadMenu(IDR_LOG);
	CMenu*	pM = popup.GetSubMenu(0);
	CPoint	p;
	GetCursorPos(&p);
//	pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);
	CXTPCommandBars::TrackPopupMenu(pM,TPM_RIGHTBUTTON,p.x,p.y,this);

}

void CLogView::OnEventDelete() 
{
	// TODO: Add your command handler code here
	POSITION pos = m_pLogList->GetFirstSelectedItemPosition();
	if  (pos  ==  NULL)
	{
		::MessageBox(NULL,"请先选择要删除的事件记录 ...","提示",MB_ICONINFORMATION);
		return;
	}
	else
	{
		while (pos)
		{
			int nItem = m_pLogList->GetNextSelectedItem(pos);
			m_pLogList->DeleteItem(nItem);
			pos = m_pLogList->GetFirstSelectedItemPosition();
		}
	}
}

void CLogView::OnAllDelete() 
{
	// TODO: Add your command handler code here
	m_pLogList->DeleteAllItems();
}

void CLogView::OnEventSave() 
{
	// TODO: Add your command handler code here
	POSITION pos = m_pLogList->GetFirstSelectedItemPosition();
	if  (pos  ==  NULL)
	{
		::MessageBox(NULL,"请先选择要保存的事件记录 ...","提示",MB_ICONINFORMATION);
		return;
	}
	else
	{
		CTime time = CTime::GetCurrentTime(); ///构造CTime对象 
		CString strTime = time.Format("%Y-%m-%d %H-%M-%S");
		
		CFileDialog dlg(FALSE, _T("log"), strTime, OFN_OVERWRITEPROMPT, _T("*.log|*.log|*.txt|*.log"), NULL);
		if (dlg.DoModal() != IDOK)
			return;
		CFile file;
		char strip[50] = {0};
		if (file.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite))
		{
			while (pos)
			{
				int nItem = m_pLogList->GetNextSelectedItem(pos);
				CString strTitle;
				strTitle.Format(_T("时间:%s 事件:%s"), m_pLogList->GetItemText(nItem, 0),m_pLogList->GetItemText(nItem, 1));
				char *strip = strTitle.GetBuffer(strTitle.GetLength() + 1);	//CString转换为char*
				strTitle.ReleaseBuffer();
				strcat(strip, "\r\n");
				file.Write(strip, strlen(strip));
				memset(strip, 0, 50);
			}
			file.Close();
			strTime = time.Format("[%Y-%m-%d %H:%M:%S]") + " 日志导出成功 ...";
			::MessageBox(0,strTime,"提示",MB_ICONINFORMATION);
	  }
	}
}

void CLogView::OnEventCopy() 
{
	// TODO: Add your command handler code here
	int nItem;
    CString strText(_T(""));
    POSITION pos = m_pLogList->GetFirstSelectedItemPosition();
	if  (pos  ==  NULL)
	{
		::MessageBox(NULL,"请先选择要复制的事件记录 ...","提示",MB_ICONINFORMATION);
		return;
	}
	else
	{
		//获取所有选中项目的内容。
		while (pos)
		{
			nItem = m_pLogList->GetNextSelectedItem(pos);
			strText += m_pLogList->GetItemText(nItem, 0) + " ";
			strText += m_pLogList->GetItemText(nItem, 1) + _T("\r\n");
		}
		//将内容保存到剪贴板。
		if (!strText.IsEmpty())
		{
			if (OpenClipboard())
			{
				EmptyClipboard();
				HGLOBAL hClipboardData = GlobalAlloc(GHND | GMEM_SHARE, (strText.GetLength() + 1) * sizeof(TCHAR));
				if (hClipboardData)
				{
					TCHAR* pszData = (TCHAR *)GlobalLock(hClipboardData);
					_tcscpy(pszData, strText);
					GlobalUnlock(hClipboardData);
					SetClipboardData(CF_TEXT, hClipboardData);
				}
				CloseClipboard();
			}
		}
	}
}

