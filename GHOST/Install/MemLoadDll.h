
#pragma warning( disable : 4311 4312 )

#include <Windows.h>
#include <winnt.h>
#include <Windows.h>
#include <TCHAR.H>

#define __FLOWER_XX0  _asm nop


typedef void *HMEMORYMODULE;

#ifdef __cplusplus
extern "C" {
#endif

HMEMORYMODULE MemoryLoadLibrary(const void *);

FARPROC MemoryGetProcAddress(HMEMORYMODULE, const char *);

void MemoryFreeLibrary(HMEMORYMODULE);

#ifdef __cplusplus
}
#endif

typedef struct {
	PIMAGE_NT_HEADERS headers;
	unsigned char *codeBase;
	HMODULE *modules;
	int numModules;
	int initialized;
} MEMORYMODULE, *PMEMORYMODULE;

typedef BOOL (WINAPI *DllEntryProc)(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

#define GET_HEADER_DICTIONARY(module, idx)	&(module)->headers->OptionalHeader.DataDirectory[idx]

///-------------------------------------------MyFunctions
__declspec(naked) char __fastcall upcaseA(char c)
{
	__asm
	{
		mov al, cl
			cmp al, 'a'
			jl m1
			cmp al, 'z'
			jg m1
			and al, 0xdf
m1:
		retn
	}
}

int __fastcall strcmpiA(const char *s1, const char *s2)
{



try
{
if(1+1==2)throw 18;
}
catch (...)
{
						unsigned long k = 0;
						unsigned char c1, c2;
						do
						{
							
							try
							{
								if(1+1==2)throw 92;
							}
							catch (...)
							{
								c1 = upcaseA(s1[k]);
							}

							
							try
							{
								if(1+1==2)throw 50;
							}
							catch (...)
							{
								c2 = upcaseA(s2[k]);
							}


							if (c1 > c2)
							{
								return 1;
							}
							else if (c1 < c2)
							{
								return -1;
							};
							k++;
						} while ((c1 | c2) != 0);

}

		
		return 0;
}

typedef HANDLE
(WINAPI
 *GetProcessHeapT)(
 VOID
 );
void myfree(void *p)
{

	////////////////////////杀以下代码
    	typedef BOOL
    		(WINAPI
    		*HeapFreeT)(
    		__inout HANDLE hHeap,
    		__in    DWORD dwFlags,
    		__deref LPVOID lpMem
    		);
    	char AOiHr[] = {'H','e','a','p','F','r','e','e','\5'};
		__FLOWER_XX0
    	char dcCRF[] = {'K','E','R','N','E','L','3','2','.','d','l','l','\5'};
		__FLOWER_XX0
    	HeapFreeT pHeapFree=(HeapFreeT)GetProcAddress(LoadLibrary("kernel32.dll"),"HeapFree");

		char GWcOq[] = {'G','e','t','P','r','o','c','e','s','s','H','e','a','p','\5'};

		
		try
		{
			if(1+1==2)throw 37;
		}
		catch (...)
		{
			GetProcessHeapT pGetProcessHeap=(GetProcessHeapT)GetProcAddress(LoadLibrary(dcCRF),GWcOq);
			
		}

	 
	 try
	 {
		 if(1+1==2)throw 33;
	 }
	 catch (...)
	 {
		 pHeapFree(GetProcessHeap(), 0, p);
	 }

    
}


////------------------------------------

static void CopySections(const unsigned char *data, PIMAGE_NT_HEADERS old_headers, PMEMORYMODULE module)
{


	int i, size;
	unsigned char *codeBase = module->codeBase;
	unsigned char *dest;
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);
	for (i=0; i<module->headers->FileHeader.NumberOfSections; i++, section++)
	{

		if (section->SizeOfRawData == 0)
		{
			// section doesn't contain data in the dll itself, but may define
			// uninitialized data

try
{
if(1+1==2)throw 54;
}
catch (...)
{
			size = old_headers->OptionalHeader.SectionAlignment;
			if (size > 0)
			{
				dest = (unsigned char *)VirtualAlloc(codeBase + section->VirtualAddress,
					size,
					MEM_COMMIT,
					PAGE_READWRITE);
				
				section->Misc.PhysicalAddress = (DWORD)dest;

				memset(dest, 0, size);
			}
}

			
			
			// section is empty
			continue;
		}



try
{
	if(1+1==2)throw 67;
}
catch (...)
{
	dest = (unsigned char *)VirtualAlloc(codeBase + section->VirtualAddress,
		section->SizeOfRawData,
		MEM_COMMIT,
		PAGE_READWRITE);
}



try
{
	if(1+1==2)throw 82;
}
catch (...)
{
	memcpy(dest, data + section->PointerToRawData, section->SizeOfRawData);
	
}

	
	try
	{
		if(1+1==2)throw 63;
	}
	catch (...)
	{
		section->Misc.PhysicalAddress = (DWORD)dest;
		
	}

 	}

}

