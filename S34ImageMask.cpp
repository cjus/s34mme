// S34ImageMask.cpp: implementation of the CS34ImageMask class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S34ImageMask.h"

static const BYTE mask[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS34ImageMask::CS34ImageMask()
{
	m_width = 0;
	m_height = 0;
	m_size = 0;
	m_buffCreated = FALSE;
	m_maskBuff = NULL;
}

CS34ImageMask::CS34ImageMask(UINT width, UINT height) 
{ 
	Create(width, height); 
}

CS34ImageMask::~CS34ImageMask()
{
	if (m_maskBuff && m_buffCreated)
		delete [] m_maskBuff;
}

//****************************************************
// Create 2 dimensional MASK array w/wo initialization
//****************************************************
void CS34ImageMask::Create(UINT width, UINT height, BOOL bMask) 
{
	if (m_maskBuff && m_buffCreated)
	{
		delete [] m_maskBuff;
		m_maskBuff = NULL;
	}

	m_TrueWidth = width;
	m_TrueHeight = height;

	m_width =  (width + 7) >> 3;
	m_height = height;
	m_size = m_width * m_height;
	m_buffCreated = TRUE;

	m_maskBuff = new BYTE [m_size];
	if (bMask)
		memset(m_maskBuff, 0xFF, m_size);  // initialize to 1s
	else
		memset(m_maskBuff, 0, m_size);     // initialize to all 0s
}

//**********************
// Initialize MASK array
//**********************
void CS34ImageMask::Initialize(BOOL bMask)
{
	if (bMask)
		memset(m_maskBuff, 0xFF, m_size);  // initialize to 1s
	else
		memset(m_maskBuff, 0, m_size);     // initialize to all 0s
}

//***************
// Set MASK value
//***************
void CS34ImageMask::SetPixel(UINT x, UINT y, BOOL bMask)
{
	if (bMask)
		m_maskBuff[y*m_width+(x>>3)] |= mask[x & 0x07];
	else
		m_maskBuff[y*m_width+(x>>3)] &= ~mask[x & 0x07];
}

//***************
// Get MASK value
//***************
BOOL CS34ImageMask::GetPixel(UINT x, UINT y)
{
	if (x < 0 || x > m_TrueWidth || y < 0 || y > m_TrueHeight)
		return FALSE;
	return (m_maskBuff[y*m_width+(x>>3)] & mask[x & 0x07]) ? TRUE : FALSE;
}

//******************************************************************
// Input an allocated mask buffer and copy the pointer, width & size
//******************************************************************
void CS34ImageMask::SetBuffer(BYTE *&pMaskBuff, UINT width, UINT height)
{
	if (m_maskBuff && m_buffCreated)
	{
		delete []m_maskBuff;
		m_maskBuff = NULL;
	}

	m_buffCreated = FALSE;
	m_maskBuff = pMaskBuff;
	m_width = width;
	m_height = height;
	m_size = m_width * m_height;
}

//**************************************************
// Get the buffer/size.  Ready for storing to a file
//**************************************************
void CS34ImageMask::GetBuffer(BYTE *&pMaskBuff, UINT &iSize)
{
	pMaskBuff = m_maskBuff;
	iSize = m_size;
}

UINT CS34ImageMask::GetWidth()
{ 
	return m_width; 
}

UINT CS34ImageMask::GetHeight() 
{ 
	return m_height; 
}

UINT CS34ImageMask::GetSize()   
{ 
	return m_size; 
}

