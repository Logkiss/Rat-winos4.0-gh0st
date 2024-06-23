// GraphicView.cpp : implementation of the CGraphicView class
//

#include "stdafx.h"
#include "Graphic.h"
//#include "Pegrpapi.h"
#include "GraphicDoc.h"
#include "GraphicView.h"
#include "math.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "CoolControlsManager.h"
#define GetRandom(min,max) ((rand()%(int)(((max)+1)-(min)))+(min))

/////////////////////////////////////////////////////////////////////////////
// CGraphicView

IMPLEMENT_DYNCREATE(CGraphicView, CView)

BEGIN_MESSAGE_MAP(CGraphicView, CView)
//{{AFX_MSG_MAP(CGraphicView)
ON_COMMAND(ID_DRAW_BAR, OnDrawBar)
ON_WM_ERASEBKGND()
ON_COMMAND(ID_DRAW_LINE, OnDrawLine)
ON_COMMAND(ID_DRAW_PIE, OnDrawPie)
ON_COMMAND(ID_DRAW_SCATTER, OnDrawScatter)
ON_COMMAND(ID_DRAW_WHISKER, OnDrawWhisker)
ON_COMMAND(ID_DRAW_STACKED_BAR, OnDrawStackedBar)
ON_COMMAND(ID_DRAW_XY_LINE, OnDrawXyLine)
ON_WM_TIMER()
ON_COMMAND(ID_DRAW_3D_BAR, OnDraw3dBar)
ON_COMMAND(ID_DRAW_3D_STACKED_BAR, OnDraw3dStackedBar)
ON_COMMAND(ID_DRAW_3D_LINE, OnDraw3dLine)
ON_COMMAND(ID_DRAW_3D_PIE, OnDraw3dPie)
ON_COMMAND(ID_BK_LINE, OnBkLine)
ON_COMMAND(ID_BK_XY, OnBkXy)
ON_COMMAND(ID_BK_AREA, OnBkArea)
ON_COMMAND(ID_BK_BAR, OnBkBar)
ON_COMMAND(ID_BK_BAR_GROUP, OnBkBarGroup)
ON_COMMAND(ID_BK_PIE, OnBkPie)
ON_COMMAND(ID_BUTTON32781, OnButton32781)
ON_WM_SIZE()
ON_COMMAND(ID_DRAW_NEW, OnDrawNew)
ON_COMMAND(ID_DRAW_SILE, OnDrawSile)
ON_COMMAND(ID_REAL, OnReal)
ON_COMMAND(ID_DRAWGRAPH, OnDrawgraph)
//}}AFX_MSG_MAP
// Standard printing commands
ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphicView construction/destruction

CGraphicView::CGraphicView()
{
	// TODO: add construction code here
	m_nRealTimeCounter = 0;
	m_nSinCounter = 0;
}

CGraphicView::~CGraphicView()
{
}

BOOL CGraphicView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGraphicView drawing

void CGraphicView::OnDraw(CDC* pDC)
{
	CGraphicDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CGraphicView printing

BOOL CGraphicView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGraphicView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGraphicView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CGraphicView diagnostics

#ifdef _DEBUG
void CGraphicView::AssertValid() const
{
	CView::AssertValid();
}

void CGraphicView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGraphicDoc* CGraphicView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGraphicDoc)));
	return (CGraphicDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGraphicView message handlers

void CGraphicView::OnInitialUpdate() 
{
	CView::OnInitialUpdate(); 
	AfxGetApp()->GetMainWnd()->SetWindowText(_T("曲线显示完整版 V1.0"));
	GetCtrlManager().InstallHook();//使所有控件浮动
	//	OnDraw3dStackedBar();
	// TODO: Add your specialized code here and/or call the base class
	
}

void CGraphicView::OnDrawBar() 
{
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	if(m_hPE)
		m_hPE=NULL;
	//! Right button click to show popup menu. //
	//! Double Click to show customization dialog. //
	
	// This example builds upon the basic CreateSimpleGraph '000' example chart //
	CreateSimpleGraph();
	
	// Below tells object to automatically generate statistical
	// comparison subsets, object will actually have
	// 6 subsets when finished initializing.
//	int nArray[2];
//	nArray[0] = PEAS_AVGAP;
//	nArray[1] = PEAS_AVGPP; 
///	PEvset (m_hPE, PEP_naAUTOSTATSUBSETS, nArray, 2);
	
	// Other various properties //
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_DARK_LINE);
// 	PEnset(m_hPE, PEP_nLEGENDSTYLE, PELS_1_LINE_INSIDE_AXIS);
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);
// 	PEnset(m_hPE, PEP_bMARKDATAPOINTS, TRUE); 
	
} 

void CGraphicView::CreateSimpleGraph()
{ 
	//! Right button click to show popup menu. //
	//! Double Click to show customization dialog. //
	//! Left-Click and drag to draw zoom box. Use popup memu or 'z' to undo zoom. // 
	
	// Simple example show the basics of a graph object. //
	// Graph's generally only contain YData because we assume
	// data is plotted equally spaced left to right.
	m_canReal=FALSE;
	CRect rect;
	GetClientRect( rect ); 
//	m_hPE = PEcreate(PECONTROL_GRAPH, WS_CHILD|WS_VISIBLE, &rect, m_hWnd, 1001);
// 	if (m_hPE) 
// 	{
// 		// Prepare images in memory //
// //		PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
// 		
// 		// Pass Data //
// 	//	PEnset(m_hPE, PEP_nSUBSETS, 4);
// 	//	PEnset(m_hPE, PEP_nPOINTS, 12);
// 		
// 		float fY;
// 		for (int s=0; s<=3; s++)
// 		{
// 			for (int p=0; p<=11; p++)
// 			{
// 				fY = float((p + 1) * 50) + GetRandom(2, 250);
// 				PEvsetcellEx(m_hPE, PEP_faYDATA, s, p, &fY);
// 			}
// 		}
		
		// Set DataShadows to show 3D //
// /		PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_SHADOWS);
// 		PEnset(m_hPE, PEP_nBORDERTYPES, PETAB_DROP_SHADOW);
// 		
// 		PEszset(m_hPE, PEP_szMAINTITLE, "历史线性统计曲线");
// 		PEszset(m_hPE, PEP_szSUBTITLE, "程序设计与实现--戚高"); 
// 		PEszset(m_hPE, PEP_szYAXISLABEL, "最大应用量");
// 		PEszset(m_hPE, PEP_szXAXISLABEL, "月份");
// 		PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
// 		PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);//现实类型
// 		PEnset(m_hPE, PEP_nGRIDLINECONTROL, PEGLC_NONE);//是否有X,Y
// 		PEnset(m_hPE, PEP_bALLOWRIBBON, TRUE);
//// 		PEnset(m_hPE, PEP_nALLOWZOOMING, PEAZ_HORZANDVERT);
//		PEnset(m_hPE, PEP_nZOOMSTYLE, PEZS_RO2_NOT);
// 		
		// subset labels //
// 		PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 0, "长沙" );
// 		PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 1, "上海" );
// 		PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 2, "北京" );
// 		PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 3, "广州" );
// 		
// 		// point labels //
// 		PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 0, "一月" );
// 		PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 1, "二月" );
// 		PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 2, "三月" );
// 		PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 3, "四月" );
// 		PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 4, "五月" );
// 		PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 5, "六月" );
// 		PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 6, "七月" );
// 		PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 7, "八月" );
// 		PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 8, "九月" );
// 		PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 9, "十月" );
// 		PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 10, "十一月" );
// 		PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 11, "十二月" );
// 		
// 		// subset colors //
// 		DWORD dwArray[4] = { RGB(198,0,0), RGB( 0, 198, 198 ), RGB( 198,198,0 ), RGB( 0,198,0 ) };
// 		PEvsetEx(m_hPE, PEP_dwaSUBSETCOLORS, 0, 4, dwArray, 0);
// 		
// 		// subset line types //
// 		int nLineTypes[] = { PELT_MEDIUMSOLID, PELT_MEDIUMSOLID, 
// 			PELT_MEDIUMSOLID, PELT_MEDIUMSOLID, PELT_MEDIUMSOLID, 
// 			PELT_MEDIUMSOLID, PELT_MEDIUMSOLID, PELT_MEDIUMSOLID};
// 		PEvset(m_hPE, PEP_naSUBSETLINETYPES, nLineTypes, 8);
// 		
// 		// subset point types //
// 		int nPointTypes[] = { PEPT_DOTSOLID, PEPT_UPTRIANGLESOLID, 
// 			PEPT_SQUARESOLID, PEPT_DOWNTRIANGLESOLID, PEPT_DOT, 
// 			PEPT_UPTRIANGLE, PEPT_SQUARE, PEPT_DOWNTRIANGLE };
// 		PEvset(m_hPE, PEP_naSUBSETPOINTTYPES, nPointTypes, 8);
// 		
// 		// Allow stacked type graphs //
// 		PEnset(m_hPE, PEP_bNOSTACKEDDATA, FALSE);
// 		
// 		// Various other features //
// 		PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
// 		PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// 		PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_LINE);
// 		PEnset(m_hPE, PEP_bSIMPLEPOINTLEGEND, TRUE);
// 		PEnset(m_hPE, PEP_bSIMPLELINELEGEND, TRUE);
// 		PEnset(m_hPE, PEP_nLEGENDSTYLE, PELS_1_LINE);
// 		PEnset(m_hPE, PEP_nGRAPHPLUSTABLE, PEGPT_BOTH);
// 		
// 		PEnset(m_hPE, PEP_nGRADIENTBARS, 8);
// 		PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
// 		PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
// 		PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
// 		PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
// 		PEnset(m_hPE, PEP_bLINESHADOWS, TRUE);
// 		PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE);
// 		
// 	}
}

BOOL CGraphicView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CGraphicView::OnDrawLine() 
{ 
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	if(m_hPE)
		m_hPE=NULL;
	CreateSimpleGraph();  
	int nArray[2];
//	nArray[0] = PEAS_AVGAP;
//	nArray[1] = PEAS_AVGPP; 
//	PEvset (m_hPE, PEP_naAUTOSTATSUBSETS, nArray, 2); 
// 	nArray[0] = 4;  // fourth subset
// 	nArray[1] = 5;  // fifth subset
// 	PEvset(m_hPE, PEP_naRANDOMSUBSETSTOGRAPH, nArray, 2); 
// 	PEnset(m_hPE, PEP_nSCROLLINGSUBSETS, 1);
// 	PEnset(m_hPE, PEP_nTABLEWHAT, PETW_ALLSUBSETS); 
// 	PEnset(m_hPE, PEP_nGRAPHPLUSTABLE, PEGPT_BOTH);
// 	PEnset(m_hPE, PEP_nDATAPRECISION, 0);
// 	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE); 
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_NO_BORDER);
// 	PEnset(m_hPE, PEP_nLEGENDSTYLE, PELS_1_LINE_INSIDE_AXIS);
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);
// 	PEnset(m_hPE, PEP_nBESTFITDEGREE, PEBFD_4TH);
// 	PEnset(m_hPE, PEP_bMARKDATAPOINTS, TRUE);
// 	PEnset(m_hPE, PEP_nLEGENDSTYLE, PELS_1_LINE);  
}



void CGraphicView::OnDrawPie() 
{
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	// This example builds upon the basic CreateSimpleGraph '000' example chart //
	CreateSimpleGraph();
	
	// Disable default scale //
////	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	
// 	// Generally, a custom y axis will need manual range control //
// 	PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
// 	
// 	double manminY = 0.0F;
// 	PEvset(m_hPE, PEP_fMANUALMINY, &manminY, 1);
// 	
// 	double manmaxY = 1000.0F;
// 	PEvset(m_hPE, PEP_fMANUALMAXY, &manmaxY, 1);
// 	
	// Create custom grid lines with HorzLineAnnotations //
// 	// Note that GridLineControl will control these annotations //
// 	double horzlineannot = 200.0F;
// 	PEvsetcell(m_hPE, PEP_faHORZLINEANNOTATION, 0, &horzlineannot);
// 	int horzlinetyp = PELAT_GRIDLINE;
// 	PEvsetcell(m_hPE, PEP_naHORZLINEANNOTATIONTYPE, 0, &horzlinetyp);
// 	PEvsetcell(m_hPE, PEP_szaHORZLINEANNOTATIONTEXT, 0, "|LLow Value");
// 	
// 	horzlineannot = 500.0F;
// 	PEvsetcell(m_hPE, PEP_faHORZLINEANNOTATION, 1, &horzlineannot);
// 	horzlinetyp = PELAT_GRIDLINE;
// 	PEvsetcell(m_hPE, PEP_naHORZLINEANNOTATIONTYPE, 1, &horzlinetyp);
// 	PEvsetcell(m_hPE, PEP_szaHORZLINEANNOTATIONTEXT, 1, "|LMedium Value");
// 	
// 	horzlineannot = 800.0F;
// 	PEvsetcell(m_hPE, PEP_faHORZLINEANNOTATION, 2, &horzlineannot);
// 	horzlinetyp = PELAT_GRIDLINE;
// 	PEvsetcell(m_hPE, PEP_naHORZLINEANNOTATIONTYPE, 2, &horzlinetyp);
// 	PEvsetcell(m_hPE, PEP_szaHORZLINEANNOTATIONTEXT, 2, "|LHigh Value");
// 	
// 	horzlineannot = 350.0F;
// 	PEvsetcell(m_hPE, PEP_faHORZLINEANNOTATION, 3, &horzlineannot);
// 	horzlinetyp = PELAT_GRIDTICK;
// 	PEvsetcell(m_hPE, PEP_naHORZLINEANNOTATIONTYPE, 3, &horzlinetyp);
// 	PEvsetcell(m_hPE, PEP_szaHORZLINEANNOTATIONTEXT, 3, "");
// 	
// 	horzlineannot = 650.0F;
// 	PEvsetcell(m_hPE, PEP_faHORZLINEANNOTATION, 4, &horzlineannot);
// 	horzlinetyp = PELAT_GRIDTICK;
// 	PEvsetcell(m_hPE, PEP_naHORZLINEANNOTATIONTYPE, 4, &horzlinetyp);
// 	PEvsetcell(m_hPE, PEP_szaHORZLINEANNOTATIONTEXT, 4, "");
// 	
// 	// Set LeftMargin to allocate space for line annotation text //
// 	// Use the longest string used in annotations.
// 	PEszset(m_hPE, PEP_szLEFTMARGIN, "Medium Value ");
// 	
// 	// Set this to see annotations //
// 	PEnset(m_hPE, PEP_bSHOWANNOTATIONS, TRUE);
// 	
// 	// Increase line annotation text size //
// 	PEnset(m_hPE, PEP_nLINEANNOTATIONTEXTSIZE, 100);
// 	
// 	// Put Grid In Front of Bars //
// 	PEnset(m_hPE, PEP_bGRIDINFRONT, TRUE);
// 	
// 	// Other various properties //
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_LIGHT_INSET);
	
}


