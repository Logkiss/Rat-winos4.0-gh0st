// TabView.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "TabView.h"
#include "ClientView.h"
#include "InputDlg.h"
#include "Play.h"
#include "LogView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_TABCONTROL 100

CTabView* g_pTabView;
extern ClientView* g_pConnectView;
CXTPTabManagerItem* pRightItem;

/////////////////////////////////////////////////////////////////////////////
// CTabView

IMPLEMENT_DYNCREATE(CTabView, CView)

CTabView::CTabView()
{
	g_pTabView = this;
}

CTabView::~CTabView()
{
}


BEGIN_MESSAGE_MAP(CTabView, CView)
	//{{AFX_MSG_MAP(CTabView)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_AddGroup, OnAddGroup)
	ON_COMMAND(ID_DelGroup, OnDelGroup)
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABCONTROL, OnSelectedChanged)
	ON_WM_MOUSEACTIVATE()
	ON_MESSAGE(WM_ADDFINDGROUP, OnAddFindGroup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabView drawing

void CTabView::OnDraw(CDC* pDC)
{
	CClientDoc* pDoc = GetDocument();
	// TODO: add draw code here
}

BOOL CTabView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
/*	cs.style |= WS_CLIPCHILDREN;*/
	cs.style &=~WS_BORDER;
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTabView diagnostics

#ifdef _DEBUG
void CTabView::AssertValid() const
{
	CView::AssertValid();
}

void CTabView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CClientDoc* CTabView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CClientDoc)));
	return (CClientDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTabView message handlers

void CTabView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (m_wndTabControl.GetSafeHwnd())
	{
		m_wndTabControl.MoveWindow(0, 0, cx, cy);
	}
}

void CTabView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CFrameWnd* pFrame = GetParentFrame();
	CView* pView = DYNAMIC_DOWNCAST(CView, CWnd::FromHandle(m_wndTabControl.GetSelectedItem()->GetHandle()));
	ASSERT_KINDOF(CView, pView);
	pView->RedrawWindow(NULL);
	// Do not call CView::OnPaint() for painting messages
}

BOOL CTabView::AddView(CRuntimeClass* pViewClass, LPCTSTR lpszTitle)
{
	CCreateContext contextT;
	contextT.m_pCurrentDoc     = GetDocument();
	contextT.m_pNewViewClass   = pViewClass;
	contextT.m_pNewDocTemplate = GetDocument()->GetDocTemplate();
	
	CWnd* pWnd;
	TRY
	{
		pWnd = (CWnd*)pViewClass->CreateObject();
		if (pWnd == NULL)
		{
			AfxThrowMemoryException();
		}
	}
	CATCH_ALL(e)
	{
		TRACE0( "Out of memory creating a view.\n" );
		// Note: DELETE_EXCEPTION(e) not required
		return FALSE;
	}
	END_CATCH_ALL
		
	DWORD dwStyle = AFX_WS_DEFAULT_VIEW;
	dwStyle &= ~WS_BORDER;
	
	int nTab = m_wndTabControl.GetItemCount();
	
	// Create with the right size (wrong position)
	CRect rect(0, 0, 0, 0);
	if (!pWnd->Create(NULL, NULL, dwStyle,
		rect, &m_wndTabControl, (AFX_IDW_PANE_FIRST + nTab), &contextT))
	{
		TRACE0( "Warning: couldn't create client tab for view.\n" );
		// pWnd will be cleaned up by PostNcDestroy
		return NULL;
	}
	m_wndTabControl.InsertItem(nTab, lpszTitle, pWnd->GetSafeHwnd());
	pWnd->SetOwner(this);
	if (nTab != 0)
	{
		CPcView* pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(m_wndTabControl.GetItem(nTab)->GetHandle()));
		pView->NewInitialUpdate();
		pView->m_iocpServer = g_pConnectView->m_iocpServer;
	}
	return TRUE;
	
}

