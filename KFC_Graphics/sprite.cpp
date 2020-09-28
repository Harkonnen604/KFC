#include "kfc_graphics_pch.h"
#include "sprite.h"

#include "graphics_device_globals.h"

// --------------
// Sprite states
// --------------
TSpriteStates::TSpriteStates()
{
    m_szN = 0;
}

TSpriteStates::TSpriteStates(const float* pSStates, size_t szSN)
{
    m_szN = 0;

    Allocate(pSStates, szSN);
}

TSpriteStates::TSpriteStates(float fSState)
{
    m_szN = 0;

    Allocate(fSState);
}

void TSpriteStates::Release()
{
    m_szN = 0;
}

void TSpriteStates::Allocate(const float *pSStates, size_t szSN)
{
    Release();

    m_szN = szSN;

    if(m_szN == 1)
        m_fState = *pSStates;
    else
        m_pStates = pSStates;
}

void TSpriteStates::Allocate(float fSState)
{
    Allocate(&fSState, 1);
}

float TSpriteStates::GetCurrent() const
{
    switch(m_szN)
    {
    case 0:
        return DEFAULT_SPRITE_STATE;

    case 1:
        return m_fState;

    default:
        return *m_pStates;
    }
}

TSpriteStates& TSpriteStates::operator ++ ()
{
    if(m_szN > 0)
    {
        if(m_szN > 1)
            m_pStates++;

        if(--m_szN == 1)
            m_fState = *m_pStates;
    }

    return *this;
}

TSpriteStates TSpriteStates::operator + (size_t szStep) const
{
    if(szStep == 0)
        return *this;

    if(m_szN <= szStep)
        return TSpriteStates();

    return TSpriteStates(m_pStates + szStep, m_szN - szStep);
}

// -----------------------
// Sprite creation struct
// -----------------------
TSpriteCreationStruct::TSpriteCreationStruct()
{
    m_Color = WhiteColor();
}

void TSpriteCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
    TInfoParameterConstIterator PIter;

    if((PIter = InfoNode->FindParameter(TEXT("Color"))).IsValid())
    {
        ReadColor(  PIter->m_Value,
                    m_Color,
                    TEXT("sprite color"));
    }
}

// -------
// Sprite
// -------
TSprite::TSprite()
{
    m_bAllocated = false;
}

void TSprite::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;
    }
}

void TSprite::Allocate(const TSpriteCreationStruct& CreationStruct)
{
    Release();

    try
    {
        m_Color = CreationStruct.m_Color;

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

FSIZE TSprite::GetDefaultSize() const
{
    FSIZE Size;
    GetDefaultSize(Size);

    return Size;
}

// ----------------
// Global routines
// ----------------

// Point helpers
FPOINT& ReadPointResized(   const KString&  String,
                            FPOINT&         RPoint,
                            LPCTSTR         pValueName,
                            const FRECT&    ParentRect,
                            const FRECT&    Resolution)
{
    return ResizePoint( ParentRect,
                        FRECT(0.0f, 0.0f, 1.0f, 1.0f),
                        ResizePoint(Resolution,
                                    g_GraphicsDeviceGlobals.m_ScreenRect,
                                    ReadBiTypeValue(String, RPoint, pValueName)));
}

// Rect helpers
FRECT& ReadRectResized( const KString&  String,
                        FRECT&          RRect,
                        LPCTSTR         pValueName,
                        const FRECT&    ParentRect,
                        const FRECT&    Resolution)
{
    return ResizeRect(  ParentRect,
                        FRECT(0.0f, 0.0f, 1.0f, 1.0f),
                        ResizeRect( Resolution,
                                    g_GraphicsDeviceGlobals.m_ScreenRect,
                                    ReadBiTypeValue(String, RRect, pValueName)));
}

// Size helpers
FSIZE& ReadSizeResized( const KString&  String,
                        FSIZE&          RSize,
                        LPCTSTR         pValueName,
                        const FRECT&    ParentRect,
                        const FRECT&    Resolution)
{
    return ResizeSize(  FSIZE(ParentRect),
                        FSIZE(1.0f, 1.0f),
                        ResizeSize( FSIZE(Resolution),
                                    FSIZE(g_GraphicsDeviceGlobals.m_ScreenRect),
                                    ReadBiTypeValue(String, RSize, pValueName)));
}
