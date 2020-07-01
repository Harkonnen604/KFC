#include "kfc_graphics_pch.h"
#include "2d_effects.h"

#include "graphics_device_globals.h"
#include "graphics_state_manager.h"
#include "graphics_effect_globals.h"

// ------------
// Rect effect
// ------------
void TRectEffect::DrawRect(	FRECT		Rect,
							D3DCOLOR	Color)
{
	const DWORD dwColorAlpha = Color >> 24;

	if(dwColorAlpha == 0)
		return;

	RoundUp(Rect);

	ShiftRect(Rect, FSIZE(-0.5f, -0.5f));

	{
		T2DEffectVertex* pData;

		TEffectVertexBufferLocker Locker0(	g_GraphicsEffectGlobals.m_EffectVertexBuffer,
											pData,
											D3DLOCK_DISCARD,
											&SZSEGMENT(0, 4));

		pData->SetScreenCoords(Rect.m_Left, Rect.m_Top);
		pData->m_Color = Color;
		pData++;

		pData->SetScreenCoords(Rect.m_Right, Rect.m_Top);
		pData->m_Color = Color;
		pData++;

		pData->SetScreenCoords(Rect.m_Left, Rect.m_Bottom);
		pData->m_Color = Color;
		pData++;

		pData->SetScreenCoords(Rect.m_Right, Rect.m_Bottom);
		pData->m_Color = Color;
//		pData++;
	}

	// Rendering
	{
		g_GraphicsEffectGlobals.m_EffectVertexBuffer.Install();

		g_GraphicsStateManager.SetStateBlockNode
			(&g_GraphicsStateManager.m_NonTextured2DPrimitiveStateBlockNodes[dwColorAlpha < 0xFF]);

		DEBUG_EVALUATE_VERIFY
			(!g_GraphicsDeviceGlobals.m_pD3DDevice->
				DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2));
	}
}

void TRectEffect::DrawRect(	FRECT			Rect,
							const D3DCOLOR	Colors[4])
{
	const DWORD dwTotalColorAlpha =
		(Colors[0] >> 24) +
		(Colors[1] >> 24) +
		(Colors[2] >> 24) +
		(Colors[3] >> 24);

	if(dwTotalColorAlpha == 0)
		return;

	RoundUp(Rect);

	ShiftRect(Rect, FSIZE(-0.5f, -0.5f));

	{
		T2DEffectVertex* pData;		

		TEffectVertexBufferLocker Locker0(	g_GraphicsEffectGlobals.m_EffectVertexBuffer,
											pData,
											D3DLOCK_DISCARD,
											&SZSEGMENT(0, 4));

		pData->SetScreenCoords(Rect.m_Left, Rect.m_Top);
		pData->m_Color = Colors[0];
		pData++;

		pData->SetScreenCoords(Rect.m_Right, Rect.m_Top);
		pData->m_Color = Colors[1];
		pData++;

		pData->SetScreenCoords(Rect.m_Left, Rect.m_Bottom);
		pData->m_Color = Colors[2];
		pData++;

		pData->SetScreenCoords(Rect.m_Right, Rect.m_Bottom);
		pData->m_Color = Colors[3];
//		pData++;
	}

	// Rendering
	{
		g_GraphicsEffectGlobals.m_EffectVertexBuffer.Install();

		g_GraphicsStateManager.SetStateBlockNode
			(&g_GraphicsStateManager.m_NonTextured2DPrimitiveStateBlockNodes[dwTotalColorAlpha < 0xFF * 4]);

		DEBUG_EVALUATE_VERIFY
			(!g_GraphicsDeviceGlobals.m_pD3DDevice->
				DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2));
	}
}

// -----------------
// Wire rect effect
// -----------------
void TWireRectEffect::DrawRect(	FRECT		Rect,
								D3DCOLOR	Color)
{
	const DWORD dwColorAlpha = Color >> 24;

	if(dwColorAlpha == 0)
		return;

	RoundUp(Rect);

	ShiftRect(Rect, FSIZE(-0.5f, -0.5f));

	{
		T2DEffectVertex* pData;

		TEffectVertexBufferLocker Locker0(	g_GraphicsEffectGlobals.m_EffectVertexBuffer,
											pData,
											D3DLOCK_DISCARD,
											&SZSEGMENT(0, 5));

		pData->SetScreenCoords(Rect.m_Left, Rect.m_Top);
		pData->m_Color = Color;
		pData++;

		pData->SetScreenCoords(Rect.m_Right, Rect.m_Top);
		pData->m_Color = Color;
		pData++;

		pData->SetScreenCoords(Rect.m_Right, Rect.m_Bottom);
		pData->m_Color = Color;
		pData++;

		pData->SetScreenCoords(Rect.m_Left, Rect.m_Bottom);
		pData->m_Color = Color;
		pData++;

		pData->SetScreenCoords(Rect.m_Left, Rect.m_Top);
		pData->m_Color = Color;
//		pData++;
	}

	// Rendering
	{
		g_GraphicsEffectGlobals.m_EffectVertexBuffer.Install();

		g_GraphicsStateManager.SetStateBlockNode
			(&g_GraphicsStateManager.m_NonTextured2DPrimitiveStateBlockNodes[dwColorAlpha < 0xFF]);

		DEBUG_EVALUATE_VERIFY
			(!g_GraphicsDeviceGlobals.m_pD3DDevice->
				DrawPrimitive(D3DPT_LINESTRIP, 0, 4));
	}
}

void TWireRectEffect::DrawRect(	FRECT			Rect, // altered inside
								const D3DCOLOR	Colors[4])
{
	const DWORD dwTotalColorAlpha =
		(Colors[0] >> 24) +
		(Colors[1] >> 24) +
		(Colors[2] >> 24) +
		(Colors[3] >> 24);

	if(dwTotalColorAlpha == 0)
		return;

	RoundUp(Rect);

	ShiftRect(Rect, FSIZE(-0.5f, -0.5f));

	{
		T2DEffectVertex* pData;

		TEffectVertexBufferLocker Locker0(	g_GraphicsEffectGlobals.m_EffectVertexBuffer,
											pData,
											D3DLOCK_DISCARD,
											&SZSEGMENT(0, 5));

		pData->SetScreenCoords(Rect.m_Left, Rect.m_Top);
		pData->m_Color = Colors[0];
		pData++;

		pData->SetScreenCoords(Rect.m_Right, Rect.m_Top);
		pData->m_Color = Colors[1];
		pData++;

		pData->SetScreenCoords(Rect.m_Right, Rect.m_Bottom);
		pData->m_Color = Colors[2];
		pData++;

		pData->SetScreenCoords(Rect.m_Left, Rect.m_Bottom);
		pData->m_Color = Colors[3];
		pData++;

		pData->SetScreenCoords(Rect.m_Left, Rect.m_Top);
		pData->m_Color = Colors[0];
//		pData++;
	}

	// Rendering
	{
		g_GraphicsEffectGlobals.m_EffectVertexBuffer.Install();

		g_GraphicsStateManager.SetStateBlockNode
			(&g_GraphicsStateManager.m_NonTextured2DPrimitiveStateBlockNodes[dwTotalColorAlpha < 0xFF * 4]);

		DEBUG_EVALUATE_VERIFY
			(!g_GraphicsDeviceGlobals.m_pD3DDevice->
				DrawPrimitive(D3DPT_LINESTRIP, 0, 4));
	}
}
