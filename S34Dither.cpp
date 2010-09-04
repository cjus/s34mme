// S34Dither.cpp: implementation of the CS34Dither class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdlib.h"

#include "S34Palette.h"
//#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
    
//#include "errors.h"
#include "S34Dither.h"

/////////////////////////defines
#if 0 
    #define STACK_ALLOC malloc
    #define STACK_FREE free
#else 
    #define STACK_ALLOC _alloca
    #define STACK_FREE NULL
#endif

#define DITHER_SHIFT 4
#define DITHER_HALFVAL 8 //1<<(DITHER_SHIFT-1)

#define DITHER_GETVALUE(p,d) \
    (d)=((p)+DITHER_HALFVAL)>>DITHER_SHIFT; \
    if ((d)<0) (d)=0; else if ((d)>255) (d)=255; 

/////////////////////////////codes
void CS34Dither::Reset()
{ 
    pRoot=0; usedNode=0; 
    transMode=false;
}

////////class CS34Dither static functions
int __cdecl CS34Dither::sortcb_R(const void*p1, const void*p2 )
{   
    return ((DWORD*)p1)[0] - ((DWORD*)p2)[0];
}
int __cdecl CS34Dither::sortcb_G(const void*p1, const void*p2 )
{   
    return ((DWORD*)p1)[1] - ((DWORD*)p2)[1];
}
int __cdecl CS34Dither::sortcb_B(const void*p1, const void*p2 )
{   
    return ((DWORD*)p1)[2] - ((DWORD*)p2)[2];
}
int __cdecl CS34Dither::sortcb_RGB(const void*p1, const void*p2 )
{   int dif;
    dif=((DWORD*)p1)[0] - ((DWORD*)p2)[0];
    if (dif) return dif;
    dif=((DWORD*)p1)[1] - ((DWORD*)p2)[1];
    if (dif) return dif;
    dif=((DWORD*)p1)[2] - ((DWORD*)p2)[2];
    return dif;
}
////////end of class CS34Dither static functions

static void pixels_IndexToIntRGB(
        BYTE*pInBuf, int inLen, //input pixel value
        DWORD*pRGBA,            //effective palette
        int*pOut, int shiftBits)
{   DWORD*p1;

    while (inLen--)
    {
        p1=pRGBA+ ((*pInBuf++)<<2);
        *pOut++ = p1[0]<<shiftBits;
        *pOut++ = p1[1]<<shiftBits;
        *pOut++ = p1[2]<<shiftBits;
    }
} //static void pixels_IndexToIntRGB

static int palette_DumpToBuffer(CS34Palette*pPal,DWORD*pBuf)
{   int i,numClr;
    BYTE r,g,b;

    pPal->GetTotalColors(&numClr);
//    assert(numClr>=0 && numClr<=256);

    for (i=0;i<numClr;i++,pBuf+=4)
    {
        pPal->GetIndexData(i,&r,&g,&b);
        pBuf[0]=r; pBuf[1]=g; pBuf[2]=b;
        pBuf[3]=i;
    }
    return numClr;
} //static int palette_DumpToBuffer

int CS34Dither::GetBiggestColorRange(
        DWORD*pb, int numClr)
//return which channel has the biggest range
//1=red, 2=green, 3=blue
{   DWORD rMin,gMin,bMin,rMax,gMax,bMax;
    int i;

    rMin=rMax=pb[0];
    gMin=gMax=pb[1];
    bMin=bMax=pb[2];
    for (i=numClr;i;i--,pb+=4)
    {
        if      (pb[0]<rMin) rMin=pb[0];
        else if (pb[0]>rMax) rMax=pb[0];
        if      (pb[1]<gMin) gMin=pb[1];
        else if (pb[1]>gMax) gMax=pb[1];
        if      (pb[2]<bMin) bMin=pb[2];
        else if (pb[2]>bMax) bMax=pb[2];
    }
    rMax-=rMin;
    gMax-=gMin;
    bMax-=bMin;
    if (rMax>=gMax && rMax>=bMax) return 1; //red
    if (gMax>=bMax) return 2;
    return 3;
}

inline PALNODE*CS34Dither::NextUnusedNode()
{   PALNODE*pn;
    
//    assert(usedNode<MAX_PAL_LEN*2);
    pn=nodes + usedNode;
    usedNode++;
    memset(pn,0,sizeof(PALNODE));
    return pn;        
}

