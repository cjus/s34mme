// S34Image.cpp: implementation of the CS34Image class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S34Image.h"
#include "S34Palette.h"

#include "stdlib.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS34Image::CS34Image()
{
	m_width = 0;
	m_height = 0;
	m_pData = NULL;
	m_iDataCount = 0;
	m_iTotalColors = 0;
	m_bHasMask = FALSE;
	m_bHasExternalPalette = FALSE;
	m_ImageType = eS34IMG_SMALL;
	m_pColorTable = NULL;
	m_pImageMask = NULL;
	m_iColorDepth = 8;
	m_bBitModePalette = FALSE;
}

CS34Image::~CS34Image()
{
	if (m_pData)
		delete []m_pData;

	if (!m_bHasExternalPalette)
		delete []m_pColorTable;
	
	m_pData = NULL;
}

int CS34Image::Create(int width, int height, 
					  int TransparentColorIndex, 
					  BYTE bUseExternalPalette, BYTE bCreateMask, 
					  CS34Palette *pPalette, BYTE*pData)
{
	int ret = S34GRA_E_NOERROR;

	int *pScanLine;

	m_bHasMask = bCreateMask;
	m_bHasExternalPalette = bUseExternalPalette;

	RGBTRIPLE *pColorTableEntry=NULL;
	RGBTRIPLE TransparencyColorTableEntry;
	int nBeginSegment = -1;
	int iTotalSegments = 0;
	int iCount = 0;
	int iDataSize = 256000;
	int line;
	BYTE *pSrc;
	int j;

	// validate parameters
	//
	if (TransparentColorIndex > 256 ||
		pPalette == NULL ||
		pData == NULL ||
		width < 1 || height < 1)
		return S34GRA_E_INVALIDPARAM;

	m_iDataCount = 0;
	pPalette->GetTotalColors(&m_iTotalColors);
	// if colors less then 1 return error
	//
	if (m_iTotalColors < 1)
		return S34GRA_E_PALETTE_EMPTY;

	if (m_pData)
		delete []m_pData;

	if (m_pImageMask)
		delete m_pImageMask;

	// allocate a block of memory used to hold the converted image
	// the amount of memory allocated is a guess and will be increased
	// as needed duing the execution of this function.
	//
	m_pData = (BYTE*)new BYTE[iDataSize];
	if (m_pData == NULL)
		return S34GRA_E_MEMORYALLOCATION_FAILED;

	// determine whether we're encoding a small or large image
	//
	if (width > 255 || height > 255)
	{
		// encode large image
		m_ImageType = eS34IMG_LARGE;
		m_lih.ihHasExternalPalette = m_bHasExternalPalette;
		m_lih.ihHasMask = m_bHasMask;
		m_lih.ihImageType = eS34IMG_LARGE;
		m_lih.ihWidth = width;
		m_lih.ihHeight = height;
		m_width = m_lih.ihWidth;
		m_height = m_lih.ihHeight;
		m_lih.ihTotalSegments = 0; // we'll set this later!
		m_lih.ihTotalPaletteEntries = m_iTotalColors-1;
	}
	else
	{
		// code small image
		m_ImageType = eS34IMG_SMALL;
		m_sih.ihHasExternalPalette = m_bHasExternalPalette;
		m_sih.ihHasMask = m_bHasMask;
		m_sih.ihImageType = eS34IMG_SMALL;
		m_sih.ihWidth = width;
		m_sih.ihHeight = height;
		m_width = m_sih.ihWidth;
		m_height = m_sih.ihHeight;
		m_sih.ihTotalSegments = 0; // we'll set this later!
		m_sih.ihTotalPaletteEntries = m_iTotalColors-1;
	}

	if (bCreateMask)
	{
		m_pImageMask = new CS34ImageMask();
		if (m_pImageMask == NULL)
		{
			ret = S34GRA_E_MEMORYALLOCATION_FAILED;
			BAIL();
		}

		// we set mask to all true for non transparent images
		// that need a mask.
		//
		if (TransparentColorIndex != -1)
			m_pImageMask->Create(m_width, m_height, FALSE);
		else
			m_pImageMask->Create(m_width, m_height, TRUE);
	}

	// init internal color table with the passed in palette object
	//
	m_pColorTable = new RGBTRIPLE[256];
	if (m_pColorTable == NULL)
	{
		ret = S34GRA_E_MEMORYALLOCATION_FAILED;
		BAIL();
	}

	for (j = 0; j < m_iTotalColors; j++)
	{
		pPalette->GetIndexData(j, &m_pColorTable[j].rgbtRed, &m_pColorTable[j].rgbtGreen, &m_pColorTable[j].rgbtBlue);
	}

	if (TransparentColorIndex != -1)
	{
		TransparencyColorTableEntry.rgbtBlue = m_pColorTable[TransparentColorIndex].rgbtBlue;
		TransparencyColorTableEntry.rgbtGreen  = m_pColorTable[TransparentColorIndex].rgbtGreen;
		TransparencyColorTableEntry.rgbtRed = m_pColorTable[TransparentColorIndex].rgbtRed;
	}

	// allocate space for a single scanline
	//
	pScanLine = new int[width];

	if (pScanLine==NULL)
	{
		ret = S34GRA_E_MEMORYALLOCATION_FAILED;
		BAIL();
	}

	pSrc = pData;
	for (line=0; line < height; line++)
	{
		// copy scanline values from pSrc which is a BYTE array
		// into pScanLine which is a int array.
		// this is done because pScanLine may recieve negative values
		// which could not be stored in a BYTE array
		//
		for (int y=0; y < width; y++)
			pScanLine[y] = (int)pSrc[y];

		// break scanline into segments based on transparent areas
		// if transparentcolorindex is -1, then we're not encoding this 
		// image based on transparency information
		//
		if (TransparentColorIndex != -1)
		{	// perform transparancy encoding
			for (int i=0; i < width; i++)
			{
				pColorTableEntry = (RGBTRIPLE*)&m_pColorTable[(BYTE)pScanLine[i]];
				if (pColorTableEntry->rgbtBlue == TransparencyColorTableEntry.rgbtBlue &&
				    pColorTableEntry->rgbtGreen	== TransparencyColorTableEntry.rgbtGreen &&
				    pColorTableEntry->rgbtRed == TransparencyColorTableEntry.rgbtRed)
				{
					pScanLine[i] = 0xffff;
				}
				else
				{
					if (m_bHasMask)
						m_pImageMask->SetPixel(i, line, TRUE);
				}
			}
		}

		// attempt to RLE encode segments within pScanLine
		// by converting areas to negative version of the indexes
		//
		// should be the number of bytes required for a large    
		// or small header.  Avoid encoding RLE segments that    
		// are smaller then the scanline header overhead.        
		// This will yield the opposite of compression!          
		int nTreshold = (m_ImageType == eS34IMG_SMALL) ? 
						sizeof(SMALLSCANLINEHEADER) : 
						sizeof(LARGESCANLINEHEADER);

		// find first pixel which is not 0xffff and make that 
		// the starting index
		nBeginSegment = -1;
		RGBTRIPLE LastColor, CurColor;
		int colorIndex = 0xffff;
		for (int prescan = 0; prescan < width; prescan++)
		{
			if (pScanLine[prescan] != 0xffff)
			{
				colorIndex = pScanLine[prescan];
				break;
			}
		}
		if (colorIndex != 0xffff)
		{
			LastColor.rgbtRed  = m_pColorTable[colorIndex].rgbtRed;
			LastColor.rgbtGreen= m_pColorTable[colorIndex].rgbtGreen;
			LastColor.rgbtBlue = m_pColorTable[colorIndex].rgbtBlue;
			int iLastColorIndex = colorIndex;
			int iMatchingColorCount = 0;

			for (int i = prescan+1; i < width; i++)
			{
				if (pScanLine[i] != 0xffff)
				{
					CurColor.rgbtRed  = m_pColorTable[pScanLine[i]].rgbtRed;
					CurColor.rgbtGreen= m_pColorTable[pScanLine[i]].rgbtGreen;
					CurColor.rgbtBlue = m_pColorTable[pScanLine[i]].rgbtBlue;

					if (i!=(width-1) &&
						(LastColor.rgbtRed == CurColor.rgbtRed &&
						 LastColor.rgbtGreen == CurColor.rgbtGreen &&
						 LastColor.rgbtBlue == CurColor.rgbtBlue))
					{
						if (nBeginSegment == -1)
							nBeginSegment = i-1;
						iMatchingColorCount++;
					}
					else
					{	// check if last pixel matches
						if (i == (width-1) &&
							LastColor.rgbtRed == CurColor.rgbtRed &&
							LastColor.rgbtGreen == CurColor.rgbtGreen &&
							LastColor.rgbtBlue == CurColor.rgbtBlue)
						{
							iMatchingColorCount++;
						}

						if (iMatchingColorCount > nTreshold)
						{
							for (int kk = nBeginSegment; kk < (nBeginSegment+iMatchingColorCount)+1; kk++)
								pScanLine[kk] = -iLastColorIndex;
						}
						else
						{
							if (pScanLine[i] != 0xffff)
							{
								iMatchingColorCount = 0;
								LastColor.rgbtRed  = m_pColorTable[pScanLine[i]].rgbtRed;
								LastColor.rgbtGreen= m_pColorTable[pScanLine[i]].rgbtGreen;
								LastColor.rgbtBlue = m_pColorTable[pScanLine[i]].rgbtBlue;
								iLastColorIndex = pScanLine[i];
							}
						}
						iMatchingColorCount = 0;
						nBeginSegment = -1;
					}
				}
				else
				{
					if (iMatchingColorCount > nTreshold)
					{
						for (int kk = nBeginSegment; kk < (nBeginSegment+iMatchingColorCount); kk++)
							pScanLine[kk] = -iLastColorIndex;
					}
					iMatchingColorCount = 0;
					nBeginSegment = -1;
				}
			}
		}


		// Encode scanline into one or more ScanLine Segments
		//
		for (int k =0; k<width; k++)
		{
			if (pScanLine[k] == 0xffff)
				continue;

			// if pScanLine value is less than zero, then we're about to encode an RLE segment
			//
			if (pScanLine[k] < 0)
			{
				if (m_ImageType == eS34IMG_SMALL)
				{
					iCount = 0;
					SMALLSCANLINEHEADER hdr;
					hdr.slColorIndex = -pScanLine[k];
					hdr.slSegmentType = eS34IMG_RLE;
					for (int prescan = k; prescan<width; prescan++)
					{
						if (pScanLine[prescan] != pScanLine[k])
							break;
						else
							iCount++;
					}
					hdr.slTotalPixels = iCount;
					hdr.slXOffset = k;
					hdr.slYOffset = line;

					if ((int)(m_iDataCount + sizeof(SMALLSCANLINEHEADER)) > iDataSize)
					{
						if (IncreaseBufferSize(m_pData, iDataSize, iDataSize*2)==TRUE)
						{
							iDataSize *= 2;
						}
						else
						{
							ret = S34GRA_E_MEMORYALLOCATION_FAILED;
							BAIL();
						}
					}
					memcpy(&m_pData[m_iDataCount], &hdr, sizeof(SMALLSCANLINEHEADER));
					m_iDataCount += sizeof(SMALLSCANLINEHEADER);
					iTotalSegments++;

					k = prescan-1;
					continue;
				}

				if (m_ImageType == eS34IMG_LARGE)
				{
					iCount = 0;
					LARGESCANLINEHEADER hdr;
					hdr.slColorIndex = -pScanLine[k];
					hdr.slSegmentType = eS34IMG_RLE;
					for (int prescan = k; prescan<width; prescan++)
					{
						if (pScanLine[prescan] != pScanLine[k])
							break;
						else
							iCount++;
					}
					hdr.slTotalPixels = iCount;
					hdr.slXOffset = k;
					hdr.slYOffset = line;

					if ((int)(m_iDataCount + sizeof(LARGESCANLINEHEADER)) >  iDataSize)
					{
						if (IncreaseBufferSize(m_pData, iDataSize, iDataSize*2)==TRUE)
						{
							iDataSize *= 2;
						}
						else
						{
							ret = S34GRA_E_MEMORYALLOCATION_FAILED;
							BAIL();
						}
					}
					memcpy(&m_pData[m_iDataCount], &hdr, sizeof(LARGESCANLINEHEADER));
					m_iDataCount += sizeof(LARGESCANLINEHEADER);
					iTotalSegments++;

					k = prescan-1;
					continue;
				}
			}
			
			// if pScanline value is greater or equal to zero, then we're about to encode a RAW segment
			//
			if (pScanLine[k] >= 0)
			{
				if (m_ImageType == eS34IMG_SMALL)
				{
					iCount = 0;
					SMALLSCANLINEHEADER hdr;
					hdr.slColorIndex = -1; // not used for RAW segments
					hdr.slSegmentType = eS34IMG_RAW;
					for (int prescan = k; prescan<width; prescan++)
					{
						if (pScanLine[prescan] == 0xffff || 
							pScanLine[prescan] < 0)
						{
							break;
						}
						else
							iCount++;
					}
					hdr.slTotalPixels = iCount;
					hdr.slXOffset = k;
					hdr.slYOffset = line;

					if ((int)(m_iDataCount + sizeof(SMALLSCANLINEHEADER) + iCount) >  iDataSize)
					{
						if (IncreaseBufferSize(m_pData, iDataSize, iDataSize*2)==TRUE)
						{
							iDataSize *= 2;
						}
						else
						{
							// report memory allocation error
						}
					}
					memcpy(&m_pData[m_iDataCount], &hdr, sizeof(SMALLSCANLINEHEADER));
					m_iDataCount += sizeof(SMALLSCANLINEHEADER);
					iTotalSegments++;

					// Raw segments are followed by hdr.slTotalPixels worth of real pixel data!
					// that's why we say that a raw segment contains raw data!
					//
					for (int z = 0; z<iCount; z++)
					{
						m_pData[m_iDataCount + z] = pScanLine[k+z];
					}
					m_iDataCount += iCount;
					k = prescan-1;
					continue;
				}
				if (m_ImageType == eS34IMG_LARGE)
				{
					iCount = 0;
					LARGESCANLINEHEADER hdr;
					hdr.slColorIndex = -1; // not used for RAW segments
					hdr.slSegmentType = eS34IMG_RAW;
					for (int prescan = k; prescan<width; prescan++)
					{
						if (pScanLine[prescan] == 0xffff || 
							pScanLine[prescan] < 0)
						{
							break;
						}
						else
							iCount++;
					}
					hdr.slTotalPixels = iCount;
					hdr.slXOffset = k;
					hdr.slYOffset = line;

					if ((int)(m_iDataCount + sizeof(LARGESCANLINEHEADER) + iCount) >  iDataSize)
					{
						if (IncreaseBufferSize(m_pData, iDataSize, iDataSize*2)==TRUE)
						{
							iDataSize *= 2;
						}
						else
						{
							// report memory allocation error
						}
					}
					memcpy(&m_pData[m_iDataCount], &hdr, sizeof(LARGESCANLINEHEADER));
					m_iDataCount += sizeof(LARGESCANLINEHEADER);
					iTotalSegments++;

					// Raw segments are followed by hdr.slTotalPixels worth of real pixel data!
					// that's why we say that a raw segment contains raw data!
					//
					for (int z = 0; z<iCount; z++)
					{
						m_pData[m_iDataCount + z] = pScanLine[k+z];
					}
					m_iDataCount += iCount;
					k = prescan-1;
				}
			}
		}
  
		// Move to next line in image
		//
		pSrc += width;
	}

	// update Total Segments
	//
	if (m_ImageType == eS34IMG_SMALL)
	{
		m_sih.ihRecordLength = m_iDataCount;
		m_sih.ihTotalSegments = iTotalSegments;
	}
	else
	{
		m_lih.ihRecordLength = m_iDataCount;
		m_lih.ihTotalSegments = iTotalSegments;
	}

	delete []pScanLine;
	return S34GRA_E_NOERROR;

exit_with_error:
	delete []pScanLine;
	delete []m_pData;
	m_pData = NULL;
	return ret;
}

