// ShllCodeDec.cpp : Defines the entry point for the application.
//
#include <afx.h>
#include "Stdafx.h"
HANDLE hDllFile;
void*  MemDll;
int    SizeDll;
DWORD  BytesRead;

void DecryptData(unsigned char *szRec, unsigned long nLen, unsigned long key)//加密
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

bool SaveFile(char *FileName)
{
	bool Result = false;
	
	CFile file;
	if(file.Open (FileName, CFile::modeCreate | CFile::modeWrite))
	{
		try
		{
			file.Write(MemDll, SizeDll);
			// 写入6个'C',是服务的名称和描述
			char NBRqd01[] = {'S','S','S','S','S','S','V','I','D','\0'};
			char lpDllVid[] = {':','2','0','1','3','-','S','V','1','\0'};  //文件识别字符串

			file.Write(NBRqd01, lstrlen(NBRqd01));
			file.Write(lpDllVid, lstrlen(lpDllVid));
			file.Close();
			Result = true;
			MessageBox(NULL,"文件保存成功，请用加壳软件进行压缩 -:)","提示",NULL);
		}
		catch(...)
		{
			MessageBox(NULL,"文件保存失败，请检查","提示",MB_OK|MB_ICONSTOP);
			Result = false;
		}
	}
	return Result;
}


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
  	// TODO: Place code here.
	hDllFile =CreateFile("Consys21.dll",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);//这里第一个参数调用上面那个GetModuleFileName
	SizeDll  =GetFileSize(hDllFile,0);
	MemDll   =VirtualAlloc(0,SizeDll,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
	ReadFile(hDllFile,MemDll,SizeDll,&BytesRead,0);
	CloseHandle(hDllFile);

	
 	DecryptData((unsigned char *)MemDll,SizeDll,1024);

	DeleteFile("Consys21.dll");
	SaveFile("Consys21.dll");

	return 0;
}



