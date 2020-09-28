#include "kfc_gui_pch.h"
#include "gui_elements.h"

#include "gui_consts.h"
#include "gui_device_globals.h"
#include "gui.h"

#include "resource.h"

// ----------------
// Global routines
// ----------------
size_t MeasureCheckText(HDC hDC)
{
    return Max(MeasureNoCheckText(hDC), 24u) + 4;
}

size_t MeasureNoCheckText(HDC hDC)
{
    return GetTextSize(hDC, TEXT("MW")).cy;
}

static void DrawPostBGText( HDC             hDC,
                            LPCTSTR         pText,
                            const IRECT&    Rect,
                            TAlignment      Alignment,
                            const ISIZE&    BorderSpacing,
                            COLORREF        crTextBGColor,
                            COLORREF        crTextFGColor,
                            kflags_t        flFlags)
{
    DEBUG_VERIFY(hDC);

    DEBUG_VERIFY(pText);

    if(!*pText)
        return;

    T_DC_Clipper Clipper0(hDC, Rect);

    TGDIObjectSelector Selector0(hDC, GetStockObject(DEFAULT_GUI_FONT));

    ISIZE TextSize = TO_I(GetTextSize(hDC, pText));

    IRECT BorderRect;

    if(BorderSpacing.cx == 0)
    {
        BorderRect.m_Left = GetAlignedOrigin(   ISEGMENT(Rect.m_Left, Rect.m_Right),
                                                TextSize.cx + 4,
                                                Alignment);

        BorderRect.m_Right = BorderRect.m_Left + TextSize.cx + 4;
    }
    else
    {
        DEBUG_VERIFY(BorderSpacing.cx > 0);

        BorderRect.m_Left   = Rect.m_Left   + BorderSpacing.cx;
        BorderRect.m_Right  = Rect.m_Right  - BorderSpacing.cx;
    }

    if(BorderSpacing.cy == 0)
    {
        BorderRect.m_Top = GetAlignedOrigin(ISEGMENT(Rect.m_Top, Rect.m_Bottom),
                                            TextSize.cy + 2,
                                            ALIGNMENT_MID);

        BorderRect.m_Bottom = BorderRect.m_Top + TextSize.cy + 2;
    }
    else
    {
        DEBUG_VERIFY(BorderSpacing.cy > 0);

        BorderRect.m_Top    = Rect.m_Top    + BorderSpacing.cy;
        BorderRect.m_Bottom = Rect.m_Bottom - BorderSpacing.cy;
    }

    IntersectRect(BorderRect, Rect);

    const IPOINT TextCoords =
        GetAlignedOrigin(   IRECT(  BorderRect.m_Left + 2,
                                    Rect.m_Top,
                                    BorderRect.m_Right - 2,
                                    Rect.m_Bottom),
                            TextSize,
                            ALSIZE(Alignment, ALIGNMENT_MID));

    IRECT TextRect;

    TextRect.m_Left     = TextCoords.x;
    TextRect.m_Top      = TextCoords.y;
    TextRect.m_Right    = TextRect.m_Left   + TextSize.cx;
    TextRect.m_Bottom   = TextRect.m_Top    + TextSize.cy;

    if(crTextBGColor != CLR_INVALID)
        FillRect(hDC, BorderRect, TBrush(crTextBGColor));

    TBkModeSelector Selector1(hDC, TRANSPARENT);

    TTextColorSelector Selector2(hDC, crTextFGColor);

    DrawText(   hDC,
                pText,
                -1,
                TextRect,
                DT_SINGLELINE | DT_VCENTER | DT_CENTER | flFlags);
}

