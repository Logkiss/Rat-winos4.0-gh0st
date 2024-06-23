/***************************************************************************

// This is a part of the SkinMagic SDK library.
// Copyright (C) 2003-2002 Appspeed Inc.
// All rights reserved.
//
// This source code is only intended as a supplement to the
// SkinMagic SDK Reference and related electronic documentation 
// provided with the library.
// See these sources for detailed information regarding the
// SkinMagic SDK product.

// SkinMagicLib SDK is a library designed for C/C++ programmer, 
// you can add the most popular function of changing skin to your
// application with the API supplied by this library. SkinMagic 
// is picture-based skin system, you can power your application
// with skin-changeable with simple call of several functions. 
// And with the skin editor of SkinMagicBuilder, you can customize
// your own skin.

*****************************************************************************/
#ifndef _SKINMAGICLIB_H__
#define _SKINMAGICLIB_H__

#include <windows.h>

///////////////////////////////////////////////////////////////////////////
//Function: InitSkinMagicLib(HINSTANCE hInstance, char* szApplication , 
//					  char* szReserved1,
//					  char* szReserved2
//			
//Parameters: 
//		hInstance 
//			[in] Handle of application instance.
//		szApplication
//			[in] Application defined in skin file. If you set this parameter the 
//			library can only load the skin file which application object named
//			match szApplication.
//		szReserved1
//			[in] Reserved.
//		szReserved2
//			[in] Reserved.
//Return Values:
//   If the function success, the return value is nonzero
//   If the function fail, the return value is zero
//Remarks:
//	 InitSkinMagicLib function should be the first API called in SkinMagicLib SDK,
//	 followed by other APIs. 
//	 For multithreaded applications, you must call InitSkinMagicLib in every thread. 
//   
int InitSkinMagicLib( HINSTANCE hInstance, char* szApplication , 
					  char* szReserved1,
					  char* szReserved2 );

///////////////////////////////////////////////////////////////////////////
//Function: LoadSkinFile( char* SkinFile )
//			Load skin from file.
//Parameters: 
//		SkinFile 
//			[in] Specifies the skin file name. 
//Return Values:
//   If the function success, the return value is nonzero
//   If the function fail, the return value is zero
//Remarks:
//	 ExitSkinMagicLib function should be the last API called in SkinMagicLib SDK.
//	 For multithreaded applications, you must call InitSkinMagicLib in every thread. 
void ExitSkinMagicLib();

///////////////////////////////////////////////////////////////////////////
//Function: LoadSkinFile( char* SkinFile )
//			Load skin from file.
//Parameters: 
//		SkinFile 
//			[in] Specifies the skin file name. 
//Return Values:
//   If the function success, the return value is nonzero
//   If the function fail, the return value is zero
//Remarks:
//		Load skin from file.	
int LoadSkinFile( char* SkinFile );


///////////////////////////////////////////////////////////////////////////
//Function: LoadSkinFromResource(HMODULE hModule, char* lpSkinName ,char* lpType)
//			Use this function to load skin from resource.
//Parameters: 
//		hModule 
//			[in] Handle to the module whose executable file contains the resource. 
//			A value of NULL specifies the module handle associated with the image 
//			file that the operating system used to create the current process. 
//		lpSkinName 
//			[in] Specifies the name of the skin resource. 
//		lpType 
//			[in] Specifies the resource type. 

//Return Values:
//   If the function success, the return value is nonzero
//   If the function fail, the return value is zero
//Remarks:
//		The application can use skin defined in resource.
int LoadSkinFromResource(HMODULE hModule, char* lpSkinName ,char* lpType);

///////////////////////////////////////////////////////////////////////////
//Function: SetWindowSkin( HWND hWnd , char* SkinName )
//			Use SetWindowSkin to skin the standard windows - with rectangular
//          opaque client area and region-based non-client area, menu bar, 
//          system/maximize/minimize/close buttons, caption and sizing edges
//          and corners. 
//			
//Parameters: 
//   hWnd
//		[in]Handle to the window to be remove skin.     
//   SkinName
//		[in]Specifies the name of FrameWnd object which defined in skin file. 
//
//Return Values:
//   If the function success, the return value is nonzero
//   If the function fail, the return value is zero
//Remarks:
//			Use SetWindowSkin to skin the standard windows - with rectangular
//          opaque client area and region-based non-client area, menu bar, 
//          system/maximize/minimize/close buttons, caption and sizing edges
//          and corners.
int SetWindowSkin( HWND hWnd , char* SkinName );


///////////////////////////////////////////////////////////////////////////
//Function: SetShapeWindowSkin( HWND hWnd , char* SkinName )
//			Use SetShapeWindowSkin to skin application-specific windows 
//			without non-client area and menu bar. These windows can have
//			absolutely any shape or even be translucent. 
//			
//Parameters: 
//   hWnd
//		[in]Handle to the window to be remove skin.     
//   SkinName
//		[in]Specifies the name of ShapeWnd object which defined in skin file. 
//
//Return Values:
//   If the function success, the return value is nonzero
//   If the function fail, the return value is zero
//Remarks:
//	 Use SetShapeWindowSkin to skin application-specific windows 
//	 without non-client area and menu bar. These windows can have
//	 absolutely any shape or even be translucent.
//
int SetShapeWindowSkin( HWND hWnd , char* SkinName );


