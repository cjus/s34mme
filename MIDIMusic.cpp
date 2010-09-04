#include "stdafx.h"
#include "mmsystem.h"
#include "MIDIMusic.h"


CS34MIDIMusic::CS34MIDIMusic(HWND hWndNotify)
{
    // Check if MIDI device exist
   	if (midiOutGetNumDevs() > 0)
		m_bSupported = TRUE;
	else
		m_bSupported = FALSE;

	// Check if MIDI device supported volume change
	MIDIOUTCAPS midiCaps;
	midiOutGetDevCaps(0,&midiCaps,sizeof(MIDIOUTCAPS));
	if (midiCaps.dwSupport == MIDICAPS_VOLUME ||
	    midiCaps.dwSupport == MIDICAPS_LRVOLUME)
	{
		m_bVolumeSupport = TRUE;
    }
	else
	{
		m_bVolumeSupport = FALSE;
	}

    m_hWndNotify = hWndNotify;
    if (m_bSupported==TRUE && m_bVolumeSupport==TRUE)
    	m_dwOrigSoundVolume = GetVolume();
}


CS34MIDIMusic::~CS34MIDIMusic()
{
    SetVolume(m_dwOrigSoundVolume);
}


//  Stop current MIDI music
LONG CS34MIDIMusic::Stop()
{
    if (m_bSupported!=TRUE)
    	return 0L;

    MCI_PLAY_PARMS mciPlayParms;
    mciPlayParms.dwCallback = (DWORD) m_hWndNotify;
    return(mciSendCommand(m_wMidiDeviceID, MCI_CLOSE, MCI_WAIT,
                    (DWORD)(LPVOID)&mciPlayParms));
}

// Play a MIDI file
LONG CS34MIDIMusic::Play(LPSTR lpszSoundFileName)
{
    if (m_bSupported!=TRUE)
    	return 0L;

    Stop();

    DWORD dwReturn, dwStringLen;
    MCI_OPEN_PARMS mciOpenParms;
    MCI_PLAY_PARMS mciPlayParms;

    dwStringLen = lstrlen(lpszSoundFileName);

    // Open the device by specifying the device name and device element.
    // MCI will choose a device capable of playing the given file
    mciOpenParms.lpstrDeviceType = "sequencer";
    mciOpenParms.lpstrElementName = lpszSoundFileName;

    dwReturn = mciSendCommand(0, MCI_OPEN,
                         MCI_OPEN_TYPE | MCI_OPEN_ELEMENT,
                         (DWORD)(LPVOID) &mciOpenParms);
    if (dwReturn)
    {
        // Failed to open device; don't close it, just return error
        return (dwReturn);
    }

    // Device opened successfully, get the device ID
    m_wMidiDeviceID = mciOpenParms.wDeviceID;

    // Begin playback.  The window procedure function for the parent window
    // will be notified with an MM_MCINOTIFY message when playback is
    // complete.  At this time, the procudure closes the device.
    mciPlayParms.dwCallback = (DWORD) m_hWndNotify;
    dwReturn = mciSendCommand(m_wMidiDeviceID, MCI_PLAY, MCI_NOTIFY,
                        (DWORD)(LPVOID)&mciPlayParms);

    if (dwReturn)
    {
        mciSendCommand(m_wMidiDeviceID, MCI_CLOSE, 0, NULL);
        return (dwReturn);
   }
   return (0L);
}

void CS34MIDIMusic::Pause()
{
	mciSendCommand(m_wMidiDeviceID, MCI_PAUSE, 0, NULL);
}

void CS34MIDIMusic::Resume()
{
	mciSendCommand(m_wMidiDeviceID, MCI_RESUME, 0, NULL);
}


/********************************************************************/
/* Return values:                                                   */
/*     DWORD containg volume.  LOWORD contains left channel volume  */
/*                             HIWORD contains right channel volume */
/*     returns -1 or -2 if error occurs.  -1 means that the handle  */
/*     is invalid or that a driver for the specified device does not*/
/*     exist.  -2 means that volume control is not supported for    */
/*     the existing sound device.                                   */
/*                                                                  */
/*     LOWORD and HIWORD is a value ranging from 0 - 65536          */
/*     0 is sound off and 65536 is max volume.                      */
/********************************************************************/
LONG CS34MIDIMusic::GetVolume()
{
    if (m_bSupported!=TRUE)
    	return 0;
	if (m_bVolumeSupport!=TRUE)
		return -2;

    UINT uRet;
    LONG dwVolume;

    uRet = midiOutGetVolume(0, (DWORD*)&dwVolume);
    if (uRet == MMSYSERR_INVALHANDLE || uRet == MMSYSERR_NODRIVER)
        return (DWORD)-1;
    if (uRet == MMSYSERR_NOTSUPPORTED)
        return (DWORD)-2;

    return dwVolume;
}



/********************************************************************/
/* Return values:                                                   */
/*     UINT containg volume.  LOWORD contains left channel volume   */
/*                             HIWORD contains right channel volume */
/*     returns -1 or -2 if error occurs.  -1 means that the handle  */
/*     is invalid or that a driver for the specified device does not*/
/*     exist.  -2 means that volume control is not supported for    */
/*     the existing sound device.                                   */
/*                                                                  */
/*     LOWORD and HIWORD is a value ranging from 0 - 65536          */
/*     0 is sound off and 65536 is max volume.                      */
/********************************************************************/
LONG CS34MIDIMusic::SetVolume(LONG dwVolume)
{
    if (m_bSupported!=TRUE)
    	return 0;
	if (m_bVolumeSupport!=TRUE)
		return -2;

    int nRet = midiOutSetVolume(0, dwVolume);
    if (nRet == MMSYSERR_INVALHANDLE || nRet == MMSYSERR_NODRIVER)
        return -1;
    if (nRet == MMSYSERR_NOTSUPPORTED)
        return -2;
    return nRet;
}


LONG CS34MIDIMusic::SetScaleVolume(int nVolumeSetting)
{
    if (m_bSupported!=TRUE)
    	return 0L;

    LONG dwVolume;

    switch (nVolumeSetting)
	{
        case SVOLUME_OFF:
            dwVolume = VOLUME_OFF;
            break;
        case SVOLUME_LOW:
            dwVolume = VOLUME_LOW;
            break;
        case SVOLUME_MEDIUM:
            dwVolume = VOLUME_MEDIUM;
            break;
        case SVOLUME_HIGH:
            dwVolume = VOLUME_HIGH;
            break;
	}

    return SetVolume(dwVolume);
}
