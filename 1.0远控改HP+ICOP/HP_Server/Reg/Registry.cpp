// Registry.cpp : implementation file
//
#include "stdafx.h"
#include "Registry.h"
#include "RegeditOpt.h"
/////////////////////////////////////////////////////////////////////////////
// CRegistry

CRegistry::CRegistry(HKEY hKey)
{
	m_hKey=hKey;
}
CRegistry::CRegistry(char b)
{
   switch(b){
	   case MHKEY_CLASSES_ROOT:
                m_hKey=HKEY_CLASSES_ROOT;
		   break;
	   case MHKEY_CURRENT_USER:
            m_hKey=HKEY_CURRENT_USER;
		   break;
	   case MHKEY_LOCAL_MACHINE:
		   m_hKey=HKEY_LOCAL_MACHINE;
		   break;
	   case MHKEY_USERS:
		   m_hKey=HKEY_USERS;
		   break;
	   case MHKEY_CURRENT_CONFIG:
		   m_hKey=HKEY_CURRENT_CONFIG;
		   break;
	   default:
            m_hKey=HKEY_LOCAL_MACHINE;
		break;
	}
}
CRegistry::~CRegistry()
{
	Close();
}

/////////////////////////////////////////////////////////////////////////////
// CRegistry Functions

BOOL CRegistry::CreateKey(LPCTSTR lpSubKey)
{
	HKEY hKey;
	DWORD dw;
	long lReturn=RegCreateKeyEx(m_hKey,lpSubKey,0L,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&dw);
    
	if(lReturn==ERROR_SUCCESS)
	{
		m_hKey=hKey;
		return TRUE;
	}
	
	return FALSE;
	
}

BOOL CRegistry::Open(LPCTSTR lpSubKey)
{
   
	HKEY hKey;
	long lReturn=RegOpenKeyEx(m_hKey,lpSubKey,0L,KEY_ALL_ACCESS,&hKey);
    
	if(lReturn==ERROR_SUCCESS)
	{
        m_hKey=hKey;
		return TRUE;
	}
	return FALSE;
	
}

void CRegistry::Close()
{
	if(m_hKey)
	{
		RegCloseKey(m_hKey);
		m_hKey=NULL;
	}
	
}

BOOL CRegistry::DeleteValue(LPCTSTR lpValueName)
{
	long lReturn=RegDeleteValue(m_hKey,lpValueName);
	
	if(lReturn==ERROR_SUCCESS)
		return TRUE;
	return FALSE;
	
}

BOOL CRegistry::DeleteKey(HKEY hKey, LPCTSTR lpSubKey)
{
	long lReturn=RegDeleteKey(hKey,lpSubKey);
	
	if(lReturn==ERROR_SUCCESS)
		return TRUE;
	return FALSE;
	
}

bool CRegistry::DeleteKey(char *path)
{
    long lReturn=RegDeleteKey(m_hKey,path);
	
	if(lReturn==ERROR_SUCCESS)
		return TRUE;
	return FALSE;
}


BOOL CRegistry::Write(LPCTSTR lpSubKey, int nVal)
{

	DWORD dwValue;
	dwValue=(DWORD)nVal;
	
	long lReturn=RegSetValueEx(m_hKey,lpSubKey,0L,REG_DWORD,(const BYTE *) &dwValue,sizeof(DWORD));
	
   	if(lReturn==ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
	
}

BOOL CRegistry::Write(LPCTSTR lpSubKey, DWORD dwVal)
{

	long lReturn=RegSetValueEx(m_hKey,lpSubKey,0L,REG_DWORD,(const BYTE *) &dwVal,sizeof(DWORD));
	
   	if(lReturn==ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
	
}

BOOL CRegistry::Write(LPCTSTR lpValueName, LPCTSTR lpValue)
{
	long lReturn=RegSetValueEx(m_hKey,lpValueName,0L,REG_SZ,(const BYTE *) lpValue,strlen(lpValue)+1);
	
   	if(lReturn==ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
	
}


BOOL CRegistry::Read(LPCTSTR lpValueName, int* pnVal)
{
	
	DWORD dwType;
	DWORD dwSize=sizeof(DWORD);
	DWORD dwDest;
	long lReturn=RegQueryValueEx(m_hKey,lpValueName,NULL,&dwType,(BYTE *)&dwDest,&dwSize);
	
	if(lReturn==ERROR_SUCCESS)
	{
		*pnVal=(int)dwDest;
		return TRUE;
	}
	return FALSE;
	
}

BOOL CRegistry::Read(LPCTSTR lpValueName, DWORD* pdwVal)
{
	
	DWORD dwType;
	DWORD dwSize=sizeof(DWORD);
	DWORD dwDest;
	long lReturn=RegQueryValueEx(m_hKey,lpValueName,NULL,&dwType,(BYTE *)&dwDest,&dwSize);
	
	if(lReturn==ERROR_SUCCESS)
	{
		*pdwVal=dwDest;
		return TRUE;
	}
	return FALSE;
	
}

BOOL CRegistry::RestoreKey(LPCTSTR lpFileName)
{
	
	long lReturn=RegRestoreKey(m_hKey,lpFileName,REG_WHOLE_HIVE_VOLATILE);
	
	if(lReturn==ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

BOOL CRegistry::SaveKey(LPCTSTR lpFileName)
{
	long lReturn=RegSaveKey(m_hKey,lpFileName,NULL);
	
	if(lReturn==ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}


BOOL CRegistry::Read(LPCTSTR lpValueName, BYTE* lpVal)
{

	DWORD dwType;
	DWORD dwSize=260;
	//char  buf[2550];
	//memset(buf,0,2550);
	//long lReturn=RegQueryValueEx(m_hKey,lpValueName,NULL,&dwType,(BYTE *)szString,&dwSize);
	//long lReturn=RegQueryValueEx(m_hKey, lpValueName, 0, &dwType, (BYTE *)szString, &dwSize);
	long lReturn=RegQueryValueEx(m_hKey, lpValueName, 0, &dwType, (BYTE*)lpVal, &dwSize);
	
	//DWORD type,size;
	//long lReturn=RegQueryValueEx(m_hKey, "netsvcs", 0, &type, (buf), &size);
	if(lReturn==ERROR_SUCCESS)
	{
		//*lpVal=buf;
		return TRUE;
	}
	return FALSE;
	
}

HKEY CRegistry::GetKey()
{
  return m_hKey;
}

BOOL CRegistry::ReadBuf(LPCTSTR lpValueName, char *lpVal,DWORD &size)
{ 	
    DWORD type;
	long lReturn=RegQueryValueEx(m_hKey, lpValueName, 0, &type, (BYTE*)lpVal, &size);
	if(lReturn==ERROR_SUCCESS)
	{
		return size;
	}
	return FALSE;
   return false;
}

BOOL CRegistry::WriteBuf(LPCTSTR lpValueName, LPCTSTR lpValue) 
{
	long lReturn=RegSetValueEx(m_hKey,lpValueName,0L, REG_EXPAND_SZ,(const BYTE *) lpValue,strlen(lpValue)+1);
	
   	if(lReturn==ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

