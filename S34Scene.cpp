#include "stdafx.h"
#include "s34Scene.h"

//#define FAST_DRAW 1

CS34Scene::CS34Scene()
{
	m_pBackgroundImg = NULL;
	m_pVirginImg = NULL;
	m_bSortScene = TRUE;
	pAnimContainerTop = pAnimContainer = (AnimContainer*)NULL;

	m_dwPitch = 0;
	m_dwRectTotal = 0;
}

CS34Scene::~CS34Scene()
{
	free(m_pBackgroundImg);
	free(m_pVirginImg);

	pAnimContainer = pAnimContainerTop;
	if (pAnimContainer != NULL)
	{
		AnimContainer *p;
		while (TRUE)
		{
			p = pAnimContainer->pNext;
			delete pAnimContainer;
			if (p==NULL)
				break; 
			else
				pAnimContainer = p;
		}
	}

	m_pBackgroundImg = NULL;
	m_pVirginImg = NULL;
	pAnimContainerTop = NULL;
}

int CS34Scene::Setup(BYTE *pImageData, int width, int height, int color_depth)
{
	m_ScreenRect.left = 0;
	m_ScreenRect.top = 0;
	m_ScreenRect.right = width;
	m_ScreenRect.bottom = height;
	m_ColorDepth = color_depth;

	int iSize = width*height*(color_depth/8);

	m_dwPitch = (color_depth/8 == 1) ?	width : width<<1;

	m_pBackgroundImg = (BYTE*)malloc(iSize);
	m_pVirginImg = (BYTE*)malloc(iSize);
	if (m_pBackgroundImg==NULL || m_pVirginImg == NULL)
	{
		#pragma message("Make sure to create problem message ids for this anim class")
		return -1; // error
	}
	
	memcpy(m_pBackgroundImg, pImageData, iSize);
	memcpy(m_pVirginImg, pImageData, iSize);

	return 0;
}

void CS34Scene::OnChron()
{
	// tell all anims which are running automation scripts
	// to update their internal state machines
	// 
	CS34Anim *pAnimObject;
	pAnimContainer = pAnimContainerTop;
	if (pAnimContainer==NULL)
		return;
	while (TRUE)
	{
		pAnimObject = pAnimContainer->pAnimObject;
//		if (pAnimObject->IsAnimScriptLoaded())
			pAnimObject->OnChron();
		if (pAnimContainer->pNext == NULL)
			break;
		pAnimContainer = pAnimContainer->pNext;
	}
}

