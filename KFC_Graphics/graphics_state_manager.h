#ifndef graphics_state_manager_h
#define graphics_state_manager_h

#include <KFC_KTL\globals.h>
#include "graphics_limits.h"
#include "graphics_device_globals.h"
#include "d3d_state_block.h"
#include "d3d_material.h"
#include "d3d_light.h"

// -----------------------
// Graphics state manager
// -----------------------
class TGraphicsStateManager : public TGlobals
{
private:
	// Dynamic states
	const TD3DStateBlockNode* m_pActiveStateBlockNode;

	IDirect3DVertexBuffer9*	m_ActiveStreamSourceVertexBuffers	[MAX_D3D_STREAM_SOURCES];
	size_t					m_ActiveStreamSourceOffsets			[MAX_D3D_STREAM_SOURCES];
	size_t					m_ActiveStreamSourceStrides			[MAX_D3D_STREAM_SOURCES];

	IDirect3DIndexBuffer9* m_pActiveIndexBuffer;

	IDirect3DBaseTexture9* m_ActiveTextures[MAX_D3D_TEXTURE_STAGES];

	IDirect3DVertexShader9* m_pActiveVertexShader;

	IDirect3DPixelShader9* m_pActivePixelShader;

	DWORD m_dwActiveFVF;


	void OnUninitialize();
	void OnInitialize	();

public:
	// State block nodes
	TD3DStateBlockNode m_2DStateBlockNode;
	TD3DStateBlockNode m_3DStateBlockNode;

	// col_alpha
	TD3DStateBlockNode m_NonTextured2DPrimitiveStateBlockNodes[2];

	// tex_rgb/tex_alpha/col_alpha
	TD3DStateBlockNode m_Textured2DPrimitiveStateBlockNodes[2][2][2];

	// col_alpha
	TD3DStateBlockNode m_NonTextured3DPrimitiveStateBlockNodes[2];

	// tex_rgb/tex_alpha/col_alpha
	TD3DStateBlockNode m_Textured3DPrimitiveStateBlockNodes[2][2][2];


	TGraphicsStateManager();

	void SetStateBlockNode(const TD3DStateBlockNode* pNode)
	{
		DEBUG_VERIFY(pNode == NULL || pNode->IsAllocated());

		if(m_pActiveStateBlockNode == pNode) // speed-up  (rather comon case)
			return;

		// Getting common ancestor and recording path history
		const TD3DStateBlockNode*	History[MAX_D3D_STATE_BLOCK_DEPTH];
		size_t						szHistorySize;

		TD3DStateBlockNode::GetComonAncestor(	m_pActiveStateBlockNode,
												pNode,
												History,
												szHistorySize);

		// Shifting onto new target if new path is not completely within previous one
		if(szHistorySize)
			m_pActiveStateBlockNode = pNode;

		// Applying difference along recorded path
		const TD3DStateBlockNode** pCur = History + szHistorySize;

		for( ; szHistorySize ; --szHistorySize)
			(*(--pCur))->m_StateBlock.Apply();
	}

	void InvalidateStateBlockNode(const TD3DStateBlockNode* pNode)
	{
		if(m_pActiveStateBlockNode == NULL) // nothing to unroll
			return;

		if(pNode == NULL) // complete unroll
		{
			m_pActiveStateBlockNode = NULL;
			return;
		}

		if(!pNode->IsAllocated()) // can't be set at this time and probably contains invalid depth
			return;

		const TD3DStateBlockNode*	History[MAX_D3D_STATE_BLOCK_DEPTH];
		size_t						szHistorySize;

		// Checking if 'pNode' is our ancestor
		if(TD3DStateBlockNode::GetComonAncestor(m_pActiveStateBlockNode,
												pNode,
												History,
												szHistorySize) == pNode)
		{
			// Shifting to its parent (most recent alive node on our path)
			m_pActiveStateBlockNode = pNode ? pNode->GetParent() : NULL;
		}
	}

