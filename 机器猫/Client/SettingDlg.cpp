// SettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "SettingDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame	*g_pFrame;
extern CIOCPServer	*m_iocpServer;
/////////////////////////////////////////////////////////////////////////////
// CSettingDlg dialog

CString prev_connect_pass;
BOOL dawed_1,dawed_2,dawed_3,dawed_4,dawed_5,dawed_6,dawed_7,dawed_8,dawed_9,dawed_10,dawed_11,dawed_12,dawed_13,dawed_14,dawed_15,dawed_16,dawed_17;
CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingDlg)
	m_disble_poptips = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "PopTips", FALSE);
	m_listen_port = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "ListenPort", 8081);
	m_warning_tone =  ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Alarm", FALSE);
	xintiao = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Xintiao", 0);

	m_line  = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Wange", false);//网格线
	m_national  = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Guoqi", true);//国旗
	m_bisDisablePifu = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "SKIN", true);//软件皮肤

	dawed_1 = m_dawed_1 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width01",TRUE);
	dawed_2 = m_dawed_2 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width02",TRUE);
	dawed_3 = m_dawed_3 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width03",FALSE);
	dawed_4 = m_dawed_4 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width04",TRUE);
	dawed_5 = m_dawed_5 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width05",TRUE);
	dawed_6 = m_dawed_6 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width06",TRUE);
	dawed_7 = m_dawed_7 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width07",TRUE);
	dawed_8 = m_dawed_8 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width08",TRUE);
	dawed_9 = m_dawed_9 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width09",TRUE);
	dawed_10 = m_dawed_10 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width10",TRUE);
	dawed_11 = m_dawed_11 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width11",TRUE);
	dawed_12 = m_dawed_12 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width12",TRUE);
	dawed_13 = m_dawed_13 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width13",TRUE);
	dawed_14 = m_dawed_14 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width14",FALSE);
	dawed_15 = m_dawed_15 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width15",TRUE);
	dawed_16 = m_dawed_16 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width16",TRUE);
	dawed_17 = m_dawed_17 = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width17",TRUE);

	CString ConPass =  ((CClientApp *)AfxGetApp())->m_IniFile.GetString("Settings", "ConnetPass", "");
	ConPass = Base64Decode(ConPass);
	m_ConnetPass = Base64Decode(ConPass);
	//}}AFX_DATA_INIT
	prev_connect_pass = m_ConnetPass;
}


void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingDlg)
	DDX_Check(pDX, IDC_CHECK_DISABLE_POPTIPS, m_disble_poptips);
	DDX_Text(pDX, IDC_EDIT_LISTEN_PORT, m_listen_port);
	DDV_MinMaxUInt(pDX, m_listen_port, 1, 65535);
	DDX_Check(pDX, IDC_CHECK_WARNING_TONE, m_warning_tone);
	DDX_Text(pDX, IDC_EDIT_HEARTBEAT_TIME, xintiao);
	DDV_MinMaxUInt(pDX, xintiao, 0, 60);
	DDX_Text(pDX, IDC_EDIT_CONNETPASS, m_ConnetPass);

	DDX_Control(pDX, IDC_COMBO_Color, m_ListColor); //立即刷新颜色

	DDX_Check(pDX, IDC_NATIONAL, m_national);//国旗
	DDX_Check(pDX, IDC_LINE, m_line); //列表网格
	DDX_Check(pDX, IDC_DISABLE_Pifu, m_bisDisablePifu);//皮肤
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_DAWED_1, m_dawed_1);
	DDX_Check(pDX, IDC_DAWED_2, m_dawed_2);
	DDX_Check(pDX, IDC_DAWED_3, m_dawed_3);
	DDX_Check(pDX, IDC_DAWED_4, m_dawed_4);
	DDX_Check(pDX, IDC_DAWED_5, m_dawed_5);
	DDX_Check(pDX, IDC_DAWED_6, m_dawed_6);
	DDX_Check(pDX, IDC_DAWED_7, m_dawed_7);
	DDX_Check(pDX, IDC_DAWED_8, m_dawed_8);
	DDX_Check(pDX, IDC_DAWED_9, m_dawed_9);
	DDX_Check(pDX, IDC_DAWED_10, m_dawed_10);
	DDX_Check(pDX, IDC_DAWED_11, m_dawed_11);
	DDX_Check(pDX, IDC_DAWED_12, m_dawed_12);
	DDX_Check(pDX, IDC_DAWED_13, m_dawed_13);
	DDX_Check(pDX, IDC_DAWED_14, m_dawed_14);
	DDX_Check(pDX, IDC_DAWED_15, m_dawed_15);
	DDX_Check(pDX, IDC_DAWED_16, m_dawed_16);
	DDX_Check(pDX, IDC_DAWED_17, m_dawed_17);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CSettingDlg)
	ON_BN_CLICKED(IDC_DISABLE_Pifu, OnDISABLEPifu)//皮肤
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingDlg message handlers

