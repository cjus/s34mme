#include "stdafx.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "gif.h"
//#include "errors.h"

CGif::CGif() { pGifData=0; pImg0=0; Reset(); }
CGif::~CGif() { Reset(); }

void CGif::Reset()
{
    if (pGifData) {
        free(pGifData); pGifData=0;
    }
    if (pImg0) {
        free(pImg0); pImg0=0;
    }
}

int CGif::SkipBlocks()
{   unsigned char uc;

    do {
        if (NotEnoughBytes(1)) return -1;
        uc=*pGifRead++;
        if (NotEnoughBytes(uc)) return -1;
        pGifRead+=uc;
    } while (uc);
    return 0;
}

int CGif::GetInfo(int*pXs,int*pYs,int*pNum)
{
    if (0==pImg0) return S34GRA_E_INVALIDPARAM;
    if (pXs) *pXs=ImgDescScrWidth;
    if (pYs) *pYs=ImgDescScrHeight;
    if (pNum) *pNum=imageCount;
    return 0;
}

int CGif::CountImages()
//Get the image count before actual decoding, also have
//the effect of validating the GIF file down to block 
//level. If this fails no decoding will be done.
{   unsigned char*pOld,uc;
    int palLen;

    pOld=pGifRead;
    imageCount=0;
    while (1) {
        if (NotEnoughBytes(1)) return -1;
        uc=*pGifRead++;
        if ('!'==uc) { //extension
            if (NotEnoughBytes(1)) return -1;
            pGifRead++; //extension type
            if (SkipBlocks()) return -1;
        } else if (';'==uc) { //end of GIF marker
            break;
        } else if (','==uc) { //image
            if (NotEnoughBytes(9)) return -1;
            palLen=0;
            if (0x80&pGifRead[8]) { //local pal
                palLen=1+(7&pGifRead[8]); //BPP
                palLen=3*(1<<palLen);
            }
            pGifRead+=9;
            if (NotEnoughBytes(palLen)) return -1;
            pGifRead+=palLen;
            //next byte is initial code len
            if (NotEnoughBytes(1)) return -1;
            pGifRead++;
            if (SkipBlocks()) return -1;
            imageCount++;
        } else { //invalid char
            return -1;
        }
    }
    pGifRead=pOld; //restore read pointer
    return (0==imageCount) ? -1 : 0;
}

int CGif::FileIntoMemory(char*pf)
//uses C runtime FILE functions, not Win API File* functions.
{   FILE*fp;

    Reset();
    fp=fopen(pf,"rb");
    if (0==fp) return S34GRA_E_FILEOPEN_FAILED;

    if (1!=fread(ImgDescSignature,6,1,fp)) {
        fclose(fp);
        return S34GRA_E_INVALIDIMAGEDATA;
    }
    if (memcmp(ImgDescSignature,"GIF89a",6) &&
        memcmp(ImgDescSignature,"GIF87a",6)) {
        fclose(fp);
        return S34GRA_E_INVALIDIMAGEDATA;
    }

    fseek(fp,0,SEEK_END);
    dataLen=ftell(fp);
    if (0== dataLen) {
        fclose(fp);
        return S34GRA_E_FILEREAD_FAILED;
    }
    pGifData=(unsigned char*)malloc(dataLen-6);
    if (0==pGifData) {
        fclose(fp);
        return S34GRA_E_MEMORYALLOCATION_FAILED;
    }
    fseek(fp,6,SEEK_SET);
    int i=fread(pGifData,dataLen-6,1,fp);
    fclose(fp);
    if (1!=i) {
        Reset();
        return S34GRA_E_FILEREAD_FAILED;
    }
    pGifRead=pGifData;
    return 0;
}