int CS34Image::SetExternalColorTable(RGBTRIPLE *pColorTable, BOOL bBitMode)
{
	m_bBitModePalette = bBitMode;
	m_bHasExternalPalette = TRUE;
	if (m_pColorTable)
		delete []m_pColorTable;
	m_pColorTable = pColorTable;
	return S34GRA_E_NOERROR;
}


BOOL CS34Image::IncreaseBufferSize(BYTE *pBuffer, int iCurrentSize, int iNewSize)
{
	BYTE *pTemp;
	pTemp = new BYTE[iNewSize];
	if (pTemp == NULL)
		return FALSE;
	memcpy(pTemp, pBuffer, iCurrentSize);
	delete []pBuffer;
	pBuffer = pTemp;
	return TRUE;
}

int CS34Image::Unpack(BYTE *pData, int iSize)
{
	int ret = S34GRA_E_NOERROR;

	if (m_pData==NULL || (m_ImageType!=eS34IMG_SMALL && m_ImageType!=eS34IMG_LARGE))
		return S34GRA_E_IMAGE_NOT_CREATED_NOR_LOADED;
	if (pData==NULL)
		return S34GRA_E_INVALIDPARAM;

	// validate iSize by checking width and height against it
	//
	if (m_ImageType == eS34IMG_SMALL)
	{
		if ((m_sih.ihWidth * m_sih.ihHeight) != iSize)
			return S34GRA_E_INVALIDSIZEPARAM;
	}
	else
	{
		if ((m_lih.ihWidth * m_lih.ihHeight) != iSize)
			return S34GRA_E_INVALIDSIZEPARAM;
	}

	int width,j; 
	BYTE *pDst = pData;
	BYTE *pSrc = m_pData;
	BYTE *pPixelAddress;
	int i = 0;
	int iTotalSegments;

	if (m_ImageType == eS34IMG_SMALL)
	{
		SMALLSCANLINEHEADER ssh;
		iTotalSegments = m_sih.ihTotalSegments;
		width = m_sih.ihWidth;
		while (i < iTotalSegments)
		{
			memcpy(&ssh, pSrc, sizeof(SMALLSCANLINEHEADER));
			pSrc += sizeof(SMALLSCANLINEHEADER);
			pPixelAddress = (BYTE*)(pDst + (ssh.slYOffset * width) + ssh.slXOffset);
			if (ssh.slSegmentType != eS34IMG_RLE &&
				ssh.slSegmentType != eS34IMG_RAW)
			{
				ret = S34GRA_E_INVALIDIMAGEDATA;
				BAIL();
			}

			if (ssh.slSegmentType == eS34IMG_RLE)
			{
				for (j=0; j<ssh.slTotalPixels; j++)
					*(pPixelAddress+j) = ssh.slColorIndex;	
			}
			if (ssh.slSegmentType == eS34IMG_RAW)
			{
				for (j=0; j<ssh.slTotalPixels; j++)
					*(pPixelAddress+j) = *(pSrc+j);
				pSrc += j;
			}
			i++;
		}
	}
	else
	{
		LARGESCANLINEHEADER lsh;
		iTotalSegments = m_lih.ihTotalSegments;
		width = m_lih.ihWidth;
		while (i < iTotalSegments)
		{
			memcpy(&lsh, pSrc, sizeof(LARGESCANLINEHEADER));
			pSrc += sizeof(LARGESCANLINEHEADER);
			pPixelAddress = (BYTE*)(pDst + (lsh.slYOffset * width) + lsh.slXOffset);
			if (lsh.slSegmentType != eS34IMG_RLE &&
				lsh.slSegmentType != eS34IMG_RAW)
			{
				ret = S34GRA_E_INVALIDIMAGEDATA;
				BAIL();
			}
			if (lsh.slSegmentType == eS34IMG_RLE)
			{
				for (j=0; j<lsh.slTotalPixels; j++)
					*(pPixelAddress+j) = lsh.slColorIndex;	
			}
			if (lsh.slSegmentType == eS34IMG_RAW)
			{
				for (j=0; j<lsh.slTotalPixels; j++)
					*(pPixelAddress+j) = *(pSrc+j);
				pSrc += j;
			}
			i++;
		}
	}
	return S34GRA_E_NOERROR;

exit_with_error:
	*pData = NULL;
	return ret;
}

