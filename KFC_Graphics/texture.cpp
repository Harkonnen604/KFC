#include "kfc_graphics_pch.h"
#include "texture.h"

#include <KFC_Common\pow2.h>
#include "surface.h"
#include "pixel_formats.h"
#include "graphics_device_globals.h"
#include "graphics_state_manager.h"

// --------
// Texture
// --------
TTexture::TTexture()
{
	m_pTexture = NULL;
}

TTexture::TTexture(LPDIRECT3DTEXTURE9 pSTexture)
{
	m_pTexture = NULL;

	Allocate(pSTexture);
}

void TTexture::Release()
{
	if(m_pTexture)
	{
		g_GraphicsStateManager.InvalidateTexture(UINT_MAX, m_pTexture);

		m_pTexture->Release(), m_pTexture = NULL;
	}
}

void TTexture::Allocate(const SZSIZE&	SSize,
						D3DFORMAT		SFormat,
						D3DPOOL			Pool,
						flags_t			flUsage,
						size_t			szSLevels)
{
	Release();

	try
	{
		DEBUG_VERIFY(SFormat != D3DFMT_UNKNOWN);

		SZSIZE TempSize;

		if(g_GraphicsDeviceGlobals.m_bPow2OnlyTextures)
		{
			TempSize.Set(	MinPow2GreaterEq(SSize.cx),
							MinPow2GreaterEq(SSize.cy));
		}
		else
		{
			TempSize = SSize;
		}

		if(g_GraphicsDeviceGlobals.m_bSquareOnlyTextures)
		{
			if(TempSize.cx > TempSize.cy)
				TempSize.cy = TempSize.cx;
			else if(TempSize.cy > TempSize.cx)
				TempSize.cx = TempSize.cy;
		}

		DEBUG_VERIFY(!TempSize.IsFlat());

		m_Size		= TempSize;		
		m_szLevels	= szSLevels;
		m_Format	= SFormat;

		m_InvSize.Set(1.0f / m_Size.cx, 1.0f / m_Size.cy);

		if(g_GraphicsDeviceGlobals.m_pD3DDevice->CreateTexture(	m_Size.cx,
																m_Size.cy,
																m_szLevels,
																flUsage,
																m_Format,
																Pool,
																&m_pTexture,
																NULL))
		{
			m_pTexture = NULL;
			INITIATE_FAILURE;
		}
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TTexture::Allocate(LPDIRECT3DTEXTURE9 pSTexture)
{
	Release();

	try
	{
		DEBUG_VERIFY(pSTexture != NULL);

		m_pTexture = pSTexture;
		
		m_szLevels = m_pTexture->GetLevelCount();

		m_Size = TSurface(GetSurface()).GetSize();

		m_InvSize.Set(1.0f / m_Size.cx, 1.0f / m_Size.cy);
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TTexture::CreateFromImage(	const TImage&	Image,
								D3DFORMAT		SFormat,
								const SZRECT*	pSrcRect,
								bool			bClampOutside,
								D3DPOOL			Pool,
								flags_t			flUsage,
								size_t			szSLevels)
{
	Release();

	try
	{
		DEBUG_VERIFY(Image.IsAllocated());

//		DEBUG_VERIFY(Pool != D3DPOOL_DEFAULT); // non-lockable

		// Allocating
		Allocate(	pSrcRect ? SZSIZE(*pSrcRect) : Image.GetSize(),
					SFormat,
					Pool,
					flUsage,
					szSLevels);

		// Filling
		FillFromImage(Image, pSrcRect, bClampOutside);
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TTexture::FillFromImage(	const TImage&	Image,
								const SZRECT*	pSrcRect,
								bool			bClampOutside)
{
	DEBUG_VERIFY_ALLOCATION;
	
	DEBUG_VERIFY(Image.IsAllocated());
	
	// Destination size
	const SZSIZE DstSize = pSrcRect ? SZSIZE(*pSrcRect) : Image.GetSize();

	if(DstSize > m_Size)
		INITIATE_DEFINED_FAILURE(TEXT("Image is larger then texture on at least one axis."));

	// Destination rect
	SZRECT			DstRect;
	const SZRECT*	pDstRect;

	if(pSrcRect)
		DstRect.Set(0, 0, DstSize.cx, DstSize.cy), pDstRect = &DstRect;
	else
		pDstRect = NULL;

	// Filling
	{
		void*	pData;
		size_t	szPitch;

		TTextureLocker Locker1(*this, pData, szPitch);

		FillD3DPixelBuffer(	Image,
							pData,
							szPitch,
							m_Format,
							pSrcRect,
							bClampOutside ? &m_Size : 0);
	}
}

void TTexture::Lock(void*&			pRData,
					size_t&			szRPitch,
					size_t			szLevel,
					const SZRECT*	pRect,
					flags_t			flFlags)
{
	DEBUG_VERIFY_ALLOCATION;

	const size_t szPitchDivisor = TPixelFormatsInfo::GetPixelBytesLength(m_Format);

	D3DLOCKED_RECT LockedRect;
	DEBUG_EVALUATE_VERIFY(m_pTexture->LockRect(szLevel, &LockedRect, *pRect, flFlags) == 0);

	pRData		= LockedRect.pBits;
	szRPitch	= LockedRect.Pitch / szPitchDivisor;
}

void TTexture::Lock(BYTE*&			pRData,
					size_t&			szRPitch,
					size_t			szLevel,
					const SZRECT*	pRect,
					flags_t			flFlags)
{
	DEBUG_VERIFY_ALLOCATION;

	D3DLOCKED_RECT LockedRect;
	DEBUG_EVALUATE_VERIFY(m_pTexture->LockRect(szLevel, &LockedRect, *pRect, flFlags) == 0);

	pRData		= (BYTE*)	LockedRect.pBits;
	szRPitch	= (size_t)	LockedRect.Pitch;
}

void TTexture::Lock(WORD*&			pRData,
					size_t&			szRPitch,
					size_t			szLevel,
					const SZRECT*	pRect,
					flags_t			flFlags)
{
	DEBUG_VERIFY_ALLOCATION;

	D3DLOCKED_RECT LockedRect;
	DEBUG_EVALUATE_VERIFY(m_pTexture->LockRect(szLevel, &LockedRect, *pRect, flFlags) == 0);

	pRData		= (WORD*)	LockedRect.pBits;
	szRPitch	= (size_t)	LockedRect.Pitch >> 1;
}

void TTexture::Lock(DWORD*&			pRData,
					size_t&			szRPitch,
					size_t			szLevel,
					const SZRECT*	pRect,
					flags_t			flFlags)
{
	DEBUG_VERIFY_ALLOCATION;

	D3DLOCKED_RECT LockedRect;
	DEBUG_EVALUATE_VERIFY(m_pTexture->LockRect(szLevel, &LockedRect, *pRect, flFlags) == 0);

	pRData		= (DWORD*)	LockedRect.pBits;
	szRPitch	= (size_t)	LockedRect.Pitch >> 2;
}

void TTexture::Unlock(size_t szLevel)
{
	DEBUG_VERIFY_ALLOCATION;

	m_pTexture->UnlockRect(szLevel);
}

void TTexture::Install(size_t szStage) const
{
	DEBUG_VERIFY_ALLOCATION;

	g_GraphicsStateManager.SetTexture(szStage, m_pTexture);
}

void TTexture::DrawRect(const FPOINT&	DstCoords,
						D3DCOLOR		Color,
						const FSIZE*	pDstSize,
						const FRECT*	pSrcRect) const
{
	DEBUG_VERIFY_ALLOCATION;

	const DWORD dwColorAlpha = Color >> 24;

	if(dwColorAlpha == 0)
		return;

	// Internal destination size
	FSIZE TempDstSize;

	if(pDstSize == NULL)
	{
		pDstSize = &TempDstSize.Set((float)m_Size.cx, (float)m_Size.cy);
	}
	else
	{
		if(pDstSize->IsFlat())
			return;
	}

	// Destination rect
	const FRECT DstRect(DstCoords.x - 0.5f,
						DstCoords.y - 0.5f,
						DstCoords.x + pDstSize->cx - 0.5f,
						DstCoords.y + pDstSize->cy - 0.5f);

	// Internal source rect
	FRECT TempSrcRect;

	if(pSrcRect == NULL)
		pSrcRect = &TempSrcRect.Set(0.0f, 0.0f, 1.0f, 1.0f);

	// Preparing vertex buffer
	{
		TTLVertex<1>* pData;

		TRectsStripVertexBufferLocker Locker0(	g_GraphicsDeviceGlobals.m_RectsStripVertexBuffer,
												pData,
												D3DLOCK_DISCARD,
												&SZSEGMENT(0, 4));
		
		pData->SetScreenCoords(DstRect.m_Left, DstRect.m_Top);
		pData->m_Color = Color;
		pData->m_TextureCoords[0].Set(pSrcRect->m_Left, pSrcRect->m_Top);
		pData++;

		pData->SetScreenCoords(DstRect.m_Right, DstRect.m_Top);
		pData->m_Color = Color;
		pData->m_TextureCoords[0].Set(pSrcRect->m_Right, pSrcRect->m_Top);
		pData++;

		pData->SetScreenCoords(DstRect.m_Left, DstRect.m_Bottom);
		pData->m_Color = Color;
		pData->m_TextureCoords[0].Set(pSrcRect->m_Left, pSrcRect->m_Bottom);
		pData++;

		pData->SetScreenCoords(DstRect.m_Right, DstRect.m_Bottom);
		pData->m_Color = Color;
		pData->m_TextureCoords[0].Set(pSrcRect->m_Right, pSrcRect->m_Bottom);
//		pData++;
	}

	// Rendering
	{
		Install();

		g_GraphicsDeviceGlobals.m_RectsStripVertexBuffer.Install();

		g_GraphicsStateManager.SetStateBlockNode
			(	&g_GraphicsStateManager.m_Textured2DPrimitiveStateBlockNodes
				[TPixelFormatsInfo::IsRGBPixelFormat  (m_Format)]
				[TPixelFormatsInfo::IsAlphaPixelFormat(m_Format)]
				[dwColorAlpha < 0xFF]);

		DEBUG_EVALUATE_VERIFY
			(!g_GraphicsDeviceGlobals.m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2));
	}
}

TTexture::operator LPDIRECT3DTEXTURE9 ()
{
	DEBUG_VERIFY_ALLOCATION;

	return m_pTexture;
}

LPDIRECT3DSURFACE9 TTexture::GetSurface(size_t szLevel)
{
	DEBUG_VERIFY_ALLOCATION;

	int r;

	LPDIRECT3DSURFACE9 pSurface = NULL;

	if(r = m_pTexture->GetSurfaceLevel(szLevel, &pSurface))
		INITIATE_DEFINED_CODE_FAILURE(TEXT("Error getting DirectGraphics surface level"), r);

	return pSurface;
}
// ---------------
// Texture locker
// ---------------
TTextureLocker::TTextureLocker(	TTexture&		STexture,
								void*&			pRData,
								size_t&			szRPitch,
								size_t			szSLevel,
								const SZRECT*	pRect,
								flags_t			flFlags) :	m_Texture(STexture),
															m_szLevel(szSLevel)
{
	m_Texture.Lock(pRData, szRPitch, m_szLevel, pRect, flFlags);
}

TTextureLocker::TTextureLocker(	TTexture&		STexture,
								BYTE*&			pRData,
								size_t&			szRPitch,
								size_t			szSLevel,
								const SZRECT*	pRect,
								flags_t			flFlags) :	m_Texture(STexture),
															m_szLevel(szSLevel)
{
	m_Texture.Lock(pRData, szRPitch, m_szLevel, pRect, flFlags);
}

TTextureLocker::TTextureLocker(	TTexture&		STexture,
								WORD*&			pRData,
								size_t&			szRPitch,
								size_t			szSLevel,
								const SZRECT*	pRect,
								flags_t			flFlags) :	m_Texture(STexture),
															m_szLevel(szSLevel)
{
	m_Texture.Lock(pRData, szRPitch, m_szLevel, pRect, flFlags);
}

TTextureLocker::TTextureLocker(	TTexture&		STexture,
								DWORD*&			pRData,
								size_t&			szRPitch,
								size_t			szSLevel,
								const SZRECT*	pRect,
								flags_t			flFlags) :	m_Texture(STexture),
															m_szLevel(szSLevel)
{
	m_Texture.Lock(pRData, szRPitch, m_szLevel, pRect, flFlags);
}

TTextureLocker::~TTextureLocker()
{
	m_Texture.Unlock(m_szLevel);
}