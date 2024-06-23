#ifndef _CCPUUSGESCTL_H_
#define _CCPUUSGESCTL_H_

#ifdef __cplusplus
	extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////
// CCpuUsgesCtl window

class CCpuUsgesCtl : public CWnd
{
// Construction
public:
	CCpuUsgesCtl();
	BOOL Create(DWORD dwStyle, const RECT& rect,
		        CWnd* pParentWnd, UINT nID); 

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCpuUsgesCtl)
    //}}AFX_VIRTUAL
// Implementation
public:
	void SetGridHeight(int iNewCounts);
	void SetGridCount(int iNewHeight);
	void SetRefreshInterval(int iMs);
	virtual ~CCpuUsgesCtl();
private:
	void GetCpuUsgesNt();
  	void DrawCpuUsges();
	BOOL IsNt();
	void GetCpuUsges9X();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCpuUsgesCtl)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	COLORREF        m_LableColor;
	COLORREF		m_ForeFontColor;
	COLORREF		m_ForeColor;
	COLORREF		m_BackColor;
	COLORREF		m_FreeColor;
	COLORREF		m_UsedColor;
	int				m_iGridHeight;
	int				m_iGridCount;
	int				m_iRefreshInterval;
	unsigned long	m_fNewUsges;
	LARGE_INTEGER   m_liOldIdleTime;
    LARGE_INTEGER   m_liOldSystemTime;
  
};

#ifdef __cplusplus
	}
#endif

#endif