// BulidServer.cpp: implementation of the CBulidServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "PcShare.h"
#include "BulidServer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBulidServer::CBulidServer()
{
	m_dwErrorCode = 0;
}

CBulidServer::~CBulidServer()
{

}

// 创建客户
// pStructPointer 结构体指针
// iStructPointerSize 结构体大小
// strDatFilePath Dat文件路径
// strBulidSavePath 保存路径


BOOL CBulidServer::BulidServer(PVOID pStructPointer, INT iStructPointerSize, PCTSTR strDatFilePath, PCTSTR strBulidSavePath)
{


//	MessageBox(NULL,strDatFilePath,NULL,NULL);
	if (!IsDatFile(strDatFilePath))
	{    
	    
		SetErrorCode(__ERROR_CODE_OPEN_DAT_FILE_FAILED);
    
		return FALSE;
	}

	// Dat文件数据指针
	PCHAR pDatFileDataBuffer = NULL;
	// Dat文件数据大小
	DWORD dwDatFileDataSize = 0;



	if (!ReadDatDataToBuffer(strDatFilePath, pDatFileDataBuffer, dwDatFileDataSize))
	{
		SetErrorCode(__ERROR_CODE_OPEN_DAT_FILE_FAILED);

		return FALSE;
	}

	if (!CreateServer(pStructPointer, iStructPointerSize, strBulidSavePath, pDatFileDataBuffer, dwDatFileDataSize))
	{
		SetErrorCode(__ERROR_CODE_CREATE_SERVER_FAILED);

		return FALSE;
	}

	return TRUE;
}

// 设置错误代码
// dwErrorCode 错误代码
void CBulidServer::SetErrorCode(DWORD dwErrorCode)
{
	m_dwErrorCode = dwErrorCode;
}

// 获取错误代码具体信息
// 返回具体错误信息字符串
PCTSTR CBulidServer::GetErrorCodeInfo(void)
{
	switch (m_dwErrorCode)
	{
	case __ERROR_CODE_SUCCESS:
		{
			return _T("=>>操作成功!");
		}
		break;
	case __ERROR_CODE_OPEN_DAT_FILE_FAILED:
		{
			return _T("=>>打开文件失败,请检查文件是否存在");
		}
		break;
	case __ERROR_CODE_CREATE_SERVER_FAILED:
		{
			return _T("=>>创建失败，请关闭杀毒软件，重新生成");
		}
		break;
	default:
		return _T("=>>生成错误，检查文件是否存在");
	}

	return NULL;
}

// 判断Dat文件是否存在
// strDatPath Dat文件路径
// 成功返回TRUE,否则失败,并会设置错误信息
BOOL CBulidServer::IsDatFile(PCTSTR strDatPath)
{
	// 打开Dat文件
	HANDLE hFile = INVALID_HANDLE_VALUE;

	hFile = CreateFile(strDatPath, GENERIC_ALL, NULL, NULL, OPEN_EXISTING, NULL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	CloseHandle(hFile);

	return TRUE;
}

// 读取Dat数据到Buffer
// strDatPath Dat文件路径
// pBuffer 入参,Buffer指针
// dwBufferSize 入参,Buffer数据大小
// 成功返回TRUE,否则失败,并会设置错误信息
BOOL CBulidServer::ReadDatDataToBuffer(PCTSTR strDatPath, PCHAR &pBuffer, DWORD &dwBufferSize)
{
	DWORD dwFileSize = 0;
	DWORD dwReadSize = 0;
	
	// 打开Dat文件
	HANDLE hFile = INVALID_HANDLE_VALUE;

	hFile = CreateFile(strDatPath, GENERIC_ALL, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		goto ERROR_HANDLE;
	}

	// 获取文件大小
	dwFileSize = GetFileSize(hFile, NULL);

	if (dwFileSize == 0)
	{
		goto ERROR_HANDLE;
	}

	// 分配内存用于存放读取的文件数据

	pBuffer = (PCHAR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize);

	if (pBuffer == NULL)
	{
		goto ERROR_HANDLE;
	}

	// 读取文件数据
	ReadFile(hFile, pBuffer, dwFileSize, &dwReadSize, NULL);

	if (dwReadSize != dwFileSize)
	{
		goto ERROR_HANDLE;
	}

	// 赋值入参
	dwBufferSize = dwFileSize;

	CloseHandle(hFile);

	return TRUE;

ERROR_HANDLE:

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}

	if (pBuffer != NULL)
	{
		HeapFree(GetProcessHeap(), NULL, pBuffer);

		pBuffer = NULL;
	}

	return FALSE;
}