int CS34Image::Load(char *pFileName)
{
	int ret = S34GRA_E_NOERROR;

	if (pFileName == NULL)
		return S34GRA_E_INVALIDPARAM;

	// open file
	//
	HANDLE hFile;
	hFile = ::CreateFile((const char*)pFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return S34GRA_E_FILEOPEN_FAILED;
	}

	ret = Load((HANDLE)hFile);
	if (ret!=S34GRA_E_NOERROR)
		BAIL();

	::CloseHandle((HANDLE)hFile);
	return S34GRA_E_NOERROR;
exit_with_error:
	delete []m_pData;
	m_pData = NULL;
	::CloseHandle((HANDLE)hFile);
	return ret;
}

int CS34Image::Load(HANDLE hFile)
{
	DWORD dwFileSize;
	DWORD nRead;
	int ret;

	m_iDataCount = 0;
	if (m_pData)
		delete []m_pData;

	// read first byte to determine if this is a small
	// or large image.
	if (!::ReadFile((HANDLE)hFile, &m_ImageType, sizeof(BYTE), &nRead, NULL))
	{
		ret = S34GRA_E_FILEREAD_FAILED;
		BAIL();
	}

	::SetFilePointer((HANDLE)hFile, -1, NULL, FILE_CURRENT);
	
	if (m_ImageType != eS34IMG_SMALL &&
		m_ImageType != eS34IMG_LARGE)
	{
		// report error.
	}

	if (m_ImageType == eS34IMG_SMALL)
	{
		if (!::ReadFile((HANDLE)hFile, &m_sih, sizeof(SMALLIMAGEHEADER), &nRead, NULL))
		{
			ret = S34GRA_E_FILEREAD_FAILED;
			BAIL();
		}
		m_iTotalColors = m_sih.ihTotalPaletteEntries+1;
		m_width = m_sih.ihWidth;
		m_height = m_sih.ihHeight;
		m_bHasMask = m_sih.ihHasMask;
		m_bHasExternalPalette = m_sih.ihHasExternalPalette;
		dwFileSize = m_sih.ihRecordLength;

	}
	else
	{
		if (!::ReadFile((HANDLE)hFile, &m_lih, sizeof(LARGEIMAGEHEADER), &nRead, NULL))
		{
			ret = S34GRA_E_FILEREAD_FAILED;
			BAIL();
		}
		m_iTotalColors = m_lih.ihTotalPaletteEntries+1;
		m_width = m_lih.ihWidth;
		m_height = m_lih.ihHeight;
		m_bHasMask = m_lih.ihHasMask;
		m_bHasExternalPalette = m_lih.ihHasExternalPalette;
		dwFileSize = m_lih.ihRecordLength;
	}

	if (!m_bHasExternalPalette)
	{
		m_pColorTable = new RGBTRIPLE[256];
		if (m_pColorTable == NULL)
		{
			ret = S34GRA_E_MEMORYALLOCATION_FAILED;
			BAIL();
		}
		if (!::ReadFile((HANDLE)hFile, m_pColorTable, sizeof(RGBTRIPLE)*m_iTotalColors, &nRead, NULL))
		{
			ret = S34GRA_E_FILEREAD_FAILED;
			BAIL();
		}
	}


	// allocate space for image data
	//
	m_pData = new BYTE[dwFileSize];
	if (m_pData==NULL)
	{
		ret = S34GRA_E_MEMORYALLOCATION_FAILED;
		BAIL();
	}
	if (!::ReadFile((HANDLE)hFile, m_pData, dwFileSize, &nRead, NULL))
	{
		ret = S34GRA_E_FILEREAD_FAILED;
	}
	m_iDataCount = dwFileSize;


	// load image mask if one exist
	//
	if (m_bHasMask)
	{
		BYTE *pData=NULL;
		UINT iSize;
		m_pImageMask = new CS34ImageMask();
		if (m_pImageMask == NULL)
			return S34GRA_E_MEMORYALLOCATION_FAILED;
		m_pImageMask->Create(m_width, m_height, FALSE);
		m_pImageMask->GetBuffer(pData, iSize);
		if (!::ReadFile((HANDLE)hFile, pData, iSize, &nRead, NULL))
		{
			ret = S34GRA_E_FILEREAD_FAILED;
		}
	}

	return S34GRA_E_NOERROR;
exit_with_error:
	return ret;
}


