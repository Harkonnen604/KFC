#ifndef date_time_h
#define date_time_h

#include "kstring.h"

// Second casters
#define MIN_SECS    (60)
#define HOUR_SECS   (60 * MIN_SECS)
#define DAY_SECS    (24 * HOUR_SECS)

// ----------
// Date time
// ----------
struct TDateTime
{
public:
    static const size_t ms_MonthDays        [2][12];
    static const size_t ms_TotalMonthDays   [2][12];

    #ifndef _MSC_VER
        static const QWORD ms_qwEpochOffset;
        static const QWORD ms_qwFILETIME_Offset;
    #endif // _MSC_VER

public:
    size_t m_szYear;
    size_t m_szMonth;
    size_t m_szDay;

    size_t m_szHour;
    size_t m_szMin;
    size_t m_szSec;

public:
    TDateTime()
        { Invalidate(); }

    TDateTime(  size_t szSYear,
                size_t szSMonth,
                size_t szSDay,
                size_t szSHour  = 0,
                size_t szSMin   = 0,
                size_t szSSec   = 0)
    {
        Set(szSYear, szSMonth, szSDay, szSHour, szSMin, szSSec);
    }

    TDateTime(QWORD v)
        { *this = v; }

    TDateTime(FILETIME ft)
        { *this = ft; }

    TDateTime(SYSTEMTIME st)
        { *this = st; }

    TDateTime& Set( size_t szSYear,
                    size_t szSMonth,
                    size_t szSDay,
                    size_t szSHour  = 0,
                    size_t szSMin   = 0,
                    size_t szSSec   = 0)
    {
        m_szYear    = szSYear;
        m_szMonth   = szSMonth;
        m_szDay     = szSDay;
        m_szHour    = szSHour;
        m_szMin     = szSMin;
        m_szSec     = szSSec;

        return *this;
    }

    bool IsValid() const;

    TDateTime& Invalidate()
        { memset(this, 0, sizeof(*this)); return *this; }

    TDateTime& GetCurrentLocal  (size_t& szRMSec = temp<size_t>());
    TDateTime& GetCurrentGlobal (size_t& szRMSec = temp<size_t>());

    static QWORD GetCurrentLocalWithMSec()
    {
        size_t szMSec;

        QWORD qwDT = TDateTime().GetCurrentLocal(szMSec);

        return qwDT * 1000 + szMSec;
    }

    static QWORD GetCurrentGlobalWithMSec()
    {
        size_t szMSec;

        QWORD qwDT = TDateTime().GetCurrentGlobal(szMSec);

        return qwDT * 1000 + szMSec;
    }

    void SetAsCurrentLocal (size_t szMSec = 0);
    void SetAsCurrentGlobal(size_t szMSec = 0);

    static FILETIME GetFILETIME_FromMSec(QWORD v)
        { return TDateTime(v / 1000).GetFILETIME((size_t)(v % 1000)); }

    static SYSTEMTIME GetSYSTEMTIME_FromMSec(QWORD v)
        { return TDateTime(v / 1000).GetSYSTEMTIME((size_t)(v % 1000)); }

    TDateTime& LocalToGlobal();
    TDateTime& GlobalToLocal();

    #ifdef _MSC_VER
        double GetCOM_DT(size_t szMSec = 0) const;

        TDateTime& SetCOM_DT(double dDT);
    #endif // _MSC_VER

    // Decreasing
    TDateTime& DecSec   (size_t szAmt = 1);
    TDateTime& DecMin   (size_t szAmt = 1);
    TDateTime& DecHour  (size_t szAmt = 1);
    TDateTime& DecDay   (size_t szAmt = 1);
    TDateTime& DecMonth (size_t szAmt = 1);
    TDateTime& DecYear  (size_t szAmt = 1);

    // Increasing
    TDateTime& IncSec   (size_t szAmt = 1);
    TDateTime& IncMin   (size_t szAmt = 1);
    TDateTime& IncHour  (size_t szAmt = 1);
    TDateTime& IncDay   (size_t szAmt = 1);
    TDateTime& IncMonth (size_t szAmt = 1);
    TDateTime& IncYear  (size_t szAmt = 1);

    // Aligning
    TDateTime& AlignToSec   ();
    TDateTime& AlignToMin   ();
    TDateTime& AlignToHour  ();
    TDateTime& AlignToDay   ();
    TDateTime& AlignToMonth ();
    TDateTime& AlignToYear  ();

    TDateTime AlignedToSec() const
        { return make_temp(*this)().AlignToSec(); }

    TDateTime AlignedToMin() const
        { return make_temp(*this)().AlignToMin(); }

    TDateTime AlignedToHour() const
        { return make_temp(*this)().AlignToHour(); }

    TDateTime AlignedToDay() const
        { return make_temp(*this)().AlignToDay(); }

    TDateTime AlignedToMonth() const
        { return make_temp(*this)().AlignToMonth(); }

    TDateTime AlignedToYear() const
        { return make_temp(*this)().AlignToYear(); }

    bool IsSecAligned   () const;
    bool IsMinAligned   () const;
    bool IsHourAligned  () const;
    bool IsDayAligned   () const;
    bool IsMonthAligned () const;
    bool IsYearAligned  () const;

