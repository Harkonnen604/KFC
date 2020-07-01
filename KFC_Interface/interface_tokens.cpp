#include "kfc_interface_pch.h"
#include "interface_tokens.h"

#include <KFC_Common\common_tokens.h>
#include <KFC_Graphics\graphics_tokens.h>
#include <KFC_Sound\sound_tokens.h>
#include "interface_consts.h"
#include "interface_cfg.h"
#include "interface_initials.h"
#include "interface_sprite_defs.h"
#include "interface_font_defs.h"
#include "interface_sound_defs.h"
#include "control_defs.h"

TInterfaceTokens g_InterfaceTokens;

// -----------------
// Interface tokens
// -----------------
TInterfaceTokens::TInterfaceTokens() : TGlobals(TEXT("Interace tokens"))
{
	AddSubGlobals(g_InterfaceCfg);
	AddSubGlobals(g_InterfaceInitials);
}

void TInterfaceTokens::OnUninitialize()
{
	m_ControlIDTokensRegisterer.	Release();
	m_ControlTypeTokensRegisterer.	Release();
	m_SoundIndexTokensRegisterer.	Release();
	m_FontIndexTokensRegisterer.	Release();	
	m_SpriteIndexTokensRegisterer.	Release();
	m_FileNameTokensRegisterer.		Release();

	m_ControlIDTokens.	Clear();
	m_ControlTypeTokens.Clear();
}

void TInterfaceTokens::OnInitialize()
{
	size_t i;

	// --- Filename tokens ---
	m_FileNameTokensRegisterer.Allocate(FILENAME_TOKENS);

	m_FileNameTokensRegisterer.Add(TEXT("[InterfacesFolder]"), g_InterfaceConsts.m_InterfacesFolderName);

	// --- Sprite index tokens ---
	m_SpriteIndexTokensRegisterer.Allocate(SPRITE_INDEX_TOKENS);

	m_SpriteIndexTokensRegisterer.Add(TEXT("[MousePointer]"), MOUSE_POINTER_SPRITE_INDEX);

	for(i = 0 ; i < 4 ; i++)
	{
		m_SpriteIndexTokensRegisterer.Add(	(KString)TEXT("[HotPointer") + i + TEXT("]"),
											HOT_POINTER_SPRITES_START_INDEX + i);
	}

	m_SpriteIndexTokensRegisterer.Add(TEXT("[CheckBox]"), CHECKBOX_SPRITE_INDEX);

	m_SpriteIndexTokensRegisterer.Add(TEXT("[Ok]"),		OK_SPRITE_INDEX);
	m_SpriteIndexTokensRegisterer.Add(TEXT("[Cancel]"),	CANCEL_SPRITE_INDEX);

	// --- Font index tokens ---
	m_FontIndexTokensRegisterer.Allocate(FONT_INDEX_TOKENS);

	m_FontIndexTokensRegisterer.Add(TEXT("[HotMessage]"),	HOT_MESSAGE_FONT_INDEX);
	m_FontIndexTokensRegisterer.Add(TEXT("[DefaultLabel"),	DEFAULT_LABEL_FONT_INDEX);

	// --- Sound index tokens ---
	m_SoundIndexTokensRegisterer.Allocate(SOUND_INDEX_TOKENS);
	
	m_SoundIndexTokensRegisterer.Add(TEXT("[HotMessage]"),	HOT_MESSAGE_SOUND_INDEX);
	m_SoundIndexTokensRegisterer.Add(TEXT("[Push]"),		PUSH_SOUND_INDEX);
	m_SoundIndexTokensRegisterer.Add(TEXT("[Click]"),		CLICK_SOUND_INDEX);

	// --- Control type tokens ---
	m_ControlTypeTokensRegisterer.Allocate(CONTROL_TYPE_TOKENS);

	m_ControlTypeTokensRegisterer.Add(TEXT("[Dummy]"),			CONTROL_TYPE_DUMMY);
	m_ControlTypeTokensRegisterer.Add(TEXT("[Image]"),			CONTROL_TYPE_IMAGE);
	m_ControlTypeTokensRegisterer.Add(TEXT("[Button]"),			CONTROL_TYPE_BUTTON);
	m_ControlTypeTokensRegisterer.Add(TEXT("[CheckBox]"),		CONTROL_TYPE_CHECKBOX);
	m_ControlTypeTokensRegisterer.Add(TEXT("[Scroll]"),			CONTROL_TYPE_SCROLL);
	m_ControlTypeTokensRegisterer.Add(TEXT("[EasyLabel]"),		CONTROL_TYPE_EASY_LABEL);
	m_ControlTypeTokensRegisterer.Add(TEXT("[EasyCheckBox]"),	CONTROL_TYPE_EASY_CHECKBOX);

	// --- Control ID tokens ---
	m_ControlIDTokensRegisterer.Allocate(CONTROL_ID_TOKENS);

	m_ControlIDTokensRegisterer.Add(TEXT("[None]"), CONTROL_ID_NONE);
}