	void SetStreamSource(	size_t					szStreamNumber,
							IDirect3DVertexBuffer9*	pVertexBuffer,
							size_t					szOffset,
							size_t					szStride)
	{
		DEBUG_VERIFY(szStreamNumber < MAX_D3D_STREAM_SOURCES);

		if(	m_ActiveStreamSourceVertexBuffers	[szStreamNumber] != pVertexBuffer	||
			m_ActiveStreamSourceOffsets			[szStreamNumber] != szOffset		||
			m_ActiveStreamSourceStrides			[szStreamNumber] != szStride)
		{
			DEBUG_EVALUATE_VERIFY(	!g_GraphicsDeviceGlobals.m_pD3DDevice->SetStreamSource(	szStreamNumber,
																							m_ActiveStreamSourceVertexBuffers	[szStreamNumber] = pVertexBuffer,
																							m_ActiveStreamSourceOffsets			[szStreamNumber] = szOffset,
																							m_ActiveStreamSourceStrides			[szStreamNumber] = szStride));
		}
	}

	void InvalidateStreamSource(size_t					szStreamNumber,
								IDirect3DVertexBuffer9*	pVertexBuffer)
	{
		if(szStreamNumber == UINT_MAX)
		{
			size_t i;

			for(i = 0 ; i < MAX_D3D_STREAM_SOURCES ; i++)
			{
				if(pVertexBuffer == NULL || m_ActiveStreamSourceVertexBuffers[i] == pVertexBuffer)
					SetStreamSource(i, NULL, 0, 0);
			}
		}
		else
		{
			DEBUG_VERIFY(szStreamNumber < MAX_D3D_STREAM_SOURCES);

			if(pVertexBuffer == NULL || m_ActiveStreamSourceVertexBuffers[szStreamNumber] == pVertexBuffer)
				SetStreamSource(szStreamNumber, NULL, 0, 0);
		}
	}

	void SetIndices(IDirect3DIndexBuffer9* pIndexBuffer)
	{
		if(m_pActiveIndexBuffer != pIndexBuffer)
			DEBUG_EVALUATE_VERIFY(!g_GraphicsDeviceGlobals.m_pD3DDevice->SetIndices(m_pActiveIndexBuffer = pIndexBuffer));
	}

	void InvalidateIndices(IDirect3DIndexBuffer9* pIndexBuffer)
	{
		if(pIndexBuffer == NULL || m_pActiveIndexBuffer == pIndexBuffer)
			SetIndices(NULL);
	}

	void SetTexture(size_t					szStage,
					IDirect3DBaseTexture9*	pTexture)
	{
		DEBUG_VERIFY(szStage < MAX_D3D_TEXTURE_STAGES);

		if(m_ActiveTextures[szStage] != pTexture)
		{
			DEBUG_EVALUATE_VERIFY(!g_GraphicsDeviceGlobals.m_pD3DDevice->SetTexture(szStage,
																					m_ActiveTextures[szStage] = pTexture));
		}
	}

	void InvalidateTexture(	size_t					szStage,
							IDirect3DBaseTexture9*	pTexture)
	{
		if(szStage == UINT_MAX)
		{
			size_t i;

			for(i = 0 ; i < MAX_D3D_TEXTURE_STAGES ; i++)
			{
				if(pTexture == NULL || m_ActiveTextures[i] == pTexture)
					SetTexture(i, NULL);
			}
		}
		else
		{
			DEBUG_VERIFY(szStage < MAX_D3D_TEXTURE_STAGES);

			if(pTexture == NULL || m_ActiveTextures[szStage] == pTexture)
				SetTexture(szStage, NULL);
		}
	}

	void SetVertexShader(IDirect3DVertexShader9* pVertexShader)
	{
		if(m_pActiveVertexShader != pVertexShader)
			DEBUG_EVALUATE_VERIFY(!g_GraphicsDeviceGlobals.m_pD3DDevice->SetVertexShader(m_pActiveVertexShader = pVertexShader));
	}