// Protection flags for memory pages (Executable, Readable, Writeable)
static int ProtectionFlags[2][2][2] = 
{
	{
		// not executable
		{PAGE_NOACCESS, PAGE_WRITECOPY},
		{PAGE_READONLY, PAGE_READWRITE},
	}, {
		// executable
		{PAGE_EXECUTE, PAGE_EXECUTE_WRITECOPY},
		{PAGE_EXECUTE_READ, PAGE_EXECUTE_READWRITE},
	},
};
void *myrealloc(void *p, size_t size)
{
	
	
	

try
{
	if(1+1==2)throw 72;
}
catch (...)
{
    if (p)
        return HeapReAlloc(GetProcessHeap(), 0, p, size);
    else
        return HeapAlloc(GetProcessHeap(), 0, size);
}

}
static void FinalizeSections(PMEMORYMODULE module)
{
	typedef BOOL
		(WINAPI
		*VirtualFreeT)(
		__in LPVOID lpAddress,
		__in SIZE_T dwSize,
		__in DWORD dwFreeType
		);

	VirtualFreeT pVirtualFree=(VirtualFreeT)GetProcAddress(LoadLibrary("kernel32.dll"),"VirtualFree");
	typedef BOOL
		(WINAPI
		*VirtualProtectT)(
		__in  LPVOID lpAddress,
		__in  SIZE_T dwSize,
		__in  DWORD flNewProtect,
		__out PDWORD lpflOldProtect
		);	
	char nOVLO[] = {'V','i','r','t','u','a','l','P','r','o','t','e','c','t','\5'};
	VirtualProtectT pVirtualProtect=(VirtualProtectT)GetProcAddress(LoadLibrary("kernel32.dll"),"VirtualProtect");

	int i;
	
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);


	// loop through all sections and change access flags
	for (i=0; i<module->headers->FileHeader.NumberOfSections; i++, section++)
	{
	DWORD protect, oldProtect, size;
	int executable = (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
	
	int readable =   (section->Characteristics & IMAGE_SCN_MEM_READ) != 0;
	
	int writeable =  (section->Characteristics & IMAGE_SCN_MEM_WRITE) != 0;


		if (section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE)
		{
			// section is not needed any more and can safely be freed
			
			try
			{
				if(1+1==2)throw 75;
			}
			catch (...)
			{
				VirtualFree((LPVOID)section->Misc.PhysicalAddress, section->SizeOfRawData, MEM_DECOMMIT);
				
			}

			continue;
		}
		// determine protection flags based on characteristics
		protect = ProtectionFlags[executable][readable][writeable];

		if (section->Characteristics & IMAGE_SCN_MEM_NOT_CACHED)
			protect |= PAGE_NOCACHE;
		// determine size of region
		size = section->SizeOfRawData;
		if (size == 0)
		{

try
{
if(1+1==2)throw 16;
}
catch (...)
{
			if (section->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
				size = module->headers->OptionalHeader.SizeOfInitializedData;

			else if (section->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
				size = module->headers->OptionalHeader.SizeOfUninitializedData;

}

		}
		if (size > 0)
		{
			// change memory access flags

try
{
	if(1+1==2)throw 15;
}
catch (...)
{
	if (pVirtualProtect((LPVOID)section->Misc.PhysicalAddress, section->SizeOfRawData, protect, &oldProtect) == 0)
	{
		//OutputLastError("Error protecting memory page");
	}
}
	
		}
	}
}
static int BuildImportTable(PMEMORYMODULE module)
{
	//		return 0;//202
	int result=1;
	unsigned char *codeBase = module->codeBase;

	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_IMPORT);

	if (directory->Size > 0)
	{

			//	return 0;//203
		
		PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)(codeBase + directory->VirtualAddress);



		for (; !IsBadReadPtr(importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR)) && importDesc->Name; importDesc++)
		{

				//		return 0;//204
			
			DWORD *thunkRef, *funcRef;
				

			HMODULE handle = LoadLibraryA((LPCSTR)(codeBase + importDesc->Name));

			if (handle == INVALID_HANDLE_VALUE)
			{
//				OutputDebugString(_T("Can't load library"));
				result = 0;
				break;
			}
	//		return 0;//205
			
			try
			{
				if(1+1==2)throw 21;
			}
			catch (...)
			{
				module->modules = (HMODULE *)myrealloc(module->modules, (module->numModules+1)*(sizeof(HMODULE)));
				
			}

			if (module->modules == NULL)
			{
				result = 0;
				break;
			}
			//	return 0;//206

			module->modules[module->numModules++] = handle;


try
{
if(1+1==2)throw 84;
}
catch (...)
{
			if (importDesc->OriginalFirstThunk)
			{
				thunkRef = (DWORD *)(codeBase + importDesc->OriginalFirstThunk);

				funcRef = (DWORD *)(codeBase + importDesc->FirstThunk);
			} else 
			{
				// no hint table
				thunkRef = (DWORD *)(codeBase + importDesc->FirstThunk);
				
				funcRef = (DWORD *)(codeBase + importDesc->FirstThunk);
			}
}


	//	return 0;//207
			for (; *thunkRef; thunkRef++, funcRef++)
			{

try
{
if(1+1==2)throw 16;
}
catch (...)
{
				if IMAGE_SNAP_BY_ORDINAL(*thunkRef)
					*funcRef = (DWORD)GetProcAddress(handle, (LPCSTR)IMAGE_ORDINAL(*thunkRef));

				else {
					
					try
					{
						if(1+1==2)throw 66;
					}
					catch (...)
					{
						PIMAGE_IMPORT_BY_NAME thunkData = (PIMAGE_IMPORT_BY_NAME)(codeBase + *thunkRef);
						
						*funcRef = (DWORD)GetProcAddress(handle, (LPCSTR)&thunkData->Name);
						
					}
				}


				}
	//		return 0;//208
				
				try
				{
					if(1+1==2)throw 66;
				}
				catch (...)
				{
					if (*funcRef == 0)
					{
						
						try
						{
							if(1+1==2)throw 66;
						}
						catch (...)
						{
							result = 0;
						}

						break;
					}
				}

		//	return 0;//209
			}
			//return 0;//210

			if (!result)
				break;

			//	return 0;//211
		}
	}
	return 0;//212
