// CaptureVideo.cpp: implementation of the CCaptureVideo class.
//
//////////////////////////////////////////////////////////////////////

#include "CaptureVideo.h"

#include <atlbase.h>
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "strmbase.lib") 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCaptureVideo::CCaptureVideo()
{
	if(FAILED(CoInitialize(NULL))) /*, COINIT_APARTMENTTHREADED)))*/
		return;
	
	m_pCapture = NULL; 
	m_pGB = NULL;
	m_pMC = NULL;
	m_pBF = NULL; 
	m_pGrabber = NULL; 
    m_pNullRenderer = NULL;

	m_nWidth = 0;
	m_nHeight = 0;

	m_bIsStarted = FALSE;
	m_pEnc = NULL;
}

CCaptureVideo::~CCaptureVideo()
{
	if (m_pEnc)
	{
		if (m_bIsStarted)
		{
			m_pEnc->Close();
		}
		delete m_pEnc ;
        m_pEnc = NULL;
		Stop();
	}

	CoUninitialize(); 
}

LPBITMAPINFO CCaptureVideo::GetBmpInfo()
{
	return mCB.GetBmpInfo(); 
}

void  CCaptureVideo::SendEnd()            //发送结束  设置可以再取数据
{
	InterlockedExchange((LPLONG)&mCB.bStact,CMD_CAN_COPY);      //原子自增可以发送        //原子自减  发送完毕 可以copy 
}

LPBYTE CCaptureVideo::GetDIB(DWORD& nSize)
{
	BYTE *buf = NULL;

	if (mCB.bStact==CMD_CAN_SEND)      //这里改变了一下发送的状态
	{
		buf = mCB.GetNextScreen(nSize);
	}

	if (buf == NULL)
		return NULL;
    return m_pEnc->Encode(buf,nSize);
}


HRESULT CCaptureVideo::InitCaptureGraphBuilder()
{
	HRESULT hr;
	
	// 创建IGraphBuilder接口
	hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGB);
	// 创建ICaptureGraphBuilder2接口
	hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
		IID_ICaptureGraphBuilder2, (void **) &m_pCapture);
	if (FAILED(hr))
		return hr;

	m_pCapture->SetFiltergraph(m_pGB);
	hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);

	if (FAILED(hr))	
		return hr;
	return hr;
}

bool CCaptureVideo::BindFilter(int deviceId, IBaseFilter **pFilter)
{
	if (deviceId < 0)
		return false;
	
	// 举所有视频捕捉设备
	CComPtr<ICreateDevEnum> pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if (hr != NOERROR)
		return false;
	
	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
	if (hr != NOERROR) 
		return false;

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

void CCaptureVideo::FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0) 
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		// Strictly unnecessary but tidier
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL)
	{
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

HRESULT CCaptureVideo::Start(int iDeviceID, HWND hWnd , int width, int height)
{
	HRESULT hr;
	
	hr = InitCaptureGraphBuilder();
	if (FAILED(hr))
	   return hr;
	
	// 绑定设备过滤器。我们知道设备因为id
	if(!BindFilter(iDeviceID, &m_pBF))
		return S_FALSE;
	
	hr = m_pGB->AddFilter(m_pBF, L"Capture Filter");
	// create a sample grabber
	hr = CoCreateInstance( CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_ISampleGrabber, (void**)&m_pGrabber );
	if(FAILED(hr))		return hr;
	
	
	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID*) &m_pNullRenderer);
	if(FAILED(hr))		return hr;
	

	hr = m_pGB->AddFilter(m_pNullRenderer, L"NullRenderer");
	CComQIPtr< IBaseFilter, &IID_IBaseFilter > pGrabBase( m_pGrabber );
	
	//设置视频格式
	AM_MEDIA_TYPE mt; 
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24; // MEDIASUBTYPE_RGB24 ; 
	
	hr = m_pGrabber->SetMediaType(&mt);
	
	if( FAILED(hr) )
	{
		return hr;
	}
	hr = m_pGB->AddFilter( pGrabBase, L"Grabber" );
	if( FAILED( hr ) )
	{
		return hr;
	}

	IAMStreamConfig *pConfig;
	hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Interleaved,m_pBF,IID_IAMStreamConfig,(void **)&pConfig);
	if( hr != NOERROR )
	{
		hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,m_pBF,IID_IAMStreamConfig,(void **)&pConfig);
		if(hr != NOERROR)
			return hr;
	}	
	m_nWidth = width;
	m_nHeight = height;
	AM_MEDIA_TYPE   *mmt   =   NULL;   
	hr   =   pConfig-> GetFormat(&mmt); 
	VIDEOINFOHEADER *vvih = (VIDEOINFOHEADER*)mmt->pbFormat; 
	if(vvih == NULL)
	{
		return S_FALSE;
	}
	AM_MEDIA_TYPE *pmt = mmt; 
	VIDEOINFOHEADER *pvih = (VIDEOINFOHEADER*)pmt->pbFormat; 
	if(pvih == NULL)
	{
		return S_FALSE;
	}
	pvih->bmiHeader.biHeight=height;     //修改采集视频的高为240 
	pvih->bmiHeader.biWidth=width;       //修改采集视频的宽为320 
	pvih->AvgTimePerFrame = 10000000/10;
	pmt->pbFormat = (unsigned char*)pvih; 
	hr = pConfig->SetFormat(pmt);       //重新设置参数 
	if(FAILED(hr))
	{
		return hr;
	}
	SAFE_RELEASE(pConfig);
	// try to render preview/capture pin
	hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,m_pBF,pGrabBase,m_pNullRenderer);
	if( FAILED( hr ) )
	{
		hr = m_pCapture->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,m_pBF,pGrabBase,m_pNullRenderer);
		
		if( FAILED( hr ) )
			return hr;
		
	}

	mCB.lWidth = width;
	mCB.lHeight = height;
	FreeMediaType(mt);

	hr = m_pGrabber->SetBufferSamples( FALSE );
	hr = m_pGrabber->SetOneShot( FALSE );
	hr = m_pGrabber->SetCallback( &mCB, 1 );//设置视频捕获回调函数 也就是如果有视频数据时就会调用这个类的BufferCB函数

	m_pEnc = new CXvidEnc;

	if (m_pEnc == NULL)
	{
		return S_FALSE;
	}
	m_pEnc->AttachCaller(mCB.lWidth, mCB.lHeight) ; 
	m_pEnc->XVID_GLOBAL_INIT() ; 
	m_pEnc->Open();
    m_bIsStarted = TRUE;

	hr = m_pMC->Run();//开始视频捕捉
	if(FAILED(hr))
	{
		return hr;	// 失败
	}
	return S_OK;
}

