#include "kfc_graphics_pch.h"
#include "transition_sprite.h"

#include <math.h>
#include "color_defs.h"

// ----------------------------------
// Transition sprite creation struct
// ----------------------------------
TTransitionSpriteCreationStruct::TTransitionSpriteCreationStruct()
{
	m_FloorSubSpriteBlendMode	= TSSBM_OPAQUE;
	m_CeilSubSpriteBlendMode	= TSSBM_BLEND;
}

void TTransitionSpriteCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
	TCompoundSpriteCreationStruct::Load(InfoNode);
	
	TInfoParameterConstIterator PIter;

	// Getting floor sprite blend mode
	if((PIter = InfoNode->FindParameter(TEXT("FloorSubSpriteBlendMode"))).IsValid())
	{
		ReadTransitionSubSpriteBlendMode(	PIter->m_Value,
											m_FloorSubSpriteBlendMode,
											TEXT("floor sub-sprite blend mode"));
	}

	// Getting ceil sprite blend mode
	if((PIter = InfoNode->FindParameter(TEXT("CeilSubSpriteBlendMode"))).IsValid())
	{
		ReadTransitionSubSpriteBlendMode(	PIter->m_Value,
											m_CeilSubSpriteBlendMode,
											TEXT("ceil sub-sprite blend mode"));
	}		
}

// -----------------------------------
// Transition sprite sprites provider
// -----------------------------------
TTransitionSpriteSpritesProvider::TTransitionSpriteSpritesProvider()
{
}

void TTransitionSpriteSpritesProvider::Load(TInfoNodeConstIterator	InfoNode,
											size_t					szNItems)
{
	TCompoundSpriteSpritesProvider::Load(InfoNode, szNItems, CSOM_TAKE_PREV);
}

// ------------------
// Transition sprite
// ------------------
TSprite* TTransitionSprite::Create(type_t tpType)
{
	DEBUG_VERIFY(tpType == SPRITE_TYPE_TRANSITION);

	return new TTransitionSprite;
}

TTransitionSprite::TTransitionSprite()
{
	m_bAllocated = false;
}

void TTransitionSprite::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		TCompoundSprite::Release();
	}
}

void TTransitionSprite::Allocate(	const TTransitionSpriteCreationStruct&	CreationStruct,
									TTransitionSpriteSpritesProvider&		SpritesProvider)
{
	Release();

	try
	{		
		TCompoundSprite::Allocate(CreationStruct, SpritesProvider, CSOM_TAKE_PREV);

		m_FloorSubSpriteBlendMode	= CreationStruct.m_FloorSubSpriteBlendMode;
		m_CeilSubSpriteBlendMode	= CreationStruct.m_CeilSubSpriteBlendMode;

		m_bAllocated = true;
	}
	
	catch(...)
	{
		Release(true);
		throw;
	}
}

void TTransitionSprite::Load(TInfoNodeConstIterator InfoNode)
{
	Release();

	DEBUG_VERIFY(InfoNode.IsValid());

	TTransitionSpriteCreationStruct CreationStruct;
	CreationStruct.Load(InfoNode);

	TTransitionSpriteSpritesProvider SpritesProvider;
	SpritesProvider.Load(InfoNode, CreationStruct.m_szNItems);

	Allocate(CreationStruct, SpritesProvider);
}

void TTransitionSprite::DrawNonScaled(	const FPOINT&			DstCoords,
										const TD3DColor&		Color,
										const TSpriteStates&	States) const
{
	DEBUG_VERIFY_ALLOCATION;

	const TD3DColor TargetColor = Color * m_Color;	

	float fState = *States;

	BoundValue(fState, FSEGMENT(0.0f, (float)(GetNSubObjects() - 1)));

	const float fFloor	= floorf(fState);
	const float fCeil	= ceilf	(fState);

	const size_t szFloorState	= (size_t)fFloor;
	const size_t szCeilState	= (size_t)fCeil;

	const TSprite* pFloorSprite	= GetItems()[szFloorState].	m_Sprite.GetDataPtr();
	const TSprite* pCeilSprite	= GetItems()[szCeilState].	m_Sprite.GetDataPtr();

	const TD3DColor FloorColor	= GetItems()[szFloorState].	m_Parameters.m_Color * TargetColor;
	const TD3DColor CeilColor	= GetItems()[szCeilState].	m_Parameters.m_Color * TargetColor;

	const FSIZE& FloorOffset	= GetItems()[szFloorState].	m_Parameters.m_Offset;
	const FSIZE& CeilOffset		= GetItems()[szCeilState].	m_Parameters.m_Offset;

	const FPOINT FloorDstCoords	= ShiftPoint(FPOINT(DstCoords), FloorOffset);
	const FPOINT CeilDstCoords	= ShiftPoint(FPOINT(DstCoords), CeilOffset);

	if(Compare(fState, fFloor) == 0) // floor sprite only
	{
		pFloorSprite->DrawNonScaled(FloorDstCoords, FloorColor, States.Inced());
	}
	else if(Compare(fState, fCeil) == 0) // ceil sprite only
	{
		pCeilSprite->DrawNonScaled(CeilDstCoords, CeilColor, States.Inced());
	}
	else // both floor and ceil sprites
	{
		// Intermediate color
		TD3DColor IntermColor;

		GenerateLinearTransition(	FloorColor,
									CeilColor,
									fState - fFloor,
									IntermColor);

		if(	pFloorSprite	== pCeilSprite &&
			FloorDstCoords	== CeilDstCoords) // same floor and ceil sprites
		{
			pFloorSprite->DrawNonScaled(FloorDstCoords, IntermColor, States.Inced());
		}
		else // different floor and ceil sprites
		{
			TD3DColor TransColor = IntermColor;

			// Floor
			if(m_FloorSubSpriteBlendMode != TSSBM_TRANSPARENT)
			{
				TransColor.m_fAlpha = IntermColor.m_fAlpha;

				if(m_FloorSubSpriteBlendMode == TSSBM_BLEND)
					TransColor.m_fAlpha *= fCeil - fState;
				
				pFloorSprite->DrawNonScaled(FloorDstCoords, TransColor, States.Inced());
			}

			// Ceil
			if(m_CeilSubSpriteBlendMode != TSSBM_TRANSPARENT)
			{
				TransColor.m_fAlpha = IntermColor.m_fAlpha;

				if(m_CeilSubSpriteBlendMode == TSSBM_BLEND)
					TransColor.m_fAlpha *= fState - fFloor;
				
				pCeilSprite->DrawNonScaled(CeilDstCoords, TransColor, States.Inced());
			}
		}
	}
}

