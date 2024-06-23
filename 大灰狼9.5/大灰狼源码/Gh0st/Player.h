#if !defined(AFX_PLAYER_H__0C76ADAB_76AA_4CBD_950F_9A2A0130065E__INCLUDED_)
#define AFX_PLAYER_H__0C76ADAB_76AA_4CBD_950F_9A2A0130065E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Player.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPlayer dialog

class CPlayer : public CFormView
{
protected:
	CPlayer();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPlayer)


	CString m_title;//歌曲名称 	
	UINT m_count;//标志 	
	DWORD cdlen; //歌曲时间 	
	CString FileName; 
	BOOL m_flag; 
	int i; 
	int second,hour,minute; 
	int m_max; 
	CString stime;//播放时长 
	CString allTime;//歌曲总时长 
	DWORD cdfrom,cdto,aa;//歌曲的起点和终点 
// Dialog Data
	//{{AFX_DATA(CPlayer)
	enum { IDD = IDD_PLAYER_DIALOG };
	CListBox	m_list3;
	CStatic m_playing; 
	CStatic m_ST_Pause; 
	CSliderCtrl m_setvoice; 
	CSliderCtrl m_control; 
	CButton m_delete; 
	CButton m_per; 
	CButton m_next; 
	CButton m_play; 
	CButton m_pause; 
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlayer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPlayer)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void AddSong(); 	
	afx_msg void stopPlay(); 	
	afx_msg void playSong(CString str); 	
	afx_msg void OnButtonPause(); 	
	afx_msg void DelSong(); 	
	afx_msg void OnSelchangelist(); 	
	afx_msg void songListDBClk(); 	
	afx_msg void NextSong(); 	
	afx_msg DWORD setvolume(DWORD vol); 	
	afx_msg void preSong(); 	
	afx_msg void OnTimer(UINT nIDEvent) ; 	
	afx_msg void OnButton1(); 	
	afx_msg void deltSong(); 	
	afx_msg DWORD getinfo(DWORD item); 	
	afx_msg void OnVoice(NMHDR* pNMHDR, LRESULT* pResult); 	
	afx_msg void Onsetvoices(NMHDR* pNMHDR, LRESULT* pResult); 	
	afx_msg void OnPrevis(); 	
	afx_msg void OnNexts(); 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYER_H__0C76ADAB_76AA_4CBD_950F_9A2A0130065E__INCLUDED_)