HRESULT CCaptureVideo::Stop() 
{
	if(m_pMC)
		m_pMC->Stop();
//		m_pMC->StopWhenReady();
	
	SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pGB);
	SAFE_RELEASE(m_pBF);
	SAFE_RELEASE(m_pNullRenderer);
	SAFE_RELEASE(m_pGrabber); 
	SAFE_RELEASE(m_pCapture);
	
	m_pMC = NULL;
	m_pGB = NULL;
	m_pBF = NULL; 
	m_pNullRenderer = NULL;
	m_pGrabber = NULL; 
	m_pCapture = NULL; 
	return S_OK ; 
}

int CCaptureVideo::EnumDevices(DeviceInfo *head)
{
	if (!head) 
		return 0;
	
	DeviceInfo *ptr = head;
	int id = 0;//枚举视频扑捉设备
	//枚举视频扑捉设备
	ICreateDevEnum *pCreateDevEnum;   //ICreateDevEnum接口建立指定类型的列表
	ICaptureGraphBuilder2 *pBuilder = NULL;
	IBaseFilter *pVCap = NULL;
	IAMStreamConfig *pVSC = NULL;

	HRESULT hr = CoCreateInstance((REFCLSID)CLSID_CaptureGraphBuilder2,	NULL, CLSCTX_INPROC, (REFIID)IID_ICaptureGraphBuilder2,(void **)&pBuilder);
	if (hr != NOERROR)
		return -1;
	
	//用指定的类标识符创建一个未初始化的对象,这里的参数为CLSID_SystemDeviceEnum即枚举设备的对象
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if (hr != NOERROR)
		return -1;
	
	//指出要枚举那个类型的设备这里参数CLSID_VideoInputDeviceCategory枚举视频设备
	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
	if (hr != NOERROR)
		return -1;
	
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	
	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK) 
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

				char str[2048]; 
				if (ptr->deviceId != -1)
				{
					ptr->next = new DeviceInfo();
			    	ptr = ptr->next;
				}
					
				ptr->deviceId = id++;
				WideCharToMultiByte(CP_ACP,0,var.bstrVal, -1, str, 2048, NULL, NULL);
				ptr->friendlyName = str;
					
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
			
		hr = pM->BindToObject(0, 0, IID_IBaseFilter, (void**)&pVCap);
		if(SUCCEEDED(hr))
        {
			hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,pVCap, IID_IAMStreamConfig, (void **)&pVSC);
			if(hr != NOERROR)
			{
				hr = pBuilder->FindInterface(&PIN_CATEGORY_PREVIEW,&MEDIATYPE_Video,
					pVCap, IID_IAMStreamConfig, (void **)&pVSC);
			}
			
			if(SUCCEEDED(hr))
			{
				int iCount = 0, iSize = 0;
				hr = pVSC->GetNumberOfCapabilities(&iCount, &iSize);
				ResolutionInfo** pRes = &(ptr->resInfo);
				if(iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
				{
					for (int iFormat = 0; iFormat < iCount; iFormat++)
					{
						VIDEO_STREAM_CONFIG_CAPS scc;
						AM_MEDIA_TYPE *pmtConfig = NULL;
						hr = pVSC->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);
						if (SUCCEEDED(hr))
						{
							//分辨率
							if(HEADER(pmtConfig->pbFormat)->biWidth != 0 && HEADER(pmtConfig->pbFormat)->biHeight != 0)
							{
								char cResolution[32] = {0};
								sprintf(cResolution,"%d * %d:",HEADER(pmtConfig->pbFormat)->biWidth,HEADER(pmtConfig->pbFormat)->biHeight);
								*pRes = new ResolutionInfo;
								(*pRes)->Resolution = cResolution;
								pRes = &((*pRes)->next);
								
							}
						}
					}
				}
				pM->AddRef();
			}
		}

		pM->Release();
	}
	SAFE_RELEASE(pBuilder);
	SAFE_RELEASE(pVCap);
	SAFE_RELEASE(pVSC);
	return id;
}