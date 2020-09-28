#ifndef api_hook_h
#define api_hook_h

#include <KFC_KTL\tls_storage.h>

// --------------
// API hook base
// --------------
class T_API_HookBase
{
public:
    // In-hook counter
    struct TInHookCounter
    {
        // Order should not change
        DWORD m_dwCounter;
        DWORD m_dwEDI;
        DWORD m_dwESI;


        TInHookCounter() : m_dwCounter(0) {}
    };

    // TLS in-hook counter getter
    typedef TInHookCounter& T_TLS_InHookCounterGetter();

private:
    // Protector
    class TProtector
    {
    private:
        void*   m_pData;
        size_t  m_szSize;
        DWORD   m_dwOldProtect;

    public:
        TProtector();

        TProtector(void* pSData, size_t szSSize, DWORD dwProtect);

        ~TProtector()
            { Release(); }

        bool IsAllocated() const
            { return m_pData; }

        void Release();

        void Allocate(void* pSData, size_t szSSize, DWORD dwProtect);
    };


    bool m_bAllocated;

    void* m_pOrigProc;

    // Must go one after another, size shouldn't change
    BYTE m_OuterCallWrapper [64];
    BYTE m_TrampCode        [64];

    void* m_pTrampProc;

    TProtector m_TrampCodeProtector;

    TProtector m_OuterCallWrapperProtector;

public:
    T_API_HookBase();

    ~T_API_HookBase()
        { Release(); }

    bool IsAllocated() const
        { return m_bAllocated; }

    void Release();

    bool Allocate(  LPCTSTR                     pName,
                    void*                       pSOrigProc,
                    void*                       pHookProc,
                    T_TLS_InHookCounterGetter*  pInHookCounterGetter, // NULL for none
                    bool                        bSafe);

    void* GetTrampProc() const
        { DEBUG_VERIFY_ALLOCATION; return m_pTrampProc; }

    bool IsHooked() const
        { DEBUG_VERIFY_ALLOCATION; return m_pTrampProc != m_pOrigProc; }
};

// ---------
// API hook
// ---------
template <class ft>
class T_API_Hook : public T_API_HookBase
{
public:
    bool Allocate(  LPCTSTR                     pName,
                    ft*                         pSOrigProc,
                    ft*                         pHookProc,
                    T_TLS_InHookCounterGetter*  pInHookCounterGetter,
                    bool                        bSafe);

    ft* GetTrampProc() const
        { return (ft*)T_API_HookBase::GetTrampProc(); }

    ft* operator () () const
        { return GetTrampProc(); }
};

template <class ft>
bool T_API_Hook<ft>::Allocate(  LPCTSTR                     pName,
                                ft*                         pSOrigProc,
                                ft*                         pHookProc,
                                T_TLS_InHookCounterGetter*  pInHookCounterGetter,
                                bool                        bSafe)
{
    return T_API_HookBase::Allocate(pName,
                                    (void*)pSOrigProc,
                                    (void*)pHookProc,
                                    pInHookCounterGetter,
                                    bSafe);
}

#endif // api_hook_h
