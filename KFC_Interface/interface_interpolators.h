#ifndef interface_interpolators_h
#define interface_interpolators_h

#include <KFC_Common\interpolator.h>
#include "interface_defs.h"
#include "interface_interpolator_macros.h"

// -----------------------------------
// Control parameter dst value setter
// -----------------------------------
template <  class DstValueType,
            class ControlStateParameterSetterType>

class TControlParameterDstValueSetter :
    public TDstValueSetter<DstValueType>,
    public ControlStateParameterSetterType
{
private:
    bool m_bAllocated;

    TControl* m_pControl;

public:
    TControlParameterDstValueSetter();

    ~TControlParameterDstValueSetter()
        { Release(); }

    bool IsAllocated() const
        { return TDstValueSetter<DstValueType>::IsAllocated() && m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(TControl* pSControl);

    void SetDstValue(const DstValueType& DstValue);
};

template <  class DstValueType,
            class ControlStateParameterSetterType>

TControlParameterDstValueSetter<DstValueType,
                                ControlStateParameterSetterType>::
    TControlParameterDstValueSetter()
{
    m_bAllocated = false;
}

template <  class DstValueType,
            class ControlStateParameterSetterType>

void TControlParameterDstValueSetter<   DstValueType,
                                        ControlStateParameterSetterType>::
    Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        TDstValueSetter<DstValueType>::Release();
    }
}

template <  class DstValueType,
            class ControlStateParameterSetterType>

void TControlParameterDstValueSetter<   DstValueType,
                                        ControlStateParameterSetterType>::
    Allocate(TControl* pSControl)
{
    Release();

    try
    {
        DEBUG_VERIFY(pSControl);

        TDstValueSetter<DstValueType>::Allocate();

        m_pControl = pSControl;

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

template <  class DstValueType,
            class ControlStateParameterSetterType>

void TControlParameterDstValueSetter<   DstValueType,
                                        ControlStateParameterSetterType>::
    SetDstValue(const DstValueType& DstValue)
{
    DEBUG_VERIFY_ALLOCATION;

    TDstValueSetter<DstValueType>::SetDstValue(DstValue);

    TControlState State = m_pControl->GetCurrentClientState();

    ControlStateParameterSetterType::SetControlStateParameter(State, DstValue);

    TInterface::SetControlClientState(m_pControl, State);
}

// ---------------------------------
// Control state common rect setter
// ---------------------------------
class TControlStateCommonRectSetter
{
protected:
    static void SetControlStateParameter(   TControlState&  State,
                                            const FRECT&    Rect);
};

// ------------------------------
// Control state own rect setter
// ------------------------------
class TControlStateOwnRectSetter
{
protected:
    static void SetControlStateParameter(   TControlState&  State,
                                            const FRECT&    Rect);
};

// -----------------------------------
// Control state children rect setter
// -----------------------------------
class TControlStateChildrenRectSetter
{
protected:
    static void SetControlStateParameter(   TControlState&  State,
                                            const FRECT&    Rect);
};

// --------------------------------
// Control state hover rect setter
// --------------------------------
class TControlStateHoverRectSetter
{
protected:
    static void SetControlStateParameter(   TControlState&  State,
                                            const FRECT&    Rect);
};

// ----------------------------------
// Control state common color setter
// ----------------------------------
class TControlStateCommonColorSetter
{
protected:
    static void SetControlStateParameter(   TControlState&      State,
                                            const TD3DColor&    Color);
};

// -------------------------------
// Control state own color setter
// -------------------------------
class TControlStateOwnColorSetter
{
protected:
    static void SetControlStateParameter(   TControlState&      State,
                                            const TD3DColor&    Color);
};

// ------------------------------------
// Control state children color setter
// ------------------------------------
class TControlStateChildrenColorSetter
{
protected:
    static void SetControlStateParameter(   TControlState&      State,
                                            const TD3DColor&    Color);
};

// ---------------------------------------
// Control state common visibility setter
// ---------------------------------------
class TControlStateCommonVisibilitySetter
{
protected:
    static void SetControlStateParameter(   TControlState&  State,
                                            const float&    fVisibility);
};

// ------------------------------------
// Control state own visibility setter
// ------------------------------------
class TControlStateOwnVisibilitySetter
{
protected:
    static void SetControlStateParameter(   TControlState&  State,
                                            const float&    fVisibility);
};

// ----------------------------------------
// Control state children visibility setter
// -----------------------------------------
class TControlStateChildrenVisibilitySetter
{
protected:
    static void SetControlStateParameter(   TControlState&  State,
                                            const float&    fVisibility);
};

// ---------------------------------------
// Control state common enablement setter
// ---------------------------------------
class TControlStateCommonEnablementSetter
{
protected:
    static void SetControlStateParameter(   TControlState&  State,
                                            const float&    fEnablement);
};

// ------------------------------------
// Control state own enablement setter
// ------------------------------------
class TControlStateOwnEnablementSetter
{
protected:
    static void SetControlStateParameter(   TControlState&  State,
                                            const float&    fEnablement);
};

// -----------------------------------------
// Control state children enablement setter
// -----------------------------------------
class TControlStateChildrenEnablementSetter
{
protected:
    static void SetControlStateParameter(   TControlState&  State,
                                            const float&    fEnablement);
};

#endif // interface_interpolators_h
