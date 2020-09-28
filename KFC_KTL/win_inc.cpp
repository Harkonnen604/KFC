#include "kfc_ktl_pch.h"
#include "win_inc.h"

#ifndef _MSC_VER

#include "ktl_device_globals.h"
#include "date_time.h"

TCriticalSection g_InterlockedCS(false);

TConditionVariable g_SyncCV;

volatile QWORD g_qwNextTimerTime = -1;

namespace _test
{
    kfc_static_assert(sizeof(pthread_key_t) == sizeof(DWORD));
}

// Waitable timer sync object
/*
bool TWaitableTimer::Acquire()
{
    m_qwOldTimeDue = m_qwTimeDue;
    m_bOldSignaled = m_bSignaled;
    m_szOldTimesSignaled = m_szTimesSignaled;

    if(m_qwTimeDue == _UI64_MAX) // canceled timer
    {
        if(!m_bManualReset) // canceled auto-reset timer will report signlaed only once
            m_bSignaled = false;

        return m_bOldSignaled;
    }

    UINT64 qwTime = TDateTime::GetCurrentGlobalWithMSec();

    if(m_bManualReset && !m_szPeriod) // manual reset non-periodic timer
    {
        return qwTime >= m_qwTimeDue;
    }
    else // auto reset or periodic
    {
        if(qwTime < m_qwTimeDue)  // too early
            return false;

        if(m_szPeriod)
            m_qwTimeDue += m_szPeriod;
        else
            m_qwTimeDue = _UI64_MAX;

        return true;
    }
}

void TWaitableTimerSyncObject::Revert()
{
    m_qwTimeDue = m_qwOldTimeDue;

    m_bOldSignaled = m_bOldSignaled;

    m_szTimesSignaled = m_szOldTimesSignaled;
}

void TWaitableTimerSyncObject::Set(UINT64 qwTimeDue, size_t szPeriod)
{
    assert(qwTimeDue != _UI64_MAX);

    TConditionVariableLocker Locker0(g_SyncCV);

    m_qwTimeDue = qwTimeDue, m_szPeriod = szPeriod;

    m_bSignaled = false;
}

void TWaitableTimerSyncObject::Cancel()
{
    TConditionVariableLocker Locker0(g_SyncCV);

    if(m_qwTimeDue == _UI64_MAX) // already canceled
        return;

    if(TDateTime::GetCurrentGlobalWithMSec() >= m_qwTimeDue) // setting final signaled state
        m_bSignaled = true;
}*/

// Waitable timer API
/*
HANDLE CreateWaitableTimer(SECURITY_ATTRIBUTES* pSecurity, BOOL bManualReset, LPCTSTR pName)
{
    return (HANDLE)new TWaitableTimerSyncObject(bManualReset);
}

BOOL SetWaitableTimer(  HANDLE                  hTimer,
                        const LARGE_INTEGER*    pDueTime,
                        LONG                    lPeriod,
                        void*                   pCompletionRoutine,
                        void*                   pCompletionArg,
                        BOOL                    fResume)
{
    if(!hTimer)
        return FALSE;

    if(!pDueTime)
        return FALSE;

    assert(dynamic_cast<TWaitableTimerSyncObject*>((TSyncObject*)hTimer));

    QWORD qwTimeDue = (const QWORD&)*pDueTime / 10000;

    assert(qwTimeDue != -1);

    if((INT64)qwTimeDue < 0)
        qwTimeDue = TDateTime::GetCurrentGlobalWithMSec() - qwTimeDue;

    ((TWaitableTimerSyncObject*)hTimer)->Set(qwTimeDue, lPeriod);

    return TRUE;
}

BOOL CancelWaitableTimer(HANDLE hTimer)
{
    if(!hTimer)
        return FALSE;

    assert(dynamic_cast<TWaitableTimerSyncObject*>((TSyncObject*)hTimer));

    ((TWaitableTimerSyncObject*)hTimer)->Cancel();

    return TRUE;
}*/

// Waiters
void Sleep(DWORD dwTime)
{
    if(dwTime == INFINITE)
    {
        for(;;)
            Sleep(10 * 60 * 1000);
    }
    else
    {
        if(!dwTime)
            dwTime = 1;

        timeval tv = {dwTime / 1000, dwTime % 1000 * 1000};

        select(0, NULL, NULL, NULL, &tv);
    }
}

