// ClientDoc.cpp : implementation of the CClientDoc class
//

#include "stdafx.h"
#include "Client.h"

#include "ClientDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientDoc

IMPLEMENT_DYNCREATE(CClientDoc, CDocument)

BEGIN_MESSAGE_MAP(CClientDoc, CDocument)
END_MESSAGE_MAP()


// CClientDoc construction/destruction

CClientDoc::CClientDoc()
{
	// TODO: add one-time construction code here

}

CClientDoc::~CClientDoc()
{
}

BOOL CClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CClientDoc serialization

void CClientDoc::Serialize(CArchive& ar)
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


// CClientDoc diagnostics

#ifdef _DEBUG
void CClientDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CClientDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CClientDoc commands