void CGraphicView::OnDrawScatter() 
{ 
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	CreateSimpleGraph(); 
	DWORD color; 
	for (int s=0;s<=3;s++)  //4-1
	{    
		for (int p=0;p<=11;p++)
		{
			switch(s)
			{    
			case 0:
				color = RGB(15 + ((p + 1) * 20), 0, 0);  
//				PEvsetcellEx(m_hPE, PEP_dwaPOINTCOLORS, s, p, &color);
				break;
			case 1:
				color = RGB(0, 15 + ((p + 1) * 20), 0);
			//	PEvsetcellEx(m_hPE, PEP_dwaPOINTCOLORS, s, p, &color);
				break;
			case 2:
				color =  RGB(0, 0, 15 + ((p + 1) * 20));
			///*	*/	PEvsetcellEx(m_hPE, PEP_dwaPOINTCOLORS, s, p, &color);
				break;
			case 3:
				color = RGB(0, 15 + ((p + 1) * 20), 15 + ((p + 1) * 20));
			//	PEvsetcellEx(m_hPE, PEP_dwaPOINTCOLORS, s, p, &color);
				break;
			default:
				break;
			}
		}
	} 
	color = RGB(198, 0, 0);
//	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 0, &color); 
// 	color = RGB(0, 198, 0);
// 	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 1, &color); 
// 	color = RGB(0, 0, 198);
// 	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 2, &color); 
// 	color = RGB(0, 198, 198);
// 	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 3, &color); 
// 	PEnset(m_hPE, PEP_nSCROLLINGSUBSETS, 1);
// 	PEnset(m_hPE, PEP_nTABLEWHAT, PETW_ALLSUBSETS);
// 	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE); 
// 	PEnset(m_hPE, PEP_dwGRAPHBACKCOLOR, 1);
// 	PEnset(m_hPE, PEP_nGRAPHGRADIENTSTYLE, 0); 
// 	PEnset(m_hPE, PEP_dwSHADOWCOLOR, RGB(0, 0, 0)); 
// 	PEnset(m_hPE, PEP_nGRAPHPLUSTABLE, PEGPT_BOTH);
// 	PEnset(m_hPE, PEP_nDATAPRECISION, 0); 
// 	PEnset(m_hPE, PEP_nFORCEVERTICALPOINTS, PEFVP_SLANTED); 
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);
// 	PEnset(m_hPE, PEP_nBORDERTYPES, PETAB_NO_BORDER); 
}

void CGraphicView::OnDrawWhisker() 
{ 
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	//! Try this... Right Click chart and change the Plotting Method to Horizontal Bar Stacked. //
	
	// This example builds upon the basic CreateSimpleGraph '000' example chart //
	CreateSimpleGraph();
	
// 	// Enable Stacked type charts //
// 	PEnset(m_hPE, PEP_bNOSTACKEDDATA, FALSE);
// 	
// 	// Set plotting method //
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BARSTACKED);
// 	
// 	// Add a table //
// 	PEnset(m_hPE, PEP_nGRAPHPLUSTABLE, PEGPT_BOTH);
// 	PEnset(m_hPE, PEP_nDATAPRECISION, 2);
// 	
// 	PEnset(m_hPE, PEP_bALLOWHORZBARSTACKED, TRUE);
// 	
}


void CGraphicView::OnDrawStackedBar() 
{ 
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	//! Right button click to show popup menu. //
	//! Double Click to show customization dialog. //
	//! Left-Click and drag to draw zoom box. Use popup memu or 'z' to undo zoom. // 
	
	// This example builds upon the basic CreateSimpleGraph '000' example chart //
	CreateSimpleGraph();
	
// 	// Now sub-divide subsets into 4 individual axes //
// 	// Each axis with one subset in each //
// 	int nArray[4] = {1,1,1,1};
// 	PEvset (m_hPE, PEP_naMULTIAXESSUBSETS, nArray, 4);
// 	
// 	// Set first axis parameters //
// 	PEnset(m_hPE, PEP_nWORKINGAXIS, 0);
// 	PEszset(m_hPE, PEP_szYAXISLABEL, "标度1");
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);
// 	
// 	// Set second axis parameters //
// 	PEnset(m_hPE, PEP_nWORKINGAXIS, 1);
// 	PEszset(m_hPE, PEP_szYAXISLABEL, "标度2");
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);
// 	
// 	// Set third axis parameters //
// 	PEnset(m_hPE, PEP_nWORKINGAXIS, 2);
// 	PEszset(m_hPE, PEP_szYAXISLABEL, "标度3");
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);
// 	
// 	// Set fourth axis parameters //
// 	PEnset(m_hPE, PEP_nWORKINGAXIS, 3);
// 	PEszset(m_hPE, PEP_szYAXISLABEL, "标度4");
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);
// 	
// 	// Reset WorkingAxis when done //
// 	PEnset(m_hPE, PEP_nWORKINGAXIS, 0);
// 	
// 	// Add Axis Separator //
// 	PEnset(m_hPE, PEP_nMULTIAXESSEPARATORS, PEMAS_THICKPLUSTICK);
// 	
// 	// Axis Sizing //
// 	PEnset(m_hPE, PEP_nMULTIAXESSIZING, TRUE);
// 	
// 	// Get rid of 3d stuff //
// 	PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_NONE);
// 	
// 	// Other various properties //
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_DARK_NO_BORDER);
// 	PEnset(m_hPE, PEP_bMARKDATAPOINTS, TRUE);
// 	
}


void CGraphicView::OnDrawXyLine() 
{
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	RECT rect;
	GetClientRect( &rect );
	
	m_canReal=TRUE; 
	m_Real=TRUE;
// 	
// 	char szSubsets[] = "电流\t电压\t频率\t";
// 	PEvset (m_hPE, PEP_szaSUBSETLABELS, szSubsets, 3);
// 
// 	// Set Various Other Properties ///
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_DARK_INSET);
// 	
// 	m_hPE = PEcreate(PECONTROL_GRAPH, WS_VISIBLE, &rect, m_hWnd, 1001);
// 	PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
// 	PEnset(m_hPE, PEP_bCACHEBMP, TRUE); 
// 	PEnset(m_hPE, PEP_nSUBSETS, 2);		// set number of subsets
// 	PEnset(m_hPE, PEP_nPOINTS, 200);	// number of data points	
// 	PEnset(m_hPE, PEP_nPOINTSTOGRAPH, 20);
// 	PEnset(m_hPE, PEP_nPOINTSTOGRAPHINIT, PEPTGI_LASTPOINTS);	   		
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_POINTSPLUSLINE);	  
// 	PEszset(m_hPE, PEP_szMAINTITLE, "实时曲线示例");
// 	PEszset(m_hPE, PEP_szSUBTITLE, "编程设计实现-戚高"); // no subtitle
// 	PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
// 	double manminY = 1.0F;
// 	PEvset(m_hPE, PEP_fMANUALMINY, &manminY, 1);
// 	double manmaxY = 100.0F;
// 	PEvset(m_hPE, PEP_fMANUALMAXY, &manmaxY, 1);
// 	PEszset(m_hPE, PEP_szMANUALMAXPOINTLABEL, "000.000" );
// 	PEszset(m_hPE, PEP_szMANUALMAXDATASTRING, "00:00:00xx");
// 	PEnset(m_hPE, PEP_bNOSTACKEDDATA, TRUE); 
// 	PEnset(m_hPE, PEP_bNORANDOMPOINTSTOGRAPH, TRUE);
// 	PEnset(m_hPE, PEP_bALLOWHISTOGRAM, FALSE);	
// 	PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
// 	PEnset(m_hPE, PEP_nGRIDLINECONTROL, FALSE);
// 	PEnset(m_hPE, PEP_nDATAPRECISION, 1);									  
// 	PEszset(m_hPE, PEP_szaPOINTLABELS, "");
// 	float f1 = 0.0F;
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 0, &f1);
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 1, &f1);
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 2, &f1);
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 3, &f1);
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, FALSE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_LIGHT_SHADOW);
// 	PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
// 	PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
// 	PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
//	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE); 
}			


void CGraphicView::OnTimer(UINT nIDEvent) 
{ 
	CString ttext;  
	double newx2;
	float YData[250];
	double XData[250]; 
	double dx; 
	CTime t;
/////	TM petm; 
	if(m_canReal)
	{
		if(nIDEvent==2)
		{
			if(m_Real)
			{
				float newy = 50.0F + float(sin((double) m_nSinCounter * 0.075F) * 30.0F) + GetRandom(1, 15);
				float newy1,newy2;
				newy1 = 50.0F - float(sin((double) m_nSinCounter * 0.075F) * 30.0F) + GetRandom(1, 15);
				newy2 = 50.0F + float(sin((double) m_nSinCounter * 0.075F) * 30.0F) - GetRandom(1, 15);
				// Determine newx2 as current date and time in VB format //
				t = CTime::GetCurrentTime();
// /				petm.nDay = t.GetDay();
// 				petm.nMonth = t.GetMonth();
// 				petm.nYear = t.GetYear() - 1900;
// 				petm.nHour = t.GetHour();
// 				petm.nMinute = t.GetMinute();
// 				petm.nSecond = t.GetSecond();
// 				PEcreateserialdate(&newx2, &petm, PEDTM_VB);
				
				// Shift data when counter reaches end of data //
				if(m_nRealTimeCounter >= 1999)
				{
					memset(YData, 0, 250 * sizeof(float));
					memset(XData, 0, 250 * sizeof(double));
					// Shift Data by 250 data points, oldest 250 will be lost.
					m_nRealTimeCounter = m_nRealTimeCounter - 250;
// 					PEvset(m_hPE, PEP_faAPPENDYDATA, YData, 250);
// 					PEvset(m_hPE, PEP_faAPPENDXDATAII, XData, 250);
				}
				
				// Store new values at current index //
// 				PEvsetcellEx(m_hPE, PEP_faXDATAII, 0, m_nRealTimeCounter, &newx2);
// 				PEvsetcellEx(m_hPE, PEP_faYDATA, 0, m_nRealTimeCounter, &newy);
// 				
// 				// Store new values at current index //
// 				PEvsetcellEx(m_hPE, PEP_faXDATAII, 1, m_nRealTimeCounter, &newx2);
// 				PEvsetcellEx(m_hPE, PEP_faYDATA, 1, m_nRealTimeCounter, &newy1);
// 				
// 				// Store new values at current index //
// 				PEvsetcellEx(m_hPE, PEP_faXDATAII, 2, m_nRealTimeCounter, &newx2);
// 				PEvsetcellEx(m_hPE, PEP_faYDATA, 2, m_nRealTimeCounter, &newy2);
				
				// Increment index counter //
				m_nRealTimeCounter += 1;
				
				// SinCounter is only to produce sin wave data //
				m_nSinCounter += 1;
				if (m_nSinCounter > 30000) 
					m_nSinCounter = 1;
				
				// If current time is beyond zoom max, the re-adjust zoom max //
				if (newx2 > ZoomEnd)
				{
					ZoomEnd = ZoomEnd + ZoomSmallInterval;
					ZoomStart = ZoomEnd - ZoomInterval;
// 					PEvset(m_hPE, PEP_fZOOMMINX, &ZoomStart, 1);
// 					PEvset(m_hPE, PEP_fZOOMMAXX, &ZoomEnd, 1);
// 					
// 					PEvgetcellEx(m_hPE, PEP_faXDATAII, 0, 1, &dx);
// 					PEvset(m_hPE, PEP_fMANUALMINX, &dx, 1);
// 					PEvset(m_hPE, PEP_fMANUALMAXX, &ZoomEnd, 1);
				}
				
// 				// Update image and force paint //
// 				PEreinitialize( m_hPE );
// 				PEresetimage( m_hPE, 0, 0 );
				::InvalidateRect(m_hPE, NULL, FALSE);
			}
		}
		else
		{
			if(m_Real)
			{
				/*PEvset(m_hPE, PEP_szaAPPENDPOINTLABELDATA, NULL, 1);
				
				// make some random data //
				float r1,r2,fNew[2];
				r1 = (float) GetRandom(1, 20);
				r2 = (float) GetRandom(1, 100);
				
				fNew[0] = r1 + (r2 * .0001F);
				fNew[1] = fNew[0] * 4;
				PEvset(m_hPE, PEP_faAPPENDYDATA, &fNew[0], 1);*/
				///////////////////////////////////  
				float newy = 50.0F + float(sin((double) m_nSinCounter * 0.075F) * 30.0F) + GetRandom(1, 15);
				float newx = (float)m_nRealTimeCounter; 
				
// 				// Append new values  //
// 				PEvset(m_hPE, PEP_faAPPENDYDATA, &newy, 1);
// 				PEvset(m_hPE, PEP_faAPPENDXDATA, &newx, 1); 
// 				m_nRealTimeCounter = m_nRealTimeCounter + 1;
// 				
// 				// Switch to AutoScaling x axis after receiving 100 data points //
// 				if (m_nRealTimeCounter == 100)
// 					PEnset(m_hPE, PEP_nMANUALSCALECONTROLX, PEMSC_NONE);
// 				
				// SinCounter is only to produce sin wave data //
				m_nSinCounter = m_nSinCounter + 1;
				if (m_nSinCounter > 30000) 
					m_nSinCounter = 1;
				
				// Update image and force paint //
//				PEreinitialize( m_hPE );
	//			PEresetimage( m_hPE, 0, 0 );
				::InvalidateRect(m_hPE, NULL, FALSE);
			}
			else
			{
				////**********************************************
				float newy,newx;
				newy = 50.0F + float(sin((double) m_nSinCounter * 0.075F) * 30.0F) + GetRandom(1, 15);
				newx = float(m_nRealTimeCounter);
// 				
// 				// Update new data at current index //
// 				PEvsetcellEx(m_hPE, PEP_faYDATA, 0, m_nRealTimeCounter, &newy);
// 				PEvsetcellEx(m_hPE, PEP_faXDATA, 0, m_nRealTimeCounter, &newx);
// 				newy = 50.0F - float(sin((double) m_nSinCounter * 0.075F) * 30.0F) + GetRandom(1, 15);
// 				newx = float(m_nRealTimeCounter);
// 				PEvsetcellEx(m_hPE, PEP_faYDATA, 1, m_nRealTimeCounter, &newy);
// 				PEvsetcellEx(m_hPE, PEP_faXDATA, 1, m_nRealTimeCounter, &newx);
// 				newy = 50.0F + float(sin((double) m_nSinCounter * 0.075F) * 12.0F) - GetRandom(1, 15);
// 				newx = float(m_nRealTimeCounter);
// 				PEvsetcellEx(m_hPE, PEP_faYDATA, 2, m_nRealTimeCounter, &newy);
// 				PEvsetcellEx(m_hPE, PEP_faXDATA, 2, m_nRealTimeCounter, &newx);
				
// 				double dx;
// 				dx = (double) newx;
// 				PEvsetcell(m_hPE, PEP_faVERTLINEANNOTATION, 0, &dx);
// 				PEvsetcell(m_hPE, PEP_faVERTLINEANNOTATION, 2, &dx);
// 				PEvsetcell(m_hPE, PEP_faVERTLINEANNOTATION, 3, &dx);
// 				int lt = PELT_MEDIUMSOLID;
// 				PEvsetcell(m_hPE, PEP_naVERTLINEANNOTATIONTYPE, 0, &lt);
// 				COLORREF col;
// 				col = RGB(0,0,198);
// 				PEvsetcell(m_hPE, PEP_dwaVERTLINEANNOTATIONCOLOR, 0, &col);
// 				col = RGB(0,198,0);
// 				PEvsetcell(m_hPE, PEP_dwaVERTLINEANNOTATIONCOLOR, 1, &col);
// 				col = RGB(255,198,0);
// 				PEvsetcell(m_hPE, PEP_dwaVERTLINEANNOTATIONCOLOR, 2, &col);
// 				// Increment counter //
				m_nRealTimeCounter = m_nRealTimeCounter + 1;
				
				// Reset counter at end of data //
				if (m_nRealTimeCounter == 100)
					m_nRealTimeCounter = 0;
				
				// SinCounter is only to produce sin wave data //
				m_nSinCounter = m_nSinCounter + 1;
				if (m_nSinCounter > 30000) 
					m_nSinCounter = 1;
				
// 				// Update image and force paint //
// 				PEreinitialize( m_hPE );
// 				//	PEresetimage( m_hPE, 0, 0 );
// 				//	PEresetimage( m_hPE, 1, 0 );
// 				PEresetimage( m_hPE, 2, 0 );
				::InvalidateRect(m_hPE, NULL, FALSE);
			}
		}
	}
	CView::OnTimer(nIDEvent);
}

