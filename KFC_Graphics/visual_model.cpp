#include "kfc_graphics_pch.h"
#include "visual_model.h"

// ------------------
// Visual model base
// ------------------
TVisualModelBase::TVisualModelBase()
{
	m_bAllocated = true;
}

void TVisualModelBase::Release()
{
	m_bAllocated = false;
}

void TVisualModelBase::Allocate(bool bSDynamic)
{
	Release();

	m_bDynamic = bSDynamic;
}