CString CSettingDlg::Base64Encode(LPCTSTR lpszSrc)   //加密函数
{
	ASSERT(lpszSrc != NULL && AfxIsValidString(lpszSrc));
	const char BASE64_ENCODE_TABLE[64] = {
		65,  66,  67,  68,  69,  70,  71,  72,  // 00 - 07
			73,  74,  75,  76,  77,  78,  79,  80,  // 08 - 15
			81,  82,  83,  84,  85,  86,  87,  88,  // 16 - 23
			89,  90,  97,  98,  99, 100, 101, 102,  // 24 - 31
			103, 104, 105, 106, 107, 108, 109, 110,  // 32 - 39
			111, 112, 113, 114, 115, 116, 117, 118,  // 40 - 47
			119, 120, 121, 122,  48,  49,  50,  51,  // 48 - 55
			52,  53,  54,  55,  56,  57,  43,  47 };// 56 - 63
		
		unsigned int iTest;
		
		LPCTSTR pInBuffer=lpszSrc;
		
		int nSize = (int)_tcslen(lpszSrc);
		char* pOutBuffer=new char[nSize/3*4+5];
		ZeroMemory(pOutBuffer,nSize/3*4+5);
		
		for(UINT i=0;i<_tcslen(lpszSrc) / 3;i++)
		{
			iTest = (unsigned char) *pInBuffer++;
			iTest = iTest << 8;
			
			iTest = iTest | (unsigned char) *pInBuffer++;
			iTest = iTest << 8;
			
			iTest = iTest | (unsigned char) *pInBuffer++;
			
			//以4 byte倒序写入输出缓冲
			pOutBuffer[3] = BASE64_ENCODE_TABLE[iTest & 0x3F];
			iTest = iTest >> 6;
			pOutBuffer[2] = BASE64_ENCODE_TABLE[iTest & 0x3F];
			iTest = iTest >> 6;
			pOutBuffer[1] = BASE64_ENCODE_TABLE[iTest & 0x3F];
			iTest = iTest >> 6;
			pOutBuffer[0] = BASE64_ENCODE_TABLE[iTest];
			pOutBuffer+=4;
		}
		
		//设置尾部
		switch (_tcslen(lpszSrc) % 3)
		{
		case 0:
			break;
		case 1:
			iTest = (unsigned char) *pInBuffer;
			iTest = iTest << 4;
			pOutBuffer[1] = BASE64_ENCODE_TABLE[iTest & 0x3F];
			iTest = iTest >> 6;
			pOutBuffer[0] = BASE64_ENCODE_TABLE[iTest];
			pOutBuffer[2] = '='; //用'='也就是64码填充剩余部分
			pOutBuffer[3] = '=';
			break;
		case 2:
			iTest = (unsigned char) *pInBuffer++;
			iTest = iTest << 8;
			iTest = iTest | (unsigned char) *pInBuffer;
			iTest = iTest << 2;
			pOutBuffer[2] = BASE64_ENCODE_TABLE[iTest & 0x3F];
			iTest = iTest >> 6;
			pOutBuffer[1] = BASE64_ENCODE_TABLE[iTest & 0x3F];
			iTest = iTest >> 6;
			pOutBuffer[0] = BASE64_ENCODE_TABLE[iTest];
			pOutBuffer[3] = '='; // Fill remaining byte.
			break;
		}
		pOutBuffer-=nSize/3*4;
		CString strEncode=pOutBuffer;
		delete [] pOutBuffer;
		pOutBuffer=NULL;
		return strEncode;
}