void CGraphicView::OnDraw3dBar() 
{ 
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	//! This shows how to place extra text inside table and within an axis. //
	//! Maximize to allow room for textual data which is drawn as fixed font size. //
	// This example builds upon the basic CreateSimpleGraph '000' example chart //
	CreateSimpleGraph();
	
	// Create two y axes //
	int nMAS[] = {2, 2};
// 	PEvset(m_hPE, PEP_naMULTIAXESSUBSETS, nMAS, 2);
// 	
// 	// Construct a simple table annotation //
// 	PEnset(m_hPE, PEP_nWORKINGTABLE, 0);
// 	PEnset(m_hPE, PEP_nTAROWS, 6);
// 	PEnset(m_hPE, PEP_nTACOLUMNS, 12);  // 12 is same number as PEP_nPOINTS
	
	// Pass the table text //
	char szTmp[48];
	float fp;
	int r, c;
	DWORD dwColor;
	BOOL b = TRUE;
	
	for (r=0; r<6; r++)
	{
		if (b)
			dwColor = RGB(175,175,175);
		else
			dwColor = RGB(255,255,255);
		b = !b;
		
		for (c=0; c<12; c++)
		{
			fp = float(sin(0.03F * (double) c + 1.0F + r) * 500.0F) + (float) GetRandom(2, 50);
			sprintf(szTmp, "%.3f ", fp);
// 			PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c, szTmp);
// 			PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, r, c, &dwColor);
		}
	}
	
// 	// Set Table Location //
// 	PEnset(m_hPE, PEP_nTALOCATION, PETAL_INSIDE_AXIS_1);
// 	PEnset(m_hPE, PEP_nTAAXISLOCATION, PETAAL_BOTTOM_TABLE_SPACED);
// 	
// 	// Other Table Related Properties ///
// 	PEnset(m_hPE, PEP_bSHOWTABLEANNOTATION, TRUE);
// 	PEnset(m_hPE, PEP_nTABORDER, PETAB_NO_BORDER);
// 	PEnset(m_hPE, PEP_dwTABACKCOLOR, 1);
// 	PEnset(m_hPE, PEP_dwTAFORECOLOR, 1);
// 	
// 	// Construct a second simple table annotation //
// 	PEnset(m_hPE, PEP_nWORKINGTABLE, 1); 
// 	PEnset(m_hPE, PEP_nTAROWS, 6);
// 	PEnset(m_hPE, PEP_nTACOLUMNS, 12);
	
	// Pass the table text //
	b = TRUE;
	for (r=0; r<6; r++)
	{
		if (b)
			dwColor = RGB(175,175,175);
		else
			dwColor = RGB(255,255,255);
		b = !b;
		
		for (c=0; c<12; c++)
		{
			fp = float(sin(0.03F * (double) c + 1.0F + (2 * r)) * 500.0F) + (float) GetRandom(2, 50);
			sprintf(szTmp, "%.3f ", fp);
// 			PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c, szTmp);
// 			PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, r, c, &dwColor);
		}
	}
	
// 	// Set Table Location //
// 	PEnset(m_hPE, PEP_nTALOCATION, PETAL_INSIDE_AXIS);
// 	PEnset(m_hPE, PEP_nTAAXISLOCATION, PETAAL_BOTTOM_TABLE_SPACED);
// 	
// 	// Other Table Related Properties ///
// 	PEnset(m_hPE, PEP_bSHOWTABLEANNOTATION, TRUE);
// 	PEnset(m_hPE, PEP_nTABORDER, PETAB_NO_BORDER);
// 	PEnset(m_hPE, PEP_dwTABACKCOLOR, 1);
// 	PEnset(m_hPE, PEP_dwTAFORECOLOR, 1);
// 	
// 	// Construct a third simple table annotation //
// 	PEnset(m_hPE, PEP_nWORKINGTABLE, 2); 
// 	PEnset(m_hPE, PEP_nTAROWS, 2);
// 	PEnset(m_hPE, PEP_nTACOLUMNS, 13);
// 	
	// Pass the table text //
	for (r=0; r<2; r++)
	{
		for (c=1; c<13; c++)
		{
			fp = float(sin(0.03F * (double) c + 1.0F) * 500.0F) + (float) GetRandom(2, 50);
			sprintf(szTmp, "%.3f ", fp);
//			PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c, szTmp);
		}
	}
	
	// Set Table Location //
// 	PEnset(m_hPE, PEP_nTALOCATION, PETAL_INSIDE_TABLE);
// 	PEnset(m_hPE, PEP_nTAAXISLOCATION, PETAAL_TOP_TABLE_SPACED);
// 	
// 	// Other Table Related Properties ///
// 	PEnset(m_hPE, PEP_bSHOWTABLEANNOTATION, TRUE);
// 	PEnset(m_hPE, PEP_dwTABACKCOLOR, RGB(0,198,198));
// 	PEnset(m_hPE, PEP_bTAHEADERCOLUMN, TRUE);
// 	
// 	// Set Text for Header Column //
// 	strcpy(szTmp, "Table Annotation Line 1");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 0, szTmp);
// 	strcpy(szTmp, "Table Annotation Line 2");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 1, 0, szTmp);
// 	
	// Done setting Table Annotations ///
	
// 	// Set Various Other Properties ///
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_DARK_SHADOW);
// 	PEnset(m_hPE, PEP_nDATAPRECISION, 0);
// 	PEnset(m_hPE, PEP_nGRAPHPLUSTABLE, PEGPT_BOTH);
// 	PEnset(m_hPE, PEP_nLEGENDSTYLE, PELS_1_LINE_INSIDE_AXIS);
// 	PEnset(m_hPE, PEP_nGRIDLINECONTROL, PEGLC_YAXIS);
// 	PEnset(m_hPE, PEP_bMARKDATAPOINTS, TRUE);
// 	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_SMALL);
// 	PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_SHADOWS);
// 	PEnset(m_hPE, PEP_bSHOWLEGEND, FALSE);
// 	
// 	PEnset(m_hPE, PEP_nWORKINGAXIS, 0);
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);
// 	PEnset(m_hPE, PEP_nRYAXISCOMPARISONSUBSETS, 1);
// 	PEnset(m_hPE, PEP_dwRYAXISCOLOR, RGB(0, 198,198));
// 	PEszset(m_hPE, PEP_szYAXISLABEL, "");
// 	PEszset(m_hPE, PEP_szRYAXISLABEL, "");
// 	
// 	PEnset(m_hPE, PEP_nWORKINGAXIS, 1);
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);
// 	PEnset(m_hPE, PEP_nRYAXISCOMPARISONSUBSETS, 1);
// 	PEnset(m_hPE, PEP_dwRYAXISCOLOR, RGB(0, 198,0));
// 	PEszset(m_hPE, PEP_szYAXISLABEL, "");
// 	PEszset(m_hPE, PEP_szRYAXISLABEL, "");
// 	
// 	DWORD dwArray[4] = { RGB(138,0,0), RGB(0, 198, 198), RGB(138,138,0), RGB(0,198,0) };
// 	PEvsetEx(m_hPE, PEP_dwaSUBSETCOLORS, 0, 4, dwArray, 0);
// 	
// 	PEnset(m_hPE, PEP_nSHOWTABLEANNOTATIONSMENU, PEMC_SHOW);
	
}



void CGraphicView::OnDraw3dStackedBar() 
{
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	//! Chart fills 100 points and once 100 points have
	//! been passed, old data is over-written as more
	//! data is passed to chart.
	m_canReal=TRUE;
	m_Real=FALSE;
	RECT rect;
	GetClientRect( &rect ); 
	
	// Construct Object //
////	m_hPE = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rect, m_hWnd, 1001);
// 	
// 	PEnset(m_hPE, PEP_nSUBSETS, 3);
// 	PEnset(m_hPE, PEP_nPOINTS, 100);
// 	
// 	// Set Manual Y scale //
// 	PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
// 	double arg = 1.0F;//
// 	PEvset(m_hPE, PEP_fMANUALMINY, &arg, 1);
// 	arg = 100.0F;
// 	PEvset(m_hPE, PEP_fMANUALMAXY, &arg, 1);
// 	
// 	// Set Manual X scale //
// 	PEnset(m_hPE, PEP_nMANUALSCALECONTROLX, PEMSC_MINMAX);
// 	arg = 1.0F;
// 	PEvset(m_hPE, PEP_fMANUALMINX, &arg, 1);
// 	arg = 100.0F;
// 	PEvset(m_hPE, PEP_fMANUALMAXX, &arg, 1);
// 	
// 	// Show Annotations //
// 	PEnset(m_hPE, PEP_bSHOWANNOTATIONS, TRUE);
// 	
// 	// Clear out default data //
// 	float val = 0;
// 	PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 0, &val);
// 	PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 1, &val);
// 	PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 2, &val);
// 	PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 3, &val); 
// 	
// 	PEvsetcellEx(m_hPE, PEP_faXDATA, 1, 0, &val);
// 	PEvsetcellEx(m_hPE, PEP_faXDATA, 1, 1, &val);
// 	PEvsetcellEx(m_hPE, PEP_faXDATA, 1, 2, &val);
// 	PEvsetcellEx(m_hPE, PEP_faXDATA, 1, 3, &val);
// 	
// 	PEvsetcellEx(m_hPE, PEP_faXDATA, 2, 0, &val);
// 	PEvsetcellEx(m_hPE, PEP_faXDATA, 2, 1, &val);
// 	PEvsetcellEx(m_hPE, PEP_faXDATA, 2, 2, &val);
// 	PEvsetcellEx(m_hPE, PEP_faXDATA, 2, 3, &val);
// 	
// 	int nTmpStyle;
// 	nTmpStyle = PELT_THINSOLID;
// 	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 0, &nTmpStyle); 
// 	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 1, &nTmpStyle); 
// 	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 2, &nTmpStyle);
// 	//	PEvset(m_hPE, PEP_naSUBSETLINETYPES, PELT_THINSOLID, 0);
// 	
// 	char szSubsets[] = "电流\t电压\t频率\t";
// 	PEvset (m_hPE, PEP_szaSUBSETLABELS, szSubsets, 3);
// 	
// 	
// 	
// 	// Set Various Other Properties ///
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_DARK_LINE);
// 	
// 	PEszset(m_hPE, PEP_szMAINTITLE, "工业控制实时曲线示例");
// 	PEszset(m_hPE, PEP_szSUBTITLE, "编程设计实现--戚高");
// 	PEnset(m_hPE, PEP_bNORANDOMPOINTSTOEXPORT, TRUE);
// 	PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
// 	PEnset(m_hPE, PEP_bALLOWBAR, FALSE);
// 	PEnset(m_hPE, PEP_bALLOWPOPUP, FALSE);
// 	PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
// 	PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
// 	PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
// 	
// 	DWORD col = RGB(0, 198, 198);
// 	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 0, &col); 
// 	
// 	col = RGB(0,198,0);
// 	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 1, &col);
// 	
// 	col = RGB(255,0,0);
// 	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 2, &col);
// 	
// 	PEnset(m_hPE, PEP_nGRADIENTBARS, 8);
// 	PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
// 	PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bLINESHADOWS, TRUE);
// 	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE);
// 	
// 	PEreinitialize(m_hPE);
// 	PEresetimage(m_hPE, 2, 0);
// 	::InvalidateRect(m_hPE, NULL, FALSE);
// 	
// 	//Initialize Counters and Timer 
// 	m_nRealTimeCounter = 1;
// 	m_nSinCounter = 1;
	SetTimer( 1, 100, NULL ); 
}


