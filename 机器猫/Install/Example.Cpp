/*
 File Example_Cpp  By 西门
 [若用在VC上 当文件较大的时候 请用 /Zm 申请内存....]
 [方法:Project ->Setting ->c/c++ ->Project Options 下输入 例如: /Zm200 100的倍数 最大2000 具体的参考Google]
 QQ 168751  Email: 88y88@126.com 
*/
#include <windows.h>
#include "C_DllMain.h"


int main()
{
	if (DllMainSaveFile("c:\\C++.exe")) WinExec("c:\\C++.exe",5);
	return 0;
}

