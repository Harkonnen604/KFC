#include "kfc_graphics_pch.h"
#include "composite_sprite.h"

#include "color_defs.h"

// ---------------------------------
// Composite sprite creation struct
// ---------------------------------
TCompositeSpriteCreationStruct::TCompositeSpriteCreationStruct()
{
}

void TCompositeSpriteCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
    TCompoundSpriteCreationStruct::Load(InfoNode);
}

// ----------------------------------
// Composite sprite sprites provider
// ----------------------------------
TCompositeSpriteSpritesProvider::TCompositeSpriteSpritesProvider()
{
}

void TCompositeSpriteSpritesProvider::Load( TInfoNodeConstIterator  InfoNode,
                                            size_t                  szNItems)
{
    TCompoundSpriteSpritesProvider::Load(InfoNode, szNItems, CSOM_NONE);
}

// -----------------
// Composite sprite
// -----------------
TSprite* TCompositeSprite::Create(type_t tpType)
{
    DEBUG_VERIFY(tpType == SPRITE_TYPE_COMPOSITE);

    return new TCompositeSprite;
}

TCompositeSprite::TCompositeSprite()
{
    m_bAllocated = false;
}

void TCompositeSprite::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        TCompoundSprite::Release();
    }
}

void TCompositeSprite::Allocate(const TCompositeSpriteCreationStruct&   CreationStruct,
                                TCompositeSpriteSpritesProvider&        SpritesProvider)
{
    Release();

    try
    {
        TCompoundSprite::Allocate(CreationStruct, SpritesProvider, CSOM_NONE);

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

void TCompositeSprite::Load(TInfoNodeConstIterator InfoNode)
{
    Release();

    DEBUG_VERIFY(InfoNode.IsValid());

    TCompositeSpriteCreationStruct CreationStruct;
    CreationStruct.Load(InfoNode);

    TCompositeSpriteSpritesProvider SpritesProvider;
    SpritesProvider.Load(InfoNode, CreationStruct.m_szNItems);

    Allocate(CreationStruct, SpritesProvider);
}

void TCompositeSprite::DrawNonScaled(   const FPOINT&           DstCoords,
                                        const TD3DColor&        Color,
                                        const TSpriteStates&    States) const
{
    DEBUG_VERIFY_ALLOCATION;

    const TD3DColor DstColor = Color * m_Color;

    size_t i;

    for(i = 0 ; i < GetItems().GetN() ; i++)
    {
        GetItems()[i].m_Sprite->DrawNonScaled(  ShiftPoint( FPOINT(DstCoords),
                                                            GetItems()[i].m_Parameters.m_Offset),
                                                DstColor,
                                                States);
    }
}

void TCompositeSprite::DrawRect(const FRECT&            DstRect,
                                const TD3DColor&        Color,
                                const TSpriteStates&    States) const
{
    DEBUG_VERIFY_ALLOCATION;

    const TD3DColor DstColor = Color * m_Color;

    size_t i;

    for(i = 0 ; i < GetItems().GetN() ; i++)
    {
        GetItems()[i].m_Sprite->DrawRect(   ShiftRect(  FRECT(DstRect),
                                                        GetItems()[i].m_Parameters.m_Offset),
                                                        DstColor,
                                                        States);
    }
}