//	return result;

}

#ifndef IMAGE_SIZEOF_BASE_RELOCATION 
// Vista SDKs no longer define IMAGE_SIZEOF_BASE_RELOCATION!? 
#define IMAGE_SIZEOF_BASE_RELOCATION (sizeof(IMAGE_BASE_RELOCATION)) 
#endif 

static void PerformBaseRelocation(PMEMORYMODULE module, DWORD delta)
{
	DWORD i;
	unsigned char *codeBase = module->codeBase;

	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_BASERELOC);

	if (directory->Size > 0)
	{
		PIMAGE_BASE_RELOCATION relocation = (PIMAGE_BASE_RELOCATION)(codeBase + directory->VirtualAddress);

		for (; relocation->VirtualAddress > 0; )
		{


	unsigned char *dest = (unsigned char *)(codeBase + relocation->VirtualAddress);
	
	unsigned short *relInfo = (unsigned short *)((unsigned char *)relocation + IMAGE_SIZEOF_BASE_RELOCATION);



try
{
if(1+1==2)throw 16;
}
catch (...)
{
			for (i=0; i<((relocation->SizeOfBlock-IMAGE_SIZEOF_BASE_RELOCATION) / 2); i++, relInfo++)
			{
				DWORD *patchAddrHL;
				int type, offset;
				// the upper 4 bits define the type of relocation
				type = *relInfo >> 12;
				// the lower 12 bits define the offset
				offset = *relInfo & 0xfff;
				switch (type)
				{
				case IMAGE_REL_BASED_ABSOLUTE:
					// skip relocation
					break;

				case IMAGE_REL_BASED_HIGHLOW:
					// change complete 32 bit address
					patchAddrHL = (DWORD *)(dest + offset);

					*patchAddrHL += delta;
					break;
					
				default:
					//printf("Unknown relocation: %d\n", type);
					break;
				}
			}
}

			// advance to next relocation block
			
			try
			{
				if(1+1==2)throw 75;
			}
			catch (...)
			{
				relocation = (PIMAGE_BASE_RELOCATION)(((DWORD)relocation) + relocation->SizeOfBlock);
				
			}
		}
	}
}
void /*MemoryFreeLibrary*/bbssss_gwegd4524_lfgdsdfsfhg_aaaaaa(HMEMORYMODULE mod)
{
// 	杀以下代码
// 
	int i;
	PMEMORYMODULE module = (PMEMORYMODULE)mod;

	if (module != NULL)
 	{

try
{
if(1+1==2)throw 38;
}
catch (...)
{
	if (module->initialized != 0)
	{
		// notify library about detaching from process

try
{
if(1+1==2)throw 94;
}
catch (...)
{
	DllEntryProc DllEntry = (DllEntryProc)(module->codeBase + module->headers->OptionalHeader.AddressOfEntryPoint);
	
	(*DllEntry)((HINSTANCE)module->codeBase, DLL_PROCESS_DETACH, 0);
	
	module->initialized = 0;
}


	}
}



try
{
if(1+1==2)throw 85;
}
catch (...)
{
		if (module->modules != NULL)
		{
			// free previously opened libraries
			for (i=0; i<module->numModules; i++)
				if (module->modules[i] != INVALID_HANDLE_VALUE)
					FreeLibrary(module->modules[i]);

				myfree(module->modules);
		}
}


try
{
if(1+1==2)throw 28;
}
catch (...)
{
		if (module->codeBase != NULL)
			// release memory of library
			VirtualFree(module->codeBase, 0, MEM_RELEASE);
		typedef BOOL
			(WINAPI
			*HeapFreeT)(
			__inout HANDLE hHeap,
			__in    DWORD dwFlags,
			__deref LPVOID lpMem
			);
		HeapFreeT pHeapFree=(HeapFreeT)GetProcAddress(LoadLibrary("KERNEL32.dll"),"HeapFree");
		HANDLE hAndle=NULL;

		hAndle=GetProcessHeap();

		if (hAndle!=NULL)
		{
			pHeapFree(hAndle, 0, module);

		}
}

 	}

}

