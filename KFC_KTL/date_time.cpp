#include "kfc_ktl_pch.h"
#include "date_time.h"

// ----------
// Date time
// ----------

#ifndef _MSC_VER
    const QWORD TDateTime::ms_qwEpochOffset     = (QWORD)TDateTime(1970, 1, 1, 0, 0, 0);
    const QWORD TDateTime::ms_qwFILETIME_Offset = (QWORD)TDateTime(1601, 1, 1, 0, 0, 0);
#endif // _MSC_VER

const size_t TDateTime::ms_MonthDays[2][12] = { {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
                                                {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

const size_t TDateTime::ms_TotalMonthDays[2][12] = {{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
                                                    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}};

bool TDateTime::IsValid() const
{
    return  m_szYear    >= 1 &&
            m_szMonth   >= 1 && m_szMonth   <= 12 &&
            m_szDay     >= 1 && m_szDay     <= GetMonthDays() &&
            m_szHour    >= 0 && m_szHour    <= 23 &&
            m_szMin     >= 0 && m_szMin     <= 59 &&
            m_szSec     >= 0 && m_szSec     <= 59;
}

TDateTime& TDateTime::GetCurrentLocal(size_t& szRMSec)
{
    #ifdef _MSC_VER
    {
        SYSTEMTIME Time;
        GetLocalTime(&Time);

        szRMSec = Time.wMilliseconds;

        return *this = Time;
    }
    #else // _MSC_VER
    {
        timeval tv;
        gettimeofday(&tv, NULL);

        szRMSec = tv.tv_usec / 1000;

        return *this = (QWORD)tv.tv_sec + ms_qwEpochOffset;
    }
    #endif // _MSC_VER
}

TDateTime& TDateTime::GetCurrentGlobal(size_t& szRMSec)
{
    #ifdef _MSC_VER
    {
        SYSTEMTIME Time;
        GetSystemTime(&Time);

        szRMSec = Time.wMilliseconds;

        return *this = Time;
    }
    #else // _MSC_VER
    {
        timeval tv;
        gettimeofday(&tv, NULL);

        szRMSec = tv.tv_usec / 1000;

        return *this = (QWORD)tv.tv_sec + ms_qwEpochOffset;
    }
    #endif // _MSC_VER
}

void TDateTime::SetAsCurrentLocal(size_t szMSec)
{
    #ifdef _MSC_VER
    {
        DEBUG_VERIFY(IsValid());
        DEBUG_VERIFY(szMSec < 1000);

        SYSTEMTIME st = GetSYSTEMTIME(szMSec);

        if(!SetLocalTime(&st))
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error setting local date/time."), GetLastError());
    }
    #else // _MSC_VER
    {
        // {{{
    }
    #endif // _MSC_VER
}

void TDateTime::SetAsCurrentGlobal(size_t szMSec)
{
    #ifdef _MSC_VER
    {
        DEBUG_VERIFY(IsValid());
        DEBUG_VERIFY(szMSec < 1000);

        SYSTEMTIME st = GetSYSTEMTIME(szMSec);

        if(!SetSystemTime(&st))
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error setting system date/time."), GetLastError());
    }
    #else // _MSC_VER
    {
        // {{{
    }
    #endif // _MSC_VER
}

TDateTime& TDateTime::LocalToGlobal()
{
    #ifdef _MSC_VER
    {
        DEBUG_VERIFY(IsValid());

        SYSTEMTIME  STime;
        FILETIME    FTime1, FTime2;

        STime = *this;

        DEBUG_EVERIFY(SystemTimeToFileTime      (&STime,  &FTime1));
        DEBUG_EVERIFY(LocalFileTimeToFileTime   (&FTime1, &FTime2));
        DEBUG_EVERIFY(FileTimeToSystemTime      (&FTime2, &STime));

        *this = STime;

        return *this;
    }
    #else // _MSC_VER
    {
        return *this;
    }
    #endif // _MSC_VER
}

TDateTime& TDateTime::GlobalToLocal()
{
    #ifdef _MSC_VER
    {
        DEBUG_VERIFY(IsValid());

        SYSTEMTIME  STime;
        FILETIME    FTime1, FTime2;

        STime = *this;

        DEBUG_EVERIFY(SystemTimeToFileTime      (&STime,  &FTime1));
        DEBUG_EVERIFY(FileTimeToLocalFileTime   (&FTime1, &FTime2));
        DEBUG_EVERIFY(FileTimeToSystemTime      (&FTime2, &STime));

        *this = STime;

        return *this;
    }
    #else // _MSC_VER
    {
        return *this;
    }
    #endif // _MSC_VER
}

#ifdef _MSC_VER

double TDateTime::GetCOM_DT(size_t szMSec) const
{
    if(!IsValid())
        return -1.0;

    double dDT;

    WORD wDate, wTime;

    DEBUG_EVERIFY(FileTimeToDosDateTime(&GetFILETIME(szMSec), &wDate, &wTime));
    DEBUG_EVERIFY(DosDateTimeToVariantTime(wDate, wTime, &dDT));

    return dDT;
}

TDateTime& TDateTime::SetCOM_DT(double dDT)
{
    if(Compare(dDT, 0.0) < 0)
        return Invalidate();

    FILETIME ftDT;

    WORD wDate, wTime;

    if( !VariantTimeToDosDateTime(dDT, &wDate, &wTime) ||
        !DosDateTimeToFileTime(wDate, wTime, &ftDT))
    {
        return Invalidate();
    }

    return *this = ftDT;
}

#endif // _MSC_VER

// Decreasing
TDateTime& TDateTime::DecSec(size_t szAmt)
{
    if(szAmt >= 60)
        DecMin(szAmt / 60), szAmt %= 60;

    for( ; szAmt ; szAmt--)
    {
        if(--m_szSec == -1)
            DecMin(), m_szSec = 59;
    }

    return *this;
}

TDateTime& TDateTime::DecMin(size_t szAmt)
{
    if(szAmt >= 60)
        DecHour(szAmt / 60), szAmt %= 60;

    for( ; szAmt ; szAmt--)
    {
        if(--m_szMin == -1)
            DecHour(), m_szMin = 59;
    }

    return *this;
}

TDateTime& TDateTime::DecHour(size_t szAmt)
{
    if(szAmt >= 24)
        DecDay(szAmt / 24), szAmt %= 24;

    for( ; szAmt ; szAmt--)
    {
        if(--m_szHour == -1)
            DecDay(), m_szHour = 23;
    }

    return *this;
}

TDateTime& TDateTime::DecDay(size_t szAmt)
{
    for( ; szAmt ; szAmt--)
    {
        if(--m_szDay == 0)
            DecMonth(), m_szDay = GetMonthDays();
    }

    return *this;
}

TDateTime& TDateTime::DecMonth(size_t szAmt)
{
    for( ; szAmt ; szAmt--)
    {
        if(--m_szMonth == 0)
            DecYear(), m_szMonth = 12;
    }

    return *this;
}

TDateTime& TDateTime::DecYear(size_t szAmt)
{
    m_szYear -= szAmt;

    return *this;
}

// Increasing
TDateTime& TDateTime::IncSec(size_t szAmt)
{
    if(szAmt >= 60)
        IncMin(szAmt / 60), szAmt %= 60;

    for( ; szAmt ; szAmt--)
    {
        if(++m_szSec == 60)
            IncMin(), m_szSec = 0;
    }

    return *this;
}

TDateTime& TDateTime::IncMin(size_t szAmt)
{
    if(szAmt >= 60)
        IncHour(szAmt / 60), szAmt %= 60;

    for( ; szAmt ; szAmt--)
    {
        if(++m_szMin == 60)
            IncHour(), m_szMin = 0;
    }

    return *this;
}

TDateTime& TDateTime::IncHour(size_t szAmt)
{
    if(szAmt >= 24)
        IncDay(szAmt / 24), szAmt %= 24;

    for( ; szAmt ; szAmt--)
    {
        if(++m_szHour == 24)
            IncDay(), m_szHour = 0;
    }

    return *this;
}

TDateTime& TDateTime::IncDay(size_t szAmt)
{
    for( ; szAmt ; szAmt--)
    {
        if(++m_szDay == GetMonthDays() + 1)
            IncMonth(), m_szDay = 1;
    }

    return *this;
}

TDateTime& TDateTime::IncMonth(size_t szAmt)
{
    for( ; szAmt ; szAmt--)
    {
        if(++m_szMonth == 13)
            IncYear(), m_szMonth = 1;
    }

    return *this;
}

TDateTime& TDateTime::IncYear(size_t szAmt)
{
    m_szYear += szAmt;

    return *this;
}

// Aligning
TDateTime& TDateTime::AlignToSec()
{
    return *this;
}

TDateTime& TDateTime::AlignToMin()
{
    m_szSec = 0, AlignToSec();

    return *this;
}

TDateTime& TDateTime::AlignToHour()
{
    m_szMin = 0, AlignToMin();

    return *this;
}

TDateTime& TDateTime::AlignToDay()
{
    m_szHour = 0, AlignToHour();

    return *this;
}

TDateTime& TDateTime::AlignToMonth()
{
    m_szDay = 1, AlignToDay();

    return *this;
}

TDateTime& TDateTime::AlignToYear()
{
    m_szMonth = 1, AlignToMonth();

    return *this;
}

bool TDateTime::IsSecAligned() const
{
    return true;
}

bool TDateTime::IsMinAligned() const
{
    return m_szSec == 0 && IsSecAligned();
}

bool TDateTime::IsHourAligned() const
{
    return m_szMin == 0 && IsMinAligned();
}

bool TDateTime::IsDayAligned() const
{
    return m_szHour == 0 && IsHourAligned();
}

bool TDateTime::IsMonthAligned() const
{
    return m_szDay == 1 && IsDayAligned();
}

bool TDateTime::IsYearAligned() const
{
    return m_szMonth == 1 && IsMonthAligned();
}

// Operators
TDateTime& TDateTime::operator = (QWORD v)
{
    if(v == -1)
        return Invalidate(), *this;

    m_szSec     = (size_t)(v % 60), v /= 60;
    m_szMin     = (size_t)(v % 60), v /= 60;
    m_szHour    = (size_t)(v % 24), v /= 24;

    size_t vv = (size_t)v;

    const size_t days400 = (365 * 400 + 97);
    const size_t days100 = (365 * 100 + 24);
    const size_t days4   = (365 * 4   + 1);
    const size_t days1   = (365);

    m_szYear = 1;

    size_t dlt;

    dlt = vv / days400, vv -= days400 * dlt;
    m_szYear += dlt * 400u;

    dlt = Min((unsigned)(vv / days100), 3u), vv -= days100 * dlt;
    m_szYear += dlt * 100u;

    dlt = Min((unsigned)(vv / days4), 24u), vv -= days4 * dlt;
    m_szYear += dlt * 4u;

    dlt = Min((unsigned)(vv / days1), 3u), vv -= days1 * dlt;
    m_szYear += dlt;

    const size_t* pMonthDays = ms_MonthDays[IsLeapYear()];

    for(m_szMonth = 1 ; vv >= *pMonthDays ; m_szMonth++, vv -= *pMonthDays++)
        DEBUG_VERIFY(m_szMonth <= 12);

    m_szDay = vv + 1;

    DEBUG_VERIFY(IsValid());

    return *this;
}

TDateTime& TDateTime::operator = (const FILETIME& ft)
{
    #ifdef _MSC_VER
    {
        SYSTEMTIME st;
        DEBUG_EVALUATE_VERIFY(FileTimeToSystemTime(&ft, &st));

        return *this = st;
    }
    #else // _MSC_VER
    {
        if(ft.dwLowDateTime == DWORD_MAX && ft.dwHighDateTime == DWORD_MAX)
            return Invalidate();

        return *this = (QWORD&)ft / 10000000 + ms_qwFILETIME_Offset;
    }
    #endif // _MSC_VER
}

TDateTime& TDateTime::operator = (const SYSTEMTIME& st)
{
    m_szYear    = st.wYear;
    m_szMonth   = st.wMonth;
    m_szDay     = st.wDay;

    m_szHour    = st.wHour;
    m_szMin     = st.wMinute;
    m_szSec     = st.wSecond;

    if(!IsValid())
        Invalidate();

    return *this;
}

TDateTime::operator QWORD () const
{
    if(!IsValid())
        return -1;

    return ((((QWORD)(m_szYear - 1) * 365 + GetNumLeapYearsBefore() +
                GetTotalMonthDaysBefore() +
                    (m_szDay - 1)) * 24 +
                        m_szHour) * 60 +
                            m_szMin) * 60 +
                                m_szSec;
}

FILETIME TDateTime::GetFILETIME(size_t szMSec) const
{
    DEBUG_VERIFY(szMSec < 1000);

    #ifdef _MSC_VER
    {
        SYSTEMTIME st = GetSYSTEMTIME(szMSec);

        FILETIME ft;

        DEBUG_EVALUATE_VERIFY(SystemTimeToFileTime(&st, &ft));

        return ft;
    }
    #else // _MSC_VER
    {
        if(!IsValid())
            return (const FILETIME&)(const QWORD&)temp<QWORD>(QWORD_MAX);

        QWORD qwTime = (QWORD)*this;

        if(qwTime < ms_qwFILETIME_Offset)
            return (const FILETIME&)(const QWORD&)temp<QWORD>(QWORD_MAX);

        return (const FILETIME&)(const QWORD&)temp<QWORD>(((qwTime - ms_qwFILETIME_Offset) * 1000 + szMSec) * 10000);
    }
    #endif // _MSC_VER
}

SYSTEMTIME TDateTime::GetSYSTEMTIME(size_t szMSec) const
{
    DEBUG_VERIFY(szMSec < 1000);

    SYSTEMTIME st;

    st.wYear    = (WORD)m_szYear;
    st.wMonth   = (WORD)m_szMonth;
    st.wDay     = (WORD)m_szDay;

  st.wHour = (WORD)m_szHour;
    st.wMinute  = (WORD)m_szMin;
    st.wSecond  = (WORD)m_szSec;

    st.wMilliseconds    = (WORD)szMSec;
    st.wDayOfWeek       = (WORD)GetDayOfWeek();

    return st;
}

// ----------------
// Global routines
// ----------------
bool FromString(KString String, TDateTime& RDateTime)
{
    for(size_t i = 0 ; i < String.GetLength() ; i++)
    {
        if(!_istdigit(String[i]))
            String.SetChar(i, TEXT(' '));
    }

    if(_stscanf(String,
                TEXT("%zu %zu %zu %zu %zu %zu"),
                    &RDateTime.m_szYear,
                    &RDateTime.m_szMonth,
                    &RDateTime.m_szDay,
                    &RDateTime.m_szHour,
                    &RDateTime.m_szMin,
                    &RDateTime.m_szSec) != 6)
    {
        RDateTime.Invalidate();
        return false;
    }

    return RDateTime.IsValid();
}
