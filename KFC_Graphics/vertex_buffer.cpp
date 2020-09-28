#include "kfc_graphics_pch.h"
#include "vertex_buffer.h"

#include "graphics_state_manager.h"

// -------------------
// Vertex buffer base
// -------------------
TVertexBufferBase::TVertexBufferBase()
{
    m_pVertexBuffer = NULL;
}

void TVertexBufferBase::Release()
{
    if(m_pVertexBuffer)
    {
        g_GraphicsStateManager.InvalidateStreamSource(UINT_MAX, m_pVertexBuffer);

        m_pVertexBuffer->Release(), m_pVertexBuffer = NULL;
    }
}

void TVertexBufferBase::Allocate(IDirect3DVertexBuffer9* pSVertexBuffer)
{
    Release();

    DEBUG_VERIFY(pSVertexBuffer);

    m_pVertexBuffer = pSVertexBuffer;
}
