#pragma once



class LFileName
{
public:
	LFileName(wchar_t* lpFileName);
	LFileName(char* lpFileName);
	LFileName();
	~LFileName(void);
protected:
	wchar_t  wstr_FullFileName[MAX_PATH];   //文件全名
    wchar_t  wstr_FileName[MAX_PATH]; //文件名
	wchar_t  wstr_PathName[MAX_PATH];   //路径
	
	char str_FullFileName[MAX_PATH];     //全名
	char str_FileName[MAX_PATH];        //文件名
	char str_PathName[MAX_PATH];         //路径名
	char str_ExpandName[MAX_PATH];       //扩展名  
	char str_Name[MAX_PATH];              //纯文件名

	BOOL  AnsiOrUnicode;     // 0  unicode  1 ansi 
    

public:
	// 得到文件名
	wchar_t* getFileName(void);
	// 得到当前路径
	wchar_t* getFilePath(void);
	// 初始化
	void init(wchar_t* lpFileName);
	void init(char* lpFileName);
	char* getFileNameA(void);
	char* getFilePathA(void);
	// 得到文件扩展名
	char* getFileExpand(void);
	// 得到纯文件名
	char* getNameA(void);
	// 初始化文件名
	void initFileName(char* lpFileName);
};
