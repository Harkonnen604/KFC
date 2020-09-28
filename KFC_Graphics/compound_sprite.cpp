#include "kfc_graphics_pch.h"
#include "compound_sprite.h"

#include "color_defs.h"
#include "sprite_storage.h"

// --------------------------------
// Compound sprite item parameters
// --------------------------------
TCompoundSpriteItemParameters::TCompoundSpriteItemParameters()
{
    m_Color = WhiteColor();

    m_Offset.Set(0.0f, 0.0f);
}

// --------------------------------
// Compound sprite creation struct
// --------------------------------
TCompoundSpriteCreationStruct::TCompoundSpriteCreationStruct()
{
}

void TCompoundSpriteCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
    TSpriteCreationStruct::Load(InfoNode);

    TInfoParameterConstIterator PIter;

    size_t i;

    // Getting number of items
    size_t szNItems;
    ReadNormalizedUINT( InfoNode->GetParameterValue(TEXT("NItems")),
                        szNItems,
                        TEXT("number of compound sprite items"),
                        SZSEGMENT(1, UINT_MAX));

    SetNItems(szNItems);

    // Getting parameters
    for(i = 0 ; i < m_szNItems ; i++)
    {
        // Color
        if((PIter = InfoNode->FindParameter((KString)TEXT("Color") + i)).IsValid())
        {
            ReadColor(  PIter->m_Value,
                        m_Parameters[i].m_Color,
                        (KString)TEXT("compound sprite item ") + i + TEXT(" color"));
        }

        // Offset
        if((PIter = InfoNode->FindParameter((KString)TEXT("Offset") + i)).IsValid())
        {
            ReadBiTypeValue(PIter->m_Value,
                            m_Parameters[i].m_Offset,
                            (KString)TEXT("compound sprite item ") + i + TEXT(" offset"));
        }
    }
}

void TCompoundSpriteCreationStruct::SetNItems(size_t szSNItems)
{
    DEBUG_VERIFY(szSNItems > 0);

    m_szNItems = szSNItems;

    m_Parameters.SetN(m_szNItems);
}

// ---------------------------------
// Compound sprite sprites provider
// ---------------------------------
TCompoundSpriteSpritesProvider::TCompoundSpriteSpritesProvider()
{
}

void TCompoundSpriteSpritesProvider::Load(  TInfoNodeConstIterator  InfoNode,
                                            size_t                  szNItems,
                                            TCompoundSpriteOmitMode OmitMode)
{
    size_t i;

    DEBUG_VERIFY(szNItems > 0);

    SetNItems(szNItems);

    TArray<bool, true> Omittables;

    const bool* pOmittables;

    if(OmitMode == CSOM_NONE)
    {
        pOmittables = NULL;
    }
    else if(OmitMode == CSOM_TAKE_PREV)
    {
        Omittables.SetN(szNItems);

        Omittables[0] = false;
        for(i = 1 ; i < szNItems ; i++)
            Omittables[i] = true;

        pOmittables = Omittables.GetDataPtr();
    }
    else
    {
        INITIATE_FAILURE;
    }

    g_SpriteStorage.LoadObjects(InfoNode,
                                TEXT("Sprite?"),
                                TEXT("?"),
                                m_Sprites.GetDataPtr(),
                                &szNItems,
                                pOmittables);

    if(OmitMode == CSOM_TAKE_PREV)
    {
        for(i = 1 ; i < szNItems ; i++)
        {
            if(!m_Sprites[i].IsAllocated())
                m_Sprites[i].Allocate(m_Sprites[i-1].GetDataPtr(), true);
        }
    }
}

void TCompoundSpriteSpritesProvider::SetNItems(size_t szNItems)
{
    m_Sprites.SetN(szNItems);
}

// ------------------
// Compound sprite
// ------------------
TCompoundSprite::TCompoundSprite()
{
    m_bAllocated = false;
}

void TCompoundSprite::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        m_Items.Clear();

        TSprite::Release();
    }
}

void TCompoundSprite::Allocate( const TCompoundSpriteCreationStruct&    CreationStruct,
                                TCompoundSpriteSpritesProvider&         SpritesProvider,
                                TCompoundSpriteOmitMode                 OmitMode)
{
    Release();

    try
    {
        DEBUG_VERIFY(CreationStruct.m_szNItems > 0);

        DEBUG_VERIFY(SpritesProvider.m_Sprites.GetN() == CreationStruct.m_szNItems);

        TSprite::Allocate(CreationStruct);

        for(size_t i = 0 ; i < CreationStruct.m_szNItems ; i++)
        {
            TItem& Item = m_Items.Add();

            if(!SpritesProvider.m_Sprites[i].IsAllocated())
            {
                if(OmitMode == CSOM_NONE)
                {
                    INITIATE_FAILURE; // the sprite cannot be omitted
                }
                else if(OmitMode == CSOM_TAKE_PREV)
                {
                    if(i > 0)
                        Item.m_Sprite.Allocate(m_Items[i - 1].m_Sprite.GetDataPtr(), true);
                    else
                        INITIATE_FAILURE; // the first sprite cannot be omitted
                }
                else
                {
                    INITIATE_FAILURE; // unknown OmitMode value
                }
            }
            else
            {
                Item.m_Sprite.ReOwn(SpritesProvider.m_Sprites[i]);
            }

            Item.m_Parameters = CreationStruct.m_Parameters[i];
        }

        m_bAllocated = true;

        UpdateDefaultSize();
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

bool TCompoundSprite::HasDefaultSize() const
{
    DEBUG_VERIFY_ALLOCATION;

    return m_bHasDefaultSize;
}

void TCompoundSprite::GetDefaultSize(FSIZE& RSize) const
{
    DEBUG_VERIFY_ALLOCATION;

    if(m_bHasDefaultSize)
        RSize = m_DefaultSize;
    else
        INITIATE_FAILURE;
}

void TCompoundSprite::UpdateDefaultSize()
{
    DEBUG_VERIFY_ALLOCATION;

    m_DefaultSize.Set(0.0f, 0.0f);
    m_bHasDefaultSize = true;

    for(size_t i = 0 ; i < m_Items.GetN() ; i++)
    {
        if(!m_Items[i].m_Sprite->HasDefaultSize())
        {
            m_bHasDefaultSize = false;
            break;
        }

        FSIZE Size;
        m_Items[i].m_Sprite->GetDefaultSize(Size);

        if(Size.cx > m_DefaultSize.cx)
            m_DefaultSize.cx = Size.cx;

        if(Size.cy > m_DefaultSize.cy)
            m_DefaultSize.cy = Size.cy;
    }
}
