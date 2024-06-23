// Gh0stDoc.cpp : implementation of the CGh0stDoc class
//

#include "stdafx.h"
#include "Gh0st.h"

#include "Gh0stDoc.h"
#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGh0stDoc

IMPLEMENT_DYNCREATE(CGh0stDoc, CDocument)

BEGIN_MESSAGE_MAP(CGh0stDoc, CDocument)
	//{{AFX_MSG_MAP(CGh0stDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGh0stDoc construction/destruction

CGh0stDoc::CGh0stDoc()
{
	// TODO: add one-time construction code here

}

CGh0stDoc::~CGh0stDoc()
{
}

BOOL CGh0stDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	
	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGh0stDoc serialization

void CGh0stDoc::Serialize(CArchive& ar)
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
// CGh0stDoc diagnostics

#ifdef _DEBUG
void CGh0stDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGh0stDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGh0stDoc commands
