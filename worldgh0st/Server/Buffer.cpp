// Buffer.cpp: implementation of the CBuffer class.
//
//////////////////////////////////////////////////////////////////////
#include "Buffer.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CBuffer::CBuffer()
{
	// Initial size
	m_nSize = 0;
	m_pPtr = m_pBase = NULL;
}

CBuffer::~CBuffer()
{
	if (m_pBase)
		VirtualFree(m_pBase, 0, MEM_RELEASE);
}

BOOL CBuffer::Write(PBYTE pData, UINT nSize)
{	
	if (ReAllocateBuffer(nSize + GetBufferLen()) == -1)
	{
		return false;
	}

	CopyMemory(m_pPtr,pData,nSize);

	// Advance Pointer
	m_pPtr+=nSize;
	return nSize;
}

void *mymemmove(void *dst, const void *src, size_t count)
{
	void *ret = dst;
	if (dst <= src || (char *)dst >= ((char *)src + count))
	{
		// Non-Overlapping Buffers
		// copy from lower addresses to higher addresses
		while (count--)
		{
			*(char*)dst = *(char*)src;
			dst = (char*)dst + 1;
			src = (char*)src + 1;
		}
	}
	else
	{
		// Overlapping Buffers
		// copy from higher addresses to lower addresses
		dst = (char*)dst + count - 1;
		src = (char*)src + count - 1;
		
		while (count--)
		{
			*(char*)dst = *(char*)src;
			dst = (char*)dst - 1;
			src = (char*)src - 1;
		}
	}
	
	return ret;
}

UINT CBuffer::Read(PBYTE pData, UINT nSize)
{
	// Trying to byte off more than ya can chew - eh?
	if (nSize > GetMemSize())
	{
		return 0;
	}

	// all that we have 
	if (nSize > GetBufferLen())
		nSize = GetBufferLen();

		
	if (nSize)
	{
		// Copy over required amount and its not up to us
		// to terminate the buffer - got that!!!
		CopyMemory(pData,m_pBase,nSize);

		// Slide the buffer back - like sinking the data
		mymemmove(m_pBase,m_pBase+nSize,GetMemSize() - nSize);

		m_pPtr -= nSize;
	}
		
	DeAllocateBuffer(GetBufferLen());

	return nSize;
}

UINT CBuffer::GetMemSize() 
{
	return m_nSize;
}

UINT CBuffer::GetBufferLen() 
{
	if (m_pBase == NULL)
		return 0;

	int nSize = 
		m_pPtr - m_pBase;
	return nSize;
}

UINT CBuffer::ReAllocateBuffer(UINT nRequestedSize)
{

	if (nRequestedSize < GetMemSize())
		return 0;

	// Allocate new size
	UINT nNewSize = (UINT) ceil(nRequestedSize / 1024.0) * 1024 ;

	// New Copy Data Over
	PBYTE pNewBuffer = (PBYTE) VirtualAlloc(NULL,nNewSize,MEM_COMMIT,PAGE_READWRITE);
	if (pNewBuffer == NULL)
		return -1;

	UINT nBufferLen = GetBufferLen();
	if(nBufferLen > 0)
		CopyMemory(pNewBuffer,m_pBase,nBufferLen);

	if (m_pBase)
		VirtualFree(m_pBase,0,MEM_RELEASE);

 
	// Hand over the pointer
	m_pBase = pNewBuffer;

	// Realign position pointer
	m_pPtr = m_pBase + nBufferLen;

	m_nSize = nNewSize;

	return m_nSize;
}

UINT CBuffer::DeAllocateBuffer(UINT nRequestedSize)
{
	if (nRequestedSize < GetBufferLen())
		return 0;

	// Allocate new size
	UINT nNewSize = (UINT) ceil(nRequestedSize / 1024.0) * 1024;

	if (nNewSize < GetMemSize())
		return 0;

	// New Copy Data Over
	PBYTE pNewBuffer = (PBYTE) VirtualAlloc(NULL,nNewSize,MEM_COMMIT,PAGE_READWRITE);
	if(pNewBuffer == NULL)
		return -1;

	UINT nBufferLen = GetBufferLen();
	if(nBufferLen > 0)
		CopyMemory(pNewBuffer,m_pBase,nBufferLen);

	VirtualFree(m_pBase,0,MEM_RELEASE);

	// Hand over the pointer
	m_pBase = pNewBuffer;

	// Realign position pointer
	m_pPtr = m_pBase + nBufferLen;

	m_nSize = nNewSize;

	return m_nSize;
}

void CBuffer::ClearBuffer()
{
	// Force the buffer to be empty
	if (m_pBase)
		VirtualFree(m_pBase,0,MEM_RELEASE);
	
	m_pPtr = m_pBase = NULL;
	m_nSize = 0;
}


PBYTE CBuffer::GetBuffer(UINT nPos)
{
	return m_pBase+nPos;
}