// 创建Server
// pStructPointer 结构体指针
// iStructPointerSize 结构体大小
// strBulidSavePath 保存路径
// pDatBuffer Dat文件数据Buffer
// dwDatBufferSize Dat文件数据大小
// 成功返回TRUE,否则失败,并会设置错误信息
BOOL CBulidServer::CreateServer(PVOID pStructPointer, INT iStructPointerSize, PCTSTR strBulidSavePath, PCHAR &pDatBuffer, DWORD &dwDatBufferSize)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwWriteSize = 0;

	// 结构数据在Dat数据中所在位置偏移
	UINT uStructDataOffset = 0;

	// 查找结构数据在Dat数据中所在位置偏移
	uStructDataOffset = FindStructDataOffset(pStructPointer, iStructPointerSize, pDatBuffer, dwDatBufferSize);

	if (uStructDataOffset == 0)
	{
		goto ERROR_HANDLE;
	}

	// 创建Server文件
	hFile = CreateFile(strBulidSavePath, GENERIC_ALL, NULL, NULL, CREATE_ALWAYS, NULL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		goto ERROR_HANDLE;
	}

	// 写入结构体所在位置之前的数据
	WriteFile(hFile, pDatBuffer, uStructDataOffset, &dwWriteSize, NULL);

	if (dwWriteSize != uStructDataOffset)
	{
		goto ERROR_HANDLE;
	}

	// 写入结构体数据
	WriteFile(hFile, pStructPointer, iStructPointerSize, &dwWriteSize, NULL);

	if (dwWriteSize != iStructPointerSize)
	{
		goto ERROR_HANDLE;
	}

	// 写入结构体之后的数据
	WriteFile(hFile, pDatBuffer + uStructDataOffset + iStructPointerSize, dwDatBufferSize - uStructDataOffset - iStructPointerSize, &dwWriteSize, NULL);

	if (dwWriteSize != dwDatBufferSize - uStructDataOffset - iStructPointerSize)
	{
		goto ERROR_HANDLE;
	}

	// 完毕了
	CloseHandle(hFile);

	return TRUE;

ERROR_HANDLE:
	
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}

	return FALSE;
}

// 查找结构体数据位置偏移(在Dat文件中的位置偏移)
// pStructPointer 结构体指针
// iStructPointerSize 结构体大小
// pDatBuffer Dat文件数据Buffer
// dwDatBufferSize Dat文件数据大小
// 成功返回非0,否则失败,并会设置错误信息     
UINT CBulidServer::FindStructDataOffset(PVOID pStructPointer, INT iStructPointerSize, PCHAR &pDatBuffer, DWORD &dwDatBufferSize)
{
	// 转换结构指针
	//struct Server_Data *pStructData = (struct Server_Data *)pStructPointer;

	// 结构数据在Dat数据中的位置
	UINT uOffset = 0, n = 0;

	for (UINT i = 0; i < dwDatBufferSize; i++)
	{
		if (pDatBuffer[i] == ((PCHAR)pStructPointer)[0])
		{
			for (n = 0; n < strlen((PCHAR)pStructPointer); n++)
			{
				if (pDatBuffer[i + n] != ((PCHAR)pStructPointer)[n])
				{
					break;
				}
			}

			if (n == strlen((PCHAR)pStructPointer))
			{
				uOffset = i;

				break;
			}
		}
	}

	return uOffset;
}