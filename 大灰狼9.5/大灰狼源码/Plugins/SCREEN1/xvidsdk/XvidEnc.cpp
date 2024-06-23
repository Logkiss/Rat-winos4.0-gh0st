//#include "StdAfx.h"
#include "XvidEnc.h"

#ifdef _DEBUG
#pragma comment(lib, "./xvidsdk/libxvidcored.lib")
#else
#pragma comment(lib, "./xvidsdk/libxvidcore.lib")
#endif // _DEBUG
#include <string.h>
CXvidEnc::CXvidEnc(int version, unsigned int cpu_flags, int debug)
{
	m_version = version;
	m_bOpened = false;
	/* XviD core initialization */
	m_xvid_gbl_init.version = version;
	m_xvid_gbl_init.cpu_flags = cpu_flags;
	m_xvid_gbl_init.debug = debug;
	if (xvid_global(NULL, XVID_GBL_INIT, &m_xvid_gbl_init, NULL) < 0)
		m_bInited = false;
	else
		m_bInited = true;
}

CXvidEnc::~CXvidEnc()
{
	
}

bool CXvidEnc::Open(int width, int height, int fps, int cbit, int quant)
{
	if (!m_bInited) return false;
	if (m_bOpened) return false;
	
	int csp;
	switch (cbit)
	{
	case 16:
		csp = XVID_CSP_RGB555 | XVID_CSP_VFLIP;
		break;
	case 24:
		csp = XVID_CSP_BGR | XVID_CSP_VFLIP;
		break;
	case 32:
		csp = XVID_CSP_BGRA | XVID_CSP_VFLIP;
		break;
	default:
		return false;
	}
	
	/* XviD encoder initialization */
	memset(&m_xvid_enc_create, 0, sizeof(m_xvid_enc_create));
	m_xvid_enc_create.version = m_version;
	m_xvid_enc_create.profile = XVID_PROFILE_S_L0;
	m_xvid_enc_create.width = width;
	m_xvid_enc_create.height = height;
	m_xvid_enc_create.fincr = 1;
	m_xvid_enc_create.fbase = fps; // 设置FPS值(帧率), 必须设置
	if (xvid_encore(NULL, XVID_ENC_CREATE, &m_xvid_enc_create, NULL) < 0)
		return false;
	
	/* XviD frame initialization */
	memset(&m_xvid_enc_frame, 0, sizeof(m_xvid_enc_frame));
	m_xvid_enc_frame.version = m_version;
	m_xvid_enc_frame.quant = quant;
	m_xvid_enc_frame.input.csp = csp;
	m_xvid_enc_frame.input.stride[0] = (m_xvid_enc_create.width * cbit / 8 + 3) / 4 * 4;
	
	m_bOpened = true;
	return true;
}

int CXvidEnc::Encode(void *input, void *output, int outlen)
{
	if (!m_bOpened) return -1;
	
	m_xvid_enc_frame.input.plane[0] = input;
	m_xvid_enc_frame.bitstream = output;
	m_xvid_enc_frame.length = outlen;
	
	return xvid_encore(m_xvid_enc_create.handle, XVID_ENC_ENCODE, &m_xvid_enc_frame, NULL);
}

bool CXvidEnc::Close()
{
	if (m_bOpened && xvid_encore(m_xvid_enc_create.handle, XVID_ENC_DESTROY, NULL, NULL) >= 0)
	{
		m_bOpened = false;
		return true;
	}
	else return false;
}
