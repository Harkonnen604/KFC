#include "kfc_common_pch.h"
#include "module.h"

#include "startup_globals.h"

// ----------------
// Global routines
// ----------------

#ifdef _MSC_VER

HMODULE GetKModuleHandle(LPCTSTR pName)
{
    const HMODULE hModule = GetModuleHandle(pName);

    if(hModule == NULL)
    {
        INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error getting module handle for \"") +
                                            NullString(pName) +
                                            TEXT("\""),
                                        GetLastError());
    }

    return hModule;
}

#endif // _MSC_VER

KString GetKModuleFileName(HMODULE hModule)
{
    #ifdef _MSC_VER
    {
        TCHAR Buffer[1024];

        if(!GetModuleFileName(hModule, Buffer, sizeof(Buffer) - 1))
        {
            INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error getting module filename"),
                                            GetLastError());
        }

        return Buffer;
    }
    #else // _MSC_VER
    {
        assert(!hModule);

        assert(g_StartupGlobals.IsInitialized());

        return g_StartupGlobals.m_StartFile;
    }
    #endif // _MSC_VER
}
