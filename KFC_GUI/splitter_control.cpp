#include "kfc_gui_pch.h"
#include "splitter_control.h"

#include "gui.h"

// -----------------
// Splitter control
// -----------------
ATOM TSplitterControl::ms_aHClass = 0;
ATOM TSplitterControl::ms_aVClass = 0;

UINT TSplitterControl::ms_uiResizeMessage = WM_NULL;

void TSplitterControl::Unregister()
{
    ms_uiResizeMessage = WM_NULL;

    if(ms_aVClass)
        UnregisterClass((LPCTSTR)ms_aVClass, GetModuleHandle(NULL));

    if(ms_aHClass)
        UnregisterClass((LPCTSTR)ms_aHClass, GetModuleHandle(NULL));
}

void TSplitterControl::Register()
{
    Unregister();

    {
        WNDCLASS wc;
        memset(&wc, 0, sizeof(wc));

        wc.lpfnWndProc      = StaticWindowProc;
        wc.hInstance        = GetModuleHandle(NULL);
        wc.hCursor          = LoadCursor(NULL, IDC_SIZENS);
        wc.lpszClassName    = TEXT("{2306B9F4-1E31-42b0-B96E-2A013D585250}HSplitterClass");

        ms_aHClass = RegisterClass(&wc);

        if(!ms_aHClass)
        {
            INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error registering horizontal splitter control window class"),
                                            GetLastError());
        }
    }

    {
        WNDCLASS wc;
        memset(&wc, 0, sizeof(wc));

        wc.lpfnWndProc      = StaticWindowProc;
        wc.hInstance        = GetModuleHandle(NULL);
        wc.hCursor          = LoadCursor(NULL, IDC_SIZEWE);
        wc.lpszClassName    = TEXT("{2306B9F4-1E31-42b0-B96E-2A013D585250}VSplitterClass");

        ms_aVClass = RegisterClass(&wc);

        if(!ms_aVClass)
        {
            INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error registering vertical splitter control window class"),
                                            GetLastError());
        }
    }

    ms_uiResizeMessage =
        RegisterWindowMessage(TEXT("{2306B9F4-1E31-42b0-B96E-2A013D585250}SplitterResizeMsg"));
}

TSplitterControl::TSplitterControl()
{
    m_hWnd = NULL;
}

void TSplitterControl::Release()
{
    if(m_hWnd)
        DestroyWindow(m_hWnd), m_hWnd = NULL;
}

