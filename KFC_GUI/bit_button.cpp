#include "kfc_gui_pch.h"
#include "bit_button.h"

#include "gui.h"

// ----------------
// Global routines
// ----------------
void DrawBitButton(HWND hWnd, HDC hDC, HICON hIcon, const ISIZE& IconSize)
{
    DEBUG_VERIFY(IsWindow(hWnd));
    DEBUG_VERIFY(hDC);
    DEBUG_VERIFY(hIcon);

    // Getting state
    const kflags_t flState = SendMessage(hWnd, BM_GETSTATE, 0, 0);

    // Getting rect
    IRECT Rect = GetKClientRect(hWnd);

    // Drawing edge
    DrawEdge(hDC, Rect, BF_RECT | BF_MIDDLE, (flState & BST_PUSHED) ? EDGE_SUNKEN : EDGE_RAISED);

    // Internal area
    LessenRect(Rect, ISIZE(2, 2));

    // Pushed internals offset
    if(flState & BST_PUSHED)
        ShiftRect(Rect, ISIZE(2, 2));

    // Icon origin (centered)
    const IPOINT Coords(    (Rect.m_Left + Rect.m_Right - IconSize.cx) / 2,
                            (Rect.m_Top + Rect.m_Bottom - IconSize.cy) / 2);

    // Drawing icon
    DrawIcon(hDC, Coords.x, Coords.y, hIcon);

    // Drawing focus rect
    if(flState & BST_FOCUS)
        DrawFocusRect(hDC, LessenRect(IRECT(Rect), ISIZE(2, 2)));
}

void DrawTextBitButton(HWND hWnd, HDC hDC, HICON hIcon, const ISIZE& IconSize)
{
    DEBUG_VERIFY(IsWindow(hWnd));
    DEBUG_VERIFY(hDC);
    DEBUG_VERIFY(hIcon);

    TGDIObjectSelector  Selector0(hDC, GetStockObject(DEFAULT_GUI_FONT));
    TTextColorSelector  Selector1(hDC, GetSysColor(COLOR_WINDOWTEXT));
    TBkModeSelector     Selector2(hDC, TRANSPARENT);

    // Getting state
    const kflags_t flState = SendMessage(hWnd, BM_GETSTATE, 0, 0);

    // Getting rect
    IRECT Rect = GetKClientRect(hWnd);

    // Drawing edge
    DrawEdge(hDC, Rect, BF_RECT | BF_MIDDLE, (flState & BST_PUSHED) ? EDGE_SUNKEN : EDGE_RAISED);

    // Internal area
    LessenRect(Rect, ISIZE(2, 2));

    // Pushed internals offset
    if(flState & BST_PUSHED)
        ShiftRect(Rect, ISIZE(2, 2));

    // Text
    const KString Text = GetKWindowText(hWnd);

    const ISIZE ITextSize = TO_I(GetTextSize(hDC, Text));

    // Icon origin (centered)
    const IPOINT Coords(4, (Rect.m_Top + Rect.m_Bottom - IconSize.cy) / 2);

    // Drawing icon
    DrawIcon(hDC, Coords.x, Coords.y, hIcon);

    // Drawing text
    DrawText(   hDC,
                Text,
                -1,
                IRECT(4 + IconSize.cx + 4, Rect.m_Top, Rect.m_Right, Rect.m_Bottom),
                DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);

    // Drawing focus rect
    if(flState & BST_FOCUS)
        DrawFocusRect(hDC, LessenRect(IRECT(Rect), ISIZE(2, 2)));
}
