#include "kfc_graphics_pch.h"
#include "visual_part.h"

#include "graphics_state_manager.h"

// ------------
// Visual part
// ------------
TVisualPart::TVisualPart()
{
	m_bAllocated = false;
}

void TVisualPart::Release()
{
	m_bAllocated = false;

	m_Textures.Clear();

	m_Material.Release();

	m_Model.Release();
}

void TVisualPart::Allocate()
{
	Release();

	m_bAllocated = true;
}

void TVisualPart::Render() const
{
	DEBUG_VERIFY_ALLOCATION;

	size_t i;

	g_GraphicsDeviceGlobals.m_pD3DDevice->
		SetMaterial(m_Material.IsAllocated() ? m_Material.GetDataRef() : NULL);

	for(i = 0 ; i < m_Textures.GetN() ; i++)
		m_Textures[i]->Install();

	m_Model->Render();
}