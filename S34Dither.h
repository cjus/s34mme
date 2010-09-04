/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34Dither.h
 * Header file for the CS34Dither class
 *
 * Dithers images.
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

#if !defined(AFX_S34DITHER_H__B8C9889A_01C2_11D3_96C6_00A0C982B57D__INCLUDED_)
#define AFX_S34DITHER_H__B8C9889A_01C2_11D3_96C6_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

#include "S34GRAErrors.h"
#include "S34Palette.h"

struct PALNODE //used to dither to 256-color palettes
{ 
    //internal palette format is 4 DWORDS: R, G, B, I
    DWORD rgbi[4]; //R, G, B, & pre-sort Index
    PALNODE*pL,*pR; //left & right node
    DWORD splitFlag; //0=end leaf, 1=R, 2=G, 3=B
    DWORD padding; //make it 32-byte long
};

#define MAX_PAL_LEN 256


class DLL_EXP CS34Dither  
{
public:
	CS34Dither() { Reset(); }
	~CS34Dither() {}
    void Reset(); 

    int SetPalette(CS34Palette *pPalette);
    int SetPaletteTrans(CS34Palette*pPalette, int iTransparentIndex);
    int Dither(BYTE *pData, int width, int height, 
                     CS34Palette *pSrcPalette);
    int DitherTrans(BYTE *pData, int width, int height, 
                     CS34Palette *pSrcPalette,int iTransparentIndex);
	int Dither888To555(BYTE *pData, int width, int height);
    
private:
    int GetBiggestColorRange(DWORD*pb, int numClr);
    int RemoveDuplicateEntries(DWORD*pBuf,int bufLen);
    void MatchScaledColor(int*pPixel,BYTE*pOut,int*pDif);
    inline PALNODE*NextUnusedNode();
    PALNODE*recursiveBuild(DWORD*pBuf,int bufLen);

    static int __cdecl sortcb_R  (const void*,const void*);
    static int __cdecl sortcb_G  (const void*,const void*);
    static int __cdecl sortcb_B  (const void*,const void*);
    static int __cdecl sortcb_RGB(const void*,const void*);

    PALNODE nodes[MAX_PAL_LEN*2],*pRoot;
    int usedNode;
    int transIndex; //transparent color index
    bool transMode;
};

#endif // !defined(AFX_S34DITHER_H__B8C9889A_01C2_11D3_96C6_00A0C982B57D__INCLUDED_)
