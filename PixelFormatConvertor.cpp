// PixelFormatConvertor.cpp: implementation of the CPixelFormatConvertor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PixelFormatConvertor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPixelFormatConvertor::CPixelFormatConvertor()
{

}

CPixelFormatConvertor::~CPixelFormatConvertor()
{

}

int CPixelFormatConvertor::Conv24to16(BYTE *pSrc, int width, int height, BYTE *pDst)
{
	// convert image data 
	//
	WORD wPixel;
	WORD *pwDst = (WORD*)pDst;

	int iSize = width*height*3;

	for (int j= 0; j<iSize; j+=3)	// for each pixel on the scan line
	{
		// Convert 24 bit pixel data into 16 bit format
		//
		wPixel = (WORD)(((pSrc[j+2] & 0xF8) << 8) | ((pSrc[j+1] & 0xFC) << 3) | ((pSrc[j] & 0xF8) >> 3)); 

		// Store pixel in dst
		*pwDst = wPixel;
		pwDst++;
	}
	return S_OK;
}

int CPixelFormatConvertor::Conv15to16(BYTE *pData, int width, int height)
{
	WORD *pSrc = (WORD*)pData;
	WORD *pDst = pSrc;
	WORD wData1, wData2;
	int iSize = width*height;

	for (int j= 0; j<iSize; j++)
	{
		wData1 = *pSrc;
		wData2 = ((wData1 << 1) & 0xf800) | 
				 ((wData1 << 1) & 0x07e0) | 
				 (wData1 & 0x001F);
		*pDst = wData2;
		pDst++;
		pSrc++;
	}
	return S_OK;
}

int CPixelFormatConvertor::ConvPelFormat(BYTE *pData, int width, int height, int RedMask, int GreenMask, int BlueMask)
{
	WORD *pSrc = (WORD*)pData;
	WORD *pDst = pSrc;
	WORD wData1, wData2;
	int iSize = width*height;

	for (int j= 0; j<iSize; j++)
	{
		wData1 = *pSrc;
		wData2 = ((wData1 >> 1) & RedMask) | 
				 ((wData1 >> 1) & GreenMask) | 
				 (wData1 & BlueMask);
		*pDst = wData2;
		pDst++;
		pSrc++;
	}
	return S_OK;
}
