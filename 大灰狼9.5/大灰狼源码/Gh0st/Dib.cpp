// Dib.cpp: implementation of the CDib class.
//////////////////////////////////////////////////////////////////////
//
//用途：IconSnap所需调用的类
//功能：DIB设备无关位图基础操作类
//作者：徐景周
//日期：2001年9月
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Dib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDib::CDib()
{

}

CDib::~CDib()
{

}

/****************************************************************************
*
*     FUNCTION: FindDIBits
*
*     PURPOSE:  Locate the image bits in a CF_DIB format DIB.
*
*     PARAMS:   LPSTR lpbi - pointer to the CF_DIB memory block
*
*     RETURNS:  LPSTR - pointer to the image bits
*
*
\****************************************************************************/
LPSTR CDib::FindDIBBits( LPSTR lpbi )
{
   return ( lpbi + *(LPDWORD)lpbi + PaletteSize( lpbi ) );
}
/* End FindDIBits() *********************************************************/

/****************************************************************************
*
*     FUNCTION: DIBNumColors
*
*     PURPOSE:  Calculates the number of entries in the color table.
*
*     PARAMS:   LPSTR lpbi - pointer to the CF_DIB memory block
*
*     RETURNS:  WORD - Number of entries in the color table.
*
*
\****************************************************************************/
WORD CDib::DIBNumColors( LPSTR lpbi )
{
    WORD wBitCount;
    DWORD dwClrUsed;

    dwClrUsed = ((LPBITMAPINFOHEADER) lpbi)->biClrUsed;

    if (dwClrUsed)
        return (WORD) dwClrUsed;

    wBitCount = ((LPBITMAPINFOHEADER) lpbi)->biBitCount;

    switch (wBitCount)
    {
        case 1: return 2;
        case 4: return 16;
        case 8:	return 256;
        default:return 0;
    }
    return 0;
}
/* End DIBNumColors() ******************************************************/

/****************************************************************************
*
*     FUNCTION: PaletteSize
*
*     PURPOSE:  Calculates the number of bytes in the color table.
*
*     PARAMS:   LPSTR lpbi - pointer to the CF_DIB memory block
*
*     RETURNS:  WORD - number of bytes in the color table
*
*
\****************************************************************************/
WORD CDib::PaletteSize( LPSTR lpbi )
{
    return ( DIBNumColors( lpbi ) * sizeof( RGBQUAD ) );
}
/* End PaletteSize() ********************************************************/



/****************************************************************************
*
*     FUNCTION: BytesPerLine
*
*     PURPOSE:  Calculates the number of bytes in one scan line.
*
*     PARAMS:   LPBITMAPINFOHEADER lpBMIH - pointer to the BITMAPINFOHEADER
*                                           that begins the CF_DIB block
*
*     RETURNS:  DWORD - number of bytes in one scan line (DWORD aligned)
*
*
\****************************************************************************/
DWORD CDib::BytesPerLine( LPBITMAPINFOHEADER lpBMIH )
{
    return WIDTHBYTES(lpBMIH->biWidth * lpBMIH->biPlanes * lpBMIH->biBitCount);
}
/* End BytesPerLine() ********************************************************/

