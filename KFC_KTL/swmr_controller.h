#ifndef swmr_controller_h
#define swmr_controller_h

#include "event.h"

// ----------------
// SWMR controller
// ----------------
class T_SWMR_Controller
{
private:
    // Item
    struct TItem
    {
    public:
        TEvent  m_GrantedEvent;
        TEvent  m_FreedEvent;
        bool    m_bWriter;

    public:
        TItem();
    };

private:
    TCriticalSection m_AccessCS;

    TArray<TItem> m_Items;

    size_t m_szHead;
    size_t m_szTail;
    size_t m_szN;

    size_t  m_szTimesReadLocked;
    bool    m_bWriteLocked;

public:
    T_SWMR_Controller(size_t szMaxThreads = 0);

    ~T_SWMR_Controller()
        { Release(); }

    bool IsAllocated() const
        { return !m_Items.IsEmpty(); }

    void Release();

    void Allocate(size_t szMaxThreads);

    bool TryLockRead();

    void LockRead();

    void UnlockRead();

    bool TryLockWrite();

    void LockWrite();

    void UnlockWrite();
};

// -----------------
// SWMR read locker
// -----------------
class T_SWMR_ReadLocker
{
private:
    T_SWMR_Controller& m_Controller;

public:
    T_SWMR_ReadLocker(T_SWMR_Controller& Controller) :
        m_Controller(Controller)
    {
        DEBUG_VERIFY(m_Controller.IsAllocated());

        m_Controller.LockRead();
    }

    ~T_SWMR_ReadLocker()
        { m_Controller.UnlockRead(); }
};

// -------------------
// SWMR read unlocker
// -------------------
class T_SWMR_ReadUnlocker
{
private:
    T_SWMR_Controller& m_Controller;

public:
    T_SWMR_ReadUnlocker(T_SWMR_Controller& Controller) :
        m_Controller(Controller)
    {
        DEBUG_VERIFY(m_Controller.IsAllocated());
    }

    ~T_SWMR_ReadUnlocker()
        { m_Controller.UnlockRead(); }
};

// ------------------
// SWMR write locker
// ------------------
class T_SWMR_WriteLocker
{
private:
    T_SWMR_Controller& m_Controller;

public:
    T_SWMR_WriteLocker(T_SWMR_Controller& Controller) :
        m_Controller(Controller)
    {
        DEBUG_VERIFY(m_Controller.IsAllocated());

        m_Controller.LockWrite();
    }

    ~T_SWMR_WriteLocker()
        { m_Controller.UnlockWrite(); }
};

// --------------------
// SWMR write unlocker
// --------------------
class T_SWMR_WriteUnlocker
{
private:
    T_SWMR_Controller& m_Controller;

public:
    T_SWMR_WriteUnlocker(T_SWMR_Controller& Controller) :
        m_Controller(Controller)
    {
        DEBUG_VERIFY(m_Controller.IsAllocated());
    }

    ~T_SWMR_WriteUnlocker()
        { m_Controller.UnlockWrite(); }
};

#endif // swmr_controller_h
