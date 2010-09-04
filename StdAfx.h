// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7525DA56_1B25_11D3_9703_00A0C982B57D__INCLUDED_)
#define AFX_STDAFX_H__7525DA56_1B25_11D3_9703_00A0C982B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include "stdio.h"
#include "stdlib.h"


// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

// useful macros
//
#define BAIL() goto exit_with_error

// Useful debugging and maintance reminder
// Usage: #pragma _MEMO_(This code needs to be tested)
//
#define __STRINGIZE__(x)	#x
#define __STR__(x)			__STRINGIZE__(x)
#define _LOC_				__FILE__"("__STR__(__LINE__)") : "
#define _MEMO_(x)			message(_LOC_ "*** Build message: "#x)

//#define BLIT_TEST 1

#endif // !defined(AFX_STDAFX_H__7525DA56_1B25_11D3_9703_00A0C982B57D__INCLUDED_)
