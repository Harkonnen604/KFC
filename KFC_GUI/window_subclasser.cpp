#include "kfc_gui_pch.h"
#include "window_subclasser.h"

TWindowSubclasserGlobals g_WindowSubclasserGlobals;

static const BYTE gs_Code[] =
{
    0x58, // POP EAX
    0x68, 0x00, 0x00, 0x00, 0x00, // PUSH recaller
    0x50, // PUSH EAX
    0xE9, 0x00, 0x00, 0x00, 0x00 // JMP stub
};

#define RECALLER_OFFSET     (2u)
#define STUB_PROC_OFFSET    (8u)

// -------------
// WS stub proc
// -------------
static LRESULT __stdcall WS_StubProc
    (T_WS_Recaller* pRecaller, HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    DEBUG_VERIFY(hWnd);

    DEBUG_VERIFY(pRecaller);

    return pRecaller->Invoke(hWnd, uiMsg, wParam, lParam);
}

// --------------------------
// Window subclasser globals
// --------------------------
TWindowSubclasserGlobals::TWindowSubclasserGlobals() :
    TGlobals(TEXT("Window subclasser globals"))
{
    m_pFirstFree = NULL;

    m_aFirstWS = 0;
}

void TWindowSubclasserGlobals::OnUninitialize()
{
    if(m_aFirstWS)
        DeleteAtom(m_aFirstWS), m_aFirstWS = 0;

    for(size_t i = m_Allocations.GetN() - 1 ; i != UINT_MAX ; i--)
        VirtualFree(m_Allocations[i], 0, MEM_RELEASE);

    m_Allocations.Clear();
}

void TWindowSubclasserGlobals::OnInitialize()
{
    m_pFirstFree = NULL;

    m_aFirstWS = AddAtom(TEXT("KFC_FirstWS"));
    KFC_VERIFY(m_aFirstWS);
}

