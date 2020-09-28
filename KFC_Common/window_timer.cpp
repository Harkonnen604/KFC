#include "kfc_common_pch.h"
#include "window_timer.h"

#ifdef _MSC_VER

// -------------
// Window timer
// -------------
TWindowTimer::TWindowTimer()
{
	m_bAllocated = false;
}

void TWindowTimer::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		KillTimer(m_hWnd, m_hWnd ? m_szEventID : m_szTimerID);
	}
}

void TWindowTimer::Allocate(HWND		hSWnd,
							size_t		szTimeout,
							size_t		szSEventID,
							TIMERPROC	pTimerProc)
{
	Release();

	try
	{
		m_hWnd		= hSWnd;
		m_szEventID	= szSEventID;

		m_szTimerID = SetTimer(m_hWnd, m_szEventID, (UINT)szTimeout, pTimerProc);

		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

size_t TWindowTimer::GetTimerID() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_szTimerID;
}

#endif // _MSC_VER