void CGraphicView::OnDraw3dLine() 
{   
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	m_canReal=FALSE; 
	RECT rect;
	GetClientRect( &rect );
	
	// Construct Object //
// 	m_hPE = PEcreate(PECONTROL_PGRAPH, WS_VISIBLE, &rect, m_hWnd, 1001);
// 	
// 	PEnset(m_hPE, PEP_nSUBSETS, 2);
// 	PEnset(m_hPE, PEP_nPOINTS, 360);
// 	
	int p;
	float f1, f2;
	for (p=0; p<360; p++)
	{
		f1 = (float) p;
		f2 = (float) 150 * (float)sin((double) p * 0.054F);
//		PEvsetcellEx(m_hPE, PEP_faXDATA, 0, p, &f1);
//		PEvsetcellEx(m_hPE, PEP_faYDATA, 0, p, &f2);
	}
	
	for (p=0; p<360; p++)
	{
		f1 = (float) p;
// 		f2 = (float) 150 * (float)sin(((double) p * 0.044F)) * 2;
// 		PEvsetcellEx(m_hPE, PEP_faXDATA, 1, p, &f1);
// 		PEvsetcellEx(m_hPE, PEP_faYDATA, 1, p, &f2);
	}
	
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
// 	PEnset(m_hPE, PEP_nALLOWZOOMING, 1);
// 	PEszset(m_hPE, PEP_szMAINTITLE, "Polar Chart");
// 	PEszset(m_hPE, PEP_szSUBTITLE, ""); // no subtitle
// 	float ftmp = .9F;
// 	PEvset(m_hPE, PEP_fFONTSIZELEGENDCNTL, &ftmp, 1 );
// 	PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
// 	PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
// 	PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
// 	
// 	// Set line types
// 	int nTypes[] = { PELT_MEDIUMSOLID, PELT_MEDIUMSOLID };
// 	PEvset(m_hPE, PEP_naSUBSETLINETYPES, nTypes, 2);
// 	
// 	// Set point types
// 	int nPTypes[] = { PEPT_DOTSOLID, PEPT_PLUS };
// 	PEvset(m_hPE, PEP_naSUBSETPOINTTYPES, nTypes, 2);
// 	
// 	// subset colors
// 	DWORD dwArray[2] = { RGB(198,0,0), RGB(0, 198, 0) };
// 	PEvset( m_hPE, PEP_dwaSUBSETCOLORS, dwArray, 2);
// 	
// 	// subset labels
// 	PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 0, "Signal #1" );
// 	PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 1, "Signal #2" );
// 	
// 	double dnull = -99999.0F;
// 	PEvset(m_hPE, PEP_fNULLDATAVALUEX, &dnull, 1);
// 	PEvset(m_hPE, PEP_fNULLDATAVALUE, &dnull, 1);
// 	
// 	// Set Various other features //
// 	PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_NO_BORDER);
// 	PEnset(m_hPE, PEP_bSIMPLEPOINTLEGEND, TRUE);
// 	PEnset(m_hPE, PEP_bSIMPLELINELEGEND, TRUE);
// 	PEnset(m_hPE, PEP_nLEGENDSTYLE, PELS_1_LINE);
// 	PEnset(m_hPE, PEP_nZOOMSTYLE, PEZS_RO2_NOT);
// 	PEnset(m_hPE, PEP_nPOINTSIZE, PEPS_SMALL);
// 	
// 	PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
// 	PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bLINESHADOWS, TRUE);
// 	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE);
// 	PEnset(m_hPE, PEP_nGRADIENTBARS, 8);
// 	PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_SHADOWS);
// 	float f = 1.2F;
// 	PEvset(m_hPE, PEP_fFONTSIZEGNCNTL, &f, 1); 
// 	// Enable Smith Mode //
// 	PEnset(m_hPE, PEP_nSMITHCHART, PESC_SMITH);
// 	PEszset(m_hPE, PEP_szMAINTITLE, "Smith Chart");
// 	
// 	// Change the data to something more appropriate //
// 	PEnset(m_hPE, PEP_nSUBSETS, 2);
// 	PEnset(m_hPE, PEP_nPOINTS, 40);
// 	
// 	// Set first subset's data // 
// 	int s = 0;
// 	for (p=0; p<=39; p++)
// 	{
// 		f = (float) p * 0.1F;
// 		PEvsetcellEx(m_hPE, PEP_faXDATA, s, p, &f);
// 		f = (float) sin((double) p * 0.54F) * 0.2F;
// 		PEvsetcellEx(m_hPE, PEP_faYDATA, s, p, &f);
// 	}
// 	
// 	// Set second subset's data //
// 	s = 1;
// 	for (p=0; p<=39; p++)
// 	{
// 		f = (float) p * 0.5F;
// 		PEvsetcellEx(m_hPE, PEP_faXDATA, s, p, &f);
// 		f = (float) sin((double) p * 0.44F) * 0.5F;
// 		PEvsetcellEx(m_hPE, PEP_faYDATA, s, p, &f);
// 	}
// 	
// 	// Set Various other features //
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_DARK_SHADOW);
// 	
// 	// Add Some Padding on Right and Bottom //
// 	PEnset(m_hPE, PEP_nIMAGEADJUSTLEFT, 400);
// 	PEnset(m_hPE, PEP_nIMAGEADJUSTRIGHT, 400);
// 	PEnset(m_hPE, PEP_nIMAGEADJUSTBOTTOM, 400);
}




void CGraphicView::OnDraw3dPie() 
{  
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	RECT rect;
	GetClientRect( &rect ); 
	m_canReal=FALSE;
// 	m_hPE = PEcreate(PECONTROL_PIE, WS_VISIBLE, &rect, m_hWnd, 1001);
// 	PEnset(m_hPE, PEP_nSUBSETS, 5);
// 	PEnset(m_hPE, PEP_nPOINTS, 12);
// 	int s,p;          
// 	for (s=0; s<5; s++)
// 	{
// 		for (p=0; p<12; p++)
// 		{
// 			float f1 = (float) GetRandom(1, 5) + ((float) GetRandom(1, 150) / 100.0F);
// 			PEvsetcellEx(m_hPE, PEP_faXDATA, s, p, &f1);
// 		}
// 	}
// 	float f1= 0.0F;
// 	PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 3, &f1); 
// 	PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 0, "北京" );
// 	PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 1, "上海" );
// 	PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 2, "广州"  );
// 	PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 3, "长沙" );
// 	PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 4, "重庆" ); 
// 	PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 0, "一月"      );
// 	PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 1, "二月"   );
// 	PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 2, "三月");
// 	PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 3, "四月" );
// 	PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 4, "五月"   );
// 	PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 5, "六月"    );
// 	PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 6, "七月"       );
// 	PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 7, "八月"   );
// 	PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 8, "九月"    );
// 	PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 9, "十月"     );
// 	PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 10, "十一月"   );
// 	PEvsetcell( m_hPE, PEP_szaPOINTLABELS, 11, "十二月"    ); 
// 	DWORD c[12];
// 	c[0] = RGB(198, 0, 0);
// 	c[1] = RGB(0, 198, 0);
// 	c[2] = RGB(198, 198, 0);
// 	c[3] = RGB(0, 0, 198);
// 	c[4] = RGB(198, 0, 198);
// 	c[5] = RGB(0, 198, 198);
// 	c[6] = RGB(192, 192, 192);
// 	c[7] = RGB(148, 0, 0);
// 	c[8] = RGB(0, 148, 0);
// 	c[9] = RGB(148, 148, 0);
// 	c[10] = RGB(0, 148, 148);
// 	c[11] = RGB(122, 122, 122);
// 	PEvset(m_hPE, PEP_dwaSUBSETCOLORS, c, 12); 
// 	PEszset(m_hPE, PEP_szMAINTITLE, "3D PIE图表显示编程");
// 	PEszset(m_hPE, PEP_szSUBTITLE, "编程设计实现-戚高"); // no subtitle 
// 	PEnset(m_hPE, PEP_nDATAPRECISION, 1);				   
// 	PEnset(m_hPE, PEP_nGROUPINGPERCENT, 4);
// 	PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_3D);					 
// 	PEnset(m_hPE, PEP_bDISABLE3DSHADOW, FALSE);
// 	PEnset(m_hPE, PEP_nAUTOEXPLODE, PEAE_ALLSUBSETS);
// 	PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
// 	PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
// 	PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
// 	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE);
// 	PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE); 
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_NO_BORDER); 
// 	PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
// 	PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
// 	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE); 
}


void CGraphicView::OnBkLine() 
{ 
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	OnDraw3dPie(); 
	float f1 = 1; 
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 3, &f1);
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 5, &f1); 
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, FALSE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_SHADOW); 
// 	PEnset(m_hPE, PEP_nIMAGEADJUSTLEFT, 400);
// 	PEnset(m_hPE, PEP_nIMAGEADJUSTRIGHT, 400);
// 	PEnset(m_hPE, PEP_nIMAGEADJUSTBOTTOM, 400);
// 	//***********************************************
// 	OnDraw3dPie();
// 	f1 = 1;
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 3, &f1);
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 5, &f1); 
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, FALSE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_SHADOW); 
// 	PEnset(m_hPE, PEP_nIMAGEADJUSTLEFT, 400);
// 	PEnset(m_hPE, PEP_nIMAGEADJUSTRIGHT, 400);
// 	PEnset(m_hPE, PEP_nIMAGEADJUSTBOTTOM, 400); 
}


void CGraphicView::OnBkXy() 
{ 
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	RECT rect;
	GetClientRect( &rect );
// 	
// 	// Construct Object //
// 	m_hPE = PEcreate(PECONTROL_PGRAPH, WS_VISIBLE, &rect, m_hWnd, 100);
// 	
// 	// Set to rose mode //	
// 	PEnset(m_hPE, PEP_nSMITHCHART, PESC_ROSE);  // Select Rose Plot Mode
// 	
// 	// Set titles //
// 	PEszset(m_hPE, PEP_szMAINTITLE, "Wind Rose Example");
// 	PEszset(m_hPE, PEP_szSUBTITLE, " ");
// 	
// 	// Set data //
// 	PEnset(m_hPE, PEP_nSUBSETS, 13);
// 	PEnset(m_hPE, PEP_nPOINTS, 16);
// 	
// 	float tempxdata[] =
// 	{ 
// 		90.0F, 	67.5F,	45.0F,	22.5F,	0.0F,	337.5F,	315.0F,	292.5F,	270.0F,	247.5F,	225.0F,	202.5F,	180.0F,	157.5F,	135.0F,	112.5F,
// 			90.0F, 	67.5F,	45.0F,	22.5F,	0.0F,	337.5F,	315.0F,	292.5F,	270.0F,	247.5F,	225.0F,	202.5F,	180.0F,	157.5F,	135.0F,	112.5F,
// 			90.0F, 	67.5F,	45.0F,	22.5F,	0.0F,	337.5F,	315.0F,	292.5F,	270.0F,	247.5F,	225.0F,	202.5F,	180.0F,	157.5F,	135.0F,	112.5F,
// 			90.0F, 	67.5F,	45.0F,	22.5F,	0.0F,	337.5F,	315.0F,	292.5F,	270.0F,	247.5F,	225.0F,	202.5F,	180.0F,	157.5F,	135.0F,	112.5F,
// 			90.0F, 	67.5F,	45.0F,	22.5F,	0.0F,	337.5F,	315.0F,	292.5F,	270.0F,	247.5F,	225.0F,	202.5F,	180.0F,	157.5F,	135.0F,	112.5F,
// 			90.0F, 	67.5F,	45.0F,	22.5F,	0.0F,	337.5F,	315.0F,	292.5F,	270.0F,	247.5F,	225.0F,	202.5F,	180.0F,	157.5F,	135.0F,	112.5F,
// 			90.0F, 	67.5F,	45.0F,	22.5F,	0.0F,	337.5F,	315.0F,	292.5F,	270.0F,	247.5F,	225.0F,	202.5F,	180.0F,	157.5F,	135.0F,	112.5F,
// 			90.0F, 	67.5F,	45.0F,	22.5F,	0.0F,	337.5F,	315.0F,	292.5F,	270.0F,	247.5F,	225.0F,	202.5F,	180.0F,	157.5F,	135.0F,	112.5F,
// 			90.0F, 	67.5F,	45.0F,	22.5F,	0.0F,	337.5F,	315.0F,	292.5F,	270.0F,	247.5F,	225.0F,	202.5F,	180.0F,	157.5F,	135.0F,	112.5F,
// 			90.0F, 	67.5F,	45.0F,	22.5F,	0.0F,	337.5F,	315.0F,	292.5F,	270.0F,	247.5F,	225.0F,	202.5F,	180.0F,	157.5F,	135.0F,	112.5F,
// 			90.0F, 	67.5F,	45.0F,	22.5F,	0.0F,	337.5F,	315.0F,	292.5F,	270.0F,	247.5F,	225.0F,	202.5F,	180.0F,	157.5F,	135.0F,	112.5F,
// 			90.0F, 	67.5F,	45.0F,	22.5F,	0.0F,	337.5F,	315.0F,	292.5F,	270.0F,	247.5F,	225.0F,	202.5F,	180.0F,	157.5F,	135.0F,	112.5F,
// 			90.0F, 	67.5F,	45.0F,	22.5F,	0.0F,	337.5F,	315.0F,	292.5F,	270.0F,	247.5F,	225.0F,	202.5F,	180.0F,	157.5F,	135.0F,	112.5F	};
// 		PEvset(m_hPE, PEP_faXDATA, tempxdata, 208);
// 		
// 		float tempydata[] = 
// 		{	.77F, .75F, .78F, .88F, 1.07F, .98F, .69F, .51F, .38F, .50F, .56F, .65F, .72F, .70F, .62F, .63F,
// 		1.04F, 1.72F, 2.68F, 4.76F, 4.58F, 2.59F, 1.29F, .99F, 1.19F, 1.77F, 2.34F, 2.89F, 2.05F, 1.31F, 1.16F, 1.08F,
// 		.43F, 1.05F, 3.41F, 7.40F, 5.97F, 1.48F, .30F, .39F, .73F, 1.68F, 3.53F, 2.73F, 1.24F, .55F, .25F, .27F,
// 		.20F, 1.07F, 3.71F, 4.49F, 2.08F, .23F, .10F, .11F, .29F, 1.16F, 2.32F, 1.26F, .54F, .18F, .09F, .22F,
// 		.05F, .48F, .66F, 1.31F, .26F, .01F, .02F, .04F, .14F, .33F, .75F, .46F, .18F, .05F, .07F, .03F,
// 		.04F, .10F, .07F, .02F, .02F, .02F, .01F, .01F, .01F, .05F, .12F, .09F, .04F, .02F, .03F, .04F,
// 		.01F, 0.0F, .01F, .01F,	0.0F, .01F,	.01F, .01F,	.01F, .01F,	.01F, .03F,	0.0F, 0.0F,	.02F, .01F,
// 		.01F, 0.0F, .01F, 0.0F,	.01F, 0.0F,	.01F, 0.0F,	0.0F, .01F,	0.0F, .01F,	.02F, .01F,	.01F, 0.0F,
// 		.01F, 0.0F, 0.0F, 0.0F,	0.0F, 0.0F,	0.0F, .01F,	0.0F, 0.0F,	0.0F, 0.1F,	0.0F, 0.0F,	0.0F, 0.0F,
// 		0.0F, 0.0F, .01F, 0.0F,	0.0F, 0.0F,	0.0F, .01F,	0.0F, 0.0F,	0.0F, 0.0F,	0.0F, 0.0F,	0.0F, 0.0F,
// 		0.0F, 0.0F, .01F, 0.0F,	0.0F, 0.0F,	0.0F, .01F,	0.0F, 0.0F,	0.0F, 0.0F,	0.0F, 0.0F,	0.0F, 0.0F,
// 		0.0F, 0.0F, .01F, 0.0F,	0.0F, 0.0F,	0.0F, 0.0F,	0.0F, 0.0F,	0.0F, 0.0F,	0.0F, 0.0F,	0.0F, 0.0F,
// 		0.0F, 0.0F, .01F, 0.0F,	0.0F, 0.0F,	0.0F, 0.0F,	0.0F, 0.0F,	0.0F,0.0F,	0.0F, 0.0F,	0.0F, 0.0F	};
// 		PEvset(m_hPE, PEP_faYDATA, tempydata, 208);
// 		
// 		char tempptlabels[] = "E\tENE\tNE\tNNE\tN\tNNW\tNW\tWNW\tW\tWSW\tSW\tSSW\tS\tSSE\tSE\tESE\t";
// 		char tempsslabels[] = "3.75\t6.25\t8.75\t11.25\t13.75\t16.25\t18.75\t21.25\t23.75\t26.25\t28.75\t31.25\t33.75\t";
// 		PEvset(m_hPE, PEP_szaPOINTLABELS, tempptlabels, 16);
// 		PEvset(m_hPE, PEP_szaSUBSETLABELS, tempsslabels, 13);
// 		
// 		// Manually control degree line frequency //
// 		PEnset(m_hPE, PEP_bMANUALXAXISTICKNLINE, TRUE);
// 		double d = 22.5F;
// 		PEvset(m_hPE, PEP_fMANUALXAXISLINE, &d, 1);
// 		PEvset(m_hPE, PEP_fMANUALXAXISTICK, &d, 1);
// 		
// 		// Make it so monochrome image is just black and white //
// 		DWORD bwc[2];
// 		bwc[0] = 0L;
// 		bwc[1] = RGB(255,255,255);
// 		PEvset(m_hPE, PEP_dwaSUBSETSHADES, bwc, 2);
// 		
// 		// Remove standard legend, rose has a special legend //
// 		int stl = -1;
// 		PEvset(m_hPE, PEP_naSUBSETSTOLEGEND, &stl, 1);
// 		
// 		// Set inner circle label //
// 		PEszset(m_hPE, PEP_szMANUALMAXDATASTRING, "1.25|1.1%");
// 		
// 		// Get rid of radius labels //
// 		PEnset(m_hPE, PEP_bGRIDINFRONT, FALSE);
// 		
// 		// Enable zooming //
// 		PEnset(m_hPE, PEP_nALLOWZOOMING, PEAZ_HORZANDVERT);
// 		PEnset(m_hPE, PEP_nZOOMSTYLE, PEZS_RO2_NOT);
// 		
// 		PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
// 		PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
// 		PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
// 		PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, FALSE);
// 		PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_NO_BORDER);
// 		PEnset(m_hPE, PEP_bSIMPLEPOINTLEGEND, TRUE);
// 		PEnset(m_hPE, PEP_bSIMPLELINELEGEND, TRUE);
// 		PEnset(m_hPE, PEP_nLEGENDSTYLE, PELS_1_LINE);
// 		
// 		// Add Some Padding on Right and Bottom //
// 		PEnset(m_hPE, PEP_nIMAGEADJUSTLEFT, 400);
// 		PEnset(m_hPE, PEP_nIMAGEADJUSTRIGHT, 400);
// 		PEnset(m_hPE, PEP_nIMAGEADJUSTBOTTOM, 400);
// 		
// 		PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
// 		PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
// 		PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
// 		PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
// 		PEnset(m_hPE, PEP_bLINESHADOWS, TRUE);
// 		PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE);
// 		PEnset(m_hPE, PEP_nGRADIENTBARS, 8);
// 		PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_SHADOWS);
// 		float f = 1.2F;
// 		PEvset(m_hPE, PEP_fFONTSIZEGNCNTL, &f, 1);
		
}



