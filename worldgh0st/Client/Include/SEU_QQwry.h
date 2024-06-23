/*********************************************************************
* SEU_QQwry.h
*
* 楚茗收集整理
*
* 说明:纯真IP数据库 QQWry.dat的操作类
* 
* 部分代码来源于网络,你可以随意使用,传播或修改.但是不可用于商业用途
*********************************************************************/
#include "..\stdafx.h"
#if !defined(AFX_SEU_QQWRY_H)
#define AFX_SEU_QQWRY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAXBUF 50

typedef struct _tagEndInfo
{
	BYTE b0;
	BYTE b1;
	BYTE b2;
	BYTE b3;
	BYTE buf[MAXBUF];
	BYTE bMode;
	int offset1;
	int offset2;
}EndInfo,PEndInfo;

typedef struct _tagIPOFF
{
	BYTE b0;
	BYTE b1;
	BYTE b2;
	BYTE b3;
	BYTE off1;
	BYTE off2;
	BYTE off3;
}IPOFF,*PIPOFF;

typedef struct _tagBE
{
	int uBOff;
	int uEOff;
}BE,*PBE;

class SEU_QQwry  
{
public://普通用户接口函数
	void SetPath(CString path);//设置QQWry.dat的路径
    CString IPtoAdd(CString szIP);//参数是IP,返回IP对应的地址   
	void SaveToFile(CString Name);//将QQWry.dat文件的内容导出为指定的文件

public://高级用户函数	
	DWORD m_dwLastIP;
	CString GetCountryLocal(int index);
	DWORD GetSIP(int index);
	DWORD IPtoDWORD(CString szIP);
	int GetIndex(CString szIP);
	CString GetStr(void);
	CString GetCountryLocal(BYTE bMode,int ioffset);
	CString GetStr(int ioffset);
	int GetRecordCount(void);
	int m_i;
	int GetStartIPInfo(int iIndex);
	CString m_buf;
	bool GetBE(void);
	bool OpenQQwry(CString szFileName);//打开QQwry数据库
	void CloseQQwry(void);//关闭QQwry数据库
	BE m_be;
	IPOFF m_ipoff;
	EndInfo m_ei;
	SEU_QQwry();
	virtual ~SEU_QQwry();
private:
	bool m_bOpen;
	CFile m_file;
};

#endif // !defined(AFX_SEU_QQWRY_H)
