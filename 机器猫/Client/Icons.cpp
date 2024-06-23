// Icons.cpp: implementation of the CIcons class.
//////////////////////////////////////////////////////////////////////
//
//用途：IconSnap所需调用的类
//功能：图标基础操作类
//作者：徐景周
//日期：2001年9月
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Icons.h"
#include "Dib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CDib *pDib;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIcons::CIcons()
{

}

CIcons::~CIcons()
{

}

/****************************************************************************
*
*     FUNCTION: MakeIconFromResource
*
*     PURPOSE:  Makes an HICON from an icon resource
*
*     PARAMS:   LPICONIMAGE	lpIcon - pointer to the icon resource
*
*     RETURNS:  HICON - handle to the new icon, NULL for failure
*
*
\****************************************************************************/
HICON CIcons::MakeIconFromResource( LPICONIMAGE lpIcon )
{
    HICON        	hIcon = NULL;

    // Sanity Check
    if( lpIcon == NULL )
        return NULL;
    if( lpIcon->lpBits == NULL )
        return NULL;
    // Let the OS do the real work :)
    hIcon = CreateIconFromResourceEx( lpIcon->lpBits, lpIcon->dwNumBytes, TRUE, 0x00030000, 
            (*(LPBITMAPINFOHEADER)(lpIcon->lpBits)).biWidth, (*(LPBITMAPINFOHEADER)(lpIcon->lpBits)).biHeight/2, 0 );
    
    // It failed, odds are good we're on NT so try the non-Ex way
    if( hIcon == NULL )
    {
        // We would break on NT if we try with a 16bpp image
        if(lpIcon->lpbi->bmiHeader.biBitCount != 16)
        {	
            hIcon = CreateIconFromResource( lpIcon->lpBits, lpIcon->dwNumBytes, TRUE, 0x00030000 );
        }
    }
    return hIcon;
}
/* End MakeIconFromResource() **********************************************/

