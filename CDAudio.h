/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * CDAudio.h
 * Header file for the CS34CDAudio object
 *
 * Provides the ability to play CD Audio
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

#ifndef S34CDAUDIO_H
#define S34CDAUDIO_H

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

class DLL_EXP CS34CDAudio
{
public:
    CS34CDAudio();
    CS34CDAudio(HWND hWndNotify);
    ~CS34CDAudio();
	void  Init();
    BOOL  Open();
    BOOL  Close();
    BOOL  Stop();
    BOOL  Eject();
    BOOL  Play(int nTrack);
    DWORD GetVolume();
    int   SetVolume(DWORD dwVolume);
    BOOL  ExistCD();       
	int   ChangeVolume();    
	DWORD GetTotalTracks();	
private:
    HWND    m_hWndNotify;
    DWORD   m_dwOrigSoundVolume;
	UINT 	m_wAuxDeviceID;    
	BOOL	m_bSupported;
	DWORD   m_dwTotalTracks;

	enum
	{
		NO_VOLUME_SUPPORT,
		SINGLE_CHANNEL,
		LEFTRIGHT_CHANNEL
	};
	int m_nVolumeSupport;
};
                
#endif  // S34CDAUDIO_H
