/*********************************************************************
 * S E C T I O N 3 4   
 * M U L T I M E D I A   E X T E N T I O N   L I B R A R Y   D L L
 * 
 * MIDIMusic.h
 * Header file for the CS34MIDIMusic class
 *
 * Provides the ability to playback MIDI music.
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

#ifndef S34MIDIMUSIC
#define S34MIDIMUSIC

// Volumes
const LONG VOLUME_OFF =        0;
const LONG VOLUME_LOW =        MAKELONG(21845,21845);
const LONG VOLUME_MEDIUM =     MAKELONG(43690,43690);
const LONG VOLUME_HIGH =       MAKELONG(64536,64536);

#ifdef DLL_EXPORTS
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP __declspec(dllimport)
#endif

class DLL_EXP CS34MIDIMusic
{
public:
	enum
	{
		SVOLUME_OFF,
		SVOLUME_LOW,
		SVOLUME_MEDIUM,
		SVOLUME_HIGH
	};

    CS34MIDIMusic(HWND hWndNotify);
    ~CS34MIDIMusic();
    LONG  Stop();
    LONG  Play(LPSTR lpszSoundFileName);
	void  Pause();
	void  Resume();
    LONG  GetVolume();
    LONG  SetVolume(LONG dwVolume);
    LONG  SetScaleVolume(int nVolumeSetting);
	BOOL  ExistMIDI() { return m_bSupported; }
	BOOL  ChangeVolume() {return m_bVolumeSupport; }	
private:
    HWND    m_hWndNotify;
    LONG    m_dwOrigSoundVolume;
	UINT 	m_wMidiDeviceID;
	BOOL	m_bSupported;
	BOOL	m_bVolumeSupport;
};
                
#endif //S34MIDIMUSIC
