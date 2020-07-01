#include "kfc_interface_pch.h"
#include "checkbox_control.h"

#include <KFC_Graphics\graphics_device_globals.h>
#include <KFC_Graphics\font_storage.h>
#include <KFC_Graphics\sprite_storage.h>
#include <KFC_Graphics\sprite_helpers.h>
#include <KFC_Graphics\composite_sprite.h>
#include <KFC_Graphics\transition_sprite.h>
#include <KFC_Sound\sound_storage.h>
#include "interface_consts.h"
#include "interface_font_defs.h"
#include "interface_sprite_defs.h"
#include "interface.h"

// ---------------------------------
// Checkbox control creation struct
// ---------------------------------
TCheckBoxControlCreationStruct::TCheckBoxControlCreationStruct()
{
	m_bChecked = false;

	m_bPushClickCheckToggling = false;

	m_fCheckTransitionDelay =
		g_InterfaceConsts.m_fDefaultCheckBoxControlCheckTransitionDelay;
}

void TCheckBoxControlCreationStruct::Load(	TInfoNodeConstIterator	InfoNode,
											const TControl*			pParentControl,
											const FRECT&			Resolution)
{
	TInteractiveControlCreationStruct::Load(InfoNode, pParentControl, Resolution);

	TInfoParameterConstIterator PIter;

	// Getting check state
	if((PIter = InfoNode->FindParameter(TEXT("Checked"))).IsValid())
	{
		ReadBool(	PIter->m_Value,
					m_bChecked,
					TEXT("checkbox check state"));
	}

	// Getting push click check toggling
	if((PIter = InfoNode->FindParameter(TEXT("PushClickCheckToggling"))).IsValid())
	{
		ReadBool(	PIter->m_Value,
					m_bPushClickCheckToggling,
					TEXT("push click checkbox check state toggling"));
	}

	// Getting check transition delay
	if(InfoNode->HasTrueParameter(TEXT("ImmediateCheckTransition")))
	{
		SetImmediateCheckTransition();		
	}
	else
	{
		if((PIter = InfoNode->FindParameter(TEXT("CheckTransitionDelay"))).IsValid())
		{
			ReadFloat(	PIter->m_Value,
						m_fCheckTransitionDelay,
						TEXT("check transition delay"));
		}
	}
}

void TCheckBoxControlCreationStruct::SetImmediateCheckTransition()
{
	m_fCheckTransitionDelay = IMMEDIATE_TIME;
}

bool TCheckBoxControlCreationStruct::GetCheck(bool* pRSuccess) const
{
	if(pRSuccess)
		*pRSuccess = true;

	return m_bChecked;
}

bool TCheckBoxControlCreationStruct::SetCheck(bool bCheck)
{
	m_bChecked = bCheck;

	return true;
}

// ----------------------------------
// Checkbox control sprites provider
// ----------------------------------
TCheckBoxControlSpritesProvider::TCheckBoxControlSpritesProvider()
{
	m_Sprite.Allocate(SPRITES_REGISTRATION_MANAGER[CHECKBOX_SPRITE_INDEX], true);
}

void TCheckBoxControlSpritesProvider::Load(TInfoNodeConstIterator InfoNode)
{
	g_SpriteStorage.LoadObject(InfoNode, TEXT("Sprite"), m_Sprite, true);
}

KString TCheckBoxControlSpritesProvider::GetText(bool* pRSuccess) const
{
	return GetSpriteText(m_Sprite.GetDataPtr(), pRSuccess);
}

bool TCheckBoxControlSpritesProvider::SetText(const KString& Text)
{
	return SetSpriteText(m_Sprite.GetDataPtr(), Text);
}

// ---------------------------------
// Checkbox control sounds provider
// ---------------------------------
TCheckBoxControlSoundsProvider::TCheckBoxControlSoundsProvider()
{
}

void TCheckBoxControlSoundsProvider::Load(TInfoNodeConstIterator InfoNode)
{
	TInteractiveControlSoundsProvider::Load(InfoNode);
}