LRESULT CTabView::OnAddFindGroup(WPARAM wParam, LPARAM lParam)
{	
	ClientContext	*pContext = (ClientContext *)lParam;
	if (pContext == NULL)
	{	
		return -1;
	}
	try
	{	
		// 不合法的数据包
		if (pContext->m_DeCompressionBuffer.GetBufferLen() != sizeof(LOGININFO))
		{	
			return -1;
		}
		LOGININFO*	LoginInfo = (LOGININFO*)pContext->m_DeCompressionBuffer.GetBuffer();
		
		BOOL bFind = false;
		CString strGroupName, strTemp, strLoginGroup;
		CClientView* pView = NULL;
		if (lstrlen(LoginInfo->UpGroup) != 0)
		{	
			strLoginGroup.Format(_T("%s"), LoginInfo->UpGroup);
			lstrcpy(pContext->m_group, strLoginGroup);
		}
		else
		{	
			lstrcpy(pContext->m_group, _T("Default"));
		}
		int nTabs = m_wndTabControl.GetItemCount();
		for ( int i = 0; i < nTabs; i++)
		{
			strTemp = m_wndTabControl.GetItem(i)->GetCaption();
			int n = strTemp.ReverseFind('(');
			if( n > 0 )
			{
				strGroupName = strTemp.Left(n);
			}
			else
			{
				strGroupName = strTemp;
			}
			if (strGroupName == pContext->m_group)
			{	
				bFind = true;
				pView = DYNAMIC_DOWNCAST(CClientView, CWnd::FromHandle(m_wndTabControl.GetItem(i)->GetHandle()));
				pView->PostMessage( WM_ADDTOLIST, 0, (LPARAM)pContext );
				break;
			}
		}
		if (!bFind)
		{	
			strGroupName.Format( _T("%s(1)"), pContext->m_group );
			AddGroup(strGroupName);
			pView = DYNAMIC_DOWNCAST(CClientView, CWnd::FromHandle(m_wndTabControl.GetItem(nTabs)->GetHandle()));
			pView->PostMessage(WM_ADDTOLIST, 0, (LPARAM)pContext );
		}
	}
	catch (...){}
	
	return 0;
}

void CTabView::UpdateDocTitle()
{
	GetDocument()->UpdateFrameCounts();
}

void CTabView::OnSelectedChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNUSED_ALWAYS(pNMHDR);
	*pResult = 0;
	
	UpdateDocTitle();
	
	CFrameWnd* pFrame = GetParentFrame();
	CView* pView = DYNAMIC_DOWNCAST(CView, CWnd::FromHandle(m_wndTabControl.GetSelectedItem()->GetHandle()));
	ASSERT_KINDOF(CView, pView);
	
	pFrame->SetActiveView(pView);
}

int CTabView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_wndTabControl.Create(WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), this, IDC_TABCONTROL);
		m_wndTabControl.GetPaintManager()->SetAppearance(xtpTabAppearancePropertyPageFlat);//xtpTabAppearanceVisualStudio2005
	/*
	xtpTabAppearancePropertyPageFlat,       // Gives your tabs an Office 2000 Flat appearance.//Good
	xtpTabAppearancePropertyPage2003,       // Gives your tabs an Office 2003 appearance./Good
	xtpTabAppearanceStateButtons,           // Gives your tabs a State Button appearance.//Good
	xtpTabAppearanceExcel,                  // Gives your tabs an Excel appearance.//Good
	xtpTabAppearanceVisio,                  // Gives your tabs a Visio appearance.//Good
	xtpTabAppearanceVisualStudio2005,       // Gives your tabs a Visual Studio 2005 appearance.//Good
	*/
	m_wndTabControl.GetPaintManager()->SetColor(xtpTabColorVisualStudio2010);	//标签颜色
	/*
	xtpTabColorDefault          = 0x0001, // Tabs will use the default color for the currently set Appearance.
	xtpTabColorVisualStudio2003 = 0x0002, // Tabs will use the Visual Studio 2003 color style for the currently set Appearance.
	xtpTabColorOffice2003       = 0x0004, // Tabs will use the Office 2003 color style for the currently set Appearance.
	xtpTabColorWinNative        = 0x0008, // Tabs will use the Windows XP color style for the currently set Appearance.
	xtpTabColorVisualStudio2005 = 0x0010, // Tabs will use the Visual Studio 2005 color style for the currently set Appearance.
	xtpTabColorOffice2007       = 0x0020, // Tabs will use the Office 2007 color style for the currently set Appearance.
	xtpTabColorVisualStudio2008 = 0x0040, // Tabs will use the Visual Studio 2008 color style for the currently set Appearance.
	xtpTabColorOffice2007Access = 0x0080, // Tabs will use the Office 2007 Access color style for the currently set Appearance.
	xtpTabColorVisualStudio2010 = 0x0100, // Tabs will use the Visual Studio 2010 color style for the currently set Appearance.
	xtpTabColorOffice2010       = 0x0200, // Tabs will use the Office 2010 color style for the currently set Appearance.
	*/
	m_wndTabControl.GetPaintManager()->m_bHotTracking = TRUE;
//	m_wndTabControl.GetPaintManager()->DisableLunaColors(TRUE);
	m_wndTabControl.GetPaintManager()->SetPosition(xtpTabPositionBottom); //标签位置
	/*
	xtpTabPositionTop,          // 上
	xtpTabPositionLeft,         // 左
	xtpTabPositionBottom,       // 下
	xtpTabPositionRight         // 右
	*/
	AddGroup(_T("Default(0)"));	//标签名称
