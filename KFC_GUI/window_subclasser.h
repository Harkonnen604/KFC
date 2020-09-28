#ifndef window_subclasser_h
#define window_subclasser_h

#include <KFC_KTL\globals.h>
#include <KFC_KTL\critical_section.h>

class T_WS_Recaller;

// --------------------------
// Window subclasser globals
// --------------------------
class TWindowSubclasserGlobals : public TGlobals
{
private:
    void OnUninitialize ();
    void OnInitialize   ();

private:
    mutable TCriticalSection m_AllocationCS;

    TArray<void*, true> m_Allocations;

    void* m_pFirstFree;

public:
    WNDPROC Reserve(T_WS_Recaller* pRecaller);

    void Free(WNDPROC pProc);

    T_WS_Recaller* GetRecaller(WNDPROC pProc) const;

public:
    ATOM m_aFirstWS;

public:
    TWindowSubclasserGlobals();
};

extern TWindowSubclasserGlobals g_WindowSubclasserGlobals;

// ----------------
// Helper routines
// ----------------
T_WS_Recaller* _FindWS_Recaller(HWND                    hWnd,
                                const T_WS_Recaller&    Recaller,
                                T_WS_Recaller*          pAfter = NULL);

T_WS_Recaller* _SubclassWindow(HWND hWnd, TPtrHolder<T_WS_Recaller> pRecaller);

void _UnsubclassWindow(HWND hWnd, T_WS_Recaller* pRecaller);

// ------------
// WS recaller
// ------------
class T_WS_Recaller
{
protected:
    // Descender
    class TDescender
    {
    public:
        // Exit mMode
        enum TExitMode
        {
            EM_NORMAL,
            EM_DELETE,
            EM_UNSUBCLASS,
        };

    private:
        T_WS_Recaller& m_Recaller;

        HWND m_hWnd;

        TExitMode m_ExitMode;

    public:
        TDescender(T_WS_Recaller& Recaller, HWND hWnd, TExitMode ExitMode) :
            m_Recaller(Recaller), m_hWnd(hWnd), m_ExitMode(ExitMode) {}

        ~TDescender()
        {
            switch(m_ExitMode)
            {
            case EM_DELETE:
                delete &m_Recaller;
                break;

            case EM_UNSUBCLASS:
                _UnsubclassWindow(m_hWnd, &m_Recaller);
                break;
            }
        }
    };

public:
    WNDPROC m_pOwnWindowProc;

    WNDPROC m_pNextWindowProc;

    T_WS_Recaller* m_pChainPrev;
    T_WS_Recaller* m_pChainNext;

    T_WS_Recaller* m_pScanPrev;
    T_WS_Recaller* m_pScanNext;

    bool m_bDeleted;

protected:
    virtual LRESULT Callback(   HWND    hWnd,
                                UINT    uiMsg,
                                WPARAM  wParam,
                                LPARAM  lParam) = 0;

public:
    T_WS_Recaller()
    {
        m_pOwnWindowProc = g_WindowSubclasserGlobals.Reserve(this);

        m_pNextWindowProc = NULL;

        m_pChainPrev = NULL;
        m_pChainNext = NULL;

        m_pScanPrev = NULL;
        m_pScanNext = NULL;

        m_bDeleted = false;
    }

    virtual ~T_WS_Recaller()
        { g_WindowSubclasserGlobals.Free(m_pOwnWindowProc); }

    LRESULT Invoke( HWND    hWnd,
                    UINT    uiMsg,
                    WPARAM  wParam,
                    LPARAM  lParam)
    {
        TDescender Descender0(  *this,
                                hWnd,
                                uiMsg == WM_NCDESTROY ? TDescender::EM_DELETE :
                                m_bDeleted ? TDescender::EM_UNSUBCLASS :
                                TDescender::EM_NORMAL);

        return  m_bDeleted ?
                    CallWindowProc(m_pNextWindowProc, hWnd, uiMsg, wParam, lParam) :
                    Callback(hWnd, uiMsg, wParam, lParam);
    }

    virtual bool Equals(const T_WS_Recaller& Recaller) const = 0;
};

// -------------------
// Static WS recaller
// -------------------
class TStaticWS_Recaller : public T_WS_Recaller
{
public:
    // Proc
    typedef LRESULT (*TProc)(   HWND    hWnd,
                                UINT    uiMsg,
                                WPARAM  wParam,
                                LPARAM  lParam,
                                WNDPROC pOldWndProc);

private:
    TProc m_pProc;

private:
    LRESULT Callback(   HWND    hWnd,
                        UINT    uiMsg,
                        WPARAM  wParam,
                        LPARAM  lParam)
    {
        return m_pProc(hWnd, uiMsg, wParam, lParam, m_pNextWindowProc);
    }

public:
    TStaticWS_Recaller(TProc pProc) : m_pProc(pProc) {}