// -----------------
// Checkbox control
// -----------------
TControl* TCheckBoxControl::LoadControl(type_t					tpType,
										TInfoNodeConstIterator	InfoNode,
										const TControl*			pParentControl,
										const FRECT&			Resolution)
{
	DEBUG_VERIFY(tpType == CONTROL_TYPE_CHECKBOX);

	DEBUG_VERIFY(InfoNode.IsValid());

	TCheckBoxControlCreationStruct CreationStruct;
	CreationStruct.Load(InfoNode, pParentControl, Resolution);

	TCheckBoxControlSpritesProvider SpritesProvider;
	SpritesProvider.Load(InfoNode);

	TCheckBoxControlSoundsProvider SoundsProvider;
	SoundsProvider.Load(InfoNode);

	return new TCheckBoxControl(CreationStruct,
								SpritesProvider,
								SoundsProvider);
}

TCheckBoxControl::TCheckBoxControl(	const TCheckBoxControlCreationStruct&	CreationStruct,
									TCheckBoxControlSpritesProvider&		SpritesProvider,
									TCheckBoxControlSoundsProvider&			SoundsProvider) :

	TInteractiveControl(CreationStruct, SoundsProvider)
{
	m_bChecked = CreationStruct.m_bChecked;

	m_bPushClickCheckToggling = CreationStruct.m_bPushClickCheckToggling;

	m_fCheckTransitionDelay = CreationStruct.m_fCheckTransitionDelay;

	m_Sprite.ReOwn(SpritesProvider.m_Sprite);
}

// Allocation
void TCheckBoxControl::OnAllocate()
{
	TInteractiveControl::OnAllocate();

	// Check state
	m_CheckState.Allocate(&m_fCheckTransitionDelay);
}

void TCheckBoxControl::OnSetInitialValues()
{
	TInteractiveControl::OnSetInitialValues();

	m_CheckState.SetState(m_bChecked ? CS_CHECKED : CS_NONE, true);
}

// Suspension
bool TCheckBoxControl::OnResume()
{
	if(!TInteractiveControl::OnResume())
		return false;

	m_CheckState.Resume();

	return true;
}

bool TCheckBoxControl::OnSuspend()
{
	if(!TInteractiveControl::OnSuspend())
		return false;

	m_CheckState.Suspend();

	return true;
}

// Update/render events
void TCheckBoxControl::OnPreUpdate()
{
	TInteractiveControl::OnPreUpdate();

	m_CheckState.Update();
}

void TCheckBoxControl::OnRender() const
{
	TInteractiveControl::OnRender();

	// Sprite
	const TSprite* pSprite = m_Sprite.GetDataPtr();

	if(pSprite)
	{
		const float States[2] = {	GetInterpolatedCheckState(),
									GetInterpolatedInteractiveState()};

		pSprite->DrawRect(	GetCurrentScreenState().m_Rect.	m_OwnPart,
							GetCurrentScreenState().m_Color.m_OwnPart,
							TSpriteStates(States, 2));
	}
}

// Click events
void TCheckBoxControl::OnClick(bool bFromMouse)
{
	TInteractiveControl::OnClick(bFromMouse);

	if(!m_bPushClickCheckToggling)
		Check(bFromMouse);
}

void TCheckBoxControl::OnPushClick(bool bFromMouse)
{
	TInteractiveControl::OnPushClick(bFromMouse);

	if(m_bPushClickCheckToggling)
		Check(bFromMouse);
}

// Check events
void TCheckBoxControl::OnCheck(bool bFromMouse)
{
	m_bChecked = !m_bChecked;

	m_CheckState.SetState(m_bChecked ? CS_CHECKED : CS_NONE, false);
	
	new(GetInterface().GetMessageProcessor())
		TCheckBoxControlCheckMessage(this, m_bChecked, bFromMouse);
}

// Check methods
void TCheckBoxControl::Check(bool bFromMouse)
{
	OnCheck(bFromMouse);
}

KString TCheckBoxControl::GetText(bool* pRSuccess) const
{
	return GetSpriteText(m_Sprite.GetDataPtr(), pRSuccess);
}

