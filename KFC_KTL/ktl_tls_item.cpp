#include "kfc_ktl_pch.h"
#include "ktl_tls_item.h"

// -------------
// KTL TLS item
// -------------
size_t T_KTL_TLS_Item::ms_szIndex = -1;

void T_KTL_TLS_Item::FreeItemType()
{
	if(ms_szIndex != -1)
		g_TLS_Storage.FreeItemType(ms_szIndex);
}

void T_KTL_TLS_Item::ReserveItemType()
{
	assert(ms_szIndex == -1);

	ms_szIndex = g_TLS_Storage.ReserveItemType(Creator);
}

T_KTL_TLS_Item::T_KTL_TLS_Item()
{
	m_szOuterBlockDepth = 0;
}