DWORD WaitForSingleObject(HANDLE hObject, DWORD dwTimeout)
{
    assert(hObject);

    if(dwTimeout == INFINITE)
    {
        if(dynamic_cast<TThreadSyncObject*>((TSyncObject*)hObject)) // joining thread
        {
            ((TThreadSyncObject*)hObject)->Join();
            return WAIT_OBJECT_0;
        }

        TConditionVariableLocker Locker0(g_SyncCV);

        for(;;)
        {
            if(((TSyncObject*)hObject)->Acquire())
                return WAIT_OBJECT_0;

            g_SyncCV.Wait();
        }
    }
    else
    {
        assert(!dynamic_cast<TThreadSyncObject*>((TSyncObject*)hObject)); // non-INFINITE waits on threads are not allowed

        QWORD qwStart = msec_time();

        TConditionVariableLocker Locker0(g_SyncCV);

        for(;;)
        {
            if(((TSyncObject*)hObject)->Acquire())
                return WAIT_OBJECT_0;

            QWORD qwTime = msec_time();

            if(qwTime - qwStart >= dwTimeout || !g_SyncCV.Wait(dwTimeout - (DWORD)(qwTime - qwStart)))
                return WAIT_TIMEOUT;
        }
    }
}

DWORD WaitForMultipleObjects(DWORD dwN, const HANDLE* pObjects, BOOL bWaitAll, DWORD dwTimeout)
{
    assert(dwN);

    #ifdef _DEBUG
    {
        for(size_t i = 0 ; i < dwN ; i++)
            assert(pObjects[i]);
    }
    #endif // _DEBUG

    if(bWaitAll)
    {
        if(dwTimeout == INFINITE)
        {
            // Joining threads (if any)
            for(size_t i = 0 ; i < dwN ; i++)
            {
                if(dynamic_cast<TThreadSyncObject*>((TSyncObject*)pObjects[i]))
                    ((TThreadSyncObject*)pObjects[i])->Join();
            }

            TConditionVariableLocker Locker0(g_SyncCV);

            for(;;)
            {
                size_t i;

                for(i = 0 ; i < dwN ; i++)
                {
                    if(!((TSyncObject*)pObjects[i])->Acquire())
                        break;
                }

                if(i == dwN)
                    return WAIT_OBJECT_0;

                for(i-- ; i != -1 ; i--)
                    ((TSyncObject*)pObjects[i])->Revert();

                g_SyncCV.Wait();
            }
        }
        else
        {
            // No threads allowed for non-INFINITE wait-all
            #ifdef _MSC_VER
            {
                for(size_t i = 0 ; i < dwN ; i++)
                    assert(!dynamic_cast<TThreadSyncObject*>((TSyncObject*)pObjects[i]));
            }
            #endif // _MSC_VER

            QWORD qwStart = msec_time();

            TConditionVariableLocker Locker0(g_SyncCV);

            for(;;)
            {
                size_t i;

                for(i = 0 ; i < dwN ; i++)
                {
                    if(!((TSyncObject*)pObjects[i])->Acquire())
                        break;
                }

                if(i == dwN)
                    return WAIT_OBJECT_0;

                for(i-- ; i != -1 ; i--)
                    ((TSyncObject*)pObjects[i])->Revert();

                QWORD qwTime = msec_time();

                if(qwTime - qwStart >= dwTimeout || !g_SyncCV.Wait(dwTimeout - (DWORD)(qwTime - qwStart)))
                    return WAIT_TIMEOUT;
            }
        }
    }
    else
    {
        // No threads allowed for wait-any
        #ifdef _MSC_VER
        {
            for(size_t i = 0 ; i < dwN ; i++)
                assert(!dynamic_cast<TThreadSyncObject*>((TSyncObject*)pObjects[i]));
        }
        #endif // _MSC_VER

        if(dwTimeout == INFINITE)
        {
            TConditionVariableLocker Locker0(g_SyncCV);

            for(;;)
            {
                for(size_t i = 0 ; i < dwN ; i++)
                {
                    if(((TSyncObject*)pObjects[i])->Acquire())
                        return WAIT_OBJECT_0 + i;
                }

                g_SyncCV.Wait();
            }
        }
        else
        {
            QWORD qwStart = msec_time();

            TConditionVariableLocker Locker0(g_SyncCV);

            for(;;)
            {
                for(size_t i = 0 ; i < dwN ; i++)
                {
                    if(((TSyncObject*)pObjects[i])->Acquire())
                        return WAIT_OBJECT_0 + i;
                }

                QWORD qwTime = msec_time();

                if(qwTime - qwStart >= dwTimeout || !g_SyncCV.Wait(dwTimeout - (DWORD)(qwTime - qwStart)))
                    return WAIT_TIMEOUT;
            }

            return WAIT_TIMEOUT;
        }
    }
}

#endif // _MSC_VER
