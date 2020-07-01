#include "kfc_ssl_pch.h"
#include "ssl_globals.h"

#include <KFC_KTL\ktl_globals.h>
#include <KFC_Common\common_globals.h>
#include <KFC_Networking\networking_globals.h>
#include "ssl_device_globals.h"

T_SSL_Globals g_SSL_Globals;

// ------------
// SSL globals
// ------------
T_SSL_Globals::T_SSL_Globals() : TModuleGlobals(TEXT("SSL globals"))
{
	AddSubGlobals(g_KTL_Globals);
	AddSubGlobals(g_CommonGlobals);
	AddSubGlobals(g_NetworkingGlobals);
	AddSubGlobals(g_SSL_DeviceGlobals);
}