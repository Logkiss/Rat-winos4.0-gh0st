#include "xvidenc.h"
#include "xvid.h"

static const int motion_presets[] = {
	/* quality 0 */
	0,
	/* quality 1 */
	XVID_ME_ADVANCEDDIAMOND16,
	/* quality 2 */
	XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16,
	/* quality 3 */
	XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 |
	XVID_ME_ADVANCEDDIAMOND8 | XVID_ME_HALFPELREFINE8,
	/* quality 4 */
	XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 |
	XVID_ME_ADVANCEDDIAMOND8 | XVID_ME_HALFPELREFINE8 |
	XVID_ME_CHROMA_PVOP | XVID_ME_CHROMA_BVOP,
	/* quality 5 */
	XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 |
	XVID_ME_ADVANCEDDIAMOND8 | XVID_ME_HALFPELREFINE8 |
	XVID_ME_CHROMA_PVOP | XVID_ME_CHROMA_BVOP,
	/* quality 6 */
	XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 | XVID_ME_EXTSEARCH16 |
	XVID_ME_ADVANCEDDIAMOND8 | XVID_ME_HALFPELREFINE8 | XVID_ME_EXTSEARCH8 |
	XVID_ME_CHROMA_PVOP | XVID_ME_CHROMA_BVOP,
};

#define ME_ELEMENTS (sizeof(motion_presets)/sizeof(motion_presets[0]))

static const int vop_presets[] = {
	/* quality 0 */
	0,
	/* quality 1 */
	0,
	/* quality 2 */
	XVID_VOP_HALFPEL,
	/* quality 3 */
	XVID_VOP_HALFPEL | XVID_VOP_INTER4V,
	/* quality 4 */
	XVID_VOP_HALFPEL | XVID_VOP_INTER4V,
	/* quality 5 */
	XVID_VOP_HALFPEL | XVID_VOP_INTER4V |
	XVID_VOP_TRELLISQUANT,
	/* quality 6 */
	XVID_VOP_HALFPEL | XVID_VOP_INTER4V |
	XVID_VOP_TRELLISQUANT | XVID_VOP_HQACPRED,
};

#define VOP_ELEMENTS (sizeof(vop_presets)/sizeof(vop_presets[0]))
//////////////////////////////////////////////////////////////////////////
#define MAX_ZONES   64
/* Maximum number of frames to encode */
#define ABS_MAXFRAMENR 9999
static int ARG_STATS = 0;
static int ARG_DUMP = 0;
static int ARG_LUMIMASKING = 0;
static int ARG_BITRATE = 0;
static int ARG_SINGLE = 0;
static char *ARG_PASS1 = 0;
static char *ARG_PASS2 = 0;
static int ARG_QUALITY = ME_ELEMENTS - 1;
static float ARG_FRAMERATE = 25.00f;
static int ARG_MAXFRAMENR = ABS_MAXFRAMENR;
static int ARG_MAXKEYINTERVAL = 0;
static char *ARG_INPUTFILE = NULL;
static int ARG_INPUTTYPE = 0;
static int ARG_SAVEMPEGSTREAM = 0;
static int ARG_SAVEINDIVIDUAL = 0;
static char *ARG_OUTPUTFILE = NULL;
static int ARG_BQRATIO = 150;
static int ARG_BQOFFSET = 100;
static int ARG_MAXBFRAMES = 0;
static int ARG_PACKED = 0;
static int ARG_VOPDEBUG = 0;
static int ARG_GMC = 0;
static int ARG_INTERLACING = 0;
static int ARG_QPEL = 0;
static int ARG_CLOSED_GOP = 0;

#ifndef READ_PNM
#define IMAGE_SIZE(x,y) ((x)*(y)*3/2)
#else
#define IMAGE_SIZE(x,y) ((x)*(y)*3)
#endif

#define MAX(A,B) ( ((A)>(B)) ? (A) : (B) )
#define SMALL_EPS (1e-10)
#define SWAP(a) ( (((a)&0x000000ff)<<24) | (((a)&0x0000ff00)<<8) | (((a)&0x00ff0000)>>8)  | (((a)&0xff000000)>>24) )

//////////////////////////////////////////////////////////////////////////
CXvidEnc::CXvidEnc()
{
	m_enc_handle = NULL ; 
	m_key = 0 ; 
	m_width = 0 ; 
	m_height = 0 ; 
	memset(m_bitstream, 0, MAX_ENC_BUF) ; 
}
bool CXvidEnc::Close() 
{
	int xerr = 0 ; 
	//m_closed = true;

	/* Destroy the encoder instance */
	xerr = xvid_encore(m_enc_handle, XVID_ENC_DESTROY, NULL, NULL); 
	return (xerr) ? false : true ; 
}

void CXvidEnc::AttachCaller(int width, int height)
{
	m_width = width ; 
	m_height = height ; 
}

void CXvidEnc::XVID_GLOBAL_INIT(){
	/*------------------------------------------------------------------------
	 * XviD core initialization
	 *----------------------------------------------------------------------*/
	xvid_gbl_init_t xvid_gbl_init;
	memset(&xvid_gbl_init, 0, sizeof(xvid_gbl_init));
	xvid_gbl_init.version = XVID_VERSION;
	xvid_gbl_init.cpu_flags = XVID_CPU_FORCE | XVID_CPU_ASM; // here we use asm optimized code
	/* Initialize XviD core -- Should be done once per __process__ */
	xvid_global(NULL, XVID_GBL_INIT, &xvid_gbl_init, NULL);
}