int CS34Dither::RemoveDuplicateEntries(
        DWORD*pBuf,int bufLen)
{   DWORD*pHead,*pTail;
    int i0;

//    assert(bufLen>=0);
    qsort(pBuf,bufLen,16,sortcb_RGB);
    pTail=pBuf;
    pHead=pBuf+4;
    
    for (i0=bufLen-1;i0;i0--)
    { 
        if (pHead[0]!=pTail[0] || pHead[1]!=pTail[1] ||
            pHead[2]!=pTail[2] )
        {
            pTail+=4;
            memcpy(pTail,pHead,16);
        }
        pHead+=4;
    }
    return (pTail- pBuf)/4 +1;
} //static int RemoveDuplicatePaletteEntries

PALNODE*CS34Dither::recursiveBuild(DWORD*pBuf, int bufLen)
{   PALNODE*pn;
    int leftLen,riteLen,split;
    DWORD*p0,*p2,cval,*pEnd,*pSplit;

//    assert(bufLen>0);
    if (1==bufLen)
    {
        pn=NextUnusedNode();
        memcpy(pn->rgbi,pBuf,16);
        return pn;
    }

    split=GetBiggestColorRange(pBuf,bufLen);
    if (1==split) qsort(pBuf,bufLen,16,sortcb_R);
    if (2==split) qsort(pBuf,bufLen,16,sortcb_G);
    if (3==split) qsort(pBuf,bufLen,16,sortcb_B);

    pEnd=pBuf + 4*(bufLen-1);
    pSplit=pBuf + 4*(bufLen/2);
    cval=pSplit[split-1];
    p0=pSplit;
    while (p0>pBuf)
    {
        p0-=4;
        if (cval!=p0[split-1]) {
            p0+=4; break;
        }
    }
    p2=pSplit;
    while (p2<pEnd)
    {
        p2+=4;
        if (cval!=p2[split-1]) {
            p2-=4; break;
        }
    }
    //at this point, p0 & p2 points to start and end of a block
    //of entries all having the same value in the 'split' channel
    leftLen=p0-pBuf;
    riteLen=pEnd-p2;
    pSplit=(riteLen>=leftLen)?(p2+4):p0;

    pn=NextUnusedNode();
    pn->splitFlag=split;
    pn->rgbi[split-1]=(pSplit[split-1]+pSplit[split-1 -4])/2;
    leftLen=(pSplit-pBuf)/4;
    riteLen=(pEnd-pSplit)/4+1;
//    assert(leftLen);
//    assert(riteLen);
    pn->pL=recursiveBuild(pBuf,leftLen);
    pn->pR=recursiveBuild(pSplit,riteLen);
    return pn;
}

int CS34Dither::SetPalette(CS34Palette*pPal)
{   DWORD rgbi[1024];
    int numClr;

    Reset();
    numClr=palette_DumpToBuffer(pPal,rgbi);
    if (0==numClr) return S34GRA_E_PALETTE_EMPTY;

    numClr=RemoveDuplicateEntries(rgbi,numClr);
    pRoot= recursiveBuild(rgbi,numClr);

    return S34GRA_E_NOERROR;
}

int CS34Dither::SetPaletteTrans(CS34Palette*pPal,int transC)
{   DWORD rgbi[1024];
    int numClr;

//    assert(transC>=0 && transC<256);
    Reset(); transMode=true;
    transIndex=transC;
    numClr=palette_DumpToBuffer(pPal,rgbi);
    
    if (transC<numClr) 
    { //yank out the transparent color
        memcpy(rgbi+transC*4,rgbi+(numClr-1)*4,16);
        numClr--;
    }
    if (0==numClr) return S34GRA_E_PALETTE_EMPTY;

    numClr=RemoveDuplicateEntries(rgbi,numClr);
    pRoot= recursiveBuild(rgbi,numClr);
//Dump(pRoot);
    return S34GRA_E_NOERROR;
}

void inline CS34Dither::MatchScaledColor(
        int*pPixel, BYTE*pOut, int*pDif)
{   
    int rgbVal[3],split;
    PALNODE*pn;

    DITHER_GETVALUE(pPixel[0],rgbVal[0]);
    DITHER_GETVALUE(pPixel[1],rgbVal[1]);
    DITHER_GETVALUE(pPixel[2],rgbVal[2]);

    pn=pRoot;
    while (1)
    {
        split=pn->splitFlag;
        if (0==split) break;
        if ((DWORD)rgbVal[split-1]<=pn->rgbi[split-1]) {
            pn=pn->pL;
        } else {
            pn=pn->pR;
        }
//        assert(pn);
    }
//    assert(0==pn->pL && 0==pn->pR);
    pDif[0]=rgbVal[0] - pn->rgbi[0];
    pDif[1]=rgbVal[1] - pn->rgbi[1];
    pDif[2]=rgbVal[2] - pn->rgbi[2];
    *pOut=(BYTE)pn->rgbi[3];
} 