/****************************************************************************
*
*     FUNCTION: ConvertDIBFormat
*
*     PURPOSE:  Creates a new DIB of the requested format, copies the source
*               image to the new DIB.
*
*     PARAMS:   LPBITMAPINFO lpSrcDIB - the source CF_DIB
*               UINT         nWidth   - width for new DIB
*               UINT         nHeight  - height for new DIB
*               UINT         nbpp     - bpp for new DIB
*               BOOL         bStretch - TRUE to stretch source to dest
*                                       FALSE to take upper left of image
*
*     RETURNS:  LPBYTE - pointer to new CF_DIB memory block with new image
*               NULL on failure
*
*
\****************************************************************************/
LPBYTE CDib::ConvertDIBFormat( LPBITMAPINFO lpSrcDIB, UINT nWidth, UINT nHeight, UINT nbpp, BOOL bStretch )
{
    LPBITMAPINFO    lpbmi = NULL;
    LPBYTE        	lpSourceBits, lpTargetBits, lpResult;
    HDC            	hDC = NULL, hSourceDC, hTargetDC;
    HBITMAP        	hSourceBitmap, hTargetBitmap, hOldTargetBitmap, hOldSourceBitmap;
    DWORD        	dwSourceBitsSize, dwTargetBitsSize, dwTargetHeaderSize;

    // Allocate and fill out a BITMAPINFO struct for the new DIB
    // Allow enough room for a 256-entry color table, just in case
    dwTargetHeaderSize = sizeof( BITMAPINFO ) + ( 256 * sizeof( RGBQUAD ) );
    lpbmi = (LPBITMAPINFO)malloc( dwTargetHeaderSize );
    lpbmi->bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
    lpbmi->bmiHeader.biWidth = nWidth;
    lpbmi->bmiHeader.biHeight = nHeight;
    lpbmi->bmiHeader.biPlanes = 1;
    lpbmi->bmiHeader.biBitCount = nbpp;
    lpbmi->bmiHeader.biCompression = BI_RGB;
    lpbmi->bmiHeader.biSizeImage = 0;
    lpbmi->bmiHeader.biXPelsPerMeter = 0;
    lpbmi->bmiHeader.biYPelsPerMeter = 0;
    lpbmi->bmiHeader.biClrUsed = 0;
    lpbmi->bmiHeader.biClrImportant = 0;
    // Fill in the color table
    if( ! CopyColorTable( lpbmi, (LPBITMAPINFO)lpSrcDIB ) )
    {
        free( lpbmi );
        return NULL;
    }

    // Gonna use DIBSections and BitBlt() to do the conversion, so make 'em
    hDC = GetDC( NULL );
    hTargetBitmap = CreateDIBSection( hDC, lpbmi, DIB_RGB_COLORS, (void ** )&lpTargetBits, NULL, 0 );
    hSourceBitmap = CreateDIBSection( hDC, lpSrcDIB, DIB_RGB_COLORS,(void ** ) &lpSourceBits, NULL, 0 );
    hSourceDC = CreateCompatibleDC( hDC );
    hTargetDC = CreateCompatibleDC( hDC );

    // Flip the bits on the source DIBSection to match the source DIB
    dwSourceBitsSize = lpSrcDIB->bmiHeader.biHeight * BytesPerLine(&(lpSrcDIB->bmiHeader));
    dwTargetBitsSize = lpbmi->bmiHeader.biHeight * BytesPerLine(&(lpbmi->bmiHeader));
    memcpy( lpSourceBits, FindDIBBits((LPSTR)lpSrcDIB), dwSourceBitsSize );

    // Select DIBSections into DCs
    hOldSourceBitmap =(HBITMAP ) SelectObject( hSourceDC, hSourceBitmap );
    hOldTargetBitmap = (HBITMAP )SelectObject( hTargetDC, hTargetBitmap );

    // Set the color tables for the DIBSections
    if( lpSrcDIB->bmiHeader.biBitCount <= 8 )
        SetDIBColorTable( hSourceDC, 0, 1 << lpSrcDIB->bmiHeader.biBitCount, lpSrcDIB->bmiColors );
    if( lpbmi->bmiHeader.biBitCount <= 8 )
        SetDIBColorTable( hTargetDC, 0, 1 << lpbmi->bmiHeader.biBitCount, lpbmi->bmiColors );

    // If we are asking for a straight copy, do it
    if( (lpSrcDIB->bmiHeader.biWidth==lpbmi->bmiHeader.biWidth) && (lpSrcDIB->bmiHeader.biHeight==lpbmi->bmiHeader.biHeight) )
    {
        BitBlt( hTargetDC, 0, 0, lpbmi->bmiHeader.biWidth, lpbmi->bmiHeader.biHeight, hSourceDC, 0, 0, SRCCOPY );
    }
    else
    {
        // else, should we stretch it?
        if( bStretch )
        {
            SetStretchBltMode( hTargetDC, COLORONCOLOR );
            StretchBlt( hTargetDC, 0, 0, lpbmi->bmiHeader.biWidth, lpbmi->bmiHeader.biHeight, hSourceDC, 0, 0, lpSrcDIB->bmiHeader.biWidth, lpSrcDIB->bmiHeader.biHeight, SRCCOPY );
        }
        else
        {
            // or just take the upper left corner of the source
            BitBlt( hTargetDC, 0, 0, lpbmi->bmiHeader.biWidth, lpbmi->bmiHeader.biHeight, hSourceDC, 0, 0, SRCCOPY );
        }
    }

    // Clean up and delete the DCs
    SelectObject( hSourceDC, hOldSourceBitmap );
    SelectObject( hSourceDC, hOldTargetBitmap );
    DeleteDC( hSourceDC );
    DeleteDC( hTargetDC );
    ReleaseDC( NULL, hDC );

    // Flush the GDI batch, so we can play with the bits
    GdiFlush();

    // Allocate enough memory for the new CF_DIB, and copy bits
    lpResult =(LPBYTE ) malloc( dwTargetHeaderSize + dwTargetBitsSize );
    memcpy( lpResult, lpbmi, dwTargetHeaderSize );
    memcpy( FindDIBBits( (LPSTR)lpResult ), lpTargetBits, dwTargetBitsSize );

    // final cleanup
    DeleteObject( hTargetBitmap );
    DeleteObject( hSourceBitmap );
    free( lpbmi );

    return lpResult;
}
/* End ConvertDIBFormat() ***************************************************/

