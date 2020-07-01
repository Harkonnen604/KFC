#include "kfc_graphics_pch.h"
#include "effect_sprites.h"

#include "2d_effects.h"
#include "color_defs.h"

// -----------------------------------
// Rect effect sprite creation struct
// -----------------------------------
TRectEffectSpriteCreationStruct::TRectEffectSpriteCreationStruct()
{
	size_t i;

	for(i = 0 ; i < 4 ; i++)
		m_SubColors[i] = WhiteColor();
}

void TRectEffectSpriteCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
	size_t i;
	
	TInfoParameterConstIterator PIter;

	TSpriteCreationStruct::Load(InfoNode);

	for(i = 0 ; i < 4 ; i++)
	{
		if((PIter = InfoNode->FindParameter((KString)TEXT("SubColor") + i)).IsValid())
		{
			ReadColor(	PIter->m_Value,
						m_SubColors[i],
						TEXT("rect effect sprite sub-color"),
						true);
		}
	}
}

// -------------------
// Rect effect sprite
// -------------------
TSprite* TRectEffectSprite::Create(type_t tpType)
{
	DEBUG_VERIFY(tpType == SPRITE_TYPE_RECT_EFFECT);

	return new TRectEffectSprite;
}

TRectEffectSprite::TRectEffectSprite()
{
	m_bAllocated = false;
}

void TRectEffectSprite::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		TSprite::Release();
	}
}

void TRectEffectSprite::Allocate(const TRectEffectSpriteCreationStruct& CreationStruct)
{
	Release();

	try
	{
		TSprite::Allocate(CreationStruct);

		memcpy(m_SubColors, CreationStruct.m_SubColors, sizeof(m_SubColors));

		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

void TRectEffectSprite::Load(TInfoNodeConstIterator InfoNode)
{
	Release();

	DEBUG_VERIFY(InfoNode.IsValid());

	TRectEffectSpriteCreationStruct CreationStruct;
	CreationStruct.Load(InfoNode);

	Allocate(CreationStruct);
}

void TRectEffectSprite::DrawNonScaled(	const FPOINT&			DstCoords,
										const TD3DColor&		Color,
										const TSpriteStates&	States) const
{
	DEBUG_VERIFY_ALLOCATION;

	INITIATE_FAILURE;
}

void TRectEffectSprite::DrawRect(	const FRECT&			DstRect,
									const TD3DColor&		Color,
									const TSpriteStates&	States) const
{
	DEBUG_VERIFY_ALLOCATION;

	size_t i;

	D3DCOLOR Colors[4];

	for(i = 0 ; i < 4 ; i++)
		Colors[i] = m_SubColors[i] * m_Color * Color;

	TRectEffect::DrawRect(DstRect, Colors);
}

void TRectEffectSprite::GetDefaultSize(FSIZE& RSize) const
{
	DEBUG_VERIFY_ALLOCATION;

	INITIATE_FAILURE;
}