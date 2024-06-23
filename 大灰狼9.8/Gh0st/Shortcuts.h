#if !defined(AFX_SHORTCUTS_H__00237C2F_DFA5_435D_A062_F7C8D96F036F__INCLUDED_)
#define AFX_SHORTCUTS_H__00237C2F_DFA5_435D_A062_F7C8D96F036F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Shortcuts.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShortcuts form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CShortcuts : public CFormView
{
protected:
	CShortcuts();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CShortcuts)

// Form Data
public:
	//{{AFX_DATA(CShortcuts)
	enum { IDD = IDD_SHORTCUTS };
	CXTPButton	m_taskmgr;
	CXTPButton	m_services;
	CXTPButton	m_regedit;
	CXTPButton	m_notepad;
	CXTPButton	m_mstsc;
	CXTPButton	m_mspaint;
	CXTPButton	m_iexpress;
	CXTPButton	m_gpedit;
	CXTPButton	m_directx;
	CXTPButton	m_compmgmt;
	CXTPButton	m_cmd;
	CXTPButton	m_clac;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShortcuts)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CShortcuts();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CShortcuts)
	afx_msg void OnClac();
	afx_msg void OnCmd();
	afx_msg void OnCompmgmt();
	afx_msg void OnDirectx();
	afx_msg void OnGpedit();
	afx_msg void OnIexpress();
	afx_msg void OnMspaint();
	afx_msg void OnMstsc();
	afx_msg void OnNotepad();
	afx_msg void OnRegedit();
	afx_msg void OnServices();
	afx_msg void OnTaskmgr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHORTCUTS_H__00237C2F_DFA5_435D_A062_F7C8D96F036F__INCLUDED_)