void TSplitterControl::Create(  HWND            hParentWnd,
                                TType           Type,
                                const IRECT&    AreaRect,
                                const ISEGMENT& Range,
                                int             iCoords,
                                int             iThickness)
{
    Release();

    try
    {
        DEBUG_VERIFY(IsRegistered());

        DEBUG_VERIFY(hParentWnd);

        DEBUG_VERIFY(!(GetWindowLong(hParentWnd, GWL_STYLE) & WS_CLIPCHILDREN));

        DEBUG_VERIFY(iThickness > 0);

        m_Type = Type;

        m_AreaRect = AreaRect, m_Range = Range;

        m_iThickness = iThickness;

        Canonize();

        LimitValue(iCoords, ISEGMENT(m_Range.m_First, m_Range.m_Last - m_iThickness));

        ATOM aClass;

        IRECT Rect;

        switch(m_Type)
        {
        case TYPE_HORIZONTAL:
            aClass = ms_aHClass;

            Rect.m_Left     = m_AreaRect.m_Left;
            Rect.m_Top      = iCoords;
            Rect.m_Right    = m_AreaRect.m_Right;
            Rect.m_Bottom   = Rect.m_Top + m_iThickness;

            break;

        case TYPE_VERTICAL:
            aClass = ms_aVClass;

            Rect.m_Left     = iCoords;
            Rect.m_Top      = m_AreaRect.m_Top;
            Rect.m_Right    = Rect.m_Left + m_iThickness;
            Rect.m_Bottom   = m_AreaRect.m_Bottom;

            break;

        default:
            INITIATE_FAILURE;
        }

        m_hWnd = CreateWindow(  (LPCTSTR)aClass,
                                TEXT(""),
                                WS_CHILD | WS_VISIBLE,
                                Rect.m_Left,
                                Rect.m_Top,
                                Rect.GetWidth (),
                                Rect.GetHeight(),
                                hParentWnd,
                                NULL,
                                GetModuleHandle(NULL),
                                NULL);

        if(!m_hWnd)
        {
            INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error creating splitter control"),
                                            GetLastError());
        }

        SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

        m_bDragged = false;

        m_hOldFocus = NULL;

        NotifyResize();
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void TSplitterControl::Canonize()
{
    switch(m_Type)
    {
    case TYPE_HORIZONTAL:
        UpdateMax(m_AreaRect.m_Bottom, m_AreaRect.m_Top + m_iThickness);
        UpdateMax(m_Range.m_First, m_AreaRect.m_Top);
        UpdateMin(m_Range.m_Last,  m_AreaRect.m_Bottom - m_iThickness);
        break;

    case TYPE_VERTICAL:
        UpdateMax(m_AreaRect.m_Right, m_AreaRect.m_Left + m_iThickness);
        UpdateMax(m_Range.m_First, m_AreaRect.m_Left);
        UpdateMin(m_Range.m_Last,  m_AreaRect.m_Right - m_iThickness);
        break;

    default:
        DEBUG_INITIATE_FAILURE;
    }

    if(!m_Range.IsFlatValid())
        m_Range.m_Last = m_Range.m_First;
}

IRECT TSplitterControl::GetDragRect()
{
    DEBUG_VERIFY(m_bDragged);

    IRECT Rect;

    switch(m_Type)
    {
    case TYPE_HORIZONTAL:
        Rect.m_Left     = m_AreaRect.m_Left;
        Rect.m_Top      = LimitValue<int>(temp<int>(m_DragCoords.y), m_Range);
        Rect.m_Right    = m_AreaRect.m_Right;
        Rect.m_Bottom   = Rect.m_Top + m_iThickness;
        break;

    case TYPE_VERTICAL:
        Rect.m_Left     = LimitValue<int>(temp<int>(m_DragCoords.x), m_Range);
        Rect.m_Top      = m_AreaRect.m_Top;
        Rect.m_Right    = Rect.m_Left + m_iThickness;
        Rect.m_Bottom   = m_AreaRect.m_Bottom;
        break;

    default:
        DEBUG_INITIATE_FAILURE;
    }

    return Rect;
}

void TSplitterControl::NotifyResize()
{
    IRECT Rect = GetWindowPlacementRect(*this);

    IRECT Rect1, Rect2;

    switch(m_Type)
    {
    case TYPE_HORIZONTAL:
        Rect1.Set(m_AreaRect.m_Left, m_AreaRect.m_Top, m_AreaRect.m_Right, Rect.m_Top);
        Rect2.Set(m_AreaRect.m_Left, Rect.m_Bottom, m_AreaRect.m_Right, m_AreaRect.m_Bottom);
        break;

    case TYPE_VERTICAL:
        Rect1.Set(m_AreaRect.m_Left, m_AreaRect.m_Top, Rect.m_Left, m_AreaRect.m_Bottom);
        Rect2.Set(Rect.m_Right, m_AreaRect.m_Top, m_AreaRect.m_Right, m_AreaRect.m_Bottom);
        break;

    default:
        INITIATE_FAILURE;
    }

    SendMessage(GetParent(*this), ms_uiResizeMessage, (WPARAM)&Rect1, (LPARAM)&Rect2);
}

void TSplitterControl::Adjust(const IRECT& AreaRect, const ISEGMENT& Range)
{
    DEBUG_VERIFY_ALLOCATION;

    if(m_bDragged)
        StopDrag(false);

    ISEGMENT OldRange = m_Range;

    m_AreaRect = AreaRect, m_Range = Range;

    Canonize();

    IRECT Rect = GetWindowPlacementRect(*this);

    switch(m_Type)
    {
    case TYPE_HORIZONTAL:
        SetWindowPos(   *this,
                        NULL,
                        m_AreaRect.m_Left,
                        m_Range.m_First +
                        (   !OldRange.IsValid() ? 0 :
                            (   ((Rect.m_Top - OldRange.m_First) *
                                    m_Range.GetLength() + OldRange.GetLength() / 2) /
                                        OldRange.GetLength())),
                        m_AreaRect.GetWidth(), m_iThickness,
                        SWP_NOZORDER);

        break;

    case TYPE_VERTICAL:
        SetWindowPos(   *this,
                        NULL,
                        m_Range.m_First +
                        (   !OldRange.IsValid() ? 0 :
                            (   ((Rect.m_Left - OldRange.m_First) *
                                    m_Range.GetLength() + OldRange.GetLength() / 2) /
                                        OldRange.GetLength())),
                        m_AreaRect.m_Top,
                        m_iThickness, m_AreaRect.GetHeight(),
                        SWP_NOZORDER);

        break;

    default:
        DEBUG_INITIATE_FAILURE;
    }

    NotifyResize();
}

LRESULT CALLBACK TSplitterControl::StaticWindowProc
    (HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    TSplitterControl* pControl;

    if(hWnd && (pControl = (TSplitterControl*)GetWindowLong(hWnd, GWL_USERDATA)))
        return pControl->WindowProc(hWnd, uiMsg, wParam, lParam);

    return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

LRESULT TSplitterControl::WindowProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uiMsg)
    {
    case WM_ERASEBKGND:
        return 0;

    case WM_PAINT:
        OnPaint();
        return 0;

    case WM_LBUTTONDOWN:
        StartDrag();
        return 0;

    case WM_MOUSEMOVE:
        Drag();
        return 0;

    case WM_LBUTTONUP:
        StopDrag(true);
        return 0;

    case WM_KILLFOCUS:
        StopDrag(false);
        return 0;

    case WM_KEYDOWN:
        StopDrag(false);
        return 0;

    case WM_CANCELMODE:
        StopDrag(false);
        return 0;
    }

    return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

void TSplitterControl::OnPaint()
{
    TPaintDC DC(*this);

    FillRect(DC, GetKClientRect(*this), GetSysColorBrush(COLOR_BTNFACE));
}

void TSplitterControl::StartDrag()
{
    if(m_bDragged)
        StopDrag(false);

    SetCapture(m_hWnd);

    m_hOldFocus = SetFocus(m_hWnd);

    m_bDragged = true;

    IPOINT Coords = GetKCursorPos();

    IRECT Rect = GetWindowPlacementRect(*this);

    m_DragOffset.Set(Rect.m_Left - Coords.x, Rect.m_Top - Coords.y);

    m_bDragged = true;

    DrawDrag(true);
}

void TSplitterControl::Drag()
{
    if(!m_bDragged)
        return;

    DrawDrag(false), DrawDrag(true);
}

void TSplitterControl::StopDrag(bool bAccept)
{
    if(!m_bDragged)
        return;

    DrawDrag(false);

    if(bAccept)
    {
        IRECT Rect = GetDragRect();

        SetWindowPos(   *this,
                        NULL,
                        Rect.m_Left,
                        Rect.m_Top,
                        Rect.GetWidth (),
                        Rect.GetHeight(),
                        SWP_NOZORDER);

        NotifyResize();
    }

    m_bDragged = false;

    ReleaseCapture(m_hWnd);

    if(!m_hOldFocus || IsWindow(m_hOldFocus) && IsWindowEnabled(m_hOldFocus))
        SetFocus(m_hOldFocus);
}

void TSplitterControl::DrawDrag(bool bUpdateCoords)
{
    DEBUG_VERIFY(m_bDragged);

    if(bUpdateCoords)
    {
        m_DragCoords = GetKCursorPos();

        m_DragCoords.x += m_DragOffset.cx, m_DragCoords.y += m_DragOffset.cy;
    }

    IRECT Rect = GetDragRect();

    InvertCheckerRect(TDC(GetParent(*this)), GetDragRect());
}
