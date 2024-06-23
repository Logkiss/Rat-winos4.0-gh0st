#pragma once
#ifndef _XVID_ENCODE_H
#define _XVID_ENCODE_H
#include "xvid.h"
#include <windows.h>
//#define MY_TEST
#define MAX_ENC_BUF   1024*150


class CXvidEnc 
{
protected:
	// original encode routine in xvid lib
	int			enc_core(unsigned char *image, unsigned char *bitstream, int *key);

public:
	CXvidEnc() ; 
	bool            Open();
	bool            Close();
	static void     XVID_GLOBAL_INIT();
	LPBYTE          Encode(unsigned char * image,DWORD &dRet);
	void			AttachCaller(int width, int height) ; 
protected:
	void *           m_enc_handle;
	unsigned char    m_bitstream[MAX_ENC_BUF];
	int			     m_key ;
	int				 m_width ; 
	int				 m_height ; 
};
#endif