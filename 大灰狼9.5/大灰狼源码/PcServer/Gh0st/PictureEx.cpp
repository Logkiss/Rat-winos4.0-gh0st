//////////////////////////////////////////////////////////////////////
// PictureEx.cpp: implementation of the CPictureEx class.
//
// Picture displaying control with support for the following formats:
// GIF (including animated GIF87a and GIF89a), JPEG, BMP, WMF, ICO, CUR
// 
// Written by Oleg Bykov (oleg_bykoff@rsdn.ru)
// Copyright (c) 2001
//
// To use CPictureEx, follow these steps:
//   - place a static control on your dialog (either a text or a bitmap)
//   - change its identifier to something else (e.g. IDC_MYPIC)
//   - associate a CStatic with it using ClassWizard
//   - in your dialog's header file replace CStatic with CPictureEx
//     (don't forget to #include "PictureEx.h" and add 
//     PictureEx.h and PictureEx.cpp to your project)
//   - call one of the overloaded CPictureEx::Load() functions somewhere
//     (OnInitDialog is a good place to start)
//   - if the preceding Load() succeeded call Draw()
//  
// You can also add the control by defining a member variable of type 
// CPictureEx, calling CPictureEx::Create (derived from CStatic), then 
// CPictureEx::Load and CPictureEx::Draw.
//
// By default, the control initializes its background to COLOR_3DFACE
// (see CPictureEx::PrepareDC()). You can change the background by
// calling CPictureEx::SetBkColor(COLORREF) after CPictureEx::Load().
//
// I decided to leave in the class the functions to write separate frames from 
// animated GIF to disk. If you want to use them, uncomment #define GIF_TRACING 
// and an appropriate section in CPictureEx::Load(HGLOBAL, DWORD). These functions 
// won't be compiled and linked to your project unless you uncomment #define GIF_TRACING,
// so you don't have to worry.
// 
// Warning: this code hasn't been subject to a heavy testing, so
// use it on your own risk. The author accepts no liability for the 
// possible damage caused by this code.
//
// Version 1.0  7 Aug 2001
//              Initial release
//
// Version 1.1  6 Sept 2001
//              ATL version of the class
//
// Version 1.2  14 Oct 2001
//              - Fixed a problem with loading GIFs from resources
//                in MFC-version of the class for multi-modules apps.
//                Thanks to Ruben Avila-Carretero for finding this out.
//
//              - Got rid of waitable timer in ThreadAnimation()
//                Now CPictureEx[Wnd] works in Win95 too.
//                Thanks to Alex Egiazarov and Wayne King for the idea.
//
//              - Fixed a visual glitch of using SetBkColor.
//                Thanks to Kwangjin Lee for finding this out.
//
// Version 1.3  10 Nov 2001
//              - Fixed a DC leak. One DC leaked per each UnLoad()
//                (forgot to put a ReleaseDC() in the end of 
//                CPictureExWnd::PrepareDC() function).
//
//              - Now it is possible to set a clipping rectangle using
//                CPictureEx[Wnd]::SetPaintRect(const LPRECT) function.
//                The LPRECT parameter tells the class what portion of
//                a picture should it display. If the clipping rect is 
//                not set, the whole picture is shown.
//                Thanks to Fabrice Rodriguez for the idea.
//
//              - Added support for Stop/Draw. Now you can Stop() an
//                animated GIF, then Draw() it again, it will continue
//                animation from the frame it was stopped on. You can 
//                also know if a GIF is currently playing with the 
//                IsPlaying() function.
//             
//              - Got rid of math.h and made m_bExitThread volatile. 
//                Thanks to Piotr Sawicki for the suggestion.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PictureEx.h"
#include <process.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Nested structures member functions
//////////////////////////////////////////////////////////////////////

inline int CPictureEx::TGIFControlExt::GetPackedValue(enum ControlExtValues Value)
{
	int nRet = (int)m_cPacked;
	switch (Value)
	{
	case GCX_PACKED_DISPOSAL:
		nRet = (nRet & 28) >> 2;
		break;

	case GCX_PACKED_USERINPUT:
		nRet = (nRet & 2) >> 1;
		break;

	case GCX_PACKED_TRANSPCOLOR:
		nRet &= 1;
		break;
	};

	return nRet;
}

inline int CPictureEx::TGIFLSDescriptor::GetPackedValue(enum LSDPackedValues Value)
{
	int nRet = (int)m_cPacked;

	switch (Value)
	{
	case LSD_PACKED_GLOBALCT:
		nRet = nRet >> 7;
		break;

	case LSD_PACKED_CRESOLUTION:
		nRet = ((nRet & 0x70) >> 4) + 1;
		break;

	case LSD_PACKED_SORT:
		nRet = (nRet & 8) >> 3;
		break;

	case LSD_PACKED_GLOBALCTSIZE:
		nRet &= 7;
		break;
	};

	return nRet;
}

