// ShenChenHWenJian.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <STDIO.H>
#include <Windows.h>
BOOL CHAT(char *FileName) //生成(CHAT.h)
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
	LPVOID pOutBuff;
	HANDLE hOutFile;
	DWORD BytesWritten;
	
	hDllFile = CreateFile(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwDllSize  = GetFileSize(hDllFile, 0);
	
	pDllBuff = VirtualAlloc(NULL, dwDllSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pDllBuff == NULL)
	{
		CloseHandle(hDllFile);
		return FALSE;
	}
	if (!ReadFile(hDllFile, pDllBuff, dwDllSize, &BytesRead, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	CloseHandle(hDllFile);
	
	pOutBuff = VirtualAlloc(NULL, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pOutBuff == NULL)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	
	char *pDllChar = (char *)pDllBuff;
	char *pOutChar = (char *)pOutBuff;
	for (DWORD i = 0; i < dwDllSize; i++)
	{
		if (i == dwDllSize - 1)
		{
			sprintf(pOutChar, "0x%0.2X", (unsigned char)*pDllChar++);
			pOutChar += 4;
		}
		else
		{
			sprintf(pOutChar, "0x%0.2X,", (unsigned char)*pDllChar++);
			pOutChar += 5;
		}
		
		if (i % 32 == 31 || i == dwDllSize - 1)
		{
			*pOutChar++ = '\r';
			*pOutChar++ = '\n';
		}
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	
	char OutData2[] = "#ifndef _FUNC_CHAT_\r\n#define _FUNC_CHAT_\r\n#include <windows.h>\r\n\r\n";
	char OutData3[64] = {0};
	sprintf(OutData3, "const CHATMyFileSize = %d;\r\n", dwDllSize);
	char OutData4[] = "unsigned char CHATMyFileBuf[] = {\r\n";
	char OutData5[] = "};\r\n\r\n#endif\r\n";
	hOutFile = CreateFile("C_CHAT.h", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData2, strlen(OutData2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData3, strlen(OutData3), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData4, strlen(OutData4), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, pOutBuff, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData5, strlen(OutData5), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	VirtualFree(pOutBuff, 0, MEM_RELEASE);
	CloseHandle(hOutFile);
	return TRUE;
}
BOOL FILES(char *FileName) //生成(C_FILE.h)
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
	LPVOID pOutBuff;
	HANDLE hOutFile;
	DWORD BytesWritten;
	
	hDllFile = CreateFile(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwDllSize  = GetFileSize(hDllFile, 0);
	
	pDllBuff = VirtualAlloc(NULL, dwDllSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pDllBuff == NULL)
	{
		CloseHandle(hDllFile);
		return FALSE;
	}
	if (!ReadFile(hDllFile, pDllBuff, dwDllSize, &BytesRead, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	CloseHandle(hDllFile);
	
	pOutBuff = VirtualAlloc(NULL, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pOutBuff == NULL)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	
	char *pDllChar = (char *)pDllBuff;
	char *pOutChar = (char *)pOutBuff;
	for (DWORD i = 0; i < dwDllSize; i++)
	{
		if (i == dwDllSize - 1)
		{
			sprintf(pOutChar, "0x%0.2X", (unsigned char)*pDllChar++);
			pOutChar += 4;
		}
		else
		{
			sprintf(pOutChar, "0x%0.2X,", (unsigned char)*pDllChar++);
			pOutChar += 5;
		}
		
		if (i % 32 == 31 || i == dwDllSize - 1)
		{
			*pOutChar++ = '\r';
			*pOutChar++ = '\n';
		}
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	
	char OutData2[] = "#ifndef _FUNC_FILE_\r\n#define _FUNC_FILE_\r\n#include <windows.h>\r\n\r\n";
	char OutData3[64] = {0};
	sprintf(OutData3, "const FILEMyFileSize = %d;\r\n", dwDllSize);
	char OutData4[] = "unsigned char FILEMyFileBuf[] = {\r\n";
	char OutData5[] = "};\r\n\r\n#endif\r\n";

	hOutFile = CreateFile("C_FILE.h", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData2, strlen(OutData2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData3, strlen(OutData3), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData4, strlen(OutData4), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, pOutBuff, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData5, strlen(OutData5), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	VirtualFree(pOutBuff, 0, MEM_RELEASE);
	CloseHandle(hOutFile);
	return TRUE;
}
BOOL KEYLOG(char *FileName) //生成(C_KEYLOG.h)
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
	LPVOID pOutBuff;
	HANDLE hOutFile;
	DWORD BytesWritten;
	
	hDllFile = CreateFile(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwDllSize  = GetFileSize(hDllFile, 0);
	
	pDllBuff = VirtualAlloc(NULL, dwDllSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pDllBuff == NULL)
	{
		CloseHandle(hDllFile);
		return FALSE;
	}
	if (!ReadFile(hDllFile, pDllBuff, dwDllSize, &BytesRead, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	CloseHandle(hDllFile);
	
	pOutBuff = VirtualAlloc(NULL, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pOutBuff == NULL)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	
	char *pDllChar = (char *)pDllBuff;
	char *pOutChar = (char *)pOutBuff;
	for (DWORD i = 0; i < dwDllSize; i++)
	{
		if (i == dwDllSize - 1)
		{
			sprintf(pOutChar, "0x%0.2X", (unsigned char)*pDllChar++);
			pOutChar += 4;
		}
		else
		{
			sprintf(pOutChar, "0x%0.2X,", (unsigned char)*pDllChar++);
			pOutChar += 5;
		}
		
		if (i % 32 == 31 || i == dwDllSize - 1)
		{
			*pOutChar++ = '\r';
			*pOutChar++ = '\n';
		}
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	
	char OutData2[] = "#ifndef _FUNC_KEYLOG_\r\n#define _FUNC_KEYLOG_\r\n#include <windows.h>\r\n\r\n";
	char OutData3[64] = {0};
	sprintf(OutData3, "const KEYLOGMyFileSize = %d;\r\n", dwDllSize);
	char OutData4[] = "unsigned char KEYLOGMyFileBuf[] = {\r\n";
	char OutData5[] = "};\r\n\r\n#endif\r\n";
	hOutFile = CreateFile("C_KEYLOG.h", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData2, strlen(OutData2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData3, strlen(OutData3), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData4, strlen(OutData4), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, pOutBuff, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData5, strlen(OutData5), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}

	VirtualFree(pOutBuff, 0, MEM_RELEASE);
	CloseHandle(hOutFile);
	return TRUE;
}
BOOL LISTEN(char *FileName) //生成(C_LISTEN.h)
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
	LPVOID pOutBuff;
	HANDLE hOutFile;
	DWORD BytesWritten;
	
	hDllFile = CreateFile(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwDllSize  = GetFileSize(hDllFile, 0);
	
	pDllBuff = VirtualAlloc(NULL, dwDllSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pDllBuff == NULL)
	{
		CloseHandle(hDllFile);
		return FALSE;
	}
	if (!ReadFile(hDllFile, pDllBuff, dwDllSize, &BytesRead, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	CloseHandle(hDllFile);
	
	pOutBuff = VirtualAlloc(NULL, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pOutBuff == NULL)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	
	char *pDllChar = (char *)pDllBuff;
	char *pOutChar = (char *)pOutBuff;
	for (DWORD i = 0; i < dwDllSize; i++)
	{
		if (i == dwDllSize - 1)
		{
			sprintf(pOutChar, "0x%0.2X", (unsigned char)*pDllChar++);
			pOutChar += 4;
		}
		else
		{
			sprintf(pOutChar, "0x%0.2X,", (unsigned char)*pDllChar++);
			pOutChar += 5;
		}
		
		if (i % 32 == 31 || i == dwDllSize - 1)
		{
			*pOutChar++ = '\r';
			*pOutChar++ = '\n';
		}
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	
	char OutData2[] = "#ifndef _FUNC_LISTEN_\r\n#define _FUNC_LISTEN_\r\n#include <windows.h>\r\n\r\n";
	char OutData3[64] = {0};
	sprintf(OutData3, "const LISTENMyFileSize = %d;\r\n", dwDllSize);
	char OutData4[] = "unsigned char LISTENMyFileBuf[] = {\r\n";
	char OutData5[] = "};\r\n\r\n#endif\r\n";
	hOutFile = CreateFile("C_LISTEN.h", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData2, strlen(OutData2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData3, strlen(OutData3), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData4, strlen(OutData4), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, pOutBuff, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData5, strlen(OutData5), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	VirtualFree(pOutBuff, 0, MEM_RELEASE);
	CloseHandle(hOutFile);
	return TRUE;
}
BOOL PRANK(char *FileName) //生成(C_PRANK.h)
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
	LPVOID pOutBuff;
	HANDLE hOutFile;
	DWORD BytesWritten;
	
	hDllFile = CreateFile(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwDllSize  = GetFileSize(hDllFile, 0);
	
	pDllBuff = VirtualAlloc(NULL, dwDllSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pDllBuff == NULL)
	{
		CloseHandle(hDllFile);
		return FALSE;
	}
	if (!ReadFile(hDllFile, pDllBuff, dwDllSize, &BytesRead, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	CloseHandle(hDllFile);
	
	pOutBuff = VirtualAlloc(NULL, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pOutBuff == NULL)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	
	char *pDllChar = (char *)pDllBuff;
	char *pOutChar = (char *)pOutBuff;
	for (DWORD i = 0; i < dwDllSize; i++)
	{
		if (i == dwDllSize - 1)
		{
			sprintf(pOutChar, "0x%0.2X", (unsigned char)*pDllChar++);
			pOutChar += 4;
		}
		else
		{
			sprintf(pOutChar, "0x%0.2X,", (unsigned char)*pDllChar++);
			pOutChar += 5;
		}
		
		if (i % 32 == 31 || i == dwDllSize - 1)
		{
			*pOutChar++ = '\r';
			*pOutChar++ = '\n';
		}
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	
	char OutData2[] = "#ifndef _FUNC_PRANK_\r\n#define _FUNC_PRANK_\r\n#include <windows.h>\r\n\r\n";
	char OutData3[64] = {0};
	sprintf(OutData3, "const PRANKMyFileSize = %d;\r\n", dwDllSize);
	char OutData4[] = "unsigned char PRANKMyFileBuf[] = {\r\n";
	char OutData5[] = "};\r\n\r\n#endif\r\n";
	hOutFile = CreateFile("C_PRANK.h", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData2, strlen(OutData2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData3, strlen(OutData3), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData4, strlen(OutData4), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, pOutBuff, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData5, strlen(OutData5), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	VirtualFree(pOutBuff, 0, MEM_RELEASE);
	CloseHandle(hOutFile);
	return TRUE;
}
BOOL PROXY(char *FileName) //生成(C_PROXY.h)
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
	LPVOID pOutBuff;
	HANDLE hOutFile;
	DWORD BytesWritten;
	
	hDllFile = CreateFile(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwDllSize  = GetFileSize(hDllFile, 0);
	
	pDllBuff = VirtualAlloc(NULL, dwDllSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pDllBuff == NULL)
	{
		CloseHandle(hDllFile);
		return FALSE;
	}
	if (!ReadFile(hDllFile, pDllBuff, dwDllSize, &BytesRead, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	CloseHandle(hDllFile);
	
	pOutBuff = VirtualAlloc(NULL, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pOutBuff == NULL)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	
	char *pDllChar = (char *)pDllBuff;
	char *pOutChar = (char *)pOutBuff;
	for (DWORD i = 0; i < dwDllSize; i++)
	{
		if (i == dwDllSize - 1)
		{
			sprintf(pOutChar, "0x%0.2X", (unsigned char)*pDllChar++);
			pOutChar += 4;
		}
		else
		{
			sprintf(pOutChar, "0x%0.2X,", (unsigned char)*pDllChar++);
			pOutChar += 5;
		}
		
		if (i % 32 == 31 || i == dwDllSize - 1)
		{
			*pOutChar++ = '\r';
			*pOutChar++ = '\n';
		}
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	
	char OutData2[] = "#ifndef _FUNC_PROXY_\r\n#define _FUNC_PROXY_\r\n#include <windows.h>\r\n\r\n";
	char OutData3[64] = {0};
	sprintf(OutData3, "const PROXYMyFileSize = %d;\r\n", dwDllSize);
	char OutData4[] = "unsigned char PROXYMyFileBuf[] = {\r\n";
	char OutData5[] = "};\r\n\r\n#endif\r\n";
	hOutFile = CreateFile("C_PROXY.h", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData2, strlen(OutData2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData3, strlen(OutData3), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData4, strlen(OutData4), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, pOutBuff, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData5, strlen(OutData5), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	VirtualFree(pOutBuff, 0, MEM_RELEASE);
	CloseHandle(hOutFile);
	return TRUE;
}
BOOL PROXYMAP(char *FileName) //生成(C_PROXYMAP.h)
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
	LPVOID pOutBuff;
	HANDLE hOutFile;
	DWORD BytesWritten;
	
	hDllFile = CreateFile(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwDllSize  = GetFileSize(hDllFile, 0);
	
	pDllBuff = VirtualAlloc(NULL, dwDllSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pDllBuff == NULL)
	{
		CloseHandle(hDllFile);
		return FALSE;
	}
	if (!ReadFile(hDllFile, pDllBuff, dwDllSize, &BytesRead, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	CloseHandle(hDllFile);
	
	pOutBuff = VirtualAlloc(NULL, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pOutBuff == NULL)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	
	char *pDllChar = (char *)pDllBuff;
	char *pOutChar = (char *)pOutBuff;
	for (DWORD i = 0; i < dwDllSize; i++)
	{
		if (i == dwDllSize - 1)
		{
			sprintf(pOutChar, "0x%0.2X", (unsigned char)*pDllChar++);
			pOutChar += 4;
		}
		else
		{
			sprintf(pOutChar, "0x%0.2X,", (unsigned char)*pDllChar++);
			pOutChar += 5;
		}
		
		if (i % 32 == 31 || i == dwDllSize - 1)
		{
			*pOutChar++ = '\r';
			*pOutChar++ = '\n';
		}
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	
	char OutData2[] = "#ifndef _FUNC_PROXYMAP_\r\n#define _FUNC_PROXYMAP_\r\n#include <windows.h>\r\n\r\n";
	char OutData3[64] = {0};
	sprintf(OutData3, "const PROXYMAPMyFileSize = %d;\r\n", dwDllSize);
	char OutData4[] = "unsigned char PROXYMAPMyFileBuf[] = {\r\n";
	char OutData5[] = "};\r\n\r\n#endif\r\n";
	hOutFile = CreateFile("C_PROXYMAP.h", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData2, strlen(OutData2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData3, strlen(OutData3), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData4, strlen(OutData4), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, pOutBuff, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData5, strlen(OutData5), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	VirtualFree(pOutBuff, 0, MEM_RELEASE);
	CloseHandle(hOutFile);
	return TRUE;
}
BOOL REGEDIT(char *FileName) //生成(C_REGEDIT.h)
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
	LPVOID pOutBuff;
	HANDLE hOutFile;
	DWORD BytesWritten;
	
	hDllFile = CreateFile(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwDllSize  = GetFileSize(hDllFile, 0);
	
	pDllBuff = VirtualAlloc(NULL, dwDllSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pDllBuff == NULL)
	{
		CloseHandle(hDllFile);
		return FALSE;
	}
	if (!ReadFile(hDllFile, pDllBuff, dwDllSize, &BytesRead, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	CloseHandle(hDllFile);
	
	pOutBuff = VirtualAlloc(NULL, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pOutBuff == NULL)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	
	char *pDllChar = (char *)pDllBuff;
	char *pOutChar = (char *)pOutBuff;
	for (DWORD i = 0; i < dwDllSize; i++)
	{
		if (i == dwDllSize - 1)
		{
			sprintf(pOutChar, "0x%0.2X", (unsigned char)*pDllChar++);
			pOutChar += 4;
		}
		else
		{
			sprintf(pOutChar, "0x%0.2X,", (unsigned char)*pDllChar++);
			pOutChar += 5;
		}
		
		if (i % 32 == 31 || i == dwDllSize - 1)
		{
			*pOutChar++ = '\r';
			*pOutChar++ = '\n';
		}
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	
	char OutData2[] = "#ifndef _FUNC_REGEDIT_\r\n#define _FUNC_REGEDIT_\r\n#include <windows.h>\r\n\r\n";
	char OutData3[64] = {0};
	sprintf(OutData3, "const REGEDITMyFileSize = %d;\r\n", dwDllSize);
	char OutData4[] = "unsigned char REGEDITMyFileBuf[] = {\r\n";
	char OutData5[] = "};\r\n\r\n#endif\r\n";
	hOutFile = CreateFile("C_REGEDIT.h", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData2, strlen(OutData2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData3, strlen(OutData3), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData4, strlen(OutData4), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, pOutBuff, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData5, strlen(OutData5), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	VirtualFree(pOutBuff, 0, MEM_RELEASE);
	CloseHandle(hOutFile);
	return TRUE;
}
BOOL SCREEN(char *FileName) //生成(C_SCREEN.h)
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
	LPVOID pOutBuff;
	HANDLE hOutFile;
	DWORD BytesWritten;
	
	hDllFile = CreateFile(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwDllSize  = GetFileSize(hDllFile, 0);
	
	pDllBuff = VirtualAlloc(NULL, dwDllSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pDllBuff == NULL)
	{
		CloseHandle(hDllFile);
		return FALSE;
	}
	if (!ReadFile(hDllFile, pDllBuff, dwDllSize, &BytesRead, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	CloseHandle(hDllFile);
	
	pOutBuff = VirtualAlloc(NULL, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pOutBuff == NULL)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	
	char *pDllChar = (char *)pDllBuff;
	char *pOutChar = (char *)pOutBuff;
	for (DWORD i = 0; i < dwDllSize; i++)
	{
		if (i == dwDllSize - 1)
		{
			sprintf(pOutChar, "0x%0.2X", (unsigned char)*pDllChar++);
			pOutChar += 4;
		}
		else
		{
			sprintf(pOutChar, "0x%0.2X,", (unsigned char)*pDllChar++);
			pOutChar += 5;
		}
		
		if (i % 32 == 31 || i == dwDllSize - 1)
		{
			*pOutChar++ = '\r';
			*pOutChar++ = '\n';
		}
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	
	char OutData2[] = "#ifndef _FUNC_SCREEN_\r\n#define _FUNC_SCREEN_\r\n#include <windows.h>\r\n\r\n";
	char OutData3[64] = {0};
	sprintf(OutData3, "const SCREENMyFileSize = %d;\r\n", dwDllSize);
	char OutData4[] = "unsigned char SCREENMyFileBuf[] = {\r\n";
	char OutData5[] = "};\r\n\r\n#endif\r\n";
	hOutFile = CreateFile("C_SCREEN.h", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData2, strlen(OutData2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData3, strlen(OutData3), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData4, strlen(OutData4), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, pOutBuff, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData5, strlen(OutData5), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	VirtualFree(pOutBuff, 0, MEM_RELEASE);
	CloseHandle(hOutFile);
	return TRUE;
}
BOOL SCREENS(char *FileName) //生成(C_SCREENS.h)
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
	LPVOID pOutBuff;
	HANDLE hOutFile;
	DWORD BytesWritten;
	
	hDllFile = CreateFile(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwDllSize  = GetFileSize(hDllFile, 0);
	
	pDllBuff = VirtualAlloc(NULL, dwDllSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pDllBuff == NULL)
	{
		CloseHandle(hDllFile);
		return FALSE;
	}
	if (!ReadFile(hDllFile, pDllBuff, dwDllSize, &BytesRead, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	CloseHandle(hDllFile);
	
	pOutBuff = VirtualAlloc(NULL, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pOutBuff == NULL)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	
	char *pDllChar = (char *)pDllBuff;
	char *pOutChar = (char *)pOutBuff;
	for (DWORD i = 0; i < dwDllSize; i++)
	{
		if (i == dwDllSize - 1)
		{
			sprintf(pOutChar, "0x%0.2X", (unsigned char)*pDllChar++);
			pOutChar += 4;
		}
		else
		{
			sprintf(pOutChar, "0x%0.2X,", (unsigned char)*pDllChar++);
			pOutChar += 5;
		}
		
		if (i % 32 == 31 || i == dwDllSize - 1)
		{
			*pOutChar++ = '\r';
			*pOutChar++ = '\n';
		}
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	
	char OutData2[] = "#ifndef _FUNC_SCREENS_\r\n#define _FUNC_SCREENS_\r\n#include <windows.h>\r\n\r\n";
	char OutData3[64] = {0};
	sprintf(OutData3, "const SCREENSMyFileSize = %d;\r\n", dwDllSize);
	char OutData4[] = "unsigned char SCREENSMyFileBuf[] = {\r\n";
	char OutData5[] = "};\r\n\r\n#endif\r\n";
	hOutFile = CreateFile("C_SCREENS.h", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData2, strlen(OutData2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData3, strlen(OutData3), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData4, strlen(OutData4), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, pOutBuff, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData5, strlen(OutData5), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	VirtualFree(pOutBuff, 0, MEM_RELEASE);
	CloseHandle(hOutFile);
	return TRUE;
}
BOOL SERVICE(char *FileName) //生成(C_SERVICE.h)
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
	LPVOID pOutBuff;
	HANDLE hOutFile;
	DWORD BytesWritten;
	
	hDllFile = CreateFile(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwDllSize  = GetFileSize(hDllFile, 0);
	
	pDllBuff = VirtualAlloc(NULL, dwDllSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pDllBuff == NULL)
	{
		CloseHandle(hDllFile);
		return FALSE;
	}
	if (!ReadFile(hDllFile, pDllBuff, dwDllSize, &BytesRead, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	CloseHandle(hDllFile);
	
	pOutBuff = VirtualAlloc(NULL, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pOutBuff == NULL)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	
	char *pDllChar = (char *)pDllBuff;
	char *pOutChar = (char *)pOutBuff;
	for (DWORD i = 0; i < dwDllSize; i++)
	{
		if (i == dwDllSize - 1)
		{
			sprintf(pOutChar, "0x%0.2X", (unsigned char)*pDllChar++);
			pOutChar += 4;
		}
		else
		{
			sprintf(pOutChar, "0x%0.2X,", (unsigned char)*pDllChar++);
			pOutChar += 5;
		}
		
		if (i % 32 == 31 || i == dwDllSize - 1)
		{
			*pOutChar++ = '\r';
			*pOutChar++ = '\n';
		}
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	
	char OutData2[] = "#ifndef _FUNC_SERVICE_\r\n#define _FUNC_SERVICE_\r\n#include <windows.h>\r\n\r\n";
	char OutData3[64] = {0};
	sprintf(OutData3, "const SERVICEMyFileSize = %d;\r\n", dwDllSize);
	char OutData4[] = "unsigned char SERVICEMyFileBuf[] = {\r\n";
	char OutData5[] = "};\r\n\r\n#endif\r\n";

	hOutFile = CreateFile("C_SERVICE.h", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData2, strlen(OutData2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData3, strlen(OutData3), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData4, strlen(OutData4), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, pOutBuff, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData5, strlen(OutData5), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	VirtualFree(pOutBuff, 0, MEM_RELEASE);
	CloseHandle(hOutFile);
	return TRUE;
}
BOOL SHELL(char *FileName) //生成(C_SHELL.h)
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
	LPVOID pOutBuff;
	HANDLE hOutFile;
	DWORD BytesWritten;
	
	hDllFile = CreateFile(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwDllSize  = GetFileSize(hDllFile, 0);
	
	pDllBuff = VirtualAlloc(NULL, dwDllSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pDllBuff == NULL)
	{
		CloseHandle(hDllFile);
		return FALSE;
	}
	if (!ReadFile(hDllFile, pDllBuff, dwDllSize, &BytesRead, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	CloseHandle(hDllFile);
	
	pOutBuff = VirtualAlloc(NULL, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pOutBuff == NULL)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	
	char *pDllChar = (char *)pDllBuff;
	char *pOutChar = (char *)pOutBuff;
	for (DWORD i = 0; i < dwDllSize; i++)
	{
		if (i == dwDllSize - 1)
		{
			sprintf(pOutChar, "0x%0.2X", (unsigned char)*pDllChar++);
			pOutChar += 4;
		}
		else
		{
			sprintf(pOutChar, "0x%0.2X,", (unsigned char)*pDllChar++);
			pOutChar += 5;
		}
		
		if (i % 32 == 31 || i == dwDllSize - 1)
		{
			*pOutChar++ = '\r';
			*pOutChar++ = '\n';
		}
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	
	char OutData2[] = "#ifndef _FUNC_SHELL_\r\n#define _FUNC_SHELL_\r\n#include <windows.h>\r\n\r\n";
	char OutData3[64] = {0};
	sprintf(OutData3, "const SHELLMyFileSize = %d;\r\n", dwDllSize);
	char OutData4[] = "unsigned char SHELLMyFileBuf[] = {\r\n";
	char OutData5[] = "};\r\n\r\n#endif\r\n";
	hOutFile = CreateFile("C_SHELL.h", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData2, strlen(OutData2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData3, strlen(OutData3), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData4, strlen(OutData4), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, pOutBuff, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData5, strlen(OutData5), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	VirtualFree(pOutBuff, 0, MEM_RELEASE);
	CloseHandle(hOutFile);
	return TRUE;
}
BOOL SYSTEM(char *FileName) //生成(C_SYSTEM.h)
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
	LPVOID pOutBuff;
	HANDLE hOutFile;
	DWORD BytesWritten;
	
	hDllFile = CreateFile(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwDllSize  = GetFileSize(hDllFile, 0);
	
	pDllBuff = VirtualAlloc(NULL, dwDllSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pDllBuff == NULL)
	{
		CloseHandle(hDllFile);
		return FALSE;
	}
	if (!ReadFile(hDllFile, pDllBuff, dwDllSize, &BytesRead, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	CloseHandle(hDllFile);
	
	pOutBuff = VirtualAlloc(NULL, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pOutBuff == NULL)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	
	char *pDllChar = (char *)pDllBuff;
	char *pOutChar = (char *)pOutBuff;
	for (DWORD i = 0; i < dwDllSize; i++)
	{
		if (i == dwDllSize - 1)
		{
			sprintf(pOutChar, "0x%0.2X", (unsigned char)*pDllChar++);
			pOutChar += 4;
		}
		else
		{
			sprintf(pOutChar, "0x%0.2X,", (unsigned char)*pDllChar++);
			pOutChar += 5;
		}
		
		if (i % 32 == 31 || i == dwDllSize - 1)
		{
			*pOutChar++ = '\r';
			*pOutChar++ = '\n';
		}
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	
	char OutData2[] = "#ifndef _FUNC_SYSTEM_\r\n#define _FUNC_SYSTEM_\r\n#include <windows.h>\r\n\r\n";
	char OutData3[64] = {0};
	sprintf(OutData3, "const SYSTEMMyFileSize = %d;\r\n", dwDllSize);
	char OutData4[] = "unsigned char SYSTEMMyFileBuf[] = {\r\n";
	char OutData5[] = "};\r\n\r\n#endif\r\n";
	hOutFile = CreateFile("C_SYSTEM.h", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData2, strlen(OutData2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData3, strlen(OutData3), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData4, strlen(OutData4), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, pOutBuff, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData5, strlen(OutData5), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}

	VirtualFree(pOutBuff, 0, MEM_RELEASE);
	CloseHandle(hOutFile);
	return TRUE;
}
BOOL VIDEO(char *FileName) //生成(C_VIDEO.h)
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
	LPVOID pOutBuff;
	HANDLE hOutFile;
	DWORD BytesWritten;
	
	hDllFile = CreateFile(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwDllSize  = GetFileSize(hDllFile, 0);
	
	pDllBuff = VirtualAlloc(NULL, dwDllSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pDllBuff == NULL)
	{
		CloseHandle(hDllFile);
		return FALSE;
	}
	if (!ReadFile(hDllFile, pDllBuff, dwDllSize, &BytesRead, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	CloseHandle(hDllFile);
	
	pOutBuff = VirtualAlloc(NULL, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (pOutBuff == NULL)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	
	char *pDllChar = (char *)pDllBuff;
	char *pOutChar = (char *)pOutBuff;
	for (DWORD i = 0; i < dwDllSize; i++)
	{
		if (i == dwDllSize - 1)
		{
			sprintf(pOutChar, "0x%0.2X", (unsigned char)*pDllChar++);
			pOutChar += 4;
		}
		else
		{
			sprintf(pOutChar, "0x%0.2X,", (unsigned char)*pDllChar++);
			pOutChar += 5;
		}
		
		if (i % 32 == 31 || i == dwDllSize - 1)
		{
			*pOutChar++ = '\r';
			*pOutChar++ = '\n';
		}
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	
	char OutData2[] = "#ifndef _FUNC_VIDEO_\r\n#define _FUNC_VIDEO_\r\n#include <windows.h>\r\n\r\n";
	char OutData3[64] = {0};
	sprintf(OutData3, "const VIDEOMyFileSize = %d;\r\n", dwDllSize);
	char OutData4[] = "unsigned char VIDEOMyFileBuf[] = {\r\n";
	char OutData5[] = "};\r\n\r\n#endif\r\n";
	
	hOutFile = CreateFile("C_VIDEO.h", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData2, strlen(OutData2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData3, strlen(OutData3), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData4, strlen(OutData4), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, pOutBuff, (dwDllSize*5-1)+((dwDllSize+32-1)/32*2), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData5, strlen(OutData5), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}

	VirtualFree(pOutBuff, 0, MEM_RELEASE);
	CloseHandle(hOutFile);
	return TRUE;
}

int main(int argc, char* argv[])
{
	if (argc == 3)
	{
		if (argv[1][0] != '-') return -1;
		
		switch (argv[1][1])
		{
		case 'A':
			return CHAT(argv[2]) ? 0 : -1;
		case 'B':
			return FILES(argv[2]) ? 0 : -1;
		case 'C':
			return KEYLOG(argv[2]) ? 0 : -1;
		case 'D':
			return LISTEN(argv[2]) ? 0 : -1;
		case 'E':
			return PRANK(argv[2]) ? 0 : -1;
		case 'F':
			return PROXY(argv[2]) ? 0 : -1;
		case 'G':
			return PROXYMAP(argv[2]) ? 0 : -1;
		case 'H':
			return REGEDIT(argv[2]) ? 0 : -1;
		case 'I':
			return SCREEN(argv[2]) ? 0 : -1;
		case 'J':
			return SCREENS(argv[2]) ? 0 : -1;
		case 'K':
			return SERVICE(argv[2]) ? 0 : -1;
		case 'L':
			return SHELL(argv[2]) ? 0 : -1;
		case 'M':
			return SYSTEM(argv[2]) ? 0 : -1;
		case 'N':
			return VIDEO(argv[2]) ? 0 : -1;
		default:
			return -1;
		}
	}
	return 0;
}
