#include "kfc_graphics_pch.h"
#include "graphics_effect_globals.h"

#include "graphics_consts.h"
#include "graphics_cfg.h"
#include "graphics_initials.h"
#include "graphics_device_globals.h"

TGraphicsEffectGlobals g_GraphicsEffectGlobals;

// ------------------------
// Graphics effect globals
// ------------------------
TGraphicsEffectGlobals::TGraphicsEffectGlobals() : TGlobals(TEXT("Graphics effects globals"))
{
	AddSubGlobals(g_GraphicsCfg);
	AddSubGlobals(g_GraphicsInitials);
	AddSubGlobals(g_GraphicsDeviceGlobals);
}

void TGraphicsEffectGlobals::OnUninitialize()
{
	m_EffectVertexBuffer.Release();
}

void TGraphicsEffectGlobals::OnInitialize()
{
	// Effects vertex buffer
	try
	{
		m_EffectVertexBuffer.Allocate(	g_GraphicsConsts.m_szMaxGraphicsEffectVertices,
										D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
										D3DPOOL_SYSTEMMEM);
	}

	catch(...)
	{
		INITIATE_DEFINED_FAILURE(TEXT("Error allocating 2D effects vertex buffer."));
	}
}