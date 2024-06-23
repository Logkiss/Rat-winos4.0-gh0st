#include "stdafx.h"
#include "zip.h"
#include "unzip.h"
#include <OleAuto.h>
#include <vector> 
#include <atlconv.h>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
HZIP hz;
ZRESULT zr;
ZIPENTRY ze;
BOOL FolderExist(CString& strPath) 
{ 
    CString sCheckPath = strPath; 
	
    if(sCheckPath.Right(1) != L"\\") 
        sCheckPath += L"\\"; 
	
    sCheckPath += L"*.*"; 
	
    WIN32_FIND_DATA wfd; 
    BOOL rValue = FALSE; 
	
    HANDLE hFind = FindFirstFile(sCheckPath, &wfd); 
	
    if ((hFind!=INVALID_HANDLE_VALUE) && 
        (wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) || (wfd.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE)) 
    { 
        //如果存在并类型是文件夹 
        rValue = TRUE; 
    } 
	
    FindClose(hFind); 
    return rValue; 
} 
BOOL CreatedMultipleDirectory(wchar_t* direct) 
{ 
    std::wstring Directoryname = direct; 
	
    if (Directoryname[Directoryname.length() - 1] !=  '\\') 
    { 
        Directoryname.append(1, '\\'); 
    } 
    std::vector< std::wstring> vpath; 
    std::wstring strtemp; 
    BOOL  bSuccess = FALSE; 
    for (int i = 0; i < Directoryname.length(); i++) 
    { 
        if ( Directoryname[i] != '\\') 
        { 
            strtemp.append(1,Directoryname[i]);    
        } 
        else
        { 
            vpath.push_back(strtemp); 
            strtemp.append(1, '\\'); 
        } 
    } 
    std::vector<std::wstring>:: const_iterator vIter; 
    for (vIter = vpath.begin();vIter != vpath.end(); vIter++) 
	{
		USES_CONVERSION;
		bSuccess = CreateDirectoryA( W2A(vIter->c_str()), NULL ) ? true :false;
		
	}
	
    return bSuccess; 
}

BOOL Zip_UnPackFiles(CString mZipFileFullPath, CString mUnPackPath, CString strPW)
{ 
    //参数错误 
    if ((mUnPackPath.IsEmpty()) || (mZipFileFullPath.IsEmpty())) 
    { 
        //路径异常返回 
        return FALSE ; 
    } 
	
    CFileFind tFFind; 
    if (!tFFind.FindFile(mZipFileFullPath)) 
    { 
        //压缩失败(未发现压缩文件) 
        return FALSE; 
    } 
	
    //如果解压缩的路径不存在 试图创建它 
    CString tZipFilePath = mUnPackPath; 
    if(!FolderExist(tZipFilePath)) 
    { 
        //解压后存放的文件夹不存在 创建它         
		CString strWZipFilePath(tZipFilePath);
        if (FALSE == CreatedMultipleDirectory((LPWSTR)(LPCWSTR)&strWZipFilePath)) 
        { 
            //创建目录失败 
            return FALSE; 
        } 
    } 
    /************************************************************************/
    //打开ZIP文件
	if (strPW.IsEmpty())
	{
		hz=OpenZip(mZipFileFullPath,0);
	}
	else
	{
		hz=OpenZip(mZipFileFullPath,(LPCSTR)strPW);
	}     
    if(hz == 0) 
    { 
        //打开Zip文件失败 
        return FALSE; 
    } 
	
    zr=SetUnzipBaseDir(hz,mUnPackPath); 
    if(zr != ZR_OK) 
    { 
        //打开Zip文件失败 
        CloseZip(hz); 
        return FALSE;       
    } 
	
    zr=GetZipItem(hz,-1,&ze); 
    if(zr != ZR_OK) 
    { 
        //获取Zip文件内容失败 
        CloseZip(hz); 
        return FALSE;       
    } 
	
    int numitems=ze.index; 
    for (int i=0; i<numitems; i++) 
    { 
        zr=GetZipItem(hz,i,&ze); 
        zr=UnzipItem(hz,i,ze.name); 
        if(zr != ZR_OK) 
        { 
            //获取Zip文件内容失败 
            CloseZip(hz); 
            return FALSE;       
        } 
    } 
	
    CloseZip(hz); 
    return TRUE; 
}