#include "kfc_graphics_pch.h"
#include "d3d_state_block.h"

#include "graphics_state_manager.h"
#include "graphics_limits.h"

// ----------------
// D3D state block
// ----------------
TD3DStateBlock::TD3DStateBlock()
{
    m_pStateBlock = NULL;
}

void TD3DStateBlock::Release()
{
    if(m_pStateBlock)
        m_pStateBlock->Release(), m_pStateBlock = NULL;
}

TD3DStateBlock& TD3DStateBlock::Create(D3DSTATEBLOCKTYPE Type)
{
    Release();

    try
    {
        HRESULT r;

        if(r = g_GraphicsDeviceGlobals.m_pD3DDevice->CreateStateBlock(Type, &m_pStateBlock))
        {
            m_pStateBlock = NULL;
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error creating Direct3D state block."), r);
        }
    }

    catch(...)
    {
        Release();
        throw;
    }

    return *this;
}

TD3DStateBlock& TD3DStateBlock::Allocate(IDirect3DStateBlock9* pSStateBlock)
{
    Release();

    try
    {
        DEBUG_VERIFY(pSStateBlock);

        m_pStateBlock = pSStateBlock;
    }

    catch(...)
    {
        Release();
        throw;
    }

    return *this;
}

void TD3DStateBlock::Capture()
{
    DEBUG_VERIFY_ALLOCATION;

    HRESULT r;

    if(r = m_pStateBlock->Capture())
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error capturing Direct3D state."), r);
}

// -------------------------
// D3D state block recorder
// -------------------------
TD3DStateBlockRecorder::TD3DStateBlockRecorder(TD3DStateBlock& SRStateBlock) :
    m_RStateBlock(SRStateBlock)
{
    m_RStateBlock.Release(); // conserving resources

    g_GraphicsDeviceGlobals.BeginStateBlock();
}

TD3DStateBlockRecorder::~TD3DStateBlockRecorder()
{
    IDirect3DStateBlock9* pStateBlock =
        g_GraphicsDeviceGlobals.EndStateBlock();

    if(pStateBlock)
        m_RStateBlock.Allocate(pStateBlock);
}

// ---------------------
// D3D state block node
// ---------------------
void TD3DStateBlockNode::Release()
{
    g_GraphicsStateManager.InvalidateStateBlockNode(this);

    m_StateBlock.Release();

    m_szDepth = UINT_MAX;
}

void TD3DStateBlockNode::Allocate(const TD3DStateBlockNode* pSParent)
{
    Release();

    m_pParent = pSParent;

    m_szDepth = m_pParent ? m_pParent->m_szDepth + 1 : 0;

    DEBUG_VERIFY(m_szDepth < MAX_D3D_STATE_BLOCK_DEPTH);
}
