#include "kfc_ktl_pch.h"
#include "termination.h"

// ----------------
// Global routines
// ----------------
bool IsTerminated(HANDLE hTerminator, size_t szTimeout)
{
    if(hTerminator)
        return WaitForSingleObject(hTerminator, (DWORD)szTimeout) == WAIT_OBJECT_0;

    if(szTimeout > 0)
        Sleep((DWORD)szTimeout);

    return false;
}
