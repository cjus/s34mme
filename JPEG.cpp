// JPEG.cpp: implementation of the CJPEG class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JPEG.h"

#include "S34Palette.h"
#include "S34Bitmap.h"
#include "ImageManipulation.h"

#include <stdio.h>

extern "C"
{
	#include "jpeglib.h"
}


/*
 * <setjmp.h> is used for the optional error recovery mechanism shown in
 * the second part of the example.
 */

#include <setjmp.h>
extern "C" 
{
	struct my_error_mgr 
	{
		struct jpeg_error_mgr pub;	// "public" fields 
		jmp_buf setjmp_buffer;	    // for return to caller
	};

	// error handler, to avoid those pesky exit(0)'s
	typedef struct my_error_mgr * my_error_ptr;

    void my_error_exit(j_common_ptr cinfo);
}


//	for DWORD aligning a buffer
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJPEG::CJPEG()
{
	m_pData = NULL;
	m_bPaletteInUse = FALSE;
	m_width = 0;
	m_height = 0;
}

CJPEG::~CJPEG()
{
	if (m_pData)
		delete []m_pData;
	m_pData = NULL;
}

int CJPEG::Load(char *pFileName, CS34Palette *pPalette)
{
	int ret = S34GRA_E_NOERROR;

	if (pFileName == NULL)
		return S34GRA_E_INVALIDPARAM;

	if (m_pData)
		delete []m_pData;

	if (pPalette == NULL)
	{
		// read the file to RGB buffer 
		ret = ReadJPEGToRGBBuf(pFileName, &m_width, &m_height);
		if (ret != S34GRA_E_NOERROR)
			BAIL();
	}
	else
	{
		m_bPaletteInUse = TRUE;
		for (int j = 0; j < 256; j++)
			pPalette->GetIndexData(j, &m_colormap[0][j], &m_colormap[1][j], &m_colormap[2][j]);

		ret = ReadJPEGToRGBBuf(pFileName, &m_width, &m_height, m_colormap);
		if (ret != S34GRA_E_NOERROR)
			BAIL();
	}

	// do this before DWORD-alignment!!!
	// this works on packed (not DWORD-aligned) buffers
	// swap red and blue for display
	if (!m_bPaletteInUse)
	{
		RGBToBGR(m_pData, m_width, m_height);
		// vertical flip for display
		CImageManipulation::FlipImage(m_pData, m_width, m_height, 24);
	}
	else
	{
		// vertical flip for display
		CImageManipulation::FlipImage(m_pData, m_width, m_height, 8);
	}

	return S34GRA_E_NOERROR;
exit_with_error:
	if (m_pData)
		delete [] m_pData;
	return ret;
}

int CJPEG::SaveToBMPFile(char *pFileName, int quality)
{
	// TODO: Add your implementation code here
	return S34GRA_E_NOERROR;
}



