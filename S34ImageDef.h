/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34ImageDef.h
 * Header file used by the S34Image class
 *
 * Header file used by the S34Image class
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
#ifndef _S34IMAGEDEF_H
#define _S34IMAGEDEF_H

#include "windows.h"

#include <pshpack2.h>
typedef struct tagANIMFILEHEADER
{
	WORD	afhWidth;
	WORD	afhHeight;
	WORD	afhTotalFrames;
	BYTE	afhFrameRate;
	WORD 	afhTransparentIndex;
	// Header is followed by 256 RGBTRIPLE Palette entries
} ANIMFILEHEADER;
#include <poppack.h>

#include <pshpack2.h>
typedef struct tagLARGEIMAGEHEADER
{
	BYTE	ihImageType;  // eS34IMG_LARGE
	BYTE	ihHasMask;
	BYTE	ihHasExternalPalette;
	WORD	ihWidth;
	WORD	ihHeight;
	WORD	ihTotalSegments;
	BYTE	ihTotalPaletteEntries;
	DWORD	ihRecordLength;
} LARGEIMAGEHEADER;
#include <poppack.h>

#include <pshpack2.h>
typedef struct tagLARGESCANLINEHEADER
{
	BYTE	slSegmentType;  // eS34IMG_RAW or eS34IMG_RLE
	BYTE	slColorIndex;
	WORD	slXOffset;
	WORD	slYOffset;
	WORD	slTotalPixels;
	// PixelData follows this portion if Segment type is RAW
} LARGESCANLINEHEADER;
#include <poppack.h>


#include <pshpack2.h>
typedef struct tagSMALLIMAGEHEADER
{
	BYTE	ihImageType; // eS34IMG_SMALL
	BYTE	ihHasMask;
	BYTE	ihHasExternalPalette;
	BYTE	ihWidth;
	BYTE	ihHeight;
	WORD	ihTotalSegments;
	BYTE	ihTotalPaletteEntries;
	DWORD	ihRecordLength;
} SMALLIMAGEHEADER;
#include <poppack.h>


#include <pshpack2.h>
typedef struct tagSMALLSCANLINEHEADER
{
	BYTE	slSegmentType; // eS34IMG_RAW or eS34IMG_RLE
	BYTE	slColorIndex;
	BYTE	slXOffset;
	BYTE	slYOffset;
	BYTE	slTotalPixels;
	// PixelData follows this portion if Segment type is RAW
} SMALLSCANLINEHEADER;
#include <poppack.h>

#endif // _S34IMAGEDEF_H
