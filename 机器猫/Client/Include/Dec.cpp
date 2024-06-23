#include <Windows.h>
#include "dec.h"
#include "xvid.h"

CXvidDec1::CXvidDec1() 
{
	m_width = 0 ; 
	m_height = 0 ; 
	m_image = NULL ; 
	m_dec_handle = NULL ; 
}

CXvidDec1::~CXvidDec1()
{
	if(m_image) 
		delete [] m_image ; 
	m_image = NULL ; 
}

void CXvidDec1::AttachCaller(int width, int height) 
{
	m_width = width ;
	m_height = height ; 
	if((m_width > 0) && (m_height > 0)) 
	{
		m_image = new unsigned char [m_width * m_height * 3] ; 
		memset(m_image, 0, m_width * m_height * 3) ; 
	}
}

bool CXvidDec1::Close()
{
	int xerr = 0 ; 

	/* Destroy the encoder instance */
	xerr = xvid_decore(m_dec_handle, XVID_ENC_DESTROY, NULL, NULL); 

	return (xerr) ? false : true ; 
}

void CXvidDec1::XVID_GLOBAL_INIT() {
	/*------------------------------------------------------------------------
	 * XviD core initialization
	 *----------------------------------------------------------------------*/
	xvid_gbl_init_t xvid_gbl_init;
	memset(&xvid_gbl_init, 0, sizeof(xvid_gbl_init));
	xvid_gbl_init.version = XVID_VERSION;
	xvid_gbl_init.cpu_flags = 0;//XVID_CPU_FORCE | XVID_CPU_ASM | XVID_CPU_SSE ; // force to use asm optimized routine
	/* Initialize XviD core -- Should be done once per __process__ */
	xvid_global(NULL, XVID_GBL_INIT, &xvid_gbl_init, NULL);
}

bool CXvidDec1::Open() 
{
	static xvid_dec_create_t xvid_dec_create ;
	int ret = 0; 
	/*------------------------------------------------------------------------
	 * XviD encoder initialization
	 *----------------------------------------------------------------------*/
	
	memset(&xvid_dec_create, 0, sizeof(xvid_dec_create_t));
	xvid_dec_create.version = XVID_VERSION;
	/* Width and Height of input frames */
	xvid_dec_create.width = m_width ;
	xvid_dec_create.height = m_height ;
	ret = xvid_decore(NULL, XVID_DEC_CREATE, &xvid_dec_create, NULL) ;
	m_dec_handle = xvid_dec_create.handle;
	return true;
}

LPBYTE CXvidDec1::Decode(unsigned char * xvid, int xvid_len,DWORD &dRet) 
{
	int ret = 0;
	ret = dec_core(xvid, m_image, xvid_len); 
 	if (ret > 0)
	{
		dRet = ret;
		return m_image;
	}
	return NULL;
// 		m_dec_caller->PostDecHandler(m_image, ret) ; 	
}

 /* raw xvid_encode procedure  */
int CXvidDec1::dec_core(unsigned char *bitstream,unsigned char *image, int bs_size)
{
	int ret;
	xvid_dec_frame_t xvid_dec_frame;
	/* Reset all structures */
	memset(&xvid_dec_frame, 0, sizeof(xvid_dec_frame_t));
	
	/* Set version */
	xvid_dec_frame.version = XVID_VERSION;
	//xvid_dec_stats->version = XVID_VERSION;
	
	/* No general flags to set */
	xvid_dec_frame.general          = 0;
	
	/* Input stream */
	xvid_dec_frame.bitstream        = bitstream;
	xvid_dec_frame.length           = bs_size;
	
	/* Output frame structure */
	xvid_dec_frame.output.plane[0]  = image;

	xvid_dec_frame.output.stride[0] = m_width*3;
	xvid_dec_frame.output.csp = XVID_CSP_BGR|XVID_CSP_VFLIP;

	ret = xvid_decore(m_dec_handle, XVID_DEC_DECODE, &xvid_dec_frame, NULL);

	return(ret);    
}