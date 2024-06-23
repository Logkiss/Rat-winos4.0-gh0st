// ShellCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <Windows.h>

void EncryptPlug(char *szRec,long nLen,long key) //加密插件
{
	unsigned long i;
	unsigned char p;
	
	p = (unsigned char ) key % 1451 + 61;
	for(i = 0; i < nLen; i++)
	{
		*szRec -= p;
		*szRec++ ^= p;
	}
}

void EncryptData(char *szRec,long nLen,long key) //解密插件
{
	unsigned long i;
	unsigned char p;
	
	p = (unsigned char ) key % 1451 + 61;
	for(i = 0; i < nLen; i++)
	{
		*szRec ^= p;
		*szRec += p;
		szRec++;
	}
}
BOOL SaveMain(char *FileName) //生成加密后的文件(Server.dll)
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
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
	
	EncryptPlug((char *)pDllBuff, dwDllSize, 1024);
	hDllFile = CreateFile(FileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hDllFile, pDllBuff, dwDllSize, &BytesWritten, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	CloseHandle(hDllFile);
	return TRUE;
}

BOOL SaveCode(char *FileName) //生成ShellCode(ShellCode.h)
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
	
	char OutData1[] = "/*\r\n ShellCode转换器 By Anonymity\r\n My QQ ????????\r\n"
		" 直接include此单元，使用 ShellCodeSaveFile(\"xxx.xxx\");即可生成文件\r\n*/\r\n\r\n";
	char OutData2[] = "#ifndef _HEX_SHELLCODE_\r\n#define _HEX_SHELLCODE_\r\n#include <windows.h>\r\n\r\n";
	char OutData3[64] = {0};
	sprintf(OutData3, "const g_ShellCodeFileSize = %d;\r\n", dwDllSize);
	char OutData4[] = "unsigned char g_ShellCodeFileBuff[] = {\r\n";
	char OutData5[] = "};\r\n\r\n";
	char OutData6[] = "/*\r\nbool ShellCodeSaveFile(char *FileName)\r\n{\r\n\tbool Result = false;"
		"\r\n\tHANDLE hFile;\r\n\tDWORD dwBytesWritten;\r\n"
		"\thFile = CreateFile(FileName,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,NULL,NULL);\r\n"
		"\tif (hFile == INVALID_HANDLE_VALUE) Result = false;\r\n"
		"\tif (WriteFile(hFile, g_ShellCodeFileBuff, g_ShellCodeFileSize, &dwBytesWritten, NULL)) Result = true;\r\n"
		"\tCloseHandle(hFile);\r\n\treturn Result;\r\n}\r\n*/\r\n\r\n#endif\r\n";
	
	hOutFile = CreateFile("ShellCode.h", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hOutFile, OutData1, strlen(OutData1), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
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
	if (!WriteFile(hOutFile, OutData6, strlen(OutData6), &BytesWritten, NULL))
	{
		VirtualFree(pOutBuff, 0, MEM_RELEASE);
		CloseHandle(hOutFile);
		return FALSE;
	}
	VirtualFree(pOutBuff, 0, MEM_RELEASE);
	CloseHandle(hOutFile);
	return TRUE;
}

BOOL SavePlug()
{
	HANDLE hDllFile;
	DWORD dwDllSize;
	LPVOID pDllBuff;
	DWORD BytesRead;
	DWORD BytesWritten;
	
	hDllFile = CreateFile("1.dll", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
	
	if (((PIMAGE_DOS_HEADER)pDllBuff)->e_magic == IMAGE_DOS_SIGNATURE)
		EncryptPlug((char *)pDllBuff, dwDllSize, 1024);
	else
		EncryptData((char *)pDllBuff, dwDllSize, 1024);
	
	hDllFile = CreateFile("2.dll", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, NULL);
	if (hDllFile == INVALID_HANDLE_VALUE)
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		return FALSE;
	}
	if (!WriteFile(hDllFile, pDllBuff, dwDllSize, &BytesWritten, NULL))
	{
		VirtualFree(pDllBuff, 0, MEM_RELEASE);
		CloseHandle(hDllFile);
		return FALSE;
	}
	VirtualFree(pDllBuff, 0, MEM_RELEASE);
	CloseHandle(hDllFile);
	return TRUE;
}

int main(int argc, char* argv[])
{
//	unsigned char MyFileTabLe[] = {0xBE, 0x16, 0xCF, 0x52, 0xCD};
	
	if (argc == 3)
	{
		if (argv[1][0] != '-') return -1;
		
		switch (argv[1][1])
		{
		case 'E': //输入未加密文件名(Server.dll), 生成加密后的文件(Server.dll)
			return SaveMain(argv[2]) ? 0 : -1;
		case 'S': //输入已加密文件名(Server.dll), 生成"ShellCode.h"
			return SaveCode(argv[2]) ? 0 : -1;
		default:
			return -1;
		}
	}
	return SavePlug() ? 0 : -1;
}
