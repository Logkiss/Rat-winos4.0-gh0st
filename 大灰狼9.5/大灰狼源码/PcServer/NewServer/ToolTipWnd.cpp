// ToolTipWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ToolTipWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolTipWnd
DWORD CToolTipWnd::nCurntTTWStyle=TTWS_HEART;
CToolTipWnd::CToolTipWnd()
{

	lpWndCls	= AfxRegisterWndClass(0);
	
	//Defaults
	m_bMouseIn	= false;
	m_bStuck	= false;
	
	m_iWidth = 120;
	m_iHeight = 100;
    m_hWndAlpha=175;//缺省透明度
	m_clrBkColor =RGB(213,253,224);//RGB(249,254,188); //light yellow
	m_clrFrameColor =RGB(0,0,0);  //blue
	m_clrTextColor = RGB(0,0,255);	 //black

	m_iFontHeight = 14;
	m_strFontName = "Arial";
//    nCurntTTWStyle=TTWS_NACELLE;//缺省样式为吊篮样式 
	pCurrwnd = NULL;
}

CToolTipWnd::~CToolTipWnd()
{

	BTOOLINFO *stToolInfo;
	CWnd *pWnd;
	for(POSITION pos = m_ToolPtr.GetStartPosition(); pos != NULL;)
	{
		m_ToolPtr.GetNextAssoc(pos, (void *&)pWnd, (void*&) stToolInfo);
		delete stToolInfo;
	}
	m_ToolPtr.RemoveAll();

}


BEGIN_MESSAGE_MAP(CToolTipWnd, CWnd)
	//{{AFX_MSG_MAP(CToolTipWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CToolTipWnd message handlers
BOOL CToolTipWnd::Create(CWnd* pParentWnd,DWORD dwStyle) 
{//主要目的是创建一个ToolTip窗口

	BOOL bRet = CWnd::CreateEx(NULL, lpWndCls, NULL,
		WS_POPUP, 0, 0, m_iWidth, m_iHeight,
		pParentWnd->GetSafeHwnd(), NULL, NULL);

	m_hParentWnd = pParentWnd->GetSafeHwnd();
	
	if(bRet)
		SetOwner(pParentWnd);
	//设置当前样式
	this->SetCurrentTTWStyle(dwStyle);
	/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
	//设置窗口的透明度
	SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,
		GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
	HINSTANCE hInst=LoadLibrary("user32.dll");
	if(hInst)
	{//加载成功
		typedef BOOL(WINAPI* LPSETLAYWND)(HWND,COLORREF,BYTE,DWORD);
        LPSETLAYWND MyProc=(LPSETLAYWND)GetProcAddress(hInst,"SetLayeredWindowAttributes");
        MyProc(this->GetSafeHwnd(),0,m_hWndAlpha,2);
		FreeLibrary(hInst);
	}
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
	return bRet;

}

void CToolTipWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectCl;
	GetClientRect(&rectCl);

	CRgn rgnComb;	
    rgnComb.CreateRectRgn(rectCl.left,rectCl.top,rectCl.right,rectCl.bottom);
	int iRetComb = rgnComb.CombineRgn(&rgnTri, &rgn, RGN_OR);
	if(iRetComb==ERROR)
	{
		AfxMessageBox("ERROR in Combining Region");
		return;
	}
	CBrush pBrush;
	pBrush.CreateSolidBrush(m_clrFrameColor);
	
	CBrush pBrush1;
	pBrush1.CreateSolidBrush(m_clrBkColor);
	
	dc.FillRgn( &rgnComb, &pBrush1);
	dc.FrameRgn(&rgnComb, &pBrush, 2, 1);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(m_clrTextColor);
	
	CFont *pFont = dc.SelectObject(&m_fontText);
    CSize czTextWidth = dc.GetTextExtent(m_strText);
	
	if( czTextWidth.cx < m_RectText.Width())
		dc.DrawText(m_strText, m_RectText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	else
		dc.DrawText(m_strText, m_RectText, DT_CENTER | DT_WORDBREAK);
	
    dc.SelectObject(pFont);
	
}


int CToolTipWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{//合并区域
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetTTWStyle(nCurntTTWStyle);//创建一个当前指定的窗口样式
	m_fontText.CreateFont(m_iFontHeight, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,m_strFontName);	
	
	return 0;
}

