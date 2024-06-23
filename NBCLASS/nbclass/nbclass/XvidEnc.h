#ifndef _XVID_ENCODE_H
#define _XVID_ENCODE_H

#include "xvid.h"

class CXvidEnc
{
public:
	CXvidEnc(int version = XVID_VERSION, unsigned int cpu_flags = 0, int debug = 0);
	~CXvidEnc();
	bool m_bOpened;
	bool Open(int width, int height, int fps, int cbit, int quant);
	int  Encode(void *input, void *output, int outlen);
	bool Close();

private:
	int  m_version;
	bool m_bInited;
	xvid_gbl_init_t m_xvid_gbl_init;
	xvid_enc_create_t m_xvid_enc_create;
	xvid_enc_frame_t m_xvid_enc_frame;
};

#endif
