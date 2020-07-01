#include "kfc_graphics_pch.h"
#include "surface.h"

#include "graphics_device_globals.h"
#include "pixel_formats.h"

// --------
// Surface
// --------
TSurface::TSurface()
{
	m_pSurface = NULL;
}

TSurface::TSurface(IDirect3DSurface9* pSSurface)
{
	m_pSurface = NULL;

	Allocate(pSSurface);
}

void TSurface::Release()
{
	if(m_pSurface)
		m_pSurface->Release(), m_pSurface = NULL;
}

void TSurface::Allocate(const SZSIZE&	SSize,
						D3DFORMAT		SFormat,
						D3DPOOL			Pool)
{
	Release();

	try
	{
		DEBUG_VERIFY(!SSize.IsFlat());
			
		DEBUG_VERIFY(SFormat != D3DFMT_UNKNOWN);

		m_Size		= SSize;
		m_Format	= SFormat;

		if(g_GraphicsDeviceGlobals.m_pD3DDevice->CreateOffscreenPlainSurface(	m_Size.cx,
																				m_Size.cy,
																				m_Format,
																				Pool,
																				&m_pSurface,
																				NULL))
		{
			m_pSurface = NULL;
			INITIATE_FAILURE;
		}
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TSurface::Allocate(IDirect3DSurface9* pSSurface)
{
	Release();

	try
	{
		DEBUG_VERIFY(pSSurface != NULL);

		m_pSurface = pSSurface;

		D3DSURFACE_DESC Desc;
		m_pSurface->GetDesc(&Desc);

		m_Size.Set(Desc.Width, Desc.Height);
		m_Format = Desc.Format;
	}
	
	catch(...)
	{
		Release();
		throw;
	}
}

void TSurface::CreateFromImage(	const TImage&	Image,
								D3DFORMAT		SFormat,
								const SZRECT*	pSrcRect,
								bool			bClampOutside)
{
	Release();

	try
	{
		DEBUG_VERIFY(Image.IsAllocated());

		// Allocating
		Allocate(pSrcRect ? SZSIZE(*pSrcRect) : Image.GetSize(), SFormat);

		// Filling
		FillFromImage(Image, pSrcRect, bClampOutside);
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TSurface::FillFromImage(	const TImage&	Image,
								const SZRECT*	pSrcRect,
								bool			bClampOutside)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(Image.IsAllocated());

	// Destination size
	const SZSIZE DstSize = pSrcRect ? SZSIZE(*pSrcRect) : Image.GetSize();

	DEBUG_VERIFY(!(DstSize > m_Size));

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

		TSurfaceLocker Locker1(*this, pData, szPitch);

		FillD3DPixelBuffer(	Image,
							pData,
							szPitch,
							m_Format,
							pSrcRect,
							bClampOutside ? &m_Size : 0);
	}
}

void TSurface::Lock(void*&			pRData,
					size_t&			szRPitch,
					const SZRECT*	pRect,
					flags_t			flFlags)
{
	DEBUG_VERIFY_ALLOCATION;

	const size_t szPitchDivisor = TPixelFormatsInfo::GetPixelBytesLength(m_Format);

	D3DLOCKED_RECT LockedRect;
	DEBUG_EVALUATE_VERIFY(m_pSurface->LockRect(&LockedRect, *pRect, flFlags) == 0);

	pRData		= LockedRect.pBits;
	szRPitch	= LockedRect.Pitch / szPitchDivisor;
}

void TSurface::Lock(BYTE*&			pRData,
					size_t&			szRPitch,
					const SZRECT*	pRect,
					flags_t			flFlags)
{
	DEBUG_VERIFY_ALLOCATION;

	D3DLOCKED_RECT LockedRect;
	DEBUG_EVALUATE_VERIFY(m_pSurface->LockRect(&LockedRect, *pRect, flFlags) == 0);

	pRData		= (BYTE*)	LockedRect.pBits;
	szRPitch	= (size_t)	LockedRect.Pitch;
}

void TSurface::Lock(WORD*&			pRData,
					size_t&			szRPitch,
					const SZRECT*	pRect,
					flags_t			flFlags)
{
	DEBUG_VERIFY_ALLOCATION;

	D3DLOCKED_RECT LockedRect;
	DEBUG_EVALUATE_VERIFY(m_pSurface->LockRect(&LockedRect, *pRect, flFlags) == 0);

	pRData		= (WORD*)	LockedRect.pBits;
	szRPitch	= (size_t)	LockedRect.Pitch >> 1;
}

void TSurface::Lock(DWORD*&			pRData,
					size_t&			szRPitch,
					const SZRECT*	pRect,
					flags_t			flFlags)
{
	DEBUG_VERIFY_ALLOCATION;

	D3DLOCKED_RECT LockedRect;
	DEBUG_EVALUATE_VERIFY(m_pSurface->LockRect(&LockedRect, *pRect, flFlags) == 0);

	pRData		= (DWORD*)LockedRect.pBits;
	szRPitch	= (size_t)LockedRect.Pitch >> 2;
}

void TSurface::Unlock()
{
	DEBUG_VERIFY_ALLOCATION;

	m_pSurface->UnlockRect();
}

void TSurface::Copy(TSurface&				DstSurface,
					const IPOINT&			DstCoords,
					const IRECT*			pSrcRect,
					D3DTEXTUREFILTERTYPE	Filter) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(DstSurface.IsAllocated());

	IRECT TempRect;

	if(pSrcRect == NULL)
		pSrcRect = &TempRect.Set(0, 0, m_Size.cx, m_Size.cy);

	if(g_GraphicsDeviceGlobals.m_pD3DDevice->StretchRect(	m_pSurface,
															*pSrcRect,
															DstSurface,
															ShiftRect(IRECT(*pSrcRect), ISIZE(DstCoords)),
															Filter))
	{
		throw;
	}
}

void TSurface::CopyClipped(	TSurface&				DstSurface,
							IPOINT&					DstCoords,
							IRECT*					pSrcRect,
							const IRECT*			pDstClipRect,
							D3DTEXTUREFILTERTYPE	Filter) const
{
	DEBUG_VERIFY_ALLOCATION;	

	DEBUG_VERIFY(DstSurface.IsAllocated());

	// Internal source rect
	IRECT TempSrcRect;

	if(pSrcRect == NULL)
		pSrcRect = &TempSrcRect.Set(0, 0, m_Size.cx, m_Size.cy);

	// Internal destination clip rect
	IRECT TempDstClipRect;

	if(pDstClipRect == NULL)
		pDstClipRect = &TempDstClipRect.Set(0, 0, DstSurface.GetSize().cx, DstSurface.GetSize().cy);

	// Shifting to destination coords
	ShiftRect(*pSrcRect, ISIZE(DstCoords));
	
	// Intersecting
	if(IntersectRect(*pSrcRect, *pDstClipRect) == 0)
		return;

	// Shifting back
	ShiftRect(*pSrcRect, -ISIZE(DstCoords));

	// Fixing desination coords
	if(DstCoords.x < 0)
		DstCoords.x = 0;

	if(DstCoords.y < 0)
		DstCoords.y = 0;

	Copy(DstSurface, DstCoords, pSrcRect, Filter);
}

void TSurface::CopyRects(	TSurface&				DstSurface,
							const IRECT*			pSrcRects,
							const IPOINT*			pDstCoords,
							size_t					szN,
							D3DTEXTUREFILTERTYPE	Filter) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(DstSurface.IsAllocated());

	if(pSrcRects == NULL && pDstCoords == NULL && szN > 1)
		szN = 1;

	size_t i;

	for(i = 0 ; i < szN ; i++)
	{
		Copy(	DstSurface,
				pDstCoords ?
					pDstCoords[i] :
					IPOINT(0, 0),
				pSrcRects ?
					pSrcRects + i :
					NULL,
				Filter);
	}
}