void CGraphicView::OnBkArea() 
{ 
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	// This example builds upon the basic CreateSimpleGraph '000' example chart //
	CreateSimpleGraph();
	
	/////////////////////////////////////////
	// Construct a simple table annotation //
	/////////////////////////////////////////
// 	PEnset(m_hPE, PEP_nWORKINGTABLE, 0);
// 	PEnset(m_hPE, PEP_nTAROWS, 1);
// 	PEnset(m_hPE, PEP_nTACOLUMNS, 2);
// 	PEnset(m_hPE, PEP_nTATEXTSIZE, 85);
// 	
// 	// Pass the table text //
// 	char szTmp[48];
// 	strcpy(szTmp, "Category 1");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 0, szTmp);
// 	strcpy(szTmp, "Category 2");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 1, szTmp);
// 	
// 	// Set column widths ///
// 	int nWidth = 28;
// 	PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 0, &nWidth);
// 	PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 1, &nWidth);
// 	
// 	PEnset(m_hPE, PEP_bSHOWTABLEANNOTATION, TRUE);
// 	
// 	PEnset(m_hPE, PEP_nTALOCATION, PETAL_INSIDE_AXIS_0);
// 	PEnset(m_hPE, PEP_nTAAXISLOCATION, PETAAL_TOP_LEFT);
// 	PEnset(m_hPE, PEP_nTABORDER, PETAB_SINGLE_LINE);
// 	PEnset(m_hPE, PEP_dwTABACKCOLOR, RGB(255,255,255));
// 	
// 	int nJust = PETAJ_CENTER;
// 	PEvsetcell(m_hPE, PEP_naTAJUSTIFICATION, 0, &nJust);
// 	PEvsetcell(m_hPE, PEP_naTAJUSTIFICATION, 1, &nJust);
// 	
// 	/////////////////////////////////////////
// 	// Construct a simple table annotation //
// 	/////////////////////////////////////////
// 	PEnset(m_hPE, PEP_nWORKINGTABLE, 1);
// 	PEnset(m_hPE, PEP_nTAROWS, 1);
// 	PEnset(m_hPE, PEP_nTACOLUMNS, 4);
// 	PEnset(m_hPE, PEP_nTATEXTSIZE, 85);
// 	
// 	strcpy(szTmp, "Sub Cat 1");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 0, szTmp);
// 	strcpy(szTmp, "Sub Cat 2");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 1, szTmp);
// 	strcpy(szTmp, "Sub Cat 3");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 2, szTmp);
// 	strcpy(szTmp, "Sub Cat 4");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 3, szTmp);
// 	
// 	nWidth = 14;
// 	PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 0, &nWidth);
// 	PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 1, &nWidth);
// 	PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 2, &nWidth);
// 	PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 3, &nWidth);
// 	
// 	nJust = PETAJ_CENTER;
// 	PEvsetcell(m_hPE, PEP_naTAJUSTIFICATION, 0, &nJust);
// 	PEvsetcell(m_hPE, PEP_naTAJUSTIFICATION, 1, &nJust);
// 	PEvsetcell(m_hPE, PEP_naTAJUSTIFICATION, 2, &nJust);
// 	PEvsetcell(m_hPE, PEP_naTAJUSTIFICATION, 3, &nJust);
// 	
// 	PEnset(m_hPE, PEP_dwTABACKCOLOR, RGB(240,240,240));
// 	PEnset(m_hPE, PEP_bSHOWTABLEANNOTATION, TRUE);
// 	
// 	PEnset(m_hPE, PEP_nTALOCATION, PETAL_INSIDE_AXIS_0);
// 	PEnset(m_hPE, PEP_nTAAXISLOCATION, PETAAL_TOP_LEFT + PETAAL_NEW_ROW);
// 	
// 	PEnset(m_hPE, PEP_nTABORDER, PETAB_SINGLE_LINE);
// 	
// 	/////////////////////////////////////////
// 	// Construct a simple table annotation //
// 	/////////////////////////////////////////
// 	PEnset(m_hPE, PEP_nWORKINGTABLE, 2);
// 	PEnset(m_hPE, PEP_nTAROWS, 4);
// 	PEnset(m_hPE, PEP_nTACOLUMNS, 8);
// 	PEnset(m_hPE, PEP_nTATEXTSIZE, 85);
// 	
// 	// Set Column Widths //
// 	nWidth = 7;
// 	PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 0, &nWidth);
// 	PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 1, &nWidth);
// 	PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 2, &nWidth);
// 	PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 3, &nWidth);
// 	PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 4, &nWidth);
// 	PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 5, &nWidth);
// 	PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 6, &nWidth);
// 	PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 7, &nWidth);
// 	
	// Pass the table text //
	float fp;
	int nTmp;
	for (int r=0; r<4; r++)
	{
		for (int c=0; c<8; c++)
		{
			fp = float(sin(0.3F * (double) c + 1) * 500.0F) + GetRandom(2, 50);
//			sprintf(szTmp, "%.2f ", fp);
//			PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c, szTmp);
			
			if (c > 5)
				nTmp = RGB(0, 148, 0);
			else if (c > 3)
				nTmp = RGB(148, 148, 0);
			else if (c > 1)
				nTmp = RGB(0, 148, 148);
			else
				nTmp = RGB(198, 0, 0);
			
//			PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, r, c, &nTmp); 
			
	//		nJust = PETAJ_CENTER;
	//		PEvsetcellEx(m_hPE, PEP_naTAJUSTIFICATION, r, c, &nJust);
		}
	}
	
	// Other Table Related Properties ///
// 	PEnset(m_hPE, PEP_bSHOWTABLEANNOTATION, TRUE);
// 	
// 	PEnset(m_hPE, PEP_nTALOCATION, PETAL_INSIDE_AXIS_0);
// 	PEnset(m_hPE, PEP_nTAAXISLOCATION, PETAAL_TOP_LEFT + PETAAL_NEW_ROW + PETAAL_NEW_ROW);
// 	PEnset(m_hPE, PEP_nTABORDER, PETAB_SINGLE_LINE);
// 	PEnset(m_hPE, PEP_dwTABACKCOLOR, RGB(200, 200, 200));
// 	
// 	// Done setting Table Annotations ///
// 	
// 	// Set Various Other Properties ///
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_SHADOW);
// 	PEnset(m_hPE, PEP_nGRIDLINECONTROL, PEGLC_BOTH);
// 	PEnset(m_hPE, PEP_nDATAPRECISION, 0);
// 	
// 	double f = 1.5F;
// 	PEvset(m_hPE, PEP_fAXISNUMBERSPACING, &f, 1);
	
}


void CGraphicView::OnBkBar() 
{ 
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	// This example builds upon the basic CreateSimpleGraph '000' example chart //
	CreateSimpleGraph();
	
	// Construct a simple table annotation //
// 	PEnset(m_hPE, PEP_nWORKINGTABLE, 0);
// 	PEnset(m_hPE, PEP_nTAROWS, 4);
// 	PEnset(m_hPE, PEP_nTACOLUMNS, 7);
	
	// Pass the table text //
	char szTmp[48];
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 1, 0, "Row 2 ");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 2, 0, "Row 3 ");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 3, 0, "Row 4 ");
// 	
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 1, "Column 2 ");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 2, "Column 3 ");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 3, "Column 4 ");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 4, "Column 5 ");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 5, "Column 6 ");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 6, "Column 7 ");
	
	float fp;
	int nTmp;
	for (int r=1; r<4; r++)
	{
		for (int c=1; c<7; c++)
		{
			fp = float(sin(0.3F * (double) c + 1) * 500.0F) + GetRandom(2, 50);
			sprintf(szTmp, "%.5f ", fp);
//			PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c, szTmp);
			nTmp = RGB(198, 0, 0);
	//		PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, r, c, &nTmp); 
		}
	}
	
// 	// Other Table Related Properties ///
// 	PEnset(m_hPE, PEP_bSHOWTABLEANNOTATION, TRUE);
// 	PEnset(m_hPE, PEP_nTALOCATION, PETAL_INSIDE_TOP_LEFT);
// 	PEnset(m_hPE, PEP_dwTABACKCOLOR, RGB(255,255,255));
// 	PEnset(m_hPE, PEP_nTAHEADERROWS, 1);
// 	PEnset(m_hPE, PEP_bTAHEADERCOLUMN, TRUE);
// 	PEnset(m_hPE, PEP_nTATEXTSIZE, 85);
// 	
// 	// Construct a second simple table annotation //
// 	PEnset(m_hPE, PEP_nWORKINGTABLE, 1);  // Note WorkingTable goes to 1
// 	PEnset(m_hPE, PEP_nTAROWS, 15);
// 	PEnset(m_hPE, PEP_nTACOLUMNS, 2);
// 	
// 	// // Pass the table text //
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 0, "");
// 	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 1, "Data");
// 	nTmp = PETAJ_CENTER;
// 	PEvsetcellEx(m_hPE, PEP_naTAJUSTIFICATION, 0, 0, &nTmp);
// 	PEvsetcellEx(m_hPE, PEP_naTAJUSTIFICATION, 0, 1, &nTmp);
	
	// First Column Text //
	for (r=1; r<15; r++)
	{
		sprintf(szTmp, "%2d", r);
//		PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, 0, szTmp);
	}
	
	// // Second Column Text //
	for (r=1; r<15; r++)
	{
		fp = float(sin(0.3F * (double) r + 1) * 500.0F) + GetRandom(2, 50);
		sprintf(szTmp, "%.2f", fp);
//		PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, 1, szTmp);
	}
	
	// Other Table Related Properties ///
// 	PEnset(m_hPE, PEP_bSHOWTABLEANNOTATION, TRUE);
// 	PEnset(m_hPE, PEP_nTALOCATION, PETAL_LEFT_CENTER);
// 	PEnset(m_hPE, PEP_dwTABACKCOLOR, RGB(255,255,255));
// 	PEnset(m_hPE, PEP_nTAHEADERROWS, 1);
// 	PEnset(m_hPE, PEP_bTAHEADERCOLUMN, TRUE);
// 	
// 	// Done setting Table Annotations ///
// 	
// 	// Set Various Other Properties ///
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_LIGHT_INSET);
// 	PEnset(m_hPE, PEP_nDATAPRECISION, 0);
// 	
}


