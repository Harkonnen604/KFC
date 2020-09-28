#include "kfc_graphics_pch.h"
#include "border_sprite.h"

#include "sprite_storage.h"

// ------------------------------
// Border sprite creation struct
// ------------------------------
TBorderSpriteCreationStruct::TBorderSpriteCreationStruct()
{
    m_Placement = BSP_OUTSIDE;
}

void TBorderSpriteCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
    TSpriteCreationStruct::Load(InfoNode);

    TInfoParameterConstIterator PIter;

    // Getting placement
    if((PIter = InfoNode->FindParameter(TEXT("Placement"))).IsValid())
    {
        ReadBorderSpritePlacement(  PIter->m_Value,
                                    m_Placement,
                                    TEXT("border sprite placement"));
    }
}

// -------------------------------
// Border sprite sprites provider
// -------------------------------
TBorderSpriteSpritesProvider::TBorderSpriteSpritesProvider()
{
}

void TBorderSpriteSpritesProvider::Load(TInfoNodeConstIterator InfoNode)
{
    const size_t Limits[3] = {3, 3, 3};

    const bool Omittables[3][3] = { {false, false,  false},
                                    {false, true,   true},
                                    {false, true,   false}};

    g_SpriteStorage.LoadObjects(InfoNode,
                                TEXT("Sprite?$"),
                                TEXT("?$"),
                                &m_Sprites[0][0],
                                Limits,
                                &Omittables[0][0]);
}

// --------------
// Border sprite
// --------------
TSprite* TBorderSprite::Create(type_t tpType)
{
    DEBUG_VERIFY(tpType == SPRITE_TYPE_BORDER);

    return new TBorderSprite;
}

TBorderSprite::TBorderSprite()
{
    m_bAllocated = false;
}

void TBorderSprite::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        for(size_t i = 0 ; i < 3 ; i++)
            for(size_t j = 0 ; j < 3 ; j++)
                m_Sprites[i][j].Release();

        TSprite::Release();
    }
}

