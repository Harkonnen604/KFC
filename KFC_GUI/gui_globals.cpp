#include "kfc_gui_pch.h"
#include "gui_globals.h"

#include <KFC_KTL\ktl_globals.h>
#include <KFC_Common\common_globals.h>
#include "gui_cfg.h"
#include "gui_initials.h"
#include "gui_tokens.h"
#include "gui_device_globals.h"
#include "gui_font_storage.h"
#include "window_subclasser.h"
#include "edit_filter.h"

T_GUI_Globals g_GUI_Globals;

// ------------
// GUI globals
// ------------
T_GUI_Globals::T_GUI_Globals() : TModuleGlobals(TEXT("GUI globals"))
{
    AddSubGlobals(g_KTL_Globals);
    AddSubGlobals(g_CommonGlobals);
    AddSubGlobals(g_GUI_Cfg);
    AddSubGlobals(g_GUI_Initials);
    AddSubGlobals(g_GUI_Tokens);
    AddSubGlobals(g_GUI_DeviceGlobals);
    AddSubGlobals(g_GUI_FontStorage);
    AddSubGlobals(g_WindowSubclasserGlobals);
    AddSubGlobals(g_EditFilterGlobals);
}
