#include "kfc_graphics_pch.h"
#include "dummy_sprites.h"

#include "sprite_storage.h"

// -----------------------------
// Dummy sprite creation struct
// -----------------------------
TDummySpriteCreationStruct::TDummySpriteCreationStruct()
{
}

void TDummySpriteCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
    TSpriteCreationStruct::Load(InfoNode);
}

// -------------
// Dummy sprite
// -------------
TSprite* TDummySprite::Create(type_t tpType)
{
    DEBUG_VERIFY(tpType == SPRITE_TYPE_DUMMY);

    return new TDummySprite;
}

TDummySprite::TDummySprite()
{
    m_bAllocated = false;
}

void TDummySprite::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        TSprite::Release();
    }
}

void TDummySprite::Allocate(TDummySpriteCreationStruct& CreationStruct)
{
    Release();

    try
    {
        TSprite::Allocate(CreationStruct);

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

void TDummySprite::Load(TInfoNodeConstIterator InfoNode)
{
    DEBUG_VERIFY(InfoNode.IsValid());

    TDummySpriteCreationStruct CreationStruct;
    CreationStruct.Load(InfoNode);

    Allocate(CreationStruct);
}

void TDummySprite::DrawNonScaled(   const FPOINT&           DstCoords,
                                    const TD3DColor&        Color,
                                    const TSpriteStates&    States) const
{
    INITIATE_FAILURE;
}

void TDummySprite::DrawRect(const FRECT&            DstRect,
                            const TD3DColor&        Color,
                            const TSpriteStates&    States) const
{
}

bool TDummySprite::HasDefaultSize() const
{
    DEBUG_VERIFY_ALLOCATION;

    return false;
}

void TDummySprite::GetDefaultSize(FSIZE& RSize) const
{
    DEBUG_VERIFY_ALLOCATION;

    INITIATE_FAILURE;
}

TSprite* TDummySprite::GetSubObject(size_t szIndex)
{
    DEBUG_VERIFY_ALLOCATION;

    INITIATE_FAILURE;
}

const TSprite* TDummySprite::GetSubObject(size_t szIndex) const
{
    DEBUG_VERIFY_ALLOCATION;

    INITIATE_FAILURE;
}

size_t TDummySprite::GetNSubObjects() const
{
    DEBUG_VERIFY_ALLOCATION;

    return 0;
}

// -------------------------------------
// State skipper sprite creation struct
// -------------------------------------
TStateSkipperSpriteCreationStruct::TStateSkipperSpriteCreationStruct()
{
    m_szNStates = 1;
}

void TStateSkipperSpriteCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
    TInfoParameterConstIterator PIter;

    if((PIter = InfoNode->FindParameter(TEXT("NStates"))).IsValid())
        ReadUINT(PIter->m_Value, m_szNStates, TEXT("number of skipped states"));
}

// --------------------------------------
// State skipper sprite sprites provider
// --------------------------------------
TStateSkipperSpriteSpritesProvider::TStateSkipperSpriteSpritesProvider()
{
}

void TStateSkipperSpriteSpritesProvider::Load(TInfoNodeConstIterator InfoNode)
{
    g_SpriteStorage.LoadObject(InfoNode, TEXT("Sprite"), m_Sprite, false);
}

// ---------------------
// State skipper sprite
// ---------------------
TSprite* TStateSkipperSprite::Create(type_t tpType)
{
    DEBUG_VERIFY(tpType == SPRITE_TYPE_STATE_SKIPPER);

    return new TStateSkipperSprite;
}

TStateSkipperSprite::TStateSkipperSprite()
{
    m_bAllocated = false;

    m_szNStates = 1;
}

void TStateSkipperSprite::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        m_szNStates = 1;

        TSprite::Release();
    }
}

void TStateSkipperSprite::Allocate( const TStateSkipperSpriteCreationStruct&    CreationStruct,
                                    TStateSkipperSpriteSpritesProvider&         SpritesProvider)
{
    Release();

    try
    {
        TSprite::Allocate(CreationStruct);

        m_szNStates = CreationStruct.m_szNStates;

        m_Sprite.ReOwn(SpritesProvider.m_Sprite);

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

void TStateSkipperSprite::Load(TInfoNodeConstIterator InfoNode)
{
    DEBUG_VERIFY(InfoNode.IsValid());

    TStateSkipperSpriteCreationStruct CreationStruct;
    CreationStruct.Load(InfoNode);

    TStateSkipperSpriteSpritesProvider SpritesProvider;
    SpritesProvider.Load(InfoNode);

    Allocate(CreationStruct, SpritesProvider);
}

void TStateSkipperSprite::DrawNonScaled(const FPOINT&           DstCoords,
                                        const TD3DColor&        Color,
                                        const TSpriteStates&    States) const
{
    DEBUG_VERIFY_ALLOCATION;

    m_Sprite->DrawNonScaled(DstCoords, Color, States + m_szNStates);
}

void TStateSkipperSprite::DrawRect( const FRECT&            DstRect,
                                    const TD3DColor&        Color,
                                    const TSpriteStates&    States) const
{
    DEBUG_VERIFY_ALLOCATION;

    m_Sprite->DrawRect(DstRect, Color, States + m_szNStates);
}

bool TStateSkipperSprite::HasDefaultSize() const
{
    DEBUG_VERIFY_ALLOCATION;

    return m_Sprite->HasDefaultSize();
}

void TStateSkipperSprite::GetDefaultSize(FSIZE& RSize) const
{
    DEBUG_VERIFY_ALLOCATION;

    m_Sprite->GetDefaultSize(RSize);
}

TSprite* TStateSkipperSprite::GetSubObject(size_t szIndex)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(szIndex == 0);

    return m_Sprite.GetDataPtr();
}

const TSprite* TStateSkipperSprite::GetSubObject(size_t szIndex) const
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(szIndex == 0);

    return m_Sprite.GetDataPtr();
}

size_t TStateSkipperSprite::GetNSubObjects() const
{
    DEBUG_VERIFY_ALLOCATION;

    return 1;
}
