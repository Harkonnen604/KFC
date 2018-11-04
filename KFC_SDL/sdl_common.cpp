#include "kfc_sdl_pch.h"
#include "sdl_common.h"

#include "sdl_control.h"
#include "sdl_interface.h"

// ------------
// SDL clipper
// ------------
T_SDL_Clipper::T_SDL_Clipper(SDL_Surface* pSurface, IRECT Rect)
{
	DEBUG_VERIFY(pSurface);
	
	m_pSurface = pSurface;

	m_OldClipRect = GetSDL_ClipRect(pSurface);

	if(!IntersectRect(Rect, m_OldClipRect))
		Rect.Set(1, 1, 1, 1); // avoiding potential "whole surface" treatment of (0, 0, 0, 0)

	SetSDL_ClipRect(m_pSurface, Rect);
}

T_SDL_Clipper::~T_SDL_Clipper()
{
	SetSDL_ClipRect(m_pSurface, m_OldClipRect);
}

// ----------------
// Global routines
// ----------------

// Image
void FillSDL_Image(SDL_Surface* pSurface, UINT32 uiColor, bool bUpdate)
{
	DEBUG_VERIFY(pSurface);
	DEBUG_VERIFY(!pSurface->locked);
	
	uiColor = GetSDL_Color(pSurface, uiColor);

	DEBUG_EVERIFY(!SDL_FillRect(pSurface, NULL, uiColor));
	
	if(bUpdate)
		UpdateSDL_Image(pSurface);
}

void FillSDL_Image(SDL_Surface* pSurface, IRECT Rect, UINT uiColor, bool bUpdate)
{
	DEBUG_VERIFY(pSurface);
	DEBUG_VERIFY(!pSurface->locked);

	if(!IntersectRect(Rect, GetSDL_ClipRect(pSurface))) // this might seem redundant, but it also checks for valid rect
		return;

	DEBUG_EVERIFY(!SDL_FillRect(pSurface, &temp<T_SDL_Rect>(Rect)(), GetSDL_Color(pSurface, uiColor)));
	
	if(bUpdate)
		UpdateSDL_ImageNoClip(pSurface, Rect);
}

void FrameSDL_Image(SDL_Surface* pSurface, const IRECT& Rect, UINT uiColor, bool bUpdate)
{
	if(!Rect.IsValid())
		return;

	DrawSDL_HLine(pSurface, IPOINT(Rect.m_Left, Rect.m_Top),		Rect.GetWidth(), uiColor, bUpdate);
	DrawSDL_HLine(pSurface, IPOINT(Rect.m_Left, Rect.m_Bottom - 1),	Rect.GetWidth(), uiColor, bUpdate);

	if(Rect.GetHeight() > 2)
	{
		DrawSDL_VLine(pSurface, IPOINT(Rect.m_Left,			Rect.m_Top + 1), Rect.GetHeight() - 2, uiColor, bUpdate);
		DrawSDL_VLine(pSurface, IPOINT(Rect.m_Right - 1,	Rect.m_Top + 1), Rect.GetHeight() - 2, uiColor, bUpdate);
	}
}

template <class t>
inline void BlendColor(SDL_Surface* pSurface, t& v, int r, int g, int b, int a)
{
	UINT32 c = GetSDL_RGB(pSurface, v);

	int cr = GetRValue(c);
	int cg = GetGValue(c);
	int cb = GetBValue(c);

	cr = (((r - cr) * a) >> 10) + cr;
	cg = (((g - cg) * a) >> 10) + cg;
	cb = (((b - cb) * a) >> 10) + cb;

	v = (t)GetSDL_Color(pSurface, RGB(cr, cg, cb));
}

template <class t>
void BlendFillSDL_ImageBPP_NoLock(SDL_Surface* pSurface, IRECT Rect, UINT uiColor, double dAlpha)
{
	DEBUG_VERIFY(!SDL_MUSTLOCK(pSurface) || pSurface->locked);

	int r = GetRValue(uiColor);
	int g = GetGValue(uiColor);
	int b = GetBValue(uiColor);

	int a = (int)Round(dAlpha * 1024);

	t* pCur = (t*)((BYTE*)pSurface->pixels + Rect.m_Top * pSurface->pitch) + Rect.m_Left;

	size_t szDelta = pSurface->pitch - Rect.GetWidth() * sizeof(t);

	for(size_t y = Rect.GetHeight() ; y ; y--, (BYTE*&)pCur += szDelta)
	{
		for(size_t x = Rect.GetWidth() ; x ; x--, pCur++)
			BlendColor(pSurface, *pCur, r, g, b, a);
	}
}

void BlendFillSDL_Image(SDL_Surface* pSurface, IRECT Rect, UINT uiColor, double dAlpha, bool bUpdate)
{
	DEBUG_VERIFY(pSurface);

	DEBUG_VERIFY(HitsSegmentBounds(dAlpha, DSEGMENT(0, 1)));

	if(!Compare(dAlpha, 0.0))
		return;

	if(!Compare(dAlpha, 1.0))
	{
		FillSDL_Image(pSurface, Rect, uiColor, bUpdate);
		return;
	}

	if(!IntersectRect(Rect, GetSDL_ClipRect(pSurface)))
		return;

	{
		T_SDL_SurfaceLocker Locker0(pSurface);

		if(pSurface->format->BytesPerPixel == 1)
			BlendFillSDL_ImageBPP_NoLock<BYTE>(pSurface, Rect, uiColor, dAlpha);
		else if(pSurface->format->BytesPerPixel == 2)
			BlendFillSDL_ImageBPP_NoLock<WORD>(pSurface, Rect, uiColor, dAlpha);
		else if(pSurface->format->BytesPerPixel == 4)
			BlendFillSDL_ImageBPP_NoLock<DWORD>(pSurface, Rect, uiColor, dAlpha);
		else
			INITIATE_DEFINED_FAILURE("Unsupported color depth for blended SDL image fill.");
	}

	if(bUpdate)
		UpdateSDL_ImageNoClip(pSurface, Rect);
}

