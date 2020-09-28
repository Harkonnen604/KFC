#include "kfc_graphics_pch.h"
#include "d3d_material.h"

#include "graphics_state_manager.h"
#include "color_defs.h"

// -------------
// D3D material
// -------------
TD3DMaterial& TD3DMaterial::SetDefaults()
{
    m_DiffuseColor  = WhiteColor(0);
    m_AmbientColor  = WhiteColor(0);
    m_SpecularColor = BlackColor(0);
    m_EmissiveColor = BlackColor(0);

    m_fPower = 0.0f;

    return *this;
}

void TD3DMaterial::Install() const
{
    g_GraphicsStateManager.SetMaterial(*this);
}
