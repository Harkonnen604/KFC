#include "kfc_ktl_pch.h"
#include "mutex.h"

// ------
// Mutex
// ------
TMutex::TMutex()
{
    m_hMutex = NULL;
}

TMutex::TMutex(bool bInitialOwnership, LPCTSTR pName)
{
    m_hMutex = NULL;

    Create(bInitialOwnership, pName);
}

#ifdef _MSC_VER

TMutex::TMutex(LPCTSTR pName)
{
    m_hMutex = NULL;

    Open(pName);
}

#endif // _MSC_VER

void TMutex::Release()
{
    if(m_hMutex)
        CloseHandle(m_hMutex), m_hMutex = NULL;
}

bool TMutex::Create(bool bInitialOwnership, LPCTSTR pName)
{
    Release();

    try
    {
        m_hMutex = CreateMutex(NULL, bInitialOwnership, pName);

        if(!m_hMutex)
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error creating mutex"), GetLastError());

        return GetLastError() != ERROR_ALREADY_EXISTS;
    }

    catch(...)
    {
        Release();
        throw;
    }
}

#ifdef _MSC_VER

void TMutex::Open(LPCTSTR pName)
{
    Release();

    DEBUG_VERIFY(pName != NULL);

    try
    {
        m_hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, pName);
        if(m_hMutex == NULL)
        {
            INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error opening mutex:\r\n\"") +
                                                pName +
                                                TEXT("\"\r\n"),
                                            GetLastError());
        }
    }

    catch(...)
    {
        Release();
        throw;
    }
}

#endif // _MSC_VER
