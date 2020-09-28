#include "kfc_interface_pch.h"
#include "interface.h"

#include <KFC_KTL\text_container.h>
#include "controls_factory.h"
#include "check_container.h"
#include "ctrl_deref.h"

// ----------
// Interface
// ----------
TInterface::TInterface()
{
    m_bAllocated = false;
}

void TInterface::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        m_Controls.Clear();

        g_InterfaceDeviceGlobals.InvalidateMouseOwnership(this, false);

        m_MessageProcessor.Release();
    }
}

TControl* TInterface::Allocate(TControl* pDesktopControl)
{
    Release();

    try
    {
        // Suspendable
        ResetSuspendCount();

        // Message processor
        m_MessageProcessor.Allocate();

        // Desktop
        TControls::TIterator Iter = m_Controls.AddRoot();

        (*Iter = pDesktopControl)->Allocate(*this, Iter);

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }

    return GetDesktopControl();
}

TControl* TInterface::MergeControl( TControl* pSrcControl,
                                    TControl* pDstParentControl)
{
    TControl* pDstParentControl2 =
        AddLastChildControl(pSrcControl, pDstParentControl);

    for(TControls::TIterator SrcIter2 = CtrlToIter(pSrcControl).GetFirstChild() ;
        SrcIter2.IsValid() ;
        SrcIter2.ToNextSibling())
    {
        MergeControl(IterToCtrl(SrcIter2), pDstParentControl2);
    }

    return pDstParentControl2;
}

TControl* TInterface::Merge(TControl* pSrcControl,
                            TControl* pDstParentControl)
{
    DEBUG_VERIFY_ALLOCATION;

    return MergeControl(pSrcControl, pDstParentControl);
}

void TInterface::LoadControl(   TInfoNodeConstIterator  InfoNode,
                                TControl*               pParentControl,
                                const FRECT&            Resolution)
{
    // Loading new control
    TObjectPointer<TControl> Control(   g_ControlsFactory.LoadControl(InfoNode, pParentControl, Resolution),
                                        false);

    // Attaching the loaded control
    TControl* pControl =
        pParentControl ?
            AddLastChildControl(Control.GetDataPtr(), pParentControl) :
            Allocate(Control.GetDataPtr());

    // Invalidating safe pointer
    Control.MakeExternal();

    // Loading sub-controls
    TInfoNodeConstIterator InfoNode2;

    while((InfoNode2 = TStructuredInfo::FindNode(InfoNode, TEXT("Control"), InfoNode2)).IsValid())
        LoadControl(InfoNode2, pControl, Resolution);
}

