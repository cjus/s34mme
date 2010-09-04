// Bitmap.cpp: implementation of the CS34Bitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S34Bitmap.h"
#include <stdio.h>
#include <stdlib.h>

#include "S34Palette.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS34Bitmap::CS34Bitmap()
{
	m_pData = NULL;
	m_RowLength = 0;
	m_TotalPaletteEntries = 0;
}

CS34Bitmap::~CS34Bitmap()
{
	if (m_pData)
	{
		delete []m_pData;
		m_pData = NULL;
	}
}

int CS34Bitmap::Load(char *pFileName)
{
	int ret = S34GRA_E_NOERROR;

	if (pFileName == NULL)
		return S34GRA_E_INVALIDPARAM;
	
	if (m_pData)
	{
		free(m_pData);
		m_pData = NULL;
	}
	BITMAPFILEHEADER bfh;
	int iSize;

	// open file
	//
	HANDLE hFile;
	hFile = ::CreateFile((const char*)pFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return S34GRA_E_FILEOPEN_FAILED;
	}

	DWORD nRead;
	if (!::ReadFile((HANDLE)hFile, &bfh, sizeof(BITMAPFILEHEADER), &nRead, NULL))
	{
		ret = S34GRA_E_FILEREAD_FAILED;
		BAIL();
	}

	if (!::ReadFile((HANDLE)hFile, &m_InfoHeader, sizeof(BITMAPINFOHEADER), &nRead, NULL))
	{
		ret = S34GRA_E_FILEREAD_FAILED;
		BAIL();
	}

	// allocate space for the dib bits
	//
	ComputeRowLength(m_InfoHeader.biWidth, m_InfoHeader.biBitCount);
	iSize = m_RowLength * m_InfoHeader.biHeight;
	m_pData = (BYTE*)malloc(iSize);
	if (m_pData == NULL)
	{
		ret = S34GRA_E_MEMORYALLOCATION_FAILED;
		BAIL();
	}
	ZeroMemory(m_pData,  iSize);


	if (m_InfoHeader.biBitCount == 8)
	{
//		#pragma _MEMO_("determine how many palette entries a bmp has and use only that many entries!")
		m_TotalPaletteEntries =  256;
		if (!::ReadFile((HANDLE)hFile, &m_colortable, m_TotalPaletteEntries * sizeof(RGBQUAD), &nRead, NULL))
		{
			ret = S34GRA_E_FILEREAD_FAILED;
			BAIL();
		}
	}

	if (!::ReadFile((HANDLE)hFile, m_pData, iSize, &nRead, NULL))
	{
		ret = S34GRA_E_FILEREAD_FAILED;
		BAIL();
	}

	::CloseHandle((HANDLE)hFile);
	return S34GRA_E_NOERROR;
exit_with_error:
	if (m_pData)
		free(m_pData);
	m_pData = NULL;
	::CloseHandle((HANDLE)hFile);
	return ret;
}

int CS34Bitmap::Save(char *pFileName)
{
	HRESULT hr = S34GRA_E_NOERROR;

	if (pFileName == NULL)
		return S34GRA_E_INVALIDPARAM;
	if (m_pData == NULL)
		return S34GRA_E_IMAGE_NOT_CREATED_NOR_LOADED;

	// compute header and pixel offsets
	//
	int iPixelOffset =	sizeof(BITMAPFILEHEADER) + 
						sizeof(BITMAPINFOHEADER) + 
						(m_TotalPaletteEntries * sizeof(RGBQUAD));
	int iPixelDataSize = (m_RowLength * m_InfoHeader.biHeight);
	int iFileSize = iPixelOffset + iPixelDataSize;


	// create header
	//
	BITMAPFILEHEADER bfh;
	bfh.bfType = 0x4D42; // "BM"
	bfh.bfSize = iFileSize;
	bfh.bfReserved1 = 0; 
    bfh.bfReserved2 = 0;
	bfh.bfOffBits = iPixelOffset; 

	// create file
	//
	HANDLE hFile;
	hFile = ::CreateFile((const char*)pFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return S34GRA_E_FILECREATE_FAILED;
	}

	DWORD nWritten;
	if (!::WriteFile((HANDLE)hFile, &bfh, sizeof(BITMAPFILEHEADER), &nWritten, NULL))
	{
		hr = S34GRA_E_FILEWRITE_FAILED;
		BAIL();
	}


	if (!::WriteFile((HANDLE)hFile, &m_InfoHeader, sizeof(BITMAPINFOHEADER), &nWritten, NULL))
	{
		hr = S34GRA_E_FILEWRITE_FAILED;
		BAIL();
	}

	if (m_InfoHeader.biBitCount == 8)
	{
		if (!::WriteFile((HANDLE)hFile, &m_colortable[0], m_TotalPaletteEntries * sizeof(RGBQUAD), &nWritten, NULL))
		{
		hr = S34GRA_E_FILEWRITE_FAILED;
		BAIL();
		}
	}

	if (!::WriteFile((HANDLE)hFile, m_pData, iPixelDataSize, &nWritten, NULL))
	{
		hr = S34GRA_E_FILEWRITE_FAILED;
		BAIL();
	}

	::CloseHandle((HANDLE)hFile);
	return S34GRA_E_NOERROR;
exit_with_error:
	::CloseHandle((HANDLE)hFile);
	return hr;
}