void CToolTipWnd::RelayEvent(LPMSG lpMsg)
{

	switch(lpMsg->message) 
	{
	case WM_KEYDOWN:
			if(IsWindowVisible())
			{		
				ShowWindow(SW_HIDE);
			}
			break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
			if(IsWindowVisible())
			{
				ShowWindow(SW_HIDE);
			}
			break;

	case WM_MOUSEMOVE:
		{
			CWnd *pFocusWnd = AfxGetApp()->m_pMainWnd->GetFocus();
			if(pFocusWnd==NULL)
				break;
			CWnd* pWnd = CWnd::FromHandle(lpMsg->hwnd);

			HWND hWndTemp = ::GetParent(lpMsg->hwnd);
			
			CPoint pt;
			pt.x = lpMsg->pt.x;
			pt.y = lpMsg->pt.y;
			
			BTOOLINFO *stToolInfo;
			CWnd *pBToolWnd;
			
			for(POSITION pos = m_ToolPtr.GetStartPosition(); pos != NULL;)
			{
				m_ToolPtr.GetNextAssoc(pos, (void *&)pBToolWnd, (void*&) stToolInfo);
				
				if(!m_bMouseIn)
				{
					if(lpMsg->hwnd == stToolInfo->hwndTool)
					{
					
						if(m_bStuck && IsWindowVisible())
						{
							SetWindowPos(&wndTop,pt.x,pt.y,m_iWidth,m_iHeight,SWP_NOACTIVATE);
							ShowWindow(SW_SHOWNOACTIVATE);
						}

						m_bMouseIn = true;
						m_clrTextColor = stToolInfo->clrToolTextClr; 
						m_strText = stToolInfo->strToolText; 
						
						SetWindowPos(&wndTop,pt.x,pt.y,m_iWidth,m_iHeight,SWP_NOACTIVATE);
						ShowWindow(SW_SHOWNOACTIVATE);
						
						pCurrwnd = stToolInfo->hwndTool;

						break;
					}
				}
				else
				{
					CRect rect;
					::GetWindowRect(pCurrwnd, &rect);
					if(m_bStuck && IsWindowVisible())
					{
						SetWindowPos(&wndTop,pt.x,pt.y,m_iWidth,m_iHeight,SWP_NOACTIVATE);
						ShowWindow(SW_SHOWNOACTIVATE);
					}
					
					
					CWnd* pWnd = CWnd::FromHandle(lpMsg->hwnd);
					CWnd *WndPt = pWnd->WindowFromPoint(lpMsg->pt);
					if(WndPt->GetSafeHwnd() != pCurrwnd)
					{
						m_bMouseIn = false;
						ShowWindow(SW_HIDE);
					}
					

					break;
				}
			}
			
		}	
		break; //WM_MOUSEMOVE
	}
	 
}

void CToolTipWnd::AddTool(CWnd *pWnd, CString strText, COLORREF clrTextColor)
{

	BTOOLINFO *stToolInfo;

	if(!m_ToolPtr.Lookup( pWnd, ( void*& )  stToolInfo))
	{
		stToolInfo = new BTOOLINFO;
		stToolInfo->hwndTool = pWnd->GetSafeHwnd();
		stToolInfo->strToolText = strText;
		if(clrTextColor==NULL)
			stToolInfo->clrToolTextClr = m_clrTextColor;
		else
			stToolInfo->clrToolTextClr = clrTextColor;
	
		m_ToolPtr.SetAt(pWnd, stToolInfo);
	}

}

BOOL CToolTipWnd::SetTTWStyle(DWORD nStyle)
{//设置指定样式窗口
	CRect rectCl;
	GetClientRect(&rectCl);
	//区域初始化
	rgn.m_hObject = NULL;
	rgnTri.m_hObject = NULL;
	rgnComb.m_hObject = NULL;
	this->RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);
/////////////////////////////////////////////////////////////////////
//////////////////       设置气球样式
/////////////////////////////////////////////////////////////////////
   if(nStyle==TTWS_BALLOON)
   {//气球样式
	/*=======================创建气球提示框=========================*/
	int x=0, y=0;
	CRect rectTemp;

	rectTemp = rectCl;
	rectTemp.left = rectTemp.left + 10;//影响m_RectText
	
	x = (int)( (float)((float)rectTemp.Width() / 2.0) / 1.41421);
	y = (int)( (float)((float)rectTemp.Height() / 2.0) / 1.41421);

	m_RectText.top = ( (rectTemp.Height() / 2) - y);
	m_RectText.left = ( (rectTemp.Width() / 2) - x) + 10;
	m_RectText.right = ( (rectTemp.Width() / 2) + x) + 10;
	m_RectText.bottom = ( (rectTemp.Height() / 2) + y);
    BOOL bRegRet = rgn.CreateEllipticRgn(rectCl.left+10,rectCl.top,rectCl.right,rectCl.bottom);
		
	CPoint ptTri[3];
	ptTri[0].x = rectCl.left;
	ptTri[0].y = (rectCl.bottom / 2) - 10;

	ptTri[1].x = rectCl.left + 15;
	ptTri[1].y = (rectCl.bottom / 2) - 5;

	ptTri[2].x = rectCl.left + 15;
	ptTri[2].y = (rectCl.bottom / 2) + 5;
	
	ptTri[3].x = rectCl.left;
	ptTri[3].y = (rectCl.bottom / 2) - 10;

	BOOL bRegTriRet = rgnTri.CreatePolygonRgn(ptTri, 3, ALTERNATE);

	rgnComb.CreateRectRgn(rectCl.left+10,rectCl.top,rectCl.right,rectCl.bottom);
	int iRetComb = rgnComb.CombineRgn(&rgnTri, &rgn, RGN_OR);

	if(iRetComb == ERROR)
	{
		AfxMessageBox("ERROR in Combining Region");
		return -1;
	}
	int bRgnWnd = SetWindowRgn(rgnComb.operator HRGN( ), TRUE);	
	/*=========================创建完成============================*/
//	nCurntTTWStyle=TTWS_BALLOON;
   }

