// Icons.h: interface for the CIcons class.
//////////////////////////////////////////////////////////////////////
//
//用途：IconSnap所需调用的类
//功能：图标基础操作类
//作者：徐景周
//日期：2001年9月
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICONS_H__22E22BB7_2BD4_42EB_B682_EAFB6A753BAC__INCLUDED_)
#define AFX_ICONS_H__22E22BB7_2BD4_42EB_B682_EAFB6A753BAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/****************************************************************************/
// These first two structs represent how the icon information is stored
// when it is bound into a EXE or DLL file. Structure members are WORD
// aligned and the last member of the structure is the ID instead of
// the imageoffset.
#pragma pack( push )
#pragma pack( 2 )
typedef struct
{
	BYTE	bWidth;               // Width of the image
	BYTE	bHeight;              // Height of the image (times 2)
	BYTE	bColorCount;          // Number of colors in image (0 if >=8bpp)
	BYTE	bReserved;            // Reserved
	WORD	wPlanes;              // Color Planes
	WORD	wBitCount;            // Bits per pixel
	DWORD	dwBytesInRes;         // how many bytes in this resource?
	WORD	nID;                  // the ID
} MEMICONDIRENTRY, *LPMEMICONDIRENTRY;
typedef struct 
{
	WORD			idReserved;   // Reserved
	WORD			idType;       // resource type (1 for icons)
	WORD			idCount;      // how many images?
	MEMICONDIRENTRY	idEntries[1]; // the entries for each image
} MEMICONDIR, *LPMEMICONDIR;
#pragma pack( pop )

// These next two structs represent how the icon information is stored
// in an ICO file.
typedef struct
{
	BYTE	bWidth;               // Width of the image
	BYTE	bHeight;              // Height of the image (times 2)
	BYTE	bColorCount;          // Number of colors in image (0 if >=8bpp)
	BYTE	bReserved;            // Reserved
	WORD	wPlanes;              // Color Planes
	WORD	wBitCount;            // Bits per pixel
	DWORD	dwBytesInRes;         // how many bytes in this resource?
	DWORD	dwImageOffset;        // where in the file is this image
} ICONDIRENTRY, *LPICONDIRENTRY;
typedef struct 
{
	WORD			idReserved;   // Reserved
	WORD			idType;       // resource type (1 for icons)
	WORD			idCount;      // how many images?
	ICONDIRENTRY	idEntries[1]; // the entries for each image
} ICONDIR, *LPICONDIR;


// The following two structs are for the use of this program in
// manipulating icons. They are more closely tied to the operation
// of this program than the structures listed above. One of the
// main differences is that they provide a pointer to the DIB
// information of the masks.
typedef struct
{
	UINT			Width, Height, Colors; // Width, Height and bpp
	LPBYTE			lpBits;                // ptr to DIB bits
	DWORD			dwNumBytes;            // how many bytes?
	LPBITMAPINFO	lpbi;                  // ptr to header
	LPBYTE			lpXOR;                 // ptr to XOR image bits
	LPBYTE			lpAND;                 // ptr to AND image bits
} ICONIMAGE, *LPICONIMAGE;
typedef struct
{
	BOOL		bHasChanged;                     // Has image changed?
	TCHAR		szOriginalICOFileName[MAX_PATH]; // Original name
	TCHAR		szOriginalDLLFileName[MAX_PATH]; // Original name
	UINT		nNumImages;                      // How many images?
	ICONIMAGE	IconImages[1];                   // Image entries
} ICONRESOURCE, *LPICONRESOURCE;
/****************************************************************************/

/****************************************************************************/
// Resource Position info - size and offset of a resource in a file
typedef struct
{
    DWORD	dwBytes;
    DWORD	dwOffset;
} RESOURCEPOSINFO, *LPRESOURCEPOSINFO;
// EXE/DLL icon information - filename, instance handle and ID
typedef struct
{
    LPCTSTR    	szFileName;
    HINSTANCE	hInstance;
    LPTSTR    	lpID;
} EXEDLLICONINFO, *LPEXEDLLICONINFO;
/****************************************************************************/

#define MAX_ICON_WIDTH	128              // Max width
#define MIN_ICON_WIDTH	16               // Min width
#define MAX_ICON_HEIGHT	MAX_ICON_WIDTH   // Max height
#define MIN_ICON_HEIGHT	MIN_ICON_WIDTH   // Min height

class CIcons  
{
public:
	CIcons();
	virtual ~CIcons();

public:
	LPICONRESOURCE ReadIconFromICOFile( LPCTSTR szFileName );
	BOOL WriteIconToICOFile( LPICONRESOURCE lpIR, LPCTSTR szFileName );
	HICON MakeIconFromResource( LPICONIMAGE lpIcon );
	HICON GetIconFromInstance( HINSTANCE hInstance, LPTSTR nIndex );
	LPICONRESOURCE ReadIconFromEXEFile( LPCTSTR szFileName, LPTSTR lpID);
	BOOL IconImageToClipBoard( LPICONIMAGE lpii );
	BOOL IconImageFromClipBoard( LPICONIMAGE lpii, BOOL bStretchToFit );
	BOOL IconImageFromBMPFile( LPCTSTR szFileName, LPICONIMAGE lpii, BOOL bStretchToFit );
	BOOL IconImageToBMPFile( LPCTSTR szFileName, LPICONIMAGE lpii );

private:
	UINT ReadICOHeader( HANDLE hFile );
	BOOL AdjustIconImagePointers( LPICONIMAGE lpImage );
	BOOL WriteICOHeader( HANDLE hFile, UINT nNumEntries );
	DWORD CalculateImageOffset( LPICONRESOURCE lpIR, UINT nIndex );
	BOOL DIBToIconImage( LPICONIMAGE lpii, LPBYTE lpDIB, BOOL bStretch );

};

#endif // !defined(AFX_ICONS_H__22E22BB7_2BD4_42EB_B682_EAFB6A753BAC__INCLUDED_)