//*********************************************************
//	read a JPEG file and return an BMP image buffer
//*********************************************************
int CJPEG::ReadJPEGToRGBBuf(const char *fileName, UINT *width, UINT *height)
{
	// basic code from IJG Jpeg Code v6 example.c

	*width=0;
	*height=0;

	/* This struct contains the JPEG decompression parameters and pointers to
	* working space (which is allocated as needed by the JPEG library).
	*/
	struct jpeg_decompress_struct cinfo;
	/* We use our private extension JPEG error handler.
	* Note that this struct must live as long as the main JPEG parameter
	* struct, to avoid dangling-pointer problems.
	*/
	struct my_error_mgr jerr;
	/* More stuff */
	FILE * infile = NULL;		/* source file */

	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */

	/* In this example we want to open the input file before doing anything else,
	* so that the setjmp() error recovery below can assume the file is open.
	* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	* requires it in order to read binary files.
	*/

	if ((infile = fopen(fileName, "rb")) == NULL) 
	{
		return S34GRA_E_FILEOPEN_FAILED;
	}

	/* Step 1: allocate and initialize JPEG decompression object */

	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;


	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */

		jpeg_destroy_decompress(&cinfo);

		if (infile != NULL)
			fclose(infile);
		return S34GRA_E_JPEG_SIGNALED_ERROR;
	}

	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */

	jpeg_stdio_src(&cinfo, infile);

	/* Step 3: read file parameters with jpeg_read_header() */

	(void) jpeg_read_header(&cinfo, TRUE);
	/* We can ignore the return value from jpeg_read_header since
	*   (a) suspension is not possible with the stdio data source, and
	*   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	* See libjpeg.doc for more info.
	*/

	/* Step 4: set parameters for decompression */

	/* In this example, we don't need to change any of the defaults set by
	* jpeg_read_header(), so we do nothing here.
	*/

	/* Step 5: Start decompressor */

	(void) jpeg_start_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	* with the stdio data source.
	*/

	/* We may need to do some setup of our own at this point before reading
	* the data.  After jpeg_start_decompress() we have the correct scaled
	* output image dimensions available, as well as the output colormap
	* if we asked for color quantization.
	* In this example, we need to make an output work buffer of the right size.
	*/ 

	// get our buffer set to hold data
	BYTE *dataBuf;

	////////////////////////////////////////////////////////////
	// alloc and open our new buffer
	dataBuf = (BYTE *)new BYTE[cinfo.output_width * 3 * cinfo.output_height];

	if (dataBuf == NULL) 
	{
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return S34GRA_E_MEMORYALLOCATION_FAILED;
	}

	// how big is this thing gonna be?
	*width = cinfo.output_width;
	*height = cinfo.output_height;
	
	/* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.output_components;

	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/
	while (cinfo.output_scanline < cinfo.output_height) {
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		/* Assume put_scanline_someplace wants a pointer and sample count. */

		// asuumer all 3-components are RGBs
		if (cinfo.out_color_components == 3) {
			
			j_putRGBScanline(buffer[0], 
								*width,
								dataBuf,
								cinfo.output_scanline-1);

		} 
//		else if (cinfo.out_color_components==1) 
//		{
//			// assume all single component images are grayscale
//			j_putGrayScanlineToRGB(buffer[0], 
//								*width,
//								dataBuf,
//								cinfo.output_scanline-1);
//		}

	}

	/* Step 7: Finish decompression */

	(void) jpeg_finish_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	* with the stdio data source.
	*/

	/* Step 8: Release JPEG decompression object */

	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);

	/* After finish_decompress, we can close the input file.
	* Here we postpone it until after no more JPEG errors are possible,
	* so as to simplify the setjmp error logic above.  (Actually, I don't
	* think that jpeg_destroy can do an error exit, but why assume anything...)
	*/
	fclose(infile);

	/* At this point you may want to check to see whether any corrupt-data
	* warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	*/

	m_pData = dataBuf;
	return S34GRA_E_NOERROR;
}




