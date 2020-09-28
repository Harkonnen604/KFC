#include "kfc_interface_pch.h"
#include "interactive_control.h"

#include <KFC_Common\timer.h>
#include <KFC_Input\keyboard_key_defs.h>
#include <KFC_Input\input_tokens.h>
#include <KFC_Input\input_device_globals.h>
#include <KFC_Sound\sound_storage.h>
#include "interface_consts.h"
#include "interface_sound_defs.h"
#include "interface.h"

// ------------------------------------
// Interactive control creation struct
// ------------------------------------
TInteractiveControlCreationStruct::TInteractiveControlCreationStruct()
{
    memcpy( m_InteractiveTransitionDelays,
            g_InterfaceConsts.m_DefaultInteractiveControlTransitionDelays,
            sizeof(m_InteractiveTransitionDelays));

    m_szHotKey = DIK_NONE;

    m_bAllowNonHoveredClicks    = false;
    m_bAllowMouseStealing       = true;

    m_szPushClickFirstDelay     = UINT_MAX;
    m_szPushClickRepeatDelay    = UINT_MAX;
}

void TInteractiveControlCreationStruct::Load(   TInfoNodeConstIterator  InfoNode,
                                                const TControl*         pParentControl,
                                                const FRECT&            Resolution)
{
    TControlCreationStruct::Load(InfoNode, pParentControl, Resolution);

    TInfoParameterConstIterator PIter;

    size_t i;

    // Getting transition delays
    if(InfoNode->HasTrueParameter(TEXT("ImmediateInteractiveTransition")))
    {
        SetImmediateInteractiveTransitions();
    }
    else
    {
        for(i = 0 ; i < 3 ; i++)
        {
            if((PIter = InfoNode->FindParameter((KString)TEXT("InteractiveTransitionDelay") + i)).IsValid())
            {
                ReadNormalizedFloat(PIter->m_Value,
                                    m_InteractiveTransitionDelays[i],
                                    (KString)TEXT("interactive transition delay ") + i,
                                    FSEGMENT(0.0, FLT_MAX));
            }
        }
    }

    // Getting hotkey
    if((PIter = InfoNode->FindParameter(TEXT("HotKey"))).IsValid())
    {
        ReadUINT(   KEYBOARD_KEY_TOKENS.Process(PIter->m_Value),
                    m_szHotKey,
                    TEXT("hotkey"));
    }

    // Geting non-hovered clicks allowance
    if((PIter = InfoNode->FindParameter(TEXT("AllowNonHoveredClicks"))).IsValid())
    {
        ReadBool(   PIter->m_Value,
                    m_bAllowNonHoveredClicks,
                    TEXT("non-hovered clicks allowance"));
    }

    // Getting mouse stealing allowance
    if((PIter = InfoNode->FindParameter(TEXT("AllowMouseStealing"))).IsValid())
    {
        ReadBool(   PIter->m_Value,
                    m_bAllowMouseStealing,
                    TEXT("mouse stealing allowance"));
    }
    else
    {
        m_bAllowMouseStealing = !AreAlternativeKeyboardKeys(m_szHotKey, DIK_ESCAPE);
    }

    // Getting push click first delay
    if((PIter = InfoNode->FindParameter(TEXT("PushClickFirstDelay"))).IsValid())
    {
        ReadUINT(   PIter->m_Value,
                    m_szPushClickFirstDelay,
                    TEXT("push click first delay"));
    }

    // Getting push click repeat delay
    if((PIter = InfoNode->FindParameter(TEXT("PushClickRepeatDelay"))).IsValid())
    {
        ReadUINT(   PIter->m_Value,
                    m_szPushClickRepeatDelay,
                    TEXT("push click repeat delay"));
    }
}

void TInteractiveControlCreationStruct::SetImmediateInteractiveTransitions()
{
    size_t i;

    for(i = 0 ; i < 3 ; i++)
        m_InteractiveTransitionDelays[i] = IMMEDIATE_TIME;
}

// ------------------------------------
// Interactive control sounds provider
// ------------------------------------
TInteractiveControlSoundsProvider::TInteractiveControlSoundsProvider()
{
    m_PushSound.    Allocate(SOUNDS_REGISTRATION_MANAGER[PUSH_SOUND_INDEX],     true);
    m_ClickSound.   Allocate(SOUNDS_REGISTRATION_MANAGER[CLICK_SOUND_INDEX],    true);
}

void TInteractiveControlSoundsProvider::Load(TInfoNodeConstIterator InfoNode)
{
    g_SoundStorage.LoadObject(InfoNode, TEXT("PushSound"),  m_PushSound,    true);
    g_SoundStorage.LoadObject(InfoNode, TEXT("ClickSound"), m_ClickSound,   true);
}

