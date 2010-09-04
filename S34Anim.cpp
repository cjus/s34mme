// Anim.cpp: implementation of the CS34Anim class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "s34Anim.h"
#include "stdlib.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CS34Anim::CS34Anim()
{
	m_bAnimDragable = TRUE;
	m_iPitch =-1;
	m_State = VISIBLE;
	m_TotalFrames = 0;
	m_CurrentFrame = 0;
	m_Location = CS34Point(0,0);
	m_ZOrder = 0;
	m_bZHasChanged = FALSE;
	m_Size = CS34Size(0,0);
	m_Delta = CS34Point(0,0);

	m_CurrentRect.SetRectEmpty();
	m_BoundingMovementRect.SetRectEmpty();

	m_bFrameLoop = FALSE;
	m_bAnimScript = FALSE;
	m_iScriptInstruction = CS34AnimScript::S34ANIM_START;
	m_dwTimeDelay = 0;
	m_bScriptState = FALSE;

	m_iAnimPathLocation = 0;
	m_iAnimPathSteps = 0;
	m_pAnimPathPoints = NULL;
	SetAnimPathUpdateDelay(5);

	m_dwTimeDelayCount = 0;
}

CS34Anim::~CS34Anim()
{
}


BOOL CS34Anim::AddFrame(CS34Image* pImg, CS34Rect *pClipRect)
{
	if (m_TotalFrames > MAX_ANIM_FRAMES)
		return FALSE;

	CS34Size size;
	CS34Rect Rect;

	if (pClipRect == NULL)
	{
		pImg->GetSize(&size.cx, &size.cy);
		Rect = CS34Rect(0,0,size.cx, size.cy);
	}
	else
	{
		size.cx = pClipRect->right - pClipRect->left;
		size.cy = pClipRect->bottom - pClipRect->top;
		Rect = *pClipRect;
	}

	AnimFrames[m_TotalFrames].ClipRect = Rect;
	AnimFrames[m_TotalFrames].pFrame = pImg;
	m_Size = size;
	m_TotalFrames++;

	return TRUE;
}


void CS34Anim::SetState(int state) 
{ 
	m_State = state; 
	m_bDirty = TRUE;
}