int CS34Image::Save(char *pFileName)
{
	int ret = S34GRA_E_NOERROR;

	if (pFileName == NULL)
		return S34GRA_E_INVALIDPARAM;
	if (m_pData==NULL || (m_ImageType!=eS34IMG_SMALL && m_ImageType!=eS34IMG_LARGE))
		return S34GRA_E_IMAGE_NOT_CREATED_NOR_LOADED;

	// create file
	//
	HANDLE hFile;
	hFile = ::CreateFile((const char*)pFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return S34GRA_E_FILECREATE_FAILED;
	}

	ret = Save((HANDLE)hFile);
	if (ret!=S34GRA_E_NOERROR)
		BAIL();

	::CloseHandle((HANDLE)hFile);
	return S34GRA_E_NOERROR;
exit_with_error:
	::CloseHandle((HANDLE)hFile);
	return ret;
}


int CS34Image::Save(HANDLE hFile)
{
	DWORD nWritten;
	int ret;

	// write image header
	//
	if (m_ImageType == eS34IMG_SMALL)
	{
		if (!::WriteFile((HANDLE)hFile, &m_sih, sizeof(SMALLIMAGEHEADER), &nWritten, NULL))
		{
			ret = S34GRA_E_FILEWRITE_FAILED;
			BAIL();
		}
	}
	else
	{
		if (!::WriteFile((HANDLE)hFile, &m_lih, sizeof(LARGEIMAGEHEADER), &nWritten, NULL))
		{
			ret = S34GRA_E_FILEWRITE_FAILED;
			BAIL();
		}
	}

	// write color palette
	//
	if (!m_bHasExternalPalette)
	{
		if (!::WriteFile((HANDLE)hFile, m_pColorTable, sizeof(RGBTRIPLE)*m_iTotalColors, &nWritten, NULL))
		{
			ret = S34GRA_E_FILEWRITE_FAILED;
			BAIL();
		}
	}

	// write data
	//
	if (!::WriteFile((HANDLE)hFile, m_pData, m_iDataCount, &nWritten, NULL))
	{
		ret = S34GRA_E_FILEWRITE_FAILED;
		BAIL();
	}


	// write mask if one exist for this image
	//
	if (m_bHasMask)
	{
		BYTE *pData=NULL;
		UINT iSize;
		m_pImageMask->GetBuffer(pData, iSize);
		if (!::WriteFile((HANDLE)hFile, pData, iSize, &nWritten, NULL))
		{
			ret = S34GRA_E_FILEWRITE_FAILED;
			BAIL();
		}
	}

	return S34GRA_E_NOERROR;
exit_with_error:
	return ret;
}



