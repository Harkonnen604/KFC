#ifndef ktl_tls_item_h
#define ktl_tls_item_h

#include "tls_storage.h"

// -------------
// KTL TLS item
// -------------
struct T_KTL_TLS_Item : public T_TLS_Storage::TItem
{
private:
	static size_t ms_szIndex;

private:
	static T_TLS_Storage::TItem* Creator()
		{ return new T_KTL_TLS_Item; }

public:
	KString m_ErrorText;

	size_t m_szOuterBlockDepth;

public:
	static void FreeItemType();

	static void ReserveItemType();

	static T_KTL_TLS_Item& Get()
		{ return (T_KTL_TLS_Item&)g_TLS_Storage[ms_szIndex]; }

public:
	T_KTL_TLS_Item();
};

#endif // ktl_tls_item_h
