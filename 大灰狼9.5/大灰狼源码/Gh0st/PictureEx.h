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

#if !defined(AFX_PICTUREEX_H__0EFE5DE0_7B68_4DB7_8B34_5DC634948438__INCLUDED_)
#define AFX_PICTUREEX_H__0EFE5DE0_7B68_4DB7_8B34_5DC634948438__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

//#define GIF_TRACING  // uncomment it if you want detailed TRACEs

class CPictureEx : public CStatic
{
public:

struct TFrame    // structure that keeps a single frame info
{
	IPicture *m_pPicture;  // pointer to the interface used for drawing
	SIZE     m_frameSize;
	SIZE     m_frameOffset;
	UINT     m_nDelay;     // delay (in 1/100s of a second)
	UINT     m_nDisposal;  // disposal method
};

#pragma pack(1)   // turn byte alignment on

enum GIFBlockTypes
{
	BLOCK_UNKNOWN,
	BLOCK_APPEXT,
	BLOCK_COMMEXT,
	BLOCK_CONTROLEXT,
	BLOCK_PLAINTEXT,
	BLOCK_IMAGE,
	BLOCK_TRAILER
};

enum ControlExtValues // graphic control extension packed field values
{
	GCX_PACKED_DISPOSAL,  // disposal method
	GCX_PACKED_USERINPUT,
	GCX_PACKED_TRANSPCOLOR
};

enum LSDPackedValues  // logical screen descriptor packed field values
{
	LSD_PACKED_GLOBALCT,
	LSD_PACKED_CRESOLUTION,
	LSD_PACKED_SORT,
	LSD_PACKED_GLOBALCTSIZE
};

enum IDPackedValues   // image descriptor packed field values
{
	ID_PACKED_LOCALCT,
	ID_PACKED_INTERLACE,
	ID_PACKED_SORT,
	ID_PACKED_LOCALCTSIZE
};

struct TGIFHeader       // GIF header  
{
	char m_cSignature[3]; // Signature - Identifies the GIF Data Stream
						  // This field contains the fixed value 'GIF'
	char m_cVersion[3];	// Version number. May be one of the following:
						// "87a" or "89a"
};

struct TGIFLSDescriptor // Logical Screen Descriptor
{
	WORD m_wWidth;	// 2 bytes. Logical screen width
	WORD m_wHeight; // 2 bytes. Logical screen height

	unsigned char m_cPacked;      // packed field	

	unsigned char m_cBkIndex;     // 1 byte. Background color index
	unsigned char m_cPixelAspect; // 1 byte. Pixel aspect ratio
	inline int GetPackedValue(enum LSDPackedValues Value);
};

struct TGIFAppExtension // application extension block
{
	unsigned char m_cExtIntroducer; // extension introducer (0x21)
	unsigned char m_cExtLabel; // app. extension label (0xFF)
	unsigned char m_cBlockSize; // fixed value of 11
	char m_cAppIdentifier[8];   // application identifier
	char m_cAppAuth[3];  // application authentication code
};

struct TGIFControlExt // graphic control extension block
{
	unsigned char m_cExtIntroducer; // extension introducer (0x21)
	unsigned char m_cControlLabel;  // control extension label (0xF9)
	unsigned char m_cBlockSize; // fixed value of 4
	unsigned char m_cPacked;    // packed field
	WORD m_wDelayTime;	// delay time
	unsigned char m_cTColorIndex; // transparent color index
	unsigned char m_cBlockTerm;   // block terminator (0x00)
public:
	inline int GetPackedValue(enum ControlExtValues Value);
};

struct TGIFCommentExt  // comment extension block
{
	unsigned char m_cExtIntroducer; // extension introducer (0x21)
	unsigned char m_cCommentLabel;  // comment extension label (0xFE)
};

struct TGIFPlainTextExt // plain text extension block
{
	unsigned char m_cExtIntroducer;  // extension introducer (0x21)
	unsigned char m_cPlainTextLabel; // text extension label (0x01)
	unsigned char m_cBlockSize; // fixed value of 12
	WORD m_wLeftPos;    // text grid left position
	WORD m_wTopPos;     // text grid top position
	WORD m_wGridWidth;  // text grid width
	WORD m_wGridHeight; // text grid height
	unsigned char m_cCellWidth;  // character cell width
	unsigned char m_cCellHeight; // character cell height
	unsigned char m_cFgColor; // text foreground color index
	unsigned char m_cBkColor; // text background color index
};

struct TGIFImageDescriptor // image descriptor block
{
	unsigned char m_cImageSeparator; // image separator byte (0x2C)
	WORD m_wLeftPos; // image left position
	WORD m_wTopPos;  // image top position
	WORD m_wWidth;   // image width
	WORD m_wHeight;  // image height
	unsigned char m_cPacked; // packed field
	inline int GetPackedValue(enum IDPackedValues Value);
};

#pragma pack() // turn byte alignment off

public:
	BOOL GetPaintRect(RECT *lpRect);
	BOOL SetPaintRect(const RECT *lpRect);
	CPictureEx();
	virtual ~CPictureEx();
	void Stop();   // stops animation
	void UnLoad(); // stops animation plus releases all resources

