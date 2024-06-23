// PcServerDoc.cpp : implementation of the CPcServerDoc class
//

#include "stdafx.h"
#include "PcServer.h"

#include "PcServerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPcServerDoc

IMPLEMENT_DYNCREATE(CPcServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CPcServerDoc, CDocument)
	//{{AFX_MSG_MAP(CPcServerDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPcServerDoc construction/destruction

CPcServerDoc::CPcServerDoc()
{
	// TODO: add one-time construction code here

}

CPcServerDoc::~CPcServerDoc()
{
}

BOOL CPcServerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CPcServerDoc serialization

void CPcServerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPcServerDoc diagnostics

#ifdef _DEBUG
void CPcServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPcServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPcServerDoc commands
