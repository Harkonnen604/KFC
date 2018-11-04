#ifndef ktl_device_globals_h
#define ktl_device_globals_h

#include "globals.h"
#include "critical_section.h"
#include "process.h"

// -------------------
// KTL device globals
// -------------------
class T_KTL_DeviceGlobals : public TGlobals
{
private:
	void OnUninitialize	();
	void OnInitialize	();
	
private:
/*	struct TItem
	{
	public:
		TWaitableTimerSyncObject* m_pTimer;		
		
	public:
		TItem(TWaitableTimerSyncObject* pTimer) : m_pTimer(pTimer) {}
	};
	
	friend inline int Compare(TItem Item1, TItem Item2);

	typedef T_AVL_Storage<TItem> TWaitTimes;*/
	
public:
//	typedef TWaitTimes::TIterator TTimerIter;
	
private:
/*	TWaitTimes m_WaitTimes;
	
	TConditionVariable m_WaitTimesCV;
	
	HANDLE m_hWaitTimesThread;
	
	bool m_bTerminate;*/
	
private:
/*	static DECLARE_THREAD_PROC(StaticWaitTimesThreadProc, pParam);
	
	void WaitTimesThreadProc();*/

public:
	T_KTL_DeviceGlobals();
	
/*	TTimerIter RegisterTimer(TWaitableTimerSyncObject* pTimer)
	{
		TConditionVariableLocker Locker0(m_WaitTimesCV);
	
		TTimerIter Iter = m_WaitTimes.Add(pTimer);
		
		m_WaitTimesCV.Signal();
		
		return Iter;
	}
	
	void UnregisterTimer(TTimerIter Iter)
	{
		assert(Iter.IsValid());
		
		TConditionVariableLocker Locker0(m_WaitTimesCV);

		m_WaitTimes.Del(Iter);
	}
	
	void UpdateTimer(TTimerIter Iter)
	{
		assert(Iter.IsValid());
		
		TConditionVariableLocker Locker0(m_WaitTimesCV);

		m_WaitTimes.Modify(Iter);
		
		m_WaitTimesCV.Signal();		
	}*/
};

/*
inline int Compare(T_KTL_DeviceGlobals::TItem Item1, T_KTL_DeviceGlobals::TItem Item2)
{
	return Compare(Item1.m_pTimer->GetTimeDue(), Item2.m_pTimer->GetTimeDue());
}*/

extern T_KTL_DeviceGlobals g_KTL_DeviceGlobals;

#endif // ktl_device_globals_h