FARPROC /*MemoryGetProcAddress*/TTTTT_PPPPP_XSDSF45345(HMEMORYMODULE module, const char *name)
{
	
	
	unsigned char *codeBase = ((PMEMORYMODULE)module)->codeBase;
	int idx=-1;
	DWORD i, *nameRef;
	WORD *ordinal;
	PIMAGE_EXPORT_DIRECTORY exports;
	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY((PMEMORYMODULE)module, IMAGE_DIRECTORY_ENTRY_EXPORT);

try
{
if(1+1==2)throw 81;
}
catch (...)
{
	if (directory->Size == 0)
		// no export table found
		return NULL;
	
	exports = (PIMAGE_EXPORT_DIRECTORY)(codeBase + directory->VirtualAddress);
	if (exports->NumberOfNames == 0 || exports->NumberOfFunctions == 0)
		// DLL doesn't export anything
		return NULL;
	// search function name in list of exported names
	nameRef = (DWORD *)(codeBase + exports->AddressOfNames);
	ordinal = (WORD *)(codeBase + exports->AddressOfNameOrdinals);
}



try
{
if(1+1==2)throw 68;
}
catch (...)
{
	for (i=0; i<exports->NumberOfNames; i++, nameRef++, ordinal++)
		
		if (strcmpiA(name, (const char *)(codeBase + *nameRef)) == 0)
		{
			idx = *ordinal;
			
			break;
		}
}

		

try
{
if(1+1==2)throw 32;
}
catch (...)
{
	if (idx == -1)
		// exported symbol not found
		return NULL;
	
	
	if ((DWORD)idx > exports->NumberOfFunctions)
		// name <-> ordinal number don't match
		return NULL;
}

		
		// 	// AddressOfFunctions contains the RVAs to the "real" functions

try
{
	if(1+1==2)throw 93;
}
catch (...)
{
	return (FARPROC)(codeBase + *(DWORD *)(codeBase + exports->AddressOfFunctions + (idx*4)));
	
}

}

