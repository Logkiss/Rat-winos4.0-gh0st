// FaderWnd.cpp : implementation file
//CFaderWnd类为透明窗口的接口类,其实现的原理是:
//先从要实现透明的窗口得到窗口类指针,作为参数传给CFaderWnd类
//CFaderWnd类得到窗口指针类后,创建一个和得到的窗口指针类很相似的窗口
//创建的窗口类中必须指定WS_EX_LAYERED(0x00080000)和WS_EX_TOPMOST样式
//通过WS_EX_TOPMOST样式的设置,将当前创建的窗口作为分层窗口的上层窗口
//在创建结构体中CFaderWnd()内置一个时间器
//通过调用UpdateLayeredWindow()函数更新分层窗口的透过度
//通过时间器的作用,每隔一定的时间就将透明度降低,渐渐的趋向透明,实现窗口的渐隐功能
//当设置的时间完了后,上层窗口已经透明,这个时候就将下层窗口(也就是要实现渐隐的窗口)关闭或隐藏起来
//调用方法:
//主框架为基础的窗口实现方法:
//new CFaderWnd(this);
//ShowWindow(WS_HIDE);
//一般的文档类的窗口(如CAboutDlg类)实现方法:
//new CFaderWnd(this);
//EndDialog(ID);

#include "stdafx.h"
//#include "FaderWndTest.h"
#include "FaderWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFaderWnd

CFaderWnd::CFaderWnd(CWnd*pWndToFader/*=NULL*/,UINT nFaderTime/*=2000*/,BYTE ALPHA/*=255*/)
:m_pWndToFader(pWndToFader)//m_pWndtoFader类指针的作用是将要实现渐隐的窗口于当前的类联系起来
{
	//检查对象的合法性,只有在调试版本中有用,发行版本中没有用
	ASSERT(pWndToFader);
	ASSERT_VALID(pWndToFader);
	//得到合法的ALPHA值或调用缺省的值
	if(ALPHA<5)  return;
	//加载UpdateLayeredWindow()函数从user32.dll
	HMODULE m_hInstUser32=GetModuleHandle("user32.dll");
    m_lpfunUpdateLayeredWnd=(lpUpdateLayeredWindow)GetProcAddress(m_hInstUser32,"UpdateLayeredWindow");
	if(m_hInstUser32==NULL)
	{
		if(m_lpfunUpdateLayeredWnd==NULL)
		{
			AfxMessageBox("不能加载user32.dll或UpdateLayeredWindow()函数不存在!!");
		     delete this ;
		}
	}
	else
	{
		//创建分层窗口
		CPoint point(0,0);//初始话点对象
		SIZE   size;
		CRect  pWndToFaderRect;
		//得到对象窗口的矩形区域
	    m_pWndToFader->GetWindowRect(&pWndToFaderRect);
		//得到窗口的对象属性
        size.cx=pWndToFaderRect.Width();
		size.cy=pWndToFaderRect.Height();
		point=pWndToFaderRect.TopLeft();
		//创建分层窗口的上层窗口
		//样式WS_EX_LAYERED必要要,是为了为UpdateLayeredWindow()函数的调用
		//WS_EX_TRANSPARENT allows mouse clicks through to the window "underneath",
		CreateEx(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_TRANSPARENT,
			"STATIC","",WS_VISIBLE|WS_POPUP,pWndToFaderRect,AfxGetMainWnd(),0); 
		//创建BLENDFUNCTION结构体为UpdateLayeredWindow()所用
        m_Blend.BlendOp=AC_SRC_OVER;  //唯一保留标志
		m_Blend.BlendFlags=0;
		m_Blend.SourceConstantAlpha=ALPHA; //定义初始时候的透明度
		m_Blend.AlphaFormat=0;
		//设计初始时候的分层窗口的透明度
        m_lpfunUpdateLayeredWnd(GetSafeHwnd(),NULL,NULL,&size,::GetDC(m_pWndToFader->GetSafeHwnd()),&point,0,&m_Blend, 0x00000002);
		//设置渐隐窗口的时间器
		UINT nFaderTimeUnit=nFaderTime/(ALPHA/5);//定义单位时间间隔
		SetTimer(FaderWndTimer,nFaderTimeUnit,NULL);

	}
}

CFaderWnd::~CFaderWnd()
{
}


BEGIN_MESSAGE_MAP(CFaderWnd, CWnd)
	//{{AFX_MSG_MAP(CFaderWnd)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFaderWnd message handlers

void CFaderWnd::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent== FaderWndTimer)
	{
		//若是窗口的渐隐时间器的话
		if(m_Blend.SourceConstantAlpha>=5)
		{
			//透明度必须大于或等于5,否则的话透明度减少到负数时又从默认的值开始,达不到透明的效果
         m_Blend.SourceConstantAlpha-=5; //逐渐减少透明度,渐渐趋向透明
		 //更新分层窗口的透明程度
         m_lpfunUpdateLayeredWnd(GetSafeHwnd(),NULL,NULL,NULL,NULL,NULL,NULL,&m_Blend,  0x00000002);
		}
		else
		{
			KillTimer(FaderWndTimer);//销毁窗口时间器
			DestroyWindow();//销毁窗口
		}
	}
//	CWnd::OnTimer(nIDEvent);
}


