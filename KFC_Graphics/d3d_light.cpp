#include "kfc_graphics_pch.h"
#include "d3d_light.h"

#include "color_defs.h"
#include "graphics_state_manager.h"

// ----------
// D3D light
// ----------
TD3DLight& TD3DLight::SetDefaults(D3DLIGHTTYPE SType)
{
	m_Type = SType;

	m_DiffuseColor	= WhiteColor();
	m_SpecularColor = WhiteColor();
	m_AmbientColor	= BlackColor();

	m_Coords.	SetZero	();
	m_Direction.SetZAxis();
	
	m_fRange		= 0.0f;
	m_fFalloff		= 0.0f;
	m_fAttenuation0	= 0.0f;
	m_fAttenuation1	= 0.0f;
	m_fAttenuation2	= 0.0f;
	m_fHotSpotTheta	= 0.0f;
	m_fHotSpotPhi	= 0.0f;

	return *this;
}

void TD3DLight::Install(size_t szIndex, bool bSetEnable, bool bEnable) const
{
	g_GraphicsStateManager.SetLight(szIndex, *this);

	if(bSetEnable)
		Enable(szIndex, bEnable);
}

void TD3DLight::Enable(size_t szIndex, bool bEnable)
{
	g_GraphicsStateManager.EnableLight(szIndex, bEnable);
}