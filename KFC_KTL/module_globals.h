#ifndef module_globals_h
#define module_globals_h

#include "globals.h"

// ---------------
// Module globals
// ---------------
class TModuleGlobals : public TGlobals
{
private:
    void OnUninitialize () {}
    void OnInitialize   () {}

public:
    TModuleGlobals(LPCTSTR pSName) : TGlobals(pSName) {}
};

#endif // module_globals_h