    // Operators
    TDateTime& operator = (QWORD v);

    TDateTime& operator = (const FILETIME& ft);

    TDateTime& operator = (const SYSTEMTIME& st);

    operator QWORD () const;

    TDateTime operator + (INT64 iDlt) const
        { DEBUG_VERIFY(IsValid()); return TDateTime((QWORD)*this + iDlt); }

    TDateTime operator - (INT64 iDlt) const
        { DEBUG_VERIFY(IsValid()); return TDateTime((QWORD)*this - iDlt); }

    TDateTime& operator += (INT64 iDlt)
        { DEBUG_VERIFY(IsValid()); return *this = *this + iDlt; }

    TDateTime& operator -= (INT64 iDlt)
        { DEBUG_VERIFY(IsValid()); return *this = *this - iDlt; }

    FILETIME GetFILETIME(size_t szMSec = 0) const;

    operator FILETIME () const
        { return GetFILETIME(); }

    SYSTEMTIME GetSYSTEMTIME(size_t szMSec = 0) const;

    operator SYSTEMTIME () const
        { return GetSYSTEMTIME(); }

    // Static helpers
    static bool IsLeapYear(size_t szYear)
    {
        DEBUG_VERIFY(szYear >= 1);

        return szYear % 400 == 0 || szYear % 4 == 0 && szYear % 100 != 0;
    }

    static size_t GetMonthDays(size_t szYear, size_t szMonth)
    {
        DEBUG_VERIFY(szYear >= 1);

        DEBUG_VERIFY(szMonth >= 1 && szMonth <= 12);

        return ms_MonthDays[IsLeapYear(szYear) ? 1 : 0][szMonth - 1];
    }

    static size_t GetTotalMonthDaysBefore(size_t szYear, size_t szMonth)
    {
        DEBUG_VERIFY(szYear >= 1);

        DEBUG_VERIFY(szMonth >= 1 && szMonth <= 12);

        return ms_TotalMonthDays[IsLeapYear(szYear) ? 1 : 0][szMonth - 1];
    }

    static size_t GetYearDays(size_t szYear)
    {
        DEBUG_VERIFY(szYear >= 1);

        return IsLeapYear(szYear) ? 366 : 365;
    }

    static size_t GetNumLeapYearsBefore(size_t szYear)
    {
        DEBUG_VERIFY(szYear >= 1);

        szYear--;

        return szYear / 4 - szYear / 100 + szYear / 400;
    }

    // Helpers
    bool IsLeapYear() const
        { return IsLeapYear(m_szYear); }

    size_t GetMonthDays() const
        { return GetMonthDays(m_szYear, m_szMonth); }

    size_t GetTotalMonthDaysBefore() const
        { return GetTotalMonthDaysBefore(m_szYear, m_szMonth); }

    size_t GetYearDays() const
        { return GetYearDays(m_szYear); }

    size_t GetNumLeapYearsBefore() const
        { return GetNumLeapYearsBefore(m_szYear); }

    size_t GetDayOfWeek() const // 0 for Sunday
        { return (size_t)(((QWORD)*this / (24*60*60) + 1) % 7); }

    // Other
    KString FormatDate() const
        { return KString::Formatted(TEXT("%.4u-%.2u-%.2u"), m_szYear, m_szMonth, m_szDay); }

    KString FormatTime() const
        { return KString::Formatted(TEXT("%.2u:%.2u:%.2u"), m_szHour, m_szMin, m_szSec); }

    operator KString () const
        { return FormatDate() + ' ' + FormatTime(); }
};

DECLARE_BASIC_STREAMING(TDateTime);

// ------------------
// Date time segment
// ------------------
typedef TSegment<TDateTime> DTSEGMENT;

inline QWSEGMENT TO_QW(const DTSEGMENT& Segment)
    { return QWSEGMENT(Segment.m_First, Segment.m_Last); }

inline DTSEGMENT TO_DT(const QWSEGMENT& Segment)
    { return DTSEGMENT(Segment.m_First, Segment.m_Last); }

// ----------------
// Global routines
// ----------------
inline int Compare(const TDateTime& dt1, const TDateTime& dt2)
{
    if(dt1.m_szYear != dt2.m_szYear)
        return (int)(dt1.m_szYear - dt2.m_szYear);

    if(dt1.m_szMonth != dt2.m_szMonth)
        return (int)(dt1.m_szMonth - dt2.m_szMonth);

    if(dt1.m_szDay != dt2.m_szDay)
        return (int)(dt1.m_szDay - dt2.m_szDay);

    if(dt1.m_szHour != dt2.m_szHour)
        return (int)(dt1.m_szHour - dt2.m_szHour);

    if(dt1.m_szMin != dt2.m_szMin)
        return (int)(dt1.m_szMin - dt2.m_szMin);

    if(dt1.m_szSec != dt2.m_szSec)
        return (int)(dt1.m_szSec - dt2.m_szSec);

    return 0;
}

bool FromString(KString String, TDateTime& RDateTime);

#endif // date_time_h
