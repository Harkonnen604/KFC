#include "kfc_interface_pch.h"
#include "control.h"

#include <KFC_Graphics\graphics_device_globals.h>
#include <KFC_Input\input_device_globals.h>
#include "interface_tokens.h"
#include "interface.h"
#include "interface_message_defs.h"
#include "ctrl_deref.h"

// --------------
// Control state
// --------------
void TControlState::Reset()
{
    m_Rect.m_CommonPart.    Set(0.0f, 0.0f, 1.0f, 1.0f);
    m_Rect.m_OwnPart.       Set(0.0f, 0.0f, 1.0f, 1.0f);
    m_Rect.m_ChildrenPart.  Set(0.0f, 0.0f, 1.0f, 1.0f);

    m_HoverRect.        Set(0.0f, 0.0f, 1.0f, 1.0f);
    m_HotPointerRect.   Set(0.0f, 0.0f, 1.0f, 1.0f);

    m_Color.m_CommonPart    = WhiteColor();
    m_Color.m_OwnPart       = WhiteColor();
    m_Color.m_ChildrenPart  = WhiteColor();

    m_Visibility.m_OwnPart      = 1.0f;
    m_Visibility.m_ChildrenPart = 1.0f;

    m_Enablement.m_OwnPart      = 1.0f;
    m_Enablement.m_ChildrenPart = 1.0f;

    m_Visibility.m_CommonPart = 1.0f;
    m_Enablement.m_CommonPart = 1.0f;
}

void TControlState::SetScreenState( const TControlState* pParentScreenState,
                                    const TControlState& ClientState)
{
    //  Temp rect
    TControlState TempParentScreenState;
    if(pParentScreenState == NULL)
    {
        // Rect
        TempParentScreenState.m_Rect.m_ChildrenPart = g_GraphicsDeviceGlobals.m_ScreenRect;

        // Color
        TempParentScreenState.m_Color.m_ChildrenPart = WhiteColor();

        // Visibility
        TempParentScreenState.m_Visibility.m_ChildrenPart = 1.0f;

        // Enablement
        TempParentScreenState.m_Enablement.m_ChildrenPart = 1.0f;

        pParentScreenState = &TempParentScreenState;
    }

    // Rect
    ResizeRect( FRECT(0.0f, 0.0f, 1.0f, 1.0f),
                pParentScreenState->m_Rect.m_ChildrenPart,
                ClientState.m_Rect.m_CommonPart,
                m_Rect.m_CommonPart);

    ResizeRect( FRECT(0.0f, 0.0f, 1.0f, 1.0f),
                m_Rect.m_CommonPart,
                ClientState.m_Rect.m_OwnPart,
                m_Rect.m_OwnPart);

    ResizeRect( FRECT(0.0f, 0.0f, 1.0f, 1.0f),
                m_Rect.m_CommonPart,
                ClientState.m_Rect.m_ChildrenPart,
                m_Rect.m_ChildrenPart);

    // Hover rect
    ResizeRect( FRECT(0.0f, 0.0f, 1.0f, 1.0f),
                m_Rect.m_OwnPart,
                ClientState.m_HoverRect,
                m_HoverRect);

    // Hot pointer rect
    ResizeRect( FRECT(0.0f, 0.0f, 1.0f, 1.0f),
                m_Rect.m_OwnPart,
                ClientState.m_HotPointerRect,
                m_HotPointerRect);

    // Color
    m_Color.m_CommonPart =  pParentScreenState->m_Color.m_ChildrenPart *
                                ClientState.m_Color.m_CommonPart;

    m_Color.m_OwnPart = m_Color.m_CommonPart *
                            ClientState.m_Color.m_OwnPart;

    m_Color.m_ChildrenPart =    m_Color.m_CommonPart *
                                    ClientState.m_Color.m_ChildrenPart;

    // Visibility
    m_Visibility.m_CommonPart = pParentScreenState->m_Visibility.m_ChildrenPart *
                                    ClientState.m_Visibility.m_CommonPart;

    m_Visibility.m_OwnPart =    m_Visibility.m_CommonPart *
                                    ClientState.m_Visibility.m_OwnPart;

    m_Visibility.m_ChildrenPart =   m_Visibility.m_CommonPart *
                                        ClientState.m_Visibility.m_ChildrenPart;

    // Enablement
    m_Enablement.m_CommonPart = pParentScreenState->m_Enablement.m_ChildrenPart *
                                    ClientState.m_Enablement.m_CommonPart;

    m_Enablement.m_OwnPart =    m_Enablement.m_CommonPart *
                                    ClientState.m_Enablement.m_OwnPart;

    m_Enablement.m_ChildrenPart =   m_Enablement.m_CommonPart *
                                        ClientState.m_Enablement.m_ChildrenPart;

    // Cross-influences
    if(!m_Rect.m_OwnPart.IsValid())
        m_Visibility.m_OwnPart = 0.0f;

    m_Color.m_OwnPart.m_fAlpha *= m_Visibility.m_OwnPart;

    m_Enablement.m_OwnPart *= m_Visibility.m_OwnPart;
}

