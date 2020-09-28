#include "kfc_interface_pch.h"
#include "dummy_control.h"

// ------------------------------
// Dummy control creation struct
// ------------------------------
TDummyControlCreationStruct::TDummyControlCreationStruct()
{
}

void TDummyControlCreationStruct::Load( TInfoNodeConstIterator  InfoNode,
                                        const TControl*         pParentControl,
                                        const FRECT&            Resolution)
{
    TControlCreationStruct::Load(InfoNode, pParentControl, Resolution);
}

// --------------
// Dummy control
// --------------
TControl* TDummyControl::LoadControl(   type_t                  tpType,
                                        TInfoNodeConstIterator  InfoNode,
                                        const TControl*         pParentControl,
                                        const FRECT&            Resolution)
{
    DEBUG_VERIFY(tpType == CONTROL_TYPE_DUMMY);

    DEBUG_VERIFY(InfoNode.IsValid());

    TDummyControlCreationStruct CreationStruct;
    CreationStruct.Load(InfoNode, pParentControl, Resolution);

    return new TDummyControl(CreationStruct);
}

TDummyControl::TDummyControl(const TDummyControlCreationStruct& CreationStruct) :
    TControl(CreationStruct)
{
}
