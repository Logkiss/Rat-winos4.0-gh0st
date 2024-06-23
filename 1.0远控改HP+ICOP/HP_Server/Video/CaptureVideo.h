/////////////////////////////////////////////////////////////////////
#if !defined(AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_)
#define AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_
/////////////////////////////////////////////////////////////////////
// CaptureVideo.h : header file
/////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <atlbase.h>
#include <windows.h>
#include <dshow.h>
#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include "include/qedit.h"

#define  MYDEBUG   1
#ifndef srelease
#define srelease(x) \
	if ( NULL != x ) \
{ \
	x->Release( ); \
	x = NULL; \
}
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE( x ) \
	if ( NULL != x ) \
{ \
	x->Release( ); \
	x = NULL; \
}
#endif

#define RGB2GRAY(r,g,b) (((b)*117 + (g)*601 + (r)*306) >> 10)

//发送和拷贝的 状态
enum{
	CMD_CAN_COPY,
	CMD_CAN_SEND
};
//搜索一下这个类的对象的定义
class CSampleGrabberCB : public ISampleGrabberCB 
{
       public:
           long       lWidth ; 
           long       lHeight ; 
            //CVdoFrameHandler * frame_handler ; 
            BOOL       bGrabVideo ; 
			BYTE* buf;         //位图数据
			BYTE  state;       //1，可以copy ,2 可以发送
        	DWORD bufSize;      //数据大小
			BITMAPINFO	*lpbmi;     //位图文件头 
			BYTE*   m_lpFullBits;     //图像数据
			DWORD   dwSize;             //数据大小
			BOOL   bStact;           //0可以复制  1可以发送

public:
       CSampleGrabberCB(){ 
            lWidth = 0 ; 
            lHeight = 0 ; 
             bGrabVideo = FALSE ; 
			 state=0;
			 lpbmi=NULL;
		     m_lpFullBits=NULL;
			 dwSize=0;
			 bStact=CMD_CAN_COPY;
//             frame_handler = NULL ; 
	   } 
        ~CSampleGrabberCB()
		{
			if (m_lpFullBits!=NULL)
			{
				delete[] m_lpFullBits;
			}
			if (lpbmi!=NULL)
			{
				delete[] lpbmi;
			}
		}
       STDMETHODIMP_(ULONG) AddRef() { return 2; }
       STDMETHODIMP_(ULONG) Release() { return 1; }

STDMETHODIMP QueryInterface(REFIID riid, void ** ppv)
	   
{
             if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ){ 
                     *ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
                     return NOERROR;
			  } 
              return E_NOINTERFACE;
	   
}
STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample ) 
 {
             return 0;
	 
 }
	  //回调函数 在这里得到 bmp 的数据
 STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize )
	  {
		  if (!pBuffer)return E_POINTER;
		 
		  if (bStact==CMD_CAN_COPY)         //未初始化 发送的同差异的一样
		  {
               memcpy(m_lpFullBits,pBuffer,lBufferSize);     //从这里得到视频的数据，他为什么能得到视频的数据呢？？
			                                                  //我们到他的类声明看一下
			   InterlockedExchange((LPLONG)&bStact,CMD_CAN_SEND);      //原子自增可以发送  
		  }

		return 0;
}
LPBITMAPINFO ConstructBI(int biBitCount, bool m_bIsGray, int biWidth, int biHeight)
	 {
	 /*
	 biBitCount 为1 (黑白二色图) 、4 (16 色图) 、8 (256 色图) 时由颜色表项数指出颜色表大小
	 biBitCount 为16 (16 位色图) 、24 (真彩色图, 不支持) 、32 (32 位色图) 时没有颜色表
		 */
		 
		 int	color_num = biBitCount <= 8 ? 1 << biBitCount : 0;
		 
		 int nBISize = sizeof(BITMAPINFOHEADER) + (color_num * sizeof(RGBQUAD));
		 lpbmi = (BITMAPINFO *) new BYTE[nBISize];
		 
		 BITMAPINFOHEADER	*lpbmih = &(lpbmi->bmiHeader);
		 lpbmih->biSize = sizeof(BITMAPINFOHEADER);
		 lpbmih->biWidth = biWidth;
		 lpbmih->biHeight = biHeight;
		 lpbmih->biPlanes = 1;
		 lpbmih->biBitCount = biBitCount;
		 lpbmih->biCompression = BI_RGB;
		 lpbmih->biXPelsPerMeter = 0;
		 lpbmih->biYPelsPerMeter = 0;
		 lpbmih->biClrUsed = 0;
		 lpbmih->biClrImportant = 0;
		 lpbmih->biSizeImage = (((lpbmih->biWidth * lpbmih->biBitCount + 31) & ~31) >> 3) * lpbmih->biHeight;
		 
		 // 16位和以后的没有颜色表，直接返回
		 
		 /////////////////////初始化成员数据///////////////////////////////////////////////
		 dwSize=lpbmih->biSizeImage;
		 m_lpFullBits=new BYTE[dwSize+10];
		 ZeroMemory(m_lpFullBits,dwSize+10);
		 if (biBitCount >= 16)
			 return lpbmi;
			 /*
			 Windows 95和Windows 98：如果lpvBits参数为NULL并且GetDIBits成功地填充了BITMAPINFO结构，那么返回值为位图中总共的扫描线数。
			 
			   Windows NT：如果lpvBits参数为NULL并且GetDIBits成功地填充了BITMAPINFO结构，那么返回值为非0。如果函数执行失败，那么将返回0值。Windows NT：若想获得更多错误信息，请调用callGetLastError函数。
		 */
		 
		 HDC	hDC = GetDC(NULL);
		 HBITMAP hBmp = CreateCompatibleBitmap(hDC, 1, 1); // 高宽不能为0
		 GetDIBits(hDC, hBmp, 0, 0, NULL, lpbmi, DIB_RGB_COLORS);
		 ReleaseDC(NULL, hDC);
		 DeleteObject(hBmp);
		 
		 if (m_bIsGray)
		 {
			 for (int i = 0; i < color_num; i++)
			 {
				 int color = RGB2GRAY(lpbmi->bmiColors[i].rgbRed, lpbmi->bmiColors[i].rgbGreen, lpbmi->bmiColors[i].rgbBlue);
				 lpbmi->bmiColors[i].rgbRed = lpbmi->bmiColors[i].rgbGreen = lpbmi->bmiColors[i].rgbBlue = color;
			 }
		 }
		 return lpbmi;	
}