int CS34Dither::Dither(
        BYTE *pData, int width, int height, 
        CS34Palette *pSrCS34Palette)
{   DWORD rgbSrc[1024]; 
    BYTE*pb; 
    int ix,iy,difs[3];
    int*pLines,*pLine1,*pLine2,*p1,*p2;
    
//    assert(width>0 && height>0);
//    assert(pRoot && usedNode); //gotta have these
//    assert(32==sizeof(PALNODE)); //check padding

    if (true==transMode) return S34GRA_E_INVALIDPARAM;
    
    //each RGB channel expand to full 32 bits
    ix=(width+2)*3; //ints per line, one extra pixel at both ends
    //get enough buffer for 2 lines
    pLines=(int*)STACK_ALLOC(ix*sizeof(int)*2);
    if (0==pLines) return S34GRA_E_MEMORYALLOCATION_FAILED;
    pLine1=pLines; 
    pLine2=pLines+ix; //3 ints per pixel
    
    palette_DumpToBuffer(pSrCS34Palette,rgbSrc);
    pb=pData;
    pixels_IndexToIntRGB(pb,width,rgbSrc,pLine1+3,DITHER_SHIFT);

    for (iy=0;iy<height-1;iy++)
    {
        p1=pLine1+3; p2=pLine2;
        
        pixels_IndexToIntRGB(pb+width,width,rgbSrc,p2+3,DITHER_SHIFT);

        for (ix=width;ix;ix--)
        {
            MatchScaledColor(p1, pb, difs);

            //current line
            p1[3]+=difs[0]*7; p1[4]+=difs[1]*7; p1[5]+=difs[2]*7;
            //next line
            p2[0]+=difs[0]*3; p2[1]+=difs[1]*3; p2[2]+=difs[2]*3;
            p2[3]+=difs[0]*5; p2[4]+=difs[1]*5; p2[5]+=difs[2]*5;
            p2[6]+=difs[0];   p2[7]+=difs[1];   p2[8]+=difs[2];

            p1+=3; p2+=3; pb++; 
        }
        p1=pLine1; pLine1=pLine2; pLine2=p1; //swap lines
    }
    //last line
    p1=pLine1+3;
    for (ix=width;ix;ix--)
    {
        MatchScaledColor(p1,pb, difs);
        pb++; p1+=3; 
        p1[0]+=difs[0]*7; p1[1]+=difs[1]*7; p1[2]+=difs[2]*7;
    }

    STACK_FREE(pLines);
    return S34GRA_E_NOERROR;
}

int CS34Dither::DitherTrans(
        BYTE *pData, int width, int height, 
        CS34Palette *pSrCS34Palette,int transC)
{   DWORD rgbSrc[1024]; 
    BYTE*pb; 
    int ix,iy,difs[3];
    int*pLines,*pLine1,*pLine2,*p1,*p2;
    
//    assert(width>0 && height>0);
//    assert(pRoot && usedNode); //gotta have these
//    assert(32==sizeof(PALNODE)); //check padding
//    assert(transC>=0 && transC<256);

    if (false==transMode) return S34GRA_E_INVALIDPARAM;
    
    //each RGB channel expand to full 32 bits
    ix=(width+2)*3; //ints per line, one extra pixel at both ends
    //get enough buffer for 2 lines
    pLines=(int*)STACK_ALLOC(ix*sizeof(int)*2);
    if (0==pLines) return S34GRA_E_MEMORYALLOCATION_FAILED;
    pLine1=pLines; 
    pLine2=pLines+ix; //3 ints per pixel
    
    palette_DumpToBuffer(pSrCS34Palette,rgbSrc);
    pb=pData;
    pixels_IndexToIntRGB(pb,width,rgbSrc,pLine1+3,DITHER_SHIFT);

    for (iy=0;iy<height-1;iy++)
    {
        p1=pLine1+3; p2=pLine2;
        
        pixels_IndexToIntRGB(pb+width,width,rgbSrc,p2+3,DITHER_SHIFT);

        for (ix=width;ix;ix--)
        {
            if (pb[0]==transC) {
                pb[0]=transIndex;
            } else {
                MatchScaledColor(p1, pb, difs);
                //current line
                p1[3]+=difs[0]*7; p1[4]+=difs[1]*7; p1[5]+=difs[2]*7;
                //next line
                p2[0]+=difs[0]*3; p2[1]+=difs[1]*3; p2[2]+=difs[2]*3;
                p2[3]+=difs[0]*5; p2[4]+=difs[1]*5; p2[5]+=difs[2]*5;
                p2[6]+=difs[0];   p2[7]+=difs[1];   p2[8]+=difs[2];
            }
            p1+=3; p2+=3; pb++; 
        }
        p1=pLine1; pLine1=pLine2; pLine2=p1; //swap lines
    }
    //last line
    p1=pLine1+3;
    for (ix=width;ix;ix--)
    {
        if (pb[0]==transC) {
            pb[0]=transIndex;
            pb++; p1+=3; 
        } else {
            MatchScaledColor(p1,pb, difs);
            pb++; p1+=3; 
            p1[0]+=difs[0]*7; p1[1]+=difs[1]*7; p1[2]+=difs[2]*7;
        }
    }

    STACK_FREE(pLines);
    return S34GRA_E_NOERROR;
}



