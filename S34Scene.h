/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34Scene.h
 * Header file for the CS34Scene class
 *
 * Provides a scene where sprites can be animated in.
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

#if !defined(AFX_SCENE_H__3BD67532_FE00_11D2_96B8_00A0C982B57D__INCLUDED_)
#define AFX_SCENE_H__3BD67532_FE00_11D2_96B8_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

#include "s34image.h"
#include "s34anim.h"
#include "s34point.h"
#include "s34rect.h"

#define MAX_RECTS 256
#define MAX_SPRITES MAX_RECTS

class DLL_EXP CS34Scene  
{
public:
	CS34Scene();
	virtual ~CS34Scene();

	int  Setup(BYTE *pImageData, int width, int height, int color_depth);

	BYTE *GetCompositImg() { return (BYTE*)m_pBackgroundImg; }
	BYTE *GetViginImg() { return (BYTE*)m_pVirginImg; }

	void Update(BYTE* pSurface, LONG lPitch, BOOL bFullScreen, HWND hWnd);
	void UpdateWindow(HWND hWnd, HBITMAP m_hDibSection, HPALETTE hPalette=NULL);

	void OnChron();

	BOOL AttachAnimObject(CS34Anim* pAnimObject);
	CS34Anim* GetAnimFromPoint(CS34Point *point);
	BOOL AnimCollide(CS34Anim* pAnim1, CS34Anim* pAnim2);

private:
	struct AnimContainer
	{
		CS34Anim *pAnimObject;
		AnimContainer* pNext;
		AnimContainer* pLast;
	};
	AnimContainer *pAnimContainer;
	AnimContainer *pAnimContainerTop;
	AnimContainer AnimTemp;
	CS34Rect RectCache[MAX_RECTS];

	BYTE *m_pVirginImg;
	BYTE *m_pBackgroundImg;

	BOOL m_bSortScene;
	CS34Rect  m_ScreenRect;
	int m_ColorDepth;
	DWORD m_dwPitch;
	DWORD m_dwRectTotal;
};

#endif // !defined(AFX_SCENE_H__3BD67532_FE00_11D2_96B8_00A0C982B57D__INCLUDED_)

