#ifndef physics_device_globals_h
#define physics_device_globals_h

#include <KFC_KTL\globals.h>

// -----------------------
// Physics device globals
// -----------------------
class TPhysicsDeviceGlobals : public TGlobals
{
private:
    void OnUninitialize ();
    void OnInitialize   ();

public:
    TPhysicsDeviceGlobals();
};

extern TPhysicsDeviceGlobals g_PhysicsDeviceGlobals;

#endif // physics_device_globals_h
