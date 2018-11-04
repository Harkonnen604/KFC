#include "kfc_formats_pch.h"
#include "formats_globals.h"

#include <KFC_KTL/ktl_globals.h>
#include "encoding_globals.h"

TFormatsGlobals g_FormatsGlobals;

// ----------------
// Formats globals
// ----------------
TFormatsGlobals::TFormatsGlobals() : TModuleGlobals(TEXT("Formats globals"))
{
	AddSubGlobals(g_KTL_Globals);
	AddSubGlobals(g_EncodingGlobals);
}
