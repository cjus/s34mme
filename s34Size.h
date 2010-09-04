/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34Size.h
 * Header file for the CS34Size class
 *
 * SIZE class based on the MFC CSize class
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

#if !defined(AFX_SIZE_H__3BD67537_FE00_11D2_96B8_00A0C982B57D__INCLUDED_)
#define AFX_SIZE_H__3BD67537_FE00_11D2_96B8_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

class DLL_EXP CS34Size  
{
public:
	int cx;
	int cy;
	CS34Size();
	~CS34Size();

	CS34Size(int NewX, int NewY) { cx = NewX; cy = NewY; }
	CS34Size(CS34Size &size) { cx = size.cx; cy = size.cy; }
};

#endif // !defined(AFX_SIZE_H__3BD67537_FE00_11D2_96B8_00A0C982B57D__INCLUDED_)
