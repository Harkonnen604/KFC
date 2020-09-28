#include "kfc_graphics_pch.h"
#include "vertex_shader.h"

#include "graphics_state_manager.h"

// --------------
// Vertex shader
// --------------
TVertexShader::TVertexShader()
{
    m_pVertexShader = NULL;
}

void TVertexShader::Release()
{
    if(m_pVertexShader)
    {
        g_GraphicsStateManager.InvalidateVertexShader(m_pVertexShader);

        m_pVertexShader->Release(), m_pVertexShader = NULL;
    }
}

void TVertexShader::Allocate(IDirect3DVertexShader9* pSVertexShader)
{
    Release();

    DEBUG_VERIFY(pSVertexShader);

    m_pVertexShader = pSVertexShader;
}

void TVertexShader::Install() const
{
    g_GraphicsStateManager.SetVertexShader(m_pVertexShader);
}
