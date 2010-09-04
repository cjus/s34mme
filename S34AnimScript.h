/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34AnimScript.h
 * Header file for the CS34AnimScript class
 *
 * Animation Script manager.
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

#if !defined(AFX_S34ANIMSCRIPT_H__37995594_04D4_11D3_96D2_00A0C982B57D__INCLUDED_)
#define AFX_S34ANIMSCRIPT_H__37995594_04D4_11D3_96D2_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "S34AnmErrors.h"
#include "S34DblList.h"

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

class DLL_EXP CS34AnimScript  
{
public:
	CS34AnimScript();
	virtual ~CS34AnimScript();

	enum
	{
		S34ANIM_NOP,
		S34ANIM_SPEED,
		S34ANIM_GO,
		S34ANIM_FORWARD,
		S34ANIM_BACKWARD,
		S34ANIM_WAIT,
		S34ANIM_STOP,
		S34ANIM_START,
		S34ANIM_END
	};

	int AddCommand(int iCommandID, int iValue);
	int GetCommand(int &iCommandID, int &iValue);

	int Clear();
	int GotoStart();
	int GotoNext();

	int LoadAnimScript(char *pFileName);

private:
	CS34DblList m_InstructionQueue;
};

#endif // !defined(AFX_S34ANIMSCRIPT_H__37995594_04D4_11D3_96D2_00A0C982B57D__INCLUDED_)
