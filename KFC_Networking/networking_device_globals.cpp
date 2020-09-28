#include "kfc_networking_pch.h"
#include "networking_device_globals.h"

#include <KFC_KTL/process.h>
#include "networking_consts.h"

TNetworkingDeviceGlobals g_NetworkingDeviceGlobals;

// --------------------------
// Networking device globals
// --------------------------
TNetworkingDeviceGlobals::TNetworkingDeviceGlobals() : TGlobals(TEXT("Networking device globals"))
{
    // WSA
    #ifdef _MSC_VER
        m_bStarted = false;
    #endif // _MSC_VER

    m_lNResolveThreads = 0;
}

void TNetworkingDeviceGlobals::OnUninitialize()
{
    WaitGuardedThreads(m_lNResolveThreads);

    // WSA
    #ifdef _MSC_VER
    {
        if(m_bStarted)
            WSACleanup(), m_bStarted = false;
    }
    #endif // _MSC_VER
}

void TNetworkingDeviceGlobals::OnInitialize()
{
    // WSA
    #ifdef _MSC_VER
    {
        SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if(VALID_SOCKET(s))
        {
            closesocket(s);
            return;
        }

        if(WSAStartup(MAKEWORD(2, 0), &m_WSAData))
            INITIATE_DEFINED_FAILURE(TEXT("Error initializing WinSock (2.0)."));

        m_bStarted = true;
    }
    #endif // _MSC_VER
}
