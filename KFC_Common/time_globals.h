#ifndef time_globals_h
#define time_globals_h

#include <KFC_KTL/globals.h>

// -------------
// Time globals
// -------------
struct TTimeGlobals : public TGlobals
{
private:
    QWORD m_qwFrequency;

    QWORD m_qwLastStartTime;
    QWORD m_qwAccumulatedTime;


    void OnInitialize   ();
    void OnUninitialize ();

    bool OnSuspend  ();
    bool OnResume   ();

public:
    TTimeGlobals();

    QWORD GetGlobalMSEC() const
    {
        DEBUG_VERIFY_INITIALIZATION;

        return GetSystemTimerValue(m_qwFrequency);
    }

    QWORD GetMSEC() const;

    #ifdef _MSC_VER

        static QWORD GetSystemTimerFrequency();

        static QWORD GetSystemTimerValue(QWORD qwFrequency);

    #else // _MSC_VER

        static QWORD GetSystemTimerFrequency()
            { return 1000; }

        static QWORD GetSystemTimerValue(QWORD qwFrequency)
            { assert(qwFrequency == 1000); return msec_time(); }

    #endif // _MSC_VER

    // ---------------- TRIVIALS ----------------
    #ifdef _MSC_VER
        QWORD GetFrequency() const { return m_qwFrequency; }
    #endif // _MSC_VER
};

extern TTimeGlobals g_TimeGlobals;

#endif // time_globals_h
