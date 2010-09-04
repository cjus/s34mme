// Palette.cpp: implementation of the CS34Palette class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S34Palette.h"

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS34Palette::CS34Palette()
{
	m_iTotalColors = 0;
}

CS34Palette::~CS34Palette()
{

}

int CS34Palette::Load(char *pFileName)
{
	int ret = S34GRA_E_NOERROR;
	int i;
	PALETTEENTRY *pal;

	m_iTotalColors = 256;

	const int RIFF_HEADER_SIZE = 24;
	const int RIFF_DATA_SIZE = 1048;
	FILE* f;

	char *pBuffer = new char[RIFF_DATA_SIZE];
	if (pBuffer == NULL)
	{
		ret = S34GRA_E_MEMORYALLOCATION_FAILED;
		BAIL();
	}

	pal = new PALETTEENTRY[256];
	if (pal==NULL)
	{
		ret = S34GRA_E_MEMORYALLOCATION_FAILED;
		BAIL();
	}

	// Open Pal file and extract palette data
	if ((f = fopen(pFileName, "rb")) == NULL)
	{
		ret = S34GRA_E_FILEOPEN_FAILED;
		BAIL();
	}

	if (fread(pBuffer, sizeof(char), (unsigned int)RIFF_DATA_SIZE, f) == NULL)
	{
		ret = S34GRA_E_FILEREAD_FAILED;
		BAIL();
	}
	fclose(f);

	memcpy(pal, &pBuffer[RIFF_HEADER_SIZE], RIFF_DATA_SIZE - RIFF_HEADER_SIZE);
	for (i = 0; i < 256; i++)
	{
		m_colormap[i].rgbRed   = pal[i].peRed;
		m_colormap[i].rgbGreen = pal[i].peGreen;
		m_colormap[i].rgbBlue  = pal[i].peBlue;
		m_colormap[i].rgbReserved = 0;
	}

	delete []pBuffer;
	delete []pal;
	return S34GRA_E_NOERROR;
exit_with_error:
	if (pBuffer)
		delete []pBuffer;
	if (pal)
		delete []pal;
	return ret;
}

int CS34Palette::FindNearestColorMatch(BYTE red, BYTE green, BYTE blue, BYTE *pIndex)
{
	pIndex = NULL;

	register int i;
	WORD closest = 255L + 255L + 255L + 1L;
	WORD distance = 0L;
	int  nIndex = -1;

	// use pointer to see if we can speed up the process
	BYTE *pal = (BYTE *)m_colormap;

	for (i = 0; i < 256; i++)
	{
		//distance = abs(m_colormap[i].blue  - color.blue)  + 
		//		   abs(m_colormap[i].green - color.green) + 
		//		   abs(m_colormap[i].red   - color.red);
		distance = abs((*pal++) - red)  + 
				   abs((*pal++) - green) + 
				   abs((*pal++) - blue);
		pal++;

		if (distance == 0)
			return i;

		if (distance < closest)
		{
			closest = distance;
			nIndex =  i;
		}
	}
	
	*pIndex = nIndex;
	return S34GRA_E_NOERROR;
}



int CS34Palette::GetIndexData(int iIndex, BYTE *pRed, BYTE *pGreen, BYTE *pBlue)
{
	*pRed = NULL;
	*pGreen = NULL;
	*pBlue = NULL;

	if (iIndex < 0 || iIndex > 256)
		return S34GRA_E_INVALIDPARAM;

	*pRed = m_colormap[iIndex].rgbRed;
	*pGreen = m_colormap[iIndex].rgbGreen;
	*pBlue = m_colormap[iIndex].rgbBlue;
	return S34GRA_E_NOERROR;
}


int CS34Palette::GetTotalColors(int *pTotalColors)
{
	*pTotalColors = m_iTotalColors;
	return S34GRA_E_NOERROR;
}

int CS34Palette::PutIndexData(int index, BYTE red, BYTE green, BYTE blue)
{
	if (index < 0 || index > 256)
		return S34GRA_E_INVALIDPARAM;

	m_colormap[index].rgbRed = red;
	m_colormap[index].rgbGreen = green;
	m_colormap[index].rgbBlue = blue;

	return S34GRA_E_NOERROR;
}

