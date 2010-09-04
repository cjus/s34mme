/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34AnimPathManager
 * Header file for the CS34AnimPathManager class
 *
 * Animation path manager.
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

#if !defined(AFX_S34ANIMPATHMANAGER_H__A48D56B6_113C_11D3_96EA_00A0C982B57D__INCLUDED_)
#define AFX_S34ANIMPATHMANAGER_H__A48D56B6_113C_11D3_96EA_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "S34Curve.h"

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

class DLL_EXP CS34AnimPathManager  
{
public:
	CS34AnimPathManager();
	virtual ~CS34AnimPathManager();

	HANDLE CreatePath(POINT ptStartPoint, POINT ptEndPoint, POINT ptControlPoint, int iSteps);
	BOOL GetPath(HANDLE hHandle, POINT **pData, int *piSteps);
private:
	CS34Curve m_CurveGenerator;
};

#endif // !defined(AFX_S34ANIMPATHMANAGER_H__A48D56B6_113C_11D3_96EA_00A0C982B57D__INCLUDED_)

