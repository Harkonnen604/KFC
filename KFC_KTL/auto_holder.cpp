#include "kfc_ktl_pch.h"
#include "auto_holder.h"

#include "ktl_device_globals.h"

TAutoHolderGlobals g_AutoHolderGlobals;

// --------------------
// Auto holder globals
// --------------------
TAutoHolderGlobals::TAutoHolderGlobals() : TGlobals(TEXT("Auto holder globals"))
{
    AddSubGlobals(g_KTL_DeviceGlobals);
}

void TAutoHolderGlobals::OnUninitialize()
{
    assert(!(m_PtrHeap. IsAllocated() && !m_PtrHeap. IsEmpty()));
    assert(!(m_IterHeap.IsAllocated() && !m_IterHeap.IsEmpty()));

    m_IterHeap.Release();

    m_PtrHeap.Release();
}

void TAutoHolderGlobals::OnInitialize()
{
    m_PtrHeap.Allocate(1024);

    m_IterHeap.Allocate(1024);
}
