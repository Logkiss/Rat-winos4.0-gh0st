#if !defined(AFX_SETCOLOR_H__24A1F4D9_4E9F_4E18_91BC_452EE87179F1__INCLUDED_)
#define AFX_SETCOLOR_H__24A1F4D9_4E9F_4E18_91BC_452EE87179F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetColor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetColor dialog

class CSetColor : public CDialog
{
// Construction
public:
	CSetColor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetColor)
	enum { IDD = IDD_COLOR };
	CXTPButton	m_BsendXtp;
	CXTPButton	m_Canxtp;
	CXTPComboBox	m_ComBox_Color;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetColor)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetColor)
	afx_msg void OnBSend();
	afx_msg void OnBcancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int nSecletIndex;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETCOLOR_H__24A1F4D9_4E9F_4E18_91BC_452EE87179F1__INCLUDED_)