CString CSettingDlg::Base64Decode(LPCTSTR lpszSrc)  //解密函数
{
	ASSERT(lpszSrc != NULL && AfxIsValidString(lpszSrc));
	const unsigned int BASE64_DECODE_TABLE[256] = {
		255, 255, 255, 255, 255, 255, 255, 255, //  00 -  07
			255, 255, 255, 255, 255, 255, 255, 255, //  08 -  15
			255, 255, 255, 255, 255, 255, 255, 255, //  16 -  23
			255, 255, 255, 255, 255, 255, 255, 255, //  24 -  31
			255, 255, 255, 255, 255, 255, 255, 255, //  32 -  39
			255, 255, 255,  62, 255, 255, 255,  63, //  40 -  47
			52,  53,  54,  55,  56,  57,  58,  59, //  48 -  55
			60,  61, 255, 255, 255, 255, 255, 255, //  56 -  63
			255,   0,   1,   2,   3,   4,   5,   6, //  64 -  71
			7,   8,   9,  10,  11,  12,  13,  14, //  72 -  79
			15,  16,  17,  18,  19,  20,  21,  22, //  80 -  87
			23,  24,  25, 255, 255, 255, 255, 255, //  88 -  95
			255,  26,  27,  28,  29,  30,  31,  32, //  96 - 103
			33,  34,  35,  36,  37,  38,  39,  40, // 104 - 111
			41,  42,  43,  44,  45,  46,  47,  48, // 112 - 119
			49,  50,  51, 255, 255, 255, 255, 255, // 120 - 127
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255 };
		
		const int nSrcCount=(int)_tcslen(lpszSrc);
		int nSize=nSrcCount/4*3;
		if(lpszSrc[nSrcCount-1]=='=')
			nSize--;
		if(lpszSrc[nSrcCount-2]=='=')
			nSize--;
		char* pOutBuffer=new char[nSize+3];
		ZeroMemory(pOutBuffer,nSize+3);
		LPCTSTR pInBuffer=lpszSrc;
		UINT iTest,iPack;
		for(int i=0;i<nSize/3 ;i++)
		{
			for(int j=0;j<4;j++)
			{
				iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
				//InPtr++;
				if (iTest == 0xFF) 
				{
					j--;
					continue; //读到255非法字符
				}
				iPack = iPack << 6 ;
				iPack = iPack | iTest ;
			}
			pOutBuffer[2] = iPack;
			iPack = iPack >> 8;
			pOutBuffer[1] = iPack;
			iPack = iPack >> 8;
			pOutBuffer[0] = iPack;
			//准备写入后3位
			pOutBuffer+= 3; iPack = 0;
			
		}
		switch(nSize%3)
		{
		case 1:
			iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
			if (iTest != 0xFF)
			{
				iPack = iPack << 6 ;
				iPack = iPack | iTest ;
			}
			iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
			if (iTest != 0xFF)
			{
				iPack = iPack << 6 ;
				iPack = iPack | iTest ;
			}
			iPack = iPack >> 4;
			pOutBuffer[0] = iPack;
			pOutBuffer++;
			break;
		case 2:
			iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
			if (iTest != 0xFF)
			{
				iPack = iPack << 6 ;
				iPack = iPack | iTest ;
			}
			iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
			if (iTest != 0xFF)
			{
				iPack = iPack << 6 ;
				iPack = iPack | iTest ;
			}
			iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
			if (iTest != 0xFF)
			{
				iPack = iPack << 6 ;
				iPack = iPack | iTest ;
			}
			iPack = iPack >> 2;
			pOutBuffer[1] = iPack;
			iPack = iPack >> 8;
			pOutBuffer[0] = iPack;
			pOutBuffer+=2;
			break;
		default:
			break;
		}
		pOutBuffer-=nSize;
		CString strDecode=pOutBuffer;
		delete pOutBuffer;
		return strDecode;
}
BOOL CSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	HINSTANCE hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_MGICON_A),RT_GROUP_ICON);
	HICON m_hIcon = (HICON)::LoadImage(hInstResource,MAKEINTRESOURCE(IDI_MGICON_A),IMAGE_ICON,24, 24, 0);
	if( m_hIcon != 0 )
	{
		SetIcon(m_hIcon,TRUE);
	}

	// TODO: Add extra initialization here
	InitColor();
	return TRUE;  
	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