int CS34Image::GetSize(int *pWidth, int *pHeight)
{
	if (m_pData==NULL || (m_ImageType!=eS34IMG_SMALL && m_ImageType!=eS34IMG_LARGE))
		return S34GRA_E_IMAGE_NOT_CREATED_NOR_LOADED;

	*pWidth = m_width;
	*pHeight = m_height;

	return S34GRA_E_NOERROR;
}


int CS34Image::Clone(CS34Image *pSrcImg)
{
	m_iDataCount = 0;
	if (m_pData)
		delete []m_pData;

	m_iDataCount = pSrcImg->m_iDataCount;
	m_ImageType = pSrcImg->m_ImageType;
	m_bHasMask = pSrcImg->m_bHasMask;
	m_bHasExternalPalette = pSrcImg->m_bHasExternalPalette;


	if (m_ImageType == eS34IMG_LARGE)
	{
		memcpy(&m_lih, &pSrcImg->m_lih, sizeof(LARGEIMAGEHEADER));
	}
	else
	{
		memcpy(&m_sih, &pSrcImg->m_sih, sizeof(SMALLIMAGEHEADER));
	}

	m_width = pSrcImg->m_width;
	m_height = pSrcImg->m_height;
	m_pData = new BYTE[m_iDataCount];
	memcpy(m_pData, pSrcImg->m_pData, m_iDataCount);

	if (m_bHasExternalPalette)
		m_pColorTable = pSrcImg->m_pColorTable;
	else
		memcpy(m_pColorTable, pSrcImg->m_pColorTable, sizeof(RGBTRIPLE)*256);
	m_iTotalColors = pSrcImg->m_iTotalColors;


	if (m_bHasMask)
	{
		BYTE *pDstData=NULL;
		BYTE *pSrcData=NULL;
		UINT iSize;
		m_pImageMask = new CS34ImageMask();
		if (m_pImageMask == NULL)
			return S34GRA_E_MEMORYALLOCATION_FAILED;
		m_pImageMask->Create(m_width, m_height, FALSE);
		m_pImageMask->GetBuffer(pDstData, iSize);
		pSrcImg->m_pImageMask->GetBuffer(pSrcData, iSize);
		memcpy(pDstData, pSrcData, iSize);		
	}

	return S34GRA_E_NOERROR;
}

