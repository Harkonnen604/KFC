#include "kfc_sound_pch.h"
#include "sound_cfg.h"

#include <KFC_Common\common_consts.h>
#include "sound_consts.h"

TSoundCfg g_SoundCfg;

// ----------
// Sound cfg
// ----------
TSoundCfg::TSoundCfg() : TGlobals(TEXT("Sound cfg"))
{
}

void TSoundCfg::OnUninitialize()
{
}

void TSoundCfg::OnInitialize()
{
	Load(), Save();
}

void TSoundCfg::LoadItems(KRegistryKey& Key)
{
	Key.ReadUINT(TEXT("NChannels"),	m_szNChannels,	2);
	Key.ReadUINT(TEXT("Frequency"),	m_szFrequency,	44100);
	Key.ReadUINT(TEXT("BPS"),		m_szBPS,		16);
}

void TSoundCfg::SaveItems(KRegistryKey& Key) const
{
	Key.WriteUINT(TEXT("NChannels"),	m_szNChannels);
	Key.WriteUINT(TEXT("Frequency"),	m_szFrequency);
	Key.WriteUINT(TEXT("BPS"),			m_szBPS);
}

void TSoundCfg::Load()
{
	TAssignmentsList::Load(	g_CommonConsts.m_ApplicationRegistryKeyName +
							g_SoundConsts.m_RegistryKeyName +
							g_CommonConsts.m_CfgRegistryKeyName);	
}

void TSoundCfg::Save() const
{
	TAssignmentsList::Save(	g_CommonConsts.m_ApplicationRegistryKeyName +
							g_SoundConsts.m_RegistryKeyName +
							g_CommonConsts.m_CfgRegistryKeyName);
}
