// Player.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "Player.h"
#include "PcView.h"
#pragma comment(lib,"winmm.lib") 
#include "mmsystem.h" 
#include "Digitalv.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString *songName=  new CString [100]; 

extern CPcView* g_pConnectView;
/////////////////////////////////////////////////////////////////////////////
// CPlayer dialog


IMPLEMENT_DYNCREATE(CPlayer, CFormView)

CPlayer::CPlayer()
: CFormView(CPlayer::IDD)
{
	//{{AFX_DATA_INIT(CPlayer)
	m_max=0;//进度条的最大位置初始值为0 
    i = 0;  
    second=0; 
    minute=0; 
    hour=0; 
	//}}AFX_DATA_INIT
}


void CPlayer::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlayer)
	DDX_Control(pDX, m_list1, m_list3);
	DDX_Control(pDX, IDC_PLAYING, m_playing); 
	DDX_Control(pDX, IDC_voice, m_setvoice);
	DDX_Control(pDX, IDC_BUTTON_DEL_SONG, m_delete);
	DDX_Control(pDX, IDC_BUTTON_PRE, m_per);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_next);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlayer, CFormView)
	//{{AFX_MSG_MAP(CPlayer)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_BUTTON_ADD_SONG, AddSong) 
    ON_BN_CLICKED(IDC_ST_Pause, OnButtonPause) 
    ON_BN_CLICKED(IDC_BUTTON_DEL_SONG, DelSong) 
    ON_LBN_DBLCLK(m_list1, songListDBClk) 
    ON_BN_CLICKED(IDC_BUTTON_NEXT, NextSong) 
    ON_BN_CLICKED(IDC_BUTTON_PRE, preSong) 
    ON_WM_TIMER() 
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_voice, Onsetvoices) 
    ON_BN_CLICKED(IDC_BUTTON_PREVIS, OnPrevis) 
    ON_BN_CLICKED(IDC_BUTTON_NEXTS, OnNexts) 
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlayer message handlers
void CPlayer::AddSong() 

{ 
    char filefiler[]="mp3文件(*.mp3)|*.mp3|" 
		"wma文件(*.wma)|*.wma|" 
		"wav文件(*.wav)|*.wav|"; 
	
    CFileDialog 
		f(true,NULL,NULL,OFN_HIDEREADONLY/*|OFN_ALLOWMULTISELECT|OFN_ENABLESIZING*/,filefiler);
  //  f.m_ofn.Flags |=512;//可以选取多个曲目 
	//选取内容部份 
	
	if(f.DoModal()==IDOK) 
		
	{  
		int index = m_list3.GetCurSel(); 		
		FileName=f.GetPathName(); //获取歌曲路径    	
		UpdateData(false); 
		
	} 
	m_list3.AddString(FileName);  //添加歌曲路径 
	songName[m_list3.GetCount()-1] = f.GetFileTitle();  
	
} 

//播放、|| 

void CPlayer::OnButtonPause()  

{ 
	
	++i; 
	
	if (i%2 == 1) 
		
	{ 
		
		KillTimer(0); 
		
		MCI_PLAY_PARMS PlayParms; 
		
		mciSendCommand (m_count, MCI_PAUSE, 0, (DWORD)(LPVOID) &PlayParms); 
		
		SetDlgItemText(IDC_ST_Pause,"▲"); 
		
	} 
	
	else if (i%2 == 0) 
		
	{  
		
		SetTimer(0,1000,NULL); 
		
		MCI_PLAY_PARMS PlayParms; 
		
		mciSendCommand (m_count, MCI_RESUME, 0, (DWORD)(LPVOID) &PlayParms); 
		
		SetDlgItemText(IDC_ST_Pause,"||"); 
		
	} 
	
	
	
}  





//删除歌曲 

void CPlayer::DelSong()  

{ 
	
	CString str; 
	
	int index = m_list3.GetCurSel(); 
	
	m_list3.GetText(index,str);  
	
	stopPlay();  
	
	m_list3.DeleteString(index); 
	
	deltSong();  
	
} 



//删除 

void CPlayer::deltSong() { 
	
	int index = m_list3.GetCurSel(); 
	
	if (m_list3.GetCount()>0) 
		
	{ 
		
		if (index == m_list3.GetCount()-1) 
			
		{ 
			
			index = -1; 
			
		} 
		
		m_list3.GetText(index+1,FileName); 
		
		m_list3.SetCurSel(index+1);  
		
		playSong(FileName); 
		
	} 
	
	else  
		
		MessageBox("没有歌曲了啊"); 
	
} 



