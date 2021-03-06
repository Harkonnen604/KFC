#ifndef window_timer_h
#define window_timer_h

#ifdef _MSC_VER

// -------------
// Window timer
// -------------
class TWindowTimer
{
private:
    bool m_bAllocated;

    HWND    m_hWnd;
    size_t  m_szEventID;
    size_t  m_szTimerID;

public:
    TWindowTimer();

    ~TWindowTimer() { Release(); }

    bool IsAllocated() const
        { return m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(  HWND        hSWnd,
                    size_t      szTimeout,
                    size_t      szSEventID  = 0,
                    TIMERPROC   pTimerProc  = NULL);

    size_t GetTimerID() const;

    operator size_t () const { return GetTimerID(); }

    // ---------------- TRIVALS ----------------
    HWND    GetWindow   () const { return m_hWnd;       }
    size_t  GetEventID  () const { return m_szEventID;  }
};

#endif // _MSC_VER

#endif // window_timer_h