void TInterface::Load(  TInfoNodeConstIterator  InfoNode,
                        const FRECT&            Resoluion)
{
    Release();

    try
    {
        DEBUG_VERIFY(InfoNode.IsValid());

        // Getting desktop info node
        TInfoNodeConstIterator DesktopNode =
            TStructuredInfo::FindNode(InfoNode, TEXT("Control"));

        if(!DesktopNode.IsValid())
            INITIATE_DEFINED_FAILURE(TEXT("Desktop control definition not found."));

        LoadControl(DesktopNode, NULL, Resoluion);

        if(TStructuredInfo::FindNode(InfoNode, TEXT("Control"), DesktopNode).IsValid())
            INITIATE_DEFINED_FAILURE(TEXT("Redundant sibling controls after desktop control."));
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

// Deletion
void TInterface::PreDeleteControl(TControl* pControl)
{
    g_InterfaceDeviceGlobals.InvalidateMouseOwnership(false);

    for(TControls::TIterator ControlIter2 = CtrlToIter(pControl).GetFirstChild() ;
        ControlIter2.IsValid() ;
        ControlIter2.ToNextSibling())
    {
        PreDeleteControl(IterToCtrl(ControlIter2));
    }
}

void TInterface::DeleteControl(TControl* pControl)
{
    DEBUG_VERIFY(pControl);

    TInterface& Interface = pControl->GetInterface();

    DEBUG_VERIFY(pControl != Interface.GetDesktopControl());

    Interface.PreDeleteControl(pControl);

    Interface.m_Controls.Del(CtrlToIter(pControl));
}

void TInterface::DetachControl(TControl* pControl)
{
    DEBUG_VERIFY(pControl);

    TInterface& Interface = pControl->GetInterface();

    DEBUG_VERIFY(pControl != Interface.GetDesktopControl());

    Interface.PreDeleteControl(pControl);

    Interface.m_Controls.ForEach(CtrlToIter(pControl), ForEach_Invalidator);
}

// Fast handle visibility setting
void TInterface::SetControlCommonVisibility(TControl*   pControl,
                                            float       fVisibility,
                                            bool        bSetInitialValues)
{
    TControlState State = pControl->GetCurrentClientState();
    State.m_Visibility.m_CommonPart = fVisibility;
    SetControlClientState(pControl, State, bSetInitialValues);
}

void TInterface::SetControlOwnVisibility(   TControl*   pControl,
                                            float       fVisibility,
                                            bool        bSetInitialValues)
{
    TControlState State = pControl->GetCurrentClientState();
    State.m_Visibility.m_OwnPart = fVisibility;
    SetControlClientState(pControl, State, bSetInitialValues);
}

void TInterface::SetControlChildrenVisibility(  TControl*   pControl,
                                                float       fVisibility,
                                                bool        bSetInitialValues)
{
    TControlState State = pControl->GetCurrentClientState();
    State.m_Visibility.m_ChildrenPart = fVisibility;
    SetControlClientState(pControl, State, bSetInitialValues);
}

// Fast handle enablement setting
void TInterface::SetControlCommonEnablement(TControl*   pControl,
                                            float       fEnablement,
                                            bool        bSetInitialValues)
{
    TControlState State = pControl->GetCurrentClientState();
    State.m_Enablement.m_CommonPart = fEnablement;
    SetControlClientState(pControl, State, bSetInitialValues);
}

void TInterface::SetControlOwnEnablement(   TControl*   pControl,
                                            float       fEnablement,
                                            bool        bSetInitialValues)
{
    TControlState State = pControl->GetCurrentClientState();
    State.m_Enablement.m_OwnPart = fEnablement;
    SetControlClientState(pControl, State, bSetInitialValues);
}

void TInterface::SetControlChildrenEnablement(  TControl*   pControl,
                                                float       fEnablement,
                                                bool        bSetInitialValues)
{
    TControlState State = pControl->GetCurrentClientState();
    State.m_Enablement.m_ChildrenPart = fEnablement;
    SetControlClientState(pControl, State, bSetInitialValues);
}

// Fash handle text getting
KString TInterface::GetControlText( const TControl* pControl,
                                    bool*           pRSuccess)
{
    bool bSuccess = false;

    KString Text;

    const TTextContainer* pTextContainer =
        dynamic_cast<const TTextContainer*>(pControl);

    if(pTextContainer)
        Text = pTextContainer->GetText(&bSuccess);

    if(pRSuccess)
        *pRSuccess = bSuccess;

    return Text;
}

// Fast handle text setting
bool TInterface::SetControlText(TControl*       pControl,
                                const KString&  Text,
                                bool            bSetInitialValues)
{
    bool bSuccess = false;

    TTextContainer* pTextContainer =
        dynamic_cast<TTextContainer*>(pControl);

    if(pTextContainer)
        bSuccess = pTextContainer->SetText(Text);

    if(bSuccess && bSetInitialValues)
        SetControlInitialValues(pControl);

    return bSuccess;
}

// Fast handle check getting
bool TInterface::GetControlCheck(   const TControl* pControl,
                                    bool*           pRSuccess)
{
    bool bSuccess = false;

    bool bCheck = false;

    const TCheckContainer* pCheckContainer =
        dynamic_cast<const TCheckContainer*>(pControl);

    if(pCheckContainer)
        bCheck = pCheckContainer->GetCheck(&bSuccess);

    if(pRSuccess)
        *pRSuccess = bSuccess;

    return bCheck;
}

// Fast handle check settting
bool TInterface::SetControlCheck(   TControl*   pControl,
                                    bool        bCheck,
                                    bool        bSetInitialValues)
{
    bool bSuccess = false;

    TCheckContainer* pCheckContainer =
        dynamic_cast<TCheckContainer*>(pControl);

    if(pCheckContainer)
        bSuccess = pCheckContainer->SetCheck(bCheck);

    if(bSuccess && bSetInitialValues)
        SetControlInitialValues(pControl);

    return bSuccess;
}

// Message accessing
TMessageProcessor& TInterface::GetMessageProcessor()
{
    DEBUG_VERIFY_ALLOCATION;

    return m_MessageProcessor;
}

const TMessageProcessor& TInterface::GetMessageProcessor() const
{
    DEBUG_VERIFY_ALLOCATION;

    return m_MessageProcessor;
}

TMessageIterator TInterface::GetFirstMessage() const
{
    DEBUG_VERIFY_ALLOCATION;

    return m_MessageProcessor.GetFirst();
}

void TInterface::UpdateControlScreenState(TControl* pControl)
{
    const TControl* pParentControl = IterToCtrl(CtrlToIter(pControl).GetParent());

    pControl->m_CurrentScreenState.SetScreenState(  pParentControl ?
                                                        &pParentControl->m_CurrentScreenState :
                                                        NULL,
                                                    pControl->m_CurrentClientState);

    if(!pControl->m_CurrentScreenState.IsEnabled())
        g_InterfaceDeviceGlobals.InvalidateMouseOwnership(pControl, true);

    for(TControls::TIterator ControlIter2 = CtrlToIter(pControl).GetFirstChild() ;
        ControlIter2.IsValid() ;
        ControlIter2.ToNextSibling())
    {
        UpdateControlScreenState(IterToCtrl(ControlIter2));
    }
}

void TInterface::SetControlClientState( TControl*               pControl,
                                        const TControlState&    ClientState,
                                        bool                    bSetInitialValues)
{
    pControl->m_CurrentClientState = ClientState;

    UpdateControlScreenState(pControl);

    if(bSetInitialValues)
        SetControlInitialValues(pControl);
}

void TInterface::SetControlID(TControl* pControl, size_t szID)
{
    pControl->SetID(szID);
}

void TInterface::SetControlInitialValues(TControl* pControl)
{
    g_InterfaceDeviceGlobals.InvalidateMouseOwnership(pControl, false);

    pControl->SetInitialValues();

    for(TControls::TIterator ControlIter2 = CtrlToIter(pControl).GetFirstChild() ;
        ControlIter2.IsValid() ;
        ControlIter2.ToNextSibling())
    {
        SetControlInitialValues(IterToCtrl(ControlIter2));
    }
}

void TInterface::SuspendControl(TControl* pControl)
{
    g_InterfaceDeviceGlobals.InvalidateMouseOwnership(pControl, true);

    if(pControl->Suspend())
    {
        for(TControls::TIterator ControlIter2 = CtrlToIter(pControl).GetFirstChild() ;
            ControlIter2.IsValid() ;
            ControlIter2.ToNextSibling())
        {
            SuspendControl(IterToCtrl(ControlIter2));
        }
    }
}

void TInterface::ResumeControl(TControl* pControl)
{
    if(pControl->Resume())
    {
        for(TControls::TIterator ControlIter2 = CtrlToIter(pControl).GetFirstChild() ;
            ControlIter2.IsValid() ;
            ControlIter2.ToNextSibling())
        {
            ResumeControl(IterToCtrl(ControlIter2));
        }
    }
}

void TInterface::CallOnInterfaceUpdated(TControl* pControl)
{
    pControl->OnInterfaceUpdated();

    for(TControls::TIterator ControlIter2 = CtrlToIter(pControl).GetFirstChild() ;
        ControlIter2.IsValid() ;
        ControlIter2.ToNextSibling())
    {
        CallOnInterfaceUpdated(IterToCtrl(ControlIter2));
    }
}

bool TInterface::OnSuspend()
{
    if(!IsAllocated())
        return false;

    if(!TSuspendable::OnSuspend())
        return false;

    SuspendControl(GetDesktopControl());

    return true;
}

bool TInterface::OnResume()
{
    if(!IsAllocated())
        return false;

    if(!TSuspendable::OnResume())
        return false;

    ResumeControl(GetDesktopControl());

    return true;
}

void TInterface::Update()
{
    DEBUG_VERIFY_ALLOCATION;

    m_MessageProcessor.Clear();

    UpdateControl(GetDesktopControl());

    CallOnInterfaceUpdated(GetDesktopControl());
}

void TInterface::UpdateControl(TControl* pControl)
{
    pControl->Update();

    for(TControls::TIterator ControlIter2 = CtrlToIter(pControl).GetFirstChild() ;
        ControlIter2.IsValid() ;
        ControlIter2.ToNextSibling())
    {
        UpdateControl(IterToCtrl(ControlIter2));
    }
}

void TInterface::RenderControl(const TControl* pControl) const
{
    pControl->Render();

    for(TControls::TConstIterator ControlIter2 = CtrlToIter(pControl).GetFirstChild() ;
        ControlIter2.IsValid() ;
        ControlIter2.ToNextSibling())
    {
        RenderControl(IterToCtrl(ControlIter2));
    }
}

void TInterface::Render() const
{
    DEBUG_VERIFY_ALLOCATION;

    RenderControl(GetDesktopControl());
}

TControl* TInterface::GetDesktopControl()
{
    DEBUG_VERIFY_ALLOCATION;

    return IterToCtrl(m_Controls.GetRoot());
}

const TControl* TInterface::GetDesktopControl() const
{
    DEBUG_VERIFY_ALLOCATION;

    return IterToCtrl(m_Controls.GetRoot());
}

TControl* TInterface::InternalFindControl(  size_t      szID,
                                            TControl*   pParentControl,
                                            TControl*   pStartControl,
                                            bool        bRecursive)
{
    for(TControls::TIterator ControlIter = CtrlToIter(pStartControl) ;
        ControlIter.IsValid() ;
        ControlIter.ToNextSibling())
    {
        TControl* pControl = IterToCtrl(ControlIter);

        DEBUG_VERIFY(pControl);

        if(pControl->GetID() == szID)
            return pControl;

        if(bRecursive)
        {
            TControl* pControl2 =
                InternalFindControl(szID,
                                    pControl,
                                    IterToCtrl(CtrlToIter(pControl).GetFirstChild()),
                                    true);

            if(pControl2)
                return pControl2;
        }
    }

    return NULL;
}

const TControl* TInterface::InternalFindControl(size_t          szID,
                                                const TControl* pParentControl,
                                                const TControl* pStartControl,
                                                bool            bRecursive)
{
    for(TControls::TConstIterator ControlIter = CtrlToIter(pStartControl) ;
        ControlIter.IsValid() ;
        ControlIter.ToNextSibling())
    {
        const TControl* pControl = IterToCtrl(ControlIter);

        DEBUG_VERIFY(pControl);

        if(pControl->GetID() == szID)
            return pControl;

        if(bRecursive)
        {
            const TControl* pControl2 =
                InternalFindControl(szID,
                                    pControl,
                                    IterToCtrl(CtrlToIter(pControl).GetFirstChild()),
                                    true);

            if(pControl2)
                return pControl2;
        }
    }

    return NULL;
}

TControl* TInterface::FindControl(  size_t      szID,
                                    TControl*   pParentControl,
                                    TControl*   pStartControl,
                                    bool        bRecursive)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(szID != CONTROL_ID_NONE);

    if(!pParentControl)
        pParentControl = GetDesktopControl();

    if(!pStartControl)
        pStartControl = IterToCtrl(CtrlToIter(pParentControl).GetFirstChild());

    return InternalFindControl( szID,
                                pParentControl,
                                pStartControl,
                                bRecursive);
}

const TControl* TInterface::FindControl(size_t          szID,
                                        const TControl* pParentControl,
                                        const TControl* pStartControl,
                                        bool            bRecursive) const
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(szID != CONTROL_ID_NONE);

    if(!pParentControl)
        pParentControl = GetDesktopControl();

    if(!pStartControl)
        pStartControl = IterToCtrl(CtrlToIter(pParentControl).GetFirstChild());

    return InternalFindControl( szID,
                                pParentControl,
                                pStartControl,
                                bRecursive);
}