/****************************************************************************
*
*     FUNCTION: ReadIconFromICOFile
*
*     PURPOSE:  Reads an Icon Resource from an ICO file
*
*     PARAMS:   LPCTSTR szFileName - Name of the ICO file
*
*     RETURNS:  LPICONRESOURCE - pointer to the resource, NULL for failure
*
*
\****************************************************************************/
LPICONRESOURCE CIcons::ReadIconFromICOFile( LPCTSTR szFileName )
{
    LPICONRESOURCE    	lpIR = NULL, lpNew = NULL;
    HANDLE            	hFile = NULL;
    LPRESOURCEPOSINFO	lpRPI = NULL;
    UINT                i;
    DWORD            	dwBytesRead;
    LPICONDIRENTRY    	lpIDE = NULL;


    // Open the file
    if( (hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL )) == INVALID_HANDLE_VALUE )
    {
        MessageBox( AfxGetMainWnd()->m_hWnd, "图标文件打开时出错!", szFileName, MB_OK );
        return NULL;
    }
    // Allocate memory for the resource structure
    if( (lpIR = (LPICONRESOURCE)malloc( sizeof(ICONRESOURCE) )) == NULL )
    {
        MessageBox( AfxGetMainWnd()->m_hWnd, "图标分配内存时出错!", szFileName, MB_OK );
        CloseHandle( hFile );
        return NULL;
    }
    // Read in the header
    if( (lpIR->nNumImages = ReadICOHeader( hFile )) == (UINT)-1 )
    {
        MessageBox( AfxGetMainWnd()->m_hWnd, "读图标文件头时出错!", szFileName, MB_OK );
        CloseHandle( hFile );
        free( lpIR );
        return NULL;
    }
    // Adjust the size of the struct to account for the images
    if( (lpNew = (LPICONRESOURCE)realloc( lpIR, sizeof(ICONRESOURCE) + ((lpIR->nNumImages-1) * sizeof(ICONIMAGE)) )) == NULL )
    {
        MessageBox( AfxGetMainWnd()->m_hWnd, "图标内存重新分配出错!", szFileName, MB_OK );
        CloseHandle( hFile );
        free( lpIR );
        return NULL;
    }
    lpIR = lpNew;
    // Store the original name
    lstrcpy( lpIR->szOriginalICOFileName, szFileName );
    lstrcpy( lpIR->szOriginalDLLFileName, "" );
    // Allocate enough memory for the icon directory entries
    if( (lpIDE = (LPICONDIRENTRY)malloc( lpIR->nNumImages * sizeof( ICONDIRENTRY ) ) ) == NULL )
    {
        MessageBox( AfxGetMainWnd()->m_hWnd, "图标文件内存重新分配出错!", szFileName, MB_OK );
        CloseHandle( hFile );
        free( lpIR );
        return NULL;
    }
    // Read in the icon directory entries
    if( ! ReadFile( hFile, lpIDE, lpIR->nNumImages * sizeof( ICONDIRENTRY ), &dwBytesRead, NULL ) )
    {
        MessageBox(  AfxGetMainWnd()->m_hWnd, "读图标文件时出错!", szFileName, MB_OK );
        CloseHandle( hFile );
        free( lpIR );
        return NULL;
    }
    if( dwBytesRead != lpIR->nNumImages * sizeof( ICONDIRENTRY ) )
    {
        MessageBox(  AfxGetMainWnd()->m_hWnd, "读图标文件时出错!", szFileName, MB_OK );
        CloseHandle( hFile );
        free( lpIR );
        return NULL;
    }
    // Loop through and read in each image
    for( i = 0; i < lpIR->nNumImages; i++ )
    {
        // Allocate memory for the resource
        if( (lpIR->IconImages[i].lpBits = (LPBYTE)malloc(lpIDE[i].dwBytesInRes)) == NULL )
        {
            MessageBox( AfxGetMainWnd()->m_hWnd, "图标内存重新分配出错!", szFileName, MB_OK );
            CloseHandle( hFile );
            free( lpIR );
            free( lpIDE );
            return NULL;
        }
        lpIR->IconImages[i].dwNumBytes = lpIDE[i].dwBytesInRes;
        // Seek to beginning of this image
        if( SetFilePointer( hFile, lpIDE[i].dwImageOffset, NULL, FILE_BEGIN ) == 0xFFFFFFFF )
        {
            MessageBox( AfxGetMainWnd()->m_hWnd, "图标内容定位时出错!", szFileName, MB_OK );
            CloseHandle( hFile );
            free( lpIR );
            free( lpIDE );
            return NULL;
        }
        // Read it in
        if( ! ReadFile( hFile, lpIR->IconImages[i].lpBits, lpIDE[i].dwBytesInRes, &dwBytesRead, NULL ) )
        {
            MessageBox( AfxGetMainWnd()->m_hWnd, "读图标内容出错!", szFileName, MB_OK );
            CloseHandle( hFile );
            free( lpIR );
            free( lpIDE );
            return NULL;
        }
        if( dwBytesRead != lpIDE[i].dwBytesInRes )
        {
            MessageBox(AfxGetMainWnd()->m_hWnd, "读图标内容出错!", szFileName, MB_OK );
            CloseHandle( hFile );
            free( lpIDE );
            free( lpIR );
            return NULL;
        }
        // Set the internal pointers appropriately
        if( ! AdjustIconImagePointers( &(lpIR->IconImages[i]) ) )
        {
            MessageBox( AfxGetMainWnd()->m_hWnd, "图标内部格式转换出错!", szFileName, MB_OK );
            CloseHandle( hFile );
            free( lpIDE );
            free( lpIR );
            return NULL;
        }
    }
    // Clean up	
    free( lpIDE );
    free( lpRPI );
    CloseHandle( hFile );
    return lpIR;
}
/* End ReadIconFromICOFile() **********************************************/