/****************************************************************************
*
*     FUNCTION: CopyColorTable
*
*     PURPOSE:  Copies the color table from one CF_DIB to another.
*
*     PARAMS:   LPBITMAPINFO lpTarget - pointer to target DIB
*               LPBITMAPINFO lpSource - pointer to source DIB
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
*
\****************************************************************************/
BOOL CDib::CopyColorTable( LPBITMAPINFO lpTarget, LPBITMAPINFO lpSource )
{
    // What we do depends on the target's color depth
    switch( lpTarget->bmiHeader.biBitCount )
    {
        // 8bpp - need 256 entry color table
        case 8:
            if( lpSource->bmiHeader.biBitCount == 8 )
            { // Source is 8bpp too, copy color table
                memcpy( lpTarget->bmiColors, lpSource->bmiColors, 256*sizeof(RGBQUAD) );
                return TRUE;
            }
            else
            { // Source is != 8bpp, use halftone palette                
                HPALETTE        hPal;
                HDC            	hDC = GetDC( NULL );
                PALETTEENTRY    pe[256];
                UINT            i;

                hPal = CreateHalftonePalette( hDC );
                ReleaseDC( NULL, hDC );
                GetPaletteEntries( hPal, 0, 256, pe );
                DeleteObject( hPal );
                for(i=0;i<256;i++)
                {
                    lpTarget->bmiColors[i].rgbRed = pe[i].peRed;
                    lpTarget->bmiColors[i].rgbGreen = pe[i].peGreen;
                    lpTarget->bmiColors[i].rgbBlue = pe[i].peBlue;
                    lpTarget->bmiColors[i].rgbReserved = pe[i].peFlags;
                }
                return TRUE;
            }
        break; // end 8bpp

        // 4bpp - need 16 entry color table
        case 4:
            if( lpSource->bmiHeader.biBitCount == 4 )
            { // Source is 4bpp too, copy color table
                memcpy( lpTarget->bmiColors, lpSource->bmiColors, 16*sizeof(RGBQUAD) );
                return TRUE;
            }
            else
            { // Source is != 4bpp, use system palette
                HPALETTE        hPal;
                PALETTEENTRY    pe[256];
                UINT            i;

                hPal = (HPALETTE)GetStockObject( DEFAULT_PALETTE );
                GetPaletteEntries( hPal, 0, 16, pe );
                for(i=0;i<16;i++)
                {
                    lpTarget->bmiColors[i].rgbRed = pe[i].peRed;
                    lpTarget->bmiColors[i].rgbGreen = pe[i].peGreen;
                    lpTarget->bmiColors[i].rgbBlue = pe[i].peBlue;
                    lpTarget->bmiColors[i].rgbReserved = pe[i].peFlags;
                }
                return TRUE;
            }
        break; // end 4bpp

        // 1bpp - need 2 entry mono color table
        case 1:
            lpTarget->bmiColors[0].rgbRed = 0;
            lpTarget->bmiColors[0].rgbGreen = 0;
            lpTarget->bmiColors[0].rgbBlue = 0;
            lpTarget->bmiColors[0].rgbReserved = 0;
            lpTarget->bmiColors[1].rgbRed = 255;
            lpTarget->bmiColors[1].rgbGreen = 255;
            lpTarget->bmiColors[1].rgbBlue = 255;
            lpTarget->bmiColors[1].rgbReserved = 0;
        break; // end 1bpp

        // no color table for the > 8bpp modes
        case 32:
        case 24:
        case 16:
        default:
            return TRUE;
        break;
    }
    return TRUE;
}
/* End CopyColorTable() *****************************************************/