#include "LogView.h"
extern CLogView* g_pLogView;
void CSettingDlg::OnOK() 
{
	// TODO: Add extra validation here
	BOOL Gupoqi = m_national; //国旗
	BOOL pre_ListGrid = m_line; //网格
	int prev_port = m_listen_port; //端口
	DWORD pre_ListColor = ListColor; //上线颜色
	UINT prev_heatbeattime = xintiao; //心跳
	BOOL pre_poptips = m_disble_poptips; //气泡
	BOOL pre_Warning_tone = m_warning_tone; //语音
	UpdateData(TRUE);
	if (m_ListColor.GetCurSel() != pre_ListColor)
	{
		g_pLogView->InsertLogItem( "上线列表颜色修改成功 ...", 0, 1 );
		if (!((CClientApp *)AfxGetApp())->m_bIsWarning_Tone)
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_SETTING),AfxGetResourceHandle(),SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
		}
		((CClientApp *)AfxGetApp())->m_IniFile.SetDWORD("Settings","ListColor",m_ListColor.GetCurSel());
	}
	if (prev_port != m_listen_port)
	{
		g_pFrame->Activate(m_listen_port, m_iocpServer->m_nMaxConnections);
		g_pLogView->InsertLogItem( "连接端口修改成功 ...", 0, 1 );
		if (!((CClientApp *)AfxGetApp())->m_bIsWarning_Tone)
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_SETTING),AfxGetResourceHandle(),SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
		}
        ((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "ListenPort", m_listen_port);
	}
	if (pre_poptips != m_disble_poptips)
	{
		((CClientApp *)AfxGetApp())->m_bIsDisablePopTips = m_disble_poptips;
		g_pLogView->InsertLogItem( "气泡提示修改成功 ...", 0, 1 );
		if (!((CClientApp *)AfxGetApp())->m_bIsWarning_Tone)
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_SETTING),AfxGetResourceHandle(),SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
		}
		((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "PopTips", m_disble_poptips);
	}
	if (pre_Warning_tone != m_warning_tone)
	{
		((CClientApp *)AfxGetApp())->m_bIsWarning_Tone = m_warning_tone;
		g_pLogView->InsertLogItem( "语音提示修改成功 ...", 0, 1 );
		if (!((CClientApp *)AfxGetApp())->m_bIsWarning_Tone)
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_SETTING),AfxGetResourceHandle(),SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
		}
		((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Alarm", m_warning_tone);
	}
	if (pre_ListGrid != m_line)
	{
		g_pLogView->InsertLogItem( "上线列表网格线修改成功，重启生效 ...", 0, 1 );
		if (!((CClientApp *)AfxGetApp())->m_bIsWarning_Tone)
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_SETTING),AfxGetResourceHandle(),SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
		}
        ((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Wange", m_line);
	}
	if (Gupoqi != m_national)
	{
		g_pLogView->InsertLogItem( "显示国旗图标修改成功，重启生效 ...", 0, 1 );
		if (!((CClientApp *)AfxGetApp())->m_bIsWarning_Tone)
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_SETTING),AfxGetResourceHandle(),SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
		}
		((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Guoqi", m_national);
	}
	////列表栏目
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width01", m_dawed_1);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width02", m_dawed_2);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width03", m_dawed_3);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width04", m_dawed_4);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width05", m_dawed_5);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width06", m_dawed_6);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width07", m_dawed_7);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width08", m_dawed_8);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width09", m_dawed_9);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width10", m_dawed_10);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width11", m_dawed_11);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width12", m_dawed_12);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width13", m_dawed_13);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width14", m_dawed_14);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width15", m_dawed_15);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width16", m_dawed_16);
	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Width17", m_dawed_17);

	BOOL dawed_DATA = NULL;
	if((dawed_1 != m_dawed_1)||(dawed_2 != m_dawed_2)||(dawed_3 != m_dawed_3)||(dawed_4 != m_dawed_4)||(dawed_5 != m_dawed_5)||
		(dawed_6 != m_dawed_6)||(dawed_7 != m_dawed_7)||(dawed_8 != m_dawed_8)||(dawed_9 != m_dawed_9)||(dawed_10 != m_dawed_10)||
		(dawed_11 != m_dawed_11)||(dawed_12 != m_dawed_12)||(dawed_13 != m_dawed_13)||(dawed_14 != m_dawed_14)||(dawed_15 != m_dawed_15)
		||(dawed_16 != m_dawed_16)||(dawed_17 != m_dawed_17))
	{
		dawed_DATA = TRUE;
	}

	if (dawed_DATA)
	{
		g_pLogView->InsertLogItem( "上线栏目修改成功，重启主控生效...", 0, 1 );
	}

	if(prev_heatbeattime!= xintiao //心跳
		||pre_ListGrid != m_line  //网格
		||Gupoqi != m_national //国旗
		||dawed_DATA)
	{
		if (!((CClientApp *)AfxGetApp())->m_bIsWarning_Tone)
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_SETTING),AfxGetResourceHandle(),SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
		}
		if(MessageBox("系统参数已修改,重启应用生效，是否确认重启！"," 温馨提示！",MB_ICONQUESTION|MB_YESNO)==IDYES)
		{
			((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "Xintiao", xintiao); //心跳
			g_pFrame->m_bSetRestart = TRUE;
			AfxGetMainWnd()->SendMessage(WM_CLOSE);
			ExitProcess(0); //删除这个重启会有提示音或者问题
		}
	}

	//通讯秘钥设置
	CString ConnetPass = Base64Encode(m_ConnetPass);
	ConnetPass = Base64Encode(ConnetPass);

	DWORD IpPass = strlen(m_ConnetPass);
	if(IpPass == 0)
 	{
 		AfxMessageBox( "通讯密码不能为空，请输入6-16位数密码");
 		return ;
 	}
	if (prev_connect_pass != m_ConnetPass)
	{
		if(IpPass <= 5)
		{
			AfxMessageBox( "通讯密码过短, 最少6位数密码... ...");
			return ;
		}
		else if(IpPass >= 16)
		{
			
			AfxMessageBox( "通讯密码过长! 请修改6-16位数内... ...");
			return ;
		}
		g_pLogView->InsertLogItem( "通讯秘钥修改成功 ...",0,1);
		if (!((CClientApp *)AfxGetApp())->m_bIsWarning_Tone)
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_SETTING),AfxGetResourceHandle(),SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
		}
		((CClientApp *)AfxGetApp())->m_IniFile.SetString("Settings", "ConnetPass", ConnetPass);  //写配置通讯秘钥
	}
	CDialog::OnOK();
}
void CSettingDlg::InitColor(void)
{
	m_ListColor.AddString("【黑色】");
	m_ListColor.SetItemData(0,0);
	m_ListColor.AddString("【红色】");
	m_ListColor.SetItemData(0,1);
	m_ListColor.AddString("【蓝色】");
	m_ListColor.SetItemData(0,2);
	m_ListColor.AddString("【棕色】");
	m_ListColor.SetItemData(0,3);
	m_ListColor.AddString("【绿色】");
	m_ListColor.SetItemData(0,4);
	m_ListColor.AddString("【紫色】");
	m_ListColor.SetItemData(0,5);
	ListColor = ((CClientApp *)AfxGetApp())->m_IniFile.GetDWORD("Settings","ListColor");
	if (ListColor <= 0 || ListColor > 6)
    {
		ListColor = 1;
	}
	m_ListColor.SetCurSel(ListColor);
}
void CSettingDlg::OnDISABLEPifu() //显示软件皮肤
{	
	UpdateData(true);
	if ( ((CButton *)GetDlgItem(IDC_DISABLE_Pifu))->GetCheck() == TRUE )
	{
		char Path[200]={0};
		GetModuleFileName(NULL,Path,200);
		*strrchr(Path,'\\')=0;
		strcat(Path,"\\Plugins\\SkinH.she");
		SkinH_AttachEx(Path,NULL);
	}
	else
	{
		SkinH_Detach();//关闭皮肤效果
	}
	m_bisDisablePifu=((CClientApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "SKIN", m_bisDisablePifu);
}

