#include "kfc_ktl_pch.h"
#include "ktl_globals.h"

#include "ktl_device_globals.h"
#include "auto_holder.h"

T_KTL_Globals g_KTL_Globals;

// ------------
// KTL globals
// ------------
T_KTL_Globals::T_KTL_Globals() : TModuleGlobals(TEXT("KTL globals"))
{
	AddSubGlobals(g_KTL_DeviceGlobals);
	AddSubGlobals(g_AutoHolderGlobals);
}
