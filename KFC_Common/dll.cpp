#include "kfc_common_pch.h"
#include "dll.h"

#ifdef _MSC_VER

// ----
// DLL
// ----
T_DLL::T_DLL()
{
    m_bAllocated = false;

    m_hModule = NULL;
}

T_DLL::T_DLL(LPCTSTR pFileName, kflags_t flFlags)
{
    m_bAllocated = false;

    m_hModule = NULL;

    Allocate(pFileName, flFlags);
}

T_DLL::T_DLL(HMODULE hSModule)
{
    m_bAllocated = false;

    m_hModule = NULL;

    Allocate(hSModule);
}

void T_DLL::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        if(m_hModule)
            FreeLibrary(m_hModule), m_hModule = NULL;
    }
}

void T_DLL::Allocate(LPCTSTR pFileName, kflags_t flFlags)
{
    Release();

    try
    {
        if(!(m_hModule = LoadLibraryEx(pFileName, NULL, flFlags)))
        {
            INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error loading DLL \"") +
                                                pFileName +
                                                TEXT("\""),
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

void T_DLL::Allocate(HMODULE hSModule)
{
    Release();

    try
    {
        DEBUG_VERIFY(hSModule != NULL);

        m_hModule = hSModule;

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

void T_DLL::ReOwn(T_DLL& DLL)
{
    Release();

    if(!DLL.IsAllocated())
        return;

    Allocate(DLL.GetModule());

    DLL.Invalidate();
}

void T_DLL::Invalidate()
{
    DEBUG_VERIFY_ALLOCATION;

    m_hModule = NULL;

    Release();
}

void* T_DLL::FindFunction(LPCSTR pFunctionName) const
{
    DEBUG_VERIFY_ALLOCATION;

    return GetProcAddress(m_hModule, pFunctionName);
}

void* T_DLL::GetFunction(LPCSTR pFunctionName) const
{
    DEBUG_VERIFY_ALLOCATION;

    void* pAddress = FindFunction(pFunctionName);

    if(!pAddress)
    {
        INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error retrieving \"") +
                                            pFunctionName +
                                            TEXT("\" from the DLL"),
                                        GetLastError());
    }

    return pAddress;
}

HMODULE T_DLL::GetModule() const
{
    DEBUG_VERIFY_ALLOCATION;

    return m_hModule;
}

#endif // _MSC_VER
