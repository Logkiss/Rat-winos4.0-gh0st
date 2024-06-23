// PcServerDoc.h : interface of the CPcServerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCSERVERDOC_H__9A0888C9_3216_40D4_995C_DDCC58DC28DB__INCLUDED_)
#define AFX_PCSERVERDOC_H__9A0888C9_3216_40D4_995C_DDCC58DC28DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPcServerDoc : public CDocument
{
protected: // create from serialization only
	CPcServerDoc();
	DECLARE_DYNCREATE(CPcServerDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPcServerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPcServerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPcServerDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCSERVERDOC_H__9A0888C9_3216_40D4_995C_DDCC58DC28DB__INCLUDED_)