///////////////////////////////////////////////////////////////////////////
//Function: RemoveWindowSkin( HWND hWnd )
//			set skin of new create dialog
//			
//Parameters: 
//   hWnd
//		[in]Handle to the window to be remove skin.      
//
//Return Values:
//   If the function success, the return value is nonzero
//   If the function fail, the return value is zero
//Remarks:
//	 Use this function to remove window skin.
//
int RemoveWindowSkin( HWND hWnd );


///////////////////////////////////////////////////////////////////////////
//Function: SetDialogSkin( char* szSkinName )
//			set skin of new create dialog
//			
//Parameters: 
//   szSkinName
//      [in]Specifies the name of FrameWnd object which defined in skin file.      
//
//Return Values:
//   If the function success, the return value is nonzero
//   If the function fail, the return value is zero
//Remarks:
//	 Use this function to set dialog skin. dialog will auto be skinned
//   after this function called.
//
int SetDialogSkin( char* szSkinName );

///////////////////////////////////////////////////////////////////////////
//Function: SetSingleDialogSkin( HWND hWnd ,char* szSkinName )
//			Set the skin of an dialog
//			
//Parameters: 
//   hWnd
//		[in]Handle to the dialog to be set skin. 
//   szSkinName
//      [in]Specifies the skin nanem.      
//
//Return Values:
//   If the function success, the return value is nonzero
//   If the function fail, the return value is zero
//Remarks:
//	 Use this function to set single dialog skin.
//
int SetSingleDialogSkin( HWND hWnd , char* szSkinName );

///////////////////////////////////////////////////////////////////////////
//Function: RemoveDialogSkin()
//			This function will stop auto skin dialog. 
//			the function will not remove the dialog's
//          skin which already set. The new create dialog will have no skin.
//			
//Parameters: 
//			none.
//Return Values:
//   If the function success, the return value is nonzero
//   If the function fail, the return value is zero
//Remarks:
//	 Use this function to stop auto skin dialog.
//
int RemoveDialogSkin();


///////////////////////////////////////////////////////////////////////////
//Function: EnableWindowScrollbarSkin( HWND hWnd ,int fnBar )
//			Initialize the skin scrollbars for a window
//			
//Parameters: 
//   hWnd
//		[in]Handle to the window to be set skin scrollbar. 
//   fnBar
//      [in]Specifies the type of scroll bar for which to skin. 
//		This parameter can be one of the following values       
//		SB_HORZ for the window's standard horizontal scroll bar.  
//		SB_VERT for the window's standard  scroll bar.  
//		SB_BOTH for both horizontal and vertical scroll bar.   
//
//Return Values:
//   If the function success, the return value is nonzero
//   If the function fail, the return value is zero
//Remarks:
//	 Use this function to skin your window scrollbar
//
int EnableWindowScrollbarSkin( HWND hWnd ,int fnBar );


///////////////////////////////////////////////////////////////////////////
//Function: DisableWindowScrollbarSkin( HWND hWnd )
//			disable the skin scrollbars for a window
//			
//Parameters: 
//   hWnd
//		[in]Handle to the window to be disable skin scrollbar. 
//
//Return Values:
//   If the function success, the return value is nonzero
//   If the function fail, the return value is zero
//Remarks:
//	 Use this function to skin your window scrollbar
//
int DisableWindowScrollbarSkin( HWND hWnd );

///////////////////////////////////////////////////////////////////////////
//Function: TrackSkinPopupMenu( HMENU hMenu,int x, int y, HWND hWnd )
//			The TrackSkinPopupMenu function displays a shortcut menu at the 
//			specified location and tracks the selection of items on the menu.
//			The shortcut menu can appear anywhere on the screen.
//			
//Parameters
//		hMenu 
//			[in] Handle to the shortcut menu to be displayed. 
//				 The handle can be obtained by calling CreatePopupMenu
//				 to create a new shortcut menu, or by calling GetSubMenu
//				 to retrieve a handle to a submenu associated with an 
//				 existing menu item. 
//uFlags 
//			[in] Use zero of more of these flags to specify function options. 
//			Use one of the following flags to specify how the function positions
//			the shortcut menu horizontally. 
//			------------------------------------------------------------------------------
//			TPM_CENTERALIGN		If this flag is set, the function centers the shortcut
//								menu horizontally relative to the coordinate specified
//								by the x parameter. 
//			TPM_LEFTALIGN		If this flag is set, the function positions the shortcut
//								menu so that its left side is aligned with the coordinate
//								specified by the x parameter. 
//			TPM_RIGHTALIGN		Positions the shortcut menu so that its right side is 
//								aligned with the coordinate specified by the x parameter. 
//			------------------------------------------------------------------------------
//			Use one of the following flags to specify how the function positions the shortcut
//			menu vertically. 
//			TPM_BOTTOMALIGN		If this flag is set, the function positions the shortcut 
//								menu so that its bottom side is aligned with the coordinate 
//								specified by the y parameter. 
//			TPM_TOPALIGN		If this flag is set, the function positions the shortcut
//								menu so that its top side is aligned with the coordinate 
//								specified by the y parameter. 
//			TPM_VCENTERALIGN	If this flag is set, the function centers the shortcut menu 
//								vertically relative to the coordinate specified by the y parameter. 
//
//		x 
//			[in] Specifies the horizontal location of the shortcut menu, 
//				 in screen coordinates. 
//		y 
//			[in] Specifies the vertical location of the shortcut menu, 
//				 in screen coordinates. 
//		hWnd 
//			[in] Handle to the window that owns the shortcut menu. 
//				 This window receives all messages from the menu. 
//				 The window does not receive a WM_COMMAND message from the
//				 menu until the function returns. 
BOOL TrackSkinPopupMenu(  HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd );


#endif