//双击播放 

void CPlayer::songListDBClk()  

{   
	
	CFileDialog f(true); 
	
	FileName=f.GetPathName(); 
	
	int i=m_list3.GetCurSel(); 
	
	m_list3.GetText(i,FileName); 
	
	SetDlgItemText(IDC_PLAYING,songName[i]); 
	
	SetDlgItemText(IDC_ST_Pause,"||"); 
	
	playSong(FileName); 
	
} 



//停止播放 

void CPlayer::stopPlay() 

{ 
	
	mciSendCommand(m_count,MCI_CLOSE,0,NULL); 
	
	m_count=0; 
	
	m_flag=true; 
	
	m_play.RedrawWindow(); 
	
} 



//播放音乐 

void CPlayer::playSong(CString str) 

{ 
	
	
	
	FileName=str; 
	
	hour=0; 
	
	minute=0;  second=0; 
	
	MCI_OPEN_PARMS mciopenparms;//打开 
	
	MCI_PLAY_PARMS mciplayparms;//播放  
	
	stopPlay(); 
	
	mciopenparms.lpstrElementName=FileName;//播放路径 
	
	mciopenparms.lpstrDeviceType=NULL;//文件类型 
	
	mciSendCommand(0,MCI_OPEN,MCI_DEVTYPE_WAVEFORM_AUDIO,(DWORD)(LPVOID)&mciopenparms);//向MCI设备发送命令消息 
	
	m_count=mciopenparms.wDeviceID;   
	
	mciplayparms.dwCallback=(DWORD)GetSafeHwnd();  
	
	cdlen=getinfo(MCI_STATUS_LENGTH);    //得到曲目长度  
	
	
	
	int sec = cdlen/1000; 
	
	int min = sec/60; 
	
	int sec2 =sec%60; 
	
	allTime.Format("%02d:%02d:%02d",0,min,sec2); 
	
	
	
	cdto=MCI_MAKE_HMS(MCI_HMS_HOUR(cdlen),MCI_HMS_MINUTE(cdlen),MCI_HMS_SECOND(cdlen));   
	
	mciplayparms.dwFrom=0; 
	
	mciplayparms.dwTo=cdto; 
	
	mciSendCommand(m_count,MCI_PLAY,MCI_FROM,(DWORD)(LPVOID)& mciplayparms); 
	
	SetTimer(0,1000,NULL); 
	
	CClientDC dc(this); 
	
	dc.SetBkColor(RGB(81,54,166)); 
	
	dc.SetTextColor(RGB(255,0,0)); 
	
	dc.TextOut(315,103,allTime); 
	
	
	
	
	
} 



//下一曲 

void CPlayer::NextSong()  

{  
	
    ++i; 
	
	int index = m_list3.GetCurSel(); 
	
	if (index ==( m_list3.GetCount()-1)) 
		
	{ 
		
		index = -1; 
		
	} 
	
	m_list3.GetText(index + 1,FileName);  
	
	m_list3.SetCurSel(index + 1); 
	
	SetDlgItemText(IDC_PLAYING,songName[index + 1]); 
	
	SetDlgItemText(IDC_ST_Pause,"||"); 
	playSong(FileName); 
	
} 





//上一曲 

void CPlayer::preSong()  

{ 
	
	++i; 
	
	int index = m_list3.GetCurSel(); 
	
	if (index == 0) 
		
	{ 
		
		index = m_list3.GetCount(); 
		
	} 
	
	m_list3.GetText(index - 1,FileName); 
	
	m_list3.SetCurSel(index - 1); 
	
	SetDlgItemText(IDC_PLAYING,songName[index - 1]); 
	
	SetDlgItemText(IDC_ST_Pause,"||"); 
	
	playSong(FileName);  
	
} 





//设置声音 

DWORD CPlayer::setvolume(DWORD vol) 

{ 
	
	MCI_DGV_SETAUDIO_PARMS setvolume;//这是设置音量的参数数据结构 
	
	setvolume.dwCallback=NULL; 
	
	setvolume.dwItem=MCI_DGV_SETAUDIO_VOLUME;//动作是设置音量 
	
	setvolume.dwValue=vol;//音量值是vol 
	
	mciSendCommand(m_count,MCI_SETAUDIO,MCI_DGV_SETAUDIO_ITEM|MCI_DGV_SETAUDIO_VALUE,(DWORD)(LPVOID)&setvolume); 
	
	return 0; 
	
} 



