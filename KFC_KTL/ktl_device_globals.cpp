#include "kfc_ktl_pch.h"
#include "ktl_device_globals.h"

T_KTL_DeviceGlobals g_KTL_DeviceGlobals;

// -------------------
// KTL device globals
// -------------------
T_KTL_DeviceGlobals::T_KTL_DeviceGlobals() : TGlobals(TEXT("KTL device globals"))
{
//  m_hWaitTimesThread = NULL;
}

void T_KTL_DeviceGlobals::OnUninitialize()
{
/*  *#ifdef _DEBUG
    {
        TConditionVariableLocker Locker0(m_WaitTimesCV);

        assert(m_WaitTimes.IsEmpty());
    }
    #endif // _DEBUG

    if(m_hWaitTimesThread)
    {
        {
            TConditionVariableLocker Locker0(m_WaitTimesCV);

            m_bTerminate = true;

            m_WaitTimesCV.Signal();
        }

        WaitForSingleObject(m_hWaitTimesThread, INFINITE);

        CloseHandle(m_hWaitTimesThread), m_hWaitTimesThread = NULL;

        m_WaitTimes.Clear();
    }*/
}

void T_KTL_DeviceGlobals::OnInitialize()
{
/*  m_bTerminate = false;

    m_hWaitTimesThread = StartThread(StaticWaitTimesThreadProc, NULL);*/
}

/*
DECLARE_MEMBER_THREAD_PROC(T_KTL_DeviceGlobals, StaticWaitTimesThreadProc, pParam)
{
    ((T_KTL_DeviceGlobals*)pParam)->WaitTimesThreadProc();

    return 0;
}

void T_KTL_DeviceGlobals::WaitTimesThreadProc()
{
    TConditionVariableLocker Locker0(m_WaitTimesCV);

    while(!m_bTerminate)
    {
        TWaitTimes::TConstIterator Iter = m_WaitTimes.GetFirst();

        QWORD qwTime = msec_time();

        if(!Iter.IsValid())
        {
            m_WaitTimesCV.Wait();
            continue;
        }

        QWORD qwDelayTill = Iter->m_pTimer->UpdateTimeDue(qwTime);

        m_WaitTimes.Wait(Iter - qwTime);
    }
}
*/
