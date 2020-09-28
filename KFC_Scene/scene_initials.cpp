#include "kfc_scene_pch.h"
#include "scene_initials.h"

#include <KFC_Common\common_consts.h>
#include "scene_consts.h"

TSceneInitials g_SceneInitials;

// ---------------
// Scene initials
// ---------------
TSceneInitials::TSceneInitials() : TGlobals(TEXT("Scene initials"))
{
}

void TSceneInitials::OnUninitialize()
{
}

void TSceneInitials::OnInitialize()
{
    if(!g_CommonConsts.m_bSkipBasicCfgInitials)
        Load(), Save();
}

void TSceneInitials::LoadItems(KRegistryKey& Key)
{
}

void TSceneInitials::SaveItems(KRegistryKey& Key) const
{
}

void TSceneInitials::Load()
{
    TAssignmentsList::Load( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_SceneConsts.m_RegistryKeyName +
                            g_CommonConsts.m_InitialsRegistryKeyName);
}

void TSceneInitials::Save() const
{
    TAssignmentsList::Save( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_SceneConsts.m_RegistryKeyName +
                            g_CommonConsts.m_InitialsRegistryKeyName);
}
