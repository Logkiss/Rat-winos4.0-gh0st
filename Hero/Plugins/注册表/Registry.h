//
//Registry.h

#if !defined(AFX_REGISTRY_H__E0610A5D_7166_4D02_9D7E_11AF7CF8E229__INCLUDED_)
#define AFX_REGISTRY_H__E0610A5D_7166_4D02_9D7E_11AF7CF8E229__INCLUDED_

//
#include "../ClientSocket.h"
#include "macros.h"
#include "../until.h"

#include <winreg.h>
/////////////////////////////////////////////////////////////////////////////
// CRegistry window

class CRegistry
{
// Construction
public:
	CRegistry(HKEY hKey=HKEY_LOCAL_MACHINE);
	CRegistry(char b);

public:
	bool DeleteKey(char* path);
    BOOL WriteBuf(LPCTSTR lpValueName, LPCTSTR lpValue);
	BOOL ReadBuf(LPCTSTR lpValueName,char* lpVal,DWORD &size);
	HKEY GetKey();
	BOOL SaveKey(LPCTSTR lpFileName);
	BOOL RestoreKey(LPCTSTR lpFileName);
	BOOL Read(LPCTSTR lpValueName, BYTE* lpVal);
	BOOL Read(LPCTSTR lpValueName, DWORD* pdwVal);
	BOOL Read(LPCTSTR lpValueName, int* pnVal);
	BOOL Write(LPCTSTR lpSubKey, LPCTSTR lpVal);
	BOOL Write(LPCTSTR lpSubKey, DWORD dwVal);
	BOOL Write(LPCTSTR lpSubKey, int nVal);
	BOOL DeleteKey(HKEY hKey, LPCTSTR lpSubKey);
	BOOL DeleteValue(LPCTSTR lpValueName);
	void Close();
	BOOL Open(LPCTSTR lpSubKey);
	BOOL CreateKey(LPCTSTR lpSubKey);
	virtual ~CRegistry();

//protected:
	HKEY m_hKey;
	
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_REGISTRY_H__E0610A5D_7166_4D02_9D7E_11AF7CF8E229__INCLUDED_)
