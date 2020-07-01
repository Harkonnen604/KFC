#include "kfc_graphics_pch.h"
#include "stated_texture.h"

#include "graphics_state_manager.h"

// ---------------
// Stated texture
// ---------------
TStatedTexture::TStatedTexture()
{
	m_szStage = UINT_MAX;
}

void TStatedTexture::Release()
{
	m_StateBlockNode.Release();	

	m_Texture.Release();

	m_szStage = UINT_MAX;
}

void TStatedTexture::Allocate(size_t szSStage)
{
	Release();

	DEBUG_VERIFY(szSStage != UINT_MAX);

	m_szStage = szSStage;
}

void TStatedTexture::Install() const
{
	DEBUG_VERIFY_ALLOCATION;

	m_Texture.Install(m_szStage);

	g_GraphicsStateManager.SetStateBlockNode(m_StateBlockNode.GetDataPtr());
}