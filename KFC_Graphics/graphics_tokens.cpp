#include "kfc_graphics_pch.h"
#include "graphics_tokens.h"

#include <KFC_Common\common_tokens.h>
#include "graphics_consts.h"
#include "graphics_cfg.h"
#include "graphics_initials.h"
#include "d3d_color.h"
#include "color_defs.h"
#include "sprite_defs.h"
#include "font_defs.h"

TGraphicsTokens g_GraphicsTokens;

// ----------------
// Graphics tokens
// ----------------
TGraphicsTokens::TGraphicsTokens() : TGlobals(TEXT("Graphics tokens"))
{
	AddSubGlobals(g_GraphicsCfg);
	AddSubGlobals(g_GraphicsInitials);
}

void TGraphicsTokens::OnUninitialize()
{
	m_SpriteIndexTokensRegisterer.	Release();
	m_SpriteTypeTokensRegisterer.	Release();
	m_FontIndexTokensRegisterer.	Release();
	m_FontTypeTokensRegisterer.		Release();	
	m_ColorTokensRegisterer.		Release();
	m_FileNameTokensRegisterer.		Release();

	m_SpriteIndexTokens.Clear();
	m_SpriteTypeTokens.	Clear();
	m_FontIndexTokens.	Clear();
	m_FontTypeTokens.	Clear();
	m_ColorTokens.		Clear();
}

void TGraphicsTokens::OnInitialize()
{
	// --- Filename tokens ---
	m_FileNameTokensRegisterer.Allocate(FILENAME_TOKENS);

	m_FileNameTokensRegisterer.Add(TEXT("[SpritesFolder]"),	g_GraphicsConsts.m_SpritesFolderName);
	m_FileNameTokensRegisterer.Add(TEXT("[FontsFolder]"),	g_GraphicsConsts.m_FontsFolderName);

	// --- Color tokens ---
	m_ColorTokensRegisterer.Allocate(COLOR_TOKENS);

	m_ColorTokensRegisterer.Add(TEXT("[Black]"),	TD3DColor(BlackColor	()));
	m_ColorTokensRegisterer.Add(TEXT("[Blue]"),		TD3DColor(BlueColor		()));
	m_ColorTokensRegisterer.Add(TEXT("[Green]"),	TD3DColor(GreenColor	()));
	m_ColorTokensRegisterer.Add(TEXT("[Red]"),		TD3DColor(RedColor		()));
	m_ColorTokensRegisterer.Add(TEXT("[Cyan]"),		TD3DColor(CyanColor		()));
	m_ColorTokensRegisterer.Add(TEXT("[Magenta]"),	TD3DColor(MagentaColor	()));
	m_ColorTokensRegisterer.Add(TEXT("[Yellow]"),	TD3DColor(YellowColor	()));
	m_ColorTokensRegisterer.Add(TEXT("[White]"),	TD3DColor(WhiteColor	()));
	m_ColorTokensRegisterer.Add(TEXT("[Gray]"),		TD3DColor(GrayColor		()));

	// --- Font type tokens ---
	m_FontTypeTokensRegisterer.Allocate(FONT_TYPE_TOKENS);

	m_FontTypeTokensRegisterer.Add(TEXT("[Basic]"),			OBJECT_TYPE_BASIC);
	m_FontTypeTokensRegisterer.Add(TEXT("[Flat]"),			FONT_TYPE_FLAT);
	m_FontTypeTokensRegisterer.Add(TEXT("[FlatShadow]"),	FONT_TYPE_FLAT_SHADOW);

	// --- Font index tokens ---
	m_FontIndexTokensRegisterer.Allocate(FONT_INDEX_TOKENS);

	m_FontIndexTokensRegisterer.Add(TEXT("[Default]"), DEFAULT_FONT_INDEX);

	// --- Sprite types tokens ---
	m_SpriteTypeTokensRegisterer.Allocate(SPRITE_TYPE_TOKENS);

	m_SpriteTypeTokensRegisterer.Add(TEXT("[Basic]"),			OBJECT_TYPE_BASIC);
	m_SpriteTypeTokensRegisterer.Add(TEXT("[Flat]"),			SPRITE_TYPE_FLAT);
	m_SpriteTypeTokensRegisterer.Add(TEXT("[Transition]"),		SPRITE_TYPE_TRANSITION);
	m_SpriteTypeTokensRegisterer.Add(TEXT("[Composite]"),		SPRITE_TYPE_COMPOSITE);
	m_SpriteTypeTokensRegisterer.Add(TEXT("[Border]"),			SPRITE_TYPE_BORDER);
	m_SpriteTypeTokensRegisterer.Add(TEXT("[RectEffect]"),		SPRITE_TYPE_RECT_EFFECT);
	m_SpriteTypeTokensRegisterer.Add(TEXT("[Text]"),			SPRITE_TYPE_TEXT);
	m_SpriteTypeTokensRegisterer.Add(TEXT("[Dummy]"),			SPRITE_TYPE_DUMMY);
	m_SpriteTypeTokensRegisterer.Add(TEXT("[StateSkipper]"),	SPRITE_TYPE_STATE_SKIPPER);

	// --- Sprite index tokens
	m_SpriteIndexTokensRegisterer.Allocate(SPRITE_INDEX_TOKENS);	
}
