#include "kfc_math_pch.h"
#include "math_device_globals.h"

TMathDeviceGlobals g_MathDeviceGlobals;

// --------------------
// Math device globals
// --------------------
TMathDeviceGlobals::TMathDeviceGlobals() : TGlobals(TEXT("Math device globals"))
{
}

void TMathDeviceGlobals::OnUninitialize()
{
}

void TMathDeviceGlobals::OnInitialize()
{
}
