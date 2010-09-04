/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * S34GRAErrors.h
 * Header file for the Graphic Error values
 *
 * Header file for the Graphic Error values
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
#ifndef S34GRAERRORS
#define S34GRAERRORS

#define S34GRA_E_NOERROR						0
#define S34GRA_E_INVALIDPARAM					0x1000
#define S34GRA_E_PALETTE_EMPTY					0x1001
#define S34GRA_E_MEMORYALLOCATION_FAILED		0x1002
#define S34GRA_E_INVALIDSIZEPARAM				0x1003
#define S34GRA_E_IMAGE_NOT_CREATED_NOR_LOADED	0x1004
#define S34GRA_E_INVALIDIMAGEDATA				0x1005
#define S34GRA_E_FILECREATE_FAILED				0x1006
#define S34GRA_E_FILEOPEN_FAILED				0x1007
#define S34GRA_E_FILEREAD_FAILED				0x1008
#define S34GRA_E_FILEWRITE_FAILED				0x1009
#define S34GRA_E_JPEG_SIGNALED_ERROR			0x100A
#define S34GRA_E_QUANTIZE_FAILED				0x100B
#define S34GRA_E_PALETTE_NOT_LOADED				0x100C

#endif //S34GRAERRORS
