#ifndef protocols_device_globals_h
#define protocols_device_globals_h

#include <KFC_KTL/globals.h>

// -------------------------
// Protocols device globals
// -------------------------
class TProtocolsDeviceGlobals : public TGlobals
{
private:
	void OnUninitialize	();
	void OnInitialize	();

public:
	TProtocolsDeviceGlobals();
};

extern TProtocolsDeviceGlobals g_ProtocolsDeviceGlobals;

#endif // protocols_device_globals_h