bool TCheckBoxControl::SetText(const KString& Text)
{
	return SetSpriteText(m_Sprite.GetDataPtr(), Text);
}

bool TCheckBoxControl::GetCheck(bool* pRSuccess) const
{
	if(pRSuccess)
		*pRSuccess = true;

	return m_bChecked;
}

bool TCheckBoxControl::SetCheck(bool bCheck)
{
	if(m_bChecked != bCheck)
		Check(false);

	return true;
}

// --------------------------------------
// Easy checkbox control creation struct
// --------------------------------------
TEasyCheckBoxControlCreationStruct::TEasyCheckBoxControlCreationStruct()
{
	size_t i, j;

	m_fTextOffset = 0.03f;

	for(i = 0 ; i < 2 ; i++)
	{
		for(j = 0 ; j < 4 ; j++)
			m_TextColors[i][j] = WhiteColor();
	}
};

void TEasyCheckBoxControlCreationStruct::Load(	TInfoNodeConstIterator	InfoNode,
												const TControl*			pParentConrol,
												const FRECT&			Resolution)
{
	size_t i, j;

	TInfoParameterConstIterator PIter;

	TCheckBoxControlCreationStruct::Load(InfoNode, pParentConrol, Resolution);

	TTextSpriteCreationStruct::Load(InfoNode);

	// Text offset
	if((PIter = InfoNode->FindParameter(TEXT("TextOffset"))).IsValid())
	{
		ReadFloat(	PIter->m_Value,
					m_fTextOffset,
					TEXT("text offset"));

		ResizeValue(FSEGMENT(0.0f, FSIZE(Resolution).cx),
					FSEGMENT(0.0f, 1.0),
					m_fTextOffset);
	}

	// Text colors
	for(j = 0 ; j < 4 ; j++)
	{
		if((PIter = InfoNode->FindParameter((KString)TEXT("TextColor") + j)).IsValid())
		{
			TD3DColor Color;

			ReadColor(	PIter->m_Value,
						Color,
						TEXT("checkbox text state color"),
						true);

			m_TextColors[0][j] = m_TextColors[1][j] = Color;
		}

		for(i = 0 ; i < 2 ; i++)
		{
			if((PIter = InfoNode->FindParameter((KString)TEXT("TextColor") + i + j)).IsValid())
			{
				ReadColor(	PIter->m_Value,
							m_TextColors[i][j],
							TEXT("checkbox text state color"),
							true);
			}
		}
	}
}

void TEasyCheckBoxControlCreationStruct::SetTextColors(	size_t				szIndex,
														const TD3DColor*	pSTextColors)
{
	DEBUG_VERIFY(szIndex < 2);

	memcpy(m_TextColors[szIndex], pSTextColors, sizeof(m_TextColors[0]));
}

void TEasyCheckBoxControlCreationStruct::SetTextColors(const TD3DColor STextColors[2][4])
{
	memcpy(m_TextColors, STextColors, sizeof(m_TextColors));
}

// ---------------------------------------
// Easy checkbox control sprites provider
// ---------------------------------------
TEasyCheckBoxControlSpritesProvider::TEasyCheckBoxControlSpritesProvider()
{
}

void TEasyCheckBoxControlSpritesProvider::Load(TInfoNodeConstIterator InfoNode)
{
	TCheckBoxControlSpritesProvider::Load(InfoNode);
}

// -------------------------------------
// Easy checkbox control fonts provider
// -------------------------------------
TEasyCheckBoxControlFontsProvider::TEasyCheckBoxControlFontsProvider()
{
	m_Font.Allocate(FONTS_REGISTRATION_MANAGER[DEFAULT_LABEL_FONT_INDEX], true);
}

void TEasyCheckBoxControlFontsProvider::Load(TInfoNodeConstIterator InfoNode)
{
	TTextSpriteFontsProvider::Load(InfoNode);
}

// --------------------------------------
// Easy checkbox control sounds provider
// --------------------------------------
TEasyCheckBoxControlSoundsProvider::TEasyCheckBoxControlSoundsProvider()
{
}

void TEasyCheckBoxControlSoundsProvider::Load(TInfoNodeConstIterator InfoNode)
{
	TCheckBoxControlSoundsProvider::Load(InfoNode);
}

