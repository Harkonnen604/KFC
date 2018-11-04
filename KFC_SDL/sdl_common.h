#ifndef sdl_common_h
#define sdl_common_h

#include <KFC_Common/time_globals.h>
#include <KFC_Image/image.h>
#include "sdl_device_globals.h"
#include "sdl_tls_item.h"
#include "sdl_decl.h"
#include "sdl_consts.h"

// Limits
#define SDL_EMBOSS_WIDTH		(1)
#define MIN_SDL_EMBOSS_SIZE		(SDL_EMBOSS_WIDTH * 2)

// ---------
// SDL rect
// ---------
struct T_SDL_Rect : public SDL_Rect
{
public:
	T_SDL_Rect() {}
	
	T_SDL_Rect(const IRECT& Rect)
		{ x = Rect.m_Left, y = Rect.m_Top, w = Rect.GetWidth(), h = Rect.GetHeight(); }

	T_SDL_Rect(int sx, int sy, int sw, int sh)
		{ x = sx, y = sy, w = sw, h = sh; }

	T_SDL_Rect& Set(int sx, int sy, int sw, int sh)
		{ x = sx, y = sy, w = sw, h = sh; return *this; }

	T_SDL_Rect& operator = (const IRECT& Rect)
		{ x = Rect.m_Left, y = Rect.m_Top, w = Rect.GetWidth(), h = Rect.GetHeight(); return *this; }

	operator IRECT () const
		{ return IRECT(x, y, x+w, y+h); }
};

// --------------
// KFC SDL event
// --------------
enum T_KSDL_Event
{
	KSDLE_NONE	= 0,
	KSDLE_KNOB	= 1,
};

// ------------------
// KNOB event params
// ------------------
#define KNOB_NONE			(0)
#define KNOB_PUSH			(1)
#define KNOB_RELEASE		(2)
#define KNOB_LEFT			(3)
#define KNOB_RIGHT			(4)
#define KNOB_PUSHED_LEFT	(5)
#define KNOB_PUSHED_RIGHT	(6)

// ------------
// SDL clipper
// ------------
class T_SDL_Clipper
{
private:
	SDL_Surface* m_pSurface;

	IRECT m_OldClipRect;

private:
	T_SDL_Clipper(const T_SDL_Clipper&);

	T_SDL_Clipper& operator = (const T_SDL_Clipper&);

public:
	T_SDL_Clipper(SDL_Surface* pSurface, IRECT Rect);

	~T_SDL_Clipper();		
};

// -------------------
// SDL surface locker
// -------------------
class T_SDL_SurfaceLocker
{
private:
	SDL_Surface* m_pSurface;

public:
	T_SDL_SurfaceLocker(SDL_Surface* pSurface)
	{
		DEBUG_VERIFY(pSurface);

		if(SDL_MUSTLOCK(pSurface))
		{
			if(SDL_LockSurface(m_pSurface = pSurface))
				INITIATE_DEFINED_FAILURE("Error locking SDL surface.");
		}
		else
		{
			m_pSurface = NULL;
		}
	}

	~T_SDL_SurfaceLocker()
		{ if(m_pSurface) SDL_UnlockSurface(m_pSurface); }
};

// ----------------
// Global routines
// ----------------

// Color
inline UINT32 GetSDL_Color(SDL_Surface* pSurface, UINT32 uiRGB)
	{ DEBUG_VERIFY(pSurface); return SDL_MapRGB(pSurface->format, R_RGB(uiRGB), G_RGB(uiRGB), B_RGB(uiRGB)); }

inline UINT32 GetSDL_RGB(SDL_Surface* pSurface, UINT32 uiColor)
{
	DEBUG_VERIFY(pSurface);

	BYTE r, g, b;
	SDL_GetRGB(uiColor, pSurface->format, &r, &g, &b);

	return RGB(r, g, b);
}

// Image
inline IRECT GetSDL_ClipRect(SDL_Surface* pSurface)
	{ DEBUG_VERIFY(pSurface); T_SDL_Rect Rect; SDL_GetClipRect(pSurface, &Rect); return Rect; }