//////////////////////////////////////////////
//
//dither RGB888 to RGB555

static void pixels_RGBToIntRGB(
        BYTE*pBuf,int bufLen, int*pOut, int shiftBits)
{
    for (bufLen*=3;bufLen;bufLen--)
    {
        *pOut++ = (*pBuf++) << shiftBits;
    }
}

int CS34Dither::Dither888To555(
        BYTE *pData, int width, int height)
{   BYTE*pb;
    WORD*pw;
    int r5,g5,b5,rd,gd,bd,i,ix,iy,bytePerLine;
    int*pLines,*pLine1,*pLine2,*p1,*p2,ditherTable[512];

//    assert(width>0 && height>0);

    for (p1=ditherTable,ix=0;ix<256;ix++,p1+=2)
    { //build DitherTable, this is fairly fast
        i=ix+4; if (i>255) i=255;
        p1[0]=i & (~7); //value        
        p1[1]=ix - p1[0]; //error
    }
    
    bytePerLine=width*3;
    ix=bytePerLine+6; //one extra pixel at each end
    pLines=(int*)STACK_ALLOC(ix*sizeof(int)*2);
    if (0==pLines)  return S34GRA_E_MEMORYALLOCATION_FAILED;
    pLine1=pLines; pLine2=pLines+ix;

    pw=(WORD*)pData;
    pb=pData;
    pixels_RGBToIntRGB(pb,width,pLine1+3,DITHER_SHIFT);
    for (iy=0;iy<height-1;iy++)
    {
        p1=pLine1+3; p2=pLine2;
        pixels_RGBToIntRGB(pb+bytePerLine,width,p2+3,DITHER_SHIFT);
        for (ix=width;ix;ix--)
        {   
            DITHER_GETVALUE(p1[0],rd);
            r5=ditherTable[rd+rd]; rd=ditherTable[rd+rd+1];
            DITHER_GETVALUE(p1[1],gd);
            g5=ditherTable[gd+gd]; gd=ditherTable[gd+gd+1];
            DITHER_GETVALUE(p1[2],bd);
            b5=ditherTable[bd+bd]; bd=ditherTable[bd+bd+1];

            *pw++ = (WORD)( (b5<<7) | (g5<<2) | (r5>>3) );
            p1[3]+=rd*7; p1[4]+=gd*7; p1[5]+=bd*7;
            p2[0]+=rd*3; p2[1]+=gd*3; p2[2]+=bd*3;
            p2[3]+=rd*5; p2[4]+=gd*5; p2[5]+=bd*5;
            p2[6]+=rd  ; p2[7]+=gd  ; p2[8]+=bd  ;
            p1+=3; p2+=3;
        }
        p1=pLine1; pLine1=pLine2; pLine2=p1; //swap lines
        pb+=bytePerLine;
    }
    //last line
    p1=pLine1+3;
    for (ix=width;ix;ix--)
    {
        DITHER_GETVALUE(p1[0],rd);
        r5=ditherTable[rd+rd]; rd=ditherTable[rd+rd+1];
        DITHER_GETVALUE(p1[1],gd);
        g5=ditherTable[gd+gd]; gd=ditherTable[gd+gd+1];
        DITHER_GETVALUE(p1[2],bd);
        b5=ditherTable[bd+bd]; bd=ditherTable[bd+bd+1];

        *pw++ = (WORD)( (b5<<7) | (g5<<2) | (r5>>3) );
        p1+=3;
        p1[0]+=rd*7; p1[1]+=gd*7; p1[2]+=bd*7;
    }
    STACK_FREE(pLines);
    return S34GRA_E_NOERROR;
}
