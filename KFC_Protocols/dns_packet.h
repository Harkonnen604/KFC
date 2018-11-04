#ifndef dns_packet_h
#define dns_packet_h

#ifdef _MSC_VER

// Ports
#define DNS_UDP_PORT	(53)

// ----------------
// DNS packet type
// ----------------
enum TDNSPacketType
{
	DPT_ANY			= 0,
	DPT_REQUEST		= 1,
	DPT_RESPONSE	= 2,	
	DPT_FORCE_UINT	= UINT_MAX
};

// ----------------
// Global routines
// ----------------
bool CheckDomainNameLabel(LPCSTR s, size_t l = UINT_MAX);

bool CheckDomainName(LPCSTR s, size_t l = UINT_MAX);

bool ParseStandardDNS_UDPPacket(TDNSPacketType			dptExpectedType,
								const void*				pData,
								size_t					szLength,
								KStrings&				RRequests,	// domain names
								TArray<DWORD, true>&	RResponses,	// hosts IP addresses
								bool					bClearFirst		= true,
								KStrings::TIterator*	pRRequestsRet	= NULL,
								size_t*					pRResponsesRet	= NULL);

#endif // _MSC_VER

#endif // dns_packet_h
