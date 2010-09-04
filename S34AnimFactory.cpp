// AnimFactory.cpp: implementation of the CAnimFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S34AnimFactory.h"
#include "S34Anim.h"
#include "S34Image.h"
#include "S34DblList.h"

CS34AnimFactory *CS34AnimFactory::m_pInstance = NULL;

class CS34AnimFactoryEntry
{
public:
	char *pszFilename;
	int iRefCount;
	CS34DblList ImageArray;
	RGBTRIPLE *pColorTable;
};

static CS34DblList Anims;

void CS34AnimFactory::Create()
{
	m_pInstance = new CS34AnimFactory();
}

void CS34AnimFactory::Destroy()
{
	delete m_pInstance;
	m_pInstance = NULL;
}

CS34AnimFactory *CS34AnimFactory::GetInstance()
{
	return m_pInstance;
}

CS34AnimFactory::CS34AnimFactory()
{
	m_iColorDepth = -1;
	m_pTempBuffer = NULL;
	m_iSrcTransIndex = 65535;
}

CS34AnimFactory::~CS34AnimFactory()
{
	CS34Image *pImage;
	CS34AnimFactoryEntry *pEntry;

	if (Anims.MoveToHead())
	{
		do 
		{
			pEntry = (CS34AnimFactoryEntry*)Anims.GetData();
			if (pEntry->ImageArray.MoveToHead())
			{			
				do 
				{
					pImage = (CS34Image*)pEntry->ImageArray.GetData();
					delete pImage;
				} while (pEntry->ImageArray.MoveNext());
			}
			delete []pEntry->pszFilename;
			delete []pEntry->pColorTable;
			delete pEntry;
		} while (Anims.MoveNext());
	}

	delete []m_pTempBuffer;
	m_pTempBuffer = NULL;
}

