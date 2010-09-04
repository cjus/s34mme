#include "stdafx.h"
#include "CDAudio.h"
#include "mmsystem.h"

CS34CDAudio::CS34CDAudio()
{
}

CS34CDAudio::CS34CDAudio(HWND hWndNotify)
{               
    m_hWndNotify = hWndNotify;
	Init();
}

CS34CDAudio::~CS34CDAudio()
{
	Stop();
    SetVolume(m_dwOrigSoundVolume);
	Close();    
}
    
void CS34CDAudio::Init()
{
	m_dwTotalTracks = 0;
	m_nVolumeSupport = NO_VOLUME_SUPPORT;

	// Check if AUX device exist
   	if (auxGetNumDevs() > 0)
		m_bSupported = TRUE;
	else
		m_bSupported = FALSE;		

	// Check if AUX device supported volume change
	AUXCAPS auxCaps;
	auxGetDevCaps(0,&auxCaps,sizeof(AUXCAPS));

	if (auxCaps.dwSupport & AUXCAPS_VOLUME)
		m_nVolumeSupport = SINGLE_CHANNEL;

	if (auxCaps.dwSupport & AUXCAPS_LRVOLUME)
		m_nVolumeSupport = LEFTRIGHT_CHANNEL;

//    m_hWndNotify = hWndNotify;
    if (m_bSupported==TRUE && m_nVolumeSupport!=NO_VOLUME_SUPPORT)
    	m_dwOrigSoundVolume = GetVolume();
    
    Open();	
	GetTotalTracks();    
}

BOOL CS34CDAudio::Open()
{
	MCI_OPEN_PARMS mciOpenParms;
	DWORD dwResult;
	BOOL bRet = TRUE;

    // Open the CD audio device by specifying the device name.
    mciOpenParms.lpstrDeviceType = "cdaudio";
    if (dwResult = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE, (DWORD)(LPVOID) &mciOpenParms))
    {
        // Failed to open device. Don't close it; just return error.
        return FALSE;
    }

    // The device opened successfully; get the device ID.
    m_wAuxDeviceID = mciOpenParms.wDeviceID;

    // Set the time format to track/minute/second/frame (TMSF).
    MCI_SET_PARMS  mciSetParms;
    mciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;
    if (dwResult = mciSendCommand(m_wAuxDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)(LPVOID) &mciSetParms))
    {
        mciSendCommand(m_wAuxDeviceID, MCI_CLOSE, 0, NULL);
        return FALSE;
    } 

	return bRet;
}    


BOOL CS34CDAudio::Close()
{
	MCI_GENERIC_PARMS mciGenParms;
	DWORD dwResult=0;
	BOOL bRet;

	if (m_wAuxDeviceID)
		dwResult = mciSendCommand(m_wAuxDeviceID, MCI_CLOSE, MCI_WAIT,
								 (DWORD)(LPVOID)&mciGenParms);		
		
	if (dwResult != 0)
		bRet = FALSE;
	else
	{
    	m_wAuxDeviceID = 0;		
		bRet = TRUE;
	}									  

	return bRet;
}
		

BOOL CS34CDAudio::Stop()
{                          
	BOOL bRet;

    if (m_bSupported!=TRUE)
    	bRet = FALSE;

	DWORD dwResult = mciSendCommand(m_wAuxDeviceID, MCI_STOP, MCI_WAIT, 
									(DWORD)(LPVOID)NULL);        	
	if (dwResult != 0)
		bRet = FALSE;
	else
		bRet = TRUE;
	
	return bRet;									  
}
	
BOOL CS34CDAudio::Eject()
{                          
	BOOL bRet;

    if (m_bSupported!=TRUE)
    	bRet = FALSE;
    	
    MCI_SET_PARMS mciSetParms;
	DWORD dwResult = mciSendCommand(m_wAuxDeviceID, MCI_SET, MCI_SET_DOOR_OPEN, 
									(DWORD)(LPVOID)&mciSetParms);        	
	if (dwResult != 0)
		bRet = FALSE;
	else
		bRet = TRUE;

	return bRet;
}
    

BOOL CS34CDAudio::Play(int nTrack)
{       
	BOOL bRet;

    if (m_bSupported != TRUE)
    	return 0L;
    
    DWORD dwReturn;
    MCI_PLAY_PARMS mciPlayParms;

    // Begin playback.  The window procedure function for the parent window
    // will be notified with an MM_MCINOTIFY message when playback is
    // complete.  At this time, the procudure closes the device.
    mciPlayParms.dwCallback = (DWORD) m_hWndNotify;

    mciPlayParms.dwFrom = MCI_MAKE_TMSF(nTrack,0,0,0);
	if (nTrack+1 < (int)m_dwTotalTracks)
	{
	    mciPlayParms.dwTo = MCI_MAKE_TMSF(nTrack+1,0,0,0);
		dwReturn = mciSendCommand(m_wAuxDeviceID, MCI_PLAY, 
    						MCI_FROM | MCI_TO | MCI_NOTIFY,
							(DWORD)(LPVOID)&mciPlayParms);
	}
	else
	{
		dwReturn = mciSendCommand(m_wAuxDeviceID, MCI_PLAY, 
    						MCI_FROM | MCI_NOTIFY,
							(DWORD)(LPVOID)&mciPlayParms);
	}
    
	if (dwReturn != 0)
		bRet = FALSE;
	else
		bRet = TRUE;

	return bRet;
}

     
DWORD CS34CDAudio::GetVolume()
{    
    if (m_bSupported!=TRUE)
    	return 0;
	if (m_nVolumeSupport==NO_VOLUME_SUPPORT)
		return (DWORD)-2;
    
    UINT uRet;
    DWORD dwVolume;                          
    
    uRet = auxGetVolume(0, &dwVolume);    
    if (uRet == MMSYSERR_INVALHANDLE || uRet == MMSYSERR_NODRIVER)
        return (DWORD)-1;
    if (uRet == MMSYSERR_NOTSUPPORTED)      
        return (DWORD)-2; 
        
    return dwVolume;
}

int CS34CDAudio::SetVolume(DWORD dwVolume)
{            
    if (m_bSupported!=TRUE)
    	return 0;
	if (m_nVolumeSupport==NO_VOLUME_SUPPORT)
		return -2;
                
    int nRet = auxSetVolume(0, dwVolume);
    if (nRet == MMSYSERR_INVALHANDLE || nRet == MMSYSERR_NODRIVER)
        return -1;
    if (nRet == MMSYSERR_NOTSUPPORTED)      
        return -2;
    return nRet;
}
    

BOOL CS34CDAudio::ExistCD()
{           
	return m_bSupported;
}
	
int CS34CDAudio::ChangeVolume()
{
	return m_nVolumeSupport;
}	

	
DWORD CS34CDAudio::GetTotalTracks()
{
    if (m_bSupported!=TRUE)
    	return FALSE;
    	
	DWORD dwTotalTracks;           
	MCI_STATUS_PARMS mciStatusParms;
	mciStatusParms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;		                     
	DWORD dwResult = mciSendCommand(m_wAuxDeviceID,MCI_STATUS,MCI_STATUS_ITEM,
		         				 (DWORD)(LPVOID)&mciStatusParms);
	if (dwResult != 0)
		dwTotalTracks = 0l;
	else
	{
		dwTotalTracks = mciStatusParms.dwReturn;
		m_dwTotalTracks = dwTotalTracks;
	}									  
		
	return dwTotalTracks;		
}