inline int CPictureEx::TGIFImageDescriptor::GetPackedValue(enum IDPackedValues Value)
{
	int nRet = (int)m_cPacked;

	switch (Value)
	{
	case ID_PACKED_LOCALCT:
		nRet >>= 7;
		break;

	case ID_PACKED_INTERLACE:
		nRet = ((nRet & 0x40) >> 6);
		break;

	case ID_PACKED_SORT:
		nRet = (nRet & 0x20) >> 5;
		break;

	case ID_PACKED_LOCALCTSIZE:
		nRet &= 7;
		break;
	};

	return nRet;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPictureEx::CPictureEx()
{
	// check structures size
	ASSERT(sizeof(TGIFImageDescriptor) == 10);
	ASSERT(sizeof(TGIFAppExtension)    == 14);
	ASSERT(sizeof(TGIFPlainTextExt)    == 15);
	ASSERT(sizeof(TGIFLSDescriptor)    ==  7);
	ASSERT(sizeof(TGIFControlExt)	   ==  8);
	ASSERT(sizeof(TGIFCommentExt)	   ==  2);
	ASSERT(sizeof(TGIFHeader)		   ==  6);

	m_pGIFLSDescriptor = NULL;
	m_pGIFHeader	   = NULL;
	m_pPicture		   = NULL;
	m_pRawData		   = NULL;
	m_hThread		   = NULL;
	m_hBitmap          = NULL;
	m_hMemDC		   = NULL;

	m_hDispMemDC       = NULL;
	m_hDispMemBM       = NULL;
	m_hDispOldBM       = NULL;

	m_bIsInitialized   = FALSE;
	m_bExitThread	   = FALSE;
	m_bIsPlaying       = FALSE;
	m_bIsGIF		   = FALSE;
	m_clrBackground    = RGB(255,255,255); // white by default
	m_nGlobalCTSize    = 0;
	m_nCurrOffset	   = 0;
	m_nCurrFrame	   = 0;
	m_nDataSize		   = 0;
	m_PictureSize.cx = m_PictureSize.cy = 0;
	SetRect(&m_PaintRect,0,0,0,0);

	m_hExitEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
}

CPictureEx::~CPictureEx()
{
	UnLoad();
	CloseHandle(m_hExitEvent);
}

BEGIN_MESSAGE_MAP(CPictureEx, CStatic)
	//{{AFX_MSG_MAP(CPictureEx)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CPictureEx::Load(HGLOBAL hGlobal, DWORD dwSize)
{
	IStream *pStream = NULL;
	UnLoad();

	if (!(m_pRawData = reinterpret_cast<unsigned char*> (GlobalLock(hGlobal))) )
	{
		TRACE(_T("Load: Error locking memory\n"));
		return FALSE;
	};

	m_nDataSize = dwSize;
	m_pGIFHeader = reinterpret_cast<TGIFHeader *> (m_pRawData);

	if ((memcmp(&m_pGIFHeader->m_cSignature,"GIF",3) != 0) &&
		((memcmp(&m_pGIFHeader->m_cVersion,"87a",3) != 0) ||
		 (memcmp(&m_pGIFHeader->m_cVersion,"89a",3) != 0)) )
	{
	// it's neither GIF87a nor GIF89a
	// do the default processing

		// clear GIF variables
		m_pRawData = NULL;
		GlobalUnlock(hGlobal);

		// don't delete memory on object's release
		if (CreateStreamOnHGlobal(hGlobal,FALSE,&pStream) != S_OK)
			return FALSE;

		if (OleLoadPicture(pStream,dwSize,FALSE,IID_IPicture,
			reinterpret_cast<LPVOID *>(&m_pPicture)) != S_OK)
		{
			pStream->Release();
			return FALSE;
		};
		pStream->Release();

		// store picture's size

		long hmWidth;
		long hmHeight;
		m_pPicture->get_Width(&hmWidth);
		m_pPicture->get_Height(&hmHeight);

		HDC hDC = ::GetDC(m_hWnd);
		m_PictureSize.cx = MulDiv(hmWidth, GetDeviceCaps(hDC,LOGPIXELSX), 2540);
		m_PictureSize.cy = MulDiv(hmHeight, GetDeviceCaps(hDC,LOGPIXELSY), 2540);
		::ReleaseDC(m_hWnd,hDC);
	}
	else
	{
		// it's a GIF
		m_bIsGIF = TRUE;
		m_pGIFLSDescriptor = reinterpret_cast<TGIFLSDescriptor *>
			(m_pRawData + sizeof(TGIFHeader));
		if (m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCT) == 1)
		{
			// calculate the globat color table size
			m_nGlobalCTSize = static_cast<int>
				(3* (1 << (m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCTSIZE)+1)));
			// get the background color if GCT is present
			unsigned char *pBkClr = m_pRawData + sizeof(TGIFHeader) + 
				sizeof(TGIFLSDescriptor) + 3*m_pGIFLSDescriptor->m_cBkIndex;
			m_clrBackground = RGB(pBkClr[0],pBkClr[1],pBkClr[2]);
		};

		// store the picture's size
		m_PictureSize.cx = m_pGIFLSDescriptor->m_wWidth;
		m_PictureSize.cy = m_pGIFLSDescriptor->m_wHeight;

		// determine frame count for this picture
		UINT nFrameCount=0;
		ResetDataPointer();
		while (SkipNextGraphicBlock())
			nFrameCount++;

#ifdef GIF_TRACING
		TRACE(
			_T(" -= GIF encountered\n"
			   "Logical Screen dimensions = %dx%d\n"
			   "Global color table = %d\n"
			   "Color depth = %d\n"
			   "Sort flag = %d\n"
			   "Size of Global Color Table = %d\n"
			   "Background color index = %d\n"
			   "Pixel aspect ratio = %d\n"
			   "Frame count = %d\n"
			   "Background color = %06Xh\n\n"
			  ),
			m_pGIFLSDescriptor->m_wWidth,
			m_pGIFLSDescriptor->m_wHeight,
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCT),
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_CRESOLUTION),
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_SORT),
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCTSIZE),
			m_pGIFLSDescriptor->m_cBkIndex,
			m_pGIFLSDescriptor->m_cPixelAspect,
			nFrameCount,
			m_clrBackground
			);
		EnumGIFBlocks();
