#include "kfc_physics_pch.h"
#include "physics_globals.h"

#include <KFC_KTL\ktl_globals.h>
#include <KFC_Math\math_globals.h>
#include <KFC_Common\common_globals.h>
#include <KFC_Windows\windows_globals.h>
#include "physics_cfg.h"
#include "physics_initials.h"
#include "physics_device_globals.h"

TPhysicsGlobals g_PhysicsGlobals;

// ----------------
// Physics globals
// ----------------
TPhysicsGlobals::TPhysicsGlobals() : TModuleGlobals(TEXT("Physics globals"))
{
	AddSubGlobals(g_KTLGlobals);
	AddSubGlobals(g_MathGlobals);
	AddSubGlobals(g_CommonGlobals);
	AddSubGlobals(g_WindowsGlobals);

	AddSubGlobals(g_PhysicsCfg);
	AddSubGlobals(g_PhysicsInitials);
	AddSubGlobals(g_PhysicsDeviceGlobals);
}