void CS34Scene::Update(BYTE* pSurface, LONG lPitch, BOOL bFullScreen, HWND hWnd)
{
//	OutputDebugString("\tUpdate()\n");
	BYTE *pDst;
	BYTE *pSrc;
	DWORD dwRectTotal = 0;
	int width, height, h;
	CS34Anim *pAnimObject;
	CS34Rect NewRect;
	CS34Rect tmpRect;

	OnChron();

	pAnimContainer = pAnimContainerTop;
	if (pAnimContainer == NULL)
	{
		if (bFullScreen)
		{
			pDst = pSurface;
			pSrc = m_pBackgroundImg;
			width = m_dwPitch;
			height = m_ScreenRect.GetHeight();
			for (h = 0; h < height; h++)
			{
				memcpy(pDst, pSrc, width);
				pSrc += width;
				pDst += lPitch;
			}
		}
		return;
	}

	//-----------------------------------------------------------
	// Check for Z Order changes.  Sort items that have changed 
	//-----------------------------------------------------------
//	if (m_bSortScene)
//	{
		int a1;

		CS34Anim *pAnimObject1, *pAnimObject2;
		AnimContainer *pAnimContainer1, *pAnimContainer2;

		pAnimContainer1 = pAnimContainerTop;
		while (TRUE)
		{
			pAnimObject1 = pAnimContainer1->pAnimObject;
			if (pAnimObject1->IsZChange())
			{
				a1 = pAnimObject1->GetZOrder();
				pAnimContainer2 = pAnimContainerTop;
				while (TRUE)
				{
					pAnimObject2 = pAnimContainer2->pAnimObject;
					if (a1 < pAnimObject2->GetZOrder())
					{
						CS34Anim *pObject = pAnimObject2;
						pAnimContainer2->pAnimObject = pAnimContainer1->pAnimObject;
						pAnimContainer1->pAnimObject = pObject;
						break;
					}
					if (pAnimContainer2->pNext == NULL)
						break;
					pAnimContainer2 = pAnimContainer2->pNext;
				}
			}

			if (pAnimContainer1->pNext == NULL)
				break;
			pAnimContainer1 = pAnimContainer1->pNext;
		}
//	}


	if (bFullScreen)
	{
		memcpy(m_pBackgroundImg, m_pVirginImg, 480*m_dwPitch);
		pAnimContainer = pAnimContainerTop;

		while (TRUE)
		{
			pAnimObject = pAnimContainer->pAnimObject;
			NewRect = pAnimObject->GetBoundingMovementRect();
			if (tmpRect.IntersectRect(m_ScreenRect, NewRect))
				pAnimObject->Update(m_ColorDepth, &tmpRect);
			if (pAnimContainer->pNext == NULL)
				break;
			pAnimContainer = pAnimContainer->pNext;
		}

		BYTE *pDst = pSurface;
		BYTE *pSrc = m_pBackgroundImg;
		int width = m_dwPitch;
		int height = m_ScreenRect.GetHeight();

		for (int h = 0; h < height; h++)
		{
			memcpy(pDst, pSrc, width);
			pSrc += width;
			pDst += lPitch;
		}
		return;
	}


	// scan dirty sprites and build dirty rect list
	//
	CS34Size AnimSize;
	pAnimContainer = pAnimContainerTop;
	while (TRUE)
	{
		pAnimObject = pAnimContainer->pAnimObject;
		if (pAnimObject->IsDirty() && pAnimObject->GetState() != CS34Anim::HIDDEN)
		{
			RectCache[dwRectTotal] = pAnimObject->GetBoundingMovementRect();
			RectCache[dwRectTotal].IntersectRect(RectCache[dwRectTotal],m_ScreenRect);
			dwRectTotal++;
		}
		if (pAnimContainer->pNext == NULL)
			break;
		pAnimContainer = pAnimContainer->pNext;
	}


	// restore background
	//
	if (dwRectTotal)
	{
		for (int i = 0; i < (int)dwRectTotal; i++)
		{
			if (RectCache[i].IsRectEmpty())
				continue;

			int lineWidth = m_dwPitch;
			if (m_dwPitch > 640)
 				width = (RectCache[i].right - RectCache[i].left)<<1;
			else
 				width = (RectCache[i].right - RectCache[i].left);

			height = RectCache[i].bottom - RectCache[i].top;

#ifdef BLIT_TEST
			if (m_dwPitch > 640)
 				width = 640<<1;
			else
 				width = 640;
			height = 480;
#endif // BLIT_TEST

			int iOffsetLocation;
			if (m_dwPitch > 640)
				iOffsetLocation = (RectCache[i].top * lineWidth) + (RectCache[i].left<<1);
			else
				iOffsetLocation = (RectCache[i].top * lineWidth) + (RectCache[i].left);

#ifdef BLIT_TEST
			pDst = m_pBackgroundImg;
			pSrc = m_pVirginImg;
#else
			pDst = m_pBackgroundImg + iOffsetLocation;
			pSrc = m_pVirginImg + iOffsetLocation;
#endif //BLIT_TEST

			for (h = 0; h < height; h++)
			{
				memcpy(pDst, pSrc, width);
				pSrc += lineWidth;
				pDst += lineWidth;
			}
		}


		// Ask AnimObjects to draw/update themselves
		//
		pAnimContainer = pAnimContainerTop;
		while (TRUE)
		{
			pAnimObject = pAnimContainer->pAnimObject;
			if (!pAnimObject->IsHidden())
			{
				pAnimObject->GetRect(&NewRect);

				// check animobject against each rect
				for (int i = 0; i < (int)dwRectTotal; i++)
				{
					if (RectCache[i].IsRectEmpty())
						continue;
					if (tmpRect.IntersectRect(NewRect, RectCache[i]))
						pAnimObject->Update(m_ColorDepth,&tmpRect);
				}
			}
			if (pAnimContainer->pNext == NULL)
				break;
			pAnimContainer = pAnimContainer->pNext;
		}

		// Blit to surface image
		//
		for (i = 0; i < (int)dwRectTotal; i++)
		{
			if (RectCache[i].IsRectEmpty())
				continue;
#ifdef BLIT_TEST
			int lineWidth = m_dwPitch;
			if (lineWidth > 640)
			{
	 			width = 640*2;
				height = 480;
				pDst = pSurface;
				pSrc = m_pBackgroundImg;
			}
			else
			{
	 			width = 640;
				height = 480;
				pDst = pSurface;
				pSrc = m_pBackgroundImg;
			}

			for (h = 0; h < height; h++)
			{
				memcpy(pDst, pSrc, width);
				pSrc += lineWidth;
				pDst += lPitch;
			}
#else
			int lineWidth = m_dwPitch;
			if (lineWidth > 640)
			{
	 			width = (RectCache[i].right - RectCache[i].left)*2;
				height = RectCache[i].bottom - RectCache[i].top;
				pDst = pSurface + (RectCache[i].top * lPitch) + (RectCache[i].left*2);
				pSrc = m_pBackgroundImg + (RectCache[i].top * lineWidth) + (RectCache[i].left*2);
			}
			else
			{
	 			width = (RectCache[i].right - RectCache[i].left);
				height = RectCache[i].bottom - RectCache[i].top;
				pDst = pSurface + (RectCache[i].top * lPitch) + (RectCache[i].left);
				pSrc = m_pBackgroundImg + (RectCache[i].top * lineWidth) + (RectCache[i].left);
			}

			for (h = 0; h < height; h++)
			{
				memcpy(pDst, pSrc, width);
				pSrc += lineWidth;
				pDst += lPitch;
			}
#endif //BLIT_TEST
		}

	}

	m_dwRectTotal = dwRectTotal;
}


