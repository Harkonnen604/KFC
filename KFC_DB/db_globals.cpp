#include "kfc_db_pch.h"
#include "db_globals.h"

#include <KFC_KTL\ktl_globals.h>
#include <KFC_Common\common_globals.h>
#include "db_device_globals.h"

T_DB_Globals g_DB_Globals;

// -----------
// DB globals
// -----------
T_DB_Globals::T_DB_Globals() : TModuleGlobals(TEXT("DB globals"))
{
    AddSubGlobals(g_KTL_Globals);
    AddSubGlobals(g_CommonGlobals);
    AddSubGlobals(g_DB_DeviceGlobals);
}
