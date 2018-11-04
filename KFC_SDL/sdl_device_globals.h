#ifndef sdl_device_globals_h
#define sdl_device_globals_h

#include <KFC_KTL/critical_section.h>
#include <KFC_KTL/globals.h>
#include <KFC_KTL/tokens.h>
#include <KFC_Common/time_globals.h>
#include <KFC_Common/structured_info.h>
#include "sdl_control.h"
#include "sdl_decl.h"

// Speed defs
#define g_pSDL_FB	(g_SDL_DeviceGlobals.GetFrameBuffer())

// -------------------
// SDL device globals
// -------------------
class T_SDL_DeviceGlobals : public TGlobals
{
private:
	// Control loaders
	typedef TTire<T_SDL_ControlLoader*> TControlLoaders;

private:
	SDL_Surface* m_pFrameBuffer;
	
	bool m_bSDL_Initialized;
	
	bool m_bSGE_TTF_Initialized;

	TControlLoaders m_ControlLoaders;

	TArray<T_SDL_Timers*, true> m_Timers;

	TCriticalSection m_TimersCS;

private:
	void OnUninitialize	();
	void OnInitialize	();
	
public:
	T_SDL_DeviceGlobals();

	SDL_Surface* GetFrameBuffer() const
		{ DEBUG_VERIFY_INITIALIZATION; return m_pFrameBuffer; }

	void RegisterControlLoader(LPCTSTR pHeading, T_SDL_ControlLoader* pLoader);

	void UnregisterControlLoader(LPCTSTR pHeading);

	T_SDL_ControlLoader* GetControlLoader(LPCTSTR pHeading) const;

	void RegisterTimers(T_SDL_Timers* pTimers);

	void UnregisterTimers(T_SDL_Timers* pTimers);

	void ResetAllTimers(QWORD qwTime = g_TimeGlobals.GetMSEC());
};

extern T_SDL_DeviceGlobals g_SDL_DeviceGlobals;

#endif // sdl_device_globals_h
