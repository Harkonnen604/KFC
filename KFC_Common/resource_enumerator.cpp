#include "kfc_common_pch.h"
#include "resource_enumerator.h"

#ifdef _MSC_VER

// --------------------
// Resource enumerator
// --------------------
TResourceEnumerator::TResourceEnumerator(   HMODULE hModule,
                                            LPCTSTR pType)
{
    DEBUG_VERIFY(pType);

    if( !EnumResourceNames(hModule, pType, StaticEnumProc, (LPARAM)this)    &&
            GetLastError() != NO_ERROR                                      &&
            GetLastError() != ERROR_RESOURCE_TYPE_NOT_FOUND)
    {
        INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error enumerating resources"),
                                        GetLastError());
    }

    m_szPos = 0;
}

BOOL CALLBACK TResourceEnumerator::StaticEnumProc(  HMODULE hModule,
                                                    LPCTSTR pType,
                                                    LPTSTR  pName,
                                                    LPARAM  lParam)
{
    DEBUG_VERIFY(lParam);

    ((TResourceEnumerator*)lParam)->m_IDs.Add() = (size_t)pName;

    return TRUE;
}

#endif // _MSC_VER
