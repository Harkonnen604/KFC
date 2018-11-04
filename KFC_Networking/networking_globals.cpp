#include "kfc_networking_pch.h"
#include "networking_globals.h"

#include <KFC_KTL/ktl_globals.h>
#include <KFC_Common/common_globals.h>
#include "networking_device_globals.h"

TNetworkingGlobals g_NetworkingGlobals;

// -------------------
// Networking globals
// -------------------
TNetworkingGlobals::TNetworkingGlobals() : TModuleGlobals(TEXT("Networking globals"))
{
	AddSubGlobals(g_KTL_Globals);
	AddSubGlobals(g_CommonGlobals);
	AddSubGlobals(g_NetworkingDeviceGlobals);
}
