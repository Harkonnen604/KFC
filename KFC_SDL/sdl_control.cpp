#include "kfc_sdl_pch.h"
#include "sdl_control.h"

#include "sdl_consts.h"
#include "sdl_interface.h"

// ------------
// SDL control
// ------------

// SDL control own definition
T_SDL_ControlOwnDefinition::T_SDL_ControlOwnDefinition()
{
    m_szID = UINT_MAX;

    m_Rect.Invalidate();

    m_bVisible = true;

    m_bEnabled = true;
}

// SDL control
T_SDL_Control::T_SDL_Control(const TDefinition& Definition) :
    T_SDL_ControlOwnDefinition(Definition)
{
    m_pInterface = NULL;

    m_szBlockRedrawCount = 0;

    m_bHasDelayedRedraw = false;
}

void T_SDL_Control::Attach(T_SDL_Interface& Interface, T_SDL_ControlHandle Handle)
{
    DEBUG_VERIFY(!IsAttached());

    DEBUG_VERIFY(Handle.IsValid());

    m_pInterface = &Interface, m_Handle = Handle;

    OnAttach();
}

void T_SDL_Control::SetRect(const IRECT& Rect)
{
    DEBUG_VERIFY(Rect.IsFlatValid());

    if(m_Rect == Rect)
        return;

    if(IsAttached())
        m_pInterface->SetControlRect(m_Handle, Rect);
    else
        m_Rect = Rect;
}

bool T_SDL_Control::CanDraw() const
{
    if(!IsAttached() || !IsVisible() || !m_pInterface->CanDraw())
        return false;

    if(IsRedrawBlocked())
    {
        m_bHasDelayedRedraw = true;
        return false;
    }

    return true;
}

IRECT T_SDL_Control::GetFocusRect(const SZSIZE& ExtFocusSize) const
{
    DEBUG_VERIFY(m_Rect.IsFlatValid());

    DEBUG_VERIFY(DoesNeedExtFocus());

    IRECT FocusRect = m_Rect;

    LessenRect(FocusRect, -TO_I(ExtFocusSize));

    return FocusRect;
}

void T_SDL_Control::Show(bool bShow)
{
    if(!IsAttached())
    {
        m_bVisible = bShow;
        return;
    }

    m_pInterface->ShowControl(m_Handle, bShow);
}

void T_SDL_Control::ShowOverlaid(bool bSetFocus)
{
    DEBUG_VERIFY(IsAttached());

    m_pInterface->ShowOverlaidControl(m_Handle, true, bSetFocus);
}

void T_SDL_Control::HideOverlaid()
{
    DEBUG_VERIFY(IsAttached());

    m_pInterface->ShowOverlaidControl(m_Handle, false, false);
}

void T_SDL_Control::Enable(bool bEnable)
{
    if(!IsAttached())
    {
        m_bEnabled = bEnable;
        return;
    }

    m_pInterface->EnableControl(m_Handle, bEnable);
}

void T_SDL_Control::SetFocus()
{
    DEBUG_VERIFY(IsAttached());

    m_pInterface->SetFocus(m_Handle);
}

void T_SDL_Control::RemoveFocus()
{
    DEBUG_VERIFY(IsAttached());

    if(m_pInterface->GetFocus() == m_Handle)
        m_pInterface->SetFocus(T_SDL_ControlHandle());
}

void T_SDL_Control::Redraw(bool bFocusChange) const
{
    if(!CanDraw())
        return;

    DEBUG_VERIFY(m_Rect.IsFlatValid());

    OnDraw(bFocusChange);

    m_bHasDelayedRedraw = false;
}

void T_SDL_Control::AddTimer(size_t szID, size_t szPeriod)
{
    DEBUG_VERIFY(IsAttached());

    GetInterface().GetTimers().Add(this, szID, szPeriod);
}

bool T_SDL_Control::RemoveTimer(size_t szID)
{
    DEBUG_VERIFY(IsAttached());

    return GetInterface().GetTimers().Remove(this, szID);
}

void T_SDL_Control::RemoveAllTimers()
{
    DEBUG_VERIFY(IsAttached());

    return GetInterface().GetTimers().Remove(this);
}

size_t T_SDL_Control::SuspendTimer(size_t szID)
{
    DEBUG_VERIFY(IsAttached());

    return GetInterface().GetTimers().Suspend(this, szID);
}

size_t T_SDL_Control::ResumeTimer(size_t szID)
{
    DEBUG_VERIFY(IsAttached());

    return GetInterface().GetTimers().Resume(this, szID);
}

void T_SDL_Control::ResetTimer(size_t szID)
{
    DEBUG_VERIFY(IsAttached());

    return GetInterface().GetTimers().Reset(this, szID);
}

bool T_SDL_Control::IsFocused() const
{
    DEBUG_VERIFY(IsAttached());

    return m_pInterface->GetFocus() == m_Handle;
}
