// RegeditOpt.h: interface for the RegeditOpt class.
//
//////////////////////////////////////////////////////////////////////

#include "../Contain/Manager.h"

#if !defined(AFX_REGEDITOPT_H__1B9BB8E7_CC9F_48AC_96F3_B6982752641C__INCLUDED_)
#define AFX_REGEDITOPT_H__1B9BB8E7_CC9F_48AC_96F3_B6982752641C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define DWORD_SIZE   8
//#define INT_SIZE     4

enum MYKEY{
    MHKEY_CLASSES_ROOT,
    MHKEY_CURRENT_USER,
	MHKEY_LOCAL_MACHINE,
	MHKEY_USERS,
	MHKEY_CURRENT_CONFIG
};
enum KEYVALUE{
     MREG_SZ,
	 MREG_DWORD,
	 MREG_BINARY,
	 MREG_EXPAND_SZ
};

struct REGMSG{
     int count;         //名字个数
     DWORD size;        //名字大小
	 DWORD valsize;     //值大小
	 
};

class RegeditOpt  
{
public:
	char* FindKey();
	void SetPath(char * path);

	char* FindPath();
	RegeditOpt();
	RegeditOpt(char b);
	virtual ~RegeditOpt();
protected:
	HKEY		MKEY;			//用于保存主
	char        KeyPath[MAX_PATH];          //项

};

#endif // !defined(AFX_REGEDITOPT_H__1B9BB8E7_CC9F_48AC_96F3_B6982752641C__INCLUDED_)