void TControlState::Load(   TInfoNodeConstIterator  InfoNode,
                            const TControl*         pParentControl,
                            const FRECT&            Resolution)
{
    TInfoParameterConstIterator PIter;

    // Getting common rect
    if((PIter = InfoNode->FindParameter(TEXT("CommonRect"))).IsValid())
    {
        ReadRectResized(PIter->m_Value,
                        m_Rect.m_CommonPart,
                        TEXT("control common rect"),
                        pParentControl,
                        Resolution);
    }

    // Evaluating common screen rect
    FRECT CommonScreenRect;

    ResizeRect( FRECT(0.0f, 0.0f, 1.0f, 1.0f),
                GetControlChildrenScreenRect(pParentControl),
                m_Rect.m_CommonPart,
                CommonScreenRect);

    // Getting own rect
    if((PIter = InfoNode->FindParameter(TEXT("OwnRect"))).IsValid())
    {
        ReadRectResized(PIter->m_Value,
                        m_Rect.m_OwnPart,
                        TEXT("control own rect"),
                        CommonScreenRect,
                        Resolution);
    }

    // Evaluating own screen rect
    FRECT OwnScreenRect;

    ResizeRect( FRECT(0.0f, 0.0f, 1.0f, 1.0f),
                CommonScreenRect,
                m_Rect.m_OwnPart,
                OwnScreenRect);

    // Getting children rect
    if((PIter = InfoNode->FindParameter(TEXT("ChildrenRect"))).IsValid())
    {
        ReadRectResized(PIter->m_Value,
                        m_Rect.m_ChildrenPart,
                        TEXT("control children rect"),
                        CommonScreenRect,
                        Resolution);
    }

    // Getting hover rect
    if((PIter = InfoNode->FindParameter(TEXT("HoverRect"))).IsValid())
    {
        ReadRectResized(PIter->m_Value,
                        m_HoverRect,
                        TEXT("control hover rect"),
                        OwnScreenRect,
                        Resolution);
    }

    // Getting hot pointer rect
    if((PIter = InfoNode->FindParameter(TEXT("HotPointerRect"))).IsValid())
    {
        ReadRectResized(PIter->m_Value,
                        m_HotPointerRect,
                        TEXT("control hot pointer rect"),
                        OwnScreenRect,
                        Resolution);
    }

    // Gettting common color
    if((PIter = InfoNode->FindParameter(TEXT("CommonColor"))).IsValid())
    {
        ReadColor(  PIter->m_Value,
                    m_Color.m_CommonPart,
                    TEXT("control common color"));
    }

    // Getting own color
    if((PIter = InfoNode->FindParameter(TEXT("OwnColor"))).IsValid())
    {
        ReadColor(  PIter->m_Value,
                    m_Color.m_OwnPart,
                    TEXT("control own color"));
    }

    // Getting children color
    if((PIter = InfoNode->FindParameter(TEXT("ChildrenColor"))).IsValid())
    {
        ReadColor(  PIter->m_Value,
                    m_Color.m_ChildrenPart,
                    TEXT("control children color"));
    }

    // Getting common visibility
    if((PIter = InfoNode->FindParameter(TEXT("CommonVisibility"))).IsValid())
    {
        ReadNormalizedFloat(PIter->m_Value,
                            m_Visibility.m_CommonPart,
                            TEXT("control common visibility"));
    }

    // Getting own visibility
    if((PIter = InfoNode->FindParameter(TEXT("OwnVisibility"))).IsValid())
    {
        ReadNormalizedFloat(PIter->m_Value,
                            m_Visibility.m_OwnPart,
                            TEXT("control own visibility"));
    }

    // Getting children visibility
    if((PIter = InfoNode->FindParameter(TEXT("ChildrenVisibility"))).IsValid())
    {
        ReadNormalizedFloat(PIter->m_Value,
                            m_Visibility.m_ChildrenPart,
                            TEXT("control children visibility"));
    }

    // Getting common enablement
    if((PIter = InfoNode->FindParameter(TEXT("CommonEnablement"))).IsValid())
    {
        ReadNormalizedFloat(PIter->m_Value,
                            m_Enablement.m_CommonPart,
                            TEXT("control common enablement"));
    }

    // Getting own enablement
    if((PIter = InfoNode->FindParameter(TEXT("OwnEnablement"))).IsValid())
    {
        ReadNormalizedFloat(PIter->m_Value,
                            m_Enablement.m_OwnPart,
                            TEXT("control own enablement"));
    }

    // Getting children enablement
    if((PIter = InfoNode->FindParameter(TEXT("ChildrenEnablement"))).IsValid())
    {
        ReadNormalizedFloat(PIter->m_Value,
                            m_Enablement.m_ChildrenPart,
                            TEXT("control children enablement"));
    }
}