/****************************************************************************
*
*     FUNCTION: AdjustIconImagePointers
*
*     PURPOSE:  Adjusts internal pointers in icon resource struct
*
*     PARAMS:   LPICONIMAGE lpImage - the resource to handle
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
*
\****************************************************************************/
BOOL CIcons::AdjustIconImagePointers( LPICONIMAGE lpImage )
{
    // Sanity check
    if( lpImage==NULL )
        return FALSE;
    // BITMAPINFO is at beginning of bits
    lpImage->lpbi = (LPBITMAPINFO)lpImage->lpBits;
    // Width - simple enough
    lpImage->Width = lpImage->lpbi->bmiHeader.biWidth;
    // Icons are stored in funky format where height is doubled - account for it
    lpImage->Height = (lpImage->lpbi->bmiHeader.biHeight)/2;
    // How many colors?
    lpImage->Colors = lpImage->lpbi->bmiHeader.biPlanes * lpImage->lpbi->bmiHeader.biBitCount;
    // XOR bits follow the header and color table
    lpImage->lpXOR = (LPBYTE)pDib->FindDIBBits((LPSTR)lpImage->lpbi);
    // AND bits follow the XOR bits
    lpImage->lpAND = lpImage->lpXOR + (lpImage->Height*pDib->BytesPerLine((LPBITMAPINFOHEADER)(lpImage->lpbi)));
    return TRUE;
}
/* End AdjustIconImagePointers() *******************************************/

/****************************************************************************
*
*     FUNCTION: ReadICOHeader
*
*     PURPOSE:  Reads the header from an ICO file
*
*     PARAMS:   HANDLE hFile - handle to the file
*
*     RETURNS:  UINT - Number of images in file, -1 for failure
*
*
\****************************************************************************/
UINT CIcons::ReadICOHeader( HANDLE hFile )
{
    WORD    Input;
    DWORD	dwBytesRead;

    // Read the 'reserved' WORD
    if( ! ReadFile( hFile, &Input, sizeof( WORD ), &dwBytesRead, NULL ) )
        return (UINT)-1;
    // Did we get a WORD?
    if( dwBytesRead != sizeof( WORD ) )
        return (UINT)-1;
    // Was it 'reserved' ?   (ie 0)
    if( Input != 0 )
        return (UINT)-1;
    // Read the type WORD
    if( ! ReadFile( hFile, &Input, sizeof( WORD ), &dwBytesRead, NULL ) )
        return (UINT)-1;
    // Did we get a WORD?
    if( dwBytesRead != sizeof( WORD ) )
        return (UINT)-1;
    // Was it type 1?
    if( Input != 1 )
        return (UINT)-1;
    // Get the count of images
    if( ! ReadFile( hFile, &Input, sizeof( WORD ), &dwBytesRead, NULL ) )
        return (UINT)-1;
    // Did we get a WORD?
    if( dwBytesRead != sizeof( WORD ) )
        return (UINT)-1;
    // Return the count
    return Input;
}
/* End ReadICOHeader() ****************************************************/

/****************************************************************************
*
*     FUNCTION: GetIconFromInstance
*
*     PURPOSE:  Callback for enumerating resources in a DLL/EXE
*
*     PARAMS:   HINSTANCE hInstance - Instance handle for this module
*               LPTSTR    nIndex    - Resource index
*
*     RETURNS:  HICON - Handle to the icon, NULL for failure
*
*
\****************************************************************************/
HICON CIcons::GetIconFromInstance( HINSTANCE hInstance, LPTSTR nIndex )
{
    HICON	hIcon = NULL;
    HRSRC	hRsrc = NULL;
    HGLOBAL	hGlobal = NULL;
    LPVOID	lpRes = NULL;
    int    	nID;

    // Find the group icon
    if( (hRsrc = FindResource( hInstance, nIndex, RT_GROUP_ICON )) == NULL )
        return NULL;
    if( (hGlobal = LoadResource( hInstance, hRsrc )) == NULL )
        return NULL;
    if( (lpRes = LockResource(hGlobal)) == NULL )
        return NULL;

    // Find this particular image
    nID = LookupIconIdFromDirectory((LPBYTE) lpRes, TRUE );
    if( (hRsrc = FindResource( hInstance, MAKEINTRESOURCE(nID), RT_ICON )) == NULL )
        return NULL;
    if( (hGlobal = LoadResource( hInstance, hRsrc )) == NULL )
        return NULL;
    if( (lpRes = LockResource(hGlobal)) == NULL )
        return NULL;
    // Let the OS make us an icon
    hIcon = CreateIconFromResource((LPBYTE) lpRes, SizeofResource(hInstance,hRsrc), TRUE, 0x00030000 );
    return hIcon;
}
/* End GetIconFromInstance() ***********************************************/

