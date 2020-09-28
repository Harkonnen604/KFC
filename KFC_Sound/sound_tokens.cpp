#include "kfc_sound_pch.h"
#include "sound_tokens.h"

#include <KFC_Common\common_tokens.h>
#include "sound_consts.h"
#include "sound_cfg.h"
#include "sound_initials.h"
#include "sound_defs.h"

TSoundTokens g_SoundTokens;

// --------------------
// Sound token s
// --------------------
TSoundTokens::TSoundTokens() : TGlobals(TEXT("Sound tokens"))
{
    AddSubGlobals(g_SoundCfg);
    AddSubGlobals(g_SoundInitials);
}

void TSoundTokens::OnUninitialize()
{
    m_SoundIndexTokensRegisterer.   Release();
    m_SoundTypeTokensRegisterer.    Release();
    m_FileNameTokensRegisterer.     Release();

    m_SoundIndexTokens. Clear();
    m_SoundTypeTokens.  Clear();
}

void TSoundTokens::OnInitialize()
{
    // --- Filename tokens ---
    m_FileNameTokensRegisterer.Allocate(FILENAME_TOKENS);

    m_FileNameTokensRegisterer.Add(TEXT("[SoundsFolder]"), g_SoundConsts.m_SoundsFolderName);

    // --- Sound type tokens ---
    m_SoundTypeTokensRegisterer.Allocate(SOUND_TYPE_TOKENS);

    m_SoundTypeTokensRegisterer.Add(TEXT("[Basic]"), OBJECT_TYPE_BASIC);
    m_SoundTypeTokensRegisterer.Add(TEXT("[Plain]"), SOUND_TYPE_PLAIN);

    // --- Sound index tokens ---
    m_SoundIndexTokensRegisterer.Allocate(SOUND_INDEX_TOKENS);
}
