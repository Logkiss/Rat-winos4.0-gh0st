#if !defined(AFX_SHOWWAVE_H__8C28D98A_3AE5_44ED_A88E_6149AE7741B1__INCLUDED_)
#define AFX_SHOWWAVE_H__8C28D98A_3AE5_44ED_A88E_6149AE7741B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// showwave.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShowWave window
#define DRAWBUFFERSIZE 1024

class CShowWave : public CStatic
{
// Construction
public:
	CShowWave();

// Attributes
public:
    int m_nBits;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowWave)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CShowWave();

	int     m_nSize;
	BYTE    DrawData[DRAWBUFFERSIZE];

	HWND m_hWnd;
	void SetHwnd(HWND hWnd);

	CRect rcClient;
	
	CBrush brush;

	void    SetSoundInBuffer(BYTE *buff,int nSize,int nBits);
	// Generated message map functions
protected:
	//{{AFX_MSG(CShowWave)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWWAVE_H__8C28D98A_3AE5_44ED_A88E_6149AE7741B1__INCLUDED_)