LPBITMAPINFO GetBmpInfo()	  
{
	if (lpbmi==NULL)
	{
		lpbmi=ConstructBI(24, 0, lWidth, lHeight);
	}
    return lpbmi;	  
}
BYTE* GetNextScreen(DWORD &nSize)       //得到下一帧数据 
{
		  nSize=dwSize;
		  return (BYTE*)m_lpFullBits;      //还是很简单 就是返回一个缓冲区的指针，可这个指针是怎么获取到视频数据的呢？
		  //我们工程里搜索一下啊
	 
}	 
};
class CSampleGrabberCB;
class CCaptureVideo 
{
	friend class CSampleGrabberCB;public:
		DWORD GetBufferSize();
		LPBYTE GetDIB(DWORD& nSize);
		LPBITMAPINFO GetBmpInfo();
		void Init();
//	void GrabVideoFrames(BOOL bGrabVideoFrames, CVdoFrameHandler * frame_handler); 
    HRESULT Open(int iDeviceID,int iPress);
	HRESULT Close(); 
    int EnumDevices(BYTE* buf);
    CCaptureVideo();
    virtual ~CCaptureVideo();protected:
    HWND      m_hWnd;
    IGraphBuilder *    m_pGB;
    ICaptureGraphBuilder2* m_pCapture;
    IBaseFilter*    m_pBF;
    IMediaControl*    m_pMC;
    IVideoWindow*    m_pVW;
    ISampleGrabber*    m_pGrabber;protected:
    void FreeMediaType(AM_MEDIA_TYPE& mt);
    bool BindVideoFilter(int deviceId, IBaseFilter **pFilter);
    void ResizeVideoWindow();
    HRESULT SetupVideoWindow();
    HRESULT InitCaptureGraphBuilder();
	IPin* FindPin(IBaseFilter *pFilter, PIN_DIRECTION dir);
public:
	BYTE* EnumCompress();
	void SendEnd();                //发送结束
};
#endif // !defined(AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_)