int CGif::LoadFile(char*pf,int padto,int userBackColor)
{   int i,padLen,rc,y;
    unsigned char*p,*prevPal,*p2;
    SUB_IMAGE_INFO*pCurSi,*pPrevSi;

    assert(padto>0);
    if (userBackColor<-1 || userBackColor>255)
        return S34GRA_E_INVALIDPARAM;

    rc=FileIntoMemory(pf);
    if (rc) return rc;

    if (ParseHeader()) goto FILE_LOAD_ERR;
    replaceBkClr=ImgDescBkClr;
    if (-1!=userBackColor && ImgDescGlobalPal) 
        replaceBkClr=(unsigned char)userBackColor;

    if (CountImages()) goto FILE_LOAD_ERR;

    //if we got here, the GIF file is valid down to block level
    stride=((ImgDescScrWidth+padto-1)/padto)*padto;
    imgPixelsLen=stride*ImgDescScrHeight;
    //each decoded image uses subImageLen bytes of memory
    subImageLen=imgPixelsLen + sizeof(SUB_IMAGE_INFO);

    //get one big memory block for all images decoded    
    pImg0=(unsigned char*)malloc(imageCount* subImageLen );
    if (0==pImg0) {
        Reset();
        return S34GRA_E_MEMORYALLOCATION_FAILED;
    }

    //setup first image
    p=pImg0+sizeof(SUB_IMAGE_INFO); //p->start of first image pixels
    memset(p,replaceBkClr,imgPixelsLen);
    padLen=stride-ImgDescScrWidth;
    assert(padLen>=0);
    if (padLen) { //zero out padding bytes    
        for (int ys=ImgDescScrHeight;ys;ys--) {
            memset(p+ImgDescScrWidth,0,padLen);
            p+=stride;
        }    
    }

    p=pImg0;
    prevPal=GlobalPal;
    for (i=imageCount;i;i--) {
        pCurSi=(SUB_IMAGE_INFO*)p;
        memcpy(pCurSi->pal,prevPal,768); //copy previous palette
        if (i!=imageCount) 
        { //if not 1st image
            p2=p+sizeof(SUB_IMAGE_INFO); //p2->upperleft pixel
            //copy previous image
            memcpy(p2,p-imgPixelsLen, imgPixelsLen); 
            if (2==pPrevSi->disposal) 
            { //if remove using background
                //p2 -> upperleft of prev dirty rect
                p2+=stride* pPrevSi->y0 + pPrevSi->x0; 
                for (y=pPrevSi->ys;y;y--) 
                { //fill dirty rect with background color
                    memset(p2,replaceBkClr,pPrevSi->xs);
                    p2+=stride;
                } //for (y=pPrevSi->ys;y;y--)
            } //if (2==pPrevSi->disposal) 
        } //if (i!=imageCount) 
        if (GetOneImage(pCurSi)) goto FILE_LOAD_ERR;
        prevPal=pCurSi->pal;
        p+=subImageLen;
        pPrevSi=pCurSi;
    }                              
    free(pGifData); pGifData=0;
    return 0;

FILE_LOAD_ERR:
    Reset();
    return S34GRA_E_INVALIDIMAGEDATA;
}

int CGif::NotEnoughBytes(unsigned long blen)
{
    if (pGifRead-pGifData + blen >dataLen) return -1;
    return 0;
}

int CGif::ParseHeader()
{   unsigned char uc;

    if (NotEnoughBytes(7)) return -1;
    ImgDescScrWidth =*(short*)pGifRead;
    ImgDescScrHeight=*(short*)(pGifRead+2);
    uc=pGifRead[4];
    ImgDescGlobalPal=(uc&0x80)?true:false;
    ImgDescSortedPal=(uc& 0x8)?true:false;
    ImgDescColorDepth=1 + ( (uc>>4) & 7 ); 
    ImgDescBPP=1+ (uc &7);
    ImgDescBkClr=pGifRead[5];
    ImgDescAspectRatio=pGifRead[6];
    pGifRead+=7;
    
    memset(GlobalPal,0,768);
    if (ImgDescGlobalPal) {
        int maxClr=1<<ImgDescBPP;
        if ( NotEnoughBytes(maxClr*3) ) return -1;
        memcpy(GlobalPal,pGifRead,maxClr*3);
        pGifRead+=maxClr*3;
    }
    return 0;
}

int CGif::ParseSubImageHeader(SUB_IMAGE_INFO*psii)
{
    if (NotEnoughBytes(9)) return -1;
    
    psii->x0=*(unsigned short*)(pGifRead);
    psii->y0=*(unsigned short*)(pGifRead+2);
    psii->xs=*(unsigned short*)(pGifRead+4);
    psii->ys=*(unsigned short*)(pGifRead+6);
    unsigned char impack=pGifRead[8];
    psii->fLocalPal = (0x80&impack) ?true :false;
    psii->fInterlace= (0x40&impack) ?true :false;
    psii->fSorted   = (0x20&impack) ?true :false;
    psii->BPP=1+(7&impack);
    pGifRead+=9;
        
    if (psii->fLocalPal) {
        int clrs=3*(1<<psii->BPP);
        if (NotEnoughBytes(clrs)) return -1;
        memcpy(psii->pal,pGifRead,clrs);
        pGifRead+=clrs;
    }
    return 0;
}

