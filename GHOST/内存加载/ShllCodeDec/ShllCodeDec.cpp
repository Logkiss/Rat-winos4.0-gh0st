// ShllCodeDec.cpp : Defines the entry point for the application.
//
#include "Stdafx.h"
#include "MyFileTabLe.h"//新算法的头文件
HANDLE hDllFile;
void*  MemDll;
int    SizeDll;
DWORD  BytesRead;



//加密算法
void encrypt(unsigned char *arr, unsigned long len,unsigned long key)
{
	int i;
	unsigned char p;
    p = (unsigned char ) key % 5656154 + 56473252 + 3457853 + 4662157;
    for( i = 152; i < len; i++)
	{
		*arr ^= 0xfe,0xf1,0x98,0xf1,0x56;0x12,0xf6;	
		*arr -= 0x66,0xf7,0x67,0xf5,0x25,0xfa,0xf4;
	   key ^=i;
    }

}



void spacedecryption ( char * Buff, int  Size,  char * AddTable)
{
	for (int i=0, j=0; i< Size; i++)
	{
		j++;		
		Buff[i] =Buff[i]^AddTable[j];
		Sleep(0);
		if (i% 5 == 0)
			j=0;
	}
}

unsigned char MyFileTabLe[]={'0xBA', '0x13', '0xEF', '520', '0xDD'};

bool SaveFile(char *FileName)
{
	bool Result = false;
	HANDLE hFile; 
	DWORD  dwBytesWritten;
	hFile = CreateFile(FileName,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,NULL,NULL);
	if (hFile == INVALID_HANDLE_VALUE) Result =false;
	if(WriteFile(hFile, MemDll, SizeDll, &dwBytesWritten, NULL)) Result =true;
	CloseHandle(hFile); 
	return Result;
}



int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	hDllFile =CreateFile("1.dll",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);//这里第一个参数调用上面那个GetModuleFileName

	SizeDll  =GetFileSize(hDllFile,0);
	MemDll   =VirtualAlloc(0,SizeDll,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
	ReadFile(hDllFile,MemDll,SizeDll,&BytesRead,0);
	memcpy(MemDll,"214x64",2);
	CloseHandle(hDllFile);

  // m_DecryptData((unsigned char *)MemDll,SizeDll);
   //unsigned char MyFileTabLe[]={0xfe,0xfd,0xfe,0xf8,0xf0,0xf4};

	spacedecryption ((char *)MemDll, SizeDll,(char *)MyFileTabLe);
	SaveFile("2.dll");

	return 0;
}