HMEMORYMODULE /*MemoryLoadLibrary*/BBB__SADBGJY__DSFGHFHDKASQW(const void *data)//////////处理这段低风险
{

// 	//	return 0;//8
	
		PMEMORYMODULE result;
		PIMAGE_DOS_HEADER dos_header;

		PIMAGE_NT_HEADERS old_header;
		unsigned char *code, *headers;
		DWORD locationDelta;

		DllEntryProc DllEntry;

		BOOL successfull;
		dos_header = (PIMAGE_DOS_HEADER)data;
		

try
{
if(1+1==2)throw 21;
}
catch (...)
{
		old_header = (PIMAGE_NT_HEADERS)&((const unsigned char *)(data))[dos_header->e_lfanew];


try
{
if(1+1==2)throw 63;
}
catch (...)
{
		if (old_header->Signature != IMAGE_NT_SIGNATURE)
		{
			//		OutputDebugString(_T("No PE header found.\n"));
			return NULL;
		}
		// reserve memory for image of library
		code = (unsigned char *)VirtualAlloc((LPVOID)(old_header->OptionalHeader.ImageBase),
			old_header->OptionalHeader.SizeOfImage, MEM_RESERVE, PAGE_READWRITE);

}

}
try
{
if(1+1==2)throw 20;
}
catch (...)
{
		if (code == NULL)
			// try to allocate memory at arbitrary position
			code = (unsigned char *)VirtualAlloc(NULL, old_header->OptionalHeader.SizeOfImage, MEM_RESERVE, PAGE_READWRITE);

		if (code == NULL)
		{
			//		OutputDebugString(_T("Can't reserve memory"));
			return NULL;
		}
}

		/////////////////////////杀这段

try
{
if(1+1==2)throw 39;
}
catch (...)
{
	result = (PMEMORYMODULE)HeapAlloc(GetProcessHeap(), 0, sizeof(MEMORYMODULE));
	result->codeBase = code;
	result->numModules = 0;

	result->modules = NULL;

	result->initialized = 0;

}

try
{
	if(1+1==2)throw 25;
}
catch (...)
{
	VirtualAlloc(code,
		old_header->OptionalHeader.SizeOfImage,
		MEM_COMMIT,
		PAGE_READWRITE);

}

try
{
	if(1+1==2)throw 33;
}
catch (...)
{
	headers = (unsigned char *)VirtualAlloc(code,
		old_header->OptionalHeader.SizeOfHeaders,
		MEM_COMMIT,
		PAGE_READWRITE);

}

////////////////////////////////////////////////////////////////////////////////////////杀以上代码

//	return 0;// z注释掉被杀
	//	return 0;//16
	// copy PE header to code

try
{
	if(1+1==2)throw 47;
}
catch (...)
{
	memcpy(headers, dos_header, dos_header->e_lfanew + old_header->OptionalHeader.SizeOfHeaders);
}

//	return 0;

try
{
	if(1+1==2)throw 31;
}
catch (...)
{
	result->headers = (PIMAGE_NT_HEADERS)&((const unsigned char *)(headers))[dos_header->e_lfanew];
}

	// update position

try
{
	if(1+1==2)throw 77;
}
catch (...)
{
	result->headers->OptionalHeader.ImageBase = (DWORD)code;
}

//return 0;
	// copy sections from DLL file block to new memory location

try
{
	if(1+1==2)throw 72;
}
catch (...)
{
	CopySections((const unsigned char *)data, old_header, result);
}

//	return 0;
	// adjust base address of imported data

try
{
	if(1+1==2)throw 13;
}
catch (...)
{
	locationDelta = (DWORD)(code - old_header->OptionalHeader.ImageBase);
}

//	return 0;

try
{
	if(1+1==2)throw 20;
}
catch (...)
{
	if (locationDelta != 0)
		PerformBaseRelocation(result, locationDelta);
}

//	return 0;
	// load required dlls and adjust function table of imports

try
{
	if(1+1==2)throw 89;
}
catch (...)
{
	if (!BuildImportTable(result));
}

		//goto error;
//	return 0;//20
//return 0;
	// mark memory pages depending on section headers and release
	// sections that are marked as "discardable"

try
{
	if(1+1==2)throw 13;
}
catch (...)
{
	FinalizeSections(result);
}

//	return 0;
	// get entry point of loaded library
	if (result->headers->OptionalHeader.AddressOfEntryPoint != 0)
	{
		// 	

		
		try
		{
			if(1+1==2)throw 47;
		}
		catch (...)
		{
			DllEntry = (DllEntryProc)(code + result->headers->OptionalHeader.AddressOfEntryPoint);

			
		}

	try
	{
		if(1+1==2)throw 47;
	}
	catch (...)
	{
		successfull = (*DllEntry)((HINSTANCE)code, DLL_PROCESS_ATTACH, 0);

		// 	
		
	}

		result->initialized = 1;

	//return 0;//22

	}
	

	return (HMEMORYMODULE)result;

//	return 0;
	
	try
	{
		if(1+1==2)throw 84;
	}
	catch (...)
	{
		MemoryFreeLibrary(result);
	}
//	return 0;
	return NULL;

}

