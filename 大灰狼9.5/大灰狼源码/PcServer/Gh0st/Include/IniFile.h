// IniFile.h: interface for the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILE_H__D5A2B7FC_6022_4EA2_9E54_91C4E7B31B8E__INCLUDED_)
#define AFX_INIFILE_H__D5A2B7FC_6022_4EA2_9E54_91C4E7B31B8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIniFile  
{
public:
	CIniFile();
	virtual ~CIniFile();
	void	SetIniFileName(CString FileName){ IniFileName = FileName; }
	CString	GetIniFileName(){ return IniFileName; }

	CString	GetString(CString AppName, CString KeyName, CString Default = "");
	int		GetInt(CString AppName, CString KeyName, int Default = 0);
	unsigned long	GetDWORD(CString AppName, CString KeyName, unsigned long Default = 0);
	
	BOOL	SetString(CString AppName, CString KeyName, CString Data);
	BOOL	SetInt(CString AppName, CString KeyName, int Data);
	BOOL	SetDouble(CString AppName, CString KeyName, double Data);
	BOOL	SetDWORD(CString AppName, CString KeyName, unsigned long Data);
private:
	CString IniFileName;
};

#endif // !defined(AFX_INIFILE_H__D5A2B7FC_6022_4EA2_9E54_91C4E7B31B8E__INCLUDED_)