void TSurface::CopyRectsClipped(TSurface&				DstSurface,
								const IRECT*			pSrcRects,
								const IPOINT*			pDstCoords,
								size_t					szN,
								const IRECT*			pDstClipRect,
								D3DTEXTUREFILTERTYPE	Filter) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(DstSurface.IsAllocated());

	// Internal destination clip rect
	IRECT TempDstClipRect;

	if(pDstClipRect == NULL)
		pDstClipRect = &TempDstClipRect.Set(0, 0, DstSurface.GetSize().cx, DstSurface.GetSize().cy);

	const IRECT  TempSrcRect  (0, 0, m_Size.cx, m_Size.cy);
	const IPOINT TempDstCoords(0, 0);

	// Clipping source rects
	for(size_t i = 0 ; i < szN ; i++)
	{
		IRECT CurSrcRect    = pSrcRects	 ? pSrcRects [i] : TempSrcRect;
		IPOINT CurDstCoords = pDstCoords ? pDstCoords[i] : TempDstCoords;

		// Shifting to destination coords
		ShiftRect(CurSrcRect, ISIZE(CurDstCoords.x, CurDstCoords.y));
		
		// Intersecting
		if(IntersectRect(CurSrcRect, *pDstClipRect) == 0)
			CurSrcRect.Invalidate();

		// Shifting back
		ShiftRect(CurSrcRect, ISIZE(-CurDstCoords.x, -CurDstCoords.y));

		// Fixing desination coords
		if(CurDstCoords.x < 0)
			CurDstCoords.x = 0;

		if(CurDstCoords.y < 0)
			CurDstCoords.y = 0;

		if(CurSrcRect.IsValid())
			Copy(DstSurface, CurDstCoords, &CurSrcRect, Filter);
	}
}

