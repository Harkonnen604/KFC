#ifndef common_globals_h
#define common_globals_h

#include <KFC_KTL/module_globals.h>

// ---------------
// Common globals
// ---------------
class TCommonGlobals : public TModuleGlobals
{
public:
	TCommonGlobals();
};

extern TCommonGlobals g_CommonGlobals;

#endif // common_globals_h