// --------------------
// Control input state
// --------------------
void TControlInputState::Update(TControl* pControl)
{
    FPOINT ScreenMouseCoords;

    ResizePoint(g_InterfaceDeviceGlobals.m_MouseRect,
                g_GraphicsDeviceGlobals.m_ScreenRect,
                g_InterfaceDeviceGlobals.m_MouseCoords,
                ScreenMouseCoords);

    const TControlState& ScreenState = pControl->GetCurrentScreenState();

    if(ScreenState.IsEnabled())
    {
        m_bHovered = HitsRect(ScreenMouseCoords, ScreenState.m_HoverRect);

        const TControl* pMouseControl =
            g_InterfaceDeviceGlobals.GetMouseOwnerControl();

        m_bMouseOwner = pMouseControl ? pMouseControl == pControl : m_bHovered;

        m_bHovered &= m_bMouseOwner;
    }
    else
    {
        m_MouseCoords.Set(0.0f, 0.0f);

        m_bMouseOwner   = false;
        m_bHovered      = false;
    }
}

// ------------------------
// Control creation struct
// ------------------------
TControlCreationStruct::TControlCreationStruct()
{
    m_ClientState.Reset();

    m_szID                  = CONTROL_ID_NONE;
    m_HotPointerDirection   = HPD_DOWN;
    m_bSuspendOnHide        = false;
}

void TControlCreationStruct::Load(  TInfoNodeConstIterator  InfoNode,
                                    const TControl*         pParentControl,
                                    const FRECT&            Resolution)
{
    m_ClientState.Load(InfoNode, pParentControl, Resolution);

    TInfoParameterConstIterator PIter;

    // Getting ID
    if((PIter = InfoNode->FindParameter(TEXT("ID"))).IsValid())
    {
        ReadUINT(   CONTROL_ID_TOKENS.Process(PIter->m_Value),
                    m_szID,
                    TEXT("control ID"));
    }

    // Getting hot pointer direction
    if((PIter = InfoNode->FindParameter(TEXT("HotPointerDirection"))).IsValid())
    {
        ReadHotPointerDirection(PIter->m_Value,
                                m_HotPointerDirection,
                                TEXT("control hot pointer direction"));
    }

    // Getting hide suspension
    if((PIter = InfoNode->FindParameter(TEXT("SuspendOnHide"))).IsValid())
    {
        ReadBool(   PIter->m_Value,
                    m_bSuspendOnHide,
                    TEXT("control hide suspension"));
    }
}

// --------
// Control
// --------

TControl::TControl(const TControlCreationStruct& CreationStruct) :
    m_CurrentClientState(CreationStruct.m_ClientState)
{
    m_szID = CreationStruct.m_szID;

    m_HotPointerDirection = CreationStruct.m_HotPointerDirection;

    m_bSuspendOnHide = CreationStruct.m_bSuspendOnHide;
}

// Allocation
void TControl::OnAllocate()
{
}

void TControl::OnSetInitialValues()
{
    // Parent handle
    TControl* pParent = IterToCtrl(GetIter().GetParent());

    // Current input state
    m_CurrentInputState.Update(this);

    // Prev states
    m_PrevClientState   = m_CurrentClientState;
    m_PrevScreenState   = m_CurrentScreenState;
    m_PrevInputState    = m_CurrentInputState;

    // Getting parent suspend count
    size_t szSuspendCount =
        pParent ? pParent->GetSuspendCount() : 0;

    // Subtracting parent visibility suspension
    if( pParent &&
        pParent->m_bSuspendOnHide &&
        !pParent->GetCurrentScreenState().IsVisible())
    {
        szSuspendCount--;
    }

    // Adding own visibility suspension
    if( m_bSuspendOnHide &&
        !m_CurrentScreenState.IsVisible())
    {
        szSuspendCount++;
    }

    // Setting own suspend count
    SetSuspendCount(szSuspendCount);

    // Setting ID
    OnSetID();
}