#endif

		if (nFrameCount == 0) // it's an empty GIF!
		{
			m_pRawData = NULL;
			GlobalUnlock(hGlobal);
			return FALSE;
		};

		// now check the frame count
		// if there's only one frame, no need to animate this GIF
		// therefore, treat it like any other pic

		if (nFrameCount == 1)
		{
			// clear GIF variables
			m_pRawData = NULL;
			GlobalUnlock(hGlobal);

			// don't delete memory on object's release
			if (CreateStreamOnHGlobal(hGlobal,FALSE,&pStream) != S_OK)
				return FALSE;

			if (OleLoadPicture(pStream,dwSize,FALSE,IID_IPicture,
				(LPVOID *)&m_pPicture) != S_OK)
			{
				pStream->Release();
				return FALSE;
			};

			pStream->Release();
		}
		else
		{
		// if, on the contrary, there are several frames
		// then store separate frames in an array

			TFrame frame;
			UINT nBlockLen;
			HGLOBAL hFrameData;
			UINT nCurFrame = 0;

			ResetDataPointer();
			while (hFrameData = GetNextGraphicBlock(&nBlockLen,
				&frame.m_nDelay, &frame.m_frameSize,
				&frame.m_frameOffset, &frame.m_nDisposal) )
			{
				#ifdef GIF_TRACING
				//////////////////////////////////////////////
				// uncomment the following strings if you want 
				// to write separate frames on disk
				//
				//	CString szName;
				//	szName.Format(_T("%.4d.gif"),nCurFrame);
				//	WriteDataOnDisk(szName,hFrameData,nBlockLen);
				//	nCurFrame++;
				#endif // GIF_TRACING

				IStream *pStream = NULL;

				// delete memory on object's release
				if (CreateStreamOnHGlobal(hFrameData,TRUE,&pStream) != S_OK)
				{
					GlobalFree(hFrameData);
					continue;
				};

				if (OleLoadPicture(pStream,nBlockLen,FALSE,
					IID_IPicture,
					reinterpret_cast<LPVOID *>(&frame.m_pPicture)) != S_OK)
				{
					pStream->Release();
					continue;
				};
				pStream->Release();
			
				// everything went well, add this frame
				m_arrFrames.push_back(frame);
			};

			// clean after ourselves
			m_pRawData = NULL;
			GlobalUnlock(hGlobal);

			if (m_arrFrames.empty()) // couldn't load any frames
				return FALSE;
		};
	}; // if (!IsGIF...

	return PrepareDC(m_PictureSize.cx,m_PictureSize.cy);
}

void CPictureEx::UnLoad()
{
	Stop();
	if (m_pPicture)
	{
		m_pPicture->Release();
		m_pPicture = NULL;
	};
	
	std::vector<TFrame>::iterator it;
	for (it=m_arrFrames.begin();it<m_arrFrames.end();it++)
		(*it).m_pPicture->Release();
	m_arrFrames.clear();

	if (m_hMemDC)
	{
		SelectObject(m_hMemDC,m_hOldBitmap);
		::DeleteDC(m_hMemDC);
		::DeleteObject(m_hBitmap);
		m_hMemDC  = NULL;
		m_hBitmap = NULL;
	};

	if (m_hDispMemDC)
	{
		SelectObject(m_hDispMemDC,m_hDispOldBM);
		::DeleteDC(m_hDispMemDC);
		::DeleteObject(m_hDispMemBM);
		m_hDispMemDC  = NULL;
		m_hDispMemBM = NULL;
	};

	SetRect(&m_PaintRect,0,0,0,0);
	m_pGIFLSDescriptor = NULL;
	m_pGIFHeader	   = NULL;
	m_pRawData		   = NULL;
	m_hThread		   = NULL;
	m_bIsInitialized   = FALSE;
	m_bExitThread	   = FALSE;
	m_bIsGIF		   = FALSE;
	m_clrBackground    = RGB(255,255,255); // white by default
	m_nGlobalCTSize	   = 0;
	m_nCurrOffset	   = 0;
	m_nCurrFrame	   = 0;
	m_nDataSize		   = 0;
}

BOOL CPictureEx::Draw()
{
	if (!m_bIsInitialized)
	{
		TRACE(_T("Call one of the CPictureEx::Load() member functions before calling Draw()\n"));
		return FALSE;
	};

	if (IsAnimatedGIF())
	{
	// the picture needs animation
	// we'll start the thread that will handle it for us
	
		unsigned int nDummy;
		m_hThread = (HANDLE) _beginthreadex(NULL,0,_ThreadAnimation,this,
			CREATE_SUSPENDED,&nDummy);
		if (!m_hThread)
		{
			TRACE(_T("Draw: Couldn't start a GIF animation thread\n"));
			return FALSE;
		} 
		else 
			ResumeThread(m_hThread);
	} 
	else
	{
		if (m_pPicture)
		{
			long hmWidth;
			long hmHeight;
			m_pPicture->get_Width(&hmWidth);
			m_pPicture->get_Height(&hmHeight);
			if (m_pPicture->Render(m_hMemDC, 0, 0, m_PictureSize.cx, m_PictureSize.cy, 
				0, hmHeight, hmWidth, -hmHeight, NULL) == S_OK)
			{
				Invalidate(FALSE);
				return TRUE;
			};
		};
	};

	return FALSE;	
}