void DrawSDL_HLineNoLock(	SDL_Surface*	pSurface,
							IPOINT			Coords,
							size_t			szLength,
							UINT32			uiColor)
{
	DEBUG_VERIFY(pSurface);
	DEBUG_VERIFY(!SDL_MUSTLOCK(pSurface) || pSurface->locked);

	IRECT ClipRect = GetSDL_ClipRect(pSurface);

	if(Coords.y < ClipRect.m_Top || Coords.y >= ClipRect.m_Bottom) // top/botttom clipping
		return;

	if(Coords.x < ClipRect.m_Left) // left clipping
	{
		if(ClipRect.m_Left - Coords.x >= (int)szLength)
			return;

		szLength -= ClipRect.m_Left - Coords.x, Coords.x = ClipRect.m_Left;
	}

	if(Coords.x >= ClipRect.m_Right) // right clipping
		return;

	UpdateMin(szLength, (size_t)(ClipRect.m_Right - Coords.x)); // right clipping

	int dlt = pSurface->pitch;

	if(pSurface->format->BytesPerPixel == 1)
	{
		BYTE v = (BYTE)GetSDL_Color(pSurface, uiColor);

		BYTE* pDst = (BYTE*)((BYTE*)pSurface->pixels + dlt * Coords.y) + Coords.x;

		for( ; szLength ; szLength--)
			*pDst++ = v;
	}
	else if(pSurface->format->BytesPerPixel == 2)
	{
		WORD v = (WORD)GetSDL_Color(pSurface, uiColor);

		WORD* pDst = (WORD*)((BYTE*)pSurface->pixels + dlt * Coords.y) + Coords.x;

		for( ; szLength ; szLength--)
			*pDst++ = v;
	}
	else if(pSurface->format->BytesPerPixel == 4)
	{
		DWORD v = (DWORD)GetSDL_Color(pSurface, uiColor);

		DWORD* pDst = (DWORD*)((BYTE*)pSurface->pixels + dlt * Coords.y) + Coords.x;

		for( ; szLength ; szLength--)
			*pDst++ = v;
	}
	else
	{
		INITIATE_DEFINED_FAILURE("Unsupported SDL surface color depth for line rendering.");
	}
}

void DrawSDL_VLineNoLock(	SDL_Surface*	pSurface,
							IPOINT			Coords,
							size_t			szLength,
							UINT32			uiColor)
{
	DEBUG_VERIFY(pSurface);
	DEBUG_VERIFY(!SDL_MUSTLOCK(pSurface) || pSurface->locked);

	IRECT ClipRect = GetSDL_ClipRect(pSurface);

	if(Coords.x < ClipRect.m_Left || Coords.x >= ClipRect.m_Right) // left/right clipping
		return;

	if(Coords.y < ClipRect.m_Top) // top clipping
	{
		if(ClipRect.m_Top - Coords.y >= (int)szLength)
			return;

		szLength -= ClipRect.m_Top - Coords.y, Coords.y = ClipRect.m_Top;
	}

	if(Coords.y >= ClipRect.m_Bottom) // bottom clipping
		return;

	UpdateMin(szLength, (size_t)(ClipRect.m_Bottom - Coords.y)); // bottom clipping

	int dlt = pSurface->pitch;

	if(pSurface->format->BytesPerPixel == 1)
	{
		BYTE v = (BYTE)GetSDL_Color(pSurface, uiColor);

		BYTE* pDst = (BYTE*)((BYTE*)pSurface->pixels + dlt * Coords.y) + Coords.x;

		for( ; szLength ; szLength--)
			*pDst = v, (BYTE*&)pDst += dlt;
	}
	else if(pSurface->format->BytesPerPixel == 2)
	{
		WORD v = (WORD)GetSDL_Color(pSurface, uiColor);

		WORD* pDst = (WORD*)((BYTE*)pSurface->pixels + dlt * Coords.y) + Coords.x;

		for( ; szLength ; szLength--)
			*pDst = v, (BYTE*&)pDst += dlt;
	}
	else if(pSurface->format->BytesPerPixel == 4)
	{
		DWORD v = (DWORD)GetSDL_Color(pSurface, uiColor);

		DWORD* pDst = (DWORD*)((BYTE*)pSurface->pixels + dlt * Coords.y) + Coords.x;

		for( ; szLength ; szLength--)
			*pDst = v, (BYTE*&)pDst += dlt;
	}
	else
	{
		INITIATE_DEFINED_FAILURE("Unsupported SDL surface color depth for line rendering.");
	}
}

void DrawSDL_DashedHLineNoLock(	SDL_Surface*	pSurface,
								IPOINT			Coords,
								size_t			szLength,
								size_t			szStrideLength,
								size_t			szSpaceLength,
								UINT32			uiColor,
								bool			bRightAlign)
{
	DEBUG_VERIFY(pSurface);
	DEBUG_VERIFY(!SDL_MUSTLOCK(pSurface) || pSurface->locked);	

	IRECT ClipRect = GetSDL_ClipRect(pSurface);

	if(Coords.y < ClipRect.m_Top || Coords.y >= ClipRect.m_Bottom) // top/botttom clipping
		return;

	size_t szTotalLength = szStrideLength + szSpaceLength;
	DEBUG_VERIFY(szTotalLength > 0);

	size_t szPos = bRightAlign ? (szStrideLength - szLength % szTotalLength + szTotalLength) % szTotalLength : 0;

	if(Coords.x < ClipRect.m_Left) // left clipping
	{
		if(ClipRect.m_Left - Coords.x >= (int)szLength)
			return;

		(szPos += ClipRect.m_Left - Coords.x) %= szTotalLength;

		szLength -= ClipRect.m_Left - Coords.x, Coords.x = ClipRect.m_Left;
	}

	if(Coords.x >= ClipRect.m_Right) // right clipping
		return;

	UpdateMin(szLength, (size_t)(ClipRect.m_Right - Coords.x)); // right clipping

	int dlt = pSurface->pitch;

	if(pSurface->format->BytesPerPixel == 1)
	{
		BYTE v = (BYTE)GetSDL_Color(pSurface, uiColor);

		BYTE* pDst = (BYTE*)((BYTE*)pSurface->pixels + dlt * Coords.y) + Coords.x;

		for( ; szLength ; szLength--, pDst++)
		{
			if(szPos < szStrideLength)
				*pDst = v;

			if(++szPos == szTotalLength)
				szPos = 0;
		}
	}
	else if(pSurface->format->BytesPerPixel == 2)
	{
		WORD v = (WORD)GetSDL_Color(pSurface, uiColor);

		WORD* pDst = (WORD*)((BYTE*)pSurface->pixels + dlt * Coords.y) + Coords.x;

		for( ; szLength ; szLength--, pDst++)
		{
			if(szPos < szStrideLength)
				*pDst = v;

			if(++szPos == szTotalLength)
				szPos = 0;
		}
	}
	else if(pSurface->format->BytesPerPixel == 4)
	{
		DWORD v = (DWORD)GetSDL_Color(pSurface, uiColor);

		DWORD* pDst = (DWORD*)((BYTE*)pSurface->pixels + dlt * Coords.y) + Coords.x;

		for( ; szLength ; szLength--, pDst++)
		{
			if(szPos < szStrideLength)
				*pDst = v;

			if(++szPos == szTotalLength)
				szPos = 0;
		}
	}
	else
	{
		INITIATE_DEFINED_FAILURE("Unsupported SDL surface color depth for line rendering.");
	}
}