// Appearance events
void TControl::OnShow()
{
    new(GetInterface().GetMessageProcessor())
        TControlVisibilityMessage(this, true);
}

void TControl::OnHide()
{
    new(GetInterface().GetMessageProcessor())
        TControlVisibilityMessage(this, false);
}

void TControl::OnEnable()
{
    new(GetInterface().GetMessageProcessor())
        TControlEnablementMessage(this, true);
}

void TControl::OnDisable()
{
    new(GetInterface().GetMessageProcessor())
        TControlEnablementMessage(this, false);
}

// Mouse events
void TControl::OnMouseButtonDown(size_t szButton)
{
    new(GetInterface().GetMessageProcessor())
        TControlMouseButtonMessage( this,
                                    szButton,
                                    true,
                                    m_CurrentInputState.m_MouseCoords);
}

void TControl::OnMouseButtonUp(size_t szButton)
{
    new(GetInterface().GetMessageProcessor())
        TControlMouseButtonMessage( this,
                                    szButton,
                                    false,
                                    m_CurrentInputState.m_MouseCoords);
}

void TControl::OnMouseMove()
{
    new(GetInterface().GetMessageProcessor())
        TControlMouseMovementMessage(   this,
                                        FSIZE(  m_PrevInputState.m_MouseCoords,
                                                m_CurrentInputState.m_MouseCoords),
                                        m_CurrentInputState.m_MouseCoords);
}

void TControl::OnMouseEnter()
{
    new(GetInterface().GetMessageProcessor())
        TControlMouseHoveringMessage(   this,
                                        true,
                                        m_CurrentInputState.m_MouseCoords);
}

void TControl::OnMouseLeave()
{
    new(GetInterface().GetMessageProcessor())
        TControlMouseHoveringMessage(   this,
                                        false,
                                        m_PrevInputState.m_MouseCoords);
}

void TControl::OnMouseCaptureLost()
{
}

// Keyboard events
void TControl::OnKeyboardKeyDown(size_t szKey)
{
/*  new(Interface.GetMessageProcessor())
        TControlKeyboardKeyMessage( this,
                                    szKey,
                                    true);*/ // overhead
}

void TControl::OnKeyboardKeyUp(size_t szKey)
{
/*  new(Interface.GetMessageProcessor())
        TControlKeyboardKeyMessage( this,
                                    szKey,
                                    false);*/ // overhead
}

void TControl::OnFocusLost()
{
    new(m_pInterface->GetMessageProcessor())
        TControlFocusMessage(   this,
                                false);
}

void TControl::OnFocusReceived()
{
    new(m_pInterface->GetMessageProcessor())
        TControlFocusMessage(   this,
                                true);
}

// Methods
TControl& TControl::Allocate(   TInterface&             SInterface,
                                TControls::TIterator    SIter)
{
    DEBUG_VERIFY(SIter.IsValid());

    // Setting persistence afffinity
    m_pInterface    = &SInterface;
    m_Iter          = SIter;

    TControl* pParent = IterToCtrl(m_Iter.GetParent());

    m_CurrentScreenState.SetScreenState(pParent ?
                                            &pParent->GetCurrentScreenState() :
                                            NULL,
                                        m_CurrentClientState);

    OnAllocate();

    SetInitialValues();

    return *this;
}

void TControl::SetInitialValues()
{
    OnSetInitialValues();
}

