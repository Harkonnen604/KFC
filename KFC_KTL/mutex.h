#ifndef mutex_h
#define mutex_h

// ------
// Mutex
// ------
class TMutex
{
private:
	HANDLE m_hMutex;

public:
	TMutex();

	TMutex(bool bInitialOwnership, LPCTSTR pName = NULL);
	
	#ifdef _MSC_VER
		TMutex(LPCTSTR pName);
	#endif // _MSC_VER

	~TMutex()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hMutex; }

	void Release();

	bool Create(bool bInitialOwnership, LPCTSTR pName = NULL); // true on new

	#ifdef _MSC_VER
		void Open(LPCTSTR pName);
	#endif // _MSC_VER

	bool Lock(size_t szTimeout = INFINITE)
	{
		DEBUG_VERIFY_ALLOCATION;

		return WaitForSingleObject(m_hMutex, szTimeout) == WAIT_OBJECT_0;
	}

	bool TerminableLock(HANDLE hTerminator, size_t szTimeout = INFINITE)
	{
		DEBUG_VERIFY_ALLOCATION;

		if(!hTerminator)
			return Lock(szTimeout);

		HANDLE Handles[2] = {hTerminator, m_hMutex};

		DWORD r = WaitForMultipleObjects(2, Handles, FALSE, szTimeout);

		if(r == WAIT_TIMEOUT)
			return false;

		if(r == WAIT_OBJECT_0 + 0)
			throw TTerminationException(__FILE__, __LINE__);

		DEBUG_VERIFY(r == WAIT_OBJECT_0 + 1);

		return true;
	}

	void Unlock()
	{
		DEBUG_VERIFY_ALLOCATION;

		ReleaseMutex(m_hMutex);
	}

	HANDLE GetMutex() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hMutex; }

	operator HANDLE () const
		{ return GetMutex(); }
};

// -------------
// Mutex locker
// -------------
class TMutexLocker
{
private:
	TMutex* m_pMutex;

private:
	TMutexLocker(const TMutexLocker&);

	TMutexLocker& operator = (const TMutexLocker&);

public:
	TMutexLocker(TMutex& Mutex, size_t szTimeout = INFINITE)
		{ m_pMutex = Mutex.Lock(szTimeout) ? &Mutex : NULL; }

	~TMutexLocker()
		{ if(m_pMutex) m_pMutex->Unlock(); }
};

// ------------------
// Safe mutex locker
// ------------------
class TSafeMutexLocker
{
private:
	TMutex* m_pMutex;

private:
	TSafeMutexLocker(const TSafeMutexLocker&);

	TSafeMutexLocker& operator = (const TSafeMutexLocker&);

public:
	TSafeMutexLocker(TMutex* pMutex, size_t szTimeout = INFINITE)
		{ m_pMutex = pMutex && pMutex->Lock(szTimeout) ? pMutex : NULL; } 

	~TSafeMutexLocker()
		{ if(m_pMutex) m_pMutex->Unlock(); }
};

// ------------------------
// Terminable mutex locker
// ------------------------
class TTerminableMutexLocker
{
private:
	TMutex* m_pMutex;

private:
	TTerminableMutexLocker(const TTerminableMutexLocker&);

	TTerminableMutexLocker& operator = (const TTerminableMutexLocker&);

public:
	TTerminableMutexLocker(TMutex& Mutex, HANDLE hTerminator, size_t szTimeout = INFINITE)
		{ m_pMutex = Mutex.TerminableLock(hTerminator, szTimeout) ? &Mutex : NULL; }

	~TTerminableMutexLocker()
		{ if(m_pMutex) m_pMutex->Unlock(); }
};

// -----------------------------
// Safe terminable mutex locker
// -----------------------------
class TSafeTerminableMutexLocker
{
private:
	TMutex* m_pMutex;

private:
	TSafeTerminableMutexLocker(const TSafeTerminableMutexLocker&);

	TSafeTerminableMutexLocker& operator = (const TSafeTerminableMutexLocker&);

public:
	TSafeTerminableMutexLocker(TMutex* pMutex, HANDLE hTerminator, size_t szTimeout = INFINITE)
		{ m_pMutex = pMutex && pMutex->TerminableLock(hTerminator, szTimeout) ? pMutex : NULL; }

	~TSafeTerminableMutexLocker()
		{ if(m_pMutex) m_pMutex->Unlock(); }
};

#endif // mutex_h