SIZE CPictureEx::GetSize() const
{
	return m_PictureSize;
}

BOOL CPictureEx::Load(LPCTSTR szFileName)
{
	ASSERT(szFileName);
	
	CFile file;
	HGLOBAL hGlobal;
	DWORD dwSize;

	if (!file.Open(szFileName,
				CFile::modeRead | 
				CFile::shareDenyWrite) )
	{
		TRACE(_T("Load (file): Error opening file %s\n"),szFileName);
		return FALSE;
	};

	dwSize = (DWORD)file.GetLength();
	hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD,dwSize);
	if (!hGlobal)
	{
		TRACE(_T("Load (file): Error allocating memory\n"));
		return FALSE;
	};
	
	char *pData = reinterpret_cast<char*>(GlobalLock(hGlobal));
	if (!pData)
	{
		TRACE(_T("Load (file): Error locking memory\n"));
		GlobalFree(hGlobal);
		return FALSE;
	};

	TRY
	{
		file.Read(pData,dwSize);
	}
	CATCH(CFileException, e);                                          
	{
		TRACE(_T("Load (file): An exception occured while reading the file %s\n"),
			szFileName);
		GlobalFree(hGlobal);
		e->Delete();
		file.Close();
		return FALSE;
	}
	END_CATCH
	GlobalUnlock(hGlobal);
	file.Close();

	BOOL bRetValue = Load(hGlobal,dwSize);
	GlobalFree(hGlobal);
	return bRetValue;
}

BOOL CPictureEx::Load(LPCTSTR szResourceName, LPCTSTR szResourceType)
{
	ASSERT(szResourceName);
	ASSERT(szResourceType);

	HRSRC hPicture = FindResource(AfxGetResourceHandle(),szResourceName,szResourceType);
	HGLOBAL hResData;
	if (!hPicture || !(hResData = LoadResource(AfxGetResourceHandle(),hPicture)))
	{
		TRACE(_T("Load (resource): Error loading resource %s\n"),szResourceName);
		return FALSE;
	};
	DWORD dwSize = SizeofResource(AfxGetResourceHandle(),hPicture);

	// hResData is not the real HGLOBAL (we can't lock it)
	// let's make it real

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD,dwSize);
	if (!hGlobal)
	{
		TRACE(_T("Load (resource): Error allocating memory\n"));
		FreeResource(hResData);
		return FALSE;
	};
	
	char *pDest = reinterpret_cast<char *> (GlobalLock(hGlobal));
	char *pSrc = reinterpret_cast<char *> (LockResource(hResData));
	if (!pSrc || !pDest)
	{
		TRACE(_T("Load (resource): Error locking memory\n"));
		GlobalFree(hGlobal);
		FreeResource(hResData);
		return FALSE;
	};
	CopyMemory(pDest,pSrc,dwSize);
	FreeResource(hResData);
	GlobalUnlock(hGlobal);

	BOOL bRetValue = Load(hGlobal,dwSize);
	GlobalFree(hGlobal);
	return bRetValue;
}

void CPictureEx::ResetDataPointer()
{
	// skip header and logical screen descriptor
	m_nCurrOffset = 
		sizeof(TGIFHeader)+sizeof(TGIFLSDescriptor)+m_nGlobalCTSize;
}

BOOL CPictureEx::SkipNextGraphicBlock()
{
	if (!m_pRawData) return FALSE;

	// GIF header + LSDescriptor [+ GCT] [+ Control block] + Data

	enum GIFBlockTypes nBlock;

	nBlock = GetNextBlock();

	while ((nBlock != BLOCK_CONTROLEXT) &&
		   (nBlock != BLOCK_IMAGE) &&
		   (nBlock != BLOCK_PLAINTEXT) &&
		   (nBlock != BLOCK_UNKNOWN) &&
		   (nBlock != BLOCK_TRAILER) )
	{
		if (!SkipNextBlock()) return NULL;
		nBlock = GetNextBlock();
	};

	if ((nBlock == BLOCK_UNKNOWN) ||
		(nBlock == BLOCK_TRAILER))
		return FALSE;

	// it's either a control ext.block, an image or a plain text

	if (GetNextBlockLen() <= 0) return FALSE;

	if (nBlock == BLOCK_CONTROLEXT)
	{
		if (!SkipNextBlock()) return FALSE;
		nBlock = GetNextBlock();

		// skip everything until we meet an image block or a plain-text block
		while ((nBlock != BLOCK_IMAGE) &&
			   (nBlock != BLOCK_PLAINTEXT) &&
			   (nBlock != BLOCK_UNKNOWN) &&
			   (nBlock != BLOCK_TRAILER) )
		{
			if (!SkipNextBlock()) return NULL;
			nBlock = GetNextBlock();
		};

		if ((nBlock == BLOCK_UNKNOWN) ||
			(nBlock == BLOCK_TRAILER))
			return FALSE;
	};

	// skip the found data block (image or plain-text)
	if (!SkipNextBlock()) return FALSE;

	return TRUE;
}