void CS34Scene::UpdateWindow(HWND hWnd,	HBITMAP hDibSection, HPALETTE hPalette)
{
//	OutputDebugString("\tUpdateWindow()\n");

//	if (!m_dwRectTotal)
//		return;

	// Blit to surface image
	//
	HPALETTE hOldPalette;
	HDC hDC = GetDC(hWnd);
	HDC dcMem = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap;

	if (hPalette)
	{
		hOldPalette = SelectPalette(hDC, hPalette, FALSE);
		RealizePalette(hDC);
	}

	hOldBitmap = (HBITMAP)::SelectObject(dcMem, hDibSection);

	if (!m_dwRectTotal)
	{
		::BitBlt(hDC,
				0,0,
				640,480,
				dcMem,
				0,0,
				SRCCOPY);
	}
	else
	{
	#ifdef BLIT_TEST
		HBRUSH br = CreateSolidBrush(RGB(0,255,0));
		RECT rect;
	#endif //BLIT_TEST
		for (int i = 0; i < (int)m_dwRectTotal; i++)
		{
			if (!RectCache[i].IsRectEmpty())
			{
				::BitBlt(hDC,
						RectCache[i].left,RectCache[i].top,
						RectCache[i].right-RectCache[i].left,RectCache[i].bottom-RectCache[i].top,
						dcMem,
						RectCache[i].left,RectCache[i].top,
						SRCCOPY);
	#ifdef BLIT_TEST
				rect.left = RectCache[i].left+1;
				rect.top = RectCache[i].top+1;
				rect.right = RectCache[i].right-1;
				rect.bottom = RectCache[i].bottom-1;
				FrameRect(hDC, &rect, br);
	#endif //BLIT_TEST
			}
		}
	#ifdef BLIT_TEST
		DeleteObject(br);
	#endif //BLIT_TEST
	}

	::SelectObject(dcMem, hOldBitmap);
	if (hPalette)
		SelectPalette(hDC, hOldPalette, FALSE);
	::DeleteDC(dcMem);
	::ReleaseDC(hWnd, hDC);
	m_dwRectTotal = 0;
}

