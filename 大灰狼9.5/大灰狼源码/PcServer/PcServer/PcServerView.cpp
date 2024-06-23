// PcServerView.cpp : implementation of the CPcServerView class
//

#include "stdafx.h"
#include "PcServer.h"

#include "PcServerDoc.h"
#include "PcServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPcServerView

IMPLEMENT_DYNCREATE(CPcServerView, CView)

BEGIN_MESSAGE_MAP(CPcServerView, CView)
	//{{AFX_MSG_MAP(CPcServerView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPcServerView construction/destruction

CPcServerView::CPcServerView()
{
	// TODO: add construction code here

}

CPcServerView::~CPcServerView()
{
}

BOOL CPcServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CPcServerView drawing

void CPcServerView::OnDraw(CDC* pDC)
{
	CPcServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CPcServerView printing

BOOL CPcServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPcServerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPcServerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CPcServerView diagnostics

#ifdef _DEBUG
void CPcServerView::AssertValid() const
{
	CView::AssertValid();
}

void CPcServerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPcServerDoc* CPcServerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPcServerDoc)));
	return (CPcServerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPcServerView message handlers