/****************************************************************************
*
*     FUNCTION: SetMonoDIBPixel
*
*     PURPOSE:  Sets/Clears a pixel in a 1bpp DIB by directly poking the bits.
*
*     PARAMS:   LPBYTE pANDBits - pointer to the 1bpp image bits
*               DWORD  dwWidth	- width of the DIB
*               DWORD  dwHeight	- height of the DIB
*               DWORD  x        - x location of pixel to set/clear
*               DWORD  y        - y location of pixel to set/clear
*               BOOL   bWhite	- TRUE to set pixel, FALSE to clear it
*
*     RETURNS:  void
*
*
\****************************************************************************/
void CDib::SetMonoDIBPixel( LPBYTE pANDBits, DWORD dwWidth, DWORD dwHeight, DWORD x, DWORD y, BOOL bWhite )
{
    DWORD	ByteIndex;
    BYTE    BitNumber;

    // Find the byte on which this scanline begins
    ByteIndex = (dwHeight - y - 1) * WIDTHBYTES(dwWidth);
    // Find the byte containing this pixel
    ByteIndex += (x >> 3);
    // Which bit is it?
    BitNumber = (BYTE)( 7 - (x % 8) );

    if( bWhite )
        // Turn it on
        pANDBits[ByteIndex] |= (1<<BitNumber);
    else
        // Turn it off
        pANDBits[ByteIndex] &= ~(1<<BitNumber);
}
/* End SetMonoDIBPixel() *****************************************************/