inline void SetSDL_ClipRect(SDL_Surface* pSurface, const IRECT& Rect)
{
	DEBUG_VERIFY(pSurface);

	DEBUG_VERIFY(	Rect.m_Left		>= 0 &&
					Rect.m_Right	>= 0 &&
					Rect.m_Right	<= pSurface->w &&
					Rect.m_Bottom	<= pSurface->h);

	SDL_SetClipRect(pSurface, &temp<T_SDL_Rect>(Rect)());	
}

inline void UpdateSDL_ImageNoClip(SDL_Surface* pSurface, const IRECT& Rect)
{
	DEBUG_VERIFY(pSurface);
	DEBUG_VERIFY(!pSurface->locked);

	if(pSurface != g_pSDL_FB)
		return;

	DEBUG_VERIFY(	Rect.m_Left		>= 0 &&
					Rect.m_Right	>= 0 &&
					Rect.m_Right	<= pSurface->w &&
					Rect.m_Bottom	<= pSurface->h);

	SDL_UpdateRect(pSurface, Rect.m_Left, Rect.m_Top, Rect.GetWidth(), Rect.GetHeight());
}
	
inline void UpdateSDL_Image(SDL_Surface* pSurface, IRECT Rect)
{
	DEBUG_VERIFY(pSurface);
	DEBUG_VERIFY(!pSurface->locked);
	
	if(pSurface != g_pSDL_FB)
		return;

	if(!IntersectRect(Rect, GetSDL_ClipRect(pSurface)))
		return;

	UpdateSDL_ImageNoClip(pSurface, Rect);
}

inline void UpdateSDL_ImageNoClip(SDL_Surface* pSurface)
{
	DEBUG_VERIFY(pSurface);
	DEBUG_VERIFY(!pSurface->locked);

	if(pSurface != g_pSDL_FB)
		return;

	SDL_UpdateRect(pSurface, 0, 0, 0, 0);
}

inline void UpdateSDL_Image(SDL_Surface* pSurface)
{
	DEBUG_VERIFY(pSurface);
	DEBUG_VERIFY(!pSurface->locked);

	if(pSurface != g_pSDL_FB)
		return;

	UpdateSDL_ImageNoClip(pSurface, GetSDL_ClipRect(pSurface));
}

void FillSDL_Image(SDL_Surface* pSurface, UINT32 uiColor, bool bUpdate = true);

void FillSDL_Image(SDL_Surface* pSurface, IRECT Rect, UINT uiColor, bool bUpdate = true);

void FrameSDL_Image(SDL_Surface* pSurface, const IRECT& Rect, UINT uiColor, bool bUpdate = true);

void BlendFillSDL_Image(SDL_Surface* pSurface, IRECT Rect, UINT uiColor, double dAlpha, bool bUpdate = true);

void DrawSDL_HLineNoLock(	SDL_Surface*	pSurface,
							IPOINT			Coords,
							size_t			szLength,
							UINT32			uiColor);

inline void DrawSDL_HLine(	SDL_Surface*	pSurface,
							const IPOINT&	Coords,
							size_t			szLength,
							UINT32			uiColor,
							bool			bUpdate = true)
{
	{
		T_SDL_SurfaceLocker Locker0(pSurface);

		DrawSDL_HLineNoLock(pSurface, Coords, szLength, uiColor);
	}

	if(bUpdate)
		UpdateSDL_Image(pSurface, RectFromCS(Coords, ISIZE(szLength, 1)));
}

void DrawSDL_VLineNoLock(	SDL_Surface*	pSurface,
							IPOINT			Coords,
							size_t			szLength,
							UINT32			uiColor);

inline void DrawSDL_VLine(	SDL_Surface*	pSurface,
							const IPOINT&	Coords,
							size_t			szLength,
							UINT32			uiColor,
							bool			bUpdate = true)
{
	{
		T_SDL_SurfaceLocker Locker0(pSurface);

		DrawSDL_VLineNoLock(pSurface, Coords, szLength, uiColor);
	}

	if(bUpdate)
		UpdateSDL_Image(pSurface, RectFromCS(Coords, ISIZE(1, szLength)));
}

