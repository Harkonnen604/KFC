#ifndef hooks_globals_h
#define hooks_globals_h

#include <KFC_KTL\module_globals.h>

// --------------
// Hooks globals
// --------------
class THooksGlobals : public TModuleGlobals
{
public:
    THooksGlobals();
};

extern THooksGlobals g_HooksGlobals;

#endif // hooks_globals_h
