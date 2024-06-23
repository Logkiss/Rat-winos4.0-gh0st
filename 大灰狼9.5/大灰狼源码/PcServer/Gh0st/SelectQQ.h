#if !defined(AFX_SELECTQQ_H__ED980DDB_1FBB_4086_87FB_814F48A0008D__INCLUDED_)
#define AFX_SELECTQQ_H__ED980DDB_1FBB_4086_87FB_814F48A0008D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectQQ.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectQQ dialog

class CSelectQQ : public CDialog
{
// Construction
public:
	CSelectQQ(CString strQQNum,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectQQ)
	enum { IDD = IDD_SELECT_QQ };
	CComboBox	m_combo_qq;
	//}}AFX_DATA
	CString m_strQQ;
	CString m_strSelect;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectQQ)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectQQ)
	afx_msg void OnSelchangeComboQq();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTQQ_H__ED980DDB_1FBB_4086_87FB_814F48A0008D__INCLUDED_)
