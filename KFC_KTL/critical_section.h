#ifndef critical_section_h
#define critical_section_h

// -----------------
// Critical section
// -----------------

#ifdef _MSC_VER

class TCriticalSection
{
private:
	mutable CRITICAL_SECTION m_CriticalSection;

private:
	TCriticalSection(const TCriticalSection&);

	TCriticalSection& operator = (const TCriticalSection&);

public:
	TCriticalSection()
		{ InitializeCriticalSection(&m_CriticalSection); }

	~TCriticalSection()
		{ DeleteCriticalSection(&m_CriticalSection); }

	void Lock()
		{ EnterCriticalSection(&m_CriticalSection); }

	bool TryLock()
		{ return TryEnterCriticalSection(&m_CriticalSection); }

	void Unlock()
		{ LeaveCriticalSection(&m_CriticalSection); }

	CRITICAL_SECTION* GetCriticalSection() const
		{ return &m_CriticalSection; }

	operator CRITICAL_SECTION* () const
		{ return GetCriticalSection(); }
};

#else // _MSC_VER

class TCriticalSection
{
private:	
	mutable pthread_mutex_t m_Mutex;
	
private:
	TCriticalSection(const TCriticalSection&);

	TCriticalSection& operator = (const TCriticalSection&);

public:
	TCriticalSection(bool bRecursive = true)
	{
		if(bRecursive)
		{
			int r;
		
			pthread_mutexattr_t Attribute;
		
			r = pthread_mutexattr_init(&Attribute);
			assert(!r);

			r = pthread_mutexattr_settype(&Attribute, PTHREAD_MUTEX_RECURSIVE);
			assert(!r);
			
			r = pthread_mutex_init(&m_Mutex, &Attribute);
			assert(!r);
			
			pthread_mutexattr_destroy(&Attribute);
		}
		else
		{
			int r;
			
			r = pthread_mutex_init(&m_Mutex, NULL);
			assert(!r);
		}
	}

	~TCriticalSection()
		{ pthread_mutex_destroy(&m_Mutex); }

	bool Lock()
		{ return !pthread_mutex_lock(&m_Mutex); }
		
	bool TryLock()
		{ return !pthread_mutex_trylock(&m_Mutex); }

	bool Unlock()
		{ return !pthread_mutex_unlock(&m_Mutex); }
		
	pthread_mutex_t* GetMutex() const
		{ return &m_Mutex; }
		
	operator pthread_mutex_t* () const
		{ return GetMutex(); }
};

// -------------------
// Condition variable
// -------------------
class TConditionVariable
{
private:
	TCriticalSection m_CS;

	mutable pthread_cond_t m_Cond;	

public:
	TConditionVariable() : m_CS(false)
	{
		int r = pthread_cond_init(&m_Cond, NULL);
		assert(!r);
	}
	
	~TConditionVariable()	
		{ pthread_cond_destroy(&m_Cond); }
		
	void Lock()
		{ m_CS.Lock(); }
		
	void Unlock()
		{ m_CS.Unlock(); }
		
	void Signal()
		{ pthread_cond_signal(&m_Cond); }
		
	void Broadcast()
		{ pthread_cond_broadcast(&m_Cond); }
		
	void Wait()
		{ pthread_cond_wait(&m_Cond, m_CS); }
		
	bool Wait(size_t szTimeout)
	{
		if(szTimeout == -1)
			return Wait(), true;
			
		timeval tv;
		gettimeofday(&tv, NULL);
		
		tv.tv_sec  += szTimeout / 1000;
		tv.tv_usec += szTimeout % 1000 * 1000;
		
		if(tv.tv_usec >= 1000000)
		{
			tv.tv_sec++, tv.tv_usec -= 1000000;
			assert(tv.tv_usec < 1000000);
		}

		timespec ts = {tv.tv_sec, tv.tv_usec * 1000};
		
		return !pthread_cond_timedwait(&m_Cond, m_CS, &ts);
	}
};

// --------------------------
// Condition variable locker
// --------------------------
class TConditionVariableLocker
{
private:
	TConditionVariable& m_CV;
	
private:
	TConditionVariableLocker(const TConditionVariableLocker&);
	
	TConditionVariableLocker& operator = (const TConditionVariableLocker&);
	
public:
	TConditionVariableLocker(TConditionVariable& CV) : m_CV(CV)
		{ m_CV.Lock(); }
		
	~TConditionVariableLocker()
		{ m_CV.Unlock(); }
};

#endif // _MSC_VER

// ------------------------
// Critical section locker
// ------------------------
class TCriticalSectionLocker
{
private:
	TCriticalSection& m_CS;

private:
	TCriticalSectionLocker();

	TCriticalSectionLocker& operator = (const TCriticalSectionLocker&);

public:
	TCriticalSectionLocker(TCriticalSection& CS) : m_CS(CS)
		{ m_CS.Lock(); }

	~TCriticalSectionLocker()
		{ m_CS.Unlock(); }
};

// -----------------------------
// Safe critical section locker
// -----------------------------
class TSafeCriticalSectionLocker
{
private:
	TCriticalSection* m_pCS;

private:
	TSafeCriticalSectionLocker();

	TSafeCriticalSectionLocker& operator = (const TSafeCriticalSectionLocker&);

public:
	TSafeCriticalSectionLocker(TCriticalSection* pCS) : m_pCS(pCS)
		{ if(m_pCS) m_pCS->Lock(); }

	~TSafeCriticalSectionLocker()
		{ if(m_pCS) m_pCS->Unlock(); }
};

// --------------------------
// Critical section unlocker
// --------------------------
class TCriticalSectionUnlocker
{
private:
	TCriticalSection& m_CS;

private:
	TCriticalSectionUnlocker();

	TCriticalSectionUnlocker& operator = (const TCriticalSectionUnlocker&);

public:
	TCriticalSectionUnlocker(TCriticalSection& CS) : m_CS(CS) {}

	~TCriticalSectionUnlocker()
		{ m_CS.Unlock(); }
};

// --------------------------------
// Delayed critical section locker
// --------------------------------
class TDelayedCriticalSectionLocker
{
private:
	TCriticalSection* m_pCS;

	bool m_bLocked;

public:
	TDelayedCriticalSectionLocker()
	{
		m_pCS = NULL;

		m_bLocked = false;
	}

	TDelayedCriticalSectionLocker(TCriticalSection& CS, bool bLock = false)
	{
		m_pCS = &CS;

		m_bLocked = false;

		if(bLock)
			Lock();
	}

	~TDelayedCriticalSectionLocker()
		{ Release(); }

	void Release()
	{
		if(m_pCS)
		{
			if(m_bLocked)
				Unlock();

			m_pCS = NULL;

			m_bLocked = false;
		}
	}

	bool TryLock(TCriticalSection& CS)
	{
		Release();

		m_pCS = &CS;

		m_bLocked = false;

		return TryLock();
	}

	bool TryLock()
	{
		assert(m_pCS);

		if(m_bLocked)
			return true;

		return m_bLocked = m_pCS->TryLock();
	}

	void Lock(TCriticalSection& CS)
	{
		Release();

		m_pCS = &CS;

		m_bLocked = false;

		Lock();
	}

	void Lock()
	{
		assert(m_pCS);

		if(m_bLocked)
			return;

		m_pCS->Lock();

		m_bLocked = true;
	}

	void Unlock()
	{
		assert(m_pCS);

		if(!m_bLocked)
			return;

		m_pCS->Unlock();

		m_bLocked = false;
	}
};

#endif // critical_section_h