void DrawSDL_DashedVLineNoLock(	SDL_Surface*	pSurface,
								IPOINT			Coords,
								size_t			szLength,
								size_t			szStrideLength,
								size_t			szSpaceLength,
								UINT32			uiColor,
								bool			bBottomAlign)
{
	DEBUG_VERIFY(pSurface);
	DEBUG_VERIFY(!SDL_MUSTLOCK(pSurface) || pSurface->locked);

	IRECT ClipRect = GetSDL_ClipRect(pSurface);

	if(Coords.x < ClipRect.m_Left || Coords.x >= ClipRect.m_Right) // left/right clipping
		return;

	size_t szTotalLength = szStrideLength + szSpaceLength;
	DEBUG_VERIFY(szTotalLength > 0);

	size_t szPos = bBottomAlign ? (szStrideLength - szLength % szTotalLength + szTotalLength) % szTotalLength : 0;

	if(Coords.y < ClipRect.m_Top) // top clipping
	{
		if(ClipRect.m_Top - Coords.y >= (int)szLength)
			return;

		(szPos += ClipRect.m_Top - Coords.y) %= szTotalLength;

		szLength -= ClipRect.m_Top - Coords.y, Coords.y = ClipRect.m_Top;
	}

	if(Coords.y >= ClipRect.m_Bottom) // bottom clipping
		return;

	UpdateMin(szLength, (size_t)(ClipRect.m_Bottom - Coords.y)); // bottom clipping

	int dlt = pSurface->pitch;

	if(pSurface->format->BytesPerPixel == 1)
	{
		BYTE v = (BYTE)GetSDL_Color(pSurface, uiColor);

		BYTE* pDst = (BYTE*)((BYTE*)pSurface->pixels + dlt * Coords.y) + Coords.x;

		for( ; szLength ; szLength--, (BYTE*&)pDst += dlt)
		{
			if(szPos < szStrideLength)
				*pDst = v;

			if(++szPos == szTotalLength)
				szPos = 0;
		}
	}
	else if(pSurface->format->BytesPerPixel == 2)
	{
		WORD v = (WORD)GetSDL_Color(pSurface, uiColor);

		WORD* pDst = (WORD*)((BYTE*)pSurface->pixels + dlt * Coords.y) + Coords.x;

		for( ; szLength ; szLength--, (BYTE*&)pDst += dlt)
		{
			if(szPos < szStrideLength)
				*pDst = v;

			if(++szPos == szTotalLength)
				szPos = 0;
		}
	}
	else if(pSurface->format->BytesPerPixel == 4)
	{
		DWORD v = (DWORD)GetSDL_Color(pSurface, uiColor);

		DWORD* pDst = (DWORD*)((BYTE*)pSurface->pixels + dlt * Coords.y) + Coords.x;

		for( ; szLength ; szLength--, (BYTE*&)pDst += dlt)
		{
			if(szPos < szStrideLength)
				*pDst = v;

			if(++szPos == szTotalLength)
				szPos = 0;
		}
	}
	else
	{
		INITIATE_DEFINED_FAILURE("Unsupported SDL surface color depth for line rendering.");
	}
}

template <class t>
void DrawSDL_LineNoLockBPP(	SDL_Surface*	pSurface,
							const IPOINT&	Coords1,
							const IPOINT&	Coords2,
							UINT32			uiColor)
{
	int dx = Coords2.x - Coords1.x;
	int dy = Coords2.y - Coords1.y;

	if(!dx && !dy)
		return;

	int xs, ys;

	if(dx < 0)
		dx = -dx, xs = -1;
	else
		xs = 1;

	if(dy < 0)
		dy = -dy, ys = -1;
	else
		ys = 1;

	int adx = abs(dx), ady = abs(dy);

	if(!ady)
	{
		DrawSDL_HLineNoLock(pSurface, IPOINT(Min(Coords1.x, Coords2.x), Coords1.y), adx + 1, uiColor);
		return;
	}

	if(!adx)
	{
		DrawSDL_VLineNoLock(pSurface, IPOINT(Coords1.x, Min(Coords1.y, Coords2.y)), ady + 1, uiColor);
		return;
	}

	IPOINT cp = Coords1;

	IRECT cr = GetSDL_ClipRect(pSurface);

	if(adx >= ady)
	{
		int e  = 0;
		int de = ady + 1;
		int cs = adx + 1;

		int iDlt = pSurface->pitch * ys;

		t* pDst = (t*)((BYTE*)pSurface->pixels + pSurface->pitch * cp.y) + cp.x;				

		t v = (t)GetSDL_Color(pSurface, uiColor);

		for(;;)
		{
			if(HitsRect(cp, cr))
				*pDst = v;

			if(cp.x == Coords2.x)
				break;

			if((e += de) >= cs)
				e -= cs, cp.y += ys, (BYTE*&)pDst += iDlt;

			cp.x += xs, pDst += xs;
		}

		DEBUG_VERIFY(cp == Coords2);
	}
	else // adx < ady
	{
		int e  = 0;
		int de = adx + 1;
		int cs = ady + 1;

		int iDlt = pSurface->pitch * ys;

		t* pDst = (t*)((BYTE*)pSurface->pixels + pSurface->pitch * cp.y) + cp.x;

		t v = (t)GetSDL_Color(pSurface, uiColor);

		for(;;)
		{
			if(HitsRect(cp, cr))
				*pDst = v;

			if(cp.y == Coords2.y)
				break;

			if((e += de) >= cs)
				e -= cs, cp.x += xs, pDst += xs;

			cp.y += ys, (BYTE*&)pDst += iDlt;
		}

		DEBUG_VERIFY(cp == Coords2);
	}
}

void DrawSDL_LineNoLock(SDL_Surface*	pSurface,
						const IPOINT&	Coords1,
						const IPOINT&	Coords2,
						UINT32			uiColor)
{
	DEBUG_VERIFY(pSurface);
	DEBUG_VERIFY(!SDL_MUSTLOCK(pSurface) || pSurface->locked);

	if(pSurface->format->BytesPerPixel == 1)
		DrawSDL_LineNoLockBPP<BYTE>(pSurface, Coords1, Coords2, uiColor);
	else if(pSurface->format->BytesPerPixel == 2)
		DrawSDL_LineNoLockBPP<WORD>(pSurface, Coords1, Coords2, uiColor);
	else if(pSurface->format->BytesPerPixel == 4)
		DrawSDL_LineNoLockBPP<DWORD>(pSurface, Coords1, Coords2, uiColor);
	else
		INITIATE_DEFINED_FAILURE("Unsupported color depth for line rendering.");
}