// --------------------
// Interactive control
// --------------------
TInteractiveControl::TInteractiveControl(   const TInteractiveControlCreationStruct&    CreationStruct,
                                            TInteractiveControlSoundsProvider&          SoundsProvider) :

    TControl(CreationStruct)

{
    memcpy( m_InteractiveTransitionDelays,
            CreationStruct.m_InteractiveTransitionDelays,
            sizeof(m_InteractiveTransitionDelays));

    m_szHotKey = CreationStruct.m_szHotKey;

    m_PushState = PS_NONE;

    m_bAllowNonHoveredClicks    = CreationStruct.m_bAllowNonHoveredClicks;
    m_bAllowMouseStealing       = CreationStruct.m_bAllowMouseStealing;

    m_szPushClickFirstDelay     = CreationStruct.m_szPushClickFirstDelay;
    m_szPushClickRepeatDelay    = CreationStruct.m_szPushClickRepeatDelay;

    m_PushSound.    ReOwn(SoundsProvider.m_PushSound);
    m_ClickSound.   ReOwn(SoundsProvider.m_ClickSound);
}

void TInteractiveControl::OnAllocate()
{
    TControl::OnAllocate();

    // Interactive state
    m_InteractiveState.Allocate(m_InteractiveTransitionDelays);
}

void TInteractiveControl::OnSetInitialValues()
{
    TControl::OnSetInitialValues();

    // Visible state
    m_InteractiveState.SetState(DetermineInteractiveState(), true);

    // Push click parameters
    m_PushClickTimer.Allocate();
    m_bFirstPushClickDelay = true;
}

// Update/render events
bool TInteractiveControl::OnSuspend()
{
    if(!TControl::OnSuspend())
        return false;

    m_InteractiveState.Suspend();

    m_PushClickTimer.Suspend();

    return true;
}

bool TInteractiveControl::OnResume()
{
    if(!TControl::OnResume())
        return false;

    m_InteractiveState.Resume();

    m_PushClickTimer.Resume();

    return true;
}

void TInteractiveControl::OnPreUpdate()
{
    TControl::OnPreUpdate();

    // Interactive state
    m_InteractiveState.Update();
}

void TInteractiveControl::OnPostUpdate()
{
    // Push clicks
    if(m_PushState != PS_NONE)
    {
        size_t szCompareTime =  m_bFirstPushClickDelay ?
                                    m_szPushClickFirstDelay :
                                    m_szPushClickRepeatDelay;

        if(szCompareTime == UINT_MAX)
            return;

        if((size_t)m_PushClickTimer >= szCompareTime)
        {
            PushClick(m_PushState == PS_MOUSE);
            m_PushClickTimer.Reset(), m_bFirstPushClickDelay = false;
        }
    }
}

// Appearance events
void TInteractiveControl::OnEnable()
{
    TControl::OnEnable();

    m_InteractiveState.SetState(DetermineInteractiveState(), false);
}

void TInteractiveControl::OnDisable()
{
    TControl::OnDisable();

    m_InteractiveState.SetState(DetermineInteractiveState(), false);
}

// Mouse events
void TInteractiveControl::OnMouseButtonDown(size_t szButton)
{
    TControl::OnMouseButtonDown(szButton);

    if(szButton == 0)
        Push(true);
}

void TInteractiveControl::OnMouseButtonUp(size_t szButton)
{
    if(m_PushState != PS_MOUSE || szButton != 0)
        return;

    Click(true);

    Unpush(true);

    TControl::OnMouseButtonUp(szButton);
}

void TInteractiveControl::OnMouseEnter()
{
    TControl::OnMouseEnter();

    m_InteractiveState.SetState(DetermineInteractiveState(), false);
}

void TInteractiveControl::OnMouseLeave()
{
    m_InteractiveState.SetState(DetermineInteractiveState(), false);

    TControl::OnMouseLeave();
}

void TInteractiveControl::OnMouseCaptureLost()
{
    TControl::OnMouseCaptureLost();

    Unpush(false);
}

// Keyboard events
void TInteractiveControl::OnKeyboardKeyDown(size_t szKey)
{
    TControl::OnKeyboardKeyDown(szKey);

    if(szKey == m_szHotKey)
        Push(false);
    else if(szKey == DIK_ESCAPE)
        Unpush(false);
}

void TInteractiveControl::OnKeyboardKeyUp(size_t szKey)
{
    TControl::OnKeyboardKeyUp(szKey);

    if(m_PushState != PS_KEYBOARD || m_szHotKey == 0)
        return;

    if(szKey == m_szHotKey)
    {
        Click(false);

        Unpush(false);
    }
}

