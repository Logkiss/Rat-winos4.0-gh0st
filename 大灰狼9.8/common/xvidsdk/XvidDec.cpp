#include "XvidDec.h"
#include <Windows.h>

#ifdef _DEBUG
#pragma comment(lib, "../common/xvidsdk/libxvidcored.lib")
#else
#pragma comment(lib, "../common/xvidsdk/libxvidcore.lib")
#endif // _DEBUG

CXvidDec::CXvidDec(int version, unsigned int cpu_flags, int debug)
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

CXvidDec::~CXvidDec()
{
	
}

bool CXvidDec::Open(int width, int height, int cbit)
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
	
	/* XviD decoder initialization */
	memset(&m_xvid_dec_create, 0, sizeof(m_xvid_dec_create));
	m_xvid_dec_create.version = m_version;
	m_xvid_dec_create.width = width;
	m_xvid_dec_create.height = height;
	if (xvid_decore(NULL, XVID_DEC_CREATE, &m_xvid_dec_create, NULL) < 0)
		return false;
	
	/* XviD frame initialization */
	memset(&m_xvid_dec_frame, 0, sizeof(m_xvid_dec_frame));
	m_xvid_dec_frame.version = m_version;
	m_xvid_dec_frame.output.csp = csp;
	m_xvid_dec_frame.output.stride[0] = (m_xvid_dec_create.width * cbit / 8 + 3) / 4 * 4;
	
	m_bOpened = true;
	return true;
}

int CXvidDec::Decode(void *input, int inlen, void *output)
{
	if (!m_bOpened) return -1;
	
	m_xvid_dec_frame.bitstream = input;
	m_xvid_dec_frame.length = inlen;
	m_xvid_dec_frame.output.plane[0] = output;
	
	return xvid_decore(m_xvid_dec_create.handle, XVID_DEC_DECODE, &m_xvid_dec_frame, NULL);
}

bool CXvidDec::Close()
{
	if (m_bOpened && xvid_decore(m_xvid_dec_create.handle, XVID_DEC_DESTROY, NULL, NULL) >= 0)
	{
		m_bOpened = false;
		return true;
	}
	else return false;
}
