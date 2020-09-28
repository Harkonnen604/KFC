#include "kfc_scene_pch.h"
#include "visual_binder.h"

// -----------------------------------
// Soft physical object visual binder
// -----------------------------------
TSoftPhysicalObjectVisualBinder::TSoftPhysicalObjectVisualBinder()
{
    m_pObject = NULL;

    m_pModel = NULL;
}

void TSoftPhysicalObjectVisualBinder::Release()
{
    m_pObject = NULL;

    m_pModel = NULL;
}

void TSoftPhysicalObjectVisualBinder::Allocate( const TSoftPhysicalObject&  SObject,
                                                TVisualModelBase&           SModel)
{
    Release();

    DEBUG_VERIFY(SModel.IsAllocated());

    DEBUG_VERIFY(SModel.IsDynamic());

    m_pObject = &SObject;

    m_pModel = &SModel;
}

void TSoftPhysicalObjectVisualBinder::Bind() const
{
    DEBUG_VERIFY_ALLOCATION;

    m_pModel->SetVertices(m_pObject->GetMesh());
}
