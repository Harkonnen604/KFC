#include "kfc_ktl_pch.h"
#include "waitable_timer.h"

#ifdef _MSC_VER

// ---------------
// Waitable timer
// ---------------
TWaitableTimer::TWaitableTimer()
{
    m_hTimer = NULL;
}

TWaitableTimer::TWaitableTimer(bool bManualReset, LPCTSTR pName)
{
    m_hTimer = NULL;

    Create(bManualReset, pName);
}

#ifdef _MSC_VER

TWaitableTimer::TWaitableTimer(LPCTSTR pName)
{
    m_hTimer = NULL;

    Open(pName);
}

#endif // _MSC_VER

void TWaitableTimer::Release()
{
    if(m_hTimer)
        CloseHandle(m_hTimer), m_hTimer = NULL;
}

bool TWaitableTimer::Create(bool bManualReset, LPCTSTR pName)
{
    Release();

    try
    {
        if(!(m_hTimer = CreateWaitableTimer(NULL, bManualReset, pName)))
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error creating waitable timer"), GetLastError());

        return GetLastError() != ERROR_ALREADY_EXISTS;
    }

    catch(...)
    {
        Release();
        throw;
    }
}

#ifdef _MSC_VER

void TWaitableTimer::Open(LPCTSTR pName)
{
    Release();

    try
    {
        DEBUG_VERIFY(pName);

        if(!(m_hTimer = OpenWaitableTimer(TIMER_ALL_ACCESS, FALSE, pName)))
        {
            INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error opening waitable timer \"") + pName + TEXT("\"."),
                                            GetLastError());
        }
    }

    catch(...)
    {
        Release();
        throw;
    }
}

#endif // _MSC_VER

void TWaitableTimer::SetRelative(size_t szDelay, size_t szPeriod)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY((int)szPeriod >= 0);

    LARGE_INTEGER tm;

    tm.QuadPart = (INT64)-10000 * szDelay;

    if(!SetWaitableTimer(m_hTimer, &tm, (DWORD)szPeriod, NULL, NULL, FALSE))
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error setting relative waitable timer"), GetLastError());
}

void TWaitableTimer::SetAbsolute(const FILETIME& GlobalTime, size_t szPeriod)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY((int)szPeriod >= 0);

    DEBUG_VERIFY((int)GlobalTime.dwHighDateTime >= 0);

    LARGE_INTEGER tm;

    tm.LowPart  = GlobalTime.dwLowDateTime;
    tm.HighPart = GlobalTime.dwHighDateTime;

    if(!SetWaitableTimer(m_hTimer, &tm, (DWORD)szPeriod, NULL, NULL, FALSE))
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error setting absolute waitable timer"), GetLastError());
}

void TWaitableTimer::Cancel(bool bSafe)
{
    DEBUG_VERIFY_ALLOCATION;

    if(!CancelWaitableTimer(m_hTimer))
    {
        if(!bSafe)
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error canceling waitable timer"), GetLastError());
    }
}

#endif // _MSC_VER
