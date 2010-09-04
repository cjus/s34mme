// S34DblList.cpp: implementation of the CS34DblList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "S34DblList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS34DblList::CS34DblList()
{
	m_pHead = m_pTail = m_pCurrent = NULL;
}

CS34DblList::~CS34DblList()
{
	Clear();
}

void CS34DblList::Clear()
{
	if (m_pHead == NULL)
		return;

	DblList *p = m_pTail;
	DblList *pTemp;
	while (p)
	{
		pTemp = p->pPrev;
		delete p;
		p = NULL;
		p = pTemp;
	}
	delete p;
	p = NULL;
	m_pHead = m_pTail = m_pCurrent = NULL;
}

BOOL CS34DblList::Add(void* pData)
{
	if (m_pHead == NULL)
	{
		m_pHead = new DblList();
		if (m_pHead==NULL)
			return FALSE;
		m_pCurrent = m_pHead;
		m_pCurrent->pData = pData;
		m_pCurrent->pNext = NULL;
		m_pCurrent->pPrev = NULL;
		m_pTail = m_pCurrent;
		return TRUE;
	}


	DblList *p = m_pTail;
	p->pNext = (DblList*)malloc(sizeof(DblList));
	p = p->pNext;
	p->pPrev = m_pTail;
	p->pData = pData;
	p->pNext = NULL;
	m_pTail = p;
	return TRUE;
}

int CS34DblList::GetTotalEntries()
{
	int iRet = 0;

	if (MoveToHead())
	{
		while (MoveNext())
		{
			iRet++;
		}
		iRet++;
	}

	MoveToHead();
	return iRet;
}

void* CS34DblList::GetData()
{
	if (m_pCurrent==NULL)
		return NULL;
	return m_pCurrent->pData;
}

BOOL CS34DblList::MoveToHead()
{
	if (m_pHead == NULL)
		return FALSE;
	m_pCurrent = m_pHead;
	return TRUE;
}

BOOL CS34DblList::MoveToTail()
{
	if (m_pTail == NULL)
		return FALSE;
	m_pCurrent = m_pTail;
	return TRUE;
}

BOOL CS34DblList::MoveNext()
{
	BOOL bRet = TRUE;
	if (m_pCurrent != NULL)
		m_pCurrent = m_pCurrent->pNext;
	if (m_pCurrent == NULL)
		bRet = FALSE;
	return bRet;	
}

BOOL CS34DblList::MovePrev()
{
	BOOL bRet = TRUE;
	if (m_pCurrent != NULL)
		m_pCurrent = m_pCurrent->pPrev;
	if (m_pCurrent == NULL)
		bRet = FALSE;
	return bRet;	
}

BOOL CS34DblList::Remove(void* pData)
{
	// find item
	DblList *p = m_pTail;
	while (p)
	{
		if (p->pData == pData)
		{
			// item found remove it
			if (p == m_pHead)
			{
				m_pHead = p->pNext;
			}
			else if (p == m_pTail)
			{
				m_pTail->pPrev->pNext = NULL;
				m_pTail = m_pTail->pPrev;
			}
			else if (p == m_pCurrent)
			{
				m_pCurrent = p->pPrev;
			}
			else
			{
				p->pPrev->pNext = p->pNext;
				p->pNext->pPrev = p->pPrev;
			}
			delete p;
			return TRUE;
		}

		p = p->pPrev;
	}
	return FALSE;
}


BOOL CS34DblList::IsEmpty()
{
	return (m_pHead==NULL);
}



//void* CS34DblList::operator new(size_t bytes)
//{
//	void* p = malloc(bytes);
//    return p;
//} 
//
//void CS34DblList::operator delete(void*p)
//{
//    free(p);
//}
