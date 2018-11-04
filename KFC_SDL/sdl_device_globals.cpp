#include "kfc_sdl_pch.h"
#include "sdl_device_globals.h"

#include "sdl_tls_item.h"
#include "sdl_consts.h"
#include "sdl_common.h"
#include "basic_controls.h"

T_SDL_DeviceGlobals g_SDL_DeviceGlobals;

// -------------------
// SDL device globals
// -------------------
T_SDL_DeviceGlobals::T_SDL_DeviceGlobals() : TGlobals(TEXT("SDL globals"))
{
	m_bSDL_Initialized = false;
	
	m_bSGE_TTF_Initialized = false;
}

void T_SDL_DeviceGlobals::OnUninitialize()
{
	UnregisterControlLoader("ProgressBar");
	UnregisterControlLoader("List");
	UnregisterControlLoader("Image");
	UnregisterControlLoader("Label");
	UnregisterControlLoader("Button");

	m_pFrameBuffer = NULL;	

	if(m_bSDL_Initialized)
		SDL_Quit(), m_bSDL_Initialized = false;

	T_SDL_TLS_Item::Free();
}

void T_SDL_DeviceGlobals::OnInitialize()
{
	T_SDL_TLS_Item::Register();

	// SDL
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER))
		INITIATE_DEFINED_FAILURE(TEXT("Error initializing SDL."));
		
	m_bSDL_Initialized = true;

	if(!(m_pFrameBuffer = SDL_SetVideoMode(	g_SDL_Consts.m_Resolution.cx,
											g_SDL_Consts.m_Resolution.cy,
											g_SDL_Consts.m_szBPP,
											SDL_HWSURFACE | SDL_ANYFORMAT)))
	{
		INITIATE_DEFINED_FAILURE(TEXT("Error setting SDL video mode."));
	}
	
	DEBUG_VERIFY(!m_bSGE_TTF_Initialized);

	// SGE TTF
	if(sge_TTF_Init())
		INITIATE_DEFINED_FAILURE(TEXT("Error initializing SGE TTF."));
		
	m_bSGE_TTF_Initialized = true;

	// Other
	SDL_FillRect(m_pFrameBuffer, NULL, 0), SDL_UpdateRect(m_pFrameBuffer, 0, 0, 0, 0);

	if(g_SDL_Consts.m_bSGE_TTF_AlphaAA)
		sge_TTF_AAOn(), sge_TTF_AA_Alpha();

 	SDL_EnableKeyRepeat(0, 0);

	// Control loaders
	RegisterControlLoader("Button",			ButtonSDL_ControlLoader);
	RegisterControlLoader("Label",			LabelSDL_ControlLoader);
	RegisterControlLoader("Image",			ImageSDL_ControlLoader);
	RegisterControlLoader("ProgressBar",	ProgressBarSDL_ControlLoader);
	RegisterControlLoader("List",			ListSDL_ControlLoader);
}

void T_SDL_DeviceGlobals::RegisterControlLoader(LPCTSTR pHeading, T_SDL_ControlLoader* pLoader)
{
	if(m_ControlLoaders.Has(pHeading))
		INITIATE_DEFINED_FAILURE((KString)"Attempt to register SDL control loader \"" + pHeading + "\" more than once.");

	*m_ControlLoaders.Add(pHeading) = pLoader;
}

void T_SDL_DeviceGlobals::UnregisterControlLoader(LPCTSTR pHeading)
{
	if(m_ControlLoaders.Has(pHeading))
		m_ControlLoaders.Del(pHeading);
}

T_SDL_ControlLoader* T_SDL_DeviceGlobals::GetControlLoader(LPCTSTR pHeading) const
{
	DEBUG_VERIFY_INITIALIZATION;

	TControlLoaders::TConstIterator Iter = m_ControlLoaders.Find(pHeading);
	
	if(!Iter.IsValid())
		INITIATE_DEFINED_FAILURE((KString)"No SDL control loader registered for \"" + pHeading + "\".");

	return *Iter;
}

void T_SDL_DeviceGlobals::RegisterTimers(T_SDL_Timers* pTimers)
{
	DEBUG_VERIFY_INITIALIZATION;

	DEBUG_VERIFY(pTimers);

	TCriticalSectionLocker Locker0(m_TimersCS);

	m_Timers.Add() = pTimers;
}

void T_SDL_DeviceGlobals::UnregisterTimers(T_SDL_Timers* pTimers)
{
	if(!IsInitialized())
		return;

	DEBUG_VERIFY(pTimers);

	TCriticalSectionLocker Locekr0(m_TimersCS);

	FOR_EACH_ARRAY_REV(m_Timers, i)
	{
		if(m_Timers[i] == pTimers)
		{
			m_Timers.DelNoFix(i);
			return;
		}
	}
}

void T_SDL_DeviceGlobals::ResetAllTimers(QWORD qwTime)
{
	DEBUG_VERIFY_INITIALIZATION;

	TCriticalSectionLocker Locker0(m_TimersCS);

	FOR_EACH_ARRAY(m_Timers, i)
		m_Timers[i]->ResetAll(qwTime);
}