int CS34Bitmap::Create(int width, int height, int depth, CS34Palette *pPalette, BYTE *pData)
{
	int ret = S34GRA_E_NOERROR;
	BYTE *pSrc;
	BYTE *pDst;
	int row;


	// validate parameters
	//
	if (depth != 8 && depth != 24)
		return S34GRA_E_INVALIDPARAM;
	if (pData == NULL)
		return S34GRA_E_INVALIDIMAGEDATA;

	if (m_pData)
	{
		free(m_pData);
		m_pData = NULL;
	}

	// allocate space for the dib bits
	//
	ComputeRowLength(width, depth);
	int iSize = m_RowLength * height;
	m_pData = (BYTE*)malloc(iSize);
	if (m_pData == NULL)
	{
		return S34GRA_E_MEMORYALLOCATION_FAILED;
	}

	ZeroMemory(m_pData,  iSize);

	// init dib 
	//
  	m_InfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_InfoHeader.biWidth = width;
	m_InfoHeader.biHeight = height;
	m_InfoHeader.biPlanes = 1;
	m_InfoHeader.biBitCount = depth;
	m_InfoHeader.biCompression = BI_RGB;
	m_InfoHeader.biSizeImage = iSize;
	m_InfoHeader.biXPelsPerMeter = 0;
	m_InfoHeader.biYPelsPerMeter = 0;
	m_InfoHeader.biClrUsed =  (depth==8) ? 256 : 0;
	m_InfoHeader.biClrImportant = (depth==8) ? 256 : 0;

	// init palette data area
	//
	if (pPalette!= NULL)
	{
		pPalette->GetTotalColors(&m_TotalPaletteEntries);
		if (m_TotalPaletteEntries < 1 || m_TotalPaletteEntries > 256)
		{
			ret = S34GRA_E_PALETTE_EMPTY;
			BAIL();
		}

		for (int i = 0; i < (int)m_TotalPaletteEntries; i++)
		{
			pPalette->GetIndexData(i, 
								&m_colortable[i].rgbRed,
								&m_colortable[i].rgbGreen,
								&m_colortable[i].rgbBlue);
			m_colortable[i].rgbReserved = 0;
		}
	}

	// save packed data into dib pixel buffer
	// scanline format (dword aligned)
	//
	pSrc = pData;
	pDst = m_pData;
	width = (width * (depth/8));
	for (row = 0; row<height; row++)
	{
		memcpy(pDst, pSrc, width);
		memset(&pDst[width], 0, m_RowLength - width);
		pDst+=m_RowLength;
		pSrc+=width;
	}

	return S34GRA_E_NOERROR;
exit_with_error:
	m_TotalPaletteEntries = 0;
	if (m_pData)
		free(m_pData);
	m_pData = NULL;
	return ret;
}

int CS34Bitmap::GetSize(int *pWidth, int *pHeight)
{
	*pWidth = m_InfoHeader.biWidth;
	*pHeight = m_InfoHeader.biHeight;
	return S34GRA_E_NOERROR;
}

int CS34Bitmap::GetColorDepth(int *pColorDepth)
{
	*pColorDepth = m_InfoHeader.biBitCount;
	return S34GRA_E_NOERROR;
}

int CS34Bitmap::GetImageData(BYTE *pImgData, int iSize)
{
	// use iSize to validate buffer size
	//
	if ((m_InfoHeader.biWidth * m_InfoHeader.biHeight * (m_InfoHeader.biBitCount/8)) != iSize)
		return S34GRA_E_INVALIDSIZEPARAM;

	if (m_pData == NULL)
		return S34GRA_E_IMAGE_NOT_CREATED_NOR_LOADED;

	BYTE *pDst = pImgData;
	BYTE *pSrc = m_pData;

	int width = m_InfoHeader.biWidth * (m_InfoHeader.biBitCount/8);
	
	for (int height = 0; height < m_InfoHeader.biHeight; height++)
	{
		memcpy(pDst, pSrc, width);
		pDst += width;
		pSrc += m_RowLength;
	}

	return S34GRA_E_NOERROR;
}

int CS34Bitmap::GetPalette(CS34Palette *pPalette)
{
	if (pPalette == NULL)
		return S34GRA_E_INVALIDPARAM;

	for (int i = 0; i < (int)m_TotalPaletteEntries; i++)
	{
		pPalette->PutIndexData(i, 
							m_colortable[i].rgbRed,
							m_colortable[i].rgbGreen,
							m_colortable[i].rgbBlue);
	}
	pPalette->SetTotalColors(m_TotalPaletteEntries);
	return S34GRA_E_NOERROR;
}

int CS34Bitmap::GetColorIndex(BYTE red, BYTE green, BYTE blue, int *pIndex)
{
	*pIndex = -1;
	for (int i = 0; i < (int)m_TotalPaletteEntries; i++)
	{
		if (m_colortable[i].rgbRed == red && 
			m_colortable[i].rgbGreen == green && 
			m_colortable[i].rgbBlue == blue)
		{
			*pIndex = i;
			return S34GRA_E_NOERROR;
		}
	}

	return S34GRA_E_NOERROR;
}

void CS34Bitmap::ComputeRowLength(int iWidth, int iBitCount)
{ 
	m_RowLength = ((iWidth * iBitCount + 31) & ~31) >> 3; 
}