template <class t>
void DrawSDL_AA_LineNoLockBPP(	SDL_Surface*	pSurface,
								const IPOINT&	Coords1,
								const IPOINT&	Coords2,
								UINT32			uiColor)
{
	int dx = Coords2.x - Coords1.x;
	int dy = Coords2.y - Coords1.y;

	if(!dx && !dy)
		return;

	int xs, ys;

	if(dx < 0)
		dx = -dx, xs = -1;
	else
		xs = 1;

	if(dy < 0)
		dy = -dy, ys = -1;
	else
		ys = 1;

	int adx = abs(dx), ady = abs(dy);

	if(!ady)
	{
		DrawSDL_HLineNoLock(pSurface, IPOINT(Min(Coords1.x, Coords2.x), Coords1.y), adx + 1, uiColor);
		return;
	}

	if(!adx)
	{
		DrawSDL_VLineNoLock(pSurface, IPOINT(Coords1.x, Min(Coords1.y, Coords2.y)), ady + 1, uiColor);
		return;
	}

	IRECT cr = GetSDL_ClipRect(pSurface);

	int r = GetRValue(uiColor);
	int g = GetGValue(uiColor);
	int b = GetBValue(uiColor);	

	if(adx >= ady)
	{
		IPOINT cp1 = Coords1;
		IPOINT cp2 = Coords1;
		cp2.y += ys;

		t* pDst1 = (t*)((BYTE*)pSurface->pixels + pSurface->pitch * cp1.y) + cp1.x;
		t* pDst2 = (t*)((BYTE*)pSurface->pixels + pSurface->pitch * cp2.y) + cp2.x;

		int iDlt = pSurface->pitch * ys;

		int e  = 0;
		int de = ady;
		int cs = adx;

		for(;;)
		{
			int a = (e<<10) / cs;

			if(HitsRect(cp1, cr))
				BlendColor<t>(pSurface, *pDst1, r, g, b, 1024-a);

			if(HitsRect(cp2, cr))
				BlendColor<t>(pSurface, *pDst2, r, g, b, a);

			if(cp1.x == Coords2.x)
				break;

			if((e += de) >= cs)
				e -= cs, cp1.y += ys, cp2.y += ys, (BYTE*&)pDst1 += iDlt, (BYTE*&)pDst2 += iDlt;

			cp1.x += xs, cp2.x += xs, pDst1 += xs, pDst2 += xs;
		}

		DEBUG_VERIFY(cp1 == Coords2);
	}
	else // adx < ady
	{
		IPOINT cp1 = Coords1;
		IPOINT cp2 = Coords1;
		cp2.x += xs;

		t* pDst1 = (t*)((BYTE*)pSurface->pixels + pSurface->pitch * cp1.y) + cp1.x;
		t* pDst2 = (t*)((BYTE*)pSurface->pixels + pSurface->pitch * cp2.y) + cp2.x;

		int iDlt = pSurface->pitch * ys;

		int e  = 0;
		int de = adx;
		int cs = ady;

		for(;;)
		{
			int a = (e<<10) / cs;

			if(HitsRect(cp1, cr))
				BlendColor<t>(pSurface, *pDst1, r, g, b, 1024-a);

			if(HitsRect(cp2, cr))
				BlendColor<t>(pSurface, *pDst2, r, g, b, a);

			if(cp1.y == Coords2.y)
				break;

			if((e += de) >= cs)
				e -= cs, cp1.x += xs, cp2.x += xs, pDst1 += xs, pDst2 += xs;

			cp1.y += ys, cp2.y += ys, (BYTE*&)pDst1 += iDlt, (BYTE*&)pDst2 += iDlt;
		}

		DEBUG_VERIFY(cp1 == Coords2);
	}
}

void DrawSDL_AA_LineNoLock(	SDL_Surface*	pSurface,
							const IPOINT&	Coords1,
							const IPOINT&	Coords2,
							UINT32			uiColor)
{
	DEBUG_VERIFY(pSurface);
	DEBUG_VERIFY(!SDL_MUSTLOCK(pSurface) || pSurface->locked);

	if(pSurface->format->BytesPerPixel == 1)
		DrawSDL_AA_LineNoLockBPP<BYTE>(pSurface, Coords1, Coords2, uiColor);
	else if(pSurface->format->BytesPerPixel == 2)
		DrawSDL_AA_LineNoLockBPP<WORD>(pSurface, Coords1, Coords2, uiColor);
	else if(pSurface->format->BytesPerPixel == 4)
		DrawSDL_AA_LineNoLockBPP<DWORD>(pSurface, Coords1, Coords2, uiColor);
	else
		INITIATE_DEFINED_FAILURE("Unsupported color depth for antialiased line rendering.");
}