/****************************************************************************
*
*     FUNCTION: ReadIconFromEXEFile
*
*     PURPOSE:  Load an Icon Resource from a DLL/EXE file
*
*     PARAMS:   LPCTSTR szFileName - name of DLL/EXE file
*               LPCTSTR lpID - Index of DLL/EXE file
*
*     RETURNS:  LPICONRESOURCE - pointer to icon resource
*
*
\****************************************************************************/
LPICONRESOURCE CIcons::ReadIconFromEXEFile( LPCTSTR szFileName,LPTSTR lpID )
{
    LPICONRESOURCE    	lpIR = NULL, lpNew = NULL;
    HINSTANCE        	hLibrary;
    EXEDLLICONINFO    	EDII;

    // Load the DLL/EXE - NOTE: must be a 32bit EXE/DLL for this to work
    if( (hLibrary = LoadLibraryEx( szFileName, NULL, LOAD_LIBRARY_AS_DATAFILE )) == NULL )
    {
        // Failed to load - abort
        MessageBox(AfxGetMainWnd()->m_hWnd , "装入文件时出错 - 请选择一个WIN32的DLL或EXE文件!", szFileName, MB_OK );
        return NULL;
    }
    // Store the info
    EDII.szFileName = szFileName;
    EDII.hInstance = hLibrary;
    // Ask the user, "Which Icon?"
    if( lpID != NULL )
    {
        HRSRC        	hRsrc = NULL;
        HGLOBAL        	hGlobal = NULL;
        LPMEMICONDIR    lpIcon = NULL;
        UINT            i;

        // Find the group icon resource
        if( (hRsrc = FindResource( hLibrary, lpID, RT_GROUP_ICON )) == NULL )
        {
            FreeLibrary( hLibrary );
            return NULL;
        }
        if( (hGlobal = LoadResource( hLibrary, hRsrc )) == NULL )
        {
            FreeLibrary( hLibrary );
            return NULL;
        }
        if( (lpIcon = (LPMEMICONDIR)LockResource(hGlobal)) == NULL )
        {
            FreeLibrary( hLibrary );
            return NULL;
        }
        // Allocate enough memory for the images
        if( (lpIR = (LPICONRESOURCE)malloc( sizeof(ICONRESOURCE) + ((lpIcon->idCount-1) * sizeof(ICONIMAGE)) )) == NULL )
        {
            MessageBox( AfxGetMainWnd()->m_hWnd, "内存分配出错!", szFileName, MB_OK );
            FreeLibrary( hLibrary );
            return NULL;
        }
        // Fill in local struct members
        lpIR->nNumImages = lpIcon->idCount;
        lstrcpy( lpIR->szOriginalDLLFileName, szFileName );
        lstrcpy( lpIR->szOriginalICOFileName, "" );
        // Loop through the images
        for( i = 0; i < lpIR->nNumImages; i++ )
        {
            // Get the individual image
            if( (hRsrc = FindResource( hLibrary, MAKEINTRESOURCE(lpIcon->idEntries[i].nID), RT_ICON )) == NULL )
            {
                free( lpIR );
                FreeLibrary( hLibrary );
                return NULL;
            }
            if( (hGlobal = LoadResource( hLibrary, hRsrc )) == NULL )
            {
                free( lpIR );
                FreeLibrary( hLibrary );
                return NULL;
            }
            // Store a copy of the resource locally
            lpIR->IconImages[i].dwNumBytes =SizeofResource( hLibrary, hRsrc );
            lpIR->IconImages[i].lpBits = (LPBYTE)malloc( lpIR->IconImages[i].dwNumBytes );
            memcpy( lpIR->IconImages[i].lpBits, LockResource( hGlobal ), lpIR->IconImages[i].dwNumBytes );
            // Adjust internal pointers
            if( ! AdjustIconImagePointers( &(lpIR->IconImages[i]) ) )
            {
                MessageBox(AfxGetMainWnd()->m_hWnd, "转换成图标内部格式时出错!", szFileName, MB_OK );
                free( lpIR );
                FreeLibrary( hLibrary );
                return NULL;
            }
        }
    }
    FreeLibrary( hLibrary );
    return lpIR;
}
/* End ReadIconFromEXEFile() ************************************************/

