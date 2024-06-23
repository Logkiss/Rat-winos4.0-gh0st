// PROXY.cpp : Defines the entry point for the DLL application.
//

#include "OpenProxy.h"
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(linker, "/ENTRY:_DllMain")
#pragma comment(linker,"/opt:nowin98")

BOOL APIENTRY _DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					   )
{
    return TRUE;
}

extern "C" __declspec(dllexport) BOOL OpenProxy(LPVOID lparam)     
{
	ProxyMain(lparam);
	return 0;
}

extern "C" __declspec(dllexport) BOOL CloseProxy()     
{
	STOPProxy();
	return 0;
}

