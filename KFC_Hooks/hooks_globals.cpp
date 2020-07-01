#include "kfc_hooks_pch.h"
#include "hooks_globals.h"

#include <KFC_KTL\ktl_globals.h>

THooksGlobals g_HooksGlobals;

// --------------
// Hooks globals
// --------------
THooksGlobals::THooksGlobals() : TModuleGlobals(TEXT("Hooks globals"))
{
	AddSubGlobals(g_KTLGlobals);
}