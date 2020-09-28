#ifndef networking_device_globals_h
#define networking_device_globals_h

#include <KFC_KTL/globals.h>

// --------------------------
// Networking device globals
// --------------------------
class TNetworkingDeviceGlobals : public TGlobals
{
private:
    #ifdef _MSC_VER
        bool m_bStarted;

        // WSA
        WSADATA m_WSAData;
    #endif // _MSC_VER

public:
    volatile LONG m_lNResolveThreads;

private:
    void OnUninitialize ();
    void OnInitialize   ();

public:
    TNetworkingDeviceGlobals();

    // ---------------- TRIVIALS ----------------
    #ifdef _MSC_VER
        const WSADATA& GetWSAData() const
            { return m_WSAData; }
    #endif // _MSC_VER
};

extern TNetworkingDeviceGlobals g_NetworkingDeviceGlobals;

#endif // networking_device_globals_h
