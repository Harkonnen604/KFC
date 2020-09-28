#include "kfc_graphics_pch.h"
#include "d3d_color.h"

#include "graphics_tokens.h"

// ----------
// D3D color
// ----------
TD3DColor& TD3DColor::Truncate(bool bProportional)
{
    if(bProportional)
    {
        FSEGMENT SrcSegment(Min(m_fRed, Min(m_fGreen, m_fBlue)),
                            Max(m_fRed, Max(m_fGreen, m_fBlue)));

        FSEGMENT DstSegment(Max(SrcSegment.m_First, 0.0f),
                            Min(SrcSegment.m_Last,  1.0f));

        ResizeValue(SrcSegment, DstSegment, m_fRed);
        ResizeValue(SrcSegment, DstSegment, m_fGreen);
        ResizeValue(SrcSegment, DstSegment, m_fBlue);
    }
    else
    {
        if(m_fRed < 0.0f)
            m_fRed = 0.0f;
        else if(m_fRed > 1.0f)
            m_fRed = 1.0f;

        if(m_fGreen < 0.0f)
            m_fGreen = 0.0f;
        else if(m_fGreen > 1.0f)
            m_fGreen = 1.0f;

        if(m_fBlue < 0.0f)
            m_fBlue = 0.0f;
        else if(m_fBlue > 1.0f)
            m_fBlue = 1.0f;

        if(m_fAlpha < 0.0f)
            m_fAlpha = 0.0f;
        else if(m_fAlpha > 1.0f)
            m_fAlpha = 1.0f;
    }

    return *this;
}

TD3DColor::operator D3DCOLOR () const
{
    TD3DColor Color = Truncated();

    return  ((DWORD)(Color.m_fAlpha * 255.0f + 0.5f) << 24) |
            ((DWORD)(Color.m_fRed   * 255.0f + 0.5f) << 16) |
            ((DWORD)(Color.m_fGreen * 255.0f + 0.5f) << 8)  |
            ((DWORD)(Color.m_fBlue  * 255.0f + 0.5f));
}

void ReadColor( const KString&  String,
                TD3DColor&      RColor,
                LPCTSTR         pValueName,
                bool            bValidate)
{
    if(!FromString( g_GraphicsTokens.m_ColorTokens.Process(String),
                    RColor))
    {
        REPORT_INCORRECT_VALUE_FORMAT;
    }

    if(bValidate && !RColor.IsValid())
    {
        REPORT_INVALID_VALUE;
    }
}