void DrawCheckText( HDC             hDC,
                    LPCTSTR         pText,
                    const IRECT&    Rect,
                    TAlignment      Alignment,
                    bool            bChecked,
                    bool            bSelected,
                    bool            bActiveControl,
                    const ISIZE&    BorderSpacing,
                    COLORREF        crTextBGColor,
                    COLORREF        crTextFGColor,
                    kflags_t        flFlags)
{
    DEBUG_VERIFY(hDC);

    DEBUG_VERIFY(pText);

    DEBUG_VERIFY(g_GUI_Consts.m_bWithCheckIcons);

    COLORREF crBGColor;
    COLORREF crFGColor;

    if(bSelected)
    {
        if(bActiveControl)
        {
            crBGColor = GetSysColor(COLOR_HIGHLIGHT);
            crFGColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
        }
        else
        {
            crBGColor = GetSysColor(COLOR_BTNFACE);
            crFGColor = GetSysColor(COLOR_WINDOWTEXT);
        }
    }
    else
    {
        crBGColor = GetSysColor(COLOR_WINDOW);
        crFGColor = GetSysColor(COLOR_WINDOWTEXT);
    }

    // BG
    {
        FillRect(hDC, Rect, TBrush(CreateSolidBrush(crBGColor)));
    }

    // Icon
    {
        DrawIcon(   hDC,
                    Rect.m_Left,
                    (Rect.m_Top + Rect.m_Bottom - 24) / 2,
                    g_GUI_DeviceGlobals.m_CheckIcons
                        [bChecked ? 1 : 0][bSelected && bActiveControl ? 1 : 0]);
    }

    // Text
    IRECT TextRect = Rect;
    TextRect.m_Left += 24 + 4;

    DrawPostBGText( hDC,
                    pText,
                    TextRect,
                    Alignment,
                    BorderSpacing,
                    crTextBGColor,
                    crTextFGColor == CLR_INVALID ? crFGColor : crTextFGColor,
                    flFlags);
}

void DrawNoCheckText(   HDC             hDC,
                        LPCTSTR         pText,
                        const IRECT&    Rect,
                        TAlignment      Alignment,
                        bool            bSelected,
                        bool            bActiveControl,
                        const ISIZE&    BorderSpacing,
                        COLORREF        crTextBGColor,
                        COLORREF        crTextFGColor,
                        kflags_t        flFlags)
{
    DEBUG_VERIFY(hDC);

    DEBUG_VERIFY(pText);

    COLORREF crBGColor;
    COLORREF crFGColor;

    if(bSelected)
    {
        if(bActiveControl)
        {
            crBGColor = GetSysColor(COLOR_HIGHLIGHT);
            crFGColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
        }
        else
        {
            crBGColor = GetSysColor(COLOR_BTNFACE);
            crFGColor = GetSysColor(COLOR_WINDOWTEXT);
        }
    }
    else
    {
        crBGColor = GetSysColor(COLOR_WINDOW);
        crFGColor = GetSysColor(COLOR_WINDOWTEXT);
    }

    // BG
    {
        FillRect(hDC, Rect, TBrush(CreateSolidBrush(crBGColor)));
    }

    // Text
    IRECT TextRect = Rect;

    if(Alignment == ALIGNMENT_MIN)
        flFlags |= DT_LEFT;
    else if(Alignment == ALIGNMENT_MAX)
        flFlags |= DT_RIGHT;
    else if(Alignment == ALIGNMENT_MID)
        flFlags |= DT_CENTER;
    else
        INITIATE_FAILURE;

    TextRect.m_Left += 4;

    DrawPostBGText( hDC,
                    pText,
                    TextRect,
                    Alignment,
                    BorderSpacing,
                    crTextBGColor,
                    crTextFGColor == CLR_INVALID ? crFGColor : crTextFGColor,
                    flFlags);
}

bool TogglesCheckText(  const IRECT&    Rect,
                        const IPOINT&   Coords)
{
    IRECT CheckRect;

    CheckRect.m_Left        = Rect.m_Left;
    CheckRect.m_Top         = (Rect.m_Top + Rect.m_Bottom - 24) / 2;
    CheckRect.m_Right       = Rect.m_Left   + 24;
    CheckRect.m_Bottom      = Rect.m_Top    + 24;

    return HitsRect(Coords, CheckRect);
}