	BOOL IsGIF() const;
	BOOL IsPlaying() const;
	BOOL IsAnimatedGIF() const;
	SIZE GetSize() const;
	int GetFrameCount() const;
	COLORREF GetBkColor() const;
	void SetBkColor(COLORREF clr);

	// draws the picture (starts an animation thread if needed)
	// if an animation was previously stopped by Stop(),
	// continues it from the last displayed frame
	BOOL Draw();

	// loads a picture from a file
	// i.e. Load(_T("mypic.gif"));
	BOOL Load(LPCTSTR szFileName);

	// loads a picture from a global memory block (allocated by GlobalAlloc)
	// Warning: this function DOES NOT free the global memory, pointed to by hGlobal
	BOOL Load(HGLOBAL hGlobal, DWORD dwSize);

	// loads a picture from a program resource
	// i.e. Load(MAKEINTRESOURCE(IDR_MYPIC),_T("GIFTYPE"));
	BOOL Load(LPCTSTR szResourceName,LPCTSTR szResourceType);

protected:

#ifdef GIF_TRACING
	void EnumGIFBlocks();
	void WriteDataOnDisk(CString szFileName, HGLOBAL hData, DWORD dwSize);
#endif // GIF_TRACING

	RECT m_PaintRect;
	SIZE m_PictureSize;
	COLORREF m_clrBackground;
	UINT m_nCurrFrame;
	UINT m_nDataSize;
	UINT m_nCurrOffset;
	UINT m_nGlobalCTSize;
	BOOL m_bIsGIF;
	BOOL m_bIsPlaying;
	volatile BOOL m_bExitThread;
	BOOL m_bIsInitialized;
	HDC m_hMemDC;

	HDC m_hDispMemDC;
	HBITMAP m_hDispMemBM;
	HBITMAP m_hDispOldBM;

	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;
	HANDLE m_hThread;
	HANDLE m_hExitEvent;
	IPicture * m_pPicture;
	TGIFHeader * m_pGIFHeader;
	unsigned char * m_pRawData;
	TGIFLSDescriptor * m_pGIFLSDescriptor;
	std::vector<TFrame> m_arrFrames;

	void ThreadAnimation();
	static UINT WINAPI _ThreadAnimation(LPVOID pParam);

	int GetNextBlockLen() const;
	BOOL SkipNextBlock();
	BOOL SkipNextGraphicBlock();
	BOOL PrepareDC(int nWidth, int nHeight);
	void ResetDataPointer();
	enum GIFBlockTypes GetNextBlock() const;
	UINT GetSubBlocksLen(UINT nStartingOffset) const;
	HGLOBAL GetNextGraphicBlock(UINT *pBlockLen, UINT *pDelay, 
		SIZE *pBlockSize, SIZE *pBlockOffset, UINT *pDisposal);

	// Generated message map functions
	//{{AFX_MSG(CPictureEx)
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_PICTUREEX_H__0EFE5DE0_7B68_4DB7_8B34_5DC634948438__INCLUDED_)
