// Text.cpp: implementation of the CS34Text class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "conio.h"
#include "s34Text.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS34Text::CS34Text()
{
	m_iBufferSize = 0;
	m_pData = NULL;
	m_pTempData = NULL;
	m_iTempDataSize = 0;
}

CS34Text::~CS34Text()
{
	Erase();
}

int CS34Text::GetLength() const
{
	if (m_pData==NULL)
		return 0;
	return strlen(m_pData);
}

BOOL CS34Text::IsEmpty() const
{
	return (GetLength() == 0) ? TRUE : FALSE;
}

CS34Text::operator const char*() const
{
	return m_pData;
}

CS34Text::operator char*() const
{
	return m_pData;
}

CS34Text& CS34Text::operator=(const CS34Text &str)
{
	Init(str.m_pData);
	return *this;
}

CS34Text& CS34Text::operator=(const char *psz)
{
	Init((char*)psz);
	return *this;
}

CS34Text& CS34Text::operator+=(const CS34Text &str)
{
	Concat(str.m_pData);	
	return *this;
}

CS34Text& CS34Text::operator+=(const char *psz)
{
	Concat((char*)psz);
	return *this;
}

int CS34Text::operator<(const CS34Text &str) const
{
	return (strcmp(m_pData, str.m_pData) < 0);
}
int CS34Text::operator>(const CS34Text &str) const
{
	return (strcmp(m_pData, str.m_pData) > 0);
}
int CS34Text::operator>=(const CS34Text &str) const
{
	return (strcmp(m_pData, str.m_pData) >= 0);
}
int CS34Text::operator<=(const CS34Text &str) const
{
	return (strcmp(m_pData, str.m_pData) <= 0);
}
int CS34Text::operator==(const CS34Text &str) const
{
	return (strcmp(m_pData, str.m_pData) == 0);
}

int CS34Text::operator<(const char *psz) const
{
	return (strcmp(m_pData, psz) < 0);
}
int CS34Text::operator>(const char *psz) const
{
	return (strcmp(m_pData, psz) > 0);
}
int CS34Text::operator>=(const char *psz) const
{
	return (strcmp(m_pData, psz) >= 0);
}
int CS34Text::operator==(const char *psz) const
{
	return (strcmp(m_pData, psz) == 0);
}

char *CS34Text::GetBuffer(int iMinSize)
{
	if (iMinSize > m_iBufferSize)
	{
		CS34Text s = *this;
		Init(iMinSize);
		*this = s;
	}
	return m_pData;
}

const char * CS34Text::Left(int iCount)
{
	char *p=NULL;
	if (iCount >= 0 && iCount < m_iBufferSize)
		p = AllocCopy(m_pData, iCount);
	return p;
}

const char * CS34Text::Mid(int iFirst, int iCount)
{
	if (iCount == -1 || iCount > m_iBufferSize)
		iCount = m_iBufferSize-1;

	char *p=NULL;
	if (iFirst >= 0 && iFirst < m_iBufferSize) 
		p = AllocCopy(&m_pData[iFirst], iCount);
	return p;
}

const char * CS34Text::Right(int iCount)
{
	if (iCount > m_iBufferSize)
		iCount = m_iBufferSize-1;

	char *p=NULL;
	if (iCount >= 0) 
		p = AllocCopy(&m_pData[m_iBufferSize-iCount-1], iCount);
	return p;
}


void CS34Text::Erase()
{
	if (m_pData)
	{
		delete []m_pData;
		m_pData = NULL;
	}
	if (m_pTempData)
	{
		delete []m_pTempData;
		m_pTempData = NULL;
	}
}


void CS34Text::Init(char *p)
{
	int i = 0;
	if (p != NULL)
		i = strlen(p);
	if (i>0)
	{
		Init(i);
		strcpy(m_pData, p);
	}
}

