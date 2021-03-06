#ifndef timelimit_h
#define timelimit_h

#ifdef _MSC_VER

// -----------------
// Timelimit result
// -----------------
enum TTimeLimitResult
{
    TR_OK           = 1,
    TR_EXPIRED      = 2,
    TR_PRE_HACK     = 3,
    TR_POST_HACK    = 4,
    TR_FORCE_UINT   = UINT_MAX,
};

// ----------------
// Global routines
// ----------------
TTimeLimitResult GetTimeLimitResult(QWORD qwExpirationPeriod, LPCTSTR pAppTypeID);

#endif // _MSC_VER

#endif // timelimit_h