bool CXvidEnc::Open() 
{
	static xvid_enc_create_t xvid_enc_create;
	int xerr = 0; 

	/*------------------------------------------------------------------------
	* XviD encoder initialization
	*----------------------------------------------------------------------*/
	memset(&xvid_enc_create, 0, sizeof(xvid_enc_create));
	xvid_enc_create.version = XVID_VERSION;

	/* Width and Height of input frames */
	xvid_enc_create.width = m_width ; 
	xvid_enc_create.height = m_height ; 
	xvid_enc_create.profile = XVID_PROFILE_AS_L4;

	/* init plugins  */
	/*
	xvid_enc_create.zones = ZONES;
	xvid_enc_create.num_zones = NUM_ZONES;
	xvid_enc_create.plugins = plugins;
	xvid_enc_create.num_plugins = 0;
	*/
	/* No fancy thread tests */
	xvid_enc_create.num_threads = 0;

	/* Frame rate - Do some quick float fps = fincr/fbase hack */    
	xvid_enc_create.fincr = 1;
	xvid_enc_create.fbase = (int)15;

	/* Maximum key frame interval */
	xvid_enc_create.max_key_interval = (int)-1;    //--default 10s
	
	/* Bframes settings */
	xvid_enc_create.max_bframes = ARG_MAXBFRAMES;
	xvid_enc_create.bquant_ratio = ARG_BQRATIO;
	xvid_enc_create.bquant_offset = ARG_BQOFFSET;
	
	/* Dropping ratio frame -- we don't need that */
	xvid_enc_create.frame_drop_ratio = 0;//50;
	
	/* Global encoder options */
	xvid_enc_create.global = 0;
	if (ARG_PACKED)
		xvid_enc_create.global |= XVID_GLOBAL_PACKED;

	if (ARG_CLOSED_GOP)
		xvid_enc_create.global |= XVID_GLOBAL_CLOSED_GOP;

	if (ARG_STATS)
		xvid_enc_create.global |= XVID_GLOBAL_EXTRASTATS_ENABLE;

	/* I use a small value here, since will not encode whole movies, but short clips */
	xerr = xvid_encore(NULL, XVID_ENC_CREATE, &xvid_enc_create, NULL);
	
	m_enc_handle = xvid_enc_create.handle;
	
	return true;
}

LPBYTE CXvidEnc::Encode(unsigned char * image,DWORD& dRet)
{
	UINT ret = 0 ;    

	ret = enc_core(image, m_bitstream, &m_key) ; 
	if (ret > 0)
	{		
		dRet = ret;
		return m_bitstream;
	}
	return NULL;
	// really encode some images into xvid data
	// 	if (ret > 0)
	// 		m_enc_caller->PostEncHandler(m_bitstream, m_key, ret) ;
}

/*
	raw CXvidEnc procedure
*/
int CXvidEnc::enc_core(unsigned char *image,unsigned char *bitstream, int * key)
{
	int ret;    
	xvid_enc_frame_t xvid_enc_frame;
	xvid_enc_stats_t xvid_enc_stats;
	    
	/* Version for the frame and the stats */
	memset(&xvid_enc_frame, 0, sizeof(xvid_enc_frame));
	xvid_enc_frame.version = XVID_VERSION;

	memset(&xvid_enc_stats, 0, sizeof(xvid_enc_stats));
	xvid_enc_stats.version = XVID_VERSION;

	/* Bind output buffer */
	xvid_enc_frame.bitstream = bitstream;
	xvid_enc_frame.length = MAX_ENC_BUF;
	    
	/* Initialize input image fields */
	xvid_enc_frame.input.plane[0] = image;

/*
	typedef struct 
	{
		int csp;				/ * [in] 色彩;或与XVID_CSP_VFLIP执行垂直翻转* / * /
		void * plane[4];		/ * [in] image plane ptrs * /
		int stride[4];			/ * [in] 图像跨步;“字节每一行”* /
	} xvid_image_t;
*/

//#ifdef MY_TEST
	xvid_enc_frame.input.csp = XVID_CSP_BGR|XVID_CSP_VFLIP; // suppose we get data from usb web cam
	xvid_enc_frame.input.stride[0] = m_width*3;
// #else
// 	xvid_enc_frame.input.csp = XVID_CSP_RGBA; // suppose we get data from usb web cam
// 	xvid_enc_frame.input.stride[0] = m_width*4;
// #endif
	/* Set up core's general features */
	xvid_enc_frame.vol_flags = 0;
	     
	/* Set up core's general features */
	xvid_enc_frame.vop_flags = vop_presets[ARG_QUALITY-2];
	   
	/* Frame type -- let core decide for us */
	xvid_enc_frame.type = XVID_TYPE_AUTO;
	    
	/* Force the right quantizer -- It is internally managed by RC plugins */
	xvid_enc_frame.quant = 0;

	/* Set up motion estimation flags */
	xvid_enc_frame.motion = motion_presets[ARG_QUALITY-2];
	/* We don't use special matrices */
	xvid_enc_frame.quant_intra_matrix = NULL;
	xvid_enc_frame.quant_inter_matrix = NULL;

	/* Encode the frame */
	ret = xvid_encore(m_enc_handle, XVID_ENC_ENCODE, &xvid_enc_frame,NULL);
	//    &xvid_enc_stats);
	//--判别是否是关键帧
	*key = (xvid_enc_frame.out_flags & XVID_KEYFRAME);
	//*stats_type = xvid_enc_stats.type;
	//*stats_quant = xvid_enc_stats.quant;
	//*stats_length = xvid_enc_stats.length;
	//sse[0] = xvid_enc_stats.sse_y;
	//sse[1] = xvid_enc_stats.sse_u;
	//sse[2] = xvid_enc_stats.sse_v;
	    
	return (ret);
}
