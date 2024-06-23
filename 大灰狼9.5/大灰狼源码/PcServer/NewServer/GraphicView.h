// GraphicView.h : interface of the CGraphicView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHICVIEW_H__44A96B0C_B5D7_4689_8DAF_45686713E90F__INCLUDED_)
#define AFX_GRAPHICVIEW_H__44A96B0C_B5D7_4689_8DAF_45686713E90F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CGraphicView : public CView
{
protected: // create from serialization only
	CGraphicView();
	DECLARE_DYNCREATE(CGraphicView)

// Attributes
public:
	CGraphicDoc* GetDocument();
	int m_nRealTimeCounter;
    int m_nSinCounter;
	HWND m_hPE;
	BOOL m_canReal;
	BOOL m_Real;

	double StartTime;
	double ZoomStart;
	double ZoomEnd;
	double ZoomInterval;
	double ZoomSmallInterval;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphicView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	void CreateSimpleGraph();
	virtual ~CGraphicView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGraphicView)
	afx_msg void OnDrawBar();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDrawLine();
	afx_msg void OnDrawPie();
	afx_msg void OnDrawScatter();
	afx_msg void OnDrawWhisker();
	afx_msg void OnDrawStackedBar();
	afx_msg void OnDrawXyLine();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDraw3dBar();
	afx_msg void OnDraw3dStackedBar();
	afx_msg void OnDraw3dLine();
	afx_msg void OnDraw3dPie();
	afx_msg void OnBkLine();
	afx_msg void OnBkXy();
	afx_msg void OnBkArea();
	afx_msg void OnBkBar();
	afx_msg void OnBkBarGroup();
	afx_msg void OnBkPie();
	afx_msg void OnButton32781();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDrawNew();
	afx_msg void OnDrawSile();
	afx_msg void OnReal();
	afx_msg void OnDrawgraph();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void CreateSimpleSGraph();
};

#ifndef _DEBUG  // debug version in GraphicView.cpp
inline CGraphicDoc* CGraphicView::GetDocument()
   { return (CGraphicDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHICVIEW_H__44A96B0C_B5D7_4689_8DAF_45686713E90F__INCLUDED_)
