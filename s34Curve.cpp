// Curve.cpp: implementation of the CS34Curve class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "s34Curve.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS34Curve::CS34Curve()
{
	m_pPoints = NULL;
	m_iTotalSegments = 0;
}

CS34Curve::~CS34Curve()
{
	Reset();
}

void CS34Curve::Reset()
{
	if (m_pPoints)
	{
		delete []m_pPoints;
		m_pPoints = NULL;
	}
	m_iTotalSegments = 0;
}


void CS34Curve::SetTotalSegments(int iTotalSegments)
{
	if (iTotalSegments != m_iTotalSegments)
	{
		if (m_pPoints)
		{
			delete []m_pPoints;
			m_pPoints = NULL;
		}
		m_pPoints = new CURVEPOINT[iTotalSegments];
		m_iTotalSegments = iTotalSegments;
	}
}


void CS34Curve::GeneratePoints(POINT ptStartPoint, POINT ptEndPoint, POINT ptControlPoint)
{
	CURVEPOINT p1, p2, p3;
	p1.x = (float)ptStartPoint.x;
	p1.y = (float)ptStartPoint.y;

	p2.x = (float)ptEndPoint.x;
	p2.y = (float)ptEndPoint.y;

	p3.x = (float)ptControlPoint.x;
	p3.y = (float)ptControlPoint.y;

	CalculatePoints(p1, p2, p3, m_iTotalSegments, m_pPoints);
}


int CS34Curve::GetPoints(POINT *p, int iTotalPoints)
{
	if (iTotalPoints > m_iTotalSegments)
		iTotalPoints = m_iTotalSegments;
	for (int i = 0; i < iTotalPoints; i++)
	{
		p[i].x = (long) m_pPoints[i].x;
		p[i].y = (long) m_pPoints[i].y;
	}
	return iTotalPoints;
}


CURVEPOINT CS34Curve::Intersect(LINE l1, LINE l2)
{
	float m1;
	float b1;
	float m2;
	float b2;
	CURVEPOINT p;

	if (Dx(l1) == 0)
	{
		p.x = l1.p1.x;
		m1 = (l2.p1.y - l2.p2.y) / (l2.p1.x - l2.p2.x);
		b1 = l2.p1.y - m1*l2.p1.x;
	}
	else if (Dx(l2) == 0)
	{
		p.x = l2.p1.x;
		m1 = (l1.p1.y - l1.p2.y) / (l1.p1.x - l1.p2.x);
		b1 = l1.p1.y - m1*l1.p1.x;
	}
	else
	{
		m1 = (l1.p1.y - l1.p2.y) / (l1.p1.x - l1.p2.x);
		b1 = l1.p1.y - m1*l1.p1.x;
		m2 = (l2.p1.y - l2.p2.y) / (l2.p1.x - l2.p2.x);
		b2 = l2.p1.y - m2*l2.p1.x;
		p.x = (b1 - b2) / (m2 - m1);
	}
	p.y = m1 * p.x + b1; 
	return p;
}



BOOL CS34Curve::CalculatePoints(CURVEPOINT p1, CURVEPOINT p2, CURVEPOINT p3, int n, CURVEPOINT *p) 
{
	LINE l1;
	l1.p1 = p1;
	l1.p2 = p3;
	LINE l2;
	l2.p1 = p3;
	l2.p2 = p2;
	float dx1 = Dx(l1) / (n + 1);
	float dx2 = Dx(l2) / (n + 1);

	float m1;
	float m2;
	float dy1;
	float dy2;
	float b1;
	float b2;

	if (dx1 != 0)
    {
		m1 = (l1.p1.y - l1.p2.y) / (l1.p1.x - l1.p2.x);
		b1 = l1.p1.y - m1*l1.p1.x;
	}
	else
    {
		dy1 = Dy(l1) / (n + 1);
    } 

	if (dx2 != 0)
	{
		m2 = (l2.p1.y - l2.p2.y) / (l2.p1.x - l2.p2.x);
		b2 = l2.p1.y - m2*l2.p1.x;
	}
	else
    {
		dy2 = Dy(l2) / (n + 1);
	}
   
	LINE ls1;
	LINE ls2;
	for (int i = 0; i < n; i++)
	{
		ls1.p1.x = l1.p1.x + (dx1 * i);
		ls1.p2.x = l2.p1.x + (dx2 * (i + 1));
		ls2.p1.x = l1.p1.x + (dx1 * (i + 1));
		ls2.p2.x = l2.p1.x + (dx2 * (i + 2));
		if (dx1 != 0)
        {
			ls1.p1.y = m1*ls1.p1.x + b1;
			ls2.p1.y = m1*ls2.p1.x + b1;
		}
		else
		{
			ls1.p1.y = l1.p1.y + (dy1 * i);
			ls2.p1.y = l1.p1.y + (dy1 * (i + 1));
		}
		if (dx2 != 0)
        {
			ls1.p2.y = m2*ls1.p2.x + b2;
			ls2.p2.y = m2*ls2.p2.x + b2;
		}
		else
		{
			ls1.p2.y = l2.p1.y + (dy2 * (i + 1));
			ls2.p2.y = l2.p1.y + (dy2 * (i + 2));
		}
		p[i] = Intersect(ls1, ls2);
	}

	return TRUE;
}

