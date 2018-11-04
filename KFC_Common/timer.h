#ifndef timer_h
#define timer_h

#include <KFC_KTL/suspendable.h>

// Immediate time
#define IMMEDIATE_TIME	(1)

// ------
// Timer
// ------
class TTimer : public TSuspendable
{
private:
	bool m_bAllocated;

	QWORD	m_qwLastStartTime;
	QWORD	m_qwAccumulatedTime;
	float	m_fSpeedCoef;

	bool OnSuspend	();
	bool OnResume	();
		
public:
	TTimer(bool bAllocate = false, QWORD qwSStartTime = QWORD_MAX);

	~TTimer()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release();

	void Allocate(QWORD qwSStartTime = QWORD_MAX);

	void Reset(QWORD qwSStartTime = QWORD_MAX);

	void SetSpeedCoef(float fSSpeedCoef, QWORD qwCurTime = QWORD_MAX);

	QWORD GetElapsedTime(QWORD qwCurTime = QWORD_MAX) const;

	operator QWORD () const
		{ return (QWORD)GetElapsedTime(); }

	// ---------------- TRIVIALS ----------------
	QWORD GetLastStartTime	() const { return m_qwLastStartTime;	}
	QWORD GetAccumulatedTime() const { return m_qwAccumulatedTime;	}

	float GetSpeedCoef() const { return m_fSpeedCoef; }
};

#endif // timer_h