	void InvalidateVertexShader(IDirect3DVertexShader9* pVertexShader)
	{
		if(pVertexShader == NULL || m_pActiveVertexShader == pVertexShader)
			SetVertexShader(NULL);
	}

	void SetPixelShader(IDirect3DPixelShader9* pPixelShader)
	{
		if(m_pActivePixelShader != pPixelShader)
			DEBUG_EVALUATE_VERIFY(!g_GraphicsDeviceGlobals.m_pD3DDevice->SetPixelShader(m_pActivePixelShader = pPixelShader));
	}

	void InvalidatePixelShader(IDirect3DPixelShader9* pPixelShader)
	{
		if(pPixelShader == NULL || m_pActivePixelShader == pPixelShader)
			SetPixelShader(NULL);
	}
	
	void SetFVF(DWORD dwFVF)
	{
		if(m_dwActiveFVF != dwFVF)
			DEBUG_EVALUATE_VERIFY(!g_GraphicsDeviceGlobals.m_pD3DDevice->SetFVF(m_dwActiveFVF = dwFVF));
	}

	void InvalidateFVF()
	{
		m_dwActiveFVF = 0;
	}

	D3DCOLOR GetAmbientLight()
	{
		D3DCOLOR Color;

		DEBUG_EVALUATE_VERIFY(!g_GraphicsDeviceGlobals.m_pD3DDevice->GetRenderState(D3DRS_AMBIENT, &Color));

		return Color;
	}

	void SetAmbientLight(D3DCOLOR Color)
	{
		DEBUG_EVALUATE_VERIFY(!g_GraphicsDeviceGlobals.m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, Color));
	}

	void GetTransform(D3DTRANSFORMSTATETYPE State, TD3DMatrix& RMatrix)
	{
		DEBUG_EVALUATE_VERIFY(!g_GraphicsDeviceGlobals.m_pD3DDevice->GetTransform(State, (D3DMATRIX*)&RMatrix));
	}

	void SetTransform(D3DTRANSFORMSTATETYPE State, const TD3DMatrix& Matrix)
	{
		DEBUG_EVALUATE_VERIFY(!g_GraphicsDeviceGlobals.m_pD3DDevice->SetTransform(State, (const D3DMATRIX*)&Matrix));
	}

	void GetMaterial(TD3DMaterial& RMaterial)
	{
		DEBUG_EVALUATE_VERIFY(!g_GraphicsDeviceGlobals.m_pD3DDevice->GetMaterial(RMaterial));
	}

	void SetMaterial(const TD3DMaterial& Material)
	{
		DEBUG_EVALUATE_VERIFY(!g_GraphicsDeviceGlobals.m_pD3DDevice->SetMaterial(Material));
	}

	void GetLight(size_t szIndex, TD3DLight& RLight)
	{
		DEBUG_EVALUATE_VERIFY(!g_GraphicsDeviceGlobals.m_pD3DDevice->GetLight(szIndex, RLight));
	}

	void SetLight(size_t szIndex, const TD3DLight& Light)
	{
		DEBUG_EVALUATE_VERIFY(!g_GraphicsDeviceGlobals.m_pD3DDevice->SetLight(szIndex, Light));
	}

	bool IsLightEnabled(size_t szIndex)
	{
		BOOL bEnabled;

		DEBUG_EVALUATE_VERIFY(!g_GraphicsDeviceGlobals.m_pD3DDevice->GetLightEnable(szIndex, &bEnabled));

		return bEnabled;
	}

	void EnableLight(size_t szIndex, bool bEnable = true)
	{
		DEBUG_EVALUATE_VERIFY(!g_GraphicsDeviceGlobals.m_pD3DDevice->LightEnable(szIndex, bEnable));
	}
};

extern TGraphicsStateManager g_GraphicsStateManager;

#endif // graphics_state_manager_h