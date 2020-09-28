#include "kfc_cgi_pch.h"
#include "cgi_globals.h"

#include <KFC_KTL/ktl_globals.h>
#include <KFC_Common/common_globals.h>
#include <KFC_Formats/formats_globals.h>
#include "cgi_device_globals.h"

T_CGI_Globals g_CGI_Globals;

// ------------
// CGI globals
// ------------
T_CGI_Globals::T_CGI_Globals() : TModuleGlobals(TEXT("CGI globals"))
{
    AddSubGlobals(g_KTL_Globals);
    AddSubGlobals(g_CommonGlobals);
    AddSubGlobals(g_FormatsGlobals);
    AddSubGlobals(g_CGI_DeviceGlobals);
}
