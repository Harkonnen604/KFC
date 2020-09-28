#include "kfc_common_pch.h"
#include "atom.h"

#ifdef _MSC_VER

// ----------------
// Global routines
// ----------------
bool KGetAtomName(ATOM aAtom, KString& RName)
{
    DEBUG_VERIFY(aAtom);

    TCHAR Buf[1024];
    Buf[ARRAY_SIZE(Buf) - 1] = 0;

    if(!GetAtomName(aAtom, Buf, ARRAY_SIZE(Buf) - 1))
        return false;

    RName = Buf;

    return true;
}

bool KGlobalGetAtomName(ATOM aAtom, KString& RName)
{
    DEBUG_VERIFY(aAtom);

    TCHAR Buf[1024];
    Buf[ARRAY_SIZE(Buf) - 1] = 0;

    if(!GlobalGetAtomName(aAtom, Buf, ARRAY_SIZE(Buf) - 1))
        return false;

    RName = Buf;

    return true;
}

#endif // _MSC_VER
