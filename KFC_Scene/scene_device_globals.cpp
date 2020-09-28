#include "kfc_scene_pch.h"
#include "scene_device_globals.h"

#include "scene_cfg.h"
#include "scene_initials.h"

TSceneDeviceGlobals g_SceneDeviceGlobals;

// -----------------------
// Scene device globals
// -----------------------
TSceneDeviceGlobals::TSceneDeviceGlobals() : TGlobals(TEXT("Scene device globals"))
{
    AddSubGlobals(g_SceneCfg);
    AddSubGlobals(g_SceneInitials);
}

void TSceneDeviceGlobals::OnUninitialize()
{
}

void TSceneDeviceGlobals::OnInitialize()
{
}
