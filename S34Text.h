/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34Text.h
 * Header file for the CS34Text class
 *
 * String handling class similar to MFC's CString class
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

#if !defined(AFX_Text_H__6A7313E2_158D_11D3_96F3_00A0C982B57D__INCLUDED_)
#define AFX_Text_H__6A7313E2_158D_11D3_96F3_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

class DLL_EXP CS34Text  
{
public:
	CS34Text();
	virtual ~CS34Text();

	int GetLength() const;
	BOOL IsEmpty() const;

	operator const char*() const;
	operator char*() const;

	CS34Text& operator+=(const CS34Text &str);
	CS34Text& operator=(const CS34Text &str);

	CS34Text& operator=(const char *psz);
	CS34Text& operator+=(const char *psz);

	int operator<(const CS34Text &str) const;
	int operator>(const CS34Text &str) const;
	int operator<=(const CS34Text &str) const;
	int operator>=(const CS34Text &str) const;
	int operator==(const CS34Text &str) const;

	int operator<(const char *psz) const;
	int operator>(const char *psz) const;
	int operator<=(const char *psz) const;
	int operator>=(const char *psz) const;
	int operator==(const char *psz) const;

	const char *Left(int iCount);
	const char *Mid(int iFirst, int iCount=-1);
	const char *Right(int iCount);

	char GetAt(int i);
	void SetAt(int i, char ch);
	void TruncateAt(int i);

	char *GetBuffer(int iMinSize);

	void Format(char *pFormat, ...);
	void FormatWin32ErrorMessage(DWORD dwErrorCode);

	UINT HashKey();
	void Crypto(char *pszKey);

	int Find(char *psz);
	int Find(const CS34Text &str);

	void DebugMsg();
  
	void Upper();
	void Lower();
	void Sscanf(const char* source, const char* format);

private:
	char *m_pData;
	int m_iBufferSize;

	char  *m_pTempData;
	int	  m_iTempDataSize;

	void Erase();
	void Init(char *p);
	void Init(int iCount);
	void Concat(char *p);
	char *AllocCopy(char *pData, int iCount);
};

#endif // !defined(AFX_Text_H__6A7313E2_158D_11D3_96F3_00A0C982B57D__INCLUDED_)
