#if !defined(AFX_DATASTATUS_H__F6A8C146_11A5_427A_BBAE_4417F61DC099__INCLUDED_)
#define AFX_DATASTATUS_H__F6A8C146_11A5_427A_BBAE_4417F61DC099__INCLUDED_
#include "resource.h"
// #include "PictureEx.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Digistatic.h"		//自定义LED数字头文件
// DataStatus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDataStatus dialog

class CDataStatus : public CDialogBar
{

// Construction
public:
	CDataStatus(CWnd* pParent = NULL);   // standard constructor
    CSize LoadMyBitmap(UINT nID);
	CRect m_rect_parent,m_rect_web,CurRect,m_rect;
	

// Dialog Data
	//{{AFX_DATA(CDataStatus)
	enum { IDD = IDD_DIALOGBAR };
	CStatic	m_gundonweb;
	CStatic	m_gundon;
	CString	m_Men;

	//}}AFX_DATA
	CDigiStatic	m_edt1;		//LED变量
// 	CPictureEx	m_Picture;
//	CString	m_gundon;
	CStatic	m_day;
	CStatic	m_night;
	CString m_strwendu,m_strtianqi,m_strfengsu,m_strgundon,m_1,m_2,m_3,m_strNewUrl1,m_strNewUrl2;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	//}}AFX_VIRTUAL

// Implementation
protected:
	afx_msg LONG OnInitDialog ( UINT, LONG );
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	

	
	CToolBar			m_wndToolBar;	
	CImageList			m_ToolBarList;
	// Generated message map functions
	//{{AFX_MSG(CDataStatus)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	
    

	BOOL GetWeather();
    CString FindStr(CString &str,CString strFind1,CString strFind2,bool yesNo = false);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATASTATUS_H__F6A8C146_11A5_427A_BBAE_4417F61DC099__INCLUDED_)
