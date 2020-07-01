#include "kfc_sound_pch.h"
#include "sound_globals.h"

#include <KFC_KTL\ktl_globals.h>
#include <KFC_Common\common_globals.h>
#include <KFC_Windows\windows_globals.h>
#include "sound_cfg.h"
#include "sound_initials.h"
#include "sound_tokens.h"
#include "sound_storage.h"
#include "sound_device_globals.h"

TSoundGlobals g_SoundGlobals;

// --------------
// Sound globals
// --------------
TSoundGlobals::TSoundGlobals() : TModuleGlobals(TEXT("Sound globals"))
{
	AddSubGlobals(g_KTLGlobals);
	AddSubGlobals(g_CommonGlobals);
	AddSubGlobals(g_WindowsGlobals);
	AddSubGlobals(g_SoundCfg);
	AddSubGlobals(g_SoundInitials);
	AddSubGlobals(g_SoundTokens);
	AddSubGlobals(g_SoundDeviceGlobals);
	AddSubGlobals(g_SoundStorage);
}