void CGraphicView::OnBkBarGroup() 
{ 
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	RECT rect;
	GetClientRect( &rect );
	m_canReal=FALSE;
	// Construct Object //
//	m_hPE = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rect, m_hWnd, 1000);
// 	
// 	PEnset(m_hPE, PEP_nSUBSETS, 4);
// 	PEnset(m_hPE, PEP_nPOINTS, 12);
	
	// Change Data to match example scales which are manually configured //
	for (int s=0; s<=3; s++)
	{
		for (int p=0; p<=11; p++)
		{
			float f;
			f =  (p + 1) * 10.0F;
//			PEvsetcellEx(m_hPE, PEP_faXDATA, s, p, &f);
			
			switch (s)
			{
			case 0:
				f = 0.01F + GetRandom(1, 4);
				break;
			case 1:
				f = 400.0F + GetRandom(1, 450);
				break;
			case 2:
				f = 0.01F + GetRandom(1, 25);
				break;
			case 3:
				f = 0.01F + GetRandom(1, 65);
				break;
			}
//			PEvsetcellEx(m_hPE, PEP_faYDATA, s, p, &f);
		}
	}
	
	// Now sub-divide subsets into 4 individual axes //
	// Each axis with one subset in each //
// 	int nArray[4] = {1,1,1,1};
// 	PEvset (m_hPE, PEP_naMULTIAXESSUBSETS, nArray, 4);
// 	
// 	// Create Overlapped Axes //
// 	int nOMA[3] = {1,1,2};
// 	PEvset (m_hPE, PEP_naOVERLAPMULTIAXES, nOMA, 3);
// 	
// 	// Set Proportions of Axes  //
// 	float fMAP[3] = {.2F, .5F, .3F};
// 	PEvset (m_hPE, PEP_faMULTIAXESPROPORTIONS, fMAP, 3);
// 	
// 	// Set up First axis parameters //  
// 	PEnset(m_hPE, PEP_nWORKINGAXIS, 0);
// 	PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
// 	double dMin = 0.0F;
// 	PEvset(m_hPE, PEP_fMANUALMINY, &dMin, 1);
// 	double dMax = 5.0F;
// 	PEvset(m_hPE, PEP_fMANUALMAXY, &dMax, 1);
// 	PEnset(m_hPE, PEP_bMANUALYAXISTICKNLINE, TRUE);
// 	double dLine = 2.5F;
// 	PEvset(m_hPE, PEP_fMANUALYAXISLINE, &dLine, 1);
// 	double dTick = 0.5F;
// 	PEvset(m_hPE, PEP_fMANUALYAXISTICK, &dTick, 1);
// 	PEnset(m_hPE, PEP_nSHOWTICKMARKY, PESTM_TICKS_HIDE); 
// 	PEszset(m_hPE, PEP_szYAXISLABEL, "NPSHR (feet)");
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);
// 	
// 	// Set up Second axis parameters //  
// 	PEnset(m_hPE, PEP_nWORKINGAXIS, 1);
// 	PEnset(m_hPE, PEP_nRYAXISCOMPARISONSUBSETS, 1);
// 	PEnset(m_hPE, PEP_nMANUALSCALECONTROLRY, PEMSC_MINMAX);
// 	dMin = 400.0F;
// 	PEvset(m_hPE, PEP_fMANUALMINRY, &dMin, 1);
// 	dMax = 900.0F;
// 	PEvset(m_hPE, PEP_fMANUALMAXRY, &dMax, 1);
// 	PEnset(m_hPE, PEP_bMANUALRYAXISTICKNLINE, TRUE);
// 	dLine = 100.0F;
// 	PEvset(m_hPE, PEP_fMANUALRYAXISLINE, &dLine, 1);
// 	dTick = 20.0F;
// 	PEvset(m_hPE, PEP_fMANUALRYAXISTICK, &dTick, 1);
// 	PEnset(m_hPE, PEP_nSHOWTICKMARKRY, PESTM_TICKS_HIDE); 
// 	PEszset(m_hPE, PEP_szRYAXISLABEL, "Total Head (feet)");
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHODII, PEGPM_POINTSPLUSSPLINE);
// 	
// 	// Set up Third axis parameters //  
// 	PEnset(m_hPE, PEP_nWORKINGAXIS, 2);
// 	PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
// 	dMin = 0.0F;
// 	PEvset(m_hPE, PEP_fMANUALMINY, &dMin, 1);
// 	dMax = 30.0F;
// 	PEvset(m_hPE, PEP_fMANUALMAXY, &dMax, 1);
// 	PEnset(m_hPE, PEP_bMANUALYAXISTICKNLINE, TRUE);
// 	dLine = 10.0F;
// 	PEvset(m_hPE, PEP_fMANUALYAXISLINE, &dLine, 1);
// 	dTick = 2.0F;
// 	PEvset(m_hPE, PEP_fMANUALYAXISTICK, &dTick, 1);
// 	PEnset(m_hPE, PEP_nSHOWTICKMARKY, PESTM_TICKS_HIDE); 
// 	PEszset(m_hPE, PEP_szYAXISLABEL, "Efficiency (%)");
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_POINTSPLUSSPLINE);
// 	
// 	// Set up Fourth axis parameters //  
// 	PEnset(m_hPE, PEP_nWORKINGAXIS, 3);
// 	PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
// 	dMin = 0.0F;
// 	PEvset(m_hPE, PEP_fMANUALMINY, &dMin, 1);
// 	dMax = 75.0F;
// 	PEvset(m_hPE, PEP_fMANUALMAXY, &dMax, 1);
// 	PEnset(m_hPE, PEP_bMANUALYAXISTICKNLINE, TRUE);
// 	dLine = 25.0F;
// 	PEvset(m_hPE, PEP_fMANUALYAXISLINE, &dLine, 1);
// 	dTick = 5.0F;
// 	PEvset(m_hPE, PEP_fMANUALYAXISTICK, &dTick, 1);
// 	PEnset(m_hPE, PEP_nSHOWTICKMARKY, PESTM_TICKS_HIDE); 
// 	PEszset(m_hPE, PEP_szYAXISLABEL, "Power(HP)");
// 	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_POINTSPLUSSPLINE);
// 	
// 	// Done Setting Y Axis Parameters, reset WorkingAxis //
// 	PEnset(m_hPE, PEP_nWORKINGAXIS, 0);
// 	
// 	// Set various other properties //
// 	PEnset(m_hPE, PEP_nGRIDLINECONTROL, PEGLC_BOTH);
// 	PEnset(m_hPE, PEP_nMULTIAXISSTYLE, PEMAS_GROUP_ALL_AXES);
// 	PEnset(m_hPE, PEP_nLEGENDSTYLE, PELS_1_LINE_INSIDE_OVERLAP);
// 	
// 	PEszset(m_hPE, PEP_szXAXISLABEL, "Flow (gpm)");
// 	
// 	PEnset(m_hPE, PEP_nBORDERTYPES, PETAB_SINGLE_LINE);
// 	PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
// 	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_MEDIUM); 
// 	PEszset(m_hPE, PEP_szMAINTITLE, "");
// 	PEszset(m_hPE, PEP_szSUBTITLE, "");
// 	PEszset(m_hPE, PEP_szSUBTITLEFONT, "Courier New");
// 	
// 	float fSize = 1.1F;
// 	PEvset(m_hPE, PEP_fFONTSIZEMSCNTL, &fSize, 1);
// 	PEvset(m_hPE, PEP_fFONTSIZEMBCNTL, &fSize, 1);
// 	
// 	
// 	PEvsetcell(m_hPE, PEP_szaMULTISUBTITLES, 0, "PUMP PREDICTED PERFORMANCE|Units: US Customary|Date: 06/07/2002");
// 	PEvsetcell(m_hPE, PEP_szaMULTISUBTITLES, 1, "Impeller Diameter (in): Max = 11.875, Rated = 11.72, Min =11.062||");
// 	
// 	PEvsetcell(m_hPE, PEP_szaMULTIBOTTOMTITLES, 1, "Customer:  Since fixed length courier font, you can create rows by number of spaces, ||");
// 	PEvsetcell(m_hPE, PEP_szaMULTIBOTTOMTITLES, 0, "Pump Model:  or, v5 has a table annotation feature which could be used to add text here. ||");
// 	
// 	PEnset(m_hPE, PEP_bYAXISONRIGHT, TRUE);
// 	
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 0, "NPSHR");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 1, "Head");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 2, "Efficiency");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 3, "Power");
// 	
// 	PEnset(m_hPE, PEP_nMULTIAXISSTYLEMENU, PEMC_SHOW);
// 	PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_NONE);
// 	PEnset(m_hPE, PEP_nALLOWZOOMING, PEAZ_HORZANDVERT);
// 	PEnset(m_hPE, PEP_nZOOMSTYLE, PEZS_RO2_NOT);
// 	
// 	PEnset(m_hPE, PEP_bSIMPLELINELEGEND, TRUE);
// 	PEnset(m_hPE, PEP_bSIMPLEPOINTLEGEND, TRUE);
// 	
// 	DWORD dwArray[4] = { RGB(198,0,0), RGB( 0, 198, 198 ), RGB( 198,198,0 ), RGB( 0,198,0 ) };
// 	PEvset(m_hPE, PEP_dwaSUBSETCOLORS, dwArray, 4);
// 	
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// 	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_DARK_NO_BORDER);
// 	
// 	PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
// 	PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bLINESHADOWS, TRUE);
// 	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE);
// 	PEnset(m_hPE, PEP_nGRADIENTBARS, 8);
// 	PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_SHADOWS);
	
}




void CGraphicView::OnBkPie() 
{
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	//! Double Click to start and stop Auto Rotation. //
	//! Click and drag scroll bar thumb tabs to quickly rotate. // 
	//! Right button click to show popup menu. //
	
	RECT rect;
	GetClientRect( &rect );
////	m_hPE = PEcreate(PECONTROL_3D, WS_VISIBLE, &rect, m_hWnd, 1001);
	
	// Non-Surface chart, set PolyMode //
//	PEnset(m_hPE, PEP_nPOLYMODE, PEPM_3DBAR);
	
	// Pass Data //
	double sMin, sMax;
	long Row, Col, o, s, e, nQ;
	long nStartRow, nEndRow, nStartCol, nEndCol, nTargetRows, nTargetCols;
	
	nStartRow = 216;
	nEndRow = 230;
	nTargetRows = nEndRow - nStartRow + 1;
	
	nStartCol = 156;
	nEndCol = 170;
	nTargetCols = nEndCol - nStartCol + 1;
	
	nQ = nTargetRows * nTargetCols;
	
	long* pElevData = NULL;
	float* pMyXData = NULL;    
	float* pMyYData = NULL;
	float* pMyZData = NULL;
	
	char szFile[] = "terrain.bin";
	FILE * pFile = NULL;
	pFile = fopen(szFile, "rb");
	
	if (pFile)
	{
		pElevData = new long[202500];
		// Transfer entire data file into memory //
		size_t cnt;
		e = 0;
		for (s = 0; s < 45; s++)
		{
			cnt = fread(&pElevData[e], sizeof(long), 4500, pFile);
			e += 4500;
		}
		fclose(pFile);
	}
	else
	{
		MessageBox("Data File [terrain.bin] not found in program directory.");
		return;
	}
	
	pMyXData = new float[nQ];
	pMyYData = new float[nQ];
	pMyZData = new float[nQ];
	
	long rowcounter, colcounter;
	BOOL foundtargetindex = FALSE;
	rowcounter = 0;
	colcounter = 0;
	
	sMax = -9999999.9F;
	sMin = 9999999.9F;
	for (Row=0; Row<450; Row++)
	{
		for (Col=0; Col<450; Col++)
		{
			foundtargetindex = FALSE;
			
			if (Row >= nStartRow && Row <= nEndRow) 
			{
				if (Col >= nStartCol && Col <= nEndCol)
				{
					foundtargetindex = TRUE;
					o = (rowcounter * nTargetRows) + colcounter;
					pMyXData[o] = (float) Col + 1;
					pMyZData[o] = (float) Row + 1;
					pMyYData[o] = pElevData[(Row * 450) + Col] * 0.1F;
				}
			}
			if (foundtargetindex) 
				colcounter++;
		}
		if (foundtargetindex)
			rowcounter++;
	}
	
//	PEnset(m_hPE, PEP_nSUBSETS, nTargetRows);
//	PEnset(m_hPE, PEP_nPOINTS, nTargetCols);
	
	// Perform the actual transfer of data //
//	PEvset(m_hPE, PEP_faXDATA, pMyXData, nQ);
//	PEvset(m_hPE, PEP_faYDATA, pMyYData, nQ);
//	PEvset(m_hPE, PEP_faZDATA, pMyZData, nQ);
	
	if (pElevData)
		delete pElevData;
	if (pMyXData)
		delete pMyXData;
	if (pMyYData)
		delete pMyYData;
	if (pMyZData)
		delete pMyZData;
	
	// Set Various Other Properties //
//	PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
//	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
////	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_DARK_SHADOW);
	
	DWORD dwColor[] = {RGB(0, 148, 0), RGB(148, 0, 0), RGB(0, 0, 148),
		RGB(148, 148, 0), RGB(0, 148, 148),
		RGB(0, 182, 0), RGB(182, 0, 0), RGB(0, 0, 182),
		RGB(182, 182, 0), RGB(0, 182, 182),
		RGB(0, 198, 0), RGB(198, 0, 0), RGB(0, 0, 198),
		RGB(198, 198, 0), RGB(0, 198, 148)  };
///	PEvset(m_hPE, PEP_dwaSUBSETCOLORS, dwColor, 15);
	
	// Set subset labels for z axis**
////	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 0, "Row 1");
//	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 1, "Row 2");
//	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 2, "Row 3");
//	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 3, "Row 4");
//	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 4, "Row 5");
//	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 5, "Row 6");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 6, "Row 7");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 7, "Row 8");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 8, "Row 9");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 9, "Row 10");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 10, "Row 11");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 11, "Row 12");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 12, "Row 13");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 13, "Row 14");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 14, "Row 15");
	
	// Set point labels for x axis //
// 	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, 0, "Col 1");
// 	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, 1, "Col 2");
// 	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, 2, "Col 3");
// 	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, 3, "Col 4");
// 	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, 4, "Col 5");
// 	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, 5, "Col 6");
// 	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, 6, "Col 7");
// 	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, 7, "Col 8");
// 	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, 8, "Col 9");
// 	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, 9, "Col 10");
// 	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, 10, "Col 11");
// 	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, 11, "Col 12");
// 	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, 12, "Col 13");
// 	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, 13, "Col 14");
// 	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, 14, "Col 15");
// 	
	// Set Titles //
//	PEszset(m_hPE, PEP_szMAINTITLE, "3D Bar");
//	PEszset(m_hPE, PEP_szSUBTITLE, "");
	
	// Set camera position //
//	PEnset(m_hPE, PEP_nVIEWINGHEIGHT, 15);
//	PEnset(m_hPE, PEP_nDEGREEOFROTATION, 314);
	
	// Set Plotting methods //
//	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, 2);   // ' Shading
	
	// Full rotation detail //
//	PEnset(m_hPE, PEP_nROTATIONDETAIL, PERD_FULLDETAIL);
	
	// Set small font size //
//	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_SMALL);
	
	// Prepare images in memory //
//	PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
// 	PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
// 	PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
// 	PEnset(m_hPE, PEP_bALLOWSTYLEPAGE, FALSE);
//	PEnset(m_hPE, PEP_nROTATIONINCREMENT, PERI_INCBY2);
	
// 	// Add Some Padding on Right and Bottom //
// 	PEnset(m_hPE, PEP_nIMAGEADJUSTLEFT, 400);
// 	PEnset(m_hPE, PEP_nIMAGEADJUSTRIGHT, 400);
// 	PEnset(m_hPE, PEP_nIMAGEADJUSTBOTTOM, 400);
	
//	PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
// 	PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
//	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE);
//	PEnset(m_hPE, PEP_dwGRAPHFORECOLOR, RGB(192,192,192));
	
}



