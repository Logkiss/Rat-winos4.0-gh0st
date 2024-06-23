// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__1DD676CB_55D6_4485_812A_D85014872E24__INCLUDED_)
#define AFX_STDAFX_H__1DD676CB_55D6_4485_812A_D85014872E24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define _WIN32_WINNT 0x0500  ///用户状态显示头文件必须要有

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <AFX.h>
#include <stdio.h>
#include <windows.h>
#include "MyFunc.h"
#include "ClientSocket.h"
#include <SHELLAPI.h>

struct DLL_INFO
{
	char LoginAddr[100];    //上线地址
	UINT LoginPort;
	char ServiceName[50];//服务名称
	char ServiceDisplayName[50]; //服务显示
	char ServiceDescription[150];  //服务描述
	char UpGroup[32];       //分组
	char strRemark[32];     //备注
	BOOL NoInstall;        // TRUE 绿色安装, FALSE 正常
	char ConnetPass[20];
	BOOL Dele_Kzj;                //离线记录
	CHAR szDownRun[300];     //捆绑地址
};

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__1DD676CB_55D6_4485_812A_D85014872E24__INCLUDED_)

