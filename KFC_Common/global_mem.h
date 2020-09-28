#ifndef global_mem_h
#define global_mem_h

#ifdef _MSC_VER

// -----------
// Global mem
// -----------
class TGlobalMem
{
private:
    bool m_bAllocated;

    HGLOBAL m_hMem;

public:
    TGlobalMem();

    TGlobalMem(size_t szSize, kflags_t flFlags);

    ~TGlobalMem() { Release(); }

    bool IsAllocated() const
        { return m_bAllocated; }

    void Invalidate();

    void Release(bool bFromAllocatorException = false);

    void Allocate(size_t szSize, kflags_t flFlags);

    void ReOwn(TGlobalMem& SGlobalMem);

    void Lock(void*& pRData);

    void Unlock();

    HGLOBAL GetMem() const
    {
        DEBUG_VERIFY_ALLOCATION;

        return m_hMem;
    }

    operator HGLOBAL () const { return GetMem(); }
};

// ------------------
// Global mem locker
// ------------------
class TGlobalMemLocker
{
private:
    TGlobalMem* m_pGlobalMem;

public:
    TGlobalMemLocker(TGlobalMem& SGlobalMem, void*& pRData);

    ~TGlobalMemLocker();

    // ---------------- TRIVIALS ----------------
    TGlobalMem& GetGlobalMem() const { return *m_pGlobalMem; }
};

#endif // _MSC_VER

#endif // global_mem_h