UINT CPictureEx::GetSubBlocksLen(UINT nStartingOffset) const
{
	UINT nRet = 0;
	UINT nCurOffset = nStartingOffset;
	
	while (m_pRawData[nCurOffset] != 0)
	{
		nRet += m_pRawData[nCurOffset]+1;
		nCurOffset += m_pRawData[nCurOffset]+1;
	};

	return nRet+1;
}

enum CPictureEx::GIFBlockTypes CPictureEx::GetNextBlock() const
{
	switch(m_pRawData[m_nCurrOffset])
	{
	case 0x21:
	// extension block
		switch(m_pRawData[m_nCurrOffset+1])
		{
		case 0x01:
		// plain text extension
			return BLOCK_PLAINTEXT;
			break;

		case 0xF9:
		// graphic control extension
			return BLOCK_CONTROLEXT;
			break;

		case 0xFE:
		// comment extension
			return BLOCK_COMMEXT;
			break;

		case 0xFF:
		// application extension
			return BLOCK_APPEXT;
			break;
		};
		break;
	
	case 0x3B:
	// trailer
		return BLOCK_TRAILER;
		break;

	case 0x2C:
	// image data
		return BLOCK_IMAGE;
		break;
	};

	return BLOCK_UNKNOWN;
}

BOOL CPictureEx::SkipNextBlock()
{
	if (!m_pRawData) return FALSE;

	int nLen = GetNextBlockLen();
	if ((nLen <= 0) || ((m_nCurrOffset+nLen) > m_nDataSize))
		return FALSE;

	m_nCurrOffset += nLen;
	return TRUE;
}

int CPictureEx::GetNextBlockLen() const
{
	GIFBlockTypes nBlock = GetNextBlock();

	int nTmp;

	switch(nBlock)
	{
	case BLOCK_UNKNOWN:
		return -1;
		break;

	case BLOCK_TRAILER:
		return 1;
		break;

	case BLOCK_APPEXT:
		nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFAppExtension));
		if (nTmp > 0)
			return sizeof(TGIFAppExtension)+nTmp;
		break;

	case BLOCK_COMMEXT:
		nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFCommentExt));
		if (nTmp > 0)
			return sizeof(TGIFCommentExt)+nTmp;
		break;

	case BLOCK_CONTROLEXT:
		return sizeof(TGIFControlExt);
		break;

	case BLOCK_PLAINTEXT:
		nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFPlainTextExt));
		if (nTmp > 0)
			return sizeof(TGIFPlainTextExt)+nTmp;
		break;

	case BLOCK_IMAGE:
		TGIFImageDescriptor *pIDescr = 
			reinterpret_cast<TGIFImageDescriptor *> (&m_pRawData[m_nCurrOffset]);
		int nLCTSize = (int)
			(pIDescr->GetPackedValue(ID_PACKED_LOCALCT)*3*
			(1 << (pIDescr->GetPackedValue(ID_PACKED_LOCALCTSIZE)+1)));

		int nTmp = GetSubBlocksLen(m_nCurrOffset+
			sizeof(TGIFImageDescriptor) + nLCTSize + 1);
		if (nTmp > 0)
			return sizeof(TGIFImageDescriptor) + nLCTSize + 1 + nTmp;
		break;
	};

	return 0;
}

UINT WINAPI CPictureEx::_ThreadAnimation(LPVOID pParam)
{
	ASSERT(pParam);
	CPictureEx *pPic = reinterpret_cast<CPictureEx *> (pParam);

	pPic->m_bIsPlaying = TRUE;
	pPic->ThreadAnimation();
	pPic->m_bIsPlaying = FALSE;

	// this thread has finished its work so we close the handle
	CloseHandle(pPic->m_hThread); 
	// and init the handle to zero (so that Stop() doesn't Wait on it)
	pPic->m_hThread = 0;
	return 0;
}