int CS34Image::Render(BYTE *pDstBuffer,	int iPitch,
					 int iDstX, int iDstY, 
					 int iImageOffsetX, int iImageOffsetY, 
					 int iImageWidth, int iImageHeight)
{
    int ret = S34GRA_E_NOERROR;

	if (m_pData==NULL || (m_ImageType!=eS34IMG_SMALL && m_ImageType!=eS34IMG_LARGE))
		return S34GRA_E_IMAGE_NOT_CREATED_NOR_LOADED;

	if (iImageWidth == 0 && iImageHeight==0)
	{
		iImageWidth = m_width;
		iImageHeight = m_height;
	}

	int j;
	BYTE *pSrc = m_pData;
	WORD *pDst = (WORD*)pDstBuffer;
	WORD *pPixelAddress;
	WORD *pPixel;
	int i = 0;
	int iTotalSegments;
	int index;
	int ImgX,ImgY;


	if (m_ImageType == eS34IMG_SMALL)
	{
		SMALLSCANLINEHEADER ssh;
		iTotalSegments = m_sih.ihTotalSegments;

		while (i < iTotalSegments)
		{
			memcpy(&ssh, pSrc, sizeof(SMALLSCANLINEHEADER));
			pSrc += sizeof(SMALLSCANLINEHEADER);

			ImgY = ssh.slYOffset;
			if (ImgY < iImageOffsetY ||	(ImgY >= (iImageHeight+iImageOffsetY)))
			{
				i++;
				if (ssh.slSegmentType == eS34IMG_RAW)
					pSrc += ssh.slTotalPixels;
				continue;
			}

			ImgX = ssh.slXOffset;
			ImgY -= iImageOffsetY;
			
			pPixelAddress = (WORD*)(pDst + ((iDstY+ImgY)* iPitch>>1) + iDstX);

#ifdef _DEBUG
			if (ssh.slSegmentType != eS34IMG_RLE &&
				ssh.slSegmentType != eS34IMG_RAW)
			{
				ret = S34GRA_E_INVALIDIMAGEDATA;
				BAIL();
			}
#endif //_DEBUG

			if (ImgX > iImageOffsetX)
				pPixelAddress += (ImgX - iImageOffsetX);

			if (ssh.slSegmentType == eS34IMG_RLE)
			{
				index = ssh.slColorIndex;
				pPixel = (WORD*)&m_pColorTable[index].rgbtBlue;
				for (j=0; j<ssh.slTotalPixels; j++,ImgX++)
				{
					if (ImgX < iImageOffsetX || ImgX >= (iImageWidth+iImageOffsetX))
						continue;
					*pPixelAddress++ = *pPixel;
				}
			}

			if (ssh.slSegmentType == eS34IMG_RAW)
			{
				for (j=0; j<ssh.slTotalPixels; j++,ImgX++)
				{
					if (ImgX < iImageOffsetX || ImgX >= (iImageWidth+iImageOffsetX))
						continue;
					*pPixelAddress++ = *((WORD*)&m_pColorTable[*(pSrc+j)].rgbtBlue);
				}
				pSrc += j;
			}
			i++;
		}
	}
	if (m_ImageType == eS34IMG_LARGE)
	{
		LARGESCANLINEHEADER lsh;
		iTotalSegments = m_lih.ihTotalSegments;

		while (i < iTotalSegments)
		{
			memcpy(&lsh, pSrc, sizeof(LARGESCANLINEHEADER));
			pSrc += sizeof(LARGESCANLINEHEADER);

			ImgY = lsh.slYOffset;
			if (ImgY < iImageOffsetY ||	(ImgY >= (iImageHeight+iImageOffsetY)))
			{
				i++;
				if (lsh.slSegmentType == eS34IMG_RAW)
					pSrc += lsh.slTotalPixels;
				continue;
			}

			ImgX = lsh.slXOffset;
			ImgY -= iImageOffsetY;
			
			pPixelAddress = (WORD*)(pDst + ((iDstY+ImgY)* iPitch>>1) + iDstX);

#ifdef _DEBUG
			if (lsh.slSegmentType != eS34IMG_RLE &&
				lsh.slSegmentType != eS34IMG_RAW)
			{
				ret = S34GRA_E_INVALIDIMAGEDATA;
				BAIL();
			}
#endif //_DEBUG

			if (ImgX > iImageOffsetX)
				pPixelAddress += (ImgX - iImageOffsetX);

			if (lsh.slSegmentType == eS34IMG_RLE)
			{
				index = lsh.slColorIndex;
				pPixel = (WORD*)&m_pColorTable[index].rgbtBlue;
				for (j=0; j<lsh.slTotalPixels; j++,ImgX++)
				{
					if (ImgX < iImageOffsetX || ImgX >= (iImageWidth+iImageOffsetX))
						continue;
					*pPixelAddress++ = *pPixel;
				}
			}
			if (lsh.slSegmentType == eS34IMG_RAW)
			{
				for (j=0; j<lsh.slTotalPixels; j++,ImgX++)
				{
					if (ImgX < iImageOffsetX || ImgX >= (iImageWidth+iImageOffsetX))
						continue;
					*pPixelAddress++ = *((WORD*)&m_pColorTable[*(pSrc+j)].rgbtBlue);
				}
				pSrc += j;
			}
			i++;
		}
	}

	return S34GRA_E_NOERROR;
exit_with_error:
	return ret;
}