//******************************************************************************
// Read a JPEG file with specified palette and return an BitMap RGB Image Buffer
//******************************************************************************
int CJPEG::ReadJPEGToRGBBuf(const char * fileName, UINT * width, UINT * height, BYTE colormap[][256])
{
	// basic code from IJG Jpeg Code v6 example.c
	*width=0;
	*height=0;

	/* This struct contains the JPEG decompression parameters and pointers to
	* working space (which is allocated as needed by the JPEG library).
	*/
	struct jpeg_decompress_struct cinfo;
	/* We use our private extension JPEG error handler.
	* Note that this struct must live as long as the main JPEG parameter
	* struct, to avoid dangling-pointer problems.
	*/
	struct my_error_mgr jerr;
	/* More stuff */
	FILE * infile = NULL;		/* source file */

	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */

	/* In this example we want to open the input file before doing anything else,
	* so that the setjmp() error recovery below can assume the file is open.
	* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	* requires it in order to read binary files.
	*/

	if ((infile = fopen(fileName, "rb")) == NULL) 
	{
		return S34GRA_E_FILEOPEN_FAILED;
	}

	/* Step 1: allocate and initialize JPEG decompression object */

	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;


	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) 
	{
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */

		jpeg_destroy_decompress(&cinfo);

		if (infile != NULL)
			fclose(infile);
		return S34GRA_E_JPEG_SIGNALED_ERROR;
	}

	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */

	jpeg_stdio_src(&cinfo, infile);

	/* Step 3: read file parameters with jpeg_read_header() */

	(void) jpeg_read_header(&cinfo, TRUE);
	/* We can ignore the return value from jpeg_read_header since
	*   (a) suspension is not possible with the stdio data source, and
	*   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	* See libjpeg.doc for more info.
	*/

	/* Step 4: set parameters for decompression */
	// Set up the specified palette to cinfo structure
	cinfo.quantize_colors = 1;
	cinfo.actual_number_of_colors = 256;

	cinfo.colormap = new JSAMPROW [3];
	for (int k = 0; k < 3; k++)
		cinfo.colormap[k] = new unsigned char [256];

	for (k = 0; k < 3; k++)
	for (int l = 0; l < 256; l++)
		cinfo.colormap[k][l] = colormap[k][l];

	/* Step 5: Start decompressor */
	(void) jpeg_start_decompress(&cinfo);

	// get our buffer set to hold data
	BYTE *dataBuf;

	////////////////////////////////////////////////////////////
	// alloc and open our new buffer
	dataBuf = (BYTE *)new BYTE[cinfo.output_width * cinfo.output_height];
	if (dataBuf == NULL) 
	{
//		sprintf(m_error, "CJPEGImage : Out of memory");
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return S34GRA_E_MEMORYALLOCATION_FAILED;
	}

	// how big is this thing gonna be?
	*width = cinfo.output_width;
	*height = cinfo.output_height;
	
	/* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.output_components;

	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/
	int i;
	int offset;

	while (cinfo.output_scanline < cinfo.output_height) {
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		/* Assume put_scanline_someplace wants a pointer and sample count. */

		offset = (cinfo.output_scanline-1) * (*width);
		for (i = 0; i < (int)*width; i++)
			*(dataBuf + offset + i) = *(buffer[0]+i);
	}

	/* Step 7: Finish decompression */
	(void) jpeg_finish_decompress(&cinfo);

	/* Step 8: Release JPEG decompression object */
	for (i = 0; i < 3; i++)
		delete [] cinfo.colormap[i];
	delete [] cinfo.colormap;

	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);

	/* After finish_decompress, we can close the input file.
	* Here we postpone it until after no more JPEG errors are possible,
	* so as to simplify the setjmp error logic above.  (Actually, I don't
	* think that jpeg_destroy can do an error exit, but why assume anything...)
	*/
	fclose(infile);

	/* At this point you may want to check to see whether any corrupt-data
	* warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	*/

	m_pData = dataBuf;
	return S34GRA_E_NOERROR;
}




//*********************************************************
//	swap Rs and Bs
//
//	Note! this does its stuff on buffers with a whole number
//	of pixels per data row!!
//*********************************************************
BOOL CJPEG::RGBToBGR(BYTE *buf, UINT widthPix, UINT height)
{
	if (buf == NULL)
		return FALSE;

	UINT col, row;

	for (row = 0; row < height; row++) {
		for (col = 0; col < widthPix; col++) {
			BYTE *pRed, *pGrn, *pBlu;
			pRed = buf + row * widthPix * 3 + col * 3;
			pGrn = buf + row * widthPix * 3 + col * 3 + 1;
			pBlu = buf + row * widthPix * 3 + col * 3 + 2;

			// swap red and blue
			BYTE tmp;
			tmp = *pRed;
			*pRed = *pBlu;
			*pBlu = tmp;
		}
	}
	return TRUE;
}

//*********************************************************
//	to handle fatal errors.
//	the original JPEG code will just exit(0). can't really
//	do that in Windows....
//*********************************************************
void my_error_exit(j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	char buffer[JMSG_LENGTH_MAX];

	/* Create the message */
	(*cinfo->err->format_message) (cinfo, buffer);

	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}



