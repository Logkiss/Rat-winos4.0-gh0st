// SoundToWav.h: interface for the CSoundToWav class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDTOWAV_H__50503AB8_A08C_4949_B5AB_9CA2576835C6__INCLUDED_)
#define AFX_SOUNDTOWAV_H__50503AB8_A08C_4949_B5AB_9CA2576835C6__INCLUDED_

#include <mmsystem.h>
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSoundToWav  
{
public:
	CSoundToWav();
	virtual ~CSoundToWav();

public:
	void  SetWaveFormat(WAVEFORMATEX *pwfx);
	DWORD WriteWavFileHeader();
	DWORD WriteWavData(LPBYTE pData,DWORD nSize);
private:
	DWORD m_dwTotalBytes;
	WAVEFORMATEX m_wfx;

public:
	
	BOOL Open(LPCTSTR lpFileName,DWORD dwDesiredAccess = FILE_WRITE_DATA|FILE_READ_DATA/*²Î¿¼CreateFile*/);
	DWORD Seek(LONG lDistanceToMove,DWORD dwMoveMethod=FILE_BEGIN/*²Î¿¼SetFilePointer*/);
	DWORD SeekToBegin( );
	DWORD SeekToEnd( );
	DWORD Write(LPVOID lpBuffer,DWORD nNumberOfBytesToWrite);
	BOOL close( );
	
	
protected:
	HANDLE m_hFile; 
};

#endif // !defined(AFX_SOUNDTOWAV_H__50503AB8_A08C_4949_B5AB_9CA2576835C6__INCLUDED_)
