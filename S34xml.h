/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34XML.h
 * Header file for the CS34XML class
 *
 * XML Parser
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

#if !defined(AFX_XML_H__0FC8954E_1477_11D3_96F1_00A0C982B57D__INCLUDED_)
#define AFX_XML_H__0FC8954E_1477_11D3_96F1_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "S34DblList.h"

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

class DLL_EXP CS34XML
{
public:
	CS34XML();
	~CS34XML();

	BOOL Setup(const char *pBuffer);
	BOOL Parse();

	char *GetFirst();
	char *GetNext();

private:
	CS34DblList m_List;
	char m_WorkingTextBuffer[2046];
	int m_iWorkingTextBufferSize;
	char m_SrcBuffer[32000];

	void CleanInput(char *pBuffer);
	void StripChars(char *pBuffer);
	void StripTrailingSpaces(char *pBuffer);
	void StripLeadingSpaces(char *pBuffer);
	void StripBeginningAndEndingQoutes(char *pBuffer);
	void ConvertXMLTokenToUpperCase(char *pBuffer);

	BOOL AddToList(char *pText);
};

#endif // !defined(AFX_XML_H__0FC8954E_1477_11D3_96F1_00A0C982B57D__INCLUDED_)
