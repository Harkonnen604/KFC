#include "kfc_protocols_pch.h"
#include "protocols_device_globals.h"

#include "protocols_tls_item.h"

TProtocolsDeviceGlobals g_ProtocolsDeviceGlobals;

// -------------------------
// Protocols device globals
// -------------------------
TProtocolsDeviceGlobals::TProtocolsDeviceGlobals() : TGlobals(TEXT("Protocols device globals"))
{
}

void TProtocolsDeviceGlobals::OnUninitialize()
{
    TProtocolsTLS_Item::Free();
}

void TProtocolsDeviceGlobals::OnInitialize()
{
    TProtocolsTLS_Item::Register();
}