//*********************************************************
//	stash a scanline
//*********************************************************
void CJPEG::j_putRGBScanline(BYTE * jpegline, int widthPix, BYTE * outBuf, int row)
{
	int offset = row * widthPix * 3;
	int count;
	for (count=0;count<widthPix;count++) 
	{
		*(outBuf + offset + count * 3 + 0) = *(jpegline + count * 3 + 0);
		*(outBuf + offset + count * 3 + 1) = *(jpegline + count * 3 + 1);
		*(outBuf + offset + count * 3 + 2) = *(jpegline + count * 3 + 2);
	}
}


//*********************************************************
//	stash a gray scanline
//*********************************************************
/*
void CJPEG::j_putGrayScanlineToRGB(BYTE * jpegline, int widthPix, BYTE * outBuf, int row)
{
	int  offset = row * widthPix;
	int  count;
	BYTE iGray;

	for (count = 0; count < widthPix; count++) 
	{
		// get our grayscale value
		iGray = *(jpegline + count);

		*(outBuf + offset + count * 3 + 0) = iGray;
		*(outBuf + offset + count * 3 + 1) = iGray;
		*(outBuf + offset + count * 3 + 2) = iGray;
	}
}
*/

//******************************************************************
// write a JPEG file from a 3-component, 1-byte per component buffer
//******************************************************************
int CJPEG::RGBToJpegFile(const char * fileName, BYTE *dataBuf,
			     UINT widthPix, UINT height, int quality/*, BOOL color*/)
{
	int ret = S34GRA_E_NOERROR;

	// Validate parameters
	//
	if (dataBuf==NULL || fileName == NULL || widthPix < 1 || height < 1 || quality < 1)
		return S34GRA_E_INVALIDPARAM;

/*
	BYTE* tmp;
	if (!color) {
		tmp = (BYTE*)new BYTE[widthPix*height];
		if (tmp==NULL) {
  		    sprintf(m_error, "CJPEGImage : Out of memory");
			return FALSE;
		}

		UINT row,col;
		for (row=0;row<height;row++) {
			for (col=0;col<widthPix;col++) {
				BYTE *pRed, *pGrn, *pBlu;
				pRed = dataBuf + row * widthPix * 3 + col * 3;
				pGrn = dataBuf + row * widthPix * 3 + col * 3 + 1;
				pBlu = dataBuf + row * widthPix * 3 + col * 3 + 2;

				// luminance
				int lum = (int)(.299 * (double)(*pRed) + .587 * (double)(*pGrn) + .114 * (double)(*pBlu));
				BYTE* pGray;
				pGray = tmp + row * widthPix + col;
				*pGray = (BYTE)lum;
			}
		}
	}
*/
	struct jpeg_compress_struct cinfo;
	/* More stuff */
	FILE * outfile=NULL;			/* target file */
	int row_stride;			/* physical row widthPix in image buffer */

	struct my_error_mgr jerr;

	/* Step 1: allocate and initialize JPEG compression object */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) 
	{
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */

		jpeg_destroy_compress(&cinfo);

		if (outfile!=NULL)
			fclose(outfile);

//		if (!color) {
//			delete [] tmp;
//		}
// 		sprintf(m_error, "CJPEGImage : JpegLib has signaled an error");
		return S34GRA_E_JPEG_SIGNALED_ERROR;
	}

	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);

	/* Step 2: specify data destination (eg, a file) */
	/* Note: steps 2 and 3 can be done in either order. */

	if ((outfile = fopen(fileName, "wb")) == NULL) 
	{
//		sprintf(m_error, "CJPEGImage : Can't open %s", fileName);
		return S34GRA_E_FILEOPEN_FAILED;
	}

	jpeg_stdio_dest(&cinfo, outfile);

	/* Step 3: set parameters for compression */
												    
	/* First we supply a description of the input image.
	* Four fields of the cinfo struct must be filled in:
	*/
	cinfo.image_width = widthPix; 	/* image widthPix and height, in pixels */
	cinfo.image_height = height;
