#ifndef ssl_device_globals_h
#define ssl_device_globals_h

#include <KFC_KTL\globals.h>

// -------------------
// SSL device globals
// -------------------
class T_SSL_DeviceGlobals : public TGlobals
{
private:
    void OnUninitialize ();
    void OnInitialize   ();

public:
    SSL_CTX* m_pServer_SSL_Context;
    SSL_CTX* m_pClient_SSL_Context;

    T_SSL_DeviceGlobals();
};

extern T_SSL_DeviceGlobals g_SSL_DeviceGlobals;

#endif // ssl_device_globals_h
