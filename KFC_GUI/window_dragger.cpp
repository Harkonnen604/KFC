#include "kfc_gui_pch.h"
#include "window_dragger.h"

#include "gui.h"

// ---------------
// Window dragger
// ---------------
TWindowDragger::TWindowDragger()
{
    m_hWnd = NULL;
}

void TWindowDragger::Release()
{
    if(m_hWnd && m_bDragged)
        ReleaseCapture(m_hWnd);

    m_hWnd = NULL;
}

void TWindowDragger::Allocate(  HWND            hWnd,
                                const IRECT&    DragRect,
                                const IRECT&    LimitRect)
{
    Release();

    try
    {
        DEBUG_VERIFY(hWnd);

        m_hWnd = hWnd;

        m_DragRect = DragRect;

        m_LimitRect = LimitRect;

        m_bDragged = false;

        ResetWindowCoords();
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void TWindowDragger::SetDragRect(const IRECT& DragRect)
{
    DEBUG_VERIFY_ALLOCATION;

    m_DragRect = DragRect;
}

void TWindowDragger::SetLimitRect(const IRECT& LimitRect)
{
    DEBUG_VERIFY_ALLOCATION;

    m_LimitRect = LimitRect;

    ResetWindowCoords();
}

bool TWindowDragger::ProcessMessage(UINT uiMsg)
{
    if(!IsAllocated())
        return false;

    if(uiMsg == WM_LBUTTONDOWN)
    {
        if(!m_bDragged)
        {
            IPOINT MouseCoords = GetKCursorPos();

            if(m_DragRect.IsValid() && !HitsRect(KScreenToClient(m_hWnd, make_temp(MouseCoords)()), m_DragRect))
                return false;

            IRECT Rect = GetWindowPlacementRect(m_hWnd);

            m_MouseCoordsOffset.cx = Rect.m_Left - MouseCoords.x;
            m_MouseCoordsOffset.cy = Rect.m_Top  - MouseCoords.y;

            SetCapture(m_hWnd);

            m_bDragged = true;

            UpdateWindowPos();

            return true;
        }
    }
    else if(uiMsg == WM_MOUSEMOVE)
    {
        if(m_bDragged)
        {
            UpdateWindowPos();

            return true;
        }
    }
    else if(uiMsg == WM_LBUTTONUP)
    {
        if(m_bDragged)
        {
            UpdateWindowPos();

            m_bDragged = false;

            ReleaseCapture(m_hWnd);

            return true;
        }
    }

    return false;
}

void TWindowDragger::UpdateWindowPos()
{
    DEBUG_VERIFY(m_bDragged);

    IPOINT MouseCoords;
    GetCursorPos(MouseCoords);

    SetWindowCoords(IPOINT( MouseCoords.x + m_MouseCoordsOffset.cx,
                            MouseCoords.y + m_MouseCoordsOffset.cy));
}

void TWindowDragger::ResetWindowCoords()
{
    SetWindowCoords(GetWindowPlacementRect(m_hWnd).GetTopLeft());
}

void TWindowDragger::SetWindowCoords(IPOINT Coords)
{
    const IRECT Rect = GetWindowPlacementRect(m_hWnd);

    const ISIZE Size = Rect;

    if(m_LimitRect.m_Left < m_LimitRect.m_Right)
    {
        UpdateMax(Coords.x, m_LimitRect.m_Right - Size.cx);
        UpdateMin(Coords.x, m_LimitRect.m_Left);
    }

    if(m_LimitRect.m_Top < m_LimitRect.m_Bottom)
    {
        UpdateMax(Coords.x, m_LimitRect.m_Bottom - Size.cy);
        UpdateMin(Coords.y, m_LimitRect.m_Top);
    }

    if(Rect.GetTopLeft() != Coords)
    {
        SetWindowPos(   m_hWnd,
                        NULL,
                        Coords.x, Coords.y,
                        0, 0,
                        SWP_NOSIZE | SWP_NOZORDER);
    }
}