template <class t>
void StretchBlitSDL_ImageNoLockBPP(SDL_Surface* pSrcSurface, const IRECT& SrcRect, SDL_Surface* pDstSurface, const IRECT& DstRect)
{
	IRECT SrcClipRect = GetSDL_ClipRect(pSrcSurface);
	IRECT DstClipRect = GetSDL_ClipRect(pDstSurface);

	const t* pSrc = (const t*)((const BYTE*)pSrcSurface->pixels + pSrcSurface->pitch * SrcRect.m_Top) + SrcRect.m_Left;

	t* pDst = (t*)((BYTE*)pDstSurface->pixels + pDstSurface->pitch * DstRect.m_Top) + DstRect.m_Left;

	size_t szSrcPitch = pSrcSurface->pitch;
	size_t szDstPitch = pDstSurface->pitch;

	if(SrcRect.GetHeight() >= DstRect.GetHeight())
	{
		int ye  = 0;
		int yde = DstRect.GetHeight();
		int ycs = SrcRect.GetHeight();

		int sy = SrcRect.m_Top;
		int dy = DstRect.m_Top;

		for(;;)
		{
			if(sy == SrcRect.m_Bottom)
				break;

			DEBUG_VERIFY(dy < DstRect.m_Bottom);

			if(sy >= SrcClipRect.m_Bottom || dy >= DstClipRect.m_Bottom)
				break;

			if(sy >= SrcClipRect.m_Top && dy >= DstClipRect.m_Top)
			{
				const t* pCurSrc = pSrc;

				t* pCurDst = pDst;

				if(SrcRect.GetWidth() >= DstRect.GetWidth())
				{
					int xe  = 0;
					int xde = DstRect.GetWidth();
					int xcs = SrcRect.GetWidth();

					int sx = SrcRect.m_Left;
					int dx = DstRect.m_Left;

					for(;;)
					{
						if(sx == SrcRect.m_Right)
							break;

						DEBUG_VERIFY(dx < DstRect.m_Right);

						if(sx >= SrcClipRect.m_Right || dx >= DstClipRect.m_Right)
							break;

						if(sx >= SrcClipRect.m_Left && dx >= DstClipRect.m_Left)
							*pCurDst = *pCurSrc;

						if((xe += xde) >= xcs)
							xe -= xcs, dx++, pCurDst++;

						sx++, pCurSrc++;
					}
				}
				else
				{
					int xe  = 0;
					int xde = SrcRect.GetWidth();
					int xcs = DstRect.GetWidth();

					int sx = SrcRect.m_Left;
					int dx = DstRect.m_Left;

					for(;;)
					{
						if(dx == DstRect.m_Right)
							break;

						DEBUG_VERIFY(sx < SrcRect.m_Right);

						if(sx >= SrcClipRect.m_Right || dx >= DstClipRect.m_Right)
							break;

						if(sx >= SrcClipRect.m_Left && dx >= DstClipRect.m_Left)
							*pCurDst = *pCurSrc;

						if((xe += xde) >= xcs)
							xe -= xcs, sx++, pCurSrc++;

						dx++, pCurDst++;
					}
				}
			}

			if((ye += yde) >= ycs)
				ye -= ycs, dy++, (BYTE*&)pDst += szDstPitch;

			sy++, (const BYTE*&)pSrc += szSrcPitch;
		}
	}
	else
	{
		int ye  = 0;
		int yde = SrcRect.GetHeight();
		int ycs = DstRect.GetHeight();

		int sy = SrcRect.m_Top;
		int dy = DstRect.m_Top;

		for(;;)
		{
			if(dy == DstRect.m_Bottom)
				break;

			DEBUG_VERIFY(sy < SrcRect.m_Bottom);

			if(sy >= SrcClipRect.m_Bottom || dy >= DstClipRect.m_Bottom)
				break;

			if(sy >= SrcClipRect.m_Top && dy >= DstClipRect.m_Top)
			{
				const t* pCurSrc = pSrc;

				t* pCurDst = pDst;

				if(SrcRect.GetWidth() >= DstRect.GetWidth())
				{
					int xe  = 0;
					int xde = DstRect.GetWidth();
					int xcs = SrcRect.GetWidth();

					int sx = SrcRect.m_Left;
					int dx = DstRect.m_Left;

					for(;;)
					{
						if(sx == SrcRect.m_Right)
							break;

						DEBUG_VERIFY(dx < DstRect.m_Right);

						if(sx >= SrcClipRect.m_Right || dx >= DstClipRect.m_Right)
							break;

						if(sx >= SrcClipRect.m_Left && dx >= DstClipRect.m_Left)
							*pCurDst = *pCurSrc;

						if((xe += xde) >= xcs)
							xe -= xcs, dx++, pCurDst++;

						sx++, pCurSrc++;
					}
				}
				else
				{
					int xe  = 0;
					int xde = SrcRect.GetWidth();
					int xcs = DstRect.GetWidth();

					int sx = SrcRect.m_Left;
					int dx = DstRect.m_Left;

					for(;;)
					{
						if(dx == DstRect.m_Right)
							break;

						DEBUG_VERIFY(sx < SrcRect.m_Right);

						if(sx >= SrcClipRect.m_Right || dx >= DstClipRect.m_Right)
							break;

						if(sx >= SrcClipRect.m_Left && dx >= DstClipRect.m_Left)
							*pCurDst = *pCurSrc;

						if((xe += xde) >= xcs)
							xe -= xcs, sx++, pCurSrc++;

						dx++, pCurDst++;
					}
				}
			}

			if((ye += yde) >= ycs)
				ye -= ycs, sy++, (const BYTE*&)pSrc += szSrcPitch;

			dy++, (BYTE*&)pDst += szDstPitch;
		}
	}
}

void StretchBlitSDL_Image(SDL_Surface* pSrcSurface, const IRECT& SrcRect, SDL_Surface* pDstSurface, const IRECT& DstRect, bool bUpdate)
{
	DEBUG_VERIFY(pSrcSurface);
	DEBUG_VERIFY(pDstSurface);
	DEBUG_VERIFY(pSrcSurface != pDstSurface);
	DEBUG_VERIFY(!pSrcSurface->locked);
	DEBUG_VERIFY(!pDstSurface->locked);
	DEBUG_VERIFY(pSrcSurface->format->BitsPerPixel == pDstSurface->format->BitsPerPixel);

	if(!SrcRect.IsValid() || !DstRect.IsValid())
		return;

	if((ISIZE)SrcRect == (ISIZE)DstRect)
	{
		BlitSDL_Image(pSrcSurface, SrcRect, pDstSurface, DstRect.GetTopLeft(), bUpdate);
		return;
	}

	{
		T_SDL_SurfaceLocker Locker0(pSrcSurface);
		T_SDL_SurfaceLocker Locker1(pDstSurface);

		if(pSrcSurface->format->BytesPerPixel == 1)
			StretchBlitSDL_ImageNoLockBPP<BYTE>(pSrcSurface, SrcRect, pDstSurface, DstRect);
		else if(pSrcSurface->format->BytesPerPixel == 2)
			StretchBlitSDL_ImageNoLockBPP<WORD>(pSrcSurface, SrcRect, pDstSurface, DstRect);
		else if(pSrcSurface->format->BytesPerPixel == 4)
			StretchBlitSDL_ImageNoLockBPP<DWORD>(pSrcSurface, SrcRect, pDstSurface, DstRect);
		else
			INITIATE_DEFINED_FAILURE("Unsupported color depth for stretched blitting of SDL image.");
	}

	if(bUpdate)
		UpdateSDL_Image(pDstSurface, DstRect);
}

template <class t>
void RotateSDL_ImageNoLockBPP(SDL_Surface* pSrcSurface, const IRECT& SrcRect, SDL_Surface* pDstSurface, const IPOINT& Coords)
{
	IRECT ClipRect = GetSDL_ClipRect(pDstSurface);	

	const t* pSrc = (const t*)((const BYTE*)pSrcSurface->pixels + SrcRect.m_Top * pSrcSurface->pitch) + (SrcRect.m_Right - 1);

	t* pDst = (t*)((BYTE*)pDstSurface->pixels + Coords.y * pDstSurface->pitch) + Coords.x;

	// ty = Coords.y + (src_w - (x - src_left) - 1)
	// tx = Coords.x + (y - src_top)

	size_t src_pitch = pSrcSurface->pitch;
	size_t dst_pitch = pDstSurface->pitch;

	int ty = Coords.y;

	for(int x = SrcRect.m_Right - 1 ; x >= SrcRect.m_Left ; x--, ty++, pSrc--, (BYTE*&)pDst += dst_pitch)
	{
		if(ty < ClipRect.m_Top)
			continue;

		if(ty >= ClipRect.m_Bottom)
			break;

		const t* pCurSrc = pSrc;

		t* pCurDst = pDst;

		int tx = Coords.x;

		for(int y = SrcRect.m_Top ; y < SrcRect.m_Bottom ; y++, tx++, (BYTE*&)pCurSrc += src_pitch, pCurDst++)
		{
			if(tx < ClipRect.m_Left)
				continue;

			if(tx >= ClipRect.m_Right)
				break;

			*pCurDst = *pCurSrc;
		}
	}
}