// ----------------------
// Easy checkbox control
// ----------------------
TControl* TEasyCheckBoxControl::LoadControl(type_t					tpType,
											TInfoNodeConstIterator	InfoNode,
											const TControl*			pParentControl,
											const FRECT&			Resolution)
{
	DEBUG_VERIFY(tpType == CONTROL_TYPE_EASY_CHECKBOX);

	DEBUG_VERIFY(InfoNode.IsValid());

	TEasyCheckBoxControlCreationStruct CreationStruct;
	CreationStruct.Load(InfoNode, pParentControl, Resolution);

	TEasyCheckBoxControlSpritesProvider SpritesProvider;
	SpritesProvider.Load(InfoNode);

	TEasyCheckBoxControlFontsProvider FontsProvider;
	FontsProvider.Load(InfoNode);

	TEasyCheckBoxControlSoundsProvider SoundsProvider;
	SoundsProvider.Load(InfoNode);

	return new TEasyCheckBoxControl(CreationStruct,
									SpritesProvider,
									FontsProvider,
									SoundsProvider);
}
	
TEasyCheckBoxControl::TEasyCheckBoxControl(	const TEasyCheckBoxControlCreationStruct&	CreationStruct,
											TEasyCheckBoxControlSpritesProvider&		SpritesProvider,
											TEasyCheckBoxControlFontsProvider&			FontsProvider,
											TEasyCheckBoxControlSoundsProvider&			SoundsProvider) :

	TCheckBoxControl(	CreationStruct,
						TCheckBoxControlSpritesProvider(),
						SoundsProvider)
{
	size_t i, j;

	TCompositeSpriteCreationStruct SpriteCreationStruct;

	SpriteCreationStruct.SetNItems(2);

	SpriteCreationStruct.m_Parameters[1].m_Offset.cx =
		CreationStruct.m_fTextOffset * g_GraphicsDeviceGlobals.m_ScreenSize.cx;

	TCompositeSpriteSpritesProvider SpriteSpritesProvider;

	SpriteSpritesProvider.SetNItems(2);

	// Checkbox check sprite
	{
		SpriteSpritesProvider.m_Sprites[0].ReOwn(SpritesProvider.m_Sprite);
	}

	// Text check sprite
	{
		TTransitionSpriteCreationStruct CheckTextSpriteCreationStruct;

		CheckTextSpriteCreationStruct.SetNItems(2);
		
		TTransitionSpriteSpritesProvider CheckTextSpriteSpritesProvider;

		CheckTextSpriteSpritesProvider.SetNItems(2);

		for(i = 0 ; i < 2 ; i++)
		{
			TTransitionSpriteCreationStruct InteractiveSpriteCreationStruct;

			InteractiveSpriteCreationStruct.SetNItems(4);

			for(j = 0 ;j < 4 ; j++)
			{
				InteractiveSpriteCreationStruct.m_Parameters[j].m_Color =
					CreationStruct.m_TextColors[i][j];
			}

			TTransitionSpriteSpritesProvider InteractiveSpriteSpritesProvider;

			InteractiveSpriteSpritesProvider.SetNItems(4);

			((TTextSprite*)InteractiveSpriteSpritesProvider.m_Sprites[0].
				Allocate(new TTextSprite, false))->
					Allocate(CreationStruct, FontsProvider);

			((TTransitionSprite*)CheckTextSpriteSpritesProvider.m_Sprites[i].
				Allocate(new TTransitionSprite, false))->
					Allocate(	InteractiveSpriteCreationStruct,
								InteractiveSpriteSpritesProvider);
		}

		((TTransitionSprite*)SpriteSpritesProvider.m_Sprites[1].
			Allocate(new TTransitionSprite, false))->
				Allocate(	CheckTextSpriteCreationStruct,
							CheckTextSpriteSpritesProvider);
	}

	((TCompositeSprite*)m_Sprite.
		Allocate(new TCompositeSprite, false))->
			Allocate(SpriteCreationStruct, SpriteSpritesProvider);
}