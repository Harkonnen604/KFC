#include "kfc_ktl_pch.h"
#include "semaphore.h"

// ----------
// Semaphore
// ----------
TSemaphore::TSemaphore()
{
    m_hSemaphore = NULL;
}

TSemaphore::TSemaphore(size_t szInitialCount, size_t szMaximumCount)
{
    m_hSemaphore = NULL;

    Allocate(szInitialCount, szMaximumCount);
}

TSemaphore::TSemaphore(LPCTSTR pName, size_t szInitialCount, size_t szMaximumCount)
{
    m_hSemaphore = NULL;

    Create(pName, szInitialCount, szMaximumCount);
}

#ifdef _MSC_VER

TSemaphore::TSemaphore(LPCTSTR pName)
{
    m_hSemaphore = NULL;

    Open(pName);
}

#endif // _MSC_VER

void TSemaphore::Release()
{
    if(m_hSemaphore)
        CloseHandle(m_hSemaphore), m_hSemaphore = NULL;
}

void TSemaphore::Allocate(size_t szInitialCount, size_t szMaximumCount)
{
    Release();

    DEBUG_VERIFY(szInitialCount <= szMaximumCount);

    if(!(m_hSemaphore = CreateSemaphore(NULL, (DWORD)szInitialCount, (DWORD)szMaximumCount, NULL)))
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error creating semaphore"), GetLastError());
}

bool TSemaphore::Create(LPCTSTR pName, size_t szInitialCount, size_t szMaximumCount)
{
    Release();

    DEBUG_VERIFY(pName);

    DEBUG_VERIFY(szInitialCount < szMaximumCount);

    if(!(m_hSemaphore = CreateSemaphore(NULL, (DWORD)szInitialCount, (DWORD)szMaximumCount, pName)))
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error creating named semaphore"), GetLastError());

    return GetLastError() != ERROR_ALREADY_EXISTS;
}

#ifdef _MSC_VER

void TSemaphore::Open(LPCTSTR pName)
{
    Release();

    DEBUG_VERIFY(pName);

    if(!(m_hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, pName)))
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error opening named semaphore"), GetLastError());
}

#endif // _MSC_VER

bool TSemaphore::WaitWithTermination(HANDLE hTerminator, size_t szTimeout)
{
    DEBUG_VERIFY_ALLOCATION;

    if(!hTerminator)
        return Wait(szTimeout);

    const HANDLE Handles[2] = {m_hSemaphore, hTerminator};

    DWORD r = WaitForMultipleObjects(ARRAY_SIZE(Handles), Handles, FALSE, (DWORD)szTimeout);

    if(r == WAIT_OBJECT_0 + 0)
        return true;

    if(r == WAIT_OBJECT_0 + 1)
        throw TTerminationException(__FILE__, __LINE__);

    return false;
}
