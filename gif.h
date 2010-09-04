/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * gif.h
 * Header file for the CGIF class
 *
 * Provides the ability to decode animated GIF images.
 *
 * This source code contains trade secrets and copyrighted materials, 
 * which is the intellectual property of Section34 Inc. 
 *
 * This source code is being licensed to SelvaSoft Inc. under contract
 * for a limited time only, for the purpose of evaluation and early
 * business development.  This code may not be used in an actual 
 * project or service.  Reverse engineering and the removal of this
 * header is expressly prohibited.
 * 
 * Unauthorized use, copying or distribution is a violation of U.S. 
 * and international laws and is strictly prohibited.
 *
 * Send inquires to: support@section34.com or contract the address 
 * or phone number shown below.
 *
 * (c)1998-99 Section34 Inc.
 * 5250 Colodny Drive, #5
 * Agoura Hills, CA 91301
 * 818 371-5785
 *
 *********************************************************************/

#ifndef  CGIF_H
#define  CGIF_H

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

#include "S34GRAErrors.h"

#define GIF_MAX_LZW_LEN 12
#define GIF_MAX_CODES (1<<GIF_MAX_LZW_LEN)

class DLL_EXP CGif
{
public:
    CGif();
    virtual ~CGif();
    void Reset();
    int LoadFile(char*pFile,int padto,int userBackColor);
    int GetInfo(int*pXs,int*pYs,int*pCount);
    int GetImageData(int imgNum,
                     unsigned char**ppPal,
                     unsigned char**ppPixel);

private:
    int CountImages();
    int NotEnoughBytes(unsigned long b);
    int SkipBlocks();
    int FileIntoMemory(char*pf); //actually load the file
    //FileIntoMemory() sets pGifData, pGifRead, dataLen
    unsigned char*pGifData, //-> file data minus GIF signature
                 *pGifRead; //decode read pointer, >= pGifData
    unsigned long dataLen;  //size of buffer pointed by pGifData

    int ParseHeader(); //read global image descriptor
//data from global image descriptor    
    unsigned short ImgDescScrWidth,ImgDescScrHeight;
    unsigned char ImgDescSignature[6];    
    unsigned char ImgDescColorDepth;
    unsigned char ImgDescBPP;
    unsigned char ImgDescBkClr;
    unsigned char ImgDescAspectRatio;
    unsigned char GlobalPal[768]; //global palette
    unsigned int stride; //line width with padding
    bool ImgDescGlobalPal,ImgDescSortedPal;
    unsigned char replaceBkClr;

    struct SUB_IMAGE_INFO { //for each sub image
        unsigned short x0,y0,xs,ys; //pos & size
        unsigned char BPP,disposal;
        bool fLocalPal,fInterlace,fSorted;
        unsigned char pal[768]; //local palette
        //followed by: BYTE pixels[ImgDescScrHeight*stride]
    };
    int ParseSubImageHeader(SUB_IMAGE_INFO*psi);
    int GetOneImage(SUB_IMAGE_INFO*psi); 

    int imageCount,      //total images in GIF
        imgPixelsLen,    //imgLen=ImgDescScrHeight*stride;
        subImageLen;     //sizeof(SUB_IMAGE_INFO)+imgPixelsLen
    unsigned char*pImg0; //memory block holding all decode images

    //variables used during LZW decode
    struct GIF_LZW_INFO {
        //codes[][0] is code, codes[][1] is root char
        unsigned short codes[GIF_MAX_CODES][2];
        unsigned int minCodeLen,codeLen,codeUsed,codeMask;
        unsigned int prevCode,curCode; //previous & current code
        unsigned int CLEARCODE,ENDCODE,transClr;
        
        //reading from pGifRead
        unsigned int byteLeft,bitLeft,remainBits;
        
        //output decoded bytes
        unsigned char*pLine,  //->current output pixel 
                     *pImage; //->upperleft corner of image
        bool interlace;
        unsigned int lineNum,  //output line number
                     lineLeft, //byte left in output line
                     linePass; //pass number for interlace GIF
        unsigned char codeByte0; //1st byte of last output code
        unsigned short x0,y0,xs,ys;
    };
    GIF_LZW_INFO lzw;

////////start: decode functions
    void DecodeResetCodes();
    int DecodeGetCode();
    int DecodeAddCode(unsigned int code,unsigned char cK);
    int DecodeOutputCode(unsigned int code);
    int DecodeOutputByte(unsigned char cK);
////////endof: decode functions

};

#endif // CGIF_H