void TBorderSprite::Allocate(   const TBorderSpriteCreationStruct&  CreationStruct,
                                TBorderSpriteSpritesProvider&       SpritesProvider)
{
    Release();

    try
    {
        TSprite::Allocate(CreationStruct);

        DEBUG_VERIFY(SpritesProvider.m_Sprites[0][0].IsAllocated());
        DEBUG_VERIFY(SpritesProvider.m_Sprites[0][1].IsAllocated());
        DEBUG_VERIFY(SpritesProvider.m_Sprites[0][2].IsAllocated());
        DEBUG_VERIFY(SpritesProvider.m_Sprites[1][0].IsAllocated());
        DEBUG_VERIFY(SpritesProvider.m_Sprites[2][0].IsAllocated());
        DEBUG_VERIFY(SpritesProvider.m_Sprites[2][2].IsAllocated());

        for(size_t i = 0 ; i < 3 ; i++)
            for(size_t j = 0 ; j < 3 ; j++)
            {
                if(SpritesProvider.m_Sprites[i][j].IsAllocated())
                {
                    DEBUG_VERIFY(SpritesProvider.m_Sprites[i][j]->HasDefaultSize());

                    m_Sprites[i][j].ReOwn(SpritesProvider.m_Sprites[i][j]);
                }
            }

        m_Placement = CreationStruct.m_Placement;

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

void TBorderSprite::Load(TInfoNodeConstIterator InfoNode)
{
    Release();

    DEBUG_VERIFY(InfoNode.IsValid());

    TBorderSpriteCreationStruct CreationStruct;
    CreationStruct.Load(InfoNode);

    TBorderSpriteSpritesProvider SpritesProvider;
    SpritesProvider.Load(InfoNode);

    Allocate(CreationStruct, SpritesProvider);
}

void TBorderSprite::DrawNonScaled(  const FPOINT&           DstCoords,
                                    const TD3DColor&        Color,
                                    const TSpriteStates&    States) const
{
    DEBUG_VERIFY_ALLOCATION;

    INITIATE_FAILURE;
}

void TBorderSprite::DrawRect(   const FRECT&            DstRect,
                                const TD3DColor&        Color,
                                const TSpriteStates&    States) const
{
    DEBUG_VERIFY_ALLOCATION;

    const TD3DColor TargetColor = Color * m_Color;

    size_t i, j;

    FPOINT  Coords;
    FRECT   Rect;

    FSIZE Sizes[3][3];

    for(i = 0 ; i < 3 ; i++)
        for(j = 0 ;j < 3 ; j++)
            if(i != 1 || j != 1)
                m_Sprites[i][j]->GetDefaultSize(Sizes[i][j]);

    if(m_Placement == BSP_INSIDE)
    {
        FSIZE DstSize = DstRect;

        // Horizontal sizes adjustment
        for(i = 0 ; i < 3 ; i++)
        {
            const float fSumSize = Sizes[i][0].cx + Sizes[i][2].cx;

            if(fSumSize > fabsf(DstSize.cx))
            {
                ResizeValue(FSEGMENT(0.0f, fSumSize),
                            FSEGMENT(0.0f, DstSize.cx),
                            Sizes[i][0].cx);

                ResizeValue(FSEGMENT(0.0f, fSumSize),
                            FSEGMENT(0.0f, DstSize.cx),
                            Sizes[i][2].cx);
            }
        }

        // Vertical sizes adjustment
        for(i = 0 ; i < 3 ; i++)
        {
            const float fSumSize = Sizes[0][i].cy + Sizes[2][i].cy;

            if(fSumSize > fabsf(DstSize.cy))
            {
                ResizeValue(FSEGMENT(0.0f, fSumSize),
                            FSEGMENT(0.0f, DstSize.cy),
                            Sizes[0][i].cy);

                ResizeValue(FSEGMENT(0.0f, fSumSize),
                            FSEGMENT(0.0f, DstSize.cy),
                            Sizes[2][i].cy);
            }
        }

        // 00
        Rect.m_Left     = DstRect.m_Left;
        Rect.m_Top      = DstRect.m_Top;
        Rect.m_Right    = DstRect.m_Left    + Sizes[0][0].cx;
        Rect.m_Bottom   = DstRect.m_Top     + Sizes[0][0].cy;

        m_Sprites[0][0]->DrawRect(Rect, TargetColor, States);

        // 01
        Rect.m_Left     = DstRect.m_Left    + Sizes[0][0].cx;
        Rect.m_Top      = DstRect.m_Top;
        Rect.m_Right    = DstRect.m_Right   - Sizes[0][2].cx;
        Rect.m_Bottom   = DstRect.m_Top     + Sizes[0][1].cy;

        m_Sprites[0][1]->DrawRect(Rect, TargetColor, States);

        // 02
        Rect.m_Left     = DstRect.m_Right   - Sizes[0][2].cx;
        Rect.m_Top      = DstRect.m_Top;
        Rect.m_Right    = DstRect.m_Right;
        Rect.m_Bottom   = DstRect.m_Top     + Sizes[0][2].cy;

        m_Sprites[0][2]->DrawRect(Rect, TargetColor, States);

        // 10
        Rect.m_Left     = DstRect.m_Left;
        Rect.m_Top      = DstRect.m_Top     + Sizes[0][0].cy;
        Rect.m_Right    = DstRect.m_Left    + Sizes[1][0].cx;
        Rect.m_Bottom   = DstRect.m_Bottom  - Sizes[2][0].cy;

        m_Sprites[1][0]->DrawRect(Rect, TargetColor, States);

        // 11
        if(m_Sprites[1][1].IsAllocated())
        {
            Rect.m_Left     = DstRect.m_Left    + Sizes[1][0].cx;
            Rect.m_Top      = DstRect.m_Top     + Sizes[0][1].cy;
            Rect.m_Right    = DstRect.m_Right   - Sizes[1][2].cx;
            Rect.m_Bottom   = DstRect.m_Bottom  - Sizes[2][1].cy;

            m_Sprites[1][1]->DrawRect(Rect, TargetColor, States);
        }

        // 12
        Rect.m_Left     = DstRect.m_Right   - Sizes[1][2].cx;
        Rect.m_Top      = DstRect.m_Top     + Sizes[0][2].cy;
        Rect.m_Right    = DstRect.m_Right;
        Rect.m_Bottom   = DstRect.m_Bottom  - Sizes[2][2].cy;

        m_Sprites[1][2]->DrawRect(Rect, TargetColor, States);

        // 20
        Rect.m_Left     = DstRect.m_Left;
        Rect.m_Top      = DstRect.m_Bottom  - Sizes[2][0].cy;
        Rect.m_Right    = DstRect.m_Left    + Sizes[2][0].cx;
        Rect.m_Bottom   = DstRect.m_Bottom;

        m_Sprites[2][0]->DrawRect(Rect, TargetColor, States);

        // 21
        Rect.m_Left     = DstRect.m_Left    + Sizes[2][0].cx;
        Rect.m_Top      = DstRect.m_Bottom  - Sizes[2][1].cy;
        Rect.m_Right    = DstRect.m_Right   - Sizes[2][2].cx;
        Rect.m_Bottom   = DstRect.m_Bottom;

        m_Sprites[2][1]->DrawRect(Rect, TargetColor, States);

        // 22
        Rect.m_Left     = DstRect.m_Right   - Sizes[2][2].cx;
        Rect.m_Top      = DstRect.m_Bottom  - Sizes[2][2].cy;
        Rect.m_Right    = DstRect.m_Right;
        Rect.m_Bottom   = DstRect.m_Bottom;

        m_Sprites[2][2]->DrawRect(Rect, TargetColor, States);
    }
    else if(m_Placement == BSP_OUTSIDE)
    {
        // 00
        Coords.x = DstRect.m_Left   - Sizes[0][0].cx;
        Coords.y = DstRect.m_Top    - Sizes[0][0].cy;

        m_Sprites[0][0]->DrawNonScaled(Coords, TargetColor, States);

        // 01
        Rect.m_Left     = DstRect.m_Left;
        Rect.m_Top      = DstRect.m_Top - Sizes[0][1].cy;
        Rect.m_Right    = DstRect.m_Right;
        Rect.m_Bottom   = DstRect.m_Top;

        m_Sprites[0][1]->DrawRect(Rect, TargetColor, States);

        // 02
        Coords.x = DstRect.m_Right;
        Coords.y = DstRect.m_Top - Sizes[0][2].cy;

        m_Sprites[0][2]->DrawNonScaled(Coords, TargetColor, States);

        // 10
        Rect.m_Left     = DstRect.m_Left - Sizes[1][0].cx;
        Rect.m_Top      = DstRect.m_Top;
        Rect.m_Right    = DstRect.m_Left;
        Rect.m_Bottom   = DstRect.m_Bottom;

        m_Sprites[1][0]->DrawRect(Rect, TargetColor, States);

        // 11
        if(m_Sprites[1][1].IsAllocated())
            m_Sprites[1][1]->DrawRect(DstRect, Color);

        // 12
        Rect.m_Left     = DstRect.m_Right;
        Rect.m_Top      = DstRect.m_Top;
        Rect.m_Right    = DstRect.m_Right + Sizes[1][2].cx;
        Rect.m_Bottom   = DstRect.m_Bottom;

        m_Sprites[1][2]->DrawRect(Rect, TargetColor, States);

        // 20
        Coords.x = DstRect.m_Left - Sizes[2][0].cx;
        Coords.y = DstRect.m_Bottom;

        m_Sprites[2][0]->DrawNonScaled(Coords, TargetColor, States);

        // 21
        Rect.m_Left     = DstRect.m_Left;
        Rect.m_Top      = DstRect.m_Bottom;
        Rect.m_Right    = DstRect.m_Right;
        Rect.m_Bottom   = DstRect.m_Bottom + Sizes[2][1].cy;

        m_Sprites[2][1]->DrawRect(Rect, TargetColor, States);

        // 22
        Coords.x = DstRect.m_Right;
        Coords.y = DstRect.m_Bottom;

        m_Sprites[2][2]->DrawNonScaled(Coords, TargetColor, States);
    }
    else
    {
        INITIATE_FAILURE;
    }
}

bool TBorderSprite::HasDefaultSize() const
{
    DEBUG_VERIFY_ALLOCATION;

    return false;
}

void TBorderSprite::GetDefaultSize(FSIZE& Size) const
{
    DEBUG_VERIFY_ALLOCATION;

    INITIATE_FAILURE;
}

// ----------------
// Global routines
// ----------------
bool FromString(const KString&          String,
                TBorderSpritePlacement& RPlacement)
{
    const KString TempString = String.Trimmed();

    if(TempString == TEXT("Inside"))
    {
        RPlacement = BSP_INSIDE;
        return true;
    }

    if(TempString == TEXT("Outside"))
    {
        RPlacement = BSP_OUTSIDE;
        return true;
    }

    return false;
}

void ReadBorderSpritePlacement( const KString&          String,
                                TBorderSpritePlacement& RPlacement,
                                LPCTSTR                 pValueName)
{
    if(!FromString(String, RPlacement))
        REPORT_INCORRECT_VALUE_FORMAT;
}