void TControl::Update()
{
    size_t i;

    // --- Updating input state ---
    m_CurrentInputState.Update(this);

    // --- Show resuming ---
    if( m_bSuspendOnHide &&
        !m_PrevScreenState.IsVisible() &&
        m_CurrentScreenState.IsVisible())
    {
        Resume();
    }

    // --- Pre-update event ---
    OnPreUpdate();

    // --- Visibility/enablement events ---
    if( !m_PrevScreenState.IsVisible() &&
        m_CurrentScreenState.IsVisible())
    {
        OnShow();
    }

    if( !m_PrevScreenState.IsEnabled() &&
        m_CurrentScreenState.IsEnabled())
    {
        OnEnable();
    }

    // --- Mouse events ---
    if(m_CurrentInputState.m_bMouseOwner)
    {
        if( m_PrevInputState.m_bMouseOwner &&
            m_CurrentInputState.m_MouseCoords != m_PrevInputState.m_MouseCoords)
        {
            OnMouseMove();
        }

        // Mouse button events
        for(i = 0 ; i < g_InputDeviceGlobals.m_ChangedMouseButtonStates.GetN() ; i++)
        {
            const size_t szValue = g_InputDeviceGlobals.m_ChangedMouseButtonStates[i];

            if(szValue & INPUT_STATE_CHANGE_VALUE_MASK)
                OnMouseButtonDown(szValue & INPUT_STATE_CHANGE_ITEM_MASK);
            else
                OnMouseButtonUp(szValue & INPUT_STATE_CHANGE_ITEM_MASK);
        }
    }

    // Checking for hovered state change
    if(m_CurrentInputState.m_bHovered != m_PrevInputState.m_bHovered)
    {
        if(m_CurrentInputState.m_bHovered)
            OnMouseEnter();
        else
            OnMouseLeave();
    }

    // --- Keyboard events ---

    // Keyboard key events
    if(m_CurrentScreenState.IsEnabled())
    {
        for(i = 0 ; i < g_InputDeviceGlobals.m_ChangedKeyboardKeyStates.GetN() ; i++)
        {
            const size_t szValue = g_InputDeviceGlobals.m_ChangedKeyboardKeyStates[i];

            if(szValue & INPUT_STATE_CHANGE_VALUE_MASK)
                OnKeyboardKeyDown(szValue & INPUT_STATE_CHANGE_ITEM_MASK);
            else
                OnKeyboardKeyUp(szValue & INPUT_STATE_CHANGE_ITEM_MASK);
        }
    }

    // --- Visibility/enablement events ---
    if( m_PrevScreenState.IsEnabled() &&
        !m_CurrentScreenState.IsEnabled())
    {
        OnDisable();
    }

    if( m_PrevScreenState.IsVisible() &&
        !m_CurrentScreenState.IsVisible())
    {
        OnHide();
    }

    // --- Post-update event ---
    OnPostUpdate();

    // --- Hide suspension ---
    if( m_bSuspendOnHide &&
        m_PrevScreenState.IsVisible() &&
        !m_CurrentScreenState.IsVisible())
    {
        Suspend();
    }

    // --- Storing previous states ---
    m_PrevClientState   = m_CurrentClientState;
    m_PrevScreenState   = m_CurrentScreenState;
    m_PrevInputState    = m_CurrentInputState;
}

void TControl::Render() const
{
    if(m_CurrentScreenState.m_Visibility.m_OwnPart > 0.0f)
        OnRender();
}

void TControl::SetID(size_t szSID)
{
    m_szID = szSID;

    OnSetID();
}

// Persistence access
TInterface& TControl::GetInterface()
{
    DEBUG_VERIFY(m_pInterface);

    return *m_pInterface;
}

const TInterface& TControl::GetInterface() const
{
    DEBUG_VERIFY(m_pInterface);

    return *m_pInterface;
}

TControls::TIterator TControl::GetIter()
{
    DEBUG_VERIFY(m_Iter.IsValid());

    return m_Iter;
}

TControls::TConstIterator TControl::GetIter() const
{
    DEBUG_VERIFY(m_Iter.IsValid());

    return m_Iter;
}

// Messages
bool TControl::IsValidMessage(const TControlMessage* pMessage) const
{
    DEBUG_VERIFY(pMessage);

    DEBUG_VERIFY(pMessage->m_pControl == this);

    return true;
}

// ----------------
// Global routines
// ----------------

// Common helpers
const FRECT& GetControlChildrenScreenRect(const TControl* pControl)
{
    return  pControl ?
                pControl->GetCurrentScreenState().m_Rect.m_ChildrenPart :
                g_GraphicsDeviceGlobals.m_ScreenRect;
}

// Point helpers
FPOINT& ReadPointResized(   const KString&  String,
                            FPOINT&         RPoint,
                            LPCTSTR         pValueName,
                            const TControl* pParentControl,
                            const FRECT&    Resolution)
{
    return ReadPointResized(String,
                            RPoint,
                            pValueName,
                            GetControlChildrenScreenRect(pParentControl),
                            Resolution);
}

// Rect helpers
FRECT& ReadRectResized( const KString&  String,
                        FRECT&          RRect,
                        LPCTSTR         pValueName,
                        const TControl* pParentControl,
                        const FRECT&    Resolution)
{
    return ReadRectResized( String,
                            RRect,
                            pValueName,
                            GetControlChildrenScreenRect(pParentControl),
                            Resolution);
}

// Size helpers
FSIZE& ReadSizeResized( const KString&  String,
                        FSIZE&          RSize,
                        LPCTSTR         pValueName,
                        const TControl* pParentControl,
                        const FRECT&    Resolution)
{
    return ReadSizeResized( String,
                            RSize,
                            pValueName,
                            GetControlChildrenScreenRect(pParentControl),
                            Resolution);
}