int CGif::GetOneImage(SUB_IMAGE_INFO*psi)
{   unsigned char uc,extType;

    lzw.transClr=0xffffFFFFL;
    while (1) {
        if (NotEnoughBytes(1)) return -1;
        uc=*pGifRead++;
        if (','==uc) break; //image
        if ('!'!=uc) return -1;
        //extension
        if (NotEnoughBytes(1)) return -1;
        extType=*pGifRead++; //extension type
        if (0xf9==extType) { //graphic animation control
            if (NotEnoughBytes(1)) return -1;
            uc=*pGifRead++;
            if (0==uc) continue;
            if (NotEnoughBytes(uc)) return -1;
            psi->disposal = 7&(pGifRead[0]>>2);
            if (uc>=4) {
                if (1 & pGifRead[0]) lzw.transClr=pGifRead[3];
            }
            pGifRead+=uc;
        }
        if (SkipBlocks()) return -1;
    }
    if (ParseSubImageHeader(psi)) return -1;

/* GIF LZW decode steps
     [1] Initialize string table;
     [2] get first code: <code>;
     [3] output the string for <code> to the charstream;
     [4] <old> = <code>;
     [5] <code> <- next code in codestream;
     [6] does <code> exist in the string table?
      (yes: output the string for <code> to the charstream;
            [...] <- translation for <old>;
            K <- first character of translation for <code>;
            add [...]K to the string table;        
            <old> <- <code>;  
      )
      (no: [...] <- translation for <old>;
           K <- first character of [...];
           output [...]K to charstream and add it to string table;
           <old> <- <code>
      )
     [7] go to [5];
*/
    if (NotEnoughBytes(1)) return -1;
    lzw.minCodeLen=*pGifRead++;
    lzw.remainBits=0;
    lzw.bitLeft=lzw.byteLeft=0; //byte left in GIF block

    //setup output image-related values in LZW structure
    lzw.pImage=(unsigned char*)(psi+1); 
    lzw.linePass=1; lzw.lineNum=0;
    lzw.lineLeft=0; //bytes left in line
    lzw.interlace=psi->fInterlace;
    lzw.x0=psi->x0; lzw.y0=psi->y0;
    lzw.xs=psi->xs; lzw.ys=psi->ys;
    
    do {
        DecodeResetCodes();
        do { //just in case we get a series of clear codes
            if (DecodeGetCode()) return -1;
        } while (lzw.CLEARCODE==lzw.curCode);

        if (lzw.ENDCODE==lzw.curCode) break;
        if (lzw.curCode>=lzw.codeUsed) return -1;
        if (DecodeOutputCode(lzw.curCode)) goto END_GET_IMAGE;
        lzw.prevCode=lzw.curCode;
        while (1) {
            if (DecodeGetCode()) return -1;
            if (lzw.CLEARCODE==lzw.curCode) break;
            if (lzw.ENDCODE==lzw.curCode) break;
            if (lzw.curCode<lzw.codeUsed) { //if code exists
                if (DecodeOutputCode(lzw.curCode)) goto END_GET_IMAGE;
                if (DecodeAddCode(lzw.prevCode,lzw.codeByte0)) return -1;
            } else { //code doesn't exist yet
                if (lzw.curCode>lzw.codeUsed) return -1;
                if (DecodeOutputCode(lzw.prevCode)) 
                    goto END_GET_IMAGE;
                if (DecodeOutputByte(lzw.codeByte0))
                    goto END_GET_IMAGE;
                if (DecodeAddCode(lzw.prevCode,lzw.codeByte0)) return -1;
            } //if (lzw.curCode<lzw.codeUsed) {
            lzw.prevCode=lzw.curCode;
        } //while (1) {
    } while (lzw.ENDCODE!=lzw.curCode);

END_GET_IMAGE:
    pGifRead+=lzw.byteLeft;
    if (SkipBlocks()) return -1;
    return 0;
}

int CGif::DecodeOutputCode(unsigned int code)
{ //recursive
    if (code>((unsigned int)1<<lzw.minCodeLen)) {
        if (DecodeOutputCode(lzw.codes[code][0])) return -1;
        return DecodeOutputByte((unsigned char)(lzw.codes[code][1]));
    } 
    lzw.codeByte0=(unsigned char)code;
    return DecodeOutputByte(lzw.codeByte0);
}

