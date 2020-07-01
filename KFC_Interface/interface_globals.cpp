#include "kfc_interface_pch.h"
#include "interface_globals.h"

#include <KFC_KTL\ktl_globals.h>
#include <KFC_Common\common_globals.h>
#include <KFC_GUI\gui_globals.h>
#include <KFC_Windows\windows_globals.h>
#include <KFC_Input\input_globals.h>
#include <KFC_Graphics\graphics_globals.h>
#include <KFC_Sound\sound_globals.h>
#include "interface_cfg.h"
#include "interface_initials.h"
#include "interface_tokens.h"
#include "controls_factory.h"
#include "interface_device_globals.h"

TInterfaceGlobals g_InterfaceGlobals;

// ------------------
// Interface globals
// ------------------
TInterfaceGlobals::TInterfaceGlobals() : TModuleGlobals(TEXT("Interface globals"))
{
	AddSubGlobals(g_KTLGlobals);
	AddSubGlobals(g_CommonGlobals);
	AddSubGlobals(g_GUIGlobals);
	AddSubGlobals(g_WindowsGlobals);
	AddSubGlobals(g_GraphicsGlobals);
	AddSubGlobals(g_SoundGlobals);
	AddSubGlobals(g_InputGlobals);
	AddSubGlobals(g_InterfaceCfg);
	AddSubGlobals(g_InterfaceInitials);
	AddSubGlobals(g_InterfaceTokens);
	AddSubGlobals(g_ControlsFactory);
	AddSubGlobals(g_InterfaceDeviceGlobals);
}
