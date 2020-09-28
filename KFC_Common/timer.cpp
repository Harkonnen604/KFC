#include "kfc_common_pch.h"
#include "timer.h"

#include "time_globals.h"

// ------
// Timer
// ------
TTimer::TTimer(bool bAllocate, QWORD qwSStartTime)
{
    m_bAllocated = false;

    if(bAllocate)
        Allocate(qwSStartTime);
}

void TTimer::Release()
{
    m_bAllocated = false;
}

void TTimer::Allocate(QWORD qwSStartTime)
{
    Release();

    try
    {
        m_fSpeedCoef = 1.0f;

        ResetSuspendCount();

        m_bAllocated = true;

        Reset(qwSStartTime);
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void TTimer::Reset(QWORD qwSStartTime)
{
    DEBUG_VERIFY_ALLOCATION;

    if(qwSStartTime == -1)
        qwSStartTime = g_TimeGlobals.GetMSEC();

    m_qwLastStartTime = qwSStartTime;

    m_qwAccumulatedTime = 0;
}

void TTimer::SetSpeedCoef(float fSSpeedCoef, QWORD qwCurTime)
{
    DEBUG_VERIFY_ALLOCATION;

    if(qwCurTime == -1)
        qwCurTime = g_TimeGlobals.GetMSEC();

    m_qwAccumulatedTime = GetElapsedTime(qwCurTime), m_qwLastStartTime = qwCurTime;

    m_fSpeedCoef = fSSpeedCoef;
}

bool TTimer::OnSuspend()
{
    if(!IsAllocated())
        return false;

    if(!TSuspendable::OnSuspend())
        return false;

    m_qwAccumulatedTime = GetElapsedTime(g_TimeGlobals.GetMSEC());

    return true;
}

bool TTimer::OnResume()
{
    if(!IsAllocated())
        return false;

    if(!TSuspendable::OnResume())
        return false;

    m_qwLastStartTime = g_TimeGlobals.GetMSEC();

    return true;
}

QWORD TTimer::GetElapsedTime(QWORD qwCurTime) const
{
    DEBUG_VERIFY_ALLOCATION;

    if(qwCurTime == -1)
        qwCurTime = g_TimeGlobals.GetMSEC();

    QWORD qwElapsedTime = m_qwAccumulatedTime;

    if(!IsSuspended())
        qwElapsedTime += (QWORD)((INT64)(qwCurTime - m_qwLastStartTime) * m_fSpeedCoef);

    return qwElapsedTime;
}
