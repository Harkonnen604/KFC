#include "kfc_graphics_pch.h"
#include "graphics_state_manager.h"

#include "graphics_cfg.h"
#include "graphics_initials.h"
#include "graphics_tokens.h"

TGraphicsStateManager g_GraphicsStateManager;

// -----------------------
// Graphics state manager
// -----------------------
TGraphicsStateManager::TGraphicsStateManager() : TGlobals(TEXT("Graphics state manager"))
{
	AddSubGlobals(g_GraphicsCfg);
	AddSubGlobals(g_GraphicsInitials);
	AddSubGlobals(g_GraphicsTokens);
	AddSubGlobals(g_GraphicsDeviceGlobals);	

	// Dynamic states
	{
		size_t i;

		m_pActiveStateBlockNode = NULL;

		for(i = 0 ; i < MAX_D3D_STREAM_SOURCES ; i++)
		{
			m_ActiveStreamSourceVertexBuffers	[i] = NULL;
			m_ActiveStreamSourceOffsets			[i] = 0;
			m_ActiveStreamSourceStrides			[i] = 0;
		}

		m_pActiveIndexBuffer = NULL;

		for(i = 0 ; i < MAX_D3D_TEXTURE_STAGES ; i++)
			m_ActiveTextures[i] = NULL;

		m_pActiveVertexShader = NULL;

		m_pActivePixelShader = NULL;

		m_dwActiveFVF = 0;
	}
}

void TGraphicsStateManager::OnUninitialize()
{
	// State block nodes
	{
		size_t tex_rgb, tex_alpha, col_alpha;

		for(tex_rgb = 1 ; tex_rgb != UINT_MAX ; tex_rgb--)
		{
			for(tex_alpha = 1 ; tex_alpha != UINT_MAX ; tex_alpha--)
			{
				for(col_alpha = 1 ; col_alpha != UINT_MAX ; col_alpha--)
					m_Textured3DPrimitiveStateBlockNodes[tex_rgb][tex_alpha][col_alpha].Release();
			}
		}

		for(col_alpha = 1 ; col_alpha != UINT_MAX ; col_alpha--)
			m_NonTextured3DPrimitiveStateBlockNodes[col_alpha].Release();

		m_3DStateBlockNode.Release();

		for(tex_rgb = 1 ; tex_rgb != UINT_MAX ; tex_rgb--)
		{
			for(tex_alpha = 1 ; tex_alpha != UINT_MAX ; tex_alpha--)
			{
				for(col_alpha = 1 ; col_alpha != UINT_MAX ; col_alpha--)
					m_Textured2DPrimitiveStateBlockNodes[tex_rgb][tex_alpha][col_alpha].Release();
			}
		}

		for(col_alpha = 1 ; col_alpha != UINT_MAX ; col_alpha--)
			m_NonTextured2DPrimitiveStateBlockNodes[col_alpha].Release();
	}

	// Dynamic states
	{
		InvalidateStreamSource(UINT_MAX, NULL);

		InvalidateIndices(NULL);

		InvalidateTexture(UINT_MAX, NULL);

		InvalidateVertexShader(NULL);

		InvalidatePixelShader(NULL);

		InvalidateFVF();
	}
}