/****************************************************************************
*
*     FUNCTION: WriteICOHeader
*
*     PURPOSE:  Writes the header to an ICO file
*
*     PARAMS:   HANDLE hFile       - handle to the file
*               UINT   nNumEntries - Number of images in file
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
*
\****************************************************************************/
BOOL CIcons::WriteICOHeader( HANDLE hFile, UINT nNumEntries )
{
    WORD    Output;
    DWORD	dwBytesWritten;

    // Write 'reserved' WORD
    Output = 0;
    if( ! WriteFile( hFile, &Output, sizeof( WORD ), &dwBytesWritten, NULL ) )
        return FALSE;
    // Did we write a WORD?
    if( dwBytesWritten != sizeof( WORD ) )
        return FALSE;
    // Write 'type' WORD (1)
    Output = 1;
    if( ! WriteFile( hFile, &Output, sizeof( WORD ), &dwBytesWritten, NULL ) )
        return FALSE;
    // Did we write a WORD?
    if( dwBytesWritten != sizeof( WORD ) )
        return FALSE;
    // Write Number of Entries
    Output = (WORD)nNumEntries;
    if( ! WriteFile( hFile, &Output, sizeof( WORD ), &dwBytesWritten, NULL ) )
        return FALSE;
    // Did we write a WORD?
    if( dwBytesWritten != sizeof( WORD ) )
        return FALSE;
    return TRUE;
}
/* End WriteICOHeader() ****************************************************/

/****************************************************************************
*
*     FUNCTION: CalculateImageOffset
*
*     PURPOSE:  Calculates the file offset for an icon image
*
*     PARAMS:   LPICONRESOURCE lpIR   - pointer to icon resource
*               UINT           nIndex - which image?
*
*     RETURNS:  DWORD - the file offset for that image
*
*
\****************************************************************************/
DWORD CIcons::CalculateImageOffset( LPICONRESOURCE lpIR, UINT nIndex )
{
    DWORD	dwSize;
    UINT    i;

    // Calculate the ICO header size
    dwSize = 3 * sizeof(WORD);
    // Add the ICONDIRENTRY's
    dwSize += lpIR->nNumImages * sizeof(ICONDIRENTRY);
    // Add the sizes of the previous images
    for(i=0;i<nIndex;i++)
        dwSize += lpIR->IconImages[i].dwNumBytes;
    // we're there - return the number
    return dwSize;
}
/* End CalculateImageOffset() ***********************************************/

/****************************************************************************
*
*     FUNCTION: WriteIconToICOFile
*
*     PURPOSE:  Writes the icon resource data to an ICO file
*
*     PARAMS:   LPICONRESOURCE lpIR       - pointer to icon resource
*               LPCTSTR        szFileName - name for the ICO file
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
*
\****************************************************************************/
BOOL CIcons::WriteIconToICOFile( LPICONRESOURCE lpIR, LPCTSTR szFileName )
{
    HANDLE    	hFile;
    UINT        i;
    DWORD    	dwBytesWritten;

    // open the file
    if( (hFile = CreateFile( szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL )) == INVALID_HANDLE_VALUE )
    {
        MessageBox( AfxGetMainWnd()->m_hWnd , "文件建立时出错!", szFileName, MB_OK );
        return FALSE;
    }
    // Write the header
    if( ! WriteICOHeader( hFile, lpIR->nNumImages ) )
    {
        MessageBox( AfxGetMainWnd()->m_hWnd , "写图标文件头时出错!", szFileName, MB_OK );
        CloseHandle( hFile );
        return FALSE;
    }
    // Write the ICONDIRENTRY's
    for( i=0; i<lpIR->nNumImages; i++ )
    {
        ICONDIRENTRY    ide;

        // Convert internal format to ICONDIRENTRY
        ide.bWidth = lpIR->IconImages[i].Width;
        ide.bHeight = lpIR->IconImages[i].Height;
        ide.bReserved = 0;
        ide.wPlanes = lpIR->IconImages[i].lpbi->bmiHeader.biPlanes;
        ide.wBitCount = lpIR->IconImages[i].lpbi->bmiHeader.biBitCount;
        if( (ide.wPlanes * ide.wBitCount) >= 8 )
            ide.bColorCount = 0;
        else
            ide.bColorCount = 1 << (ide.wPlanes * ide.wBitCount);
        ide.dwBytesInRes = lpIR->IconImages[i].dwNumBytes;
        ide.dwImageOffset = CalculateImageOffset( lpIR, i );
        // Write the ICONDIRENTRY out to disk
        if( ! WriteFile( hFile, &ide, sizeof( ICONDIRENTRY ), &dwBytesWritten, NULL ) )
            return FALSE;
        // Did we write a full ICONDIRENTRY ?
        if( dwBytesWritten != sizeof( ICONDIRENTRY ) )
            return FALSE;
    }
    // Write the image bits for each image
    for( i=0; i<lpIR->nNumImages; i++ )
    {
        DWORD dwTemp = lpIR->IconImages[i].lpbi->bmiHeader.biSizeImage;

        // Set the sizeimage member to zero
        lpIR->IconImages[i].lpbi->bmiHeader.biSizeImage = 0;
        // Write the image bits to file
        if( ! WriteFile( hFile, lpIR->IconImages[i].lpBits, lpIR->IconImages[i].dwNumBytes, &dwBytesWritten, NULL ) )
            return FALSE;
        if( dwBytesWritten != lpIR->IconImages[i].dwNumBytes )
            return FALSE;
        // set it back
        lpIR->IconImages[i].lpbi->bmiHeader.biSizeImage = dwTemp;
    }
    CloseHandle( hFile );
    return FALSE;
}
/* End WriteIconToICOFile() **************************************************/

