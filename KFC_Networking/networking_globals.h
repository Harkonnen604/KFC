#ifndef networking_globals_h
#define networking_globals_h

#include <KFC_KTL/module_globals.h>

// -------------------
// Networking globals
// -------------------
class TNetworkingGlobals : public TModuleGlobals
{
public:
	TNetworkingGlobals();
};

extern TNetworkingGlobals g_NetworkingGlobals;

#endif // networking_globals_h