void TGraphicsStateManager::OnInitialize()
{
	// Dynamic states
	{
		size_t i;

		m_pActiveStateBlockNode = NULL;

		for(i = 0 ; i < MAX_D3D_STREAM_SOURCES ; i++)
		{
			m_ActiveStreamSourceVertexBuffers	[i] = NULL;
			m_ActiveStreamSourceOffsets			[i] = 0;
			m_ActiveStreamSourceStrides			[i] = 0;
		}

		m_pActiveIndexBuffer = NULL;

		for(i = 0 ; i < MAX_D3D_TEXTURE_STAGES ; i++)
			m_ActiveTextures[i] = NULL;

		m_pActiveVertexShader = NULL;

		m_pActivePixelShader = NULL;

		m_dwActiveFVF = 0;
	}

	// Recording state blocks
	{
		size_t tex_rgb, tex_alpha, col_alpha;

		// Recording 2D state block
		{
			m_2DStateBlockNode.Allocate(NULL);

			TD3DStateBlockRecorder Recorder0(m_2DStateBlockNode.m_StateBlock);

			g_GraphicsDeviceGlobals.m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,		FALSE);
			g_GraphicsDeviceGlobals.m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,	D3DCULL_NONE);

			size_t i;

			for(i = 0 ; i < g_GraphicsDeviceGlobals.m_D3DCaps.MaxSimultaneousTextures ; i++)
			{
				g_GraphicsDeviceGlobals.m_pD3DDevice->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
				g_GraphicsDeviceGlobals.m_pD3DDevice->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			}

			for(i = 0 ; i < g_GraphicsDeviceGlobals.m_D3DCaps.MaxSimultaneousTextures ; i++)
				g_GraphicsDeviceGlobals.m_pD3DDevice->SetRenderState((D3DRENDERSTATETYPE)(D3DRS_WRAP0 + i), 0);
		}

		// Recording non-textured 2D primitive state blocks
		for(col_alpha = 0 ; col_alpha < 2 ; col_alpha++)
		{
			m_NonTextured2DPrimitiveStateBlockNodes[col_alpha].Allocate(&m_2DStateBlockNode);

			TD3DStateBlockRecorder Recorder0(m_NonTextured2DPrimitiveStateBlockNodes[col_alpha].m_StateBlock);

			g_GraphicsDeviceGlobals.m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, col_alpha ? TRUE : FALSE);

			g_GraphicsDeviceGlobals.m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
			g_GraphicsDeviceGlobals.m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
		}

		// Recording textured 2D primitive state blocks
		for(tex_rgb = 0 ; tex_rgb < 2 ; tex_rgb++)
		{
			for(tex_alpha = 0 ; tex_alpha < 2 ; tex_alpha++)
			{
				for(col_alpha = 0 ; col_alpha < 2 ; col_alpha++)
				{
					m_Textured2DPrimitiveStateBlockNodes[tex_rgb][tex_alpha][col_alpha].Allocate(&m_2DStateBlockNode);

					TD3DStateBlockRecorder Recorder0(m_Textured2DPrimitiveStateBlockNodes[tex_rgb][tex_alpha][col_alpha].m_StateBlock);

					g_GraphicsDeviceGlobals.m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, tex_alpha || col_alpha ? TRUE : FALSE);

					g_GraphicsDeviceGlobals.m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, tex_rgb ? D3DTOP_MODULATE : D3DTOP_SELECTARG2);
					g_GraphicsDeviceGlobals.m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, tex_alpha && col_alpha ? D3DTOP_MODULATE : tex_alpha ? D3DTOP_SELECTARG1 : D3DTOP_SELECTARG2);
				}
			}
		}

		// Recording 3D state block
		{
			m_3DStateBlockNode.Allocate(NULL);

			TD3DStateBlockRecorder Recorder0(m_3DStateBlockNode.m_StateBlock);

			g_GraphicsDeviceGlobals.m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,		TRUE);
			g_GraphicsDeviceGlobals.m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,	D3DCULL_CCW);
		}

		// Recording non-textured 3D primitive state blocks
		for(col_alpha = 0 ; col_alpha < 2 ; col_alpha++)
		{
			m_NonTextured3DPrimitiveStateBlockNodes[col_alpha].Allocate(&m_3DStateBlockNode);

			TD3DStateBlockRecorder Recorder0(m_NonTextured3DPrimitiveStateBlockNodes[col_alpha].m_StateBlock);

			g_GraphicsDeviceGlobals.m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, col_alpha ? TRUE : FALSE);

			g_GraphicsDeviceGlobals.m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
			g_GraphicsDeviceGlobals.m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
		}

		// Recording textured 3D primitive state blocks
		for(tex_rgb = 0 ; tex_rgb < 2 ; tex_rgb++)
		{
			for(tex_alpha = 0 ; tex_alpha < 2 ; tex_alpha++)
			{
				for(col_alpha = 0 ; col_alpha < 2 ; col_alpha++)
				{
					m_Textured3DPrimitiveStateBlockNodes[tex_rgb][tex_alpha][col_alpha].Allocate(&m_3DStateBlockNode);

					TD3DStateBlockRecorder Recorder0(m_Textured3DPrimitiveStateBlockNodes[tex_rgb][tex_alpha][col_alpha].m_StateBlock);

					g_GraphicsDeviceGlobals.m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, tex_alpha || col_alpha ? TRUE : FALSE);

					g_GraphicsDeviceGlobals.m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, tex_rgb ? D3DTOP_MODULATE : D3DTOP_SELECTARG2);
					g_GraphicsDeviceGlobals.m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, tex_alpha && col_alpha ? D3DTOP_MODULATE : tex_alpha ? D3DTOP_SELECTARG1 : D3DTOP_SELECTARG2);
				}
			}
		}
	}
}
