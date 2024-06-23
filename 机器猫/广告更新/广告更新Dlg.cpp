// 广告更新Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "广告更新.h"
#include "广告更新Dlg.h"
#include <afxinet.h>
#include <WinSock.h>
#pragma comment(lib, "ws2_32.lib")

#include "decode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMyDlg::CMyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyDlg)
	DDX_Control(pDX, IDC_PROGRESS1, m_Prog);
	DDX_Control(pDX, IDC_EDIT_LOG, m_edit_log);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyDlg, CDialog)
	//{{AFX_MSG_MAP(CMyDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR() //颜色必填
	ON_BN_CLICKED(IDB_BTN_START1,OnBtnStart1)//开始下载1
	ON_BN_CLICKED(IDB_BTN_START2,OnBtnStart2)//开始下载2
	ON_BN_CLICKED(IDB_BTN_START3,OnBtnStart3)//开始下载3
	ON_BN_CLICKED(IDB_BTN_START4,OnBtnStart4)//开始下载4
	ON_BN_CLICKED(IDB_BTN_STOP,OnBtnStop)   //退出
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDlg message handlers

///颜色提示
void CMyDlg::LogOut(LPCSTR outtext)
{
	int length = m_edit_log.GetWindowTextLength(); 
	m_edit_log.SetSel(length, length);
	m_edit_log.SetLimitText(MAXDWORD); // 设置最大长度
	m_edit_log.ReplaceSel (outtext); 
}
HBRUSH CMyDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if ((pWnd->GetDlgCtrlID() == IDC_EDIT_LOG) && (nCtlColor == CTLCOLOR_EDIT))
	{
		COLORREF clr = RGB(0, 255, 0);
		pDC->SetTextColor(clr);
		clr = RGB(0,0,0);
		pDC->SetBkColor(clr);
		return CreateSolidBrush(clr);
	}
	else
	{
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}
}
////
int nTerminate  = 0;
int nDownloaded = 0;
//获取域名IP
CString GotHost(CString YM)
{
	WSAData wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);
	hostent *phst=gethostbyname(YM);
	in_addr * iddr=(in_addr*)phst->h_addr;
	unsigned long IPUL=iddr->s_addr;
	char *IP=inet_ntoa(*iddr);
	return IP;
}
UINT DownloadFile1(LPVOID pParam)
{
	CWnd*			pwnd = AfxGetMainWnd();
	CProgressCtrl*	m_Prog = (CProgressCtrl*)pwnd->GetDlgItem(IDC_PROGRESS1);
	CButton*		bStart1 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START1);
	CButton*		bStart2 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START2);
	CButton*		bStart3 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START3);
	CButton*		bStart4 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START4);
	
	char				filebuf[512];
	CInternetSession	netSession;
	CStdioFile			*fTargFile;
	int					outfs;
	CString				szFile,DKOU,FileSize,KBin,KBsec,NewName,Perc,Install,SSVP,SSVO,HTTP;
	
	try
	{ 	
		pwnd->GetDlgItemText(IDC_EDIT3,szFile);
		pwnd->GetDlgItemText(IDC_EDIT5,DKOU);
		pwnd->SetDlgItemText(IDC_EDIT_LOG,"正在连接服务器...\r\n");

		////////////////////////////////////////////////
		CString IP=GotHost(szFile);

		char WEjnt[] = {'a','H','R','0','c','D','o','v','L','w','=','=','\0'}; 
		HTTP = Base64Decode(WEjnt);//http://
		char hGgCA[] = {'L','2','R','h','d','C','9','J','b','n','N','0','Y','W','x','s','M','S','5','k','Y','X','Q','=','\0'};
 		Install = Base64Decode(hGgCA);
		CString url0 = CString(HTTP)+IP+":"+DKOU+Install;  //Install.dat
//		MessageBoxA(0,url0,0,0);
		/////////////////////////////////////////////////////////////////////////////////////////////
		fTargFile = netSession.OpenURL(url0,1,INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);
		
		nDownloaded = 1;

		COleDateTime dlStart = COleDateTime::GetCurrentTime();
		int filesize = fTargFile->SeekToEnd();
		fTargFile->SeekToBegin();
		outfs = filesize / 1024;		// 计算文件大小（千字节）
		FileSize.Format(_T("%d"),outfs);	// 以KB为单位格式文件大小

#if 0

		CString filepath,filename;
		filepath = _T("Cache"); //自己可以考虑将文件下载到什么位置，这里设定为F盘根目录下面 
		filename = filepath + fTargFile->GetFileName();  //文件路径 
		CFile fDestFile(filename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
#endif
		// 在当前目录创建新的目标文件
	//	CFile fDestFile(fTargFile->GetFileName(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		CString filepath,filename;
		filepath = _T("Cache\\"); //自己可以考虑将文件下载到什么位置，这里设定为F盘根目录下面 
		filename = filepath + fTargFile->GetFileName();  //文件路径 
		CFile fDestFile(filename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		int byteswrite;		// 写入文件的字节数
		int pos = 0;		// 当前进度条的位置
		int nperc,kbrecv;	// 进度条的百分比,获取到的数据大小（Kbs为单位）
		double secs,kbsec;	// 记录秒数, 速度（KB/秒）
		
		// 如果文件名太长，缩短窗口的标题并在状态显示
		NewName = fTargFile->GetFileName();					// 获取新文件名
		
		if(fTargFile->GetFileName().GetLength() >= 10)
		{
			NewName = fTargFile->GetFileName().Mid(0,7);	// 分割文件
			NewName = NewName + "...";
		}
		
		pwnd->SetDlgItemText(IDC_EDIT_LOG,"正在更新...");
		m_Prog->SetRange32(0,filesize);
		
		while (byteswrite = fTargFile->Read(filebuf, 512))	// 读取文件
		{
			if(nTerminate == 1)						// 如果点击取消下载
			{
				fDestFile.Close();					// 关闭我们的目标文件
				fTargFile->Close();					// 关闭远程文件
				delete fTargFile;					// 删除CStdioFile对象，以防止泄漏
				pwnd->SetDlgItemText(IDC_EDIT_LOG,"更新时已被用户取消！"); // Set satus bar text
				AfxEndThread(0);					// 结束下载线程
			}

			// 根据开始时间与当前时间比较，获取秒数
			COleDateTimeSpan dlElapsed = COleDateTime::GetCurrentTime() - dlStart;
			secs = dlElapsed.GetTotalSeconds();
			pos = pos + byteswrite;					// 设置新的进度条位置
			fDestFile.Write(filebuf, byteswrite);	// 将实际数据写入文件
			m_Prog->SetPos(pos);
			
			nperc = pos * 100 / filesize;			// 进度百分比
			kbrecv = pos / 1024;					// 获取收到的数据
			kbsec = kbrecv / secs;					// 获取每秒下载多少（KB）

			Perc.Format(_T("%d"), nperc);				// 格式化进度百分比
			KBin.Format(_T("%d"), kbrecv);				// 格式化已下载数据大小（KB）
			KBsec.Format(_T("%d"), (int)kbsec);			// 格式化下载速度（KB/秒）

			pwnd->SetDlgItemText(IDC_EDIT_FILESIZE,FileSize + "KB");// 远程文件大小
			pwnd->SetDlgItemText(IDC_EDIT_SIZEOK,KBin + "KB");		// 已下载大小
			pwnd->SetDlgItemText(IDC_EDIT2,KBsec + "KB/秒");		// 下载速度
			pwnd->SetDlgItemText(IDC_EDIT4,Perc + "%");	
	//		pwnd->SetDlgItemText(IDC_EDIT_LOG,"----------------------------"+Perc + "%------------------------------");				// 进度百分比
		}
		// 下载完成，关闭文件
	 
		fDestFile.Close();
	}

	catch(CInternetException *IE)
	{
		CString strerror;
		TCHAR error[255];

		IE->GetErrorMessage(error,255); // 获取错误消息
		strerror = error;

		pwnd->SetDlgItemText(IDC_EDIT_LOG,strerror);
		pwnd->SetDlgItemText(IDB_BTN_STOP, _T("Exit"));
		nDownloaded = 0;
		delete fTargFile;
		IE->Delete();					// 删除异常对象，以防止泄漏
	}
	// 恢复默认
	pwnd->SetDlgItemText(IDC_EDIT2, _T("Kb/秒"));
//	pwnd->SetDlgItemText(IDC_EDIT3, _T("Loading..."));
	pwnd->SetDlgItemText(IDC_EDIT4, _T("0%"));
	
	delete fTargFile;
	if(nDownloaded == 1)
	{
		pwnd->SetDlgItemText(IDC_EDIT_LOG, "更新完成！");
		bStart1->EnableWindow(TRUE); 
		bStart2->EnableWindow(TRUE);
		bStart3->EnableWindow(TRUE);
		bStart4->EnableWindow(TRUE);
		rename("Cache\\Install1.dat","Cache\\Install.dat"); 
	}
	return 0;
}
UINT DownloadFile2(LPVOID pParam)
{
	CWnd*			pwnd = AfxGetMainWnd();
	CProgressCtrl*	m_Prog = (CProgressCtrl*)pwnd->GetDlgItem(IDC_PROGRESS1);
	CButton*		bStart1 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START1);
	CButton*		bStart2 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START2);
	CButton*		bStart3 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START3);
	CButton*		bStart4 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START4);
	
	char				filebuf[512];
	CInternetSession	netSession;
	CStdioFile			*fTargFile;
	int					outfs;
	CString				szFile,DKOU,FileSize,KBin,KBsec,NewName,Perc,Install,SSVP,SSVO,HTTP;
	
	try
	{ 	
		pwnd->GetDlgItemText(IDC_EDIT3,szFile);
		pwnd->GetDlgItemText(IDC_EDIT5,DKOU);
		pwnd->SetDlgItemText(IDC_EDIT_LOG,"正在连接服务器...\r\n");

		////////////////////////////////////////////////
		CString IP=GotHost(szFile);
		char WEjnt[] = {'a','H','R','0','c','D','o','v','L','w','=','=','\0'}; 
		HTTP = Base64Decode(WEjnt);//http://
		char hGgCA[] = {'L','2','R','h','d','C','9','J','b','n','N','0','Y','W','x','s','M','i','5','k','Y','X','Q','=','\0'};
		Install = Base64Decode(hGgCA);
		CString url0 = CString(HTTP)+IP+":"+DKOU+Install;  //Install.dat
		//		MessageBoxA(0,url0,0,0);
		/////////////////////////////////////////////////////////////////////////////////////////////
		fTargFile = netSession.OpenURL(url0,1,INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);
		
		nDownloaded = 1;

		COleDateTime dlStart = COleDateTime::GetCurrentTime();
		int filesize = fTargFile->SeekToEnd();
		fTargFile->SeekToBegin();
		outfs = filesize / 1024;		// 计算文件大小（千字节）
		FileSize.Format(_T("%d"),outfs);	// 以KB为单位格式文件大小

#if 0

		CString filepath,filename;
		filepath = _T("Cache"); //自己可以考虑将文件下载到什么位置，这里设定为F盘根目录下面 
		filename = filepath + fTargFile->GetFileName();  //文件路径 
		CFile fDestFile(filename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
#endif
		// 在当前目录创建新的目标文件
	//	CFile fDestFile(fTargFile->GetFileName(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		CString filepath,filename;
		filepath = _T("Cache\\"); //自己可以考虑将文件下载到什么位置，这里设定为F盘根目录下面 
		filename = filepath + fTargFile->GetFileName();  //文件路径 
		CFile fDestFile(filename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		int byteswrite;		// 写入文件的字节数
		int pos = 0;		// 当前进度条的位置
		int nperc,kbrecv;	// 进度条的百分比,获取到的数据大小（Kbs为单位）
		double secs,kbsec;	// 记录秒数, 速度（KB/秒）
		
		// 如果文件名太长，缩短窗口的标题并在状态显示
		NewName = fTargFile->GetFileName();					// 获取新文件名
		
		if(fTargFile->GetFileName().GetLength() >= 10)
		{
			NewName = fTargFile->GetFileName().Mid(0,7);	// 分割文件
			NewName = NewName + "...";
		}
		
		pwnd->SetDlgItemText(IDC_EDIT_LOG,"正在更新...");
		m_Prog->SetRange32(0,filesize);
		
		while (byteswrite = fTargFile->Read(filebuf, 512))	// 读取文件
		{
			if(nTerminate == 1)						// 如果点击取消下载
			{
				fDestFile.Close();					// 关闭我们的目标文件
				fTargFile->Close();					// 关闭远程文件
				delete fTargFile;					// 删除CStdioFile对象，以防止泄漏
				pwnd->SetDlgItemText(IDC_EDIT_LOG,"更新时已被用户取消！"); // Set satus bar text
				AfxEndThread(0);					// 结束下载线程
			}

			// 根据开始时间与当前时间比较，获取秒数
			COleDateTimeSpan dlElapsed = COleDateTime::GetCurrentTime() - dlStart;
			secs = dlElapsed.GetTotalSeconds();
			pos = pos + byteswrite;					// 设置新的进度条位置
			fDestFile.Write(filebuf, byteswrite);	// 将实际数据写入文件
			m_Prog->SetPos(pos);
			
			nperc = pos * 100 / filesize;			// 进度百分比
			kbrecv = pos / 1024;					// 获取收到的数据
			kbsec = kbrecv / secs;					// 获取每秒下载多少（KB）

			Perc.Format(_T("%d"), nperc);				// 格式化进度百分比
			KBin.Format(_T("%d"), kbrecv);				// 格式化已下载数据大小（KB）
			KBsec.Format(_T("%d"), (int)kbsec);			// 格式化下载速度（KB/秒）

			pwnd->SetDlgItemText(IDC_EDIT_FILESIZE,FileSize + "KB");// 远程文件大小
			pwnd->SetDlgItemText(IDC_EDIT_SIZEOK,KBin + "KB");		// 已下载大小
			pwnd->SetDlgItemText(IDC_EDIT2,KBsec + "KB/秒");		// 下载速度
			pwnd->SetDlgItemText(IDC_EDIT4,Perc + "%");	
	//		pwnd->SetDlgItemText(IDC_EDIT_LOG,"----------------------------"+Perc + "%------------------------------");				// 进度百分比
		}
		// 下载完成，关闭文件
	 
		fDestFile.Close();
	}

	catch(CInternetException *IE)
	{
		CString strerror;
		TCHAR error[255];

		IE->GetErrorMessage(error,255); // 获取错误消息
		strerror = error;

		pwnd->SetDlgItemText(IDC_EDIT_LOG,strerror);
		pwnd->SetDlgItemText(IDB_BTN_STOP, _T("Exit"));
		nDownloaded = 0;
		delete fTargFile;
		IE->Delete();					// 删除异常对象，以防止泄漏
	}
	// 恢复默认
	pwnd->SetDlgItemText(IDC_EDIT2, _T("Kb/秒"));
//	pwnd->SetDlgItemText(IDC_EDIT3, _T("Loading..."));
	pwnd->SetDlgItemText(IDC_EDIT4, _T("0%"));
	
	delete fTargFile;
	if(nDownloaded == 1)
	{
		pwnd->SetDlgItemText(IDC_EDIT_LOG, "更新完成！");
		bStart1->EnableWindow(TRUE); 
		bStart2->EnableWindow(TRUE);
		bStart3->EnableWindow(TRUE);
		bStart4->EnableWindow(TRUE);
		rename("Cache\\Install2.dat","Cache\\Install.dat"); 
	}
	return 0;
}
UINT DownloadFile3(LPVOID pParam)
{
	CWnd*			pwnd = AfxGetMainWnd();
	CProgressCtrl*	m_Prog = (CProgressCtrl*)pwnd->GetDlgItem(IDC_PROGRESS1);
	CButton*		bStart1 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START1);
	CButton*		bStart2 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START2);
	CButton*		bStart3 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START3);
	CButton*		bStart4 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START4);
	
	char				filebuf[512];
	CInternetSession	netSession;
	CStdioFile			*fTargFile;
	int					outfs;
	CString				szFile,DKOU,FileSize,KBin,KBsec,NewName,Perc,Install,SSVP,SSVO,HTTP;
	
	try
	{ 	
		pwnd->GetDlgItemText(IDC_EDIT3,szFile);
		pwnd->GetDlgItemText(IDC_EDIT5,DKOU);
		pwnd->SetDlgItemText(IDC_EDIT_LOG,"正在连接服务器...\r\n");

		////////////////////////////////////////////////
		CString IP=GotHost(szFile);
		char WEjnt[] = {'a','H','R','0','c','D','o','v','L','w','=','=','\0'}; 
		HTTP = Base64Decode(WEjnt);//http://
		char hGgCA[] = {'L','2','R','h','d','C','9','J','b','n','N','0','Y','W','x','s','M','y','5','k','Y','X','Q','=','\0'};
		Install = Base64Decode(hGgCA);
		CString url0 = CString(HTTP)+IP+":"+DKOU+Install;  //Install.dat
		//		MessageBoxA(0,url0,0,0);
		/////////////////////////////////////////////////////////////////////////////////////////////
		fTargFile = netSession.OpenURL(url0,1,INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);
		
		nDownloaded = 1;

		COleDateTime dlStart = COleDateTime::GetCurrentTime();
		int filesize = fTargFile->SeekToEnd();
		fTargFile->SeekToBegin();
		outfs = filesize / 1024;		// 计算文件大小（千字节）
		FileSize.Format(_T("%d"),outfs);	// 以KB为单位格式文件大小

#if 0
		CString filepath,filename;
		filepath = _T("Cache"); //自己可以考虑将文件下载到什么位置，这里设定为F盘根目录下面 
		filename = filepath + fTargFile->GetFileName();  //文件路径 
		CFile fDestFile(filename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
#endif
		// 在当前目录创建新的目标文件
	//	CFile fDestFile(fTargFile->GetFileName(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		CString filepath,filename;
		filepath = _T("Cache\\"); //自己可以考虑将文件下载到什么位置，这里设定为F盘根目录下面 
		filename = filepath + fTargFile->GetFileName();  //文件路径 
		CFile fDestFile(filename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		int byteswrite;		// 写入文件的字节数
		int pos = 0;		// 当前进度条的位置
		int nperc,kbrecv;	// 进度条的百分比,获取到的数据大小（Kbs为单位）
		double secs,kbsec;	// 记录秒数, 速度（KB/秒）
		
		// 如果文件名太长，缩短窗口的标题并在状态显示
		NewName = fTargFile->GetFileName();					// 获取新文件名
		if(fTargFile->GetFileName().GetLength() >= 10)
		{
			NewName = fTargFile->GetFileName().Mid(0,7);	// 分割文件
			NewName = NewName + "...";
		}
		pwnd->SetDlgItemText(IDC_EDIT_LOG,"正在更新...");
		m_Prog->SetRange32(0,filesize);
		while (byteswrite = fTargFile->Read(filebuf, 512))	// 读取文件
		{
			if(nTerminate == 1)						// 如果点击取消下载
			{
				fDestFile.Close();					// 关闭我们的目标文件
				fTargFile->Close();					// 关闭远程文件
				delete fTargFile;					// 删除CStdioFile对象，以防止泄漏
				pwnd->SetDlgItemText(IDC_EDIT_LOG,"更新时已被用户取消！"); // Set satus bar text
				AfxEndThread(0);					// 结束下载线程
			}
			// 根据开始时间与当前时间比较，获取秒数
			COleDateTimeSpan dlElapsed = COleDateTime::GetCurrentTime() - dlStart;
			secs = dlElapsed.GetTotalSeconds();
			pos = pos + byteswrite;					// 设置新的进度条位置
			fDestFile.Write(filebuf, byteswrite);	// 将实际数据写入文件
			m_Prog->SetPos(pos);
			
			nperc = pos * 100 / filesize;			// 进度百分比
			kbrecv = pos / 1024;					// 获取收到的数据
			kbsec = kbrecv / secs;					// 获取每秒下载多少（KB）

			Perc.Format(_T("%d"), nperc);				// 格式化进度百分比
			KBin.Format(_T("%d"), kbrecv);				// 格式化已下载数据大小（KB）
			KBsec.Format(_T("%d"), (int)kbsec);			// 格式化下载速度（KB/秒）

			pwnd->SetDlgItemText(IDC_EDIT_FILESIZE,FileSize + "KB");// 远程文件大小
			pwnd->SetDlgItemText(IDC_EDIT_SIZEOK,KBin + "KB");		// 已下载大小
			pwnd->SetDlgItemText(IDC_EDIT2,KBsec + "KB/秒");		// 下载速度
			pwnd->SetDlgItemText(IDC_EDIT4,Perc + "%");	
	//		pwnd->SetDlgItemText(IDC_EDIT_LOG,"----------------------------"+Perc + "%------------------------------");				// 进度百分比
		}
		// 下载完成，关闭文件
	 
		fDestFile.Close();
	}

	catch(CInternetException *IE)
	{
		CString strerror;
		TCHAR error[255];

		IE->GetErrorMessage(error,255); // 获取错误消息
		strerror = error;

		pwnd->SetDlgItemText(IDC_EDIT_LOG,strerror);
		pwnd->SetDlgItemText(IDB_BTN_STOP, _T("Exit"));
		nDownloaded = 0;
		delete fTargFile;
		IE->Delete();					// 删除异常对象，以防止泄漏
	}
	// 恢复默认
	pwnd->SetDlgItemText(IDC_EDIT2, _T("Kb/秒"));
//	pwnd->SetDlgItemText(IDC_EDIT3, _T("Loading..."));
	pwnd->SetDlgItemText(IDC_EDIT4, _T("0%"));
	
	delete fTargFile;
	if(nDownloaded == 1)
	{
		pwnd->SetDlgItemText(IDC_EDIT_LOG, "更新完成！");
		bStart1->EnableWindow(TRUE); 
		bStart2->EnableWindow(TRUE);
		bStart3->EnableWindow(TRUE);
		bStart4->EnableWindow(TRUE);
		rename("Cache\\Install3.dat","Cache\\Install.dat"); 
	}
	return 0;
}
UINT DownloadFile4(LPVOID pParam)
{
	CWnd*			pwnd = AfxGetMainWnd();
	CProgressCtrl*	m_Prog = (CProgressCtrl*)pwnd->GetDlgItem(IDC_PROGRESS1);
	CButton*		bStart1 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START1);
	CButton*		bStart2 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START2);
	CButton*		bStart3 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START3);
	CButton*		bStart4 = (CButton*)pwnd->GetDlgItem(IDB_BTN_START4);
	
	char				filebuf[512];
	CInternetSession	netSession;
	CStdioFile			*fTargFile;
	int					outfs;
	CString				szFile,DKOU,FileSize,KBin,KBsec,NewName,Perc,Install,SSVP,SSVO,HTTP;
	
	try
	{ 	
		pwnd->GetDlgItemText(IDC_EDIT3,szFile);
		pwnd->GetDlgItemText(IDC_EDIT5,DKOU);
		pwnd->SetDlgItemText(IDC_EDIT_LOG,"正在连接服务器...\r\n");

		////////////////////////////////////////////////
		CString IP=GotHost(szFile);
		char WEjnt[] = {'a','H','R','0','c','D','o','v','L','w','=','=','\0'}; 
		HTTP = Base64Decode(WEjnt);//http://
		char hGgCA[] = {'L','2','R','h','d','C','9','J','b','n','N','0','Y','W','x','s','N','C','5','k','Y','X','Q','=','\0'};
		Install = Base64Decode(hGgCA);
		CString url0 = CString(HTTP)+IP+":"+DKOU+Install;  //Install.dat
		//		MessageBoxA(0,url0,0,0);
		/////////////////////////////////////////////////////////////////////////////////////////////
		fTargFile = netSession.OpenURL(url0,1,INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);
		
		nDownloaded = 1;

		COleDateTime dlStart = COleDateTime::GetCurrentTime();
		int filesize = fTargFile->SeekToEnd();
		fTargFile->SeekToBegin();
		outfs = filesize / 1024;		// 计算文件大小（千字节）
		FileSize.Format(_T("%d"),outfs);	// 以KB为单位格式文件大小

#if 0

		CString filepath,filename;
		filepath = _T("Cache"); //自己可以考虑将文件下载到什么位置，这里设定为F盘根目录下面 
		filename = filepath + fTargFile->GetFileName();  //文件路径 
		CFile fDestFile(filename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
#endif
		// 在当前目录创建新的目标文件
	//	CFile fDestFile(fTargFile->GetFileName(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		CString filepath,filename;
		filepath = _T("Cache\\"); //自己可以考虑将文件下载到什么位置，这里设定为F盘根目录下面 
		filename = filepath + fTargFile->GetFileName();  //文件路径 
		CFile fDestFile(filename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		int byteswrite;		// 写入文件的字节数
		int pos = 0;		// 当前进度条的位置
		int nperc,kbrecv;	// 进度条的百分比,获取到的数据大小（Kbs为单位）
		double secs,kbsec;	// 记录秒数, 速度（KB/秒）
		
		// 如果文件名太长，缩短窗口的标题并在状态显示
		NewName = fTargFile->GetFileName();					// 获取新文件名
		
		if(fTargFile->GetFileName().GetLength() >= 10)
		{
			NewName = fTargFile->GetFileName().Mid(0,7);	// 分割文件
			NewName = NewName + "...";
		}
		
		pwnd->SetDlgItemText(IDC_EDIT_LOG,"正在更新...");
		m_Prog->SetRange32(0,filesize);
		
		while (byteswrite = fTargFile->Read(filebuf, 512))	// 读取文件
		{
			if(nTerminate == 1)						// 如果点击取消下载
			{
				fDestFile.Close();					// 关闭我们的目标文件
				fTargFile->Close();					// 关闭远程文件
				delete fTargFile;					// 删除CStdioFile对象，以防止泄漏
				pwnd->SetDlgItemText(IDC_EDIT_LOG,"更新时已被用户取消！"); // Set satus bar text
				AfxEndThread(0);					// 结束下载线程
			}

			// 根据开始时间与当前时间比较，获取秒数
			COleDateTimeSpan dlElapsed = COleDateTime::GetCurrentTime() - dlStart;
			secs = dlElapsed.GetTotalSeconds();
			pos = pos + byteswrite;					// 设置新的进度条位置
			fDestFile.Write(filebuf, byteswrite);	// 将实际数据写入文件
			m_Prog->SetPos(pos);
			
			nperc = pos * 100 / filesize;			// 进度百分比
			kbrecv = pos / 1024;					// 获取收到的数据
			kbsec = kbrecv / secs;					// 获取每秒下载多少（KB）

			Perc.Format(_T("%d"), nperc);				// 格式化进度百分比
			KBin.Format(_T("%d"), kbrecv);				// 格式化已下载数据大小（KB）
			KBsec.Format(_T("%d"), (int)kbsec);			// 格式化下载速度（KB/秒）

			pwnd->SetDlgItemText(IDC_EDIT_FILESIZE,FileSize + "KB");// 远程文件大小
			pwnd->SetDlgItemText(IDC_EDIT_SIZEOK,KBin + "KB");		// 已下载大小
			pwnd->SetDlgItemText(IDC_EDIT2,KBsec + "KB/秒");		// 下载速度
			pwnd->SetDlgItemText(IDC_EDIT4,Perc + "%");	
	//		pwnd->SetDlgItemText(IDC_EDIT_LOG,"----------------------------"+Perc + "%------------------------------");				// 进度百分比
		}
		// 下载完成，关闭文件
	 
		fDestFile.Close();
	}

	catch(CInternetException *IE)
	{
		CString strerror;
		TCHAR error[255];

		IE->GetErrorMessage(error,255); // 获取错误消息
		strerror = error;

		pwnd->SetDlgItemText(IDC_EDIT_LOG,strerror);
		pwnd->SetDlgItemText(IDB_BTN_STOP, _T("Exit"));
		nDownloaded = 0;
		delete fTargFile;
		IE->Delete();					// 删除异常对象，以防止泄漏
	}
	// 恢复默认
	pwnd->SetDlgItemText(IDC_EDIT2, _T("Kb/秒"));
//	pwnd->SetDlgItemText(IDC_EDIT3, _T("Loading..."));
	pwnd->SetDlgItemText(IDC_EDIT4, _T("0%"));
	
	delete fTargFile;
	if(nDownloaded == 1)
	{
		pwnd->SetDlgItemText(IDC_EDIT_LOG, "更新完成！");
		bStart1->EnableWindow(TRUE); 
		bStart2->EnableWindow(TRUE);
		bStart3->EnableWindow(TRUE);
		bStart4->EnableWindow(TRUE);
		rename("Cache\\Install4.dat","Cache\\Install.dat"); 
	}
	return 0;
}
UINT DownloadFile5(LPVOID pParam)
{
	//公告
	CWnd* pwnd = AfxGetMainWnd();
	while (1)
	{
		CString	szFile,DKOU,SSVO;
		pwnd->GetDlgItemText(IDC_EDIT3,szFile);
		pwnd->GetDlgItemText(IDC_EDIT5,DKOU);
		DWORD recvsize = 0;
		SOCKET sockInt;
		struct sockaddr_in serverAddr;
		//struct hostent *hp;
		WORD sockVersion;
		WSADATA wsaData;
		sockVersion = MAKEWORD(2,2);
		WSAStartup(sockVersion, &wsaData);
		//创建SOCK
		sockInt = socket(AF_INET, SOCK_STREAM, 0);
		if(sockInt == INVALID_SOCKET)
		{
			AfxMessageBox("socket error!\n");
			WSACleanup();
			return 0;
		}
		
		//获取服务器IP和端口
		CString IP=GotHost(szFile);
		serverAddr.sin_family = AF_INET;    
		char tgtIP[30] = {0};
		struct hostent *hp = NULL;
		if ((hp = gethostbyname(IP)) != NULL)
		{
			in_addr in;
			memcpy(&in, hp->h_addr, hp->h_length);
			lstrcpy(tgtIP,inet_ntoa(in));
		}
		
		serverAddr.sin_addr.s_addr = inet_addr(tgtIP);
		serverAddr.sin_port = htons(8688);
		
		//连接服务
		if(connect(sockInt, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			char dsafd[256] = {0};
			strcat(dsafd,"连接服务器失败.");
			strcat(dsafd,"\r\n");
			strcat(dsafd,"请联系QQ:2953571010");
			
			pwnd->SetDlgItemText(IDC_EDIT_LOG,dsafd);
			WSACleanup();
			//return 0;
		}
		
		char USERIMFOR[256] = {0}, buff[256] = {0};
		wsprintf( USERIMFOR, "Login:%s@%s", "SVP7", "#$^DDFgdsfet#@^FDGBF(^$%$@fdg" );
		if( send(sockInt, USERIMFOR, sizeof(USERIMFOR), 0) == SOCKET_ERROR )
		{
			//	AfxMessageBox("帐号过期.");
			WSACleanup();
			//	return 0;
		}
		Sleep(50);
		int Ret = recv( sockInt, buff, sizeof(buff), NULL );
		if ( Ret == 0 || Ret == SOCKET_ERROR )
		{
			//	AfxMessageBox("账号错误");
			closesocket(sockInt);
			WSACleanup();
			//	return 0;
		}
		
		if (strstr( buff, "Pass" ) != NULL)//通过验证
		{
			closesocket(sockInt);
			WSACleanup();
			char dsafd[256] = {0};
			strcat(dsafd,"连接服务器成功.");
			strcat(dsafd,"\r\n\r\n");
			strcat(dsafd,buff+6);
			
			pwnd->SetDlgItemText(IDC_EDIT_LOG,dsafd);
		}
		Sleep(5000);
	}
}
void CMyDlg::OnBtnStart1()  //开始下载1
{
	DeleteFile("Cache\\Install.dat");
	
	CButton* bStart1 = (CButton*)GetDlgItem(IDB_BTN_START);
	CButton* bStart2 = (CButton*)GetDlgItem(IDB_BTN_START2);
	CButton* bStart3 = (CButton*)GetDlgItem(IDB_BTN_START3);
	CButton* bStart4 = (CButton*)GetDlgItem(IDB_BTN_START4);
	bStart1->EnableWindow(FALSE);
	bStart2->EnableWindow(FALSE);
	bStart3->EnableWindow(FALSE);
	bStart4->EnableWindow(FALSE);
	SetDlgItemText(IDB_BTN_STOP, _T("取消"));
	nTerminate = 0; // Clear the Terminate value to 0 (starting a new download)
	// Begin our download thread
	CWinThread* pThread = AfxBeginThread(DownloadFile1,GetSafeHwnd(),THREAD_PRIORITY_NORMAL);
	
}
void CMyDlg::OnBtnStart2()  //开始下载2
{
	DeleteFile("Cache\\Install.dat");
	
	CButton* bStart1 = (CButton*)GetDlgItem(IDB_BTN_START);
	CButton* bStart2 = (CButton*)GetDlgItem(IDB_BTN_START2);
	CButton* bStart3 = (CButton*)GetDlgItem(IDB_BTN_START3);
	CButton* bStart4 = (CButton*)GetDlgItem(IDB_BTN_START4);
	bStart1->EnableWindow(FALSE);
	bStart2->EnableWindow(FALSE);
	bStart3->EnableWindow(FALSE);
	bStart4->EnableWindow(FALSE);
	SetDlgItemText(IDB_BTN_STOP, _T("取消"));
	nTerminate = 0; // Clear the Terminate value to 0 (starting a new download)
	// Begin our download thread
	CWinThread* pThread = AfxBeginThread(DownloadFile2,GetSafeHwnd(),THREAD_PRIORITY_NORMAL);
	
}
void CMyDlg::OnBtnStart3()  //开始下载3
{
	DeleteFile("Cache\\Install.dat");
	
	CButton* bStart1 = (CButton*)GetDlgItem(IDB_BTN_START);
	CButton* bStart2 = (CButton*)GetDlgItem(IDB_BTN_START2);
	CButton* bStart3 = (CButton*)GetDlgItem(IDB_BTN_START3);
	CButton* bStart4 = (CButton*)GetDlgItem(IDB_BTN_START4);
	bStart1->EnableWindow(FALSE);
	bStart2->EnableWindow(FALSE);
	bStart3->EnableWindow(FALSE);
	bStart4->EnableWindow(FALSE);
	SetDlgItemText(IDB_BTN_STOP, _T("取消"));
	nTerminate = 0; // Clear the Terminate value to 0 (starting a new download)
	// Begin our download thread
	CWinThread* pThread = AfxBeginThread(DownloadFile3,GetSafeHwnd(),THREAD_PRIORITY_NORMAL);
	
}
#include <WinBase.h>

void CMyDlg::OnBtnStart4()  //开始下载4
{
	DeleteFile("Cache\\Install.dat");
	CButton* bStart1 = (CButton*)GetDlgItem(IDB_BTN_START);
	CButton* bStart2 = (CButton*)GetDlgItem(IDB_BTN_START2);
	CButton* bStart3 = (CButton*)GetDlgItem(IDB_BTN_START3);
	CButton* bStart4 = (CButton*)GetDlgItem(IDB_BTN_START4);
	bStart1->EnableWindow(FALSE);
	bStart2->EnableWindow(FALSE);
	bStart3->EnableWindow(FALSE);
	bStart4->EnableWindow(FALSE);
	SetDlgItemText(IDB_BTN_STOP, _T("取消"));
	nTerminate = 0; // Clear the Terminate value to 0 (starting a new download)
	// Begin our download thread
	CWinThread* pThread = AfxBeginThread(DownloadFile4,GetSafeHwnd(),THREAD_PRIORITY_NORMAL);
	
}
void CMyDlg::OnBtnStop()  //退出
{
	// TODO: Add your control notification handler code here
	CButton* bGetFile1= (CButton*)GetDlgItem(IDB_BTN_START1);
	CButton* bGetFile2= (CButton*)GetDlgItem(IDB_BTN_START2);
	CButton* bGetFile3= (CButton*)GetDlgItem(IDB_BTN_START3);
	CButton* bGetFile4= (CButton*)GetDlgItem(IDB_BTN_START4);
	CString btnlabel;
	GetDlgItemText(IDB_BTN_STOP,btnlabel);
	
	if(btnlabel == "取消")
	{
		// Direct to our thread to terminate
		nTerminate = 1; 
		
		//		SetDlgItemText(IDC_EDIT3, _T("loading..."));
		SetDlgItemText(IDC_EDIT4, _T("0%"));
		SetDlgItemText(IDC_EDIT2, _T("0Kb/秒"));
		m_Prog.SetPos(0);
		SetDlgItemText(IDB_BTN_STOP, _T("退出"));
		bGetFile1->EnableWindow(TRUE);
		bGetFile2->EnableWindow(TRUE);
		bGetFile3->EnableWindow(TRUE);
		bGetFile4->EnableWindow(TRUE);
	}
	else
	{
		CDialog::OnOK();
	}
}
BOOL CMyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

//	CheckRadioButton(IDC_EDIT_LOG,IDC_EDIT_LOG,IDC_EDIT_LOG);
//	LogOut("广告更新公告\r\n");
	SetDlgItemText(IDC_EDIT3,"SVP7.NET");
	SetDlgItemText(IDC_EDIT5,"9874");


	CWinThread* pThread = AfxBeginThread(DownloadFile5,GetSafeHwnd(),THREAD_PRIORITY_NORMAL);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CMyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////////////