void RotateSDL_Image(SDL_Surface* pSrcSurface, const IRECT& SrcRect, SDL_Surface* pDstSurface, const IPOINT& Coords, bool bUpdate)
{
	DEBUG_VERIFY(pSrcSurface);
	DEBUG_VERIFY(pDstSurface);
	DEBUG_VERIFY(pSrcSurface != pDstSurface);
	DEBUG_VERIFY(!pSrcSurface->locked);
	DEBUG_VERIFY(!pDstSurface->locked);
	DEBUG_VERIFY(pSrcSurface->format->BitsPerPixel == pDstSurface->format->BitsPerPixel);
	DEBUG_VERIFY(SrcRect.m_Left >= 0 && SrcRect.m_Top >= 0 && SrcRect.m_Right <= pSrcSurface->w && SrcRect.m_Bottom <= pSrcSurface->h);

	{
		T_SDL_SurfaceLocker Locker0(pSrcSurface);
		T_SDL_SurfaceLocker Locker1(pDstSurface);

		if(pSrcSurface->format->BytesPerPixel == 1)
			RotateSDL_ImageNoLockBPP<BYTE>(pSrcSurface, SrcRect, pDstSurface, Coords);
		else if(pSrcSurface->format->BytesPerPixel == 2)
			RotateSDL_ImageNoLockBPP<WORD>(pSrcSurface, SrcRect, pDstSurface, Coords);
		else if(pSrcSurface->format->BytesPerPixel == 4)
			RotateSDL_ImageNoLockBPP<DWORD>(pSrcSurface, SrcRect, pDstSurface, Coords);
		else
			INITIATE_DEFINED_FAILURE("Unsupported SDL surface color depth for rotation.");
	}

	if(bUpdate)
		UpdateSDL_Image(pDstSurface, RectFromCS(Coords, ISIZE(SrcRect).GetTransposed()));
}

void DrawRaisedSDL_Rect(SDL_Surface* pSurface, const IRECT& Rect, const IRECT* pIntRect, UINT32 uiCenterColor, bool bUpdate)
{
	DEBUG_VERIFY(pSurface);
	DEBUG_VERIFY(!pSurface->locked);

	DEBUG_VERIFY(ISIZE(Rect) >= ISIZE(MIN_SDL_EMBOSS_SIZE, MIN_SDL_EMBOSS_SIZE));

	IRECT CRect;

	// Center
	if(uiCenterColor != UINT_MAX)
	{
		LessenRect(CRect = Rect, ISIZE(1, 1));

		if(!pIntRect || IntersectRect(CRect, *pIntRect))
			FillSDL_Image(pSurface, CRect, uiCenterColor, bUpdate);
	}

	// Top
	CRect.Set(Rect.m_Left, Rect.m_Top, Rect.m_Right - 1, Rect.m_Top + 1);

	if(!pIntRect || IntersectRect(CRect, *pIntRect))
		FillSDL_Image(pSurface, CRect, g_SDL_Consts.m_uiHeaderBG_LT_Color, bUpdate);

	// Left
	CRect.Set(Rect.m_Left, Rect.m_Top + 1, Rect.m_Left + 1, Rect.m_Bottom - 1);
	
	if(!pIntRect || IntersectRect(CRect, *pIntRect))
		FillSDL_Image(pSurface, CRect, g_SDL_Consts.m_uiHeaderBG_LT_Color, bUpdate);

	// Right
	CRect.Set(Rect.m_Right - 1, Rect.m_Top, Rect.m_Right, Rect.m_Bottom - 1);
	
	if(!pIntRect || IntersectRect(CRect, *pIntRect))
		FillSDL_Image(pSurface, CRect, g_SDL_Consts.m_uiHeaderBG_DK_Color, bUpdate);

	// Bottom
	CRect.Set(Rect.m_Left, Rect.m_Bottom - 1, Rect.m_Right, Rect.m_Bottom);

	if(!pIntRect || IntersectRect(CRect, *pIntRect))
		FillSDL_Image(pSurface, CRect, g_SDL_Consts.m_uiHeaderBG_DK_Color, bUpdate);
}

void DrawSunkenSDL_Rect(SDL_Surface* pSurface, const IRECT& Rect, const IRECT* pIntRect, UINT32 uiCenterColor, bool bUpdate)
{
	DEBUG_VERIFY(pSurface);
	DEBUG_VERIFY(!pSurface->locked);

	DEBUG_VERIFY(ISIZE(Rect) >= ISIZE(MIN_SDL_EMBOSS_SIZE, MIN_SDL_EMBOSS_SIZE));

	IRECT CRect;

	// Center
	if(uiCenterColor != UINT_MAX)
	{
		LessenRect(CRect = Rect, ISIZE(1, 1));

		if(!pIntRect || IntersectRect(CRect, *pIntRect))
			FillSDL_Image(pSurface, CRect, uiCenterColor, bUpdate);
	}

	// Top
	CRect.Set(Rect.m_Left, Rect.m_Top, Rect.m_Right - 1, Rect.m_Top + 1);

	if(!pIntRect || IntersectRect(CRect, *pIntRect))
		FillSDL_Image(pSurface, CRect, g_SDL_Consts.m_uiHeaderBG_DK_Color, bUpdate);

	// Left
	CRect.Set(Rect.m_Left, Rect.m_Top + 1, Rect.m_Left + 1, Rect.m_Bottom - 1);

	if(!pIntRect || IntersectRect(CRect, *pIntRect))
		FillSDL_Image(pSurface, CRect, g_SDL_Consts.m_uiHeaderBG_DK_Color, bUpdate);

	// Right
	CRect.Set(Rect.m_Right - 1, Rect.m_Top, Rect.m_Right, Rect.m_Bottom - 1);

	if(!pIntRect || IntersectRect(CRect, *pIntRect))
		FillSDL_Image(pSurface, CRect, g_SDL_Consts.m_uiHeaderBG_LT_Color, bUpdate);

	// Bottom
	CRect.Set(Rect.m_Left, Rect.m_Bottom - 1, Rect.m_Right, Rect.m_Bottom);

	if(!pIntRect || IntersectRect(CRect, *pIntRect))
		FillSDL_Image(pSurface, CRect, g_SDL_Consts.m_uiHeaderBG_LT_Color, bUpdate);
}

