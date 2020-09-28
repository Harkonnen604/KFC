#include "kfc_physics_pch.h"
#include "physics_device_globals.h"

#include "physics_cfg.h"
#include "physics_initials.h"

TPhysicsDeviceGlobals g_PhysicsDeviceGlobals;

// -----------------------
// Physics device globals
// -----------------------
TPhysicsDeviceGlobals::TPhysicsDeviceGlobals() : TGlobals(TEXT("Physics device globals"))
{
    AddSubGlobals(g_PhysicsCfg);
    AddSubGlobals(g_PhysicsInitials);
}

void TPhysicsDeviceGlobals::OnUninitialize()
{
}

void TPhysicsDeviceGlobals::OnInitialize()
{
}