int CS34AnimFactory::CreateAnim(char *pAnimFileName, CS34Anim &AnimObject)
{
	int i = 0;
	int iRet;
	CS34Image *pImage;
	CS34AnimFactoryEntry *pEntry;
	CS34Dither dither;
	int width, height;

	if (m_iColorDepth == -1)
		return S34ANM_E_COLORDEPTH_NOTSET;

	// Check Anim Cache to determine if animation frames are already loaded!
	//
	if (Anims.MoveToHead())
	{
		do 
		{
			pEntry = (CS34AnimFactoryEntry*)Anims.GetData();
			if (strcmp(pEntry->pszFilename, pAnimFileName)==0)
			{
				if (pEntry->ImageArray.MoveToHead())
				{			
					do 
					{
						pImage = (CS34Image*)pEntry->ImageArray.GetData();
						AnimObject.AddFrame(pImage);
					} while (pEntry->ImageArray.MoveNext());
					pEntry->iRefCount++;
					return S34ANM_E_NOERROR;
				}
			}
		} while (Anims.MoveNext());
	}


	// Ok, this is a new animation... load it up!
	//
	pEntry = new CS34AnimFactoryEntry();
	if (pEntry==NULL)
		return S34ANM_E_MEMORYALLOCATION_FAILED;

	int iFileNameLength = strlen(pAnimFileName);
	pEntry->pszFilename = new char[iFileNameLength+1];
	if (pEntry->pszFilename == NULL)
	{
		delete pEntry;
		return S34ANM_E_MEMORYALLOCATION_FAILED;
	}
	strcpy(pEntry->pszFilename, pAnimFileName);


	// Attempt to open file
	//
	HANDLE hFile;
	hFile = ::CreateFile(pEntry->pszFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		delete []pEntry->pszFilename;
		delete pEntry;
		return S34ANM_E_FILEOPEN_FAILED;
	}


	// Extract file header
	//
	ANIMFILEHEADER animHeader;
	DWORD nRead;
	if (!::ReadFile((HANDLE)hFile, &animHeader, sizeof(ANIMFILEHEADER), &nRead, NULL))
	{
		delete []pEntry->pszFilename;
		delete pEntry;
		::CloseHandle((HANDLE)hFile);
		return S34ANM_E_FILEREAD_FAILED;
	}

	width = animHeader.afhWidth;
	height = animHeader.afhHeight;
	int iImageSize = width*height;

//	animHeader.afhWidth = width;
//	animHeader.afhHeight = height;
//	animHeader.afhTotalFrames = iCount;
//	animHeader.afhFrameRate = iFrameRate;
//	animHeader.afhTransparentIndex = iTransparentIndex;
	pEntry->iRefCount = 1;

	// Extract color table
	//
	pEntry->pColorTable = new RGBTRIPLE[256];
	if (pEntry->pColorTable==NULL)
	{
		delete []pEntry->pszFilename;
		delete pEntry;
		::CloseHandle((HANDLE)hFile);
		return S34ANM_E_MEMORYALLOCATION_FAILED;
	}
	if (!::ReadFile((HANDLE)hFile, pEntry->pColorTable, sizeof(RGBTRIPLE)*256, &nRead, NULL))
	{
		delete []pEntry->pszFilename;
		delete []pEntry->pColorTable;
		delete pEntry;
		::CloseHandle((HANDLE)hFile);
		return S34ANM_E_FILEREAD_FAILED;
	}


	// process colortable if we're dealing with a video
	// mode that is non palettized.
	//
	if (m_iColorDepth == COLORDEPTH_555 || m_iColorDepth == COLORDEPTH_565)
	{
		WORD wColor;
		int j=0;
		if (m_iColorDepth == COLORDEPTH_555)
		{
			for (int j=0; j<256; j++)
			{
				wColor =(WORD) (((pEntry->pColorTable[j].rgbtRed & 0xF8) << 7) | 
								((pEntry->pColorTable[j].rgbtGreen & 0xF8) << 2) | 
								((pEntry->pColorTable[j].rgbtBlue >> 3)));
				pEntry->pColorTable[j].rgbtBlue = LOBYTE(wColor); 
				pEntry->pColorTable[j].rgbtGreen = HIBYTE(wColor);
			}
		}
		if (m_iColorDepth == COLORDEPTH_565)
		{
			for (int j=0; j<256; j++)
			{
				wColor =(WORD) (((pEntry->pColorTable[j].rgbtRed & 0xF8) << 8) | 
								((pEntry->pColorTable[j].rgbtGreen & 0xFC) << 3) | 
								((pEntry->pColorTable[j].rgbtBlue & 0xF8) >> 3));
				pEntry->pColorTable[j].rgbtBlue = LOBYTE(wColor);
				pEntry->pColorTable[j].rgbtGreen =  HIBYTE(wColor);
			}
		}
	}

	// in 256 color more we set up the destination palette to
	// prepare to dither all images below to this common palette
	//
	if (m_iColorDepth == COLORDEPTH_256)
	{
		BYTE red, green, blue;
		for (int j=0; j<256; j++)
		{
			red = pEntry->pColorTable[j].rgbtRed;
			green = pEntry->pColorTable[j].rgbtGreen;
			blue = pEntry->pColorTable[j].rgbtBlue;

			m_SrcPalette.PutIndexData(j, red, green, blue);
			if (red == 255 && green == 0 && blue == 255)
				m_iSrcTransIndex = j;
			
			m_pPalette->GetIndexData(j, 
									&pEntry->pColorTable[j].rgbtRed, 
									&pEntry->pColorTable[j].rgbtGreen, 
									&pEntry->pColorTable[j].rgbtBlue);
		}
		m_SrcPalette.SetTotalColors(256);

		if (animHeader.afhTransparentIndex == 65535)
		{
			dither.SetPalette(m_pPalette);
		}
		else
		{
			dither.SetPaletteTrans(m_pPalette, 253);
		}
	}


	// if we're going to render to an 8bit mode
	// create a tempary work buffer
	//
	if (m_iColorDepth == COLORDEPTH_256)
	{
		// create buffer
		if (m_pTempBuffer)
			delete []m_pTempBuffer;

		m_pTempBuffer = new BYTE[iImageSize];
		if (m_pTempBuffer == NULL)
		{
			delete []pEntry->pszFilename;
			delete []pEntry->pColorTable;
			delete pEntry;
			::CloseHandle((HANDLE)hFile);
			return S34ANM_E_MEMORYALLOCATION_FAILED;
		}
	}


	// Read data file to extract CS34Image data
	//
	i = 0;
	while (i < animHeader.afhTotalFrames)
	{
		// Set external color palette for new image
		//
		pImage = new CS34Image();
		if (pImage == NULL)
		{
			delete []m_pTempBuffer;
			m_pTempBuffer = NULL;
			delete []pEntry->pszFilename;
			delete []pEntry->pColorTable;
			delete pEntry;
			::CloseHandle((HANDLE)hFile);
			return S34ANM_E_MEMORYALLOCATION_FAILED;
		}


		if (m_iColorDepth == COLORDEPTH_555 || m_iColorDepth == COLORDEPTH_565)
			pImage->SetExternalColorTable(pEntry->pColorTable, TRUE);
		else
			pImage->SetExternalColorTable(pEntry->pColorTable, FALSE);

		iRet = pImage->Load(hFile);
		if (iRet != S34GRA_E_NOERROR)
		{
			switch (iRet)
			{
				case S34GRA_E_FILEREAD_FAILED:
					iRet = S34ANM_E_FILEREAD_FAILED;
					break;
				case S34GRA_E_MEMORYALLOCATION_FAILED:
					iRet = S34ANM_E_MEMORYALLOCATION_FAILED;
					break;
			};

			delete []pEntry->pszFilename;
			delete []pEntry->pColorTable;
			delete pEntry;
			::CloseHandle((HANDLE)hFile);
			return iRet;
		}


		if (m_iColorDepth == COLORDEPTH_256)
		{
			// unpack data to buffer
			//
			if (animHeader.afhTransparentIndex != 65535)
				memset(m_pTempBuffer,m_iSrcTransIndex,iImageSize);
			pImage->Unpack(m_pTempBuffer, iImageSize);

			// dither buffer
			//
			if (animHeader.afhTransparentIndex == 65535)
				dither.Dither(m_pTempBuffer, width, height, &m_SrcPalette);
			else
				dither.DitherTrans(m_pTempBuffer, width, height, &m_SrcPalette, m_iSrcTransIndex);

			// save buffer back into orginal image
			//
			if (animHeader.afhTransparentIndex != 65535)
			{
				pImage->Create(width, height, 
							  253, 
							  TRUE, pImage->HasMask(), 
							  m_pPalette, m_pTempBuffer);
			}
			else
			{
				pImage->Create(width, height, 
							  -1, 
							  TRUE, pImage->HasMask(), 
							  m_pPalette, m_pTempBuffer);
			}
		}

		pEntry->ImageArray.Add(pImage);
		i++;
	}

	// Create our AnimObject
	//
	if (pEntry->ImageArray.MoveToHead())
	{			
		do 
		{
			pImage = (CS34Image*)pEntry->ImageArray.GetData();
			AnimObject.AddFrame(pImage);
		} while (pEntry->ImageArray.MoveNext());
	}

	// Add CS34AnimFactoryEntry into Anims
	//
	Anims.Add(pEntry);

	::CloseHandle((HANDLE)hFile);
	return S34ANM_E_NOERROR;
}


int CS34AnimFactory::SetDestColorDepth(int iColorDepth)
{
	int ret = S34ANM_E_NOERROR;

	switch (iColorDepth)
	{
		case COLORDEPTH_256:
		case COLORDEPTH_555:
		case COLORDEPTH_565:
			m_iColorDepth = iColorDepth;	
			break;
		default:
			ret = S34ANM_E_INVALID_COLORDEPTH;
			break;
	};

	return ret;
}


int CS34AnimFactory::SetDstPalette(CS34Palette *pPalette)
{
	m_pPalette = pPalette;
	return S34ANM_E_NOERROR;
}
