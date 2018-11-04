#ifndef math_device_globals_h
#define math_device_globals_h

#include <KFC_KTL\globals.h>

// --------------------
// Math device globals
// --------------------
class TMathDeviceGlobals : public TGlobals
{
private:
	void OnUninitialize	();
	void OnInitialize	();	

public:
	TMathDeviceGlobals();
};

extern TMathDeviceGlobals g_MathDeviceGlobals;

#endif // math_device_globals_h