    bool Equals(const T_WS_Recaller& Recaller) const
    {
        const TStaticWS_Recaller* pRecaller =
            dynamic_cast<const TStaticWS_Recaller*>(&Recaller);

        return pRecaller && m_pProc == pRecaller->m_pProc;
    }
};

// -------------------
// Method WS recaller
// -------------------
template <class t>
class TMethodWS_Recaller : public T_WS_Recaller
{
public:
    // Proc
    typedef LRESULT (t::*TProc)(HWND    hWnd,
                                UINT    uiMsg,
                                WPARAM  wParam,
                                LPARAM  lParam,
                                WNDPROC pOldWndProc);

private:
    t& m_Object;

    TProc m_pProc;

private:
    LRESULT Callback(   HWND    hWnd,
                        UINT    uiMsg,
                        WPARAM  wParam,
                        LPARAM  lParam)
    {
        return (m_Object.*m_pProc)(hWnd, uiMsg, wParam, lParam, m_pNextWindowProc);
    }

public:
    TMethodWS_Recaller(t& Object, TProc pProc) :
        m_Object(Object), m_pProc(pProc) {}

    bool Equals(const T_WS_Recaller& Recaller) const
    {
        const TMethodWS_Recaller<t>* pRecaller =
            dynamic_cast<const TMethodWS_Recaller<t>*>(&Recaller);

        return  pRecaller &&
                &m_Object == &pRecaller->m_Object &&
                m_pProc   == pRecaller->m_pProc;
    }
};

// -----------------------
// Window subclass handle
// -----------------------
typedef struct {}* HWINSUB;

// ----------------
// Global routines
// ----------------
inline HWINSUB FindWindowSubclasser(HWND    hWnd,
                                    LRESULT (*pProc)(HWND, UINT, WPARAM, LPARAM, WNDPROC),
                                    HWINSUB hAfter = NULL)
{
    return (HWINSUB)_FindWS_Recaller(   hWnd,
                                        TStaticWS_Recaller(pProc),
                                        (T_WS_Recaller*)hAfter);
}

template <class t>
inline HWINSUB FindWindowSubclasser(HWND    hWnd,
                                    t&      Object,
                                    LRESULT (t::*pProc)(HWND, UINT, WPARAM, LPARAM, WNDPROC),
                                    HWINSUB hAfter = NULL)
{
    return (HWINSUB)_FindWS_Recaller(   hWnd,
                                        TMethodWS_Recaller<t>(Object, pProc),
                                        (T_WS_Recaller*)hAfter);
}

inline HWINSUB SubclassWindow(  HWND    hWnd,
                                LRESULT (*pProc)(HWND, UINT, WPARAM, LPARAM, WNDPROC),
                                bool    bUnique = false)
{
    if(bUnique)
    {
        if(HWINSUB hWS = FindWindowSubclasser(hWnd, pProc))
            return hWS;
    }

    return (HWINSUB)_SubclassWindow(hWnd, new TStaticWS_Recaller(pProc));
}

template <class t>
inline HWINSUB SubclassWindow(  HWND    hWnd,
                                t&      Object,
                                LRESULT (t::*pProc)(HWND, UINT, WPARAM, LPARAM, WNDPROC),
                                bool    bUnique = false)
{
    if(bUnique)
    {
        if(HWINSUB hWS = FindWindowSubclasser(hWnd, Object, pProc))
            return hWS;
    }

    return (HWINSUB)_SubclassWindow(hWnd, new TMethodWS_Recaller<t>(Object, pProc));
}

inline void UnsubclassWindow(HWND hWnd, HWINSUB hWS)
    { _UnsubclassWindow(hWnd, (T_WS_Recaller*)hWS); }

inline void UnsubclassWindow(   HWND    hWnd,
                                LRESULT (*pProc)(HWND, UINT, WPARAM, LPARAM, WNDPROC))
{
    UnsubclassWindow(hWnd, FindWindowSubclasser(hWnd, pProc));
}

template <class t>
inline void UnsubclassWindow(   HWND    hWnd,
                                t&      Object,
                                LRESULT (t::*pProc)(HWND, UINT, WPARAM, LPARAM, WNDPROC))
{
    UnsubclassWindow(hWnd, FindWindowSubclasser(hWnd, Object, pProc));
}

#endif // window_subclasser_h
