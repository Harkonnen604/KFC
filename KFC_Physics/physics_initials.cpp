#include "kfc_physics_pch.h"
#include "physics_initials.h"

#include <KFC_Common\common_consts.h>
#include "physics_consts.h"

TPhysicsInitials g_PhysicsInitials;

// -----------------
// Physics initials
// -----------------
TPhysicsInitials::TPhysicsInitials() : TGlobals(TEXT("Physics initials"))
{
}

void TPhysicsInitials::OnUninitialize()
{
}

void TPhysicsInitials::OnInitialize()
{
	Load(), Save();
}

void TPhysicsInitials::LoadItems(KRegistryKey& Key)
{
}

void TPhysicsInitials::SaveItems(KRegistryKey& Key) const
{
}

void TPhysicsInitials::Load()
{
	TAssignmentsList::Load(	g_CommonConsts.m_ApplicationRegistryKeyName +
							g_PhysicsConsts.m_RegistryKeyName +
							g_CommonConsts.m_InitialsRegistryKeyName);
}

void TPhysicsInitials::Save() const
{
	TAssignmentsList::Save(	g_CommonConsts.m_ApplicationRegistryKeyName +
							g_PhysicsConsts.m_RegistryKeyName +
							g_CommonConsts.m_InitialsRegistryKeyName);
}