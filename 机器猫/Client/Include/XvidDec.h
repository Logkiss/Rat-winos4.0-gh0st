#ifndef _XVID_DECODE_H
#define _XVID_DECODE_H
 
#include "xvid.h"

class CXvidDec {

public:
	CXvidDec() ;
	~CXvidDec() ; 
	bool               Open();
	bool               Close();
	LPBYTE               Decode(unsigned char* xvid, int xvid_len,DWORD &dRet);
	static void		   XVID_GLOBAL_INIT();
	void			   AttachCaller(int width, int height) ; 

protected:
	int                dec_core(unsigned char *bitstream, unsigned char *image, int bs_size) ;

protected:
	void*             m_dec_handle ;
	unsigned char *   m_image ;
	int				  m_width ; 
	int				  m_height ; 
};
#endif
