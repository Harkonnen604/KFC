#include "kfc_protocols_pch.h"
#include "protocols_globals.h"

#include <KFC_KTL/ktl_globals.h>
#include <KFC_Common/common_globals.h>
#include <KFC_Formats/formats_globals.h>
#include <KFC_Networking/networking_globals.h>
#include "protocols_device_globals.h"

TProtocolsGlobals g_ProtocolsGlobals;

// ------------------
// Protocols globals
// ------------------
TProtocolsGlobals::TProtocolsGlobals() : TModuleGlobals(TEXT("Protocols globals"))
{
	AddSubGlobals(g_KTL_Globals);
	AddSubGlobals(g_CommonGlobals);
	AddSubGlobals(g_FormatsGlobals);
	AddSubGlobals(g_NetworkingGlobals);
	AddSubGlobals(g_ProtocolsDeviceGlobals);
}
