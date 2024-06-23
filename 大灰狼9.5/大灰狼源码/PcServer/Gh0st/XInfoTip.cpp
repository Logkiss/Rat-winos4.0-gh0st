///////////////////////////////////////////////////////////////////////////
//                                                                         
// FILE NAME                                                               
//                                                                         
//		XInfoTip.cpp
//                                                                         
// COMPONENT                                                               
//                                                                         
//		CXInfoTip class implementation
//                                                                         
// DESCRIPTION                                                             
//
//		(see header)
//
// AUTHOR
//
//		Mark Bozeman		09-16-2001
//
///////////////////////////////////////////////////////////////////////////
// This software is released into the public domain.  
// You are free to use it in any way you like.
//
// This software is provided "as is" with no expressed 
// or implied warranty.  I accept no liability for any 
// damage or loss of business that this software may cause. 
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"			// PCH
#include "XInfoTip.h"		// Class interface

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CX_ROUNDED				12		// Tip horizontal roundness
#define CY_ROUNDED				10		// Tip vertical roundness
#define CX_LEADER				25		// Width of tip lead
#define CY_LEADER				25		// Height of tip lead
#define CX_ICON_MARGIN			5		// Width of margin between icon and tip text

#define DEFAULT_SHOW_DELAY		500		// Default show delay (ms)

#define TIMER_HIDE				500		// Hide timer (ms)


/////////////////////////////////////////////////////////////////////
// 
// CXInfoTip::CXInfoTip()
// 
// DESCRIPTION
//     
//		Constructor
//     
/////////////////////////////////////////////////////////////////////
CXInfoTip::CXInfoTip()
{
	// Register the class
	m_szClass		= AfxRegisterWndClass(0);

	m_hIcon			= NULL;

	// Set the delay defaults
	m_nShowDelay	= DEFAULT_SHOW_DELAY;

	m_IconSize		= CSize(0, 0);
	m_ptOrigin		= CPoint(0, 0);
}

/////////////////////////////////////////////////////////////////////
// 
// CXInfoTip::~CXInfoTip()
// 
// DESCRIPTION
//     
//		Deconstructor
//     
/////////////////////////////////////////////////////////////////////
CXInfoTip::~CXInfoTip()
{
}

// Message map
BEGIN_MESSAGE_MAP(CXInfoTip, CWnd)
	//{{AFX_MSG_MAP(CXInfoTip)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////
// 
// CXInfoTip::Create()
// 
// DESCRIPTION
//     
//		Creates the tip window
//
// RETURNS
//
//		[BOOL]			- TRUE on success, FALSE on failure
//
// PARAMETERS
//
//		[pParentWnd]	- Pointer to parent window
//     
/////////////////////////////////////////////////////////////////////
BOOL CXInfoTip::Create(CWnd* pParentWnd) 
{
	BOOL	bSuccess;

	// Must have a parent
	ASSERT(pParentWnd != NULL);

	bSuccess = CreateEx(NULL, m_szClass, NULL, WS_POPUP, 0, 0, 0, 0, pParentWnd->GetSafeHwnd(), NULL, NULL);

	// Use default GUI font for default font
	m_pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));

	return bSuccess;
}

/////////////////////////////////////////////////////////////////////
// 
// CXInfoTip::Show()
// 
// DESCRIPTION
//     
//		Shows the tip window
//
// RETURNS
//
//		[void]	
//
// PARAMETERS
//
//		[szText]	- Tip text
//		[pt]		- Coordinates to display tip window
//					or NULL to use the current cursor position
//     
/////////////////////////////////////////////////////////////////////
void CXInfoTip::Show(CString szText, CPoint *pt /* = NULL */)
{
	if (pt != NULL)
		m_ptOrigin	= *pt;
	else
		GetCursorPos(&m_ptOrigin);

	m_szText	= szText;

	// Start the show timer
	m_nTimer = SetTimer(timerShow, m_nShowDelay, NULL);
}

/////////////////////////////////////////////////////////////////////
// 
// CXInfoTip::OnPaint()
// 
// DESCRIPTION
//     
//		Paint the window
//
// RETURNS
//
//		[void]
//
/////////////////////////////////////////////////////////////////////
void CXInfoTip::OnPaint() 
{
	CPaintDC dc( this ); // device context for painting

	CRect	rc;
	CBrush	WindowBrush;
	CBrush	FrameBrush;
	CBrush	InnerFrameBrush;
	HRGN	hRegion;
	CRgn	*pRegion;
	CFont	*pSysFont;

	// Get the client rectangle
	GetClientRect(rc);

	// Create the brushes
	InnerFrameBrush.CreateSolidBrush(::GetSysColor(COLOR_SCROLLBAR));
	FrameBrush.CreateSolidBrush(::GetSysColor(COLOR_WINDOWTEXT));
	WindowBrush.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));

	// Get the window region
	GetWindowRegion(&dc, &hRegion);
	pRegion = CRgn::FromHandle(hRegion);

	// Draw the frame
	dc.FillRgn(pRegion, &WindowBrush);
	dc.FrameRgn(pRegion, &InnerFrameBrush, 3, 3);
	dc.FrameRgn(pRegion, &FrameBrush, 1, 1);

	// Adjust the area for the icon
	rc.DeflateRect(CX_ROUNDED, CY_ROUNDED, 0, 0);
	if (m_hIcon != NULL)
		rc.left = rc.left + m_IconSize.cx + CX_ICON_MARGIN;
	
	// Set the font
	pSysFont = (CFont *)dc.SelectObject(m_pFont);
	// Draw the tip text	
	dc.SetBkMode( TRANSPARENT );
	dc.DrawText(m_szText, &rc, DT_TOP | DT_LEFT);

	// Draw the icon
	if (m_hIcon != NULL)
		DrawIconEx(dc.m_hDC, CX_ROUNDED, CY_ROUNDED, m_hIcon, m_IconSize.cx, m_IconSize.cy, 0, NULL, DI_NORMAL);

	// Clean up GDI
	::DeleteObject(hRegion);
	dc.SelectObject(pSysFont);

}

