#include "kfc_image_pch.h"
#include "image_globals.h"

#include <KFC_KTL/ktl_globals.h>
#include <KFC_Common/common_globals.h>

TImageGlobals g_ImageGlobals;

// --------------
// Image globals
// --------------
TImageGlobals::TImageGlobals() : TModuleGlobals(TEXT("Image globals"))
{
	AddSubGlobals(g_KTL_Globals);
	AddSubGlobals(g_CommonGlobals);
}