void CPictureEx::ThreadAnimation()
{
	// first, restore background (for stop/draw support)
	// disposal method #2
	if (m_arrFrames[m_nCurrFrame].m_nDisposal == 2)
	{
		HBRUSH hBrush = CreateSolidBrush(m_clrBackground);
		if (hBrush)
		{
			RECT rect = {
				m_arrFrames[m_nCurrFrame].m_frameOffset.cx,
				m_arrFrames[m_nCurrFrame].m_frameOffset.cy,
				m_arrFrames[m_nCurrFrame].m_frameOffset.cx + m_arrFrames[m_nCurrFrame].m_frameSize.cx,
				m_arrFrames[m_nCurrFrame].m_frameOffset.cy + m_arrFrames[m_nCurrFrame].m_frameSize.cy };
			FillRect(m_hMemDC,&rect,hBrush);
			DeleteObject(hBrush);
		};
	} 
	else
		// disposal method #3
		if (m_hDispMemDC && (m_arrFrames[m_nCurrFrame].m_nDisposal == 3) )
		{
			// put it back
			BitBlt(m_hMemDC,
				m_arrFrames[m_nCurrFrame].m_frameOffset.cx,
				m_arrFrames[m_nCurrFrame].m_frameOffset.cy,
				m_arrFrames[m_nCurrFrame].m_frameSize.cx,
				m_arrFrames[m_nCurrFrame].m_frameSize.cy,
				m_hDispMemDC,0,0, SRCCOPY);
			// init variables
			SelectObject(m_hDispMemDC,m_hDispOldBM);
			DeleteDC(m_hDispMemDC); m_hDispMemDC = NULL;
			DeleteObject(m_hDispMemBM); m_hDispMemBM = NULL;
		};

	while (!m_bExitThread)
	{
		if (m_arrFrames[m_nCurrFrame].m_pPicture)
		{
		///////////////////////////////////////////////////////
		// Before rendering a frame we should take care of what's 
		// behind that frame. TFrame::m_nDisposal will be our guide:
		//   0 - no disposal specified (do nothing)
		//   1 - do not dispose (again, do nothing)
		//   2 - restore to background color (m_clrBackground)
		//   3 - restore to previous

			//////// disposal method #3
			if (m_arrFrames[m_nCurrFrame].m_nDisposal == 3)
			{
				// prepare a memory DC and store the background in it
				m_hDispMemDC = CreateCompatibleDC(m_hMemDC);
				m_hDispMemBM = CreateCompatibleBitmap(m_hMemDC,
							m_arrFrames[m_nCurrFrame].m_frameSize.cx,
							m_arrFrames[m_nCurrFrame].m_frameSize.cy);
				
				if (m_hDispMemDC && m_hDispMemBM)
				{
					m_hDispOldBM = reinterpret_cast<HBITMAP> (SelectObject(m_hDispMemDC,m_hDispMemBM));
					BitBlt(m_hDispMemDC,0,0,
						m_arrFrames[m_nCurrFrame].m_frameSize.cx,
						m_arrFrames[m_nCurrFrame].m_frameSize.cy,
						m_hMemDC,
						m_arrFrames[m_nCurrFrame].m_frameOffset.cx,
						m_arrFrames[m_nCurrFrame].m_frameOffset.cy,
						SRCCOPY);
				};
			};
			///////////////////////

			long hmWidth;
			long hmHeight;
			m_arrFrames[m_nCurrFrame].m_pPicture->get_Width(&hmWidth);
			m_arrFrames[m_nCurrFrame].m_pPicture->get_Height(&hmHeight);

			if (m_arrFrames[m_nCurrFrame].m_pPicture->Render(m_hMemDC, 
				m_arrFrames[m_nCurrFrame].m_frameOffset.cx, 
				m_arrFrames[m_nCurrFrame].m_frameOffset.cy, 
				m_arrFrames[m_nCurrFrame].m_frameSize.cx, 
				m_arrFrames[m_nCurrFrame].m_frameSize.cy, 
				0, hmHeight, hmWidth, -hmHeight, NULL) == S_OK)
			{
				Invalidate(FALSE);
			};
			
			if (m_bExitThread) break;

			// if the delay time is too short (like in old GIFs), wait for 100ms
			if (m_arrFrames[m_nCurrFrame].m_nDelay < 5) 
				WaitForSingleObject(m_hExitEvent, 100);
			else
				WaitForSingleObject(m_hExitEvent, 10*m_arrFrames[m_nCurrFrame].m_nDelay);

			if (m_bExitThread) break;

			// disposal method #2
			if (m_arrFrames[m_nCurrFrame].m_nDisposal == 2)
			{
				HBRUSH hBrush = CreateSolidBrush(m_clrBackground);
				if (hBrush)
				{
					RECT rect = {
						m_arrFrames[m_nCurrFrame].m_frameOffset.cx,
						m_arrFrames[m_nCurrFrame].m_frameOffset.cy,
						m_arrFrames[m_nCurrFrame].m_frameOffset.cx + m_arrFrames[m_nCurrFrame].m_frameSize.cx,
						m_arrFrames[m_nCurrFrame].m_frameOffset.cy + m_arrFrames[m_nCurrFrame].m_frameSize.cy };
					FillRect(m_hMemDC,&rect,hBrush);
					DeleteObject(hBrush);
				};
			} 
			else
				if (m_hDispMemDC && (m_arrFrames[m_nCurrFrame].m_nDisposal == 3) )
				{
					// put it back
					BitBlt(m_hMemDC,
						m_arrFrames[m_nCurrFrame].m_frameOffset.cx,
						m_arrFrames[m_nCurrFrame].m_frameOffset.cy,
						m_arrFrames[m_nCurrFrame].m_frameSize.cx,
						m_arrFrames[m_nCurrFrame].m_frameSize.cy,
						m_hDispMemDC,0,0, SRCCOPY);
					// init variables
					SelectObject(m_hDispMemDC,m_hDispOldBM);
					DeleteDC(m_hDispMemDC); m_hDispMemDC = NULL;
					DeleteObject(m_hDispMemBM); m_hDispMemBM = NULL;
				};
		};
		m_nCurrFrame++;
		if (m_nCurrFrame == m_arrFrames.size())
		{
			m_nCurrFrame
				= 0; 
		// init the screen for the first frame,
			HBRUSH hBrush = CreateSolidBrush(m_clrBackground);
			if (hBrush)
			{
				RECT rect = {0,0,m_PictureSize.cx,m_PictureSize.cy};
				FillRect(m_hMemDC,&rect,hBrush);
				DeleteObject(hBrush);
			};
		};
	};
}

