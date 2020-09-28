#ifndef interface_globals_h
#define interface_globals_h

#include <KFC_KTL\module_globals.h>

// ------------------
// Interface globals
// ------------------
class TInterfaceGlobals : public TModuleGlobals
{
public:
    TInterfaceGlobals();
};

extern TInterfaceGlobals g_InterfaceGlobals;

#endif // interface_globals_h