template <class t>
void GrabSDL_Surface(SDL_Surface* pSurface, TImage& RImage)
{
	RImage.Allocate(SZSIZE(pSurface->w, pSurface->h));

	T_SDL_SurfaceLocker Locker0(pSurface);

	size_t szDlt = pSurface->pitch - pSurface->w * sizeof(t);

	const t* pSrc = (const t*)pSurface->pixels;

	DWORD* pDst = RImage.GetDataPtr();

	for(size_t y = RImage.GetHeight() ; y ; y--, (BYTE*&)pSrc += szDlt)
	{
		for(size_t x = RImage.GetWidth() ; x ; x--, pSrc++, pDst++)
		{
			DWORD v = GetSDL_RGB(pSurface, *pSrc);

			*pDst = 0xFF000000 | (GetRValue(v) << 16) | (GetGValue(v) << 8) | GetBValue(v);
		}
	}
}

void GrabSDL_Surface(SDL_Surface* pSurface, TImage& RImage)
{
	DEBUG_VERIFY(pSurface);

	if(pSurface->format->BytesPerPixel == 1)
		GrabSDL_Surface<BYTE>(pSurface, RImage);
	else if(pSurface->format->BytesPerPixel == 2)
		GrabSDL_Surface<WORD>(pSurface, RImage);
	else if(pSurface->format->BytesPerPixel == 4)
		GrabSDL_Surface<DWORD>(pSurface, RImage);
	else
		INITIATE_DEFINED_FAILURE("Unsupported BPP for SDL surface grabbing.");
}

// Text
SZSIZE GetSDL_TextSize(sge_TTFont* pFont, LPCTSTR pText)
{
	DEBUG_VERIFY(pFont);
	DEBUG_VERIFY(pText);

	if(!*pText)
		return SZSIZE(0, 0);

	size_t szLineHeight = GetSDL_FontHeight(pFont);

	SDL_Rect Rect;

	if(strchr(pText, '\n')) // multi-line text
	{
		KString Text = pText;

		LPTSTR p = Text.GetDataPtr();

		SZSIZE Size(0, 0);

		for(;;)
		{
			LPCTSTR s = p;

			for( ; *p && *p != '\n' ; p++);

			if(p > s)
			{
				char oc = *p;

				*p = 0;
				Rect = sge_TTF_TextSize(pFont, (char*)s);
				*p = oc;

				UpdateMax(Size.cx, (size_t)Rect.w);
			}

			Size.cy += szLineHeight;

			if(!*p)
				break;

			p++;
		}

		return Size;
	}
	else // single-line text
	{
		Rect = sge_TTF_TextSize(pFont, (char*)pText);

		return SZSIZE(Rect.w, szLineHeight);
	}
}

IRECT GetSDL_TextRect(sge_TTFont* pFont, LPCTSTR pText, const IPOINT& Coords)
{
	DEBUG_VERIFY(pFont);
	DEBUG_VERIFY(pText);
	
	SZSIZE Size = GetSDL_TextSize(pFont, pText);
	
	IRECT Rect;
	
	Rect.m_Left		= Coords.x;
	Rect.m_Top		= Coords.y - sge_TTF_FontAscent(pFont);
	Rect.m_Right	= Rect.m_Left + Size.cx;
	Rect.m_Bottom	= Rect.m_Top  + Size.cy;	
	
	return Rect;
}

void DrawSDL_SingleLineText(SDL_Surface*	pSurface,
							sge_TTFont*		pFont,
							LPCTSTR			pText,
							const IPOINT&	Coords,
							UINT32			uiColor)
{
	DEBUG_VERIFY(pSurface);	
	DEBUG_VERIFY(!SDL_MUSTLOCK(pSurface) || !pSurface->locked);
	DEBUG_VERIFY(pFont);
	DEBUG_VERIFY(pText);
	
	if(!*pText)
		return;
	
	uiColor = GetSDL_Color(pSurface, uiColor);

	sge_tt_textout(pSurface, pFont, pText, Coords.x, Coords.y, uiColor, uiColor, SDL_ALPHA_OPAQUE);
}

void DrawClippedAlignedSDL_Text(SDL_Surface*	pSurface,
								sge_TTFont*		pFont,
								LPCTSTR			pText,
								const IRECT&	Rect,
								const ISIZE&	PreOffset,
								const ALSIZE&	Alignment,
								UINT32			uiColor,
								const ISIZE&	PostOffset)
{
	DEBUG_VERIFY(pSurface);	
	DEBUG_VERIFY(!SDL_MUSTLOCK(pSurface) || !pSurface->locked);
	DEBUG_VERIFY(pFont);
	DEBUG_VERIFY(pText);

	if(!*pText)
		return;
	
	SZSIZE TextSize = GetSDL_TextSize(pFont, pText);

	IPOINT Coords;	

	if(Alignment.cy == ALIGNMENT_MIN)
		Coords.y = Rect.m_Top + PreOffset.cy;
	else if(Alignment.cy == ALIGNMENT_MID)
		Coords.y = (Rect.m_Top + Rect.m_Bottom - TextSize.cy) >> 1;
	else if(Alignment.cy == ALIGNMENT_MAX)
		Coords.y = Rect.m_Bottom - TextSize.cy - PreOffset.cy;
	else
		DEBUG_INITIATE_FAILURE;

	Coords.y += sge_TTF_FontAscent(pFont) + PostOffset.cy;

	T_SDL_Clipper Setter0(pSurface, Rect);

	if(strchr(pText, '\n')) // multi-line text
	{
		size_t szLineHeight = GetSDL_FontHeight(pFont);

		KString Text = pText;

		LPTSTR p = Text.GetDataPtr();

		SZSIZE Size(0, 0);

		for(;;)
		{
			LPCTSTR s = p;

			for( ; *p && *p != '\n' ; p++);

			if(p > s)
			{
				char oc = *p;

				*p = 0;

				SDL_Rect SizeRect = sge_TTF_TextSize(pFont, (char*)s);

				if(Alignment.cx == ALIGNMENT_MIN)
					Coords.x = Rect.m_Left + PreOffset.cx;
				else if(Alignment.cx == ALIGNMENT_MID)
					Coords.x = (Rect.m_Left + Rect.m_Right - SizeRect.w) >> 1;
				else if(Alignment.cx == ALIGNMENT_MAX)
					Coords.x = Rect.m_Right - SizeRect.w - PreOffset.cx;
				else
					DEBUG_INITIATE_FAILURE;

				Coords.x += PostOffset.cx;

				DrawSDL_SingleLineText(pSurface, pFont, s, Coords, uiColor);

				*p = oc;
			}			

			if(!*p)
				break;

			Coords.y += szLineHeight;

			p++;
		}
	}
	else // single-line text
	{
		if(Alignment.cx == ALIGNMENT_MIN)
			Coords.x = Rect.m_Left + PreOffset.cx;
		else if(Alignment.cx == ALIGNMENT_MID)
			Coords.x = (Rect.m_Left + Rect.m_Right - TextSize.cx) >> 1;
		else if(Alignment.cx == ALIGNMENT_MAX)
			Coords.x = Rect.m_Right - TextSize.cx - PreOffset.cx;
		else
			DEBUG_INITIATE_FAILURE;

		Coords.x += PostOffset.cx;

		DrawSDL_SingleLineText(pSurface, pFont, pText, Coords, uiColor);
	}
}

