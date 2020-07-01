#ifndef physics_globals_h
#define physics_globals_h

#include <KFC_KTL\module_globals.h>

// ----------------
// Physics globals
// ----------------
class TPhysicsGlobals : public TModuleGlobals
{
public:
	TPhysicsGlobals();
};

extern TPhysicsGlobals g_PhysicsGlobals;

#endif // physics_globals_h