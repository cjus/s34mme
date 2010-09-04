/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34Bitmap.h
 * Header file for the CS34Bitmap class
 *
 * Windows BMP handler
 *
 * This source code contains trade secrets and copyrighted materials, 
 * which is the intellectual property of Section34 Inc. 
 *
 * This source code is being licensed to SelvaSoft Inc. under contract
 * for a limited time only, for the purpose of evaluation and early
 * business development.  This code may not be used in an actual 
 * project or service.  Reverse engineering and the removal of this
 * header is expressly prohibited.
 * 
 * Unauthorized use, copying or distribution is a violation of U.S. 
 * and international laws and is strictly prohibited.
 *
 * Send inquires to: support@section34.com or contract the address 
 * or phone number shown below.
 *
 * (c)1998-99 Section34 Inc.
 * 5250 Colodny Drive, #5
 * Agoura Hills, CA 91301
 * 818 371-5785
 *
 *********************************************************************/

#if !defined(AFX_BITMAP_H__38140579_FD21_11D2_96B4_00A0C982B57D__INCLUDED_)
#define AFX_BITMAP_H__38140579_FD21_11D2_96B4_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

#include "S34GRAErrors.h"

class CS34Palette;

class DLL_EXP CS34Bitmap  
{
public:
	CS34Bitmap();
	virtual ~CS34Bitmap();

	int GetPalette(CS34Palette *pPalette);
	int GetImageData(BYTE *pImgData, int iSize);
	int GetColorDepth(int *pColorDepth);
	int GetSize(int *pWidth, int *pHeight);
	int Create(int width, int height, int depth, CS34Palette *pPalette, BYTE *pData);
	int Save(char* pFileName);
	int Load(char* pFileName);
	int GetColorIndex(BYTE red, BYTE green, BYTE blue, int *pIndex);
	
private:
	BYTE *m_pData;
	BITMAPINFOHEADER m_InfoHeader;
	RGBQUAD m_colortable[256];
	int m_TotalPaletteEntries;
	int m_RowLength;
	void ComputeRowLength(int iWidth, int iBitCount);
};

#endif // !defined(AFX_BITMAP_H__38140579_FD21_11D2_96B4_00A0C982B57D__INCLUDED_)
