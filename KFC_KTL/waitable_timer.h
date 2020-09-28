#ifndef waitable_timer_h
#define waitable_timer_h

#ifdef _MSC_VER

// ---------------
// Waitable timer
// ---------------
class TWaitableTimer
{
private:
	HANDLE m_hTimer;

public:
	TWaitableTimer();
	
	TWaitableTimer(bool bManualReset, LPCTSTR pName = NULL);
	
	#ifdef _MSC_VER
		TWaitableTimer(LPCTSTR pName);
	#endif // _MSC_VER

	~TWaitableTimer()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hTimer; }

	void Release();

	bool Create(bool bManualReset, LPCTSTR pName = NULL);

	#ifdef _MSC_VER
		void Open(LPCTSTR pName);
	#endif // _MSC_VER

	void SetRelative(size_t szDelay, size_t szPeriod = 0);

	void SetAbsolute(const FILETIME& GlobalTime, size_t szPeriod = 0);

	void Cancel(bool bSafe);

	bool Wait(size_t szTimeout = INFINITE)
	{
		DEBUG_VERIFY_ALLOCATION;

		return WaitForSingleObject(m_hTimer, (DWORD)szTimeout) == WAIT_OBJECT_0;
	}

	HANDLE GetTimer() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hTimer; }

	operator HANDLE () const
		{ return GetTimer(); }
};

#endif // _MSC_VER

#endif // waitable_timer_h
