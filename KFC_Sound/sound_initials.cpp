#include "kfc_sound_pch.h"
#include "sound_initials.h"

#include <KFC_Common\common_consts.h>
#include "sound_consts.h"

TSoundInitials g_SoundInitials;

// ---------------
// Sound initials
// ---------------
TSoundInitials::TSoundInitials() : TGlobals(TEXT("Sound initials"))
{
}

void TSoundInitials::OnUninitialize()
{
}

void TSoundInitials::OnInitialize()
{
	Load(), Save();
}

void TSoundInitials::LoadItems(KRegistryKey& Key)
{
}

void TSoundInitials::SaveItems(KRegistryKey& Key) const
{
}

void TSoundInitials::Load()
{
	TAssignmentsList::Load(	g_CommonConsts.m_ApplicationRegistryKeyName +
							g_SoundConsts.m_RegistryKeyName +
							g_CommonConsts.m_InitialsRegistryKeyName);
}

void TSoundInitials::Save() const
{
	TAssignmentsList::Save(	g_CommonConsts.m_ApplicationRegistryKeyName +
							g_SoundConsts.m_RegistryKeyName +
							g_CommonConsts.m_InitialsRegistryKeyName);
}
