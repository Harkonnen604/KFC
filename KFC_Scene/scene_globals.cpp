#include "kfc_scene_pch.h"
#include "scene_globals.h"

#include <KFC_KTL\ktl_globals.h>
#include <KFC_Math\math_globals.h>
#include <KFC_Common\common_globals.h>
#include <KFC_Input\input_globals.h>
#include <KFC_Graphics\graphics_globals.h>
#include <KFC_Sound\sound_globals.h>
#include <KFC_Physics\physics_globals.h>
#include "scene_cfg.h"
#include "scene_initials.h"
#include "scene_device_globals.h"

TSceneGlobals g_SceneGlobals;

// ----------------
// Scene globals
// ----------------
TSceneGlobals::TSceneGlobals() : TModuleGlobals(TEXT("Scene globals"))
{
    AddSubGlobals(g_KTLGlobals);
    AddSubGlobals(g_MathGlobals);
    AddSubGlobals(g_CommonGlobals);
    AddSubGlobals(g_InputGlobals);
    AddSubGlobals(g_GraphicsGlobals);
    AddSubGlobals(g_SoundGlobals);
    AddSubGlobals(g_PhysicsGlobals);

    AddSubGlobals(g_SceneCfg);
    AddSubGlobals(g_SceneInitials);
    AddSubGlobals(g_SceneDeviceGlobals);
}
