/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34Anim.h 
 * Header file for the CS34Anim class
 *
 * Sprite handler.
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

#if !defined(AFX_ANIM_H__3BD67534_FE00_11D2_96B8_00A0C982B57D__INCLUDED_)
#define AFX_ANIM_H__3BD67534_FE00_11D2_96B8_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

#include "s34rect.h"
#include "s34size.h"
#include "s34point.h"
#include "s34Image.h"
#include "S34AnimScript.h"

#define MAX_ANIM_FRAMES 1024
struct DLL_EXP AnimFrameElement
{
	CS34Image *pFrame;
	CS34Rect ClipRect;
};

class DLL_EXP CS34Anim  
{
public:
	enum STATE
	{
		VISIBLE, PREHIDDEN, HIDDEN
	};

	CS34Anim();
	~CS34Anim();

	BOOL AddFrame(CS34Image* pDib, CS34Rect *pClipRect = NULL);
	void SetCompositeSurface(BYTE* pImg) { m_CompositeImg = pImg; }
	void SetDstPitch(int iPitch) { m_iPitch = iPitch; }

	int  GetFrame() { return m_CurrentFrame; }
	CS34Image* GetCurrentImageFrame() { return (CS34Image*)AnimFrames[m_CurrentFrame].pFrame; }
	void SetState(int state);

	void SetLocation(CS34Point location);
	void SetZOrder(int ZOrder);

	int GetZOrder() { return m_ZOrder; }
	void SetDelta(CS34Point *pDelta) { m_Delta.x = pDelta->x; m_Delta.y = pDelta->y; }
	void GetDelta(CS34Point *pDelta);

	int GetTotalFrames() { return m_TotalFrames; }

	void GetState(int *pState);
	int GetState();
	void GetLocation(CS34Point *pPoint);
	void GetSize(CS34Size *pSize);
	void GetRect(CS34Rect *pRect);

	void SetFrameLoop(BOOL bState) { m_bFrameLoop = bState; }
	BOOL SetFrame(int nFrameNumber);
	BOOL SetNextFrame();
	BOOL SetPriorFrame();
	void SetLocation(int x, int y, int z);

	BOOL IsZChange() { return m_bZHasChanged; }
	BOOL IsHidden() { return (m_State==HIDDEN) ? TRUE : FALSE; }
	BOOL IsDirty() { return m_bDirty; }
	void SetDirty() { m_bDirty = TRUE; }

	void Update(int iColorDepth, CS34Rect *pClipRect = NULL);
	void Render(int iColorDepth, BYTE *pDstBuffer, int iPitch, CS34Rect *pClipRect);

	CS34Rect &GetBoundingMovementRect();

	int LoadAnimScript(char *pFileName);
	BOOL IsAnimScriptLoaded() { return m_bAnimScript; }
	void PauseScript() { m_bScriptState = FALSE; }
	void ResumeScript() { m_bScriptState = TRUE; }

	void OnChron();

	BOOL IsPointInAnim(int x, int y);

	BOOL IsAnimDragable() { return m_bAnimDragable; }
	void SetAnimDragState(BOOL bDragable) { m_bAnimDragable = bDragable; }

	void SetAnimPath(POINT *p, int iSteps, int iLocation=0);
	void SetAnimPathUpdateDelay(int iDelay) { m_dwPathUpdateDelay = 0; m_dwPathUpdateDelayCount = iDelay; }

private:
	AnimFrameElement AnimFrames[MAX_ANIM_FRAMES];
	CS34Rect m_BoundingMovementRect;

	BYTE* m_CompositeImg;
	int m_iPitch;

	CS34Point m_Location;
	int    m_ZOrder;
	CS34Point m_Delta;

	BOOL m_bZHasChanged;
	BOOL m_bDirty;

	int m_State;
	BOOL m_bAnimDragable;

	int m_TotalFrames;
	int m_CurrentFrame;
	int m_LastFrame;

	CS34Size m_Size;
	CS34Rect m_CurrentRect;

	BOOL m_bFrameLoop;
	BOOL m_bAnimScript;
	BOOL m_bForwardFlag;
	BOOL m_bBackwardFlag;
	BOOL m_bWaitFlag;
	int  m_iScriptInstruction;
	int  m_iScriptInstructionValue;
	DWORD m_dwTimeDelay;
	DWORD m_dwTimeDelayCount;
	DWORD m_dwTimeWait;
	int m_iScriptMovementLoopCount;
	CS34AnimScript m_AnimScript;

	BOOL m_bScriptState;

	int m_iAnimPathLocation;
	int m_iAnimPathSteps;
	POINT *m_pAnimPathPoints;
	DWORD m_dwPathUpdateDelay;
	DWORD m_dwPathUpdateDelayCount;
};

#endif // !defined(AFX_ANIM_H__3BD67534_FE00_11D2_96B8_00A0C982B57D__INCLUDED_)
