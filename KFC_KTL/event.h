#ifndef event_h
#define event_h

// ------
// Event
// ------
class TEvent
{
private:
    HANDLE m_hEvent;

public:
    TEvent();

    TEvent(bool bInitialState, bool bManualReset);

    TEvent(LPCTSTR pName, bool bInitialState, bool bManualReset);

    #ifdef _MSC_VER
        TEvent(LPCTSTR pName);
    #endif // _MSC_VER

    ~TEvent()
        { Release(); }

    bool IsAllocated() const
        { return m_hEvent; }

    void Release();

    void Allocate(bool bInitialState, bool bManualReset);

    bool Create(LPCTSTR pName, bool bInitialState, bool bManualReset); // true on new

    #ifdef _MSC_VER
        void Open(LPCTSTR pName);
    #endif // _MSC_VER

    void Set()
    {
        DEBUG_VERIFY_ALLOCATION;

        SetEvent(m_hEvent);
    }

    void Reset()
    {
        DEBUG_VERIFY_ALLOCATION;

        ResetEvent(m_hEvent);
    }

    bool Wait(size_t szTimeout = INFINITE)
    {
        DEBUG_VERIFY_ALLOCATION;

        return WaitForSingleObject(m_hEvent, (DWORD)szTimeout) == WAIT_OBJECT_0;
    }

    bool WaitWithTermination(HANDLE hTerminator, size_t szTimeout = INFINITE);

    HANDLE GetEvent() const
    {
        DEBUG_VERIFY_ALLOCATION;

        return m_hEvent;
    }

    HANDLE GetEventSafe() const
        { return this && IsAllocated() ? m_hEvent : NULL; }

    operator HANDLE () const
        { return GetEvent(); }
};

#endif // event_h
