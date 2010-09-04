/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * JPEG.h
 * Header file for the CJPEG class
 *
 * Ability to decode JPEG images.
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

#if !defined(AFX_JPEG_H__38140578_FD21_11D2_96B4_00A0C982B57D__INCLUDED_)
#define AFX_JPEG_H__38140578_FD21_11D2_96B4_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

#include "S34GRAErrors.h"

class CS34Bitmap;
class CS34Palette;

class DLL_EXP CJPEG  
{
public:
	CJPEG();
	virtual ~CJPEG();

	int LoadBMPData(CS34Bitmap *pBitmap);
	int Save(char* pFileName, int iQuality);
	int GetImageLength(DWORD *pdwLength);
	int GetSize(int *pWidth, int *pHeight);
	int GetImageData(BYTE *pImgData, int iSize);
	int SaveToBMPFile(char *pFileName, int quality);
	int Load(char *pFileName, CS34Palette *pPalette);

private:
	BOOL m_bPaletteInUse;
	BYTE m_colormap[3][256];
	BYTE *m_pData;
	UINT m_width;
	UINT m_height;

	int ReadJPEGToRGBBuf(const char *fileName, UINT * width, UINT * height);
	int ReadJPEGToRGBBuf(const char *fileName, UINT * width, UINT * height, BYTE colormap[][256]);
	BOOL RGBToBGR(BYTE * buf, UINT widthPix, UINT height);
    void j_putRGBScanline(BYTE* jpegline, int widthPix, BYTE* outBuf,int row);
	int RGBToJpegFile(const char * fileName,BYTE  *dataBuf, UINT  width,UINT  height,int quality);
};

#endif // !defined(AFX_JPEG_H__38140578_FD21_11D2_96B4_00A0C982B57D__INCLUDED_)