/****************************************************************************
*
*     FUNCTION: IconImageToClipBoard
*
*     PURPOSE:  Copies an icon image to the clipboard in CF_DIB format
*
*     PARAMS:   LPICONIMAGE lpii - pointer to icon image data
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
*
\****************************************************************************/
BOOL CIcons::IconImageToClipBoard( LPICONIMAGE lpii )
{
    HANDLE	hGlobal;
    LPSTR	lpBits;

    // Open the clipboard
    if( OpenClipboard(AfxGetMainWnd()->m_hWnd  ) )
    {
        // empty it
        if( EmptyClipboard() )
        {
			if(lpii->dwNumBytes ==NULL)
				return false;
            // Make a buffer to send to clipboard
            hGlobal = GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, lpii->dwNumBytes );
            lpBits = (LPSTR)GlobalLock( hGlobal );
            // Copy the bits to the buffer
            memcpy( lpBits, lpii->lpBits, lpii->dwNumBytes );
            // Adjust for funky height*2 thing
            ((LPBITMAPINFOHEADER)lpBits)->biHeight /= 2;
            GlobalUnlock( hGlobal );
            // Send it to the clipboard
            SetClipboardData( CF_DIB, hGlobal );
            CloseClipboard();
            return TRUE;
        }
    }
    return FALSE;
}
/* End IconImageToClipBoard() ***********************************************/

/****************************************************************************
*
*     FUNCTION: IconImageFromClipBoard
*
*     PURPOSE:  Creates an icon image from the CF_DIB clipboard entry
*
*     PARAMS:   LPICONIMAGE lpii          - pointer to icon image data
*               BOOL        bStretchToFit - TRUE to stretch, FALSE to take
*                                           the upper left corner of the DIB
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
*
\****************************************************************************/
BOOL CIcons::IconImageFromClipBoard( LPICONIMAGE lpii, BOOL bStretchToFit )
{
    LPBITMAPINFO    lpbi;
    HANDLE        	hClipGlobal;
    BOOL            bRet = FALSE;

    // Open the clipboard
    if( OpenClipboard( AfxGetMainWnd()->m_hWnd  ) )
    {
        // Get the CF_DIB data from it
        if( (hClipGlobal = GetClipboardData( CF_DIB )) != NULL )
        {
            // Lock it down
            if( (lpbi=(LPBITMAPINFO)GlobalLock(hClipGlobal)) != NULL )
            {
                // Convert it to an icon image
                bRet = DIBToIconImage( lpii, (LPBYTE)lpbi, bStretchToFit );
                GlobalUnlock( hClipGlobal );
            }
        }
        CloseClipboard();
    }
    return bRet;
}
/* End IconImageFromClipBoard() ********************************************/

