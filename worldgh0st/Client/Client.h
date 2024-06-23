// Client.h : main header file for the Client application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "PaneColorSet.h"
#include "IniFile.h"

// CClientApp:
// See Client.cpp for the implementation of this class
//

class CClientApp : public CWinApp
{
public:
	CClientApp();

	enum AppTheme
	{
		themeOffice2007Aqua,
		themeOffice2007Black,
		themeOffice2007Blue,
		themeOffice2007System,
		themeOffice2007Silver,
		themeOffice2010Black,
		themeOffice2010Blue,
		themeOffice2010Silver,
		themeOffice2013Access,
		themeOffice2013Excel,
		themeOffice2013OneNote,
		themeOffice2013Outlook,
		themeOffice2013Publisher,
		themeOffice2013PowerPoint,
		themeOffice2013Word,
		themeVisualStudio2015Blue,
		themeVisualStudio2015Dark,
		themeVisualStudio2015Light,
		themeWindows7
	};
	AppTheme m_theme;

	CXTPModuleHandle m_hModule2010;
	CXTPModuleHandle m_hModule2013;
	CXTPModuleHandle m_hModule2016;
	CXTPModuleHandle m_hModuleWin7;

	CString	m_CompanyName;
	CPaneColorSet m_csPane;
	CIniFile	m_IniFile;
	UINT nPort;
// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CClientApp theApp;