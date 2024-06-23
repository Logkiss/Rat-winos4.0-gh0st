#ifndef _XVID_DECODE_H
#define _XVID_DECODE_H

#include "xvid.h"

class CXvidDec
{
public:
	CXvidDec(int version = XVID_VERSION, unsigned int cpu_flags = 0, int debug = 0);
	~CXvidDec();
	bool m_bOpened;
	bool Open(int width, int height, int cbit);
	int  Decode(void *input, int inlen, void *output);
	bool Close();

private:
	int  m_version;
	bool m_bInited;
	xvid_gbl_init_t m_xvid_gbl_init;
	xvid_dec_create_t m_xvid_dec_create;
	xvid_dec_frame_t m_xvid_dec_frame;
};

#endif