void CPictureEx::Stop()
{
	m_bIsPlaying = FALSE;
	m_bExitThread = TRUE;
	SetEvent(m_hExitEvent);
	if (m_hThread)
	{
		// we'll wait for 5 seconds then continue execution
		WaitForSingleObject(m_hThread,5000);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	// make it possible to Draw() again
	ResetEvent(m_hExitEvent);
	m_bExitThread = FALSE;
}

HGLOBAL CPictureEx::GetNextGraphicBlock(UINT *pBlockLen, 
	UINT *pDelay, SIZE *pBlockSize, SIZE *pBlockOffset, 
	UINT *pDisposal)
{
	if (!m_pRawData) return NULL;

	// GIF header + LSDescriptor [+ GCT] [+ Control block] + Data

	*pDisposal = 0;
	enum GIFBlockTypes nBlock;
	nBlock = GetNextBlock();

	while (
		(nBlock != BLOCK_CONTROLEXT) &&
		(nBlock != BLOCK_IMAGE) &&
		(nBlock != BLOCK_PLAINTEXT) &&
		(nBlock != BLOCK_UNKNOWN) &&
		(nBlock != BLOCK_TRAILER)
		)
	{
		if (!SkipNextBlock()) return NULL;
		nBlock = GetNextBlock();
	};

	if ((nBlock == BLOCK_UNKNOWN) ||
		(nBlock == BLOCK_TRAILER))
		return NULL;

	// it's either a control ext.block, an image or a plain text

	int nStart = m_nCurrOffset;
	int nBlockLen = GetNextBlockLen();

	if (nBlockLen <= 0) return NULL;

	if (nBlock == BLOCK_CONTROLEXT)
	{
		// get the following data
		TGIFControlExt *pControl = 
			reinterpret_cast<TGIFControlExt *> (&m_pRawData[m_nCurrOffset]);
		// store delay time
		*pDelay = pControl->m_wDelayTime;
		// store disposal method
		*pDisposal = pControl->GetPackedValue(GCX_PACKED_DISPOSAL);

		if (!SkipNextBlock()) return NULL;
		nBlock = GetNextBlock();
		
		// skip everything until we find data to display 
		// (image block or plain-text block)
		
		while (
			(nBlock != BLOCK_IMAGE) &&
			(nBlock != BLOCK_PLAINTEXT) &&
			(nBlock != BLOCK_UNKNOWN) &&
			(nBlock != BLOCK_TRAILER)
			)
		{
			if (!SkipNextBlock()) return NULL;
			nBlock = GetNextBlock();
			nBlockLen += GetNextBlockLen();
		};

		if ((nBlock == BLOCK_UNKNOWN) || (nBlock == BLOCK_TRAILER))
			return NULL;
		nBlockLen += GetNextBlockLen();
	}
	else
		*pDelay = -1; // to indicate that there was no delay value

	if (nBlock == BLOCK_IMAGE)
	{
		// store size and offsets
		TGIFImageDescriptor *pImage = 
			reinterpret_cast<TGIFImageDescriptor *> (&m_pRawData[m_nCurrOffset]);
		pBlockSize->cx = pImage->m_wWidth;
		pBlockSize->cy = pImage->m_wHeight;
		pBlockOffset->cx = pImage->m_wLeftPos;
		pBlockOffset->cy = pImage->m_wTopPos;
	};

	if (!SkipNextBlock()) return NULL;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED,
		sizeof(TGIFHeader) +
		sizeof(TGIFLSDescriptor) +
		m_nGlobalCTSize +
		nBlockLen + 
		1);  // for the trailer

	if (!hGlobal) return NULL;

	int nOffset = 0; 

	// GMEM_FIXED means we get a pointer
	unsigned char *pGlobal = reinterpret_cast<unsigned char *> (hGlobal);

	CopyMemory(pGlobal,m_pRawData, 
		sizeof(TGIFHeader)+sizeof(TGIFLSDescriptor)+m_nGlobalCTSize);
	nOffset += sizeof(TGIFHeader)+sizeof(TGIFLSDescriptor)+m_nGlobalCTSize;

	CopyMemory(pGlobal + nOffset,&m_pRawData[nStart], nBlockLen);
	nOffset += nBlockLen;

	pGlobal[nOffset] = 0x3B; // trailer
	nOffset++;

	*pBlockLen = nOffset;

	return hGlobal;
}

BOOL CPictureEx::IsGIF() const
{
	return m_bIsGIF;
}

BOOL CPictureEx::IsAnimatedGIF() const
{
	return (m_bIsGIF && (m_arrFrames.size() > 1));
}

BOOL CPictureEx::IsPlaying() const
{
	return m_bIsPlaying;
}

int CPictureEx::GetFrameCount() const
{
	if (!IsAnimatedGIF())
		return 0;

	return m_arrFrames.size();
}

COLORREF CPictureEx::GetBkColor() const
{
	return m_clrBackground;
}