WNDPROC TWindowSubclasserGlobals::Reserve(T_WS_Recaller* pRecaller)
{
    DEBUG_VERIFY_INITIALIZATION;

    TCriticalSectionLocker Locker0(m_AllocationCS);

    if(!m_pFirstFree)
    {
        BYTE*& pData = (BYTE*&)m_Allocations.Add();

        pData = (BYTE*)VirtualAlloc(NULL, 0x10000, MEM_RESERVE, PAGE_EXECUTE_READWRITE);

        if(!pData)
        {
            m_Allocations.DelLast();

            INITIATE_DEFINED_CODE_FAILURE
                (TEXT("Error reserving 64Kb address space block."), GetLastError());
        }

        BYTE* pComData =
            (BYTE*)VirtualAlloc(pData, 0x10000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

        if(!pComData)
        {
            VirtualFree(pData, 0, MEM_RELEASE);

            m_Allocations.DelLast();

            INITIATE_DEFINED_CODE_FAILURE
                (TEXT("Error committing 64Kb block of memory."), GetLastError());
        }

        assert(pComData == pData);

        size_t i;

        for(i = 0 ; i + ARRAY_SIZE(gs_Code)*2 <= 0x10000 ; i += ARRAY_SIZE(gs_Code))
            (void*&)pData[i] = (BYTE*)pData + i + ARRAY_SIZE(gs_Code);

        (void*&)pData[i] = NULL;

        m_pFirstFree = pData;
    }

    BYTE* pRet;

    pRet = (BYTE*)m_pFirstFree, m_pFirstFree = *(void**)m_pFirstFree;

    memcpy(pRet, gs_Code, ARRAY_SIZE(gs_Code));

    memcpy(pRet + RECALLER_OFFSET, &pRecaller, 4);

    int iDelta = (BYTE*)WS_StubProc - (pRet + STUB_PROC_OFFSET + 4);

    memcpy(pRet + STUB_PROC_OFFSET, &iDelta, 4);

    return (WNDPROC)pRet;
}

void TWindowSubclasserGlobals::Free(WNDPROC pProc)
{
    if(!IsInitialized())
        return;

    if(!pProc)
        return;

    TCriticalSectionLocker Locker0(m_AllocationCS);

    DEBUG_VERIFY(GetRecaller(pProc));

    *(void**)pProc = m_pFirstFree, m_pFirstFree = pProc;
}

T_WS_Recaller* TWindowSubclasserGlobals::GetRecaller(WNDPROC pProc) const
{
    DEBUG_VERIFY_INITIALIZATION;

    TCriticalSectionLocker Locker0(m_AllocationCS);

    for(size_t i = 0 ; i < m_Allocations.GetN() ; i++)
    {
        if( (BYTE*)pProc >= (BYTE*)m_Allocations[i] &&
            (BYTE*)pProc <  (BYTE*)m_Allocations[i] + 0x10000)
        {
            T_WS_Recaller* pRecaller;

            memcpy(&pRecaller, (BYTE*)pProc + RECALLER_OFFSET, sizeof(void*));

            return pRecaller;
        }
    }

    return NULL;
}

// ----------------
// Helper routines
// ----------------
T_WS_Recaller* _FindWS_Recaller(HWND                    hWnd,
                                const T_WS_Recaller&    Recaller,
                                T_WS_Recaller*          pAfter)
{
    if(!IsWindow(hWnd))
        return NULL;

    for(T_WS_Recaller* pRecaller =
            pAfter ?
                pAfter->m_pScanNext :
                (T_WS_Recaller*)GetProp
                    (hWnd, (LPCTSTR)g_WindowSubclasserGlobals.m_aFirstWS) ;
        pRecaller ;
        pRecaller = pRecaller->m_pScanNext)
    {
        if(pRecaller->Equals(Recaller))
            return pRecaller;
    }

    return NULL;
}

T_WS_Recaller* _SubclassWindow(HWND hWnd, TPtrHolder<T_WS_Recaller> pRecaller)
{
    DEBUG_VERIFY(IsWindow(hWnd));

    DEBUG_VERIFY(pRecaller);

    // Chain
    pRecaller->m_pNextWindowProc =
        (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);

    pRecaller->m_pChainNext =
        g_WindowSubclasserGlobals.GetRecaller(pRecaller->m_pNextWindowProc);

    if(pRecaller->m_pChainNext)
    {
        DEBUG_VERIFY(!pRecaller->m_pChainNext->m_pChainPrev);
        pRecaller->m_pChainNext->m_pChainPrev = pRecaller;
    }

    SetWindowLong(hWnd, GWL_WNDPROC, (LONG)pRecaller->m_pOwnWindowProc);

    // Scan
    pRecaller->m_pScanNext =
        (T_WS_Recaller*)GetProp(hWnd, (LPCTSTR)g_WindowSubclasserGlobals.m_aFirstWS);

    if(pRecaller->m_pScanNext)
        pRecaller->m_pScanNext->m_pScanPrev = pRecaller;

    SetProp(hWnd, (LPCTSTR)g_WindowSubclasserGlobals.m_aFirstWS, (HANDLE)pRecaller);

    return pRecaller.Extract();
}

void _UnsubclassWindow(HWND hWnd, T_WS_Recaller* pRecaller)
{
    if(!pRecaller)
        return;

    // Preparing rule flags
    bool bFirstOfAChain =
        !pRecaller->m_pChainPrev;

    bool bLastOfAChain =
        !pRecaller->m_pChainNext;

    bool bActualTop =
        (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC) == pRecaller->m_pOwnWindowProc;

    // Checking for deletion possibility
    if(bFirstOfAChain && !bActualTop)
    {
        pRecaller->m_bDeleted = true;

        return;
    }

    // Delinking
    if(pRecaller->m_pChainPrev)
        pRecaller->m_pChainPrev->m_pChainNext = pRecaller->m_pChainNext;

    if(pRecaller->m_pChainNext)
        pRecaller->m_pChainNext->m_pChainPrev = pRecaller->m_pChainPrev;

    // Updating prev's window proc
    if(pRecaller->m_pChainPrev)
        pRecaller->m_pChainPrev->m_pNextWindowProc = pRecaller->m_pNextWindowProc;

    // Updating actual top
    if(bActualTop)
        SetWindowLong(hWnd, GWL_WNDPROC, (LONG)pRecaller->m_pNextWindowProc);

    // Updating scan list
    if(pRecaller->m_pScanPrev)
    {
        pRecaller->m_pScanPrev->m_pScanNext = pRecaller->m_pScanNext;
    }
    else
    {
        SetProp(hWnd,
                (LPCTSTR)g_WindowSubclasserGlobals.m_aFirstWS,
                (HANDLE)pRecaller->m_pScanNext);
    }

    if(pRecaller->m_pScanNext)
        pRecaller->m_pScanNext->m_pScanPrev = pRecaller->m_pScanPrev;

    // Deleting
    delete pRecaller;
}