int CS34Palette::SetTotalColors(int iTotalColors)
{
	if (iTotalColors < 0 || iTotalColors > 256)
		return S34GRA_E_INVALIDPARAM;

	m_iTotalColors = iTotalColors;
	return S34GRA_E_NOERROR;
}

int CS34Palette::CreateWindowHPALETTE(HPALETTE &hPalette, HDC hDC)
{
	if (m_iTotalColors == 0)
		return S34GRA_E_PALETTE_NOT_LOADED;

	LOGPALETTE *pPal;
	int nSize = sizeof(LOGPALETTE) + (256 * sizeof(PALETTEENTRY));
    pPal = (LOGPALETTE*) new BYTE[nSize];
	if (pPal == NULL)
		return S34GRA_E_MEMORYALLOCATION_FAILED;

    pPal->palNumEntries = 256;
    pPal->palVersion = 0x300;

	GetSystemPaletteEntries(hDC, 0, pPal->palNumEntries, pPal->palPalEntry);
	int i,j=0;


	if (m_colormap[0].rgbRed == 0 &&
		m_colormap[0].rgbGreen == 0 &&
		m_colormap[0].rgbBlue == 0 &&
		m_colormap[255].rgbRed == 255 &&
		m_colormap[255].rgbGreen == 255 &&
		m_colormap[255].rgbBlue == 255)
	{
		// why looka here, we got ourselves an windows identity palette
		for (i=0; i<10; i++)
		{
			m_colormap[i].rgbRed = pPal->palPalEntry[i].peRed;
			m_colormap[i].rgbGreen = pPal->palPalEntry[i].peGreen;
			m_colormap[i].rgbBlue = pPal->palPalEntry[i].peBlue;
		}

		for (i=10; i<246; i++)
		{
			pPal->palPalEntry[i].peRed = m_colormap[i].rgbRed;
			pPal->palPalEntry[i].peGreen = m_colormap[i].rgbGreen;
			pPal->palPalEntry[i].peBlue = m_colormap[i].rgbBlue;
			pPal->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
		}

		for (i=246; i<256; i++)
		{
			m_colormap[i].rgbRed = pPal->palPalEntry[i].peRed;
			m_colormap[i].rgbGreen = pPal->palPalEntry[i].peGreen;
			m_colormap[i].rgbBlue = pPal->palPalEntry[i].peBlue;
		}
	}
	else
	{
		for (i=10; i<246; i++,j++)
		{
			pPal->palPalEntry[i].peRed = m_colormap[j].rgbRed;
			pPal->palPalEntry[i].peGreen = m_colormap[j].rgbGreen;
			pPal->palPalEntry[i].peBlue = m_colormap[j].rgbBlue;
			pPal->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
		}

		for (i=0; i<10; i++)
		{
			m_colormap[i].rgbRed = pPal->palPalEntry[i].peRed;
			m_colormap[i].rgbGreen = pPal->palPalEntry[i].peGreen;
			m_colormap[i].rgbBlue = pPal->palPalEntry[i].peBlue;
		}


		for (i=246; i<256; i++)
		{
			m_colormap[i].rgbRed = pPal->palPalEntry[i].peRed;
			m_colormap[i].rgbGreen = pPal->palPalEntry[i].peGreen;
			m_colormap[i].rgbBlue = pPal->palPalEntry[i].peBlue;
		}

		for (i=0; i<256; i++)
		{
			m_colormap[i].rgbRed = pPal->palPalEntry[i].peRed;
			m_colormap[i].rgbGreen = pPal->palPalEntry[i].peGreen;
			m_colormap[i].rgbBlue = pPal->palPalEntry[i].peBlue;
		}
	}

    hPalette = CreatePalette(pPal);

	delete []pPal;
	return S34GRA_E_NOERROR;
}



int CS34Palette::FindIndexColor(int *pIndex, BYTE red, BYTE green, BYTE blue)
{
	for (int i=0; i < 256; i++)
	{
		if (m_colormap[i].rgbRed == red &&
			m_colormap[i].rgbGreen == green &&
			m_colormap[i].rgbBlue == blue)
		{
			*pIndex = i;
			return S34GRA_E_NOERROR;
		}
	}
	*pIndex = -1;
	return S34GRA_E_NOERROR;
}