int CS34Image::Render256(BYTE *pDstBuffer,	int iPitch,
					 int iDstX, int iDstY, 
					 int iImageOffsetX, int iImageOffsetY, 
					 int iImageWidth, int iImageHeight)
{
    int ret = S34GRA_E_NOERROR;

	if (m_pData==NULL || (m_ImageType!=eS34IMG_SMALL && m_ImageType!=eS34IMG_LARGE))
		return S34GRA_E_IMAGE_NOT_CREATED_NOR_LOADED;

	if (iImageWidth == 0 && iImageHeight==0)
	{
		iImageWidth = m_width;
		iImageHeight = m_height;
	}

	int j;
	BYTE *pSrc = m_pData;
	BYTE *pDst = pDstBuffer;
	BYTE *pPixelAddress;
	int i = 0;
	int iTotalSegments;
	int index;
	int ImgX,ImgY;

	if (m_ImageType == eS34IMG_SMALL)
	{
		SMALLSCANLINEHEADER ssh;
		iTotalSegments = m_sih.ihTotalSegments;

		while (i < iTotalSegments)
		{
			memcpy(&ssh, pSrc, sizeof(SMALLSCANLINEHEADER));
			pSrc += sizeof(SMALLSCANLINEHEADER);

			ImgY = ssh.slYOffset;
			if (ImgY < iImageOffsetY ||	(ImgY >= (iImageHeight+iImageOffsetY)))
			{
				i++;
				if (ssh.slSegmentType == eS34IMG_RAW)
					pSrc += ssh.slTotalPixels;
				continue;
			}

			ImgX = ssh.slXOffset;
			ImgY -= iImageOffsetY;
			
			pPixelAddress = (BYTE*)(pDst + ((iDstY+ImgY) * iPitch) + iDstX);
#ifdef _DEBUG
			if (ssh.slSegmentType != eS34IMG_RLE &&
				ssh.slSegmentType != eS34IMG_RAW)
			{
				ret = S34GRA_E_INVALIDIMAGEDATA;
				BAIL();
			}
#endif //_DEBUG

			if (ImgX > iImageOffsetX)
				pPixelAddress += (ImgX - iImageOffsetX);

			if (ssh.slSegmentType == eS34IMG_RLE)
			{
				index = ssh.slColorIndex;
				for (j=0; j<ssh.slTotalPixels; j++,ImgX++)
				{
					if (ImgX < iImageOffsetX || ImgX >= (iImageWidth+iImageOffsetX))
						continue;
					*pPixelAddress++ = index;
				}
			}

			if (ssh.slSegmentType == eS34IMG_RAW)
			{
				for (j=0; j<ssh.slTotalPixels; j++,ImgX++)
				{
					if (ImgX < iImageOffsetX || ImgX >= (iImageWidth+iImageOffsetX))
						continue;
					*pPixelAddress++ = *(pSrc+j);
				}
				pSrc += j;
			}
			i++;
		}
	}
	if (m_ImageType == eS34IMG_LARGE)
	{
		LARGESCANLINEHEADER lsh;
		iTotalSegments = m_lih.ihTotalSegments;

		while (i < iTotalSegments)
		{
			memcpy(&lsh, pSrc, sizeof(LARGESCANLINEHEADER));
			pSrc += sizeof(LARGESCANLINEHEADER);

			ImgY = lsh.slYOffset;
			if (ImgY < iImageOffsetY ||	(ImgY >= (iImageHeight+iImageOffsetY)))
			{
				i++;
				if (lsh.slSegmentType == eS34IMG_RAW)
					pSrc += lsh.slTotalPixels;
				continue;
			}

			ImgX = lsh.slXOffset;
			ImgY -= iImageOffsetY;
			
			pPixelAddress = (BYTE*)(pDst + ((iDstY+ImgY)* iPitch) + iDstX);

#ifdef _DEBUG
			if (lsh.slSegmentType != eS34IMG_RLE &&
				lsh.slSegmentType != eS34IMG_RAW)
			{
				ret = S34GRA_E_INVALIDIMAGEDATA;
				BAIL();
			}
#endif //_DEBUG

			if (ImgX > iImageOffsetX)
				pPixelAddress += (ImgX - iImageOffsetX);

			if (lsh.slSegmentType == eS34IMG_RLE)
			{
				index = lsh.slColorIndex;
				for (j=0; j<lsh.slTotalPixels; j++,ImgX++)
				{
					if (ImgX < iImageOffsetX || ImgX >= (iImageWidth+iImageOffsetX))
						continue;
					*pPixelAddress++ = index;
				}
			}
			if (lsh.slSegmentType == eS34IMG_RAW)
			{
				for (j=0; j<lsh.slTotalPixels; j++,ImgX++)
				{
					if (ImgX < iImageOffsetX || ImgX >= (iImageWidth+iImageOffsetX))
						continue;
					*pPixelAddress++ = *(pSrc+j);
				}
				pSrc += j;
			}
			i++;
		}
	}

	return S34GRA_E_NOERROR;
exit_with_error:
	return ret;
}

BOOL CS34Image::IsPointInImage(int x, int y)
{
	if (!m_bHasMask)
		return FALSE;
	return m_pImageMask->GetPixel(x,y);
}

//void* CS34Image::operator new(size_t bytes)
//{
//	void* p = malloc(bytes);
//    return p;
//} 
//
//void CS34Image::operator delete(void*p)
//{
//    free(p);
//}
//
//
