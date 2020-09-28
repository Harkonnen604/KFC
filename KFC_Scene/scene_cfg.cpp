#include "kfc_scene_pch.h"
#include "scene_cfg.h"

#include <KFC_Common\common_consts.h>
#include "scene_consts.h"

TSceneCfg g_SceneCfg;

// ----------
// Scene cfg
// ----------
TSceneCfg::TSceneCfg() : TGlobals(TEXT("Scene cfg"))
{
}

void TSceneCfg::OnUninitialize()
{
}

void TSceneCfg::OnInitialize()
{
    if(!g_CommonConsts.m_bSkipBasicCfgInitials)
        Load(), Save();
}

void TSceneCfg::LoadItems(KRegistryKey& Key)
{
}

void TSceneCfg::SaveItems(KRegistryKey& Key) const
{
}

void TSceneCfg::Load()
{
    TAssignmentsList::Load( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_SceneConsts.m_RegistryKeyName +
                            g_CommonConsts.m_CfgRegistryKeyName);
}

void TSceneCfg::Save() const
{
    TAssignmentsList::Save( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_SceneConsts.m_RegistryKeyName +
                            g_CommonConsts.m_CfgRegistryKeyName);
}