//////////////////////////////////////////////////////////////////////
///////////////           设置吊篮样式 
//////////////////////////////////////////////////////////////////////
    if(nStyle==TTWS_NACELLE)
	{//吊篮样式
		
		/*====================创建一个箭头提示框=======================*/
	   	rgn.m_hObject = NULL;
	    rgnTri.m_hObject = NULL;
	    rgnComb.m_hObject = NULL;
        /*##################创建上面的小的三角形##################*/
	    CPoint pnt[3];
		pnt[0].x=rectCl.left;
		pnt[0].y=rectCl.top;//第一点
		pnt[1].x=rectCl.left+15;
		pnt[1].y=(rectCl.top+rectCl.bottom)/2;
		pnt[2].x=pnt[1].x+20;
		pnt[2].y=pnt[1].y;
        BOOL bRegTriRet=rgnTri.CreatePolygonRgn(pnt,3,ALTERNATE);
	    /*#######################END##############################*/
        
		/*######################创建圆角矩形######################*/
		CRect rect;
		rect=rectCl;
		rect.top=(rectCl.top+rectCl.bottom)/2;
		rgn.CreateRoundRectRgn(rect.left,rect.top,rect.right,rect.bottom,15,15);
        /*##########################END###########################*/
		///////设置文本框
		  m_RectText.left=rectCl.left+15;
		  m_RectText.top=(rectCl.top+rectCl.bottom)/2;
		  m_RectText.right=rectCl.right-15;
		  m_RectText.bottom=rectCl.bottom;
		///////over//////////
	rgnComb.CreateRectRgn(rectCl.left,rectCl.top,rectCl.right,rectCl.bottom);
	int iRetComb = rgnComb.CombineRgn(&rgnTri, &rgn, RGN_OR);

	 if(iRetComb == ERROR)
	 {
		AfxMessageBox("ERROR in Combining Region");
		return -1;
	 }
	 /*=========================创建完成============================*/
	int bRgnWnd = SetWindowRgn(rgnComb.operator HRGN( ), TRUE);	
//	nCurntTTWStyle=TTWS_NACELLE;
	}
//////////////////////////////////////////////////////////////////////
////                  设置心脏样式
//////////////////////////////////////////////////////////////////////
	if(nStyle==TTWS_HEART)
	{
		//########################设定文本框#######################*/
        m_RectText.left=rectCl.left+rectCl.Width()/4;
		m_RectText.top=rectCl.top+rectCl.Height()/4;
		m_RectText.right=rectCl.right-rectCl.Width()/4;
		m_RectText.bottom=rectCl.top+3*rectCl.Height()/4;
		//#########################################################*/
		/*======================设置左右椭圆=======================*/
		CRect rectLeft,rectRight;
		rectLeft=rectCl;
		rectRight=rectCl;
		//设置窗口
		rectLeft.right=rectCl.Width()/2;
		rectLeft.bottom=rectCl.Height()/2;
		rectRight.left=rectCl.Width()/2;
		rectRight.bottom=rectCl.Height()/2;
		CRgn rgnLeft,rgnRight;
		rgnLeft.CreateEllipticRgn(rectLeft.left,rectLeft.top,rectLeft.right,rectLeft.bottom);
		rgnRight.CreateEllipticRgn(rectRight.left,rectRight.top,rectRight.right,rectRight.bottom);
		rgnTri.CreateRectRgn(rectCl.left,rectCl.top,rectCl.right,rectCl.bottom/2);
		int nBet=rgnTri.CombineRgn(&rgnLeft,&rgnRight,RGN_OR);
		if(nBet==ERROR )
		{
			AfxMessageBox("合并区域失败!");
			return -1;
		}
	/*===============================================================*/
		//设置抛物线部分
		CRect rectTemp;
		rectTemp=rectCl;
		rectTemp.top=rectCl.Height()/4;
		int nWidth,nHeight;
		nWidth=rectTemp.Width();
		nHeight=rectTemp.Height();
		CPoint points[1000];
		for(int i=0;i<1000;i++)
		{
			points[i].x=rectTemp.left;
			points[i].y=rectTemp.top;
		}
		for(i=0;i<nWidth;i++)
		{
			points[i].x=rectTemp.left+i;
			points[i].y=rectTemp.top+nHeight-4*nHeight*(points[i].x-nWidth/2)*(points[i].x-nWidth/2)/(nWidth*nWidth);
		}
		rgn.CreatePolygonRgn(points,1000,ALTERNATE);
		//合并区域
		rgnComb.CreateRectRgn(rectCl.left,rectCl.top,rectCl.right,rectCl.bottom);
	   int iRetComb = rgnComb.CombineRgn(&rgnTri, &rgn, RGN_OR);

	 if(iRetComb == ERROR)
	 {
		AfxMessageBox("ERROR in Combining Region");
		return -1;
	 }
	 /*=========================创建完成============================*/
	int bRgnWnd = SetWindowRgn(rgnComb.operator HRGN( ), TRUE);	
	}

