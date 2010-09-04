// CS34XML.cpp: implementation of the CS34XML class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdlib.h"
#include "s34xml.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS34XML::CS34XML()
{
}

CS34XML::~CS34XML()
{
	if (m_List.MoveToHead())
	{
		char *pEntry;
		do 
		{
			pEntry = (char*)m_List.GetData();
			free(pEntry);
		} while (m_List.MoveNext());
	}
}

BOOL CS34XML::Setup(const char *pBuffer)
{
	strcpy(m_SrcBuffer, pBuffer);
	return TRUE;
}

BOOL CS34XML::Parse()
{
	char *pData = &m_SrcBuffer[0];
	char *pSeps = "\n";
	char *pToken;

	pToken = strtok(pData, pSeps);
	if (pToken)
	{
		CleanInput(pToken);
		AddToList(pToken);
	}
	while (pToken != NULL )
	{
		pToken = strtok(NULL, pSeps);
		if (pToken)
		{
			CleanInput(pToken);
			AddToList(pToken);
		}
	}
	return TRUE;
}


char *CS34XML::GetFirst()
{
	if (m_List.MoveToHead())
	{
		char *p = (char*)m_List.GetData();
		ConvertXMLTokenToUpperCase(p);
		return p;
	}

	return NULL;
}

char *CS34XML::GetNext()
{
	if (m_List.MoveNext())
	{
		char *p = (char*)m_List.GetData();
		ConvertXMLTokenToUpperCase(p);
		return p;
	}
	return NULL;
}

void CS34XML::CleanInput(char *pBuffer)
{
	if (pBuffer==NULL)
		return;
	StripChars(pBuffer);
	StripLeadingSpaces(pBuffer);
	StripTrailingSpaces(pBuffer);
	StripBeginningAndEndingQoutes(pBuffer);
}


void CS34XML::StripChars(char *pBuffer)
{
	int i, k=0;
	for (i=0; i < (int)strlen(pBuffer)+1; i++)
	{
		if (pBuffer[i] == 0x0D || pBuffer[i] == 0x0A || pBuffer[i] == 0x09)
		{
			continue;
		}
		pBuffer[k] = pBuffer[i];
		k++;
	}
	pBuffer[k] = 0;
}

void CS34XML::StripTrailingSpaces(char *pBuffer)
{
	for (int i=(int)strlen(pBuffer)-1; i > 0; i--)
	{
		if (pBuffer[i] == 0x20)
			pBuffer[i] = 0;
		else
			break;
	}
}


void CS34XML::StripLeadingSpaces(char *pBuffer)
{
	int k=0;
	int iLength = strlen(pBuffer);
	for (int i=0; i < iLength; i++)
	{
		if (pBuffer[i] == 0x20)
			k = i+1;
		else
			break;
	}
	memmove(pBuffer, &pBuffer[k], iLength - k);
	pBuffer[iLength - k] = 0;
}

void CS34XML::StripBeginningAndEndingQoutes(char *pBuffer)
{
	int i;
	int iLength = strlen(pBuffer);

	for (i=0; i < iLength; i++)
		if (pBuffer[i] == '"')
			break;
	if (i==iLength)
		return;

	memmove(&pBuffer[i], &pBuffer[i+1], iLength-i);

	iLength = strlen(pBuffer);
	for (i=iLength; i > 0; i--)
		if (pBuffer[i] == '"')
			break;
	memmove(&pBuffer[i], &pBuffer[i+1], iLength-i);
}

BOOL CS34XML::AddToList(char *pText)
{
	char *pEntry = NULL;
	int iLength = strlen(pText);

	BOOL bRet = TRUE;
	if (strstr(pText, "<")!=NULL)
	{
		// add begin token
		//
		for (int i=0; i< iLength; i++)
		{
			if (pText[i] == '<')
				break;
		}
		for (int j=i; j< iLength; j++)
		{
			if (pText[j] == '>')
				break;
		}
		j++;
 		memcpy(m_WorkingTextBuffer, &pText[i], j);
		m_WorkingTextBuffer[j] = 0;
		pEntry = (char*)malloc(j+1);
		if (!pEntry)
			return FALSE;
		memcpy(pEntry, m_WorkingTextBuffer, j);
		pEntry[j] = 0;
		m_List.Add(pEntry);

		if (j == iLength)
			return TRUE;

		// add value 
		//
		for (int k=j+1; k< iLength; k++)
		{
			if (pText[k] == '<')
				break;
		}
		pEntry = (char*)malloc(k-j+1);
		if (!pEntry)
			return FALSE;
		memcpy(pEntry, &pText[j], k-j);
		pEntry[k-j] = 0;
		m_List.Add(pEntry);

		
		// add end token
		//
		for (int l=iLength; l>0; l--)
		{
			if (pText[l] == '<')
				break;
		}

		pEntry = (char*)malloc(iLength - l + 1);
		if (!pEntry)
			return FALSE;
		memcpy(pEntry, &pText[l], iLength-l);
		pEntry[iLength-l] = 0;
		m_List.Add(pEntry);
	}
	else
		bRet = FALSE;

	return bRet;
}


void CS34XML::ConvertXMLTokenToUpperCase(char *pBuffer)
{
	if (pBuffer[0] == '<')
	{
		int iLength = strlen(pBuffer);
		int iStart = (pBuffer[1]=='/') ? 2 : 1;
		for (int i=iStart; i<iLength-1; i++)
		{
			pBuffer[i] = toupper(pBuffer[i]);
		}
	}
}
