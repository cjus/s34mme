/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34AnimFactory.h
 * Header file for the CS34AnimFactory class
 *
 * Class factory for Animation objects
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

#if !defined(AFX_ANIMFACTORY_H__593D3B44_0305_11D3_96CD_00A0C982B57D__INCLUDED_)
#define AFX_ANIMFACTORY_H__593D3B44_0305_11D3_96CD_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "S34AnmErrors.h"
#include "S34Palette.h"
#include "S34Dither.h"

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

class CS34Anim;
class CS34Image;

class DLL_EXP CS34AnimFactory  
{
public:
	static void Create();
	static void Destroy();
	static CS34AnimFactory *GetInstance();

	int SetDstPalette(CS34Palette *pPalette);
	int SetDestColorDepth(int iColorDepth);
	int CreateAnim(char *pAnimFileName, CS34Anim &AnimObject);

	enum S34COLORDEPTH
	{
		COLORDEPTH_256,
		COLORDEPTH_555,
		COLORDEPTH_565
	};

private:
	CS34AnimFactory();
	~CS34AnimFactory();

	static CS34AnimFactory *m_pInstance;

	int m_iColorDepth;
	CS34Palette *m_pPalette;
	CS34Palette m_SrcPalette;
	BYTE *m_pTempBuffer;
	int m_iSrcTransIndex;
};

#endif // !defined(AFX_ANIMFACTORY_H__593D3B44_0305_11D3_96CD_00A0C982B57D__INCLUDED_)