void DrawSDL_DashedHLineNoLock(	SDL_Surface*	pSurface,
								IPOINT			Coords,
								size_t			szLength,
								size_t			szStrideLength,
								size_t			szSpaceLength,
								UINT32			uiColor,
								bool			bRightAlign = false);

inline void DrawSDL_DashedHLine(SDL_Surface*	pSurface,
								IPOINT			Coords,
								size_t			szLength,
								size_t			szStrideLength,
								size_t			szSpaceLength,
								UINT32			uiColor,
								bool			bRightAlign = false,
								bool			bUpdate     = true)
{
	{
		T_SDL_SurfaceLocker Locker0(pSurface);

		DrawSDL_DashedHLineNoLock(pSurface, Coords, szLength, szStrideLength, szSpaceLength, uiColor, bRightAlign);
	}

	if(bUpdate)
		UpdateSDL_Image(pSurface, RectFromCS(Coords, ISIZE(szLength, 1)));
}

void DrawSDL_DashedVLineNoLock(	SDL_Surface*	pSurface,
								IPOINT			Coords,
								size_t			szLength,
								size_t			szStrideLength,
								size_t			szSpaceLength,
								UINT32			uiColor,
								bool			bBottomAlign = false);

inline void DrawSDL_DashedVLine(SDL_Surface*	pSurface,
								IPOINT			Coords,
								size_t			szLength,
								size_t			szStrideLength,
								size_t			szSpaceLength,
								UINT32			uiColor,
								bool			bBottomAlign = false,
								bool			bUpdate      = true)
{
	{
		T_SDL_SurfaceLocker Locker0(pSurface);

		DrawSDL_DashedVLineNoLock(pSurface, Coords, szLength, szStrideLength, szSpaceLength, uiColor, bBottomAlign);
	}

	if(bUpdate)
		UpdateSDL_Image(pSurface, RectFromCS(Coords, ISIZE(1, szLength)));
}

void DrawSDL_LineNoLock(SDL_Surface*	pSurface,
						const IPOINT&	Coords1,
						const IPOINT&	Coords2,
						UINT32			uiColor);

inline void DrawSDL_Line(	SDL_Surface*	pSurface,
							const IPOINT&	Coords1,
							const IPOINT&	Coords2,
							UINT32			uiColor,
							bool			bUpdate = true)
{
	{
		T_SDL_SurfaceLocker Locker0(pSurface);

		DrawSDL_LineNoLock(pSurface, Coords1, Coords2, uiColor);
	}

	if(bUpdate)
	{
		UpdateSDL_Image(pSurface, IRECT(Min(Coords1.x, Coords2.x),
										Min(Coords1.y, Coords2.y),
										Max(Coords1.x, Coords2.x) + 1,
										Max(Coords1.y, Coords2.y) + 1));
	}
}

void DrawSDL_AA_LineNoLock(	SDL_Surface*	pSurface,
							const IPOINT&	Coords1,
							const IPOINT&	Coords2,
							UINT32			uiColor);

inline void DrawSDL_AA_Line(SDL_Surface*	pSurface,
							const IPOINT&	Coords1,
							const IPOINT&	Coords2,
							UINT32			uiColor,
							bool			bUpdate = true)
{
	{
		T_SDL_SurfaceLocker Locker0(pSurface);

		DrawSDL_AA_LineNoLock(pSurface, Coords1, Coords2, uiColor);
	}

	if(bUpdate)
	{
		UpdateSDL_Image(pSurface, IRECT(Min(Coords1.x, Coords2.x),
										Min(Coords1.y, Coords2.y),
										Max(Coords1.x, Coords2.x) + 1,
										Max(Coords1.y, Coords2.y) + 1));
	}
}