int CGif::DecodeAddCode(unsigned int code,unsigned char cK)
{
    if (GIF_MAX_CODES==lzw.codeUsed) return -1;
    lzw.codes[lzw.codeUsed][0]=code;
    lzw.codes[lzw.codeUsed][1]=cK;
    lzw.codeUsed++;
    if (lzw.codeUsed==(unsigned int)1<<lzw.codeLen) {
        if (lzw.codeLen<GIF_MAX_LZW_LEN) {
            lzw.codeLen++; 
            lzw.codeMask=(1<<lzw.codeLen)-1;
        }
    }
    return 0;
}

void CGif::DecodeResetCodes()
{
    lzw.codeLen=lzw.minCodeLen + 1;
    if (lzw.codeLen<3) lzw.codeLen=3;
    lzw.CLEARCODE=1 << lzw.minCodeLen;
    lzw.ENDCODE  =1 + lzw.CLEARCODE;
    lzw.codeMask=(1<<lzw.codeLen)-1;
    lzw.codeUsed=lzw.ENDCODE+1; 
}

static int codecnt=0; //for debugging 
int CGif::DecodeGetCode()
{   unsigned int code;

    while (lzw.bitLeft<lzw.codeLen) {
        if (0==lzw.byteLeft) {
            if (NotEnoughBytes(1)) return -1;
            lzw.byteLeft= *pGifRead++;
            if (0==lzw.byteLeft) {
            //some GIF streams don't have the terminate code,
            //so when the buffer runs out, just fake ENDCODE
                pGifRead--;
                lzw.curCode=lzw.ENDCODE;
                return 0;
            }
            if (NotEnoughBytes(lzw.byteLeft)) return -1;
        }
        lzw.remainBits|=(pGifRead[0]<< lzw.bitLeft);
        lzw.bitLeft+=8; pGifRead++; lzw.byteLeft--;
    }
    code=lzw.remainBits & lzw.codeMask;
    lzw.remainBits >>= lzw.codeLen;
    lzw.bitLeft -= lzw.codeLen;
    lzw.curCode=code;

#if 1 //for debugging 
    if (63==codecnt)
        codecnt=63; //set breakpoint here to break on n-th code
    printf("code %d, %d %d\n",
             codecnt,lzw.codeLen,code);
    codecnt++;
#endif

    return 0;
}

int CGif::GetImageData(int imgNum,
                       unsigned char**ppPal,
                       unsigned char**ppPixel)
{   SUB_IMAGE_INFO*psi;

    if (0==pImg0 ||imgNum<0 || imgNum>=imageCount)
        return S34GRA_E_INVALIDPARAM;
    
    psi=(SUB_IMAGE_INFO*)
        (pImg0+(sizeof(SUB_IMAGE_INFO)+imgPixelsLen)*imgNum);
    
    if (ppPal) *ppPal=psi->pal;
    if (ppPixel) *ppPixel=(unsigned char*)(psi+1);
    return 0;
}

int CGif::DecodeOutputByte(unsigned char cK)
{   int liney;

    if (0==lzw.lineLeft) {
        liney=lzw.y0+lzw.lineNum;
        if (liney>=ImgDescScrHeight) return -1;
        lzw.pLine=lzw.pImage+ stride*liney+ lzw.x0;
        lzw.lineLeft=lzw.xs;
        if (lzw.interlace) { //handle interlaced images
            if (1==lzw.linePass) {
                lzw.lineNum+=8;
                if (lzw.lineNum>=lzw.ys) { 
                    lzw.lineNum=4; lzw.linePass=2;
                    goto PASS2CHECK;
                }
            } else if (2==lzw.linePass) {
                lzw.lineNum+=8;
PASS2CHECK:     if (lzw.lineNum>=lzw.ys) {
                    lzw.lineNum=2; lzw.linePass=3;
                    goto PASS3CHECK;
                }
            } else if (3==lzw.linePass) {
                lzw.lineNum+=4;
PASS3CHECK:     if (lzw.lineNum>=lzw.ys) {
                    lzw.lineNum=1; lzw.linePass=4;
                }
            } else {
                lzw.lineNum+=2;
            }
        } else {
            lzw.lineNum++;
        }
    } //if (0==lzw.lineLeft) {
    if (cK!=lzw.transClr) {
        if (ImgDescBkClr==cK) cK=replaceBkClr;
        lzw.pLine[0]=cK; 
    }
    lzw.pLine++; lzw.lineLeft--;
    return 0;
}

