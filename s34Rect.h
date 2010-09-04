/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34Rect.h
 * Header file for the CRect class
 *
 * RECT class based on the MFC CRect class
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

#if !defined(AFX_RECT_H__3BD67535_FE00_11D2_96B8_00A0C982B57D__INCLUDED_)
#define AFX_RECT_H__3BD67535_FE00_11D2_96B8_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

#include <windows.h>
#include "s34Point.h"
#include "s34Size.h"

class DLL_EXP CS34Rect  
{
public:
	int left;  // x1
	int top;   // y1
	int right; // x2
	int bottom; //y2

	CS34Rect() { left = top = right = bottom = 0; }
	CS34Rect(int NewLeft, int NewTop, int NewRight, int NewBottom)
	{
		left = NewLeft;  top = NewTop;  right = NewRight;  bottom = NewBottom;
	}
	CS34Rect(CS34Rect &rect);
	~CS34Rect() {}

	void Init(int NewLeft, int NewTop, int NewRight, int NewBottom)
	{
		left = NewLeft;  top = NewTop;  right = NewRight;  bottom = NewBottom;
	}

	void SetRectEmpty(void)
	{
		left = top = right = bottom = 0;
	}

	BOOL IsRectEmpty(void)
	{
		return (left == 0 && top == 0 && right == 0 && bottom == 0) ? TRUE : FALSE;
	}

	const int GetWidth(void) const { return (right - left); }
	const int GetHeight(void) const { return (bottom - top); }
	const CS34Size GetSize(void) const { return CS34Size(GetWidth(), GetHeight()); }

	BOOL IntersectRect(CS34Rect &r2)
	{
		if (left > r2.right) return FALSE;
		if (right < r2.left) return FALSE;
		if (top > r2.bottom) return FALSE;
		if (bottom < r2.top) return FALSE;
		return TRUE;
	}

	BOOL IntersectRect(CS34Rect &r1, CS34Rect &r2);

	void OffsetRect(CS34Point &pt);
	void OffsetRect(int x, int y);

	void InflateRect(CS34Point &pt);
	void InflateRect(int x, int y);
	void InflateRect(CS34Size size);

	BOOL IsPointInRect(CS34Point &pt);
	BOOL IsPointInRect(int x, int y);

	void UnionRect(CS34Rect &rect);
	void UnionRect(CS34Rect &rect1, CS34Rect &rect2);

	void operator=(const CS34Rect& srCS34Rect);
	int  operator==(const CS34Rect& srCS34Rect) const;

	BOOL EqualRect(const CS34Rect& srCS34Rect) const;

};

#endif // !defined(AFX_RECT_H__3BD67535_FE00_11D2_96B8_00A0C982B57D__INCLUDED_)
