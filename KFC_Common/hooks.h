#ifndef hooks_h
#define hooks_h

#include "dll.h"

#ifdef _MSC_VER

// ---------------
// Hook installer
// ---------------
class THookInstaller
{
private:
    bool m_bAllocated;

    HHOOK m_hHook;

public:
    THookInstaller();

    ~THookInstaller() { Release(); }

    bool IsAllocated() const
        { return m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(  int             iType,
                    const T_DLL&    DLL,
                    LPCSTR          pFunctionName,
                    DWORD           dwThreadID = 0);

    HHOOK GetHook() const;

    operator HHOOK () const { return GetHook(); }
};

#endif // _MSC_VER

#endif // hooks_h
