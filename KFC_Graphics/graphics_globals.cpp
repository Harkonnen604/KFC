#include "kfc_graphics_pch.h"
#include "graphics_globals.h"

#include <KFC_KTL\ktl_globals.h>
#include <KFC_Math\math_globals.h>
#include <KFC_Common\common_globals.h>
#include <KFC_GUI\gui_globals.h>
#include <KFC_Windows\windows_globals.h>
#include "graphics_cfg.h"
#include "graphics_initials.h"
#include "graphics_tokens.h"
#include "graphics_device_globals.h"
#include "graphics_state_manager.h"
#include "graphics_effect_globals.h"
#include "sprite_storage.h"
#include "font_storage.h"

TGraphicsGlobals g_GraphicsGlobals;

// -----------------
// Graphics globals
// -----------------
TGraphicsGlobals::TGraphicsGlobals() : TModuleGlobals(TEXT("Graphics globals"))
{
    AddSubGlobals(g_KTLGlobals);
    AddSubGlobals(g_MathGlobals);
    AddSubGlobals(g_CommonGlobals);
    AddSubGlobals(g_GUIGlobals);
    AddSubGlobals(g_WindowsGlobals);

    AddSubGlobals(g_GraphicsCfg);
    AddSubGlobals(g_GraphicsInitials);
    AddSubGlobals(g_GraphicsTokens);
    AddSubGlobals(g_GraphicsDeviceGlobals);
    AddSubGlobals(g_GraphicsStateManager);
    AddSubGlobals(g_GraphicsEffectGlobals);
    AddSubGlobals(g_FontStorage);
    AddSubGlobals(g_SpriteStorage);
}
