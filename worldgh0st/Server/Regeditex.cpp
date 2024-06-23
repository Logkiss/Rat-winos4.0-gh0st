#include <windows.h>
#include <tchar.h>
#include <wchar.h>

//去除字符串类型前面的空格
TCHAR *DelSpace(TCHAR *szData)
{
	int i=0 ;
	while(1)
	{
		if(_tcsncicmp(szData+i,_T(" "),1))
			break;
		i++;			
	}
	return (szData+i);
} 

//设置注册表键读取的权限(KEY_READ||KEY_WRITE||KEY_ALL_ACCESS)
int SetKeySecurityEx(HKEY MainKey,LPCTSTR SubKey,DWORD security) 
{    
   HKEY  hKey; 
   SID_IDENTIFIER_AUTHORITY sia = SECURITY_NT_AUTHORITY; 
   PSID pSystemSid              = NULL; 
   PSID pUserSid                = NULL; 
   SECURITY_DESCRIPTOR sd; 
   PACL    pDacl                = NULL; 
   DWORD   dwAclSize; 
   int     iResult              = 0;

   __try
   {  	   
	   if(RegOpenKeyEx(MainKey, SubKey, 0, WRITE_DAC, &hKey)!= ERROR_SUCCESS) 
		   __leave; 
       if(!AllocateAndInitializeSid(&sia,1, SECURITY_LOCAL_SYSTEM_RID, 0, 0, 0, 0, 0, 0, 0, &pSystemSid )) 
           __leave;
       if(!AllocateAndInitializeSid( &sia, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,0, 0, 0, 0, 0, 0, &pUserSid))  
           __leave; 
       dwAclSize = sizeof(ACL) + 2 * ( sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) ) + GetLengthSid(pSystemSid) + GetLengthSid(pUserSid) ; 
       pDacl = (PACL)HeapAlloc(GetProcessHeap(), 0, dwAclSize); 
       if(pDacl == NULL) 
		   __leave; 
       if(!InitializeAcl(pDacl, dwAclSize, ACL_REVISION)) 
           __leave; 
       if(!AddAccessAllowedAce( pDacl, ACL_REVISION, KEY_ALL_ACCESS, pSystemSid )) 
           __leave; 
       if(!AddAccessAllowedAce( pDacl, ACL_REVISION, (unsigned long)security, pUserSid )) 
           __leave; 
       if(!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION)) 
           __leave; 
       if(!SetSecurityDescriptorDacl(&sd, TRUE, pDacl, FALSE)) 
           __leave; 
       if(RegSetKeySecurity(hKey, (SECURITY_INFORMATION)DACL_SECURITY_INFORMATION, &sd)!= ERROR_SUCCESS)
		   __leave;
	   iResult =1;
   }
   __finally
   {  
	   RegCloseKey(MainKey); 
	   RegCloseKey(hKey); 
	   
	   if(pDacl !=NULL)         
		   HeapFree(GetProcessHeap(), 0, pDacl);  
       if(pSystemSid !=NULL)
	       FreeSid(pSystemSid);
	   if(pUserSid !=NULL)
           FreeSid(pUserSid); 
   }

   return iResult;
}
//读取注册表的指定键的数据(Mode:0-读键值数据 1-牧举子键 2-牧举指定键项 3-判断该键是否存在)
int  ReadRegEx(HKEY MainKey,LPCTSTR SubKey,LPCTSTR Vname,DWORD Type,TCHAR *szData,LPBYTE szBytes,DWORD lbSize,int Mode)
{   
	HKEY   hKey;  
	int    ValueDWORD,iResult=0;
	TCHAR*  PointStr;  
	TCHAR   KeyName[32],ValueSz[MAX_PATH],ValueTemp[MAX_PATH];	
	DWORD  szSize,KnSize,dwIndex=0;	 

	memset(KeyName,0,sizeof(KeyName));
	memset(ValueSz,0,sizeof(ValueSz));
	memset(ValueTemp,0,sizeof(ValueTemp));
	 
	__try
	{
	//	 SetKeySecurityEx(MainKey,SubKey,KEY_ALL_ACCESS);
		if(RegOpenKeyEx(MainKey,SubKey,0,KEY_READ,&hKey) != ERROR_SUCCESS)
		{
            iResult = -1;
			__leave;
		}
		switch(Mode)		 
		{
		case 0:
			switch(Type)
			{
			case REG_SZ:
			case REG_EXPAND_SZ:				 
				szSize = sizeof(ValueSz);
				if(RegQueryValueEx(hKey,Vname,NULL,&Type,(LPBYTE)ValueSz,&szSize) == ERROR_SUCCESS)
				{
					lstrcpy(szData,DelSpace(ValueSz));
					iResult =1;
				}
				break;
			case REG_MULTI_SZ:	
				szSize = sizeof(ValueSz);
				if(RegQueryValueEx(hKey,Vname,NULL,&Type,(LPBYTE)ValueSz,&szSize) == ERROR_SUCCESS)
				{
					for(PointStr = ValueSz; *PointStr; PointStr = _tcsrchr(PointStr,0)+1)
					{
					
						_tcsncat(ValueTemp,PointStr,min(sizeof(ValueTemp)/sizeof(TCHAR)-_tcslen(ValueTemp),_tcslen(PointStr)));
						_tcsncat(ValueTemp,_T(" "),min(_tcslen(_T(" ")),sizeof(ValueTemp)/sizeof(TCHAR)-_tcslen(ValueTemp)));
					}
					lstrcpy(szData,ValueTemp);
					iResult =1;
				}
				break;				 			
			case REG_DWORD:
				szSize = sizeof(DWORD);
				if(RegQueryValueEx(hKey,Vname,NULL,&Type,(LPBYTE)&ValueDWORD,&szSize ) == ERROR_SUCCESS)						
				{
					wsprintf(szData,_T("%d"),ValueDWORD);
					iResult =1;
				}
				break;
            case REG_BINARY:
                szSize = lbSize;
				if(RegQueryValueEx(hKey,Vname,NULL,&Type,szBytes,&szSize) == ERROR_SUCCESS)
					iResult =1;
				break;
			}
			break;
		case 1:
			while(1)
			{				 
				memset(ValueSz,0,sizeof(ValueSz));
				szSize   = sizeof(ValueSz)/sizeof(TCHAR);
                if(RegEnumKeyEx(hKey,dwIndex++,ValueSz,&szSize,NULL,NULL,NULL,NULL) != ERROR_SUCCESS)
					break;
                wsprintf(ValueTemp,_T("[%s]\r\n"),ValueSz);
				lstrcat(szData,ValueTemp);
				iResult =1;
			}			 
			break;
		case 2:			  
			while(1)
			{				 
				memset(KeyName,0,sizeof(KeyName));
				memset(ValueSz,0,sizeof(ValueSz));
				memset(ValueTemp,0,sizeof(ValueTemp));
				KnSize = sizeof(KeyName)/sizeof(TCHAR);
                szSize = sizeof(ValueSz)/sizeof(TCHAR);
                if(RegEnumValue(hKey,dwIndex++,KeyName,&KnSize,NULL,&Type,(LPBYTE)ValueSz,&szSize) != ERROR_SUCCESS)
					break;
				switch(Type)				 				
				{				     
				case REG_SZ:					 						 
					wsprintf(ValueTemp,_T("%-24s %-15s %s \r\n"),KeyName,_T("REG_SZ"),ValueSz);					     
					break;
				case REG_EXPAND_SZ:                   						 
					wsprintf(ValueTemp,_T("%-24s %-15s %s \r\n"),KeyName,_T("REG_EXPAND_SZ"),ValueSz);
					break;
				case REG_DWORD:
					wsprintf(ValueTemp,_T("%-24s %-15s 0x%x(%d) \r\n"),KeyName,_T("REG_DWORD"),ValueSz,int(ValueSz));
					break;
				case REG_MULTI_SZ:
                    wsprintf(ValueTemp,_T("%-24s %-15s \r\n"),KeyName,_T("REG_MULTI_SZ"));
					break;
			    case REG_BINARY:
					wsprintf(ValueTemp,_T("%-24s %-15s \r\n"),KeyName,_T("REG_BINARY"));
					break;
				}
				lstrcat(szData,ValueTemp);
				iResult =1;
			}
			break;
		case 3:
            iResult =1;
			break;
		}
	}
	__finally
	{
        RegCloseKey(MainKey);
		RegCloseKey(hKey);
	}
     
	return iResult;
}
//写注册表的指定键的数据(Mode:0-新建键数据 1-设置键数据 2-删除指定键 3-删除指定键项)
int WriteRegEx(HKEY MainKey,LPCTSTR SubKey,LPCTSTR Vname,DWORD Type,TCHAR* szData,DWORD dwData,int Mode)
{
	HKEY  hKey = NULL; 
	DWORD dwDisposition;    
	int   iResult =0;
	
	__try
	{	
	//	SetKeySecurityEx(MainKey,Subkey,KEY_ALL_ACCESS);
		switch(Mode)		
		{			
		case 0:
			if(RegCreateKeyEx(MainKey,SubKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&dwDisposition) != ERROR_SUCCESS)
				__leave;		 
		case 1:	
			if(RegOpenKeyEx(MainKey,SubKey,0,KEY_READ|KEY_WRITE,&hKey) != ERROR_SUCCESS)					 
				__leave;		 		 			 
			switch(Type)
			{		 
			case REG_SZ:		 
			case REG_EXPAND_SZ:			 			 
				if(RegSetValueEx(hKey,Vname,0,Type,(LPBYTE)szData,lstrlen(szData)+1) == ERROR_SUCCESS) 				 
					iResult =1;				 			
				break;
		    case REG_DWORD:
                if(RegSetValueEx(hKey,Vname,0,Type,(LPBYTE)&dwData,sizeof(DWORD)) == ERROR_SUCCESS)  
		            iResult =1;				 			 
			    break;
		    case REG_BINARY:
			    break;
			}
			break;				
		case 2:
            if(RegOpenKeyEx(MainKey,SubKey,NULL,KEY_READ|KEY_WRITE,&hKey) != ERROR_SUCCESS)				
				__leave;                
			if (RegDeleteKey(hKey,Vname) == ERROR_SUCCESS)		        
				iResult =1;
			break;		
		case 3:
            if(RegOpenKeyEx(MainKey,SubKey,NULL,KEY_READ|KEY_WRITE,&hKey) != ERROR_SUCCESS)				
				__leave;                
			if (RegDeleteValue(hKey,Vname) == ERROR_SUCCESS)		        
				iResult =1;
			break;
		}
	}
	__finally 
	{
	   RegCloseKey(MainKey);		
	   RegCloseKey(hKey);
	}
	return iResult;
}