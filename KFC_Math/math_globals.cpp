#include "kfc_math_pch.h"
#include "math_globals.h"

#include <KFC_KTL/ktl_globals.h>
#include "math_device_globals.h"

TMathGlobals g_MathGlobals;

// -------------
// Math globals
// -------------
TMathGlobals::TMathGlobals() : TModuleGlobals(TEXT("Math globals"))
{
    AddSubGlobals(g_KTL_Globals);

    AddSubGlobals(g_MathDeviceGlobals);
}
