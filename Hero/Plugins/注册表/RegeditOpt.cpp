// RegeditOpt.cpp: implementation of the RegeditOpt class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RegeditOpt.h"
#include "Registry.h"
#include <stdlib.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RegeditOpt::RegeditOpt()
{

}



RegeditOpt::RegeditOpt(char b)
{
	switch(b){
	   case MHKEY_CLASSES_ROOT:
                MKEY=HKEY_CLASSES_ROOT;
		   break;
	   case MHKEY_CURRENT_USER:
            MKEY=HKEY_CURRENT_USER;
		   break;
	   case MHKEY_LOCAL_MACHINE:
		   MKEY=HKEY_LOCAL_MACHINE;
		   break;
	   case MHKEY_USERS:
		   MKEY=HKEY_USERS;
		   break;
	   case MHKEY_CURRENT_CONFIG:
		   MKEY=HKEY_CURRENT_CONFIG;
		   break;
	   default:
            MKEY=HKEY_LOCAL_MACHINE;
		break;
	}
    ZeroMemory(KeyPath,MAX_PATH);
}

void RegeditOpt::SetPath(char *path)
{
	ZeroMemory(KeyPath,MAX_PATH);
    strcpy(KeyPath,path);
}

RegeditOpt::~RegeditOpt()
{

}

char* RegeditOpt::FindPath()
{
    char *buf=NULL;
	HKEY		hKey;			//注册表返回句柄
    if(RegOpenKeyEx(MKEY,KeyPath,0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)//打开
	{
       	DWORD dwIndex=0,NameSize,NameCnt,NameMaxLen,Type;
		DWORD KeySize,KeyCnt,KeyMaxLen,DateSize,MaxDateLen;
        //这就是枚举了
		if(RegQueryInfoKey(hKey,NULL,NULL,NULL,&KeyCnt,&KeyMaxLen,NULL,&NameCnt,&NameMaxLen,&MaxDateLen,NULL,NULL)!=ERROR_SUCCESS)
		{

			return NULL;
		}
		//一点保护措施
		 KeySize=KeyMaxLen+1;
		if(KeyCnt>0&&KeySize>1){
		         int size=sizeof(REGMSG)+1;
		         
		          //buf=new char[KeyCnt*KeySize+size+1];
		          DWORD datasize=KeyCnt*KeySize+size+1;
		            buf=(char*)LocalAlloc(LPTR, datasize);
		           ZeroMemory(buf,datasize);
	              buf[0]=TOKEN_REG_PATH;           //命令头
		          REGMSG msg;                     //数据头
		          msg.size=KeySize;
		          msg.count=KeyCnt;
		          memcpy(buf+1,(void*)&msg,size);
		
                  char * tmp=new  char[KeySize];
		          for(dwIndex=0;dwIndex<KeyCnt;dwIndex++)		//枚举项
				  {
			          ZeroMemory(tmp,KeySize);
			          DWORD i=KeySize;
			           RegEnumKeyEx(hKey,dwIndex,tmp,&i,NULL,NULL,NULL,NULL);
			           strcpy(buf+dwIndex*KeySize+size,tmp);
				  }
		          delete[] tmp;
		          RegCloseKey(hKey);
				  buf=(char*)LocalReAlloc(buf, datasize, LMEM_ZEROINIT|LMEM_MOVEABLE);
		}
		
	}
		
    return buf;
}


char* RegeditOpt::FindKey()
{

		char	*szValueName;		//键值名称
		char	*szKeyName;			//子键名称
		LPBYTE	szValueDate;		//键值数据

	char *buf=NULL;
	HKEY		hKey;			//注册表返回句柄
    if(RegOpenKeyEx(MKEY,KeyPath,0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)//打开
	{
       	DWORD dwIndex=0,NameSize,NameCnt,NameMaxLen,Type;
		DWORD KeySize,KeyCnt,KeyMaxLen,DataSize,MaxDateLen;
        //这就是枚举了
		if(RegQueryInfoKey(hKey,NULL,NULL,NULL,&KeyCnt,&KeyMaxLen,NULL,&NameCnt,&NameMaxLen,&MaxDateLen,NULL,NULL)!=ERROR_SUCCESS)
		{

			return NULL;
		}
		if(NameCnt>0&&MaxDateLen>0&&NameSize>0)
		{
			 DataSize=MaxDateLen+1;
			 NameSize=NameMaxLen+100;
			 REGMSG  msg;
             msg.count=NameCnt;        //总个数
			 msg.size=NameSize;          //名字大小
			 msg.valsize=DataSize;       //数据大小
			 int msgsize=sizeof(REGMSG);
			           // 头                   标记            名字                数据
			DWORD size=sizeof(REGMSG)+ sizeof(BYTE)*NameCnt+ NameSize*NameCnt+DataSize*NameCnt+10;
			buf=(char*)LocalAlloc(LPTR, size);
			ZeroMemory(buf,size);
			buf[0]=TOKEN_REG_KEY;         //命令头
            memcpy(buf+1,(void*)&msg,msgsize);     //数据头

            szValueName=(char *)malloc(NameSize);
			szValueDate=(LPBYTE)malloc(DataSize);
			
			char *tmp=buf+msgsize+1;
			for(dwIndex=0;dwIndex<NameCnt;dwIndex++)	//枚举键值
			{
				 ZeroMemory(szValueName,NameSize);
				 ZeroMemory(szValueDate,DataSize);

			      DataSize=MaxDateLen+1;
			      NameSize=NameMaxLen+100;
			      
			      RegEnumValue(hKey,dwIndex,szValueName,&NameSize,NULL,&Type,szValueDate,&DataSize);//读取键值
			      
				  	if(Type==REG_SZ)
					{
				       tmp[0]=MREG_SZ;  
					}
			        if(Type==REG_DWORD)
					{
						//DWORD d;//=(DWORD)*szValueDate;
                      //  CRegistry reg(hKey);
					//	reg.Read(szValueName,&d);
					//	memcpy(szValueDate,(void*)&d,sizeof(DWORD));
				        tmp[0]=MREG_DWORD;  
					}
			        if(Type==REG_BINARY)
					{
				       tmp[0]=MREG_BINARY;
					}
			       if(Type==REG_EXPAND_SZ)
				   {
				       tmp[0]=MREG_EXPAND_SZ;
				   }
				   tmp+=sizeof(BYTE);
				   strcpy(tmp,szValueName);
				   tmp+=msg.size;
				   memcpy(tmp,szValueDate,msg.valsize);
				   tmp+=msg.valsize;
			}
			free(szValueName);
			free(szValueDate);
			 buf=(char*)LocalReAlloc(buf, size, LMEM_ZEROINIT|LMEM_MOVEABLE);
		}   
			
	}
   return buf;
}
