#include "kfc_common_pch.h"
#include "common_globals.h"

#include <KFC_KTL/ktl_globals.h>
#include "startup_globals.h"
#include "common_cfg.h"
#include "common_initials.h"
#include "common_tokens.h"
#include "common_device_globals.h"
#include "time_globals.h"
#include "random_globals.h"

TCommonGlobals g_CommonGlobals;

// ---------------
// Common globals
// ---------------
TCommonGlobals::TCommonGlobals() : TModuleGlobals(TEXT("Common globals"))
{
	AddSubGlobals(g_KTL_Globals);
	AddSubGlobals(g_StartupGlobals);
	
	#ifdef _MSC_VER
	{
		AddSubGlobals(g_CommonCfg);
		AddSubGlobals(g_CommonInitials);
	}
	#endif // _MSC_VER
	
	AddSubGlobals(g_CommonTokens);
	AddSubGlobals(g_CommonDeviceGlobals);
	AddSubGlobals(g_TimeGlobals);
	AddSubGlobals(g_RandomGlobals);
}