/****************************************************************************
*
*     FUNCTION: ReadBMPFile
*
*     PURPOSE:  Reads a BMP file into CF_DIB format
*
*     PARAMS:   LPCTSTR szFileName - the name of the file to read
*
*     RETURNS:  LPBYTE - pointer to the CF_DIB, NULL for failure
*
*
\****************************************************************************/
LPBYTE CDib::ReadBMPFile( LPCTSTR szFileName )
{
    HANDLE            	hFile;
    BITMAPFILEHEADER    bfh;
    DWORD            	dwBytes;
    LPBYTE            	lpDIB = NULL, lpTemp = NULL;
    WORD                wPaletteSize = 0;
    DWORD            	dwBitsSize = 0;

    // Open the file
    if( (hFile=CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE )
    {
        MessageBox( NULL, "打开位图文件出错!", szFileName, MB_OK );
        return NULL;
    }
    // Read the header
    if( ( ! ReadFile( hFile, &bfh, sizeof(BITMAPFILEHEADER), &dwBytes, NULL ) ) || ( dwBytes != sizeof( BITMAPFILEHEADER ) ) )
    {
        CloseHandle( hFile );
        MessageBox( NULL, "读位图文件头出错!", szFileName, MB_OK );
        return NULL;
    }
    // Does it look like a BMP file?
    if( ( bfh.bfType != 0x4d42 ) || (bfh.bfReserved1!=0) || (bfh.bfReserved2!=0) )
    {
        CloseHandle( hFile );
        MessageBox( NULL, "未知的位图格式文件!", szFileName, MB_OK );
        return NULL;
    }
    // Allocate some memory
    if( (lpDIB =(LPBYTE) malloc( sizeof( BITMAPINFO ) )) == NULL )
    {
        CloseHandle( hFile );
        MessageBox( NULL, "位图内存重新分配出错!", szFileName, MB_OK );
        return NULL;
    }
    // Read in the BITMAPINFOHEADER
    if( (!ReadFile( hFile, lpDIB, sizeof(BITMAPINFOHEADER), &dwBytes, NULL )) || (dwBytes!=sizeof(BITMAPINFOHEADER)) )
    {
        CloseHandle( hFile );
        free( lpDIB );
        MessageBox( NULL, "读位图文件信息头出错!", szFileName, MB_OK );
        return NULL;
    }
    if( ((LPBITMAPINFOHEADER)lpDIB)->biSize != sizeof( BITMAPINFOHEADER ) )
    {
        CloseHandle( hFile );
        free( lpDIB );
        MessageBox( NULL, "OS/2风格的位图不支持!", szFileName, MB_OK );
        return NULL;
    }
    // How big are the elements?
    wPaletteSize = PaletteSize((LPSTR)lpDIB);
    dwBitsSize = ((LPBITMAPINFOHEADER)lpDIB)->biHeight * BytesPerLine((LPBITMAPINFOHEADER)lpDIB);
    // realloc to account for the total size of the DIB
    if( (lpTemp = (LPBYTE)realloc( lpDIB, sizeof( BITMAPINFOHEADER ) + wPaletteSize + dwBitsSize )) == NULL )
    {
        CloseHandle( hFile );
        MessageBox( NULL, "重新分配位图信息头所需内存失败!", szFileName, MB_OK );
        free( lpDIB );
        return NULL;
    }
    lpDIB = lpTemp;
    // If there is a color table, read it
    if( wPaletteSize != 0 )
    {
        if( (!ReadFile( hFile, ((LPBITMAPINFO)lpDIB)->bmiColors, wPaletteSize, &dwBytes, NULL )) || (dwBytes!=wPaletteSize) )
        {
            CloseHandle( hFile );
            free( lpDIB );
            MessageBox( NULL, "读位图颜色表出错!", szFileName, MB_OK );
            return NULL;
        }
    }
    // Seek to the bits
    // checking against 0 in case some bogus app didn't set this element
    if( bfh.bfOffBits != 0 )
    {
        if( SetFilePointer( hFile, bfh.bfOffBits, NULL, FILE_BEGIN ) == 0xffffffff )
        {
            CloseHandle( hFile );
            free( lpDIB );
            MessageBox( NULL, "位图文件大小定位出错!", szFileName, MB_OK );
            return NULL;
        }
    }
    // Read the image bits
    if( (!ReadFile( hFile, FindDIBBits((LPSTR)lpDIB), dwBitsSize, &dwBytes, NULL )) || (dwBytes!=dwBitsSize) )
    {
        CloseHandle( hFile );
        free( lpDIB );
        MessageBox( NULL, "读位图文件出错!", szFileName, MB_OK );
        return NULL;
    }
    // clean up
    CloseHandle( hFile );
    return lpDIB;
}
/* End ReadBMPFile() ********************************************************/

/****************************************************************************
*
*     FUNCTION: WriteBMPFile
*
*     PURPOSE:  Writes a BMP file from CF_DIB format
*
*     PARAMS:   LPCTSTR szFileName - the name of the file to read
*               LPBYTE - pointer to the CF_DIB, NULL for failure
*
*     RETURNS:  BOOL - TRUE for success, FALSE for Failure
*
*
\****************************************************************************/
BOOL CDib::WriteBMPFile( LPCTSTR szFileName, LPBYTE lpDIB )
{
    HANDLE            	hFile;
    BITMAPFILEHEADER    bfh;
    DWORD            	dwBytes, dwBytesToWrite;
    LPBITMAPINFOHEADER	lpbmih;

    // Open the file
    if( (hFile=CreateFile( szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE )
    {
        MessageBox( NULL, "位图文件建立失败！", szFileName, MB_OK );
        return FALSE;
    }
    bfh.bfType = 0x4d42;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfOffBits = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + PaletteSize((LPSTR) lpDIB );
    bfh.bfSize = (bfh.bfOffBits + ((LPBITMAPINFOHEADER)lpDIB)->biHeight * BytesPerLine((LPBITMAPINFOHEADER)lpDIB))/4;
    // Write the header
    if( ( ! WriteFile( hFile, &bfh, sizeof(BITMAPFILEHEADER), &dwBytes, NULL ) ) || ( dwBytes != sizeof( BITMAPFILEHEADER ) ) )
    {
        CloseHandle( hFile );
        MessageBox( NULL, "写位图文件头出错!", szFileName, MB_OK );
        return FALSE;
    }
    lpbmih = (LPBITMAPINFOHEADER)lpDIB;
//    lpbmih->biHeight /= 2;
    dwBytesToWrite = bfh.bfOffBits + (lpbmih->biHeight * BytesPerLine(lpbmih));
    if( ( ! WriteFile( hFile, lpDIB, dwBytesToWrite, &dwBytes, NULL ) ) || ( dwBytes != dwBytesToWrite ) )
    {
        CloseHandle( hFile );
        MessageBox( NULL, "写位图文件出错!", szFileName, MB_OK );
        return FALSE;
    }
    lpbmih->biHeight *= 2;
    CloseHandle( hFile );
    return TRUE;
}
/* End WriteBMPFile() *******************************************************/