TControl* TInterface::GetControl(   size_t      szID,
                                    TControl*   pParentControl,
                                    TControl*   pStartControl,
                                    bool        bRecursive)
{
    TControl* pControl = FindControl(   szID,
                                        pParentControl,
                                        pStartControl,
                                        bRecursive);

    if(!pControl)
        INITIATE_DEFINED_FAILURE((KString)TEXT("Control with ID ") + szID + TEXT("not found."));

    return pControl;
}

const TControl* TInterface::GetControl( size_t          szID,
                                        const TControl* pParentControl,
                                        const TControl* pStartControl,
                                        bool            bRecursive) const
{
    const TControl* pControl = FindControl( szID,
                                            pParentControl,
                                            pStartControl,
                                            bRecursive);

    if(!pControl)
        INITIATE_DEFINED_FAILURE((KString)TEXT("Control with ID ") + szID + TEXT("not found."));

    return pControl;
}

// ----------------
// Controls merger
// ----------------
TControlsMerger::TControlsMerger()
{
    m_pControl = NULL;
}

void TControlsMerger::Release()
{
    if(IsAllocated())
    {
        m_pControl->GetInterface().DetachControl(m_pControl);

        m_pControl = NULL;
    }
}

TControl* TControlsMerger::Allocate(TControl* pSrcControl,
                                    TControl* pDstParentControl)
{
    Release();

    DEBUG_VERIFY(pSrcControl && pDstParentControl);

    return m_pControl = pDstParentControl->GetInterface().Merge(pSrcControl, pDstParentControl);
}