/////////////////////////////////////////////////////////////////////
////////////////      设置普通矩形样式
/////////////////////////////////////////////////////////////////////
	if(nStyle==TTWS_RECT)
	{
		rgnTri.CreateRectRgn(rectCl.left,rectCl.top,rectCl.right,rectCl.bottom);
		rgn.CreateRectRgn(rectCl.left,rectCl.top,rectCl.right,rectCl.bottom);
		rgnComb.CreateRectRgn(rectCl.left,rectCl.top,rectCl.right,rectCl.bottom);
       m_RectText=rectCl;//设置文本框
       int iRetComb = rgnComb.CombineRgn(&rgnTri, &rgn, RGN_AND);

	 if(iRetComb == ERROR)
	 {
		AfxMessageBox("ERROR in Combining Region");
		return -1;
	 }
	 /*=========================创建完成============================*/
	int bRgnWnd = SetWindowRgn(rgnComb.operator HRGN( ), TRUE);	
	}

//////////////////////////////////////////////////////////////////////
/////////////////////////设置圆角矩形
//////////////////////////////////////////////////////////////////////
	if(nStyle==TTWS_ROUNDRECT)
	{
		rgnTri.CreateRoundRectRgn(rectCl.left,rectCl.top,rectCl.right,rectCl.bottom,15,15);
		rgn.CreateRoundRectRgn(rectCl.left,rectCl.top,rectCl.right,rectCl.bottom,15,15);
		rgnComb.CreateRectRgn(rectCl.left,rectCl.top,rectCl.right,rectCl.bottom);
		//设置文本框
       m_RectText.left=rectCl.left+15;
	   m_RectText.top=rectCl.top+15; 
	   m_RectText.right=rectCl.right-15;
	   m_RectText.bottom=rectCl.bottom-15;
	   //区域合并
       int iRetComb = rgnComb.CombineRgn(&rgnTri, &rgn, RGN_AND);

	 if(iRetComb == ERROR)
	 {
		AfxMessageBox("ERROR in Combining Region");
		return -1;
	 }
	 /*=========================创建完成============================*/
	int bRgnWnd = SetWindowRgn(rgnComb.operator HRGN( ), TRUE);	
	}

//////////////////////////////////////////////////////////////////////
////////////////////////椭圆区域设置
//////////////////////////////////////////////////////////////////////
    if(nStyle==TTWS_ELLIPSE)
	{
        rgnTri.CreateEllipticRgn(rectCl.left,rectCl.top,rectCl.right,rectCl.bottom);
		rgn.CreateEllipticRgn(rectCl.left,rectCl.top,rectCl.right,rectCl.bottom);
		rgnComb.CreateRectRgn(rectCl.left,rectCl.top,rectCl.right,rectCl.bottom);
		//设置文本框
		CRect rectTemp=rectCl;
		int x,y;
       x = (int)( (float)((float)rectTemp.Width() / 2.0) / 1.41421);
	   y = (int)( (float)((float)rectTemp.Height() / 2.0) / 1.41421);
	   m_RectText.left=rectCl.left+rectCl.Width()/2-x;
	   m_RectText.top=rectCl.top+rectCl.Height()/2-y;
	   m_RectText.right=rectCl.right+x-rectCl.Width()/2;
	   m_RectText.bottom=rectCl.bottom+y-rectCl.Height()/2;
	   //区域合并
       int iRetComb = rgnComb.CombineRgn(&rgnTri, &rgn, RGN_AND);

	 if(iRetComb == ERROR)
	 {
		AfxMessageBox("ERROR in Combining Region");
		return -1;
	 }
	 /*=========================创建完成============================*/
	   int bRgnWnd = SetWindowRgn(rgnComb.operator HRGN( ), TRUE);	
	}
//////////////////////////////////////////////////////////////////////
  return false;
}