//	if (color) {
		cinfo.input_components = 3;		/* # of color components per pixel */
		cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
//	} else {
//		cinfo.input_components = 1;		/* # of color components per pixel */
//		cinfo.in_color_space = JCS_GRAYSCALE; 	/* colorspace of input image */
///	}

 
  /* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */

  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   * To keep things simple, we pass one scanline per call; you can pass
   * more if you wish, though.
   */
  row_stride = widthPix * 3;	/* JSAMPLEs per row in image_buffer */

  while (cinfo.next_scanline < cinfo.image_height) 
  {
    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */
	BYTE* outRow;
//	if (color) {
		outRow = dataBuf + (cinfo.next_scanline * widthPix * 3);
//	} else {
//		outRow = tmp + ((cinfo.image_height - cinfo.next_scanline - 1) * widthPix);
//	}

    (void) jpeg_write_scanlines(&cinfo, &outRow, 1);
  }

  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);

  /* After finish_compress, we can close the output file. */
  fclose(outfile);

  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);

//  if (!color)
//	  delete [] tmp;
  /* And we're done! */

  return S34GRA_E_NOERROR;
}


int CJPEG::GetImageData(BYTE *pImgData, int iSize)
{
	memcpy(pImgData, m_pData, iSize);
	return S34GRA_E_NOERROR;
}

int CJPEG::GetSize(int *pWidth, int *pHeight)
{
	*pWidth = m_width;
	*pHeight = m_height;
	return S34GRA_E_NOERROR;
}

int CJPEG::GetImageLength(DWORD *pdwLength)
{
	if (m_bPaletteInUse)
		*pdwLength = m_width*m_height;
	else
		*pdwLength = (m_width*m_height*3);
	return S34GRA_E_NOERROR;
}

int CJPEG::Save(char *pFileName, int iQuality)
{
	int ret = S34GRA_E_NOERROR;
	UINT i;
	BYTE *pBuf = NULL;
	BYTE *pTemp, *pTemp1;
	BYTE *pImage;

	if (m_pData == NULL)
	{
		return S34GRA_E_IMAGE_NOT_CREATED_NOR_LOADED;
	}

	pImage = m_pData;

	if (m_bPaletteInUse)
	{
		pBuf = new BYTE [m_width * m_height * 3];
		if (pBuf == NULL)
			return S34GRA_E_MEMORYALLOCATION_FAILED;
		pTemp1 = pBuf;

		for (i = 0, pTemp = m_pData; i < m_width * m_height; i++, pTemp++)
		{
			*pTemp1++ = m_colormap[2][*pTemp];
			*pTemp1++ = m_colormap[1][*pTemp];
			*pTemp1++ = m_colormap[0][*pTemp];
		}
		pImage = pBuf;
	}

	// we vertical flip for display. undo that.
	m_bPaletteInUse = FALSE;
	CImageManipulation::FlipImage(pImage, m_width, m_height, 24);


	// we swap red and blue for display, undo that.
	RGBToBGR(pImage, m_width, m_height);

	// save RGB packed buffer to JPG
	ret = RGBToJpegFile(pFileName, pImage, m_width, m_height, iQuality);
	if (ret != 	S34GRA_E_NOERROR)
	{
		if (pBuf)
			delete []pBuf;
		return ret;
	}

	if (pBuf)
		delete []pBuf;

	return S34GRA_E_NOERROR;
}

/*********************************************
 *
 * only accepts true color images!
 * 
 *********************************************/
int CJPEG::LoadBMPData(CS34Bitmap *pBitmap)
{
	int width, height, depth;

	if (m_pData)
		delete []m_pData;

	pBitmap->GetSize(&width, &height);
	pBitmap->GetColorDepth(&depth);


	m_width = width;
	m_height = height;

	int iSize = width*height*(depth/8);

	m_pData = new BYTE[iSize];
	
	pBitmap->GetImageData(m_pData, iSize);

	return S34GRA_E_NOERROR;
}
