// ImageManipulation.cpp: implementation of the CImageManipulation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageManipulation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageManipulation::CImageManipulation()
{

}

CImageManipulation::~CImageManipulation()
{

}

BOOL CImageManipulation::FlipImage(BYTE *pImageBuf, int width, int height, int color_depth)
{
	width *= (color_depth/8);

	BYTE *pDst = pImageBuf;
	BYTE *pSrc = pDst + (width * (height - 1));
	BYTE *pTmp = new BYTE[width];

	for (int i = 0; i < height / 2; i++)
	{
		memcpy(pTmp, pDst, width);
		memcpy(pDst, pSrc, width);
		memcpy(pSrc, pTmp, width);
		pSrc -= width;
		pDst += width;
	}

	delete []pTmp;
	return TRUE;
}

BOOL CImageManipulation::MirrorImage(BYTE *pImageBuf, int width, int height, int color_depth)
{
	return FALSE;
}
