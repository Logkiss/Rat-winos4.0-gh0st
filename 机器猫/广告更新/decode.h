CString Base64Decode(LPCTSTR lpszSrc)  
{
	ASSERT(lpszSrc != NULL && AfxIsValidString(lpszSrc));
	const unsigned int BASE64_DECODE_TABLE[256] = {
		255, 255, 255, 255, 255, 255, 255, 255, 
			255, 255, 255, 255, 255, 255, 255, 255, 
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255, 
			255, 255, 255,  62, 255, 255, 255,  63,
			52,  53,  54,  55,  56,  57,  58,  59,
			60,  61, 255, 255, 255, 255, 255, 255, 
			255,   0,   1,   2,   3,   4,   5,   6,
			7,   8,   9,  10,  11,  12,  13,  14, 
			15,  16,  17,  18,  19,  20,  21,  22,
			23,  24,  25, 255, 255, 255, 255, 255, 
			255,  26,  27,  28,  29,  30,  31,  32,
			33,  34,  35,  36,  37,  38,  39,  40, 
			41,  42,  43,  44,  45,  46,  47,  48,
			49,  50,  51, 255, 255, 255, 255, 255, 
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255 };
		
		
		const int nSrcCount=(int)_tcslen(lpszSrc);
		int nSize=nSrcCount/4*3;
		if(lpszSrc[nSrcCount-1]=='=')
			nSize--;
		if(lpszSrc[nSrcCount-2]=='=')
			nSize--;
		char* pOutBuffer=new char[nSize+3];
		ZeroMemory(pOutBuffer,nSize+3);
		LPCTSTR pInBuffer=lpszSrc;
		UINT iTest,iPack;
		for(int i=0;i<nSize/3 ;i++)
		{
			for(int j=0;j<4;j++)
			{
				iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
				//InPtr++;
				if (iTest == 0xFF) 
				{
					j--;
					continue; //读到255非法字符
				}
				iPack = iPack << 6 ;
				iPack = iPack | iTest ;
			}
			pOutBuffer[2] = iPack;
			iPack = iPack >> 8;
			pOutBuffer[1] = iPack;
			iPack = iPack >> 8;
			pOutBuffer[0] = iPack;
			//准备写入后3位
			pOutBuffer+= 3; iPack = 0;
			
		}
		switch(nSize%3)
		{
		case 1:
			iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
			if (iTest != 0xFF)
			{
				iPack = iPack << 6 ;
				iPack = iPack | iTest ;
			}
			iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
			if (iTest != 0xFF)
			{
				iPack = iPack << 6 ;
				iPack = iPack | iTest ;
			}
			iPack = iPack >> 4;
			pOutBuffer[0] = iPack;
			pOutBuffer++;
			break;
		case 2:
			iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
			if (iTest != 0xFF)
			{
				iPack = iPack << 6 ;
				iPack = iPack | iTest ;
			}
			iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
			if (iTest != 0xFF)
			{
				iPack = iPack << 6 ;
				iPack = iPack | iTest ;
			}
			iTest = BASE64_DECODE_TABLE[*pInBuffer++]; // Read from InputBuffer.
			if (iTest != 0xFF)
			{
				iPack = iPack << 6 ;
				iPack = iPack | iTest ;
			}
			iPack = iPack >> 2;
			pOutBuffer[1] = iPack;
			iPack = iPack >> 8;
			pOutBuffer[0] = iPack;
			pOutBuffer+=2;
			break;
		default:
			break;
		}
		pOutBuffer-=nSize;
		CString strDecode=pOutBuffer;
		delete pOutBuffer;
		return strDecode;
}