/////////////////////////////////////////////////////////////////////
// 
// CXInfoTip::GetWindowRegion()
// 
// DESCRIPTION
//     
//		Retrieves the window region
//
// RETURNS
//
//		[BOOL]		- TRUE on success, FALSE on failure
//
// PARAMETERS
//
//		[pDC]		- Pointer to display device context
//		[hRegion]	- Filled with the calculated window region
//		[Size]		- Filled with the calculated window size
//					or NULL.
//     
/////////////////////////////////////////////////////////////////////
BOOL CXInfoTip::GetWindowRegion(CDC* pDC, HRGN* hRegion, CSize *Size /* = NULL */)
{
	CRect	rcWnd;
	POINT	ptLeader[3];
	CRgn	LeaderRegion;
	CRgn	CaptionRegion;
	CFont	*pSysFont;
	
	ASSERT(pDC != NULL);
	ASSERT(hRegion != NULL);

	// Calculate the are for the tip text
	pSysFont = (CFont *)pDC->SelectObject(m_pFont);
	pDC->DrawText(m_szText, &rcWnd, DT_CALCRECT);
	pDC->SelectObject(pSysFont);

	// Adjust for the rounded corners
	rcWnd.InflateRect(CX_ROUNDED, CY_ROUNDED);

	// Adjust for icon
	if (m_hIcon != NULL)
		rcWnd.right = rcWnd.right + m_IconSize.cx + CX_ICON_MARGIN;
	if (rcWnd.Height() < m_IconSize.cy)
		rcWnd.bottom = rcWnd.top + m_IconSize.cy;

	// Calculate the leader triangle coordinates

	ptLeader[0].x	= rcWnd.Width() - CX_ROUNDED;
	ptLeader[0].y	= rcWnd.Height() - CY_ROUNDED;

	ptLeader[1].x	= ptLeader[0].x;
	ptLeader[1].y	= ptLeader[0].y + CY_LEADER;

	ptLeader[2].x	= ptLeader[0].x - CX_LEADER;
	ptLeader[2].y	= rcWnd.Height() - CY_ROUNDED;

	// Create the caption region
	CaptionRegion.CreateRoundRectRgn(0, 0, rcWnd.Width(), rcWnd.Height(), CX_ROUNDED, CY_ROUNDED);
	// Create the leader region
	LeaderRegion.CreatePolygonRgn(ptLeader, 3, ALTERNATE);
	// Create window region
	*hRegion =  ::CreateRectRgn(0, 0, rcWnd.Width(), rcWnd.Height() + CY_LEADER);
	// Combine the regions
	CombineRgn(*hRegion, CaptionRegion.operator HRGN(), LeaderRegion.operator HRGN(), RGN_OR);

	// Set the window size
	if (Size != NULL)
	{
		Size->cx	= rcWnd.Width();
		Size->cy	= rcWnd.Height() + CY_LEADER;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// 
// CXInfoTip::OnCreate()
// 
// DESCRIPTION
//     
//		Window creation
//
// RETURNS
//
//		[int]				- Zero on success, -1 otherwise
//
// PARAMETERS
//
//		[lpCreateStruct]	- Pointer to creation structure
//     
/////////////////////////////////////////////////////////////////////
int CXInfoTip::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
   if ( CWnd::OnCreate( lpCreateStruct ) == -1 )
      return -1;
   
   return 0;
}

/////////////////////////////////////////////////////////////////////
// 
// CXInfoTip::OnTimer()
// 
// DESCRIPTION
//     
//		Timer event
//
// RETURNS
//
//		[void]	
//
// PARAMETERS
//
//		[nIDEvent]	- Timer identifier
//     
/////////////////////////////////////////////////////////////////////
void CXInfoTip::OnTimer( UINT nIDEvent ) 
{
	HRGN	hRegion;
	CSize	WindowSize;
	CDC		*pDC;
	CPoint	ptCursor;

	switch (nIDEvent)
	{
	/////////////////////////////////////////////////////////////////////
	// Show the tip window
	/////////////////////////////////////////////////////////////////////
	case timerShow:
		KillTimer(m_nTimer);

		pDC = GetDC();
		GetWindowRegion(pDC, &hRegion, &WindowSize);
		ReleaseDC(pDC);

		::SetWindowRgn(m_hWnd, hRegion, TRUE);

		SetWindowPos(&wndTop, m_ptOrigin.x - WindowSize.cx + CX_ROUNDED, m_ptOrigin.y - WindowSize.cy + CY_ROUNDED, WindowSize.cx, WindowSize.cy, SWP_NOACTIVATE | SWP_SHOWWINDOW);

		m_nTimer = SetTimer(timerHide, TIMER_HIDE, NULL);
		break;
	/////////////////////////////////////////////////////////////////////
	// Hide the tip window
	/////////////////////////////////////////////////////////////////////
	case timerHide:
		GetCursorPos(&ptCursor);
		if (ptCursor != m_ptOrigin)
		{
			KillTimer(m_nTimer);
			ShowWindow(SW_HIDE);
		}

		break;
	}

	CWnd::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////
// 
// CXInfoTip::OnDestroy()
// 
// DESCRIPTION
//     
//		Window destruction
//
// RETURNS
//
//		[void]	
//
/////////////////////////////////////////////////////////////////////
void CXInfoTip::OnDestroy() 
{
	KillTimer(m_nTimer);

	CWnd::OnDestroy();
}

/////////////////////////////////////////////////////////////////////
// 
// CXInfoTip::RelayEvent()
// 
// DESCRIPTION
//     
//		Call this in the parent's PreTranslateMessage() to 
//		relay tooltip event messages.
//
// RETURNS
//
//		[void]	
//
// PARAMETERS
//
//		[lpMsg]	- Pointer to message structure
//     
/////////////////////////////////////////////////////////////////////
void CXInfoTip::RelayEvent(LPMSG lpMsg)
{
	CPoint			point;
	CWnd			*pWindow;
	CString			szTooltipText;
	TipToolInfo		Info;
		
	switch(lpMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		ShowWindow(SW_HIDE);
		break;
	case WM_MOUSEMOVE:
		GetCursorPos(&point);

		if (point != m_ptOrigin)
		{
			// Find the tool
			pWindow = WindowFromPoint(point);
			if (pWindow != NULL)
			{
				if (m_ToolMap.Lookup(pWindow->m_hWnd, Info))
				{
					// Display the tooltip
					m_ptOrigin = point;
					SetIcon(Info.hIcon);
					Show(Info.szText, &point);
				}
			}
		}

		// Hide the tooltip
		if (point != m_ptOrigin)
			ShowWindow(SW_HIDE);
		
		break;
	}
}

/////////////////////////////////////////////////////////////////////
// 
// CXInfoTip::SetIcon()
// 
// DESCRIPTION
//     
//		Sets the tip window icon
//
// RETURNS
//
//		[void]	
//
// PARAMETERS
//
//		[hIcon]	- Handle to the icon
//     
/////////////////////////////////////////////////////////////////////
void CXInfoTip::SetIcon(HICON hIcon) 
{
	ICONINFO	IconInfo;

	m_hIcon = hIcon; 

	if (hIcon == NULL)
	{
		m_IconSize = CSize(0, 0);
		return;
	}
	
	// Get the icon sizes	
	ZeroMemory(&IconInfo, sizeof(ICONINFO));
	::GetIconInfo(m_hIcon, &IconInfo);

	m_IconSize.cx = (BYTE)(IconInfo.xHotspot * 2);
	m_IconSize.cy = (BYTE)(IconInfo.yHotspot * 2);
    
	::DeleteObject(IconInfo.hbmMask);
	::DeleteObject(IconInfo.hbmColor);

	if (IsWindow(m_hWnd))
		RedrawWindow();
}

/////////////////////////////////////////////////////////////////////
// 
// CXInfoTip::AddTool()
// 
// DESCRIPTION
//     
//		Adds a tool
//
// RETURNS
//
//		[void]	
//
// PARAMETERS
//
//		[pWnd]			- Pointer to the tool window
//		[szTooltipText]	- Text to display when the cursor hovers
//						over the window (pWnd)
//		[hIcon]			- Icon to display in the tooltip, or NULL
//						to display no icon.
//     
/////////////////////////////////////////////////////////////////////
void CXInfoTip::AddTool(CWnd *pWnd, LPCTSTR szTooltipText, HICON hIcon /* = NULL */)
{
	ASSERT(pWnd != NULL);

	// Store the tool information
	TipToolInfo Info;
	Info.szText = szTooltipText;
	Info.hIcon	= hIcon;

	// Add the tool 
	m_ToolMap.SetAt(pWnd->m_hWnd, Info);
};

/////////////////////////////////////////////////////////////////////
// 
// CXInfoTip::RemoveTool()
// 
// DESCRIPTION
//     
//		Removes a tool
//
// RETURNS
//
//		[void]	
//
// PARAMETERS
//
//		[pWnd]			- Pointer to the tool window to remove
//     
/////////////////////////////////////////////////////////////////////
void CXInfoTip::RemoveTool(CWnd *pWnd)
{
	m_ToolMap.RemoveKey(pWnd->m_hWnd);
}
