// SortListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SortListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

/////////////////////////////////////////////////////////////////////////////
// CSortListCtrl

CSortListCtrl::CSortListCtrl()
{
	m_fAsc=TRUE;
}

CSortListCtrl::~CSortListCtrl()
{
}


BEGIN_MESSAGE_MAP(CSortListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSortListCtrl)
	//手工定义消息映射，处理顶部按钮被按下的消息
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	//}}AFX_MSG_MAP
	

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSortListCtrl message handlers

//*******************************************************************
//
// 排序函数的实现
//
//*******************************************************************
int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CSortListCtrl* pV=(CSortListCtrl*)lParamSort; 
    CString strItem1, strItem2; 
    LVFINDINFO info; 
	int nIndex; 
    info.flags = LVFI_PARAM;                       //设置为LVFI_PARAM，后面的FindItem才能有效，详情参考MSDN的LVFINDINFO 
    info.lParam = lParam1;                         //由item data(lParam1)得到对应的item text 
    if ( (nIndex=pV->FindItem(&info)) != -1) 
       strItem1 = pV->GetItemText(nIndex, pV->m_nSortedCol); 
    info.lParam = lParam2;                         //由item data(lParam2)得到对应的item text 
    if ( (nIndex=pV->FindItem(&info)) != -1)
       strItem2 = pV->GetItemText(nIndex, pV->m_nSortedCol); 
    int iCompRes ;
		iCompRes = strcmp(strItem1, strItem2);     //根据当前的排序方式进行调整 
    if(pV->m_fAsc) 
       return iCompRes;
    else 
       return iCompRes*-1;
}

//*******************************************************************
//
// 处理顶部按钮被按下的消息
//
//*******************************************************************
void CSortListCtrl::OnColumnClick( NMHDR* pNMHDR, LRESULT* pResult )
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if( pNMListView->iSubItem == m_nSortedCol )
		m_fAsc = !m_fAsc;
	else
	{
		m_fAsc = TRUE;
		m_nSortedCol = pNMListView->iSubItem;
	}
	VERIFY(SortItems( ListCompare, reinterpret_cast<DWORD>( this )));

	*pResult = 0;
}