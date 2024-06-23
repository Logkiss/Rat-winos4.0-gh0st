// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once
#include <SDKDDKVer.h>
// #ifndef _SECURE_ATL
// #define _SECURE_ATL 1
// #endif
// 
// // #ifndef VC_EXTRALEAN
// // #define _WIN32_WINNT 0x0501
// // #endif
// 
// // Modify the following defines if you have to target a platform prior to the ones specified below.
// // Refer to MSDN for the latest info on corresponding values for different platforms.
// #ifndef WINVER              // Allow use of features specific to Windows 95 and Windows NT 4 or later.
// #define WINVER 0x0500       // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
// #endif
// 
// #ifndef _WIN32_WINNT        // Allow use of features specific to Windows NT 4 or later.
// #define _WIN32_WINNT 0x0601     // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
// #endif
// 
// #ifndef _WIN32_WINDOWS      // Allow use of features specific to Windows 98 or later.
// #define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
// #endif
// 
// #ifndef _WIN32_IE           // Allow use of features specific to IE 4.0 or later.
// #define _WIN32_IE 0x0500    // Change this to the appropriate value to target IE 5.0 or later.
// #endif
// 
// #define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  // some CString constructors will be explicit
// 
// // turns off MFC's hiding of some common and often safely ignored warning messages
// #define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxtempl.h>
#include <winsock2.h>
#include <winioctl.h>
#include "Public.h"
#include "include\IOCPServer.h"
#include "vfw.h" // DrawDibOpen
#pragma comment(lib, "vfw32.lib")

#include <XTToolkitPro.h>    // Xtreme Toolkit Pro components
#include <afxcontrolbars.h>


enum 
{
	WM_ADDTOLIST = WM_USER + 102,	// 添加到列表视图中
	WM_REMOVEFROMLIST,				// 从列表视图中删除
	WM_OPENMANAGERDIALOG,			// 打开一个文件管理窗口
	WM_OPENSCREENSPYDIALOG,			// 打开一个屏幕监视窗口
	WM_OPENKEYBOARDDIALOG,          // 打开一个键盘记录窗口
	WM_OPENURLDIALOG,               // 打开一个浏览记录窗口
	WM_OPENPSLISTDIALOG,			// 打开一个进程管理窗口
	WM_OPENSERVICEDIALOG,           // 打开一个服务管理窗口
	WM_OPENREGEDITDIALOG,           // 打开一个注册表管理窗口
	WM_OPENSHELLDIALOG,				// 打开一个shell窗口
	//////////////////////////////////////////////////////////////////////////
	FILEMANAGER_DLG = 1,
	SCREENSPY_DLG,
	KEYBOARD_DLG,
	SYSTEM_DLG,
	SERVICE_DLG,
	REGEDIT_DLG,
	SHELL_DLG,
	URL_DLG
};
typedef struct 
{
	DWORD	dwSizeHigh;
	DWORD	dwSizeLow;
}FILESIZE;


/*
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif*/



