// S34AnimPathManager.cpp: implementation of the CS34AnimPathManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S34AnimPathManager.h"
#include "S34DblList.h"
#include "malloc.h"

class CS34AnimPathEntry
{
public:
	POINT *pPoints;
	int iTotalSteps;
};

static CS34DblList CurvesList;

CS34AnimPathManager::CS34AnimPathManager()
{

}

CS34AnimPathManager::~CS34AnimPathManager()
{
	CS34AnimPathEntry *pEntry;
	if (CurvesList.MoveToHead())
	{
		do 
		{
			pEntry = (CS34AnimPathEntry*)CurvesList.GetData();
			delete []pEntry->pPoints;
			delete pEntry;
		} while (CurvesList.MoveNext());
	}
}

HANDLE CS34AnimPathManager::CreatePath(POINT ptStartPoint, POINT ptEndPoint, POINT ptControlPoint, int iSteps)
{
	CS34AnimPathEntry *pEntry = new CS34AnimPathEntry();
	if (pEntry == NULL)
		return NULL;

	pEntry->pPoints = new POINT[iSteps];
	if (pEntry->pPoints == NULL)
		return NULL;

	pEntry->iTotalSteps = iSteps;

	m_CurveGenerator.Reset();
	m_CurveGenerator.SetTotalSegments(iSteps);
	m_CurveGenerator.GeneratePoints(ptStartPoint, ptEndPoint, ptControlPoint);
	m_CurveGenerator.GetPoints(pEntry->pPoints, pEntry->iTotalSteps);

	CurvesList.Add(pEntry);
	return (HANDLE)pEntry;
}

BOOL CS34AnimPathManager::GetPath(HANDLE hHandle, POINT **pData, int *piSteps)
{
	CS34AnimPathEntry *pEntry;
	if (CurvesList.MoveToHead())
	{
		do 
		{
			pEntry = (CS34AnimPathEntry*)CurvesList.GetData();
			if (pEntry == (CS34AnimPathEntry*)hHandle)
			{
				*pData = pEntry->pPoints;
				*piSteps = pEntry->iTotalSteps;
				return TRUE;
			}
		} while (CurvesList.MoveNext());
	}
	return FALSE;
}

