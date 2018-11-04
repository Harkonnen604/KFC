#ifndef common_tls_item_h
#define common_tls_item_h

#include <KFC_KTL/tls_storage.h>
#include "message_loop.h"

// ----------------
// Common TLS item
// ----------------
struct TCommonTLS_Item : public T_TLS_Storage::TItem
{
private:
	static size_t ms_szIndex;

private:
	static T_TLS_Storage::TItem* Creator()
		{ return new TCommonTLS_Item; }

public:
	#ifdef _MSC_VER
		TAuxMessages* m_pAuxMessages;
	#endif // _MSC_VER

public:
	static void FreeItemType();

	static void ReserveItemType();

	static TCommonTLS_Item& Get()
		{ return (TCommonTLS_Item&)g_TLS_Storage[ms_szIndex]; }

public:
	TCommonTLS_Item();
};

#ifdef _MSC_VER

// --------------------
// Aux messages setter
// --------------------
class TAuxMessagesSetter
{
private:
	TAuxMessages* m_pOldAuxMessages;

public:
	TAuxMessagesSetter(TAuxMessages& AuxMessages)
	{
		TCommonTLS_Item& Item = TCommonTLS_Item::Get();

		m_pOldAuxMessages = Item.m_pAuxMessages;

		Item.m_pAuxMessages = &AuxMessages;
	}

	~TAuxMessagesSetter()
		{ TCommonTLS_Item::Get().m_pAuxMessages = m_pOldAuxMessages; }
};

#endif // _MSC_VER

#endif // common_tls_item_h
