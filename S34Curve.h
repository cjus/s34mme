/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34Curve.h
 * Header file for the CS34Curve class
 *
 * Curve handler.  Used by Sprite Animation objects to move along paths.
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

#if !defined(AFX_CURVE_H__3BD67538_FE00_11D2_96B8_00A0C982B57D__INCLUDED_)
#define AFX_CURVE_H__3BD67538_FE00_11D2_96B8_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

struct CURVEPOINT
{
   float x;
   float y;
};

struct LINE
{
   CURVEPOINT p1;
   CURVEPOINT p2;
};

class DLL_EXP CS34Curve
{
public:
	CS34Curve();
	virtual ~CS34Curve();

	void SetTotalSegments(int iTotalSegments);
	int  GetTotalSegments() { return m_iTotalSegments; }

	void GeneratePoints(POINT ptStartPoint, POINT ptEndPoint, POINT ptControlPoint);
	int  GetPoints(POINT *p, int iTotalPoints);

	void Reset();

private:

	CURVEPOINT *m_pPoints;
	int m_iTotalSegments;

	float Dy(LINE l) { return (l.p2.y - l.p1.y); }
	float Dx(LINE l) { return (l.p2.x - l.p1.x); }
	
	BOOL CalculatePoints(CURVEPOINT p1, CURVEPOINT p2, CURVEPOINT p3, int n, CURVEPOINT *p);
	CURVEPOINT Intersect(LINE l1, LINE l2);
};

#endif // !defined(AFX_CURVE_H__3BD67538_FE00_11D2_96B8_00A0C982B57D__INCLUDED_)
