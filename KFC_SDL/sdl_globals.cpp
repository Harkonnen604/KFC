#include "kfc_sdl_pch.h"
#include "sdl_globals.h"

#include <KFC_KTL/ktl_globals.h>
#include <KFC_Common/common_globals.h>
#include <KFC_Image/image_globals.h>
#include "sdl_device_globals.h"

T_SDL_Globals g_SDL_Globals;

// ------------
// SDL globals
// ------------
T_SDL_Globals::T_SDL_Globals() : TModuleGlobals(TEXT("SDL globals"))
{
	AddSubGlobals(g_KTL_Globals);
	AddSubGlobals(g_CommonGlobals);
	AddSubGlobals(g_ImageGlobals);
	AddSubGlobals(g_SDL_DeviceGlobals);
}