inline void BlitSDL_Image(SDL_Surface* pSrcSurface, const IRECT& SrcRect, SDL_Surface* pDstSurface, const IPOINT& Coords, bool bUpdate = true)
{
	DEBUG_VERIFY(pSrcSurface);
	DEBUG_VERIFY(pDstSurface);
	DEBUG_VERIFY(pSrcSurface != pDstSurface);
	DEBUG_VERIFY(!pSrcSurface->locked && !pDstSurface->locked);

	if(!SrcRect.IsValid())
		return;

	if(pSrcSurface != pDstSurface)
		DEBUG_EVERIFY(!SDL_BlitSurface(pSrcSurface, &temp<T_SDL_Rect>(SrcRect)(), pDstSurface, &temp<T_SDL_Rect>(Coords.x, Coords.y, 0, 0)()));

	if(bUpdate)
		UpdateSDL_Image(pDstSurface, RectFromCS(Coords, (ISIZE)SrcRect));
}

void StretchBlitSDL_Image(SDL_Surface* pSrcSurface, const IRECT& SrcRect, SDL_Surface* pDstSurface, const IRECT& DstRect, bool bUpdate = true);

inline void StretchBlitSDL_Image(SDL_Surface* pSrcSurface, SDL_Surface* pDstSurface, const IRECT& DstRect, bool bUpdate = true)
{
	DEBUG_VERIFY(pSrcSurface);
	DEBUG_VERIFY(pDstSurface);

	StretchBlitSDL_Image(pSrcSurface, IRECT(0, 0, pSrcSurface->w, pSrcSurface->h), pDstSurface, DstRect, bUpdate);
}

inline void BlitSDL_Image(SDL_Surface* pSrcSurface, SDL_Surface* pDstSurface, const IPOINT& Coords, bool bUpdate = true)
	{ BlitSDL_Image(pSrcSurface, IRECT(0, 0, pSrcSurface->w, pSrcSurface->h), pDstSurface, Coords, bUpdate); }

void RotateSDL_Image(SDL_Surface* pSrcSurface, const IRECT& SrcRect, SDL_Surface* pDstSurface, const IPOINT& Coords, bool bUpdate = true);

inline void RotateSDL_Image(SDL_Surface* pSrcSurface, SDL_Surface* pDstSurface, const IPOINT& Coords, bool bUpdate = true)
{
	DEBUG_VERIFY(pSrcSurface);
	DEBUG_VERIFY(pDstSurface);

	RotateSDL_Image(pSrcSurface, IRECT(0, 0, pSrcSurface->w, pSrcSurface->h), pDstSurface, Coords, bUpdate);
}

void DrawRaisedSDL_Rect(SDL_Surface*	pSurface,
						const IRECT&	Rect,
						const IRECT*	pIntRect		= NULL,
						UINT32			uiCenterColor	= g_SDL_Consts.m_uiHeaderBG_CN_Color,
						bool			bUpdate			= true);

void DrawSunkenSDL_Rect(SDL_Surface*	pSurface,
						const IRECT&	Rect,
						const IRECT*	pIntRect		= NULL,
						UINT32			uiCenterColor	= g_SDL_Consts.m_uiHeaderBG_CN_Color,
						bool			bUpdate			= true);

void GrabSDL_Surface(SDL_Surface* pSurface, TImage& RImage);

// Text
SZSIZE GetSDL_TextSize(sge_TTFont* pFont, LPCTSTR pText);

inline size_t GetSDL_FontHeight(sge_TTFont* pFont)
	{ DEBUG_VERIFY(pFont); return sge_TTF_FontHeight(pFont); }

IRECT GetSDL_TextRect(sge_TTFont* pFont, LPCTSTR pText, const IPOINT& Coords);

void DrawSDL_SingleLineText(SDL_Surface*	pSurface,
							sge_TTFont*		pFont,
							LPCTSTR			pText,
							const IPOINT&	Coords,
							UINT32			uiColor);

void DrawClippedAlignedSDL_Text(SDL_Surface*	pSurface,
								sge_TTFont*		pFont,
								LPCTSTR			pText,
								const IRECT&	Rect,
								const ISIZE&	PreOffset,
								const ALSIZE&	Alignment,
								UINT32			uiColor,
								const ISIZE&	PostOffset = ISIZE(0, 0));

// Keyboard
bool GetSDL_KeyState(size_t szKey);

