/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34Image.h
 * Header file for the CS34Image class.  
 *
 * The CS34Image object is the native image format used by the 
 * S34MME.DLL
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

#if !defined(AFX_S34IMAGE_H__3814057E_FD21_11D2_96B4_00A0C982B57D__INCLUDED_)
#define AFX_S34IMAGE_H__3814057E_FD21_11D2_96B4_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

#include "S34ImageDef.h"
#include "S34GRAErrors.h"
#include "S34ImageMask.h"

class CS34Palette;

class DLL_EXP CS34Image
{
public:
	CS34Image();
	virtual ~CS34Image();

	// Image Types
	enum // S34IMAGE_TYPES
	{
		eS34IMG_LARGE,
		eS34IMG_SMALL
	};

	// Scan line segment types
	enum // S34SEGMENT_TYPES 
	{
		eS34IMG_RAW,
		eS34IMG_RLE
	};

	int Save(char *pFileName);
	int Save(HANDLE hFile);

	int Load(char *pFileName);
	int Load(HANDLE hFile);

	int Unpack(BYTE *pData, int iSize);
	int Create(int width, int height, int TransparentColorIndex, BYTE bUseExternalPalette, BYTE bCreateMask, CS34Palette *pPalette, BYTE*pData);

	int SetExternalColorTable(RGBTRIPLE *pColorTable, BOOL bBitMode);

	int GetSize(int *pWidth, int *pHeight);

	int Clone(CS34Image *pSrcImg);

	int Render(BYTE *pDstBuffer, int iPitch,
				 int iDstX, int iDstY, 
				 int iImageOffsetX, int iImageOffsetY, 
				 int iImageWidth=0, int iImageHeight=0);
	int Render256(BYTE *pDstBuffer, int iPitch,
				 int iDstX, int iDstY, 
				 int iImageOffsetX, int iImageOffsetY, 
				 int iImageWidth=0, int iImageHeight=0);

	BOOL IsPointInImage(int x, int y);

	BOOL HasMask() { return m_bHasMask; }
private:
	BOOL m_bBitModePalette;
	BYTE m_bHasMask;
	BYTE m_bHasExternalPalette;

	int m_iColorDepth;

	BYTE *m_pData;
	int m_iDataCount;

	BYTE m_ImageType;
	LARGEIMAGEHEADER m_lih;
	SMALLIMAGEHEADER m_sih;

	CS34ImageMask *m_pImageMask;
	int	m_width;
	int	m_height;

	RGBTRIPLE *m_pColorTable;
	int m_iTotalColors;
	
	BOOL IncreaseBufferSize(BYTE *pBuffer, int iCurrentSize, int iNewSize);
};

#endif // !defined(AFX_S34IMAGE_H__3814057E_FD21_11D2_96B4_00A0C982B57D__INCLUDED_)
