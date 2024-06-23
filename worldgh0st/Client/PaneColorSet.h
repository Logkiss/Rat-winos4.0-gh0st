// PaneColorSet.h: interface for the CPaneColorSet class.
//
// (c)1998-2018 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__PANECOLORSET_H__)
#define __PANECOLORSET_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _XTP_INCLUDE_DOCKINGPANE

//===========================================================================
// Summary: Helper class used to draw thin borders for docking pane child 
//          windows.
//===========================================================================
class CPaneColorSet  
{
public:
    //-----------------------------------------------------------------------
    // Summary: Default constructor
    //-----------------------------------------------------------------------
	CPaneColorSet();

    //-----------------------------------------------------------------------
    // Summary: Destructor, handles clean-up and deallocation
    //-----------------------------------------------------------------------
	virtual ~CPaneColorSet();

    //-----------------------------------------------------------------------
    // Summary:    Call this member function to update the colors for the 
	//             specified theme.
    // Parameters: nTheme : Specifies current docking pane theme.
    //-----------------------------------------------------------------------
	virtual void RefreshMetrics(XTPDockingPanePaintTheme nTheme);

    //-----------------------------------------------------------------------
    // Summary:    Call this member to borders for non client area.
    // Parameters: pDC     : Points to a valid device context.
	//             rWindow : Size of the window to draw.
	//             pPane   : Pointer to a CXPTDockingPane object.
    //-----------------------------------------------------------------------
	virtual void DrawBorders(CDC* pDC, CRect rWindow, CXTPDockingPane* pPane = NULL);

	COLORREF m_clrText;       // Specifies window text color.
	COLORREF m_clrBack;       // Specifies window background color.
	COLORREF m_clrBorderLite; // Specifies window light border color.
	COLORREF m_clrBorderDark; // Specifies window dark border color.

	XTPDockingPanePaintTheme m_nTheme; // Specifies current docking pane theme.
};

#endif /*_XTP_INCLUDE_DOCKINGPANE*/

#endif // !defined(__PANECOLORSET_H__)