void CS34Text::Init(int iCount)
{
	iCount++;
	if (m_pData)
	{
		if (iCount > m_iBufferSize)
		{
			delete []m_pData;
			m_pData = new char[iCount];
			if (m_pData==NULL)
				return;
			m_iBufferSize = iCount;
		}
	}
	else
	{
		m_iBufferSize = iCount;
		m_pData = new char[iCount];
		if (m_pData==NULL)
			return;
	}
}


void CS34Text::Concat(char *p)
{
	int i = strlen(p)+1;
	m_iBufferSize += i;
	char *pData = new char[m_iBufferSize];
	if (pData==NULL)
		return;

	strcpy(pData, m_pData);
	strcat(pData, p);

	if (m_pData)
		delete []m_pData;
	m_pData = pData;
}

char *CS34Text::AllocCopy(char *pData, int iCount)
{
	iCount++;
	if (m_pTempData==NULL)
	{
		m_pTempData = new char[iCount];
		m_iTempDataSize = iCount;
	}

	if (m_iTempDataSize < iCount)
	{
		delete []m_pTempData;
		m_pTempData = new char[iCount];
		m_iTempDataSize = iCount;
	}

	memcpy(m_pTempData, pData, iCount);
	m_pTempData[iCount-1] = 0;

	return m_pTempData;
}


UINT CS34Text::HashKey()
{
	char *pKey = m_pData;
	UINT iHash = 0;
	while (*pKey)
		iHash = (iHash<<5) + iHash + *pKey++;
	return iHash;
}

char CS34Text::GetAt(int i)
{
	if (i >= 0 && i < m_iBufferSize)
	{
		return m_pData[i];
	}
	return 0;
}

void CS34Text::SetAt(int i, char ch)
{
	if (i >= 0 && i < m_iBufferSize)
	{
		m_pData[i] = ch;
	}
}

void CS34Text::TruncateAt(int i)
{
	if (i >= 0 && i < m_iBufferSize)
	{
		m_iBufferSize = i;
		m_pData[i] = 0;
	}
}

void CS34Text::Crypto(char *pszKey)
{
	char *pKey = pszKey;
	char *pBuffer = m_pData;
	for (int i = 0; i < m_iBufferSize; i++, pBuffer++, pKey++)
    {
		// rotate key if we reach its end
		if (!*(pKey))		// warning: assumes key is null terminated string
			pKey = pszKey;
        
		char val = (*pBuffer ^ *pKey ^ (char)i);
		*pBuffer = val;
    }
}


void CS34Text::Format(char *pFormat, ...)
{
//	Erase();

	char *p = new char[32000];
	if (p==NULL)
		return;

	va_list argptr;
	va_start(argptr, pFormat);
	vsprintf(p, pFormat, argptr);
	va_end(argptr);

	Init(p);

	delete []p;
}

void CS34Text::FormatWin32ErrorMessage(DWORD dwErrorCode)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					dwErrorCode,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					(LPTSTR) &lpMsgBuf, 0, NULL);

	Init((char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int CS34Text::Find(char *psz)
{
	int L1 = strlen(psz);
	int L2 = m_iBufferSize;

//	L2// Carlos Justiniano
//	L1// Just

	// if psz is greater then our internal buffer
	// then it can't possibly be a substring.
	//
	if (L1 > L2)
		return -1;

	for (int i=0; i<L2; i++)
	{
		for (int j=0; j<L1; j++)
		{
			if (i+j > L2)
				break;
			if (m_pData[i+j] != psz[j])
				break;
		}
		if (j==L1)
			return i;
	}

	return -1;
}

int CS34Text::Find(const CS34Text &str)
{
	return Find((char*)str);
}

void CS34Text::DebugMsg()
{
	OutputDebugString(m_pData);
}

void CS34Text::Upper()
{
	if (m_pData)
		strupr(m_pData);
}

void CS34Text::Lower()
{
	if (m_pData)
		strlwr(m_pData);
}

void CS34Text::Sscanf(const char* source, const char* format)
{
	Init((char*)source);
	sscanf(source, format, m_pData);
}

