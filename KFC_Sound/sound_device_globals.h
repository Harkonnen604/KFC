#ifndef sound_device_globals_h
#define sound_device_globals_h

#include "dmusic_inc.h"
#include "dsound_inc.h"
#include <KFC_KTL\globals.h>
#include <KFC_Common\com.h>
#include "sound.h"
#include "sound_buffer.h"

// ---------------------
// Sound device globals
// ---------------------
class TSoundDeviceGlobals : public TGlobals
{
private:
	TCOMInitializer m_COMInitializer;


	void OnUninitialize	();
	void OnInitialize	();

	bool OnSuspend	();
	bool OnResume	();
	
public:
	// Devices
	IDirectMusic8*				m_pDMusic;
	IDirectSound8*				m_pDSound;
	IDirectMusicLoader8*		m_pDMusicLoader;
	IDirectMusicPerformance8*	m_pDMusicPerformance;
	IDirectMusicAudioPath8*		m_pDefaultAudioPath;
	
	// Buffers
	TSoundBuffer m_PrimarySoundBuffer;
	
	
	TSoundDeviceGlobals();
};

extern TSoundDeviceGlobals g_SoundDeviceGlobals;

#endif // sound_device_globals_h
