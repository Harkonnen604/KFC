#include "kfc_common_pch.h"
#include "common_tls_item.h"

// ----------------
// Common TLS item
// ----------------
size_t TCommonTLS_Item::ms_szIndex = UINT_MAX;

void TCommonTLS_Item::FreeItemType()
{
	if(ms_szIndex != UINT_MAX)
		g_TLS_Storage.FreeItemType(ms_szIndex);
}

void TCommonTLS_Item::ReserveItemType()
{
	assert(ms_szIndex == UINT_MAX);

	ms_szIndex = g_TLS_Storage.ReserveItemType(Creator);
}

TCommonTLS_Item::TCommonTLS_Item()
{
	#ifdef _MSC_VER
		m_pAuxMessages = NULL;
	#endif // _MSC_VER
}