void CPictureEx::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	LONG nPaintWidth = m_PaintRect.right-m_PaintRect.left;

	if (nPaintWidth > 0)
	{
		LONG nPaintHeight = m_PaintRect.bottom - m_PaintRect.top;
		::BitBlt(dc.m_hDC, 0, 0, nPaintWidth, nPaintHeight,	
			m_hMemDC, m_PaintRect.left, m_PaintRect.top, SRCCOPY);
	}
	else
	{
		::BitBlt(dc.m_hDC, 0, 0, m_PictureSize.cx, m_PictureSize.cy,
			m_hMemDC, 0, 0, SRCCOPY);
	};
}

BOOL CPictureEx::PrepareDC(int nWidth, int nHeight)
{
	SetWindowPos(NULL,0,0,nWidth,nHeight,SWP_NOMOVE | SWP_NOZORDER);

	HDC hWinDC = ::GetDC(m_hWnd);
	if (!hWinDC) return FALSE;
	
	m_hMemDC = CreateCompatibleDC(hWinDC);
	if (!m_hMemDC) 
	{
		::ReleaseDC(m_hWnd,hWinDC);
		return FALSE;
	};

	m_hBitmap  = CreateCompatibleBitmap(hWinDC,nWidth,nHeight);
	if (!m_hBitmap) 
	{
		::ReleaseDC(m_hWnd,hWinDC);
		::DeleteDC(m_hMemDC);
		return FALSE;
	};

	m_hOldBitmap = reinterpret_cast<HBITMAP> 
						(SelectObject(m_hMemDC,m_hBitmap));
	
	// fill the background
	m_clrBackground = GetSysColor(COLOR_3DFACE);
	RECT rect = {0,0,nWidth,nHeight};
	FillRect(m_hMemDC,&rect,(HBRUSH)(COLOR_WINDOW));

	::ReleaseDC(m_hWnd,hWinDC);
	m_bIsInitialized = TRUE;
	return TRUE;
}

void CPictureEx::OnDestroy() 
{
	Stop();	
	CStatic::OnDestroy();
}

void CPictureEx::SetBkColor(COLORREF clr)
{
	if (!m_bIsInitialized) return;

	m_clrBackground = clr;

	HBRUSH hBrush = CreateSolidBrush(clr);
	if (hBrush)
	{
		RECT rect = {0,0,m_PictureSize.cx,m_PictureSize.cy};
		FillRect(m_hMemDC,&rect,hBrush);
		DeleteObject(hBrush);
	};
}

#ifdef GIF_TRACING
void CPictureEx::WriteDataOnDisk(CString szFileName, HGLOBAL hData, DWORD dwSize)
{
	CFile file;

	if (!file.Open(szFileName,
			CFile::modeCreate |
			CFile::modeWrite |
			CFile::shareDenyNone))
	{
		TRACE(_T("WriteData: Error creating file %s\n"),szFileName);
		return;
	};

	char *pData = reinterpret_cast<char *> (GlobalLock(hData));
	if (!pData)
	{
		TRACE(_T("WriteData: Error locking memory\n"));
		return;
	};

	TRY
	{
		file.Write(pData,dwSize);
	}
	CATCH(CFileException, e);                                          
	{
		TRACE(_T("WriteData: An exception occured while writing to the file %s\n"),
			szFileName);
		e->Delete();
		GlobalUnlock(hData);
		file.Close();
		return;
	}
	END_CATCH
	
	GlobalUnlock(hData);
	file.Close();
}

void CPictureEx::EnumGIFBlocks()
{
	enum GIFBlockTypes nBlock;

	ResetDataPointer();
	while(m_nCurrOffset < m_nDataSize)
	{
		nBlock = GetNextBlock();
		switch(nBlock)
		{
		case BLOCK_UNKNOWN:
			TRACE(_T("- Unknown block\n"));
			return;
			break;

		case BLOCK_TRAILER:
			TRACE(_T("- Trailer block\n"));
			break;

		case BLOCK_APPEXT:
			TRACE(_T("- Application extension block\n"));
			break;

		case BLOCK_COMMEXT:
			TRACE(_T("- Comment extension block\n"));
			break;

		case BLOCK_CONTROLEXT:
			{
			TGIFControlExt *pControl = 
				reinterpret_cast<TGIFControlExt *> (&m_pRawData[m_nCurrOffset]);
			TRACE(_T("- Graphic control extension block (delay %d, disposal %d)\n"),
					pControl->m_wDelayTime, pControl->GetPackedValue(GCX_PACKED_DISPOSAL));
			};
			break;

		case BLOCK_PLAINTEXT:
			TRACE(_T("- Plain text extension block\n"));
			break;

		case BLOCK_IMAGE:
			TGIFImageDescriptor *pIDescr = 
				reinterpret_cast<TGIFImageDescriptor *> (&m_pRawData[m_nCurrOffset]);
			TRACE(_T("- Image data block (%dx%d  %d,%d)\n"),
					pIDescr->m_wWidth,
					pIDescr->m_wHeight,
					pIDescr->m_wLeftPos,
					pIDescr->m_wTopPos);
			break;
		};

		SkipNextBlock();	
	};

	TRACE(_T("\n"));
}
#endif // GIF_TRACING

BOOL CPictureEx::SetPaintRect(const RECT *lpRect)
{
	return CopyRect(&m_PaintRect, lpRect);
}

BOOL CPictureEx::GetPaintRect(RECT *lpRect)
{
	return CopyRect(lpRect, &m_PaintRect);
}
