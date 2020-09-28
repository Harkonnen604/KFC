#include "kfc_gui_pch.h"
#include "color_label_control.h"

#include <KFC_Common\common_consts.h>
#include <KFC_Common\module.h>
#include "gui.h"

// --------------------
// Color label control
// --------------------
ATOM TColorLabelControl::s_aClass = 0;

TColorLabelControl::TColorLabelControl()
{
    m_bAllocated = false;

    m_hWnd = NULL;
}

void TColorLabelControl::UnregisterClass()
{
    if(s_aClass)
    {
        ::UnregisterClass((LPCTSTR)s_aClass, GetKModuleHandle());
        s_aClass = NULL;
    }
}

void TColorLabelControl::RegisterClass()
{
    UnregisterClass();

    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));

    const KString ClassName =
        TEXT("{A1F07DCE-BD19-40e7-A9BF-AE1A58104FC2}ColorLabelControl");

    wc.style            = CS_DBLCLKS;
    wc.lpfnWndProc      = StaticWindowProc;
    wc.hInstance        = GetKModuleHandle();
    wc.lpszClassName    = ClassName;

    s_aClass = ::RegisterClass(&wc);
    if(s_aClass == 0)
    {
        INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error registering color label control window class"),
                                        GetLastError());
    }
}

void TColorLabelControl::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        if(m_hWnd)
            DestroyWindow(m_hWnd), m_hWnd = NULL;
    }
}

void TColorLabelControl::Allocate(  HWND            hParentWnd,
                                    kflags_t        flStyle,
                                    kflags_t        flExStyle,
                                    const IRECT&    Rect,
                                    LPCTSTR         pSText,
                                    TAlignment      STextAlignment,
                                    COLORREF        crSBGColor,
                                    COLORREF        crSFGColor)
{
    Release();

    try
    {
        m_alTextAlignment = STextAlignment;

        m_crBGColor = crSBGColor;
        m_crFGColor = crSFGColor;

        m_hWnd = CreateWindowEx(flExStyle,
                                (LPCTSTR)s_aClass,
                                pSText,
                                flStyle,
                                Rect.m_Left,
                                Rect.m_Top,
                                Rect.GetWidth(),
                                Rect.GetHeight(),
                                hParentWnd,
                                NULL,
                                GetKModuleHandle(),
                                0);

        if(m_hWnd == NULL)
        {
            INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error creating color label control"),
                                            GetLastError());
        }

        SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

LRESULT CALLBACK TColorLabelControl::StaticWindowProc(  HWND    hWnd,
                                                        UINT    uiMsg,
                                                        WPARAM  wParam,
                                                        LPARAM  lParam)
{
    TColorLabelControl* pControl =
        (TColorLabelControl*)GetWindowLong(hWnd, GWL_USERDATA);

    if(pControl && pControl->IsAllocated())
        return pControl->WindowProc(hWnd, uiMsg, wParam, lParam);

    return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

LRESULT TColorLabelControl::WindowProc( HWND    hWnd,
                                        UINT    uiMsg,
                                        WPARAM  wParam,
                                        LPARAM  lParam)
{
    DEBUG_VERIFY(hWnd == m_hWnd);

    if(uiMsg == WM_SETTEXT)
    {
        if(GetKWindowText(*this) == (LPCTSTR)lParam)
            return 0;

        InvalidateRect(*this, NULL, FALSE);
    }
    else if(uiMsg == WM_PAINT)
    {
        TPaintDC DC(*this);

        IRECT ClientRect = GetKClientRect(*this);

        FillRect(DC, ClientRect, TBrush(CreateSolidBrush(m_crBGColor)));

        TGDIObjectSelector  Selector0(DC, GetStockObject(DEFAULT_GUI_FONT));
        TBkModeSelector     Selector1(DC, TRANSPARENT);
        TTextColorSelector  Selector2(DC, m_crFGColor);

        DrawText(   DC,
                    GetKWindowText(*this),
                    -1,
                    ClientRect,
                    DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        return 0;
    }

    return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

COLORREF TColorLabelControl::GetBGColor() const
{
    DEBUG_VERIFY_ALLOCATION;

    return m_crBGColor;
}

COLORREF TColorLabelControl::GetFGColor() const
{
    DEBUG_VERIFY_ALLOCATION;

    return m_crFGColor;
}

void TColorLabelControl::SetBGColor(COLORREF crSBGColor)
{
    DEBUG_VERIFY_ALLOCATION;

    if(m_crBGColor == crSBGColor)
        return;

    m_crBGColor = crSBGColor;

    InvalidateRect(*this, NULL, FALSE);
}

void TColorLabelControl::SetFGColor(COLORREF crSFGColor)
{
    DEBUG_VERIFY_ALLOCATION;

    if(m_crFGColor == crSFGColor)
        return;

    m_crFGColor = crSFGColor;

    InvalidateRect(*this, NULL, FALSE);
}

void TColorLabelControl::Move(  const IPOINT&   Coords,
                                const ISIZE&    Size,
                                const ALSIZE&   Alignment,
                                HWND            hInsertAfter)
{
    DEBUG_VERIFY_ALLOCATION;

    ISIZE TextSize;

    {
        TDC DC(*this);

        TGDIObjectSelector Selector0(DC, GetStockObject(DEFAULT_GUI_FONT));

        TextSize = TO_I(GetTextSize(DC, GetKWindowText(*this)));
    }

    TextSize.cx += Size.cx;
    TextSize.cy += Size.cy;

    SetWindowPos(   *this,
                    hInsertAfter,
                    Dealign(Coords.x, TextSize.cx, Alignment.cx),
                    Dealign(Coords.y, TextSize.cy, Alignment.cy),
                    TextSize.cx,
                    TextSize.cy,
                    hInsertAfter == NULL ? SWP_NOZORDER : 0);
}

HWND TColorLabelControl::GetWnd() const
{
    DEBUG_VERIFY_ALLOCATION;

    return m_hWnd;
}
