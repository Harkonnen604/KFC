#include "kfc_common_pch.h"
#include "common_tls_item.h"

// ----------------
// Common TLS item
// ----------------
size_t TCommonTLS_Item::ms_szIndex = -1;

void TCommonTLS_Item::FreeItemType()
{
	if(ms_szIndex != -1)
		g_TLS_Storage.FreeItemType(ms_szIndex);
}

void TCommonTLS_Item::ReserveItemType()
{
	assert(ms_szIndex == -1);

	ms_szIndex = g_TLS_Storage.ReserveItemType(Creator);
}

TCommonTLS_Item::TCommonTLS_Item()
{
	#ifdef _MSC_VER
		m_pAuxMessages = NULL;
	#endif // _MSC_VER
}