/****************************************************************************
*
*     FUNCTION: DIBToIconImage
*
*     PURPOSE:  Converts a CF_DIB memory block to an icon image
*
*     PARAMS:   LPICONIMAGE lpii          - pointer to icon image data
*               LPBYTE      lpDIB         - a pointer to the CF_DIB block
*               BOOL        bStretchToFit - TRUE to stretch, FALSE to take
*                                           the upper left corner of the DIB
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
*
\****************************************************************************/
BOOL CIcons::DIBToIconImage( LPICONIMAGE lpii, LPBYTE lpDIB, BOOL bStretch )
{
    LPBYTE    	lpNewDIB;

    // Sanity check
    if( lpDIB == NULL )
        return FALSE;

    // Let the DIB engine convert color depths if need be
    lpNewDIB = pDib->ConvertDIBFormat( (LPBITMAPINFO)lpDIB, lpii->Width, lpii->Height, lpii->Colors, bStretch );

    // Now we have a cool new DIB of the proper size/color depth
    // Lets poke it into our data structures and be done with it

    // How big is it?
    lpii->dwNumBytes = sizeof( BITMAPINFOHEADER )                    	// Header
                    + pDib->PaletteSize( (LPSTR)lpNewDIB )                    // Palette
                    + lpii->Height * pDib->BytesPerLine( (LPBITMAPINFOHEADER)lpNewDIB )	// XOR mask
                    + lpii->Height * WIDTHBYTES( lpii->Width );        	// AND mask

    // If there was already an image here, free it
    if( lpii->lpBits != NULL )
        free( lpii->lpBits );
    // Allocate enough room for the new image
    if( (lpii->lpBits = (LPBYTE)malloc( lpii->dwNumBytes )) == NULL )
    {
        free( lpii );
        return FALSE;
    }
    // Copy the bits
    memcpy( lpii->lpBits, lpNewDIB, sizeof( BITMAPINFOHEADER ) + pDib->PaletteSize( (LPSTR)lpNewDIB ) );
    // Adjust internal pointers/variables for new image
    lpii->lpbi = (LPBITMAPINFO)(lpii->lpBits);
    lpii->lpbi->bmiHeader.biHeight *= 2;
    lpii->lpXOR =(LPBYTE) pDib->FindDIBBits( (LPSTR)(lpii->lpBits) );
    memcpy( lpii->lpXOR, pDib->FindDIBBits((LPSTR)lpNewDIB), lpii->Height * pDib->BytesPerLine( (LPBITMAPINFOHEADER)lpNewDIB ) );
    lpii->lpAND = lpii->lpXOR + lpii->Height * pDib->BytesPerLine( (LPBITMAPINFOHEADER)lpNewDIB );
    memset( lpii->lpAND, 0, lpii->Height * WIDTHBYTES( lpii->Width ) );
    // Free the source
    free( lpNewDIB );
    return TRUE;
}
/* End DIBToIconImage() ***************************************************/

/****************************************************************************
*
*     FUNCTION: IconImageFromBMPFile
*
*     PURPOSE:  Creates an icon image from a BMP file
*
*     PARAMS:   LPCTSTR     szFileName    - Filename for BMP file
*               LPICONIMAGE lpii          - pointer to icon image data
*               BOOL        bStretchToFit - TRUE to stretch, FALSE to take
*                                           the upper left corner of the DIB
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
*
\****************************************************************************/
BOOL CIcons::IconImageFromBMPFile( LPCTSTR szFileName, LPICONIMAGE lpii, BOOL bStretchToFit )
{
    LPBYTE        	lpDIB = NULL;
    BOOL            bRet = FALSE;

    if( (lpDIB=pDib->ReadBMPFile(szFileName)) == NULL )
        return FALSE;
    // Convert it to an icon image
    bRet = DIBToIconImage( lpii, lpDIB, bStretchToFit );
    free( lpDIB );
    return bRet;
}
/* End IconImageFromBMPFile() ********************************************/

/****************************************************************************
*
*     FUNCTION: IconImageToBMPFile
*
*     PURPOSE:  Creates BMP file from an icon image
*
*     PARAMS:   LPCTSTR     szFileName    - Filename for BMP file
*               LPICONIMAGE lpii          - pointer to icon image data
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
*
\****************************************************************************/
BOOL CIcons::IconImageToBMPFile( LPCTSTR szFileName, LPICONIMAGE lpii )
{
    return pDib->WriteBMPFile( szFileName, (LPBYTE)lpii->lpbi );
}
