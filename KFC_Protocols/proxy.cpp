#include "kfc_protocols_pch.h"
#include "proxy.h"

#include "protocols_tls_item.h"

// -----------
// Proxy type
// -----------
bool FromString(const KString& Text, TProxyType& RType)
{
	const KString& TrimmedText = Text.Trimmed();

	if(TrimmedText == "None")
		return RType = PT_NONE, true;

	if(TrimmedText == "SOCKS4")
		return RType = PT_SOCKS4, true;

	if(TrimmedText == "SOCKS5")
		return RType = PT_SOCKS5, true;

	if(TrimmedText == "HTTP")
		return RType = PT_HTTP, true;

	if(TrimmedText == "HTTPS")
		return RType = PT_HTTPS, true;

	return false;
}

KString ToString(TProxyType Type)
{
	switch(Type)
	{
	case PT_NONE:
		return "None";

	case PT_SOCKS4:
		return "SOCKS4";

	case PT_SOCKS5:
		return "SOCKS5";

	case PT_HTTP:
		return "HTTP";

	case PT_HTTPS:
		return "HTTPS";

	default:
		INITIATE_FAILURE;
	}
}

// -------------
// Proxy setter
// -------------
TProxySetter::TProxySetter(TProxyType Type, DWORD dwIP, WORD wPort)
{
	TProtocolsTLS_Item& Item = TProtocolsTLS_Item::Get();

	m_OldType  = Item.m_ProxyType;
	m_dwOldIP  = Item.m_dwProxyIP;
	m_wOldPort = Item.m_wProxyPort;

	Item.SetProxy(Type, dwIP, wPort);
}

TProxySetter::~TProxySetter()
{
	TProtocolsTLS_Item::Get().SetProxy(m_OldType, m_dwOldIP, m_wOldPort);
}

// ----------------
// Global routines
// ----------------
TProxyType PerformProxyBypass(	TSocket&	Socket,
								DWORD		dwIP,
								WORD		wPort,
								TProxyType	ProxyType,
								DWORD		dwProxyIP,
								WORD		wProxyPort)
{
	DEBUG_VERIFY(Socket.IsAllocated());
	
	DEBUG_VERIFY(!Socket.IsConnected());

	if(ProxyType == PT_NONE || dwProxyIP == INADDR_NONE || !wProxyPort)
	{
		Socket.Connect(dwIP, wPort);

		return PT_NONE;
	}

	if(ProxyType == PT_HTTP)
	{
		TEST_BLOCK_BEGIN
		{
			Socket.Connect(dwProxyIP, wProxyPort);
		}
		TEST_BLOCK_KFC_EXCEPTION_HANDLER
		{
			INITIATE_DEFINED_FAILURE("Proxy connect failed.");
		}
		TEST_BLOCK_END

		return PT_HTTP;
	}

	if(ProxyType == PT_HTTPS)
	{
		TEST_BLOCK_BEGIN
		{
			Socket.Connect(dwProxyIP, wProxyPort);
		}
		TEST_BLOCK_KFC_EXCEPTION_HANDLER
		{
			INITIATE_DEFINED_FAILURE("Proxy connect failed.");
		}
		TEST_BLOCK_END

		char buf[128];

		sprintf(buf,
				"CONNECT %u.%u.%u.%u:%u HTTP/1.0\r\n\r\n",
					(UINT)((dwIP >> 24)),
					(UINT)((dwIP >> 16)	& 0xFF),
					(UINT)((dwIP >> 8)	& 0xFF),
					(UINT)((dwIP)		& 0xFF),
					wPort);

		Socket.Send(buf, strlen(buf));

		char lc[2] = {0, 0};

		for(;;)
		{
			char c;

			Socket >> c;

			if(	lc[1] == '\n' && c == '\n' ||
				lc[0] == '\n' && lc[1] == '\r' && c == '\n')
			{
				break;
			}

			lc[0] = lc[1], lc[1] = c;
		}

		return PT_HTTPS;
	}

	if(ProxyType == PT_SOCKS4)
	{
		TEST_BLOCK_BEGIN
		{
			Socket.Connect(dwProxyIP, wProxyPort);
		}
		TEST_BLOCK_KFC_EXCEPTION_HANDLER
		{
			INITIATE_DEFINED_FAILURE("Proxy connect failed.");
		}
		TEST_BLOCK_END

		BYTE buf[9] = {
			4, 1,
			(BYTE)(wPort>>8), (BYTE)(wPort&0xFF),
			(BYTE)(dwIP>>24), (BYTE)((dwIP>>16)&0xFF), (BYTE)((dwIP>>8)&0xFF), (BYTE)(dwIP&0xFF),
			0};

		Socket.Send(buf, 9);

		Socket.Receive(buf, 8);

		KFC_VERIFY("socks4" && buf[0] == 0x00 && buf[1] == 0x5A);

		return PT_SOCKS4;
	}

	if(ProxyType == PT_SOCKS5)
	{
		TEST_BLOCK_BEGIN
		{
			Socket.Connect(dwProxyIP, wProxyPort);
		}
		TEST_BLOCK_KFC_EXCEPTION_HANDLER
		{
			INITIATE_DEFINED_FAILURE("Proxy connect failed.");
		}
		TEST_BLOCK_END

		{
			BYTE buf[3] = {5, 1, 0};

			Socket.Send(buf, 3);

			Socket.Receive(buf, 2);

			KFC_VERIFY("socks5" && buf[0] == 0x05 && buf[1] == 0x00);
		}

		{
			BYTE buf[10] = {
				5, 1, 0, 1,
				(BYTE)(dwIP>>24), (BYTE)((dwIP>>16)&0xFF), (BYTE)((dwIP>>8)&0xFF), (BYTE)(dwIP&0xFF),
				(BYTE)(wPort>>8), (BYTE)(wPort&0xFF)};

			Socket.Send(buf, 10);

			Socket.Receive(buf, 10);

			KFC_VERIFY("socks5" && buf[0] == 0x05 && buf[1] == 0x00);
		}

		return PT_SOCKS5;
	}

	INITIATE_FAILURE;
}

TProxyType PerformProxyBypass(	TSocket&	Socket,
								DWORD		dwIP,
								WORD		wPort)
{
	const TProtocolsTLS_Item& TLS_Item = TProtocolsTLS_Item::Get();

	return PerformProxyBypass(	Socket,
								dwIP,
								wPort,
								TLS_Item.m_ProxyType,
								TLS_Item.m_dwProxyIP,
								TLS_Item.m_wProxyPort);
}
