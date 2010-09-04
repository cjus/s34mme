// Rect.cpp: implementation of the CS34Rect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "s34Rect.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS34Rect::CS34Rect(CS34Rect &rect)
{
	left = rect.left;
	top = rect.top;
	right = rect.right;
	bottom = rect.bottom;
}

BOOL CS34Rect::IntersectRect(CS34Rect &r1, CS34Rect &r2)
{
	if (r1.left > r2.right)
	{
		SetRectEmpty();
		return FALSE;
	}
	if (r1.right < r2.left)
	{
		SetRectEmpty();
		return FALSE;
	}
	if (r1.top > r2.bottom)
	{
		SetRectEmpty();
		return FALSE;
	}
	if (r1.bottom < r2.top)
	{
		SetRectEmpty();
		return FALSE;
	}

	CS34Rect tmpRect = r1;

	if (r2.top > r1.top)
		tmpRect.top = r2.top;

	if (r2.left > r1.left)
		tmpRect.left = r2.left;

	if (r2.right < r1.right)
		tmpRect.right = r2.right;

	if (r2.bottom < r1.bottom)
		tmpRect.bottom = r2.bottom;


	left = tmpRect.left;
	top = tmpRect.top;
	right = tmpRect.right;
	bottom = tmpRect.bottom;

	return TRUE;
}

void CS34Rect::OffsetRect(CS34Point &pt)
{
	left += pt.x;
	top += pt.y;
	right += pt.x;
	bottom += pt.y;
}

void CS34Rect::OffsetRect(int x, int y)
{
	left += x;  right += x; top += y;  bottom += y;
}

void CS34Rect::InflateRect(CS34Point &pt)
{
	left += pt.x; top += pt.y; right += pt.x; bottom += pt.y;
}

void CS34Rect::InflateRect(int x, int y)
{
	left += x;  right += x; top += y;  bottom += y;
}

void CS34Rect::InflateRect(CS34Size size)
{
	left += size.cx;
	right += size.cx;
	top += size.cy;
	bottom += size.cy;
}

BOOL CS34Rect::IsPointInRect(CS34Point &pt)
{
	if (pt.x < left)
		return FALSE;
	if (pt.x > right)
		return FALSE;
	if (pt.y < top)
		return FALSE;
	if (pt.y > bottom)
		return FALSE;
	return TRUE;
}

BOOL CS34Rect::IsPointInRect(int x, int y)
{
	if (x < left)
		return FALSE;
	if (x > right)
		return FALSE;
	if (y < top)
		return FALSE;
	if (y > bottom)
		return FALSE;
	return TRUE;
}

void CS34Rect::UnionRect(CS34Rect &rect)
{
	left = (left < rect.left) ? left : rect.left;
	top = (top < rect.top) ? top : rect.top;
	right = (right > rect.right) ? right : rect.right;
	bottom = (bottom > rect.bottom) ? bottom : rect.bottom;
}

void CS34Rect::UnionRect(CS34Rect &rect1, CS34Rect &rect2)
{
	left = (rect1.left < rect2.left) ? rect1.left : rect2.left;
	top = (rect1.top < rect2.top) ? rect1.top : rect2.top;
	right = (rect1.right > rect2.right) ? rect1.right : rect2.right;
	bottom = (rect1.bottom > rect2.bottom) ? rect1.bottom : rect2.bottom;
}

void CS34Rect::operator=(const CS34Rect& srCS34Rect)
{
	left = srCS34Rect.left;
	top = srCS34Rect.top;
	right = srCS34Rect.right;
	bottom = srCS34Rect.bottom;
}

int CS34Rect::operator ==(const CS34Rect& srCS34Rect) const
{
	return (left == srCS34Rect.left && top == srCS34Rect.top &&
		    right == srCS34Rect.right && bottom == srCS34Rect.bottom);
}

BOOL CS34Rect::EqualRect(const CS34Rect& srCS34Rect) const
{
	if (left == srCS34Rect.left && top == srCS34Rect.top &&
		right == srCS34Rect.right && bottom == srCS34Rect.bottom)
		return TRUE;
	else
		return FALSE;
}