inline bool IsKnobPushed()
	{ return GetSDL_KeyState(SDLK_RETURN) || GetSDL_KeyState(SDLK_KP_ENTER) || GetSDL_KeyState(SDLK_SPACE); }

// Timing
struct T_SDL_Timer
{
public:
	QWORD			m_qwStart;
	T_SDL_Control*	m_pControl;
	size_t			m_szID;
	size_t			m_szPeriod;
	size_t			m_szSuspendCount;

public:
	T_SDL_Timer()
	{
		m_szSuspendCount = 0;
	}

	T_SDL_Timer& Set(T_SDL_Control* pControl, size_t szID, size_t szPeriod)
	{
		m_qwStart	= g_TimeGlobals.GetMSEC();
		m_pControl	= pControl;
		m_szID		= szID;
		m_szPeriod	= szPeriod;

		return *this;
	}
};

class T_SDL_Timers : public TArray<T_SDL_Timer>
{
public:
	TArray<size_t, true> m_NextIndices;

public:
	T_SDL_Timers()
		{ g_SDL_DeviceGlobals.RegisterTimers(this); }

	~T_SDL_Timers()
		{ g_SDL_DeviceGlobals.UnregisterTimers(this); }

	void Clear();

	void Add(T_SDL_Control* pControl, size_t szID, size_t szPeriod);

	bool Remove(T_SDL_Control* pControl, size_t szID);

	void Remove(T_SDL_Control* pControl);

	size_t Suspend(T_SDL_Control* pControl, size_t szID);

	size_t Resume(T_SDL_Control* pControl, size_t szID);

	void Reset(T_SDL_Control* pControl, size_t szID, QWORD qwTime = g_TimeGlobals.GetMSEC());

	void ResetAll(QWORD qwTime = g_TimeGlobals.GetMSEC());
};

class T_SDL_TimersNextRegisterer
{
private:
	T_SDL_Timers& m_Timers;

	size_t m_szIndex;

public:
	T_SDL_TimersNextRegisterer(T_SDL_Timers& Timers) : m_Timers(Timers)
	{
		m_Timers.m_NextIndices.Add();

		m_szIndex = m_Timers.m_NextIndices.GetLast();

		Set(UINT_MAX);
	}

	~T_SDL_TimersNextRegisterer()
	{
		assert(m_szIndex == m_Timers.m_NextIndices.GetLast());
		m_Timers.m_NextIndices.DelNoFix(m_szIndex);
	}

	size_t Get() const
		{ return m_Timers.m_NextIndices[m_szIndex]; }

	void Set(size_t szValue)
		{ m_Timers.m_NextIndices[m_szIndex] = szValue; }
};

class T_SDL_TimerSuspender
{
private:
	T_SDL_Interface* m_pInterface;

	T_SDL_Control* m_pControl;

	size_t m_szID;

private:
	T_SDL_TimerSuspender(const T_SDL_TimerSuspender&);

	T_SDL_TimerSuspender& operator = (const T_SDL_TimerSuspender&);

public:
	T_SDL_TimerSuspender(T_SDL_Interface& Interface, size_t szID);

	T_SDL_TimerSuspender(T_SDL_Control* pControl, size_t szID);

	~T_SDL_TimerSuspender();
};

// Events
void InvokeSDL_ControlTimer(T_SDL_Control* pControl, size_t szID);

void InvokeSDL_ControlTimer(T_SDL_InterfaceEventSink& EventSink, size_t szID);

