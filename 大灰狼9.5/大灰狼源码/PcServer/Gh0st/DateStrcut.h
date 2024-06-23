#include "StdAfx.h"

struct INSTALL_INFO
{
	char PathName[100];      // 安装路径
	BOOL SysStart;           // TRUE 绿色安装, FALSE 正常
};


struct DLLSERVER_INFO
{
	char Domain[100];
	UINT Port;
	char Version[100];
	char Group[100];
	char SocketHead[100];
};

struct POSTXML
{
	char szCommand[16];
	char szUserName[18];
	char szPassWord[33];
	char szVersion[4];
	HWND mHwnd;
	LPSTR RequestBuffer;
	int nRequestLong;
	int LoginSucces;
	HANDLE eventHandle;
};

struct RECVXML
{
	char szHost[120];
	char szPort[6];
	char szVersion[40];
};
