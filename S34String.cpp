// S34String.cpp: implementation of the CS34String class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S34String.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS34String::CS34String()
{
	m_iBufferSize = 0;
	m_pData = NULL;
}

CS34String::~CS34String()
{
	Erase();
}

CS34String::CS34String(const char *psz)
{
	m_iBufferSize = 0;
	m_pData = NULL;
	Init((char*)psz);
}

CS34String::CS34String(const CS34String &str)
{
	m_iBufferSize = 0;
	m_pData = NULL;
	Init(str.m_pData);
}

int CS34String::GetLength() const
{
	return m_iBufferSize;
}

BOOL CS34String::IsEmpty() const
{
	return (m_iBufferSize == 0) ? TRUE : FALSE;
}

CS34String::operator char*() const
{
	return m_pData;
}

CS34String::operator const char*() const
{
	return m_pData;
}

CS34String& CS34String::operator=(const CS34String &str)
{
	Init(str.m_pData);
	return *this;
}

CS34String& CS34String::operator=(const char *psz)
{
	Init((char*)psz);
	return *this;
}

CS34String& CS34String::operator+=(const CS34String &str)
{
	Concat(str.m_pData);	
	return *this;
}

CS34String& CS34String::operator+=(const char *psz)
{
	Concat((char*)psz);
	return *this;
}

int CS34String::operator<(const CS34String &str) const
{
	return (strcmp(m_pData, str.m_pData) < 0);
}
int CS34String::operator>(const CS34String &str) const
{
	return (strcmp(m_pData, str.m_pData) > 0);
}
int CS34String::operator>=(const CS34String &str) const
{
	return (strcmp(m_pData, str.m_pData) >= 0);
}
int CS34String::operator<=(const CS34String &str) const
{
	return (strcmp(m_pData, str.m_pData) <= 0);
}
int CS34String::operator==(const CS34String &str) const
{
	return (strcmp(m_pData, str.m_pData) == 0);
}

int CS34String::operator<(const char *psz) const
{
	return (strcmp(m_pData, psz) < 0);
}
int CS34String::operator>(const char *psz) const
{
	return (strcmp(m_pData, psz) > 0);
}
int CS34String::operator>=(const char *psz) const
{
	return (strcmp(m_pData, psz) >= 0);
}
int CS34String::operator==(const char *psz) const
{
	return (strcmp(m_pData, psz) == 0);
}

char *CS34String::GetBuffer(int iMinSize)
{
	if (iMinSize > m_iBufferSize)
	{
		CS34String s = *this;
		Init(iMinSize);
		*this = s;
	}
	return m_pData;
}

CS34String CS34String::Left(int iCount)
{
	CS34String s;
	if (iCount >= 0 && iCount < m_iBufferSize)
		AllocCopy(s, m_pData, iCount);
	return s;
}


CS34String CS34String::Mid(int iFirst, int iCount)
{
	if (iCount == -1 || iCount > m_iBufferSize)
		iCount = m_iBufferSize-1;

	CS34String s;
	if (iFirst >= 0 && iFirst < m_iBufferSize) 
		AllocCopy(s, &m_pData[iFirst], iCount);
	return s;
}

CS34String CS34String::Right(int iCount)
{
	if (iCount > m_iBufferSize)
		iCount = m_iBufferSize-1;

	CS34String s;
	if (iCount >= 0) 
		AllocCopy(s, &m_pData[m_iBufferSize-iCount-1], iCount);
	return s;
}


void CS34String::Erase()
{
	if (m_pData)
	{
		free(m_pData);
		m_pData = NULL;
	}
}


void CS34String::Init(char *p)
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

void CS34String::Init(int iCount)
{
	iCount++;
	if (m_pData)
	{
		if (iCount > m_iBufferSize)
		{
			free(m_pData);
			m_pData = (char*)malloc(iCount);
			if (m_pData==NULL)
				return;
		}
	}
	else
	{
		m_pData = (char*)malloc(iCount);
		if (m_pData==NULL)
			return;
	}
	m_iBufferSize = iCount;
}


void CS34String::Concat(char *p)
{
	int i = strlen(p)+1;
	m_iBufferSize += i;
	char *pData = (char*)malloc(m_iBufferSize);
	if (pData==NULL)
		return;

	strcpy(pData, m_pData);
	strcat(pData, p);

	if (m_pData)
		free(m_pData);
	m_pData = pData;
}

void CS34String::AllocCopy(CS34String& dest, char *pData, int iCount) const
{
	dest.Init(iCount);
	memcpy(dest.m_pData, pData, iCount);
	dest.m_pData[iCount] = 0;
}


UINT CS34String::HashKey()
{
	char *pKey = m_pData;
	UINT iHash = 0;
	while (*pKey)
		iHash = (iHash<<5) + iHash + *pKey++;
	return iHash;
}

char CS34String::GetAt(int i)
{
	if (i >= 0 && i < m_iBufferSize)
	{
		return m_pData[i];
	}
	return 0;
}

void CS34String::SetAt(int i, char ch)
{
	if (i >= 0 && i < m_iBufferSize)
	{
		m_pData[i] = ch;
	}
}

void CS34String::TruncateAt(int i)
{
	if (i >= 0 && i < m_iBufferSize)
	{
		m_iBufferSize = i;
		m_pData[i] = 0;
	}
}

void CS34String::Crypto(char *pszKey)
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


void CS34String::Format(char *pFormat, ...)
{
	char *p = (char*)malloc(32000);
	if (p==NULL)
		return;

	va_list argptr;
	va_start(argptr, pFormat);
	vsprintf(p, pFormat, argptr);
	va_end(argptr);

	Erase();
	Init(p);

	free(p);
}

int CS34String::Find(char *psz)
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

int CS34String::Find(const CS34String &str)
{
	return Find((char*)str);
}

void CS34String::DebugMsg()
{
	OutputDebugString(m_pData);
}

void CS34String::Upper()
{
	if (m_pData)
		strupr(m_pData);
}

void CS34String::Lower()
{
	if (m_pData)
		strlwr(m_pData);
}

void CS34String::Sscanf(const char* source, const char* format)
{
	Init((char*)source);
	sscanf(source, format, m_pData);
}
