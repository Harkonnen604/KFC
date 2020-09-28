#include "kfc_graphics_pch.h"
#include "index_buffer.h"

#include "graphics_state_manager.h"

// ------------------
// Index buffer base
// ------------------
TIndexBufferBase::TIndexBufferBase()
{
    m_pIndexBuffer = NULL;
}

void TIndexBufferBase::Release()
{
    if(m_pIndexBuffer)
    {
        g_GraphicsStateManager.InvalidateIndices(m_pIndexBuffer);

        m_pIndexBuffer->Release(), m_pIndexBuffer = NULL;
    }
}

void TIndexBufferBase::Allocate(IDirect3DIndexBuffer9* pSIndexBuffer)
{
    Release();

    DEBUG_VERIFY(pSIndexBuffer);

    m_pIndexBuffer = pSIndexBuffer;
}