BOOL CS34Anim::SetFrame(int nFrameNumber)
{
	if (nFrameNumber < m_TotalFrames)
	{
		m_CurrentFrame = nFrameNumber;
		m_bDirty = TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL CS34Anim::SetNextFrame()
{
	if ((m_CurrentFrame + 1) < m_TotalFrames)
	{
		m_CurrentFrame++;
		m_bDirty = TRUE;
		return TRUE;
	}
	else
	{
		if (m_bFrameLoop)
		{
			m_CurrentFrame = 0;
			m_bDirty = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CS34Anim::SetPriorFrame()
{
	if ((m_CurrentFrame - 1) > -1)
	{
		m_CurrentFrame--;
		m_bDirty = TRUE;
		return TRUE;
	}
	else
	{
		if (m_bFrameLoop)
		{
			m_CurrentFrame = m_TotalFrames-1;
			m_bDirty = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

	
void CS34Anim::SetLocation(CS34Point location) 
{
	CS34Rect LastRect = m_CurrentRect;
	if (m_CurrentRect.IsRectEmpty())
	{
		m_CurrentRect = CS34Rect(location.x, location.y, location.x + m_Size.cx, location.y + m_Size.cy);
		m_BoundingMovementRect = m_CurrentRect;
	}
	else
	{
		m_CurrentRect = CS34Rect(location.x, location.y, location.x + m_Size.cx, location.y + m_Size.cy);
		m_BoundingMovementRect.UnionRect(LastRect);
		m_BoundingMovementRect.UnionRect(m_CurrentRect);
	}
	m_Location = location;
	m_bDirty = TRUE;
}

void CS34Anim::SetLocation(int x, int y, int z)
{
	SetLocation(CS34Point(x,y));
	if (z != (int)0x80000001)
	{
		m_ZOrder = z;
		m_bZHasChanged = TRUE;
	}
}

void CS34Anim::SetZOrder(int ZOrder)
{
	m_bDirty = TRUE;
	m_ZOrder = ZOrder;
	m_bZHasChanged = TRUE;
}

/*
void CS34Anim::MoveWithDelta()
{
	if (m_Delta.x != 0 || m_Delta.y != 0)
	{
		m_Location.x += m_Delta.x;
		m_Location.y += m_Delta.y;
		m_bDirty = TRUE;
	}
}
*/

void CS34Anim::GetState(int *pState) 
{ 
	*pState = m_State; 
}

int CS34Anim::GetState() 
{ 
	return m_State; 
}

void CS34Anim::GetDelta(CS34Point *pDelta) 
{
	pDelta->x = m_Delta.x;
	pDelta->y = m_Delta.y;
}

void CS34Anim::GetLocation(CS34Point *pPoint) 
{ 
	pPoint->x = m_Location.x;	pPoint->y = m_Location.y; 
}

void CS34Anim::GetSize(CS34Size *pSize) 
{ 
	pSize->cx = m_Size.cx; pSize->cy = m_Size.cy; 
}

void CS34Anim::GetRect(CS34Rect *pRect)
{
    pRect->left = m_Location.x;
    pRect->top = m_Location.y;
    pRect->right = m_Location.x + m_Size.cx;
    pRect->bottom = m_Location.y + m_Size.cy;
}

CS34Rect& CS34Anim::GetBoundingMovementRect() 
{
	return m_BoundingMovementRect; 
}

void CS34Anim::Update(int iColorDepth, CS34Rect *pClipRect)
{
	if (m_State==HIDDEN)
		return;

	CS34Rect rect;
	GetRect(&rect);

	// if ClipRect is valid see if this CS34Anim is within the cliprect
	if (pClipRect && !rect.IntersectRect(*pClipRect, rect))
		return;	// CS34Anim isn't within the supplied clip rect	

	if (iColorDepth>8)
		((CS34Image*)AnimFrames[m_CurrentFrame].pFrame)->Render(m_CompositeImg, m_iPitch,
					 rect.left, rect.top, 
					 AnimFrames[m_CurrentFrame].ClipRect.left  + (rect.left - m_Location.x),
					 AnimFrames[m_CurrentFrame].ClipRect.top  + (rect.top - m_Location.y), 
					 rect.right - rect.left, rect.bottom - rect.top);
	else
		((CS34Image*)AnimFrames[m_CurrentFrame].pFrame)->Render256(m_CompositeImg, m_iPitch,
					 rect.left, rect.top, 
					 AnimFrames[m_CurrentFrame].ClipRect.left  + (rect.left - m_Location.x),
					 AnimFrames[m_CurrentFrame].ClipRect.top  + (rect.top - m_Location.y), 
					 rect.right - rect.left, rect.bottom - rect.top);
	m_bDirty = FALSE;
	m_BoundingMovementRect = m_CurrentRect;
}


void CS34Anim::Render(int iColorDepth, BYTE *pDstBuffer, int iPitch, CS34Rect *pClipRect)
{
	if (m_State==HIDDEN)
		return;

	CS34Rect rect;
	GetRect(&rect);

	// if ClipRect is valid see if this CS34Anim is within the cliprect
	if (pClipRect && !rect.IntersectRect(*pClipRect, rect))
		return;	// CS34Anim isn't within the supplied clip rect	

	if (iColorDepth>8)
		((CS34Image*)AnimFrames[m_CurrentFrame].pFrame)->Render(pDstBuffer, iPitch,
					 rect.left, rect.top, 
					 AnimFrames[m_CurrentFrame].ClipRect.left  + (rect.left - m_Location.x),
					 AnimFrames[m_CurrentFrame].ClipRect.top  + (rect.top - m_Location.y), 
					 rect.right - rect.left, rect.bottom - rect.top);
	else
		((CS34Image*)AnimFrames[m_CurrentFrame].pFrame)->Render256(pDstBuffer, iPitch,
					 rect.left, rect.top, 
					 AnimFrames[m_CurrentFrame].ClipRect.left  + (rect.left - m_Location.x),
					 AnimFrames[m_CurrentFrame].ClipRect.top  + (rect.top - m_Location.y), 
					 rect.right - rect.left, rect.bottom - rect.top);
}

int CS34Anim::LoadAnimScript(char *pFileName)
{
	if (m_AnimScript.LoadAnimScript(pFileName) != S34ANM_E_NOERROR)
		return S34ANM_E_SCRIPT_UNABLE_TO_LOAD;
	m_bAnimScript = TRUE;
	m_bScriptState = TRUE;
	return S34ANM_E_NOERROR;
}

void CS34Anim::OnChron()
{
	if (m_pAnimPathPoints)
	{
		if (GetTickCount() > m_dwPathUpdateDelay)
		{
			SetLocation(CS34Point(m_pAnimPathPoints[m_iAnimPathLocation].x, m_pAnimPathPoints[m_iAnimPathLocation].y));
			if (m_iAnimPathLocation > m_iAnimPathSteps)
				m_iAnimPathLocation = 0;
			else
				m_iAnimPathLocation++;		
			m_dwPathUpdateDelay = GetTickCount() + m_dwPathUpdateDelayCount;
		}
	}

	if (m_bScriptState)
	{
		BOOL bInternalLoop = TRUE;  // used instead of a recursive function call!

		if (m_iScriptInstruction != CS34AnimScript::S34ANIM_WAIT)
		{
			if (GetTickCount() < m_dwTimeDelay)
				return;
			else
				m_dwTimeDelay = GetTickCount() + m_dwTimeDelayCount;
		}

		while (bInternalLoop)
		{
			switch (m_iScriptInstruction)
			{
				case CS34AnimScript::S34ANIM_START:
					// this command marks the beginning of the script processing
					// do initialization stuff, then get the first command!
					//
					m_AnimScript.GotoStart();
					m_AnimScript.GetCommand(m_iScriptInstruction, m_iScriptInstructionValue);
					m_AnimScript.GotoNext();
					m_bForwardFlag = FALSE;
					m_bBackwardFlag = FALSE;
					m_bWaitFlag = FALSE;
					bInternalLoop = TRUE;
					break;
				case CS34AnimScript::S34ANIM_SPEED:
					m_dwTimeDelayCount = m_iScriptInstructionValue;
					m_dwTimeDelay = GetTickCount() + m_iScriptInstructionValue;
					m_AnimScript.GetCommand(m_iScriptInstruction, m_iScriptInstructionValue);
					m_AnimScript.GotoNext();
					bInternalLoop = TRUE;
					break;
				case CS34AnimScript::S34ANIM_GO:
					SetFrame(m_iScriptInstructionValue);
					m_AnimScript.GetCommand(m_iScriptInstruction, m_iScriptInstructionValue);
					m_AnimScript.GotoNext();
					bInternalLoop = FALSE;
					break;
				case CS34AnimScript::S34ANIM_FORWARD:
					if (!m_bForwardFlag)
					{
						m_iScriptMovementLoopCount = m_iScriptInstructionValue;
						m_bForwardFlag = TRUE;
					}
					SetNextFrame();
					if (--m_iScriptMovementLoopCount==0)
					{
						m_AnimScript.GetCommand(m_iScriptInstruction, m_iScriptInstructionValue);
						m_AnimScript.GotoNext();
						m_bForwardFlag = FALSE;
						bInternalLoop = TRUE;
						continue;
					}
					bInternalLoop = FALSE;
					break;
				case CS34AnimScript::S34ANIM_BACKWARD:
					if (!m_bBackwardFlag)
					{
						m_iScriptMovementLoopCount = m_iScriptInstructionValue;
						m_bBackwardFlag = TRUE;
					}
					SetPriorFrame();
					if (--m_iScriptMovementLoopCount==0)
					{
						m_AnimScript.GetCommand(m_iScriptInstruction, m_iScriptInstructionValue);
						m_AnimScript.GotoNext();
						m_bBackwardFlag = FALSE;
						bInternalLoop = TRUE;
						continue;
					}
					bInternalLoop = FALSE;
					break;
				case CS34AnimScript::S34ANIM_WAIT:
					if (!m_bWaitFlag)
					{
						m_dwTimeWait = m_iScriptInstructionValue + GetTickCount();
						m_bWaitFlag = TRUE;
					}
					if (GetTickCount() > m_dwTimeWait)
					{
						m_bWaitFlag = FALSE;
						m_AnimScript.GetCommand(m_iScriptInstruction, m_iScriptInstructionValue);
						m_AnimScript.GotoNext();
					}
					bInternalLoop = FALSE;
					break;
	//			case CS34AnimScript::S34ANIM_STOP:
	//				m_AnimScript.GotoNext();
	//				bInternalLoop = FALSE;
	//				break
				case CS34AnimScript::S34ANIM_END:
					// we're at the end of the script, restart it
					//
					m_iScriptInstruction = CS34AnimScript::S34ANIM_START;
					bInternalLoop = TRUE;
					break;
			};
		}
	}
}

BOOL CS34Anim::IsPointInAnim(int x, int y)
{
	if (m_State==HIDDEN)
		return FALSE;
	return GetCurrentImageFrame()->IsPointInImage(x,y);
}


void CS34Anim::SetAnimPath(POINT *p, int iSteps, int iLocation)
{
	m_iAnimPathLocation = iLocation;
	m_pAnimPathPoints = p;
	m_iAnimPathSteps = iSteps;
}
