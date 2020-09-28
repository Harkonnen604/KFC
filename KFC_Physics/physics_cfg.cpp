#include "kfc_physics_pch.h"
#include "physics_cfg.h"

#include <KFC_Common\common_consts.h>
#include "physics_consts.h"

TPhysicsCfg g_PhysicsCfg;

// ------------
// Physics cfg
// ------------
TPhysicsCfg::TPhysicsCfg() : TGlobals(TEXT("Physics cfg"))
{
}

void TPhysicsCfg::OnUninitialize()
{
}

void TPhysicsCfg::OnInitialize()
{
    Load(), Save();
}

void TPhysicsCfg::LoadItems(KRegistryKey& Key)
{
    Key.ReadDouble(TEXT("InitialTickTime"), m_dInitialTickTime, 1.0 / 60); // 60 physical FPS

    Key.ReadUINT(TEXT("MaxTimeSubdivision"), m_szMaxTimeSubdivision, 4);

    if(Sign(m_dInitialTickTime) <= 0)
        INITIATE_DEFINED_FAILURE(TEXT("InitialTickTime time must be postiive."));
}

void TPhysicsCfg::SaveItems(KRegistryKey& Key) const
{
    Key.WriteDouble(TEXT("InitialTickTime"), m_dInitialTickTime);

    Key.WriteUINT(TEXT("MaxTimeSubdivision"), m_szMaxTimeSubdivision);
}

void TPhysicsCfg::Load()
{
    TAssignmentsList::Load( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_PhysicsConsts.m_RegistryKeyName +
                            g_CommonConsts.m_CfgRegistryKeyName);
}

void TPhysicsCfg::Save() const
{
    TAssignmentsList::Save( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_PhysicsConsts.m_RegistryKeyName +
                            g_CommonConsts.m_CfgRegistryKeyName);
}
