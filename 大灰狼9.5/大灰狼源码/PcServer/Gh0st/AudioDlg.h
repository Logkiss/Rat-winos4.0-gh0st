#if !defined(AFX_AUDIODLG_H__4ACB9C40_E36F_40B8_B977_9C0DEC3ED659__INCLUDED_)
#define AFX_AUDIODLG_H__4ACB9C40_E36F_40B8_B977_9C0DEC3ED659__INCLUDED_
#include "Audio.h"
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")
#include "SoundToWav.h"
#include "ShowWave.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AudioDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CAudioDlg dialog
class CAudioDlg : public CDialog
{
// Construction
public:
	CAudioDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAudioDlg)
	enum { IDD = IDD_DIALOG_AUDIO };
	CComboBox	m_combo_input_lines;
	CComboBox	m_combo_input_drive;
	CShowWave	m_datacurve;
	BOOL	m_bIsSendLocalAudio;
	BOOL	m_bCheckRec;
	//}}AFX_DATA

	void OnReceiveComplete();
	bool m_bIsWorking;
	CAudio m_Audio;


	void ShowLinesCombox(CString str,int nSelect);

	CString csFileName;
	CSoundToWav m_wavefile;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;
	CString m_IPAddress;
	HICON m_hIcon;
	UINT m_nTotalRecvBytes;
	HANDLE	m_hWorkThread;

	int m_SelectedDevice,m_SelectedLines;
	char m_cBufOut[SIZE_AUDIO_FRAME];
	char m_cBufln[SIZE_AUDIO_PACKED];

	static	DWORD WINAPI WorkThread(LPVOID lparam);
	// Generated message map functions
	//{{AFX_MSG(CAudioDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSendLocalaudio();
	afx_msg void OnCheckRec();
	afx_msg void OnSelchangeComboDriveIn();
	afx_msg void OnSelchangeComboInputlines();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIODLG_H__4ACB9C40_E36F_40B8_B977_9C0DEC3ED659__INCLUDED_)