// 	AddView(RUNTIME_CLASS(CPlay), _T("上线主机"));
// 	AddView(RUNTIME_CLASS(CPlayer), _T("本地设置"));
// 	AddView(RUNTIME_CLASS(CLockFile), _T("动态IP更新"));
	m_wndTabControl.SetCurSel(0);
	return 0;
}

int CTabView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

BOOL CTabView::AddGroup( LPCTSTR lpszTitle )
{
	BOOL Result = AddView(RUNTIME_CLASS(CClientView), lpszTitle);
	return Result;
}

BOOL CTabView::UpDateNumber()
{	
	CString strGroupName, strTemp;
	int nTabs = m_wndTabControl.GetItemCount();
	
	for ( int i = 0; i < nTabs; i++ )
	{	
		strTemp = m_wndTabControl.GetItem(i)->GetCaption();
		int n = strTemp.ReverseFind('(');
		if ( n > 0 )
		{
			strGroupName = strTemp.Left(n);
		}
		else
		{
			strGroupName = strTemp;
		}
		CClientView* pView = DYNAMIC_DOWNCAST(CClientView, CWnd::FromHandle(m_wndTabControl.GetItem(i)->GetHandle()));
		//自动删除 多余分组
		if (pView->m_pListCtrl->GetItemCount() == 0 && strGroupName != "Default")
		{
			m_wndTabControl.DeleteItem(i);
			i--;
			nTabs--;
			continue;
		}
		strTemp.Format(_T("%s(%d)"), strGroupName, pView->m_pListCtrl->GetItemCount() );
		m_wndTabControl.GetItem(i)->SetCaption(strTemp);
	}
	return TRUE;
}

void CTabView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	CMenu	popup;
	popup.LoadMenu(IDR_TabCtrl);
	CMenu*	pM = popup.GetSubMenu(0);
	CRect rc;
	CPoint	p;
	GetCursorPos(&p);
	GetWindowRect(&rc);
	point.x = point.x - rc.left;
	point.y = point.y - rc.top;
	
	int nItem = m_wndTabControl.GetItemCount();
	for (int i = 0; i < nItem; i++)
	{	
		if (m_wndTabControl.GetItem(i)->GetRect().PtInRect(point))
		{	
			pRightItem = m_wndTabControl.GetItem(i);
//			pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);
			CXTPCommandBars::TrackPopupMenu(pM, 0, p.x, p.y,this);
			break;
		}
	}
}

void CTabView::OnAddGroup() 
{
	// TODO: Add your command handler code here
	CInputDialog dlg;
	CClientView* pView = NULL;
	CString strGroup, strGroupName, strTemp;
	dlg.Init(_T("添加新分组"), _T("请输入新分组的名称："), this);
	if (dlg.DoModal() != IDOK || dlg.m_str.GetLength()== 0)   
		return;
	int nTabs = g_pTabView->m_wndTabControl.GetItemCount();
	int i=0;
	for ( i = 0; i < nTabs; i++ )
	{
		strTemp = g_pTabView->m_wndTabControl.GetItem(i)->GetCaption();
		int n = strTemp.ReverseFind('(');
		if ( n > 0 )
		{
			strGroupName = strTemp.Left(n);
		}
		else
		{
			strGroupName = strTemp;
		}
		if (dlg.m_str == strGroupName)
		{
			AfxMessageBox(_T("已存在该分组"));
			return;
		}
	}
	strGroup.Format(_T("%s(0)"), dlg.m_str);
	AddGroup(strGroup);
	pView = DYNAMIC_DOWNCAST(CClientView, CWnd::FromHandle(m_wndTabControl.GetItem(nTabs)->GetHandle()));
	pView->NewInitialUpdate();
}

void CTabView::OnDelGroup() 
{
	// TODO: Add your command handler code here
	CString strGroupName, strTemp;

	CClientView* pView = NULL;
	strTemp = pRightItem->GetCaption();
	int n = strTemp.ReverseFind('(');
	if ( n > 0 )
	{
		strGroupName = strTemp.Left(n);
	}
	else
	{
		strGroupName = strTemp;
	}

	if( strGroupName == _T("Default"))
	{
		AfxMessageBox(_T("默认分组不允许删除!"));
		return;
	}
	pView = DYNAMIC_DOWNCAST(CClientView, CWnd::FromHandle(pRightItem->GetHandle()));
	if (pView->m_pListCtrl->GetItemCount())
	{
		AfxMessageBox(_T("分组内存在主机不可删除!"));
		return;
	}
	//	DeleteGroup(pRightItem->GetCaption());
	pRightItem->Remove();
}
