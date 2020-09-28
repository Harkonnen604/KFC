#include "kfc_graphics_pch.h"
#include "pixel_shader.h"

#include "graphics_state_manager.h"

// -------------
// Pixel shader
// -------------
TPixelShader::TPixelShader()
{
    m_pPixelShader = NULL;
}

void TPixelShader::Release()
{
    if(m_pPixelShader)
    {
        g_GraphicsStateManager.InvalidatePixelShader(m_pPixelShader);

        m_pPixelShader->Release(), m_pPixelShader = NULL;
    }
}

void TPixelShader::Allocate(IDirect3DPixelShader9* pSPixelShader)
{
    Release();

    DEBUG_VERIFY(pSPixelShader);

    m_pPixelShader = pSPixelShader;
}

void TPixelShader::Install() const
{
    DEBUG_VERIFY_ALLOCATION;

    g_GraphicsStateManager.SetPixelShader(m_pPixelShader);
}
