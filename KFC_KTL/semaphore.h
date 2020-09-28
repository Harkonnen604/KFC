#ifndef semaphore_h
#define semaphore_h

// ----------
// Semaphore
// ----------
class TSemaphore
{
private:
    HANDLE m_hSemaphore;

public:
    TSemaphore();

    TSemaphore(size_t szInitialCount, size_t szMaximumCount);

    TSemaphore(LPCTSTR pName, size_t szInitialCount, size_t szMaximumCount);

    #ifdef _MSC_VER
        TSemaphore(LPCTSTR pName);
    #endif // _MSC_VER

    ~TSemaphore()
        { Release(); }

    bool IsAllocated() const
        { return m_hSemaphore; }

    void Release();

    void Allocate(size_t szInitialCount, size_t szMaximumCount);

    bool Create(LPCTSTR pName, size_t szInitialCount, size_t szMaximumCount); // true on new

    #ifdef _MSC_VER
        void Open(LPCTSTR pName);
    #endif // _MSC_VER

    bool Wait(size_t szTimeout = INFINITE)
    {
        DEBUG_VERIFY_ALLOCATION;

        return WaitForSingleObject(m_hSemaphore, (DWORD)szTimeout) == WAIT_OBJECT_0;
    }

    bool WaitWithTermination(HANDLE hTerminator, size_t szTimeout = INFINITE);

    void Unlock(size_t szCount = 1)
    {
        DEBUG_VERIFY_ALLOCATION;

        if(!szCount)
            return;

        DEBUG_VERIFY((LONG)szCount > 0);

        DEBUG_EVERIFY(ReleaseSemaphore(m_hSemaphore, (DWORD)szCount, NULL));
    }

    HANDLE GetSemaphore() const
        { DEBUG_VERIFY_ALLOCATION; return m_hSemaphore; }

    operator HANDLE () const
        { return GetSemaphore(); }
};

// -----------------
// Semaphore locker
// -----------------
class TSemaphoreLocker
{
private:
    TSemaphore& m_Semaphore;

public:
    TSemaphoreLocker(TSemaphore& Semaphore) : m_Semaphore(Semaphore)
        { m_Semaphore.Wait(); }

    ~TSemaphoreLocker()
        { m_Semaphore.Unlock(); }
};

// -------------------
// Semaphore unlocker
// -------------------
class TSemaphoreUnlocker
{
private:
    TSemaphore& m_Semaphore;

public:
    TSemaphoreUnlocker(TSemaphore& Semaphore) : m_Semaphore(Semaphore) {}

    ~TSemaphoreUnlocker()
        { m_Semaphore.Unlock(); }
};

#endif // semaphore_h