void TSurface::SaveContents(const KString& FileName) const
{
	DEBUG_VERIFY_ALLOCATION;

	TImage Image;
	Image.Allocate(m_Size);

	{
		void*	pData;
		size_t	szPitch;
		TSurfaceLocker Locker0(*const_cast<TSurface*>(this), pData, szPitch, NULL, D3DLOCK_READONLY);

		RetrieveD3DPixelBuffer(Image, pData, szPitch, m_Format);
	}

	Image.Save(FileName);
}

// ---------------
// Surface locker
// ---------------
TSurfaceLocker::TSurfaceLocker(	TSurface&		SSurface,
								void*&			pRData,
								size_t&			szRPitch,
								const SZRECT*	pRect,
								flags_t			flFlags) : m_Surface(SSurface)
{
	m_Surface.Lock(pRData, szRPitch, pRect, flFlags);
}

TSurfaceLocker::TSurfaceLocker(	TSurface&		SSurface,
								BYTE*&			pRData,
								size_t&			szRPitch,
								const SZRECT*	pRect,
								flags_t			flFlags) : m_Surface(SSurface)
{
	m_Surface.Lock(pRData, szRPitch, pRect, flFlags);
}

TSurfaceLocker::TSurfaceLocker(	TSurface&		SSurface,
								WORD*&			pRData,
								size_t&			szRPitch,
								const SZRECT*	pRect,
								flags_t			flFlags) : m_Surface(SSurface)
{
	m_Surface.Lock(pRData, szRPitch, pRect, flFlags);
}

TSurfaceLocker::TSurfaceLocker(	TSurface&		SSurface,
								DWORD*&			pRData,
								size_t&			szRPitch,
								const SZRECT*	pRect,
								flags_t			flFlags) : m_Surface(SSurface)
{
	m_Surface.Lock(pRData, szRPitch, pRect, flFlags);
}

TSurfaceLocker::~TSurfaceLocker()
{
	m_Surface.Unlock();
}
