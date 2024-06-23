// showwave.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\Client.h"
#include "ShowWave.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowWave

CShowWave::CShowWave()
{
	m_nSize     = 0;
	m_nBits     = -1;
	m_hWnd      = NULL;

	brush.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
}

CShowWave::~CShowWave()
{
}


BEGIN_MESSAGE_MAP(CShowWave, CStatic)
	//{{AFX_MSG_MAP(CShowWave)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowWave message handlers

void CShowWave::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	::GetClientRect(m_hWnd,rcClient);

	CBitmap Bitmap;
	CBitmap* pbmOld = NULL;
	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,1,RGB(172,168,153));
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	Bitmap.CreateCompatibleBitmap(&dc,rcClient.right,rcClient.bottom);
	pbmOld = dcMem.SelectObject(&Bitmap);
	dcMem.FillRect(rcClient,&brush);

	int x,y;
	if(m_nSize>0&&m_nBits>0)
	{
		if(m_nBits==8)
		{
			pOldPen = dcMem.SelectObject(&pen);
			dcMem.MoveTo(0,rcClient.bottom/2);
			for (x=0;x<m_nSize;x++)
			{
				y=(int)((rcClient.bottom/2)-(DrawData[x]-128));
				dcMem.LineTo(x,y);
			}
			dcMem.SelectObject(pOldPen);
		}
		else
		{
			short *pData = (short *)DrawData;
			int nCount = m_nSize/2;

			pOldPen = dcMem.SelectObject(&pen);
			dcMem.MoveTo(0,rcClient.bottom/2);
			for (x=0;x<nCount;x++)
			{
				y=(int)((rcClient.bottom/2)-(pData[x]-16384)*0.001);

				dcMem.LineTo(x,y-16);
			}
			dcMem.SelectObject(pOldPen);
		}
	}
	dc.BitBlt(0,0,rcClient.right,rcClient.bottom,&dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pbmOld);
	dcMem.DeleteDC();
	// Do not call CStatic::OnPaint() for painting messages
}

void  CShowWave::SetSoundInBuffer(BYTE *buff,int nSize,int nBits)
{
	ASSERT(buff!=0);
	ASSERT(nSize<=DRAWBUFFERSIZE);
	ASSERT(nBits==8||nBits==16);
	m_nBits = nBits;
	m_nSize = nSize;
	memcpy(DrawData,buff,nSize);
}

void CShowWave::SetHwnd(HWND hWnd)
{
	m_hWnd = hWnd;
}