template <class t>
inline void RunSDL_MessageLoop(	t& Object,
								size_t (t::*pMethod)(T_KSDL_Event Event, size_t szParam),								
								T_SDL_Timers& Timers,
								T_SDL_InterfaceEventSink& EventSink, // used for 'OnIdle' and 'OnTimer'
								bool& bFlag)
{
	T_SDL_TimersNextRegisterer NextIndex(Timers);

	bool bBreaker = false;

	TThreadSDL_MessageLoopBreakerSetter Setter0(bBreaker);

	for(;;)
	{
		if(!bFlag || bBreaker)
			return;

		EventSink.OnIdle();

		if(!bFlag || bBreaker)
			return;

		QWORD qwTime = g_TimeGlobals.GetMSEC();

		FOR_EACH_ARRAY(Timers, i)
		{
			T_SDL_Timer& Timer = Timers[i];

			UpdateMin(Timer.m_qwStart, qwTime);

			if(Timer.m_szSuspendCount || qwTime - Timer.m_qwStart < Timer.m_szPeriod)
				continue;

			Timer.m_qwStart += (qwTime - Timer.m_qwStart) / Timer.m_szPeriod * Timer.m_szPeriod;

			{
				NextIndex.Set(i + 1);

				if(Timer.m_pControl)
					InvokeSDL_ControlTimer(Timer.m_pControl, Timer.m_szID);
				else
					InvokeSDL_ControlTimer(EventSink, Timer.m_szID);

				i = NextIndex.Get() - 1, NextIndex.Set(UINT_MAX);
			}

			if(!bFlag || bBreaker)
				return;
		}

		SDL_Event Event;

		if(SDL_PollEvent(&Event) != 1)
		{
			Sleep(20);
			continue;
		}

		if(Event.type == SDL_QUIT)
			TERMINATE;

		if(	Event.type == SDL_KEYDOWN &&
			(	Event.key.keysym.sym == SDLK_RETURN   && !GetSDL_KeyState(SDLK_KP_ENTER) && !GetSDL_KeyState(SDLK_SPACE) ||
				Event.key.keysym.sym == SDLK_KP_ENTER && !GetSDL_KeyState(SDLK_RETURN)   && !GetSDL_KeyState(SDLK_SPACE) ||
				Event.key.keysym.sym == SDLK_SPACE    && !GetSDL_KeyState(SDLK_RETURN)   && !GetSDL_KeyState(SDLK_KP_ENTER)))
		{
			(Object.*pMethod)(KSDLE_KNOB, KNOB_PUSH);
		}
		else if(Event.type == SDL_KEYUP &&
				(	Event.key.keysym.sym == SDLK_RETURN   && !GetSDL_KeyState(SDLK_KP_ENTER) && !GetSDL_KeyState(SDLK_SPACE) ||
					Event.key.keysym.sym == SDLK_KP_ENTER && !GetSDL_KeyState(SDLK_RETURN)   && !GetSDL_KeyState(SDLK_SPACE) ||
					Event.key.keysym.sym == SDLK_SPACE    && !GetSDL_KeyState(SDLK_RETURN)   && !GetSDL_KeyState(SDLK_KP_ENTER)))
		{
			(Object.*pMethod)(KSDLE_KNOB, KNOB_RELEASE);
		}
		else if(Event.type == SDL_KEYDOWN &&
				(	Event.key.keysym.sym == SDLK_LEFT || Event.key.keysym.sym == SDLK_KP4 ||
					Event.key.keysym.sym == SDLK_UP   || Event.key.keysym.sym == SDLK_KP8))
		{
			(Object.*pMethod)(KSDLE_KNOB, IsKnobPushed() ? KNOB_PUSHED_LEFT : KNOB_LEFT);
		}
		else if(Event.type == SDL_KEYDOWN &&
				(	Event.key.keysym.sym == SDLK_RIGHT || Event.key.keysym.sym == SDLK_KP6 ||
					Event.key.keysym.sym == SDLK_DOWN  || Event.key.keysym.sym == SDLK_KP2))
		{
			(Object.*pMethod)(KSDLE_KNOB, IsKnobPushed() ? KNOB_PUSHED_RIGHT : KNOB_RIGHT);
		}
	}
}

inline void BreakCurSDL_MessageLoop()
{
	T_SDL_TLS_Item& Item = T_SDL_TLS_Item::Get();

	DEBUG_VERIFY(Item.m_pCurSDL_MessageLoopBreaker);

	*Item.m_pCurSDL_MessageLoopBreaker = true;
}

// Other
UINT32 ReadRGB(LPCTSTR pString);

KString WriteRGB(UINT32 uiColor, bool bHex = false);

#endif // sdl_common_h
