#include "kfc_protocols_pch.h"
#include "ip_headers.h"

// ----------------
// Global routines
// ----------------
LPCTSTR GetIPProtocolName(BYTE bProtocol)
{
	switch(bProtocol)
	{
		case IPPROTO_IP:
			return TEXT("ip");

		case IPPROTO_ICMP:
			return TEXT("icmp");
		
		case IPPROTO_TCP:
			return TEXT("tcp");
		
		case IPPROTO_UDP:
			return TEXT("udp");

		default:
			return TEXT("unknown");
	}
}
