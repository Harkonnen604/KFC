#ifndef ip_headers_h
#define ip_headers_h

#pragma pack(1)

// ----------
// IP header
// ----------
struct TIPHeader
{
	BYTE	m_bVIHL;		// Version and IHL
	BYTE	m_bTOS;			// Type of service
	WORD	m_wTotLen;		// Total length
	WORD	m_wID;			// Identification
	WORD	m_wFlagOff;		// Flags and fragment offset
	BYTE	m_bTTL;			// Time to live
	BYTE	m_bProtocol;	// Protocol
	WORD	m_wChecksum;	// Checksum
	DWORD	m_dwSrcIP;		// Source IP
	DWORD	m_dwDstIP;		// Destination IP
};

// -----------
// TCP header
// -----------
struct TTCPHeader
{
	WORD	m_wSrcPort;		// Source port
	WORD	m_wDstPort;		// Destination port
	DWORD	m_dwSeq;		// Sequence number
	DWORD	m_dwAck;		// Acknowledgement number
	WORD	m_wDORF;		// Data offset, reserved bits and flags
	WORD	m_wWindow;		// Window
	WORD	m_wChecksum;	// Checksum
	WORD	m_wUPTR;		// Urgent pointer
	DWORD	m_dwOptions;	// Options and padding
};

// -----------
// UDP header
// -----------
struct TUDPHeader
{
	WORD m_wSrcPort;	// Source port
	WORD m_wDstPort;	// Destination port
	WORD m_wLength;		// Length
	WORD m_wChecksum;	// Checksum
};

#pragma pack()

// ----------------
// Global routines
// ----------------
LPCTSTR GetIPProtocolName(BYTE bProtocol);

#endif // ip_headers_h