// Push/unpush events
void TInteractiveControl::OnPush(bool bFromMouse)
{
    m_PushState = bFromMouse ? PS_MOUSE : PS_KEYBOARD;

    m_InteractiveState.SetState(DetermineInteractiveState(), false);

    if(m_PushSound.IsAllocated())
        m_PushSound->Play();

    new(GetInterface().GetMessageProcessor())
        TInteractiveControlPushMessage(this, true, bFromMouse);
}

void TInteractiveControl::OnUnpush(bool bFromMouse)
{
    m_PushState = PS_NONE;

    m_InteractiveState.SetState(DetermineInteractiveState(), false);

    new(GetInterface().GetMessageProcessor())
        TInteractiveControlPushMessage(this, false, bFromMouse);
}

// Click events
void TInteractiveControl::OnClick(bool bFromMouse)
{
    if(m_ClickSound.IsAllocated())
        m_ClickSound->Play();

    new(GetInterface().GetMessageProcessor())
        TInteractiveControlClickMessage(this, bFromMouse);
}

void TInteractiveControl::OnPushClick(bool bFromMouse)
{
    new(GetInterface().GetMessageProcessor())
        TInteractiveControlPushClickMessage(this, bFromMouse);
}

// Push/unpush rouitines
void TInteractiveControl::Push(bool bFromMouse)
{
    if( !m_bAllowMouseStealing &&
        g_InterfaceDeviceGlobals.IsMouseOwned())
    {
        return;
    }

    g_InterfaceDeviceGlobals.SetMouseOwnerControl(this, true);

    OnPush(bFromMouse);

    m_PushClickTimer.Reset(), m_bFirstPushClickDelay = true;

    PushClick(bFromMouse);
}

void TInteractiveControl::Unpush(bool bFromMouse)
{
    if(m_PushState != PS_NONE)
    {
        OnUnpush(bFromMouse);

        g_InterfaceDeviceGlobals.InvalidateMouseOwnership(this, false);
    }
}

// Click routines
void TInteractiveControl::Click(bool bFromMouse)
{
    if(m_PushState == PS_NONE)
        return;

    if(bFromMouse && !m_bAllowNonHoveredClicks && !GetCurrentInputState().m_bHovered)
        return;

    OnClick(bFromMouse);
}

void TInteractiveControl::PushClick(bool bFromMouse)
{
    if(m_PushState == PS_NONE)
        return;

    if(bFromMouse && !m_bAllowNonHoveredClicks && !GetCurrentInputState().m_bHovered)
        return;

    OnPushClick(bFromMouse);
}

TInteractiveControl::TInteractiveState TInteractiveControl::DetermineInteractiveState() const
{
    TInteractiveState InteractiveState;

    const TControlState&        ScreenState = GetCurrentScreenState ();
    const TControlInputState&   InputState  = GetCurrentInputState  ();

    if(ScreenState.IsEnabled())
    {
        if(m_PushState == PS_NONE)
        {
            InteractiveState = InputState.m_bHovered ? IS_HOVERED : IS_NONHOVERED;
        }
        else if(m_PushState == PS_MOUSE)
        {
            InteractiveState =  m_bAllowNonHoveredClicks || InputState.m_bHovered ?
                                    IS_PUSHED :
                                    IS_HOVERED;
        }
        else if(m_PushState == PS_KEYBOARD)
        {
            InteractiveState = IS_PUSHED;
        }
        else
        {
            INITIATE_FAILURE;
        }
    }
    else
    {
        InteractiveState = IS_DISABLED;
    }

    return InteractiveState;
}

// Messages
bool TInteractiveControl::IsValidMessage(const TControlMessage* pMessage) const
{
    if(!TControl::IsValidMessage(pMessage))
        return false;

    // Push message
    const TInteractiveControlPushMessage* pPushMessage =
        dynamic_cast<const TInteractiveControlPushMessage*>(pMessage);

    if(pPushMessage)
        return GetCurrentScreenState().IsEnabled();

    // Click message
    const TInteractiveControlClickMessage* pClickMessage =
        dynamic_cast<const TInteractiveControlClickMessage*>(pMessage);

    if(pClickMessage)
        return GetCurrentScreenState().IsEnabled();

    // Push click message
    const TInteractiveControlPushClickMessage* pPushClickMessage =
        dynamic_cast<const TInteractiveControlPushClickMessage*>(pMessage);

    if(pPushClickMessage)
        return GetCurrentScreenState().IsEnabled();

    return true;
}

// ----------------
// Global routines
// ----------------
bool IsInteractiveControl(const TControl* pControl)
{
    return dynamic_cast<const TInteractiveControl*>(pControl) != NULL;
}
