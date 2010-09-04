/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34ImageMask.h
 * Header file for the CS34ImageMask class
 *
 * This class enables the use of Mask for images that may require them.
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

#if !defined(AFX_S34IMAGEMASK_H__CBF6205C_00AC_11D3_96C2_00A0C982B57D__INCLUDED_)
#define AFX_S34IMAGEMASK_H__CBF6205C_00AC_11D3_96C2_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

#include "S34GRAErrors.h"

class DLL_EXP CS34ImageMask  
{
public:
	CS34ImageMask();
	CS34ImageMask(UINT width, UINT height);
	virtual ~CS34ImageMask();

	void Create(UINT width, UINT height, BOOL bMask = FALSE);
	void Initialize(BOOL bMask);
	void SetPixel(UINT x, UINT y, BOOL bMask);
	BOOL GetPixel(UINT x, UINT y);

    void SetBuffer(BYTE *&pMaskBuff, UINT width, UINT height);
	void GetBuffer(BYTE *&pMaskBuff, UINT &iSize);

	UINT GetWidth();
	UINT GetHeight();
	UINT GetSize();
private:
	UINT   m_width;
	UINT   m_height;
	UINT   m_TrueWidth;
	UINT   m_TrueHeight;
	UINT   m_size;
	BOOL   m_buffCreated;
	BYTE   *m_maskBuff;
};

#endif // !defined(AFX_S34IMAGEMASK_H__CBF6205C_00AC_11D3_96C2_00A0C982B57D__INCLUDED_)