// Keyboard
bool GetSDL_KeyState(size_t szKey)
{
	int n;

	const UINT8* pKeys = SDL_GetKeyState(&n);
	
	DEBUG_VERIFY((int)szKey < n);
	
	return pKeys[szKey];
}

// Timing
void T_SDL_Timers::Clear()
{
	TArray<T_SDL_Timer>::Clear();

	FOR_EACH_ARRAY(m_NextIndices, i)
	{
		if(m_NextIndices[i] != UINT_MAX)
			m_NextIndices[i] = 0;
	}
}

void T_SDL_Timers::Add(T_SDL_Control* pControl, size_t szID, size_t szPeriod)
{
	DEBUG_VERIFY(szPeriod > 0);
	DEBUG_VERIFY(szID != UINT_MAX);

	#ifdef _DEBUG
	{
		FOR_EACH_ARRAY(*this, i)
			DEBUG_VERIFY(!((*this)[i].m_pControl == pControl && (*this)[i].m_szID == szID));
	}
	#endif // _DEBUG

	TArray<T_SDL_Timer>::Add().Set(pControl, szID, szPeriod);
}

bool T_SDL_Timers::Remove(T_SDL_Control* pControl, size_t szID)
{
	DEBUG_VERIFY(szID != UINT_MAX);

	FOR_EACH_ARRAY(*this, i)
	{
		if(!((*this)[i].m_pControl == pControl && (*this)[i].m_szID == szID))
			continue;		

		FOR_EACH_ARRAY(m_NextIndices, j)
		{
			if(m_NextIndices[j] != UINT_MAX && i < m_NextIndices[j])
				m_NextIndices[j]--;
		}

		Del(i);

		return true;
	}

	return false;
}

void T_SDL_Timers::Remove(T_SDL_Control* pControl)
{
	FOR_EACH_ARRAY(*this, i)
	{
		if((*this)[i].m_pControl != pControl)
			continue;

		FOR_EACH_ARRAY(m_NextIndices, j)
		{
			if(m_NextIndices[j] != UINT_MAX && i < m_NextIndices[j])
				m_NextIndices[j]--;
		}

		Del(i--);
	}
}

size_t T_SDL_Timers::Suspend(T_SDL_Control* pControl, size_t szID)
{
	DEBUG_VERIFY(szID != UINT_MAX);

	FOR_EACH_ARRAY(*this, i)
	{
		if((*this)[i].m_pControl != pControl || (*this)[i].m_szID != szID)
			continue;

		return ++(*this)[i].m_szSuspendCount;
	}

	return UINT_MAX;
}

size_t T_SDL_Timers::Resume(T_SDL_Control* pControl, size_t szID)
{
	DEBUG_VERIFY(szID != UINT_MAX);

	FOR_EACH_ARRAY(*this, i)
	{
		if((*this)[i].m_pControl != pControl || (*this)[i].m_szID != szID)
			continue;

		DEBUG_VERIFY((*this)[i].m_szSuspendCount > 0);

		return --(*this)[i].m_szSuspendCount;
	}

	return UINT_MAX;
}

void T_SDL_Timers::Reset(T_SDL_Control* pControl, size_t szID, QWORD qwTime)
{
	FOR_EACH_ARRAY(*this, i)
	{
		if((*this)[i].m_pControl != pControl || (*this)[i].m_szID != szID)
			continue;

		(*this)[i].m_qwStart = qwTime;

		break;
	}
}

void T_SDL_Timers::ResetAll(QWORD qwTime)
{
	FOR_EACH_ARRAY(*this, i)
		(*this)[i].m_qwStart = qwTime;
}

T_SDL_TimerSuspender::T_SDL_TimerSuspender(T_SDL_Interface& Interface, size_t szID) :
	m_pInterface(&Interface), m_pControl(NULL), m_szID(szID)
{
	DEBUG_VERIFY(szID != UINT_MAX);

	if(m_pInterface->SuspendTimer(szID) == UINT_MAX)
		m_pInterface = NULL;
}

T_SDL_TimerSuspender::T_SDL_TimerSuspender(T_SDL_Control* pControl, size_t szID) :
	m_pInterface(NULL), m_pControl(pControl), m_szID(szID)
{
	DEBUG_VERIFY(pControl);
	DEBUG_VERIFY(m_szID != UINT_MAX);

	if(m_pControl->SuspendTimer(szID) == UINT_MAX)
		m_pControl = NULL;
}

T_SDL_TimerSuspender::~T_SDL_TimerSuspender()
{
	if(m_pControl)
		m_pControl->ResumeTimer(m_szID);

	if(m_pInterface)
		m_pInterface->ResumeTimer(m_szID);
}

// Events
void InvokeSDL_ControlTimer(T_SDL_Control* pControl, size_t szID)
{
	pControl->OnControlTimer(szID);
}

void InvokeSDL_ControlTimer(T_SDL_InterfaceEventSink& EventSink, size_t szID)
{
	EventSink.OnTimer(szID);
}

// Other
UINT32 ReadRGB(LPCTSTR pString)
{
	size_t r, g, b;

	if(strstr(pString, "none"))
		return UINT_MAX;

	if(	_stscanf(pString, "%u %u %u",       &r, &g, &b) != 3 &&
		_stscanf(pString, "0x%X 0x%X 0x%X", &r, &g, &b) != 3)
	{
		INITIATE_DEFINED_FAILURE((KString)"Invalid RGB color format: \"" + pString + "\".");
	}

	return RGB(r, g, b);
}

KString WriteRGB(UINT32 v, bool bHex)
{
	if(v == UINT_MAX)
		return "none";

	return KString::Formatted(bHex ? "0x%X 0x%X 0x%X" : "%u %u %u", R_RGB(v), G_RGB(v), B_RGB(v));
}