BOOL CS34Scene::AttachAnimObject(CS34Anim* pAnimObject)
{
	int fAdded = 0;
	AnimContainer *pNew;

	// every time we attach an anim object, we let it know where 
	// it will be rendering!
	//
	pAnimObject->SetCompositeSurface(GetCompositImg());

	pAnimObject->SetDstPitch(m_dwPitch);

	pAnimContainer = pAnimContainerTop;
	if (pAnimContainer == NULL)
	{
		pAnimContainer = new AnimContainer;
		pAnimContainer->pAnimObject = pAnimObject;
		pAnimContainer->pLast = NULL;
		pAnimContainer->pNext = NULL;
		pAnimContainerTop = pAnimContainer;
		return TRUE;
	}

	// get Z order for new object
	int a1 = pAnimObject->GetZOrder();

	while (TRUE)
	{
		if (a1 < pAnimContainer->pAnimObject->GetZOrder())
		{
			if (pAnimContainer->pLast == NULL)
			{
				pNew = new AnimContainer;
				pNew->pAnimObject = pAnimObject;
				pNew->pLast = NULL;
				pNew->pNext = pAnimContainer;
				pAnimContainer->pLast = pNew;
				pAnimContainerTop = pNew;
				fAdded++;
				break;
			}
			else
			{
				pNew = new AnimContainer;

				pNew->pAnimObject = pAnimObject;
				pNew->pLast = pAnimContainer->pLast;
				pNew->pNext = pAnimContainer;

				(pAnimContainer->pLast)->pNext = pNew;
				pAnimContainer->pLast = pNew;

				fAdded++;
				break;
			}
		}

		if (pAnimContainer->pNext == NULL)
			break;
		else
			pAnimContainer = pAnimContainer->pNext;
	}

	if (!fAdded)
	{
		pNew = new AnimContainer;
		pNew->pAnimObject = pAnimObject;
		pNew->pLast = pAnimContainer;
		pNew->pNext = NULL;

		pAnimContainer->pNext = pNew;
	}
	return TRUE; 
}


CS34Anim* CS34Scene::GetAnimFromPoint(CS34Point *point)
{
	CS34Image *pImage;
	CS34Rect Rect;
	int iZOrder;
	CS34Point AnimLocation;
	CS34Anim *pAnim = NULL;
	CS34Anim *pAnimObject;
	int	nCurrZ=(int)0x80000001;

	pAnimContainer = pAnimContainerTop;
	while (pAnimContainer)
	{
		pAnimObject = pAnimContainer->pAnimObject;
		if (pAnimObject->GetState() != CS34Anim::HIDDEN)
		{
			pAnimObject->GetLocation(&AnimLocation);

			if ((point->x >= AnimLocation.x) || (point->y >= AnimLocation.y))
			{
				pImage = pAnimObject->GetCurrentImageFrame();
				if (!pImage->HasMask())
				{
					pAnimObject->GetRect(&Rect);
					if (Rect.IsPointInRect(point->x,point->y))
					{
						iZOrder = pAnimObject->GetZOrder();
						if (iZOrder > nCurrZ)
						{
							pAnim = pAnimObject;
							nCurrZ = iZOrder;
						}
					}
				}
				else
				{
					int iX, iY;
					iX = point->x - AnimLocation.x;
					iY = point->y - AnimLocation.y;
					if (pAnimObject->IsPointInAnim(iX, iY))
					{
						iZOrder = pAnimObject->GetZOrder();
						if (iZOrder > nCurrZ)
						{
							pAnim = pAnimObject;
							nCurrZ = iZOrder;
						}
					}
				}
			}
		}
		pAnimContainer = pAnimContainer->pNext;
	}
	return pAnim;
}

BOOL CS34Scene::AnimCollide(CS34Anim* pAnim1, CS34Anim* pAnim2)
{
	CS34Rect Rect1;
	CS34Rect Rect2;
	CS34Rect tmpRect;	
	pAnim1->GetRect(&Rect1);
	pAnim2->GetRect(&Rect2);

	return (Rect1.IntersectRect(Rect2)) ? TRUE : FALSE;
}

