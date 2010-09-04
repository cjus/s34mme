/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34DblList
 * Header file for the CS34DblList class
 *
 * Double Link List class.
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

#if !defined(AFX_S34DBLLIST_H__7185B11A_0358_11D3_96CD_00A0C982B57D__INCLUDED_)
#define AFX_S34DBLLIST_H__7185B11A_0358_11D3_96CD_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "windows.h"

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

class DLL_EXP CS34DblList  
{
public:
	CS34DblList();
	virtual ~CS34DblList();

	void Clear();

	BOOL Add(void* pData);
	void* GetData();
	BOOL MoveToHead();
	BOOL MoveToTail();
	BOOL MoveNext();
	BOOL MovePrev();

	BOOL IsEmpty();

	// removes from the list, you have to delete the pointer if applicable!
	BOOL Remove(void* pData);

	int GetTotalEntries();

private:
	struct DblList
	{
		void *pData;
		DblList *pNext;
		DblList *pPrev;
	};

	DblList *m_pHead;
	DblList *m_pTail;
	DblList *m_pCurrent;
};

#endif // !defined(AFX_S34DBLLIST_H__7185B11A_0358_11D3_96CD_00A0C982B57D__INCLUDED_)


