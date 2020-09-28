#include "kfc_interface_pch.h"
#include "interface_interpolators.h"

#include "interface.h"

// ---------------------------------
// Control state common rect setter
// ---------------------------------
void TControlStateCommonRectSetter::SetControlStateParameter(   TControlState&  State,
                                                                const FRECT&    Rect)
{
    State.m_Rect.m_CommonPart = Rect;
}

// ------------------------------
// Control state own rect setter
// ------------------------------
void TControlStateOwnRectSetter::SetControlStateParameter(  TControlState&  State,
                                                            const FRECT&    Rect)
{
    State.m_Rect.m_OwnPart = Rect;
}

// -----------------------------------
// Control state children rect setter
// -----------------------------------
void TControlStateChildrenRectSetter::SetControlStateParameter( TControlState&  State,
                                                                const FRECT&    Rect)
{
    State.m_Rect.m_ChildrenPart = Rect;
}

// --------------------------------
// Control state hover rect setter
// --------------------------------
void TControlStateHoverRectSetter::SetControlStateParameter(TControlState&  State,
                                                            const FRECT&    Rect)
{
    State.m_HoverRect = Rect;
}

// ----------------------------------
// Control state common color setter
// ----------------------------------
void TControlStateCommonColorSetter::SetControlStateParameter(  TControlState&      State,
                                                                const TD3DColor&    Color)
{
    (State.m_Color.m_CommonPart = Color).Truncate();
}

// -------------------------------
// Control state own color setter
// -------------------------------
void TControlStateOwnColorSetter::SetControlStateParameter( TControlState&      State,
                                                            const TD3DColor&    Color)
{
    (State.m_Color.m_OwnPart = Color).Truncate();
}

// ------------------------------------
// Control state children color setter
// ------------------------------------
void TControlStateChildrenColorSetter::SetControlStateParameter(TControlState&      State,
                                                                const TD3DColor&    Color)
{
    (State.m_Color.m_ChildrenPart = Color).Truncate();
}

// ---------------------------------------
// Control state common visibility setter
// ---------------------------------------
void TControlStateCommonVisibilitySetter::SetControlStateParameter( TControlState&  State,
                                                                    const float&    fVisibility)
{
    BoundValue(State.m_Visibility.m_CommonPart = fVisibility, FSEGMENT(0.0f, 1.0f));
}

// ------------------------------------
// Control state own visibility setter
// ------------------------------------
void TControlStateOwnVisibilitySetter::SetControlStateParameter(TControlState&  State,
                                                                const float&    fVisibility)
{
    BoundValue(State.m_Visibility.m_OwnPart = fVisibility, FSEGMENT(0.0f, 1.0f));
}

// -----------------------------------------
// Control state children visibility setter
// -----------------------------------------
void TControlStateChildrenVisibilitySetter::SetControlStateParameter(   TControlState&  State,
                                                                        const float&    fVisibility)
{
    BoundValue(State.m_Visibility.m_ChildrenPart = fVisibility, FSEGMENT(0.0f, 1.0f));
}

// ---------------------------------------
// Control state common enablement setter
// ---------------------------------------
void TControlStateCommonEnablementSetter::SetControlStateParameter( TControlState&  State,
                                                                    const float&    fEnablement)
{
    BoundValue(State.m_Enablement.m_CommonPart = fEnablement, FSEGMENT(0.0f, 1.0f));
}

// ------------------------------------
// Control state own enablement setter
// ------------------------------------
void TControlStateOwnEnablementSetter::SetControlStateParameter(TControlState&  State,
                                                                const float&    fEnablement)
{
    BoundValue(State.m_Enablement.m_OwnPart = fEnablement, FSEGMENT(0.0f, 1.0f));
}

// -----------------------------------------
// Control state children enablement setter
// -----------------------------------------
void TControlStateChildrenEnablementSetter::SetControlStateParameter(   TControlState&  State,
                                                                        const float&    fEnablement)
{
    BoundValue(State.m_Enablement.m_ChildrenPart = fEnablement, FSEGMENT(0.0f, 1.0f));
}