//调节声音 

void CPlayer::Onsetvoices(NMHDR* pNMHDR, LRESULT* pResult)  

{ 
	
	// TODO: Add your control notification handler code here 
	
	setvolume(m_setvoice.GetPos()); 
	
	*pResult = 0; 
	
} 





void CPlayer::OnTimer(UINT nIDEvent) 

{   // CString stime;  
	
	second++; 
	CClientDC dc(this); 
	dc.SetBkColor(RGB(81,54,166));//设置放置计数器区域的外观 	
    dc.SetTextColor(RGB(255,0,0));//设置数字显示的颜色 	
	if(second==60)//设置钟表的显示 		
	{ 		
		minute++; 		
		second=0; 		
	}  	
	if(minute==60) 		
	{ 		
		hour++; 	
		minute=0;		
	} 	
	if(hour==24) 		
	{ 	
		hour=0; 	
	} 	
	stime.Format("%02d:%02d:%02d",hour,minute,second);//显示时间进度 	
	dc.TextOut(55,103,stime); 
	//播放完后自动播放下一曲 
	if (stime>allTime) 		
	{ 	
		stopPlay(); 	
		NextSong(); 	
	} 	
	CFormView::OnTimer(nIDEvent); 
	
} 





DWORD CPlayer::getinfo(DWORD item) 

{ 
	
	MCI_STATUS_PARMS mcistatusparms; //歌曲格式 
	
	mcistatusparms.dwCallback=(DWORD)GetSafeHwnd(); //待获取的项目 
	
	mcistatusparms.dwItem=item; 
	
	mcistatusparms.dwReturn=0; //想多媒体设备发送指令获取当前的状态参数 
	
	mciSendCommand(m_count,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&mcistatusparms); 
	
	return mcistatusparms.dwReturn; 
	
} 



//快退 

void CPlayer::OnPrevis()  

{ 
	
	// TODO: Add your control notification handler code here 
	
	MCI_STATUS_PARMS mciStatusParms;  
	
	mciStatusParms.dwItem = MCI_STATUS_POSITION; 
	
	mciSendCommand(m_count, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mciStatusParms);  
	
	long dCurrentPosition = mciStatusParms.dwReturn; //取得当前位置的信息 
	
	MCI_PLAY_PARMS mciPlayParms; 
	
	//每次快退10秒 
	
	mciPlayParms.dwFrom = dCurrentPosition - (DWORD)(10000); 
	
	//发送从指定位置开始播放的命令 
	
	second-=10; 
	
	if(second<=0)//设置钟表的显示 
		
	{ 
		
		minute--; 
		
		second+=60; 
		
	}  
	
	if(minute<=0) 
		
	{ 
		
		hour--; 
		
		minute+=60; 
		
	} 
	
	if(hour==0) 
		
	{ 
		
		hour=24; 
		
	} 
	
	mciSendCommand(m_count, MCI_PLAY, MCI_FROM, (DWORD)&mciPlayParms); 
	
	
	
} 





//快进 

void CPlayer::OnNexts()  

{ 
	
	MCI_STATUS_PARMS mciStatusParms;  
	
	mciStatusParms.dwItem = MCI_STATUS_POSITION; 
	
	mciSendCommand(m_count, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mciStatusParms);  
	
	long dCurrentPosition = mciStatusParms.dwReturn; //取得当前位置的信息 
	
	MCI_PLAY_PARMS mciPlayParms; 
	
	//每次快进10秒 
	
	mciPlayParms.dwFrom = dCurrentPosition + (DWORD)(10000); 
	
	//发送从指定位置开始播放的命令 
	
	second+=10;  if(second>=60)//设置钟表的显示 
		
	{ 
		
		minute++; 
		
		second-=60; 
		
	}  
	
	if(minute>=60) 
		
	{ 
		
		hour++; 
		
		minute-=60; 
		
	} 
	
	if(hour==24) 
		
	{ 
		
		hour=0; 
		
	} 
	
	mciSendCommand(m_count, MCI_PLAY, MCI_FROM, (DWORD)&mciPlayParms);  
	
} 