void TTransitionSprite::DrawRect(	const FRECT&			DstRect,
									const TD3DColor&		Color,
									const TSpriteStates&	States) const
{
	DEBUG_VERIFY_ALLOCATION;

	const TD3DColor TargetColor = Color * m_Color;

	float fState = *States;

	BoundValue(fState, FSEGMENT(0.0f, (float)(GetItems().GetN() - 1)));

	const float fFloor	= floorf(fState);
	const float fCeil	= ceilf	(fState);

	const size_t szFloorState	= (size_t)fFloor;
	const size_t szCeilState	= (size_t)fCeil;

	const TSprite* pFloorSprite	= GetItems()[szFloorState].	m_Sprite.GetDataPtr();
	const TSprite* pCeilSprite	= GetItems()[szCeilState].	m_Sprite.GetDataPtr();

	const TD3DColor FloorColor	= GetItems()[szFloorState].	m_Parameters.m_Color * TargetColor;
	const TD3DColor CeilColor	= GetItems()[szCeilState].	m_Parameters.m_Color * TargetColor;

	const FSIZE& FloorOffset	= GetItems()[szFloorState].	m_Parameters.m_Offset;
	const FSIZE& CeilOffset		= GetItems()[szCeilState].	m_Parameters.m_Offset;

	const FRECT FloorDstRect	= ShiftRect(FRECT(DstRect), FloorOffset);
	const FRECT CeilDstRect		= ShiftRect(FRECT(DstRect), CeilOffset);

	if(Compare(fState, fFloor) == 0) // floor sprite only
	{
		pFloorSprite->DrawRect(FloorDstRect, FloorColor, States.Inced());
	}
	else if(Compare(fState, fCeil) == 0) // ceil sprite only
	{
		pCeilSprite->DrawRect(CeilDstRect, CeilColor, States.Inced());
	}
	else // both floor and ceil sprites
	{
		// Intermediate color
		TD3DColor IntermColor = CeilColor;
		((IntermColor -= FloorColor) *= (fState - fFloor)) += FloorColor;

		if(	pFloorSprite == pCeilSprite &&
			FloorDstRect == CeilDstRect) // same floor and ceil sprites
		{
			pFloorSprite->DrawRect(FloorDstRect, IntermColor, States.Inced());
		}
		else // different floor and ceil sprites
		{
			TD3DColor TransColor = IntermColor;

			// Floor
			if(m_FloorSubSpriteBlendMode != TSSBM_TRANSPARENT)
			{
				TransColor.m_fAlpha = IntermColor.m_fAlpha;

				if(m_FloorSubSpriteBlendMode == TSSBM_BLEND)
					TransColor.m_fAlpha *= fCeil - fState;
				
				pFloorSprite->DrawRect(FloorDstRect, TransColor, States.Inced());
			}

			// Ceil
			if(m_CeilSubSpriteBlendMode != TSSBM_TRANSPARENT)
			{
				TransColor.m_fAlpha = IntermColor.m_fAlpha;

				if(m_CeilSubSpriteBlendMode == TSSBM_BLEND)
					TransColor.m_fAlpha *= fState - fFloor;
				
				pCeilSprite->DrawRect(CeilDstRect, TransColor, States.Inced());
			}
		}
	}
}

// ----------------
// Global routines
// ----------------
bool FromString(const KString&					String,
				TTransitionSubSpriteBlendMode&	RBlendMode)
{
	const KString TempString = String.Trimmed();

	if(String == TEXT("Blend"))
	{
		RBlendMode = TSSBM_BLEND;
		return true;
	}

	if(TempString == TEXT("Transparent"))
	{
		RBlendMode = TSSBM_TRANSPARENT;
		return true;
	}

	if(TempString == TEXT("Opaque"))
	{
		RBlendMode = TSSBM_OPAQUE;
		return true;
	}

	return false;
}

void ReadTransitionSubSpriteBlendMode(	const KString&					String,
										TTransitionSubSpriteBlendMode&	RBlendMode,
										LPCTSTR							pValueName)
{
	if(!FromString(String, RBlendMode))
	{
		REPORT_INCORRECT_VALUE_FORMAT;
	}
}