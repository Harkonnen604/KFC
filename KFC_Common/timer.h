#ifndef timer_h
#define timer_h

#include <KFC_KTL/suspendable.h>

// Immediate time
#define IMMEDIATE_TIME  (1)

// ------
// Timer
// ------
class TTimer : public TSuspendable
{
private:
    bool m_bAllocated;

    QWORD   m_qwLastStartTime;
    QWORD   m_qwAccumulatedTime;
    float   m_fSpeedCoef;

    bool OnSuspend  ();
    bool OnResume   ();

public:
    TTimer(bool bAllocate = false, QWORD qwSStartTime = -1);

    ~TTimer()
        { Release(); }

    bool IsAllocated() const
        { return m_bAllocated; }

    void Release();

    void Allocate(QWORD qwSStartTime = -1);

    void Reset(QWORD qwSStartTime = -1);

    void SetSpeedCoef(float fSSpeedCoef, QWORD qwCurTime = -1);

    QWORD GetElapsedTime(QWORD qwCurTime = -1) const;

    operator QWORD () const
        { return (QWORD)GetElapsedTime(); }

    // ---------------- TRIVIALS ----------------
    QWORD GetLastStartTime  () const { return m_qwLastStartTime;    }
    QWORD GetAccumulatedTime() const { return m_qwAccumulatedTime;  }

    float GetSpeedCoef() const { return m_fSpeedCoef; }
};

#endif // timer_h
