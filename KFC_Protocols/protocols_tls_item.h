#ifndef protocols_tls_item_h
#define protocols_tls_item_h

#include <KFC_KTL/tls_storage.h>
#include "proxy.h"

// -------------------
// Protocols TLS item
// -------------------
struct TProtocolsTLS_Item : public T_TLS_Storage::TItem
{
private:
	static size_t ms_szIndex;

private:
	static T_TLS_Storage::TItem* Creator()
		{ return new TProtocolsTLS_Item; }

public:
	TProxyType	m_ProxyType;
	DWORD		m_dwProxyIP;
	WORD		m_wProxyPort;

public:
	static void Register()
	{
		if(ms_szIndex == UINT_MAX)
			ms_szIndex = g_TLS_Storage.ReserveItemType(Creator);
	}

	static void Free()
	{
		if(ms_szIndex != UINT_MAX)
			g_TLS_Storage.FreeItemType(ms_szIndex);
	}

public:
	static TProtocolsTLS_Item& Get()
		{ return (TProtocolsTLS_Item&)g_TLS_Storage[ms_szIndex]; }

public:
	TProtocolsTLS_Item()
	{
		m_ProxyType		= PT_NONE;
		m_dwProxyIP		= INADDR_NONE;
		m_wProxyPort	= 0;
	}	

	TProxyType SetProxy(TProxyType	Type,
						DWORD		dwIP,
						WORD		wPort)
	{
		if(Type != PT_NONE && dwIP != INADDR_NONE && wPort)
		{
			m_ProxyType		= Type;
			m_dwProxyIP		= dwIP;
			m_wProxyPort	= wPort;
		}
		else
		{
			m_ProxyType		= PT_NONE;
			m_dwProxyIP		= INADDR_NONE;
			m_wProxyPort	= 0;
		}

		return m_ProxyType;
	}
};

#endif // protocols_tls_item_h