void CGraphicView::OnButton32781() 
{ 
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	//! Double Click to start and stop Auto Rotation. //
	//! Click and drag scroll bar thumb tabs to quickly rotate. // 
	//! Right button click to show popup menu. //
	
	RECT rect;
	GetClientRect( &rect );
////	m_hPE = PEcreate(PECONTROL_3D, WS_VISIBLE, &rect, m_hWnd, 1001);
	
	// Non-Surface chart, set PolyMode //
//	PEnset(m_hPE, PEP_nPOLYMODE, PEPM_SCATTER);
	
	// Set the amount of data //
//	PEnset(m_hPE, PEP_nSUBSETS, 10);
//	PEnset(m_hPE, PEP_nPOINTS, 50);
	
	// Pass data, First two subsets //
	float f, f2;
	for (int s=0; s<=1; s++)
	{
		for (int p=0; p<=49; p++)
		{
			f2 = 5.0F * (float)cos(p * .24);
//			PEvsetcellEx(m_hPE, PEP_faXDATA, s, p, &f2);
			
			if (s == 0)
				f = (35.0F * (float)cos(p * 0.22F)) + (10.0F * (float)sin((s * .1F) * .94F));
			else
				f = (35.0F * (float)cos(p * 0.08F)) + (10.0F * (float)cos((s * .25F) * .6F));
//			PEvsetcellEx(m_hPE, PEP_faYDATA, s, p, &f);
			
			f = 5.0F * (float)(sin(p * .08F));
//			PEvsetcellEx(m_hPE, PEP_faZDATA, s, p, &f);
		}
	}
	
	// Pass more data, Second two subsets //
	for (s=0; s<=1; s++)
	{
		for (int p=0; p<=49; p++)
		{
			f2 = 5.0F * (float)cos(p * .14);
			f2 += 20;
//			PEvsetcellEx(m_hPE, PEP_faXDATA, s + 2, p, &f2);
			
			if (s == 0)
				f = (35.0F * (float)cos(p * 0.2F)) + (10.0F * (float)sin((s * .1F) * .74F));
			else
				f = (35.0F * (float)cos(p * 0.1F)) + (10.0F * (float)cos((s * .25F) * .74F));
			f += 40;
//			PEvsetcellEx(m_hPE, PEP_faYDATA, s + 2, p, &f);
			
			f = 5.0F * (float)(sin(p * .16F));
			f += 20;
//			PEvsetcellEx(m_hPE, PEP_faZDATA, s + 2, p, &f);
		}
	}
	
	// Pass more data, Third two subsets //
	for (s=0; s<=1; s++)
	{
		for (int p=0; p<=49; p++)
		{
			f2 = 5.0F * (float)cos(p * .07);
			f2 -= 20;
//			PEvsetcellEx(m_hPE, PEP_faXDATA, s + 4, p, &f2);
			
			if (s == 0)
				f = (35.0F * (float)cos(p * 0.08F)) + (10.0F * (float)sin((s * .07F) * .64F));
			else
				f = (35.0F * (float)cos(p * 0.2F)) + (10.0F * (float)cos((s * .15F) * 1.94F));
			f += 40;
		//	PEvsetcellEx(m_hPE, PEP_faYDATA, s + 4, p, &f);
			
			f = 5.0F * (float)(sin(p * .09F));
			f += 20;
		//	PEvsetcellEx(m_hPE, PEP_faZDATA, s + 4, p, &f);
		}
	}
	
	// Pass more data, Fourth two subsets //
	for (s=0; s<=1; s++)
	{
		for (int p=0; p<=49; p++)
		{
			f2 = 5.0F * (float)cos(p * .12);
			f2 -= 20;
		//	PEvsetcellEx(m_hPE, PEP_faXDATA, s + 6, p, &f2);
			
			if (s == 0)
				f = (35.0F * (float)cos(p * 0.3F)) + (10.0F * (float)sin((s * .08F) * 1.74F));
			else
				f = (35.0F * (float)cos(p * 0.2F)) + (10.0F * (float)cos((s * .75F) * .774F));
			f -= 40;
		//	PEvsetcellEx(m_hPE, PEP_faYDATA, s + 6, p, &f);
			
			f = 5.0F * (float)(sin(p * .36F));
			f -= 20;
		//	PEvsetcellEx(m_hPE, PEP_faZDATA, s + 6, p, &f);
		}
	}
	
	// Pass more data, Fifth two subsets //
	for (s=0; s<=1; s++)
	{
		for (int p=0; p<=49; p++)
		{
			f2 = 5.0F * (float)cos(p * .14);
			f2 += 20;
//			PEvsetcellEx(m_hPE, PEP_faXDATA, s + 8, p, &f2);
			
			if (s == 0)
				f = (25.0F * (float)cos(p * 0.1F)) + (10.0F * (float)sin((s * .02F) * 3.74F));
			else
				f = (55.0F * (float)cos(p * 0.2F)) + (10.0F * (float)cos((s * .05F) * 5.74F));
			f += 40;
//			PEvsetcellEx(m_hPE, PEP_faYDATA, s + 8, p, &f);
			
			f = 5.0F * (float)(sin(p * .16F));
			f -= 20;
	//		PEvsetcellEx(m_hPE, PEP_faZDATA, s + 8, p, &f);
		}
	}
	
	// Set Various Other Properties //
//	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, FALSE);
//	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_DARK_INSET);
	
	DWORD dwColor[] = {RGB(198, 198, 0), RGB(0, 198, 198), RGB(0, 198, 0), RGB(198, 0, 0), RGB(0, 0, 198), 
		RGB(192,192,192), RGB(198, 0, 198), RGB(200, 198, 150), RGB(200, 200, 140), RGB(150, 0, 198)};
//	PEvset(m_hPE, PEP_dwaSUBSETCOLORS, dwColor, 10);
	
//	int nPTypes[] = {PEPT_PLUS, PEPT_PLUS, PEPT_PLUS, PEPT_PLUS, PEPT_PLUS, PEPT_PLUS, PEPT_PLUS, PEPT_PLUS, PEPT_PLUS, PEPT_PLUS};
//	PEvset(m_hPE, PEP_naSUBSETPOINTTYPES, nPTypes, 10);
	
	// Set subset labels and titles //
//	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 0, "Data 1");
//	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 1, "Data 2");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 2, "Data 3");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 3, "Data 4");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 4, "Data 5");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 5, "Data 6");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 6, "Data 7");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 7, "Data 8");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 8, "Data 9");
// 	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 9, "Data 10");
// 	
//	PEszset(m_hPE, PEP_szMAINTITLE, "3D Scatter");
//	PEszset(m_hPE, PEP_szSUBTITLE, "");
	
	// Set camera position //
//	PEnset(m_hPE, PEP_nVIEWINGHEIGHT, 15);
//	PEnset(m_hPE, PEP_nDEGREEOFROTATION, 145);
	
	// Set Plotting methods //
//	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, 0);   // ' Points
	
	// Set small font size //
//	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_MEDIUM);
//	PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
	
	// Set legend location //
//	PEnset(m_hPE, PEP_nLEGENDLOCATION, PELL_LEFT);
	
	// Prepare images in memory //
//	PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
//	PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
//	PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
	
//	PEnset(m_hPE, PEP_nROTATIONDETAIL, PERD_FULLDETAIL);
//	PEnset(m_hPE, PEP_nROTATIONSPEED, 50);
//	PEnset(m_hPE, PEP_nROTATIONINCREMENT, PERI_INCBY1);
//		PEnset(m_hPE, PEP_bAUTOROTATION, TRUE);
//	PEnset(m_hPE, PEP_bSHOWLEGEND, FALSE);
	
// 	// Add Some Padding around image //
// 	PEnset(m_hPE, PEP_nIMAGEADJUSTLEFT, 400);
// 	PEnset(m_hPE, PEP_nIMAGEADJUSTRIGHT, 400);
// 	PEnset(m_hPE, PEP_nIMAGEADJUSTBOTTOM, 400);
	
//	PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
// 	PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
// //	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE);
// 	PEnset(m_hPE, PEP_dwGRAPHFORECOLOR, RGB(192,192,192));
	
}


void CGraphicView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	if(m_hPE)
//		PEdestroy(m_hPE);
	CreateSimpleGraph(); 
	// TODO: Add your message handler code here
	
}

void CGraphicView::OnDrawNew() 
{ 
	if(m_hPE)
//		PEdestroy(m_hPE);
	// This example builds upon the basic CreateSimpleGraph '000' example chart //
	CreateSimpleGraph();
	
	int nHatchType[4];
//	nHatchType[0] = PEHS_BDIAGONAL;
//	nHatchType[1] = PEHS_CROSS;
//	nHatchType[2] = PEHS_DIAGCROSS;
//	nHatchType[3] = PEHS_FDIAGONAL;
//	PEvset(m_hPE, PEP_naSUBSETHATCH, nHatchType, 4);
	
	// Set Other Various Properties ///
//	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
//	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_INSET);
	
	// Enable Stacked type charts //
//	PEnset(m_hPE, PEP_bNOSTACKEDDATA, FALSE);
	
	// Set plotting method //
//	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BARSTACKED);
//	PEnset(m_hPE, PEP_dwHATCHBACKCOLOR, RGB(100,100,100));
//	PEnset(m_hPE, PEP_bDATASHADOWS, PEDS_SHADOWS); 
	
	// Add a table //
//	PEnset(m_hPE, PEP_nGRAPHPLUSTABLE, PEGPT_BOTH);
//	PEnset(m_hPE, PEP_nDATAPRECISION, 0);
}


void CGraphicView::OnDrawSile() 
{ 
	if(m_hPE)
//		PEdestroy(m_hPE);
	//! Cross-Hair inside chart is a cursor giving a datapoint the focus //
	//! Use arrow keys or mouse to move cursor to a new location //
	//! You must click the control to give it the focus before key strokes will work. //
	//! Right and Left arrow keys move cursor up and down points //
	//! Up and Down arrow keys move cursor up and down subsets //
	
	// This example builds upon the basic CreateSimpleSGraph '100' example chart //
	CreateSimpleSGraph();
	
	// Set up cursor //
//	PEnset(m_hPE, PEP_nCURSORMODE, PECM_DATACROSS);
	
	// Help see data points //
//	PEnset(m_hPE, PEP_bMARKDATAPOINTS, TRUE);
	
	// This will allow you to move cursor by clicking data point //
//	PEnset(m_hPE, PEP_bMOUSECURSORCONTROL, TRUE);
//	PEnset(m_hPE, PEP_bALLOWDATAHOTSPOTS, TRUE);
	
	// Cursor prompting in top left corner //
//	PEnset(m_hPE, PEP_bCURSORPROMPTTRACKING, TRUE);
//	PEnset(m_hPE, PEP_nCURSORPROMPTSTYLE, PECPS_XYVALUES);
//	PEnset(m_hPE, PEP_nCURSORPROMPTLOCATION, PECPL_TOP_RIGHT);
	
	// Disable Zooming //
//	PEnset(m_hPE, PEP_nALLOWZOOMING, PEAZ_NONE);
	
	// Other possible values for cursor mode are below, //
	// PECM_NOCURSOR = 0
	// PECM_POINT = 1
	// PECM_DATACROSS = 2
	// PECM_DATASQUARE = 3
	// PECM_FLOATINGY = 4
	// PECM_FLOATINGXY = 5
	
	// Uncomment to start cursor at different location //	
	// NOTE: You must reinitialize before setting cursor location //
	// PEreinitialize(m_hPE);
	// PEresetimage(m_hPE, 0, 0);
	// PEnset(m_hPE, PEP_nCURSORSUBSET, 0);
	// PEnset(m_hPE, PEP_nCURSORPOINT, 2);
	
	////////////////////////////////////////////////////////////////////////
	// Look in OnCommand handler to see how to respond to cursor movement //
	////////////////////////////////////////////////////////////////////////
	/*
	
	  else if ((m_nLastSelection == 105) && (HIWORD(wp) == PEWN_CURSORMOVE))
	  {
	  int nSubset, nPoint;
	  float xvalue, yvalue;
	  char buffer[64];
	  
		nSubset = PEnget(m_hPE, PEP_nCURSORSUBSET);
		nPoint = PEnget(m_hPE, PEP_nCURSORPOINT);
		
		  PEvgetcellEx(m_hPE, PEP_faXDATA, nSubset, nPoint, &xvalue);
		  PEvgetcellEx(m_hPE, PEP_faYDATA, nSubset, nPoint, &yvalue);
		  
			sprintf(buffer, "Cursor at %d,%d=(%.2f, %.2f)", nSubset, nPoint, xvalue, yvalue);
			
			  CWnd* pParent = GetParent()->GetParent();
			  if (pParent) {pParent->SetWindowText(buffer);}
			  
				pParent = AfxGetMainWnd();
				if (pParent) {pParent->SetWindowText("PEWN_CURSORMOVE");}
				
				  return TRUE;
				  }
	*/
	
	
}


