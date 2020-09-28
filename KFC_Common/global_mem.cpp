#include "kfc_common_pch.h"
#include "global_mem.h"

#ifdef _MSC_VER

// -----------
// Global mem
// -----------
TGlobalMem::TGlobalMem()
{
    Invalidate();
}

TGlobalMem::TGlobalMem(size_t szSize, kflags_t flFlags)
{
    Invalidate();

    Allocate(szSize, flFlags);
}

void TGlobalMem::Invalidate()
{
    m_bAllocated = false;

    m_hMem = NULL;
}

void TGlobalMem::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        if(m_hMem)
            GlobalFree(m_hMem), m_hMem = NULL;
    }
}

void TGlobalMem::Allocate(size_t szSize, kflags_t flFlags)
{
    Release();

    try
    {
        m_hMem = GlobalAlloc(flFlags, szSize);
        if(m_hMem == NULL)
        {
            INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error allocating global mem"),
                                            GetLastError());
        }

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

void TGlobalMem::ReOwn(TGlobalMem& SGlobalMem)
{
    Release();

    if(!SGlobalMem.IsAllocated())
        return;

    m_hMem = SGlobalMem.m_hMem;

    SGlobalMem.Invalidate();

    m_bAllocated = true;
}

void TGlobalMem::Lock(void*& pRData)
{
    DEBUG_VERIFY_ALLOCATION;

    pRData = GlobalLock(m_hMem);
    if(pRData == NULL)
    {
        INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error locking global mem"),
                                        GetLastError());
    }
}

void TGlobalMem::Unlock()
{
    DEBUG_VERIFY_ALLOCATION;

    GlobalUnlock(m_hMem);
}

// ------------------
// Global mem locker
// ------------------
TGlobalMemLocker::TGlobalMemLocker(TGlobalMem& SGlobalMem, void*& pRData)
{
    DEBUG_VERIFY(SGlobalMem.IsAllocated());

    (m_pGlobalMem = &SGlobalMem)->Lock(pRData);
}

TGlobalMemLocker::~TGlobalMemLocker()
{
    if(m_pGlobalMem && m_pGlobalMem->IsAllocated())
        m_pGlobalMem->Unlock();
}

#endif // _MSC_VER