void CGraphicView::CreateSimpleSGraph()
{
	
	if(m_hPE)
//		PEdestroy(m_hPE);
	
	m_Real=FALSE;
	m_canReal=FALSE;
	//! Right button click to show popup menu. //
	//! Double Click to show customization dialog. //
	//! Left-Click and drag to draw zoom box. Use popup memu or 'z' to undo zoom. // 
	
	// Simple example show the basics of a scientific graph object. //
	// Scientific Graph's contain both YData and XData and thus data
	// is not plotted equally spaced as the graph object does.
	
	RECT rect;
	GetClientRect( &rect );
////	m_hPE = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rect, m_hWnd, 1001);
	if( m_hPE )
	{
		float fY;
		float fX;
		
		// Set number of Subsets and Points //
///		PEnset(m_hPE, PEP_nSUBSETS, 4);	
	//	PEnset(m_hPE, PEP_nPOINTS, 12);	
		
		for( int s=0; s<=3; s++ )
		{
			for( int p=0; p<=11; p++ )
			{										  
				fX = ((float) (p+1)) * 100.0F;
		//		PEvsetcellEx (m_hPE, PEP_faXDATA, s, p, &fX);
				fY = ((float) (p+1) * 1.0F) + GetRandom(1, 250);
		//		PEvsetcellEx (m_hPE, PEP_faYDATA, s, p, &fY);
			}
		}
		
		// Set DataShadows to show shadows
//		PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_SHADOWS);
		
//		PEszset(m_hPE, PEP_szMAINTITLE, "历史线性统计曲线");
// 		PEszset(m_hPE, PEP_szSUBTITLE, "程序设计与实现--戚高"); 
// 		PEszset(m_hPE, PEP_szYAXISLABEL, "最大应用量");
// 		PEszset(m_hPE, PEP_szXAXISLABEL, "月份");
// 		PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
// 		PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
// 		PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
//		PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_POINTSPLUSSPLINE);
//		PEnset(m_hPE, PEP_nGRIDLINECONTROL, PEGLC_NONE);
//		PEnset(m_hPE, PEP_nALLOWZOOMING, PEAZ_HORZANDVERT);
////		PEnset(m_hPE, PEP_nZOOMSTYLE, PEZS_RO2_NOT);
		
		// subset labels //
//		PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 0, "长沙" );
// 		PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 1, "上海" );
// 		PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 2, "北京" );
// 		PEvsetcell( m_hPE, PEP_szaSUBSETLABELS, 3, "广州" );
// 		
		// subset colors
		DWORD dwArray[4] = { RGB(198,0,0), RGB( 0, 198, 198 ), RGB( 198,198,0 ), RGB( 0,198,0 ) };
//		PEvsetEx( m_hPE, PEP_dwaSUBSETCOLORS, 0, 4, dwArray, 0 );
		
		// subset line types
//		int nLineTypes[] = { PELT_MEDIUMSOLID, PELT_MEDIUMSOLID, PELT_MEDIUMSOLID, PELT_MEDIUMSOLID };
//		PEvset(m_hPE, PEP_naSUBSETLINETYPES, nLineTypes, 4);
		
		// subset point types
////////		int nPointTypes[] = { PEPT_DOTSOLID, PEPT_UPTRIANGLESOLID, PEPT_SQUARESOLID, PEPT_DOWNTRIANGLESOLID };
//		PEvset(m_hPE, PEP_naSUBSETPOINTTYPES, nPointTypes, 4);
		
		// Version 4.0 Features //
// 		PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
// 		PEnset(m_hPE, PEP_bSIMPLEPOINTLEGEND, TRUE);
// 		PEnset(m_hPE, PEP_bSIMPLELINELEGEND, TRUE);
// //		PEnset(m_hPE, PEP_nLEGENDSTYLE, PELS_1_LINE);
//		PEnset(m_hPE, PEP_nMULTIAXISSTYLE, PEMAS_SEPARATE_AXES);
		
		// Set Various Other Properties //
	//	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
//		PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_NO_BORDER);
		
// 		PEnset(m_hPE, PEP_nGRADIENTBARS, 8);
// //		PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
// 		PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
// 		PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
// 		PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
// 		PEnset(m_hPE, PEP_bLINESHADOWS, TRUE);
// //		PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE);
// 		PEnset(m_hPE, PEP_bSCROLLINGHORZZOOM, TRUE);
		
		
	}
}



void CGraphicView::OnReal() 
{
	KillTimer(1);
	if(m_hPE)
//		PEdestroy(m_hPE);
	
	m_canReal=TRUE;
	m_Real=TRUE;
	
	//! Chart holds 2000 points.  Data is fed into
	//! data arrays at current data index.  Once 2000
	//! points have been passed, 250 are truncated as
	//! data is shifted to make room for 250 more data
	//! points.
	
	//! Depending upon ZoomMode property, this example with
	//! either show most current 3 minutes or all data in
	//! object.
	
	
	RECT rect;
	GetClientRect( &rect );
	
	// Construct Object //
//	m_hPE = PEcreate(PECONTROL_SGRAPH, WS_CHILD|WS_VISIBLE, &rect, m_hWnd, 1001);
	
// 	PEnset(m_hPE, PEP_nSUBSETS, 3);
// 	PEnset(m_hPE, PEP_nPOINTS, 2000);
	
//	PEnset(m_hPE, PEP_nDATETIMEMODE, PEDTM_VB);	// Date/Time Mode
//	PEnset(m_hPE, PEP_bUSINGXDATAII, TRUE);     // Using Double Precision XDataII
	
	// Some functional attributes            // 1440 is minutes in a day
	ZoomInterval = (3.0F / 1440.0F);         // 3 minute scale zoom interval
	ZoomSmallInterval = (0.5F / 1440.0F);    // 30 second scale adjustment
	
	// Determine current date and time in VB's format //
	CTime t = CTime::GetCurrentTime();
////	TM petm;
// 	petm.nDay = t.GetDay();
// 	petm.nMonth = t.GetMonth();
// 	petm.nYear = t.GetYear() - 1900;
// 	petm.nHour = t.GetHour();
// 	petm.nMinute = t.GetMinute();
// 	petm.nSecond = t.GetSecond();
// 	PEcreateserialdate(&StartTime, &petm, PEDTM_VB);
	
	// Extents to show all data points 
	double manmaxx;
//	PEnset(m_hPE, PEP_nMANUALSCALECONTROLX, PEMSC_MINMAX);
//	PEvset(m_hPE, PEP_fMANUALMINX, &StartTime, 1);
	manmaxx = StartTime + ZoomInterval;
//	PEvset(m_hPE, PEP_fMANUALMAXX, &manmaxx, 1);
	
//	PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
	double d = 1.0F;
//	PEvset(m_hPE, PEP_fMANUALMINY, &d, 1);
	d = 100.0F;
//	PEvset(m_hPE, PEP_fMANUALMAXY, &d, 1);
	
	// Set Zoomed partial Range of X Scale //
	ZoomStart = StartTime;
	ZoomEnd = ZoomStart + ZoomInterval;
	
// 	PEvset(m_hPE, PEP_fZOOMMINX, &ZoomStart, 1);
// 	PEvset(m_hPE, PEP_fZOOMMAXX, &ZoomEnd, 1);
// 	d = 1.0F;
// 	PEvset(m_hPE, PEP_fZOOMMINY, &d, 1);
// 	d = 100.0F;
// 	PEvset(m_hPE, PEP_fZOOMMAXY, &d, 1);
// 	
// //	PEnset(m_hPE, PEP_bZOOMMODE, TRUE);
	// If ZoomMode is True, you will see 3 minutes of data //
	// If ZoomMode is False, you will see all data in object //
	
	// Clear out default data '
	// No need to clear out XData since using XDataII '
	float f = 0.0F;
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 0, &f);
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 1, &f);
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 2, &f);
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 3, &f);
// 	
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 1, 0, &f);
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 1, 1, &f);
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 1, 2, &f);
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 1, 3, &f);
// 	
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 2, 0, &f);
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 2, 1, &f);
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 2, 2, &f);
// 	PEvsetcellEx(m_hPE, PEP_faYDATA, 2, 3, &f);
// 	
	// Show Annotations //
// 	PEnset(m_hPE, PEP_bSHOWANNOTATIONS, TRUE);
// 	
// 	// Set Various Other Properties ///
// 	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// //	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_DARK_INSET);
// 	
// 	int nTmpStyle;
// //	nTmpStyle = PELT_THINSOLID;
// 	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 0, &nTmpStyle); 
// 	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 1, &nTmpStyle); 
// 	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 2, &nTmpStyle);
	// PEvset(m_hPE, PEP_naSUBSETLINETYPES, PELT_THINSOLID, 0);
	
	char szSubsets[] = "电流\t电压\t频率\t";
// 	PEvset (m_hPE, PEP_szaSUBSETLABELS, szSubsets, 3);
// 	
// 	PEszset(m_hPE, PEP_szMAINTITLE, "工业用实时曲线");
// 	PEszset(m_hPE, PEP_szSUBTITLE, "编程设计实现-戚高");
// 	PEnset(m_hPE, PEP_bNORANDOMPOINTSTOEXPORT, TRUE);
// 	PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
// 	PEnset(m_hPE, PEP_bALLOWBAR, FALSE);
// 	PEnset(m_hPE, PEP_bALLOWPOPUP, FALSE);
// 	PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
// 	PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
// 	PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
	
	DWORD col = RGB(0, 198, 198);
// 	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 0, &col); 
// 	
// 	col = RGB(0,198,0);
// 	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 1, &col);
// 	
// 	col = RGB(255,0,0);
// 	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 2, &col);
// 	
// 	PEnset(m_hPE, PEP_nGRADIENTBARS, 8);
// //	PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
// 	PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
// 	PEnset(m_hPE, PEP_bLINESHADOWS, TRUE);
// //	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE);
// 	
// //	PEreinitialize(m_hPE);
//	PEresetimage(m_hPE, 0, 0);
	::InvalidateRect(m_hPE, NULL, FALSE);
	
	// Initialize Counters and Timer //
	m_nRealTimeCounter = 1;
	m_nSinCounter = 1;
	SetTimer( 2, 250, NULL ); 
}



void CGraphicView::OnDrawgraph() 
{  
	if(m_hPE)
	{
//		PEdestroy(m_hPE);
		Invalidate();
	}
	m_Real=TRUE;
	m_canReal=TRUE;
   //! Chart fills 100 points but x axis is initially
   //! manually scaled.  Once 100 point have been passed,
   //! the chart switches to autoscaling the x axis.

   RECT rect;
   GetClientRect( &rect );

   // Construct Object //
//   m_hPE = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rect, m_hWnd, 1001);

//    PEnset(m_hPE, PEP_nSUBSETS, 1);
//    PEnset(m_hPE, PEP_nPOINTS, 100);
// 
//    // Set Manual Y scale //
// //   PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
//    double arg = 1.0F;
//    PEvset(m_hPE, PEP_fMANUALMINY, &arg, 1);
//    arg = 100.0F;
//    PEvset(m_hPE, PEP_fMANUALMAXY, &arg, 1);
// 
//    // Set Manual X scale //
// //   PEnset(m_hPE, PEP_nMANUALSCALECONTROLX, PEMSC_MINMAX);
//    arg = 1.0F;
//    PEvset(m_hPE, PEP_fMANUALMINX, &arg, 1);
//    arg = 100;
//    PEvset(m_hPE, PEP_fMANUALMAXX, &arg, 1);
// 
//    // Clear out default data //
//    float val = 0;
//    PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 0, &val);
//    PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 1, &val);
//    PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 2, &val);
//    PEvsetcellEx(m_hPE, PEP_faXDATA, 0, 3, &val);
// 
//    PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 0, &val);
//    PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 1, &val);
//    PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 2, &val);
//    PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 3, &val);
// 
//    PEvsetcellEx(m_hPE, PEP_faXDATA, 1, 0, &val);
//    PEvsetcellEx(m_hPE, PEP_faXDATA, 1, 1, &val);
//    PEvsetcellEx(m_hPE, PEP_faXDATA, 1, 2, &val);
//    PEvsetcellEx(m_hPE, PEP_faXDATA, 1, 3, &val);
// 
//    PEvsetcellEx(m_hPE, PEP_faYDATA, 1, 0, &val);
//    PEvsetcellEx(m_hPE, PEP_faYDATA, 1, 1, &val);
//    PEvsetcellEx(m_hPE, PEP_faYDATA, 1, 2, &val);
//    PEvsetcellEx(m_hPE, PEP_faYDATA, 1, 3, &val);
// 
//    PEvsetcellEx(m_hPE, PEP_faXDATA, 2, 0, &val);
//    PEvsetcellEx(m_hPE, PEP_faXDATA, 2, 1, &val);
//    PEvsetcellEx(m_hPE, PEP_faXDATA, 2, 2, &val);
//    PEvsetcellEx(m_hPE, PEP_faXDATA, 2, 3, &val);
// 
//    PEvsetcellEx(m_hPE, PEP_faYDATA, 2, 0, &val);
//    PEvsetcellEx(m_hPE, PEP_faYDATA, 2, 1, &val);
//    PEvsetcellEx(m_hPE, PEP_faYDATA, 2, 2, &val);
//    PEvsetcellEx(m_hPE, PEP_faYDATA, 2, 3, &val);
// 
//    int nTmpStyle;
// //   nTmpStyle = PELT_THINSOLID;
//    PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 0, &nTmpStyle); 
//    PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 1, &nTmpStyle); 
//    PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 2, &nTmpStyle);
//    //	PEvset(m_hPE, PEP_naSUBSETLINETYPES, PELT_THINSOLID, 0);
//    
//    char szSubsets[] = "电流\t电压\t频率\t";
//    PEvset (m_hPE, PEP_szaSUBSETLABELS, szSubsets, 3);
//    
//    // Set Various Other Properties ///
//    PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
// //   PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_INSET);
//    
//    PEszset(m_hPE, PEP_szMAINTITLE, "工业用实时曲线程序");
//    
//    PEszset(m_hPE, PEP_szSUBTITLE, "编程设计实现-戚高");
//    PEnset(m_hPE, PEP_bNORANDOMPOINTSTOEXPORT, TRUE);
//    PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
//    PEnset(m_hPE, PEP_bALLOWBAR, FALSE);
//    PEnset(m_hPE, PEP_bALLOWPOPUP, FALSE);
//    PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
//    PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
//    PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
// 
//    DWORD col = RGB(255, 0, 0);
//    PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 0, &col); 
//    
//    col = RGB(0,198,0);
//    PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 1, &col);
//    
//    col = RGB(255,0,0);
//    PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 2, &col);
// //    
// //    PEnset(m_hPE, PEP_nGRADIENTBARS, 8);
// //    PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
// //    PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
// //    PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
// //    PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
// //    PEnset(m_hPE, PEP_bLINESHADOWS, TRUE);
// //    PEnset(m_hPE, PEP_nFONTSIZE, PEFS_LARGE);

//   PEreinitialize(m_hPE);
//   PEresetimage(m_hPE, 0, 0);

   // Initialize Counters and Timer 
   m_nRealTimeCounter = 1;
   m_nSinCounter = 1;
   SetTimer( 1, 100, NULL );
 
}
 
