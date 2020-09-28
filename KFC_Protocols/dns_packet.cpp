#include "kfc_protocols_pch.h"
#include "dns_packet.h"

#ifdef _MSC_VER

/* From RFC-1035 */

// Limits
#define MAX_UDP_DNS_DATAGRAM_LENGTH     (512)

// -------------------
// DNS [q]type values
// -------------------
#define DNS_TYPE_A          (1)
#define DNS_TYPE_NS         (2)
#define DNS_TYPE_MD         (3)
#define DNS_TYPE_MF         (4)
#define DNS_TYPE_CNAME      (5)
#define DNS_TYPE_SOA        (6)
#define DNS_TYPE_MB         (7)
#define DNS_TYPE_MG         (8)
#define DNS_TYPE_MR         (9)
#define DNS_TYPE_NULl       (10)
#define DNS_TYPE_WKS        (11)
#define DNS_TYPE_PTR        (12)
#define DNS_TYPE_HINFO      (13)
#define DNS_TYPE_MINFO      (14)
#define DNS_TYPE_MX         (15)
#define DNS_TYPE_TXT        (16)

#define DNS_QTYPE_AFXR      (252)
#define DNS_QTYPE_MAILB     (253)
#define DNS_QTYPE_MAILA     (254)
#define DNS_QTYPE_ANY       (255)

// --------------------
// DNS [q]class values
// --------------------
#define DNS_CLASS_IN    (1)
#define DNS_CLASS_CS    (2)
#define DNS_CLASS_CH    (3)
#define DNS_CLASS_HS    (4)

#define DNS_QCLASS_ANY  (255)

// ----------
// DNS flags
// ----------
#define DNS_FLAGS_QR                (0x8000)
#define DNS_FLAGS_GET_OPCODE(v)     (((v) >> 11) & 0xF)
#define DNS_FLAGS_SET_OPCODE(v)     ((v) << 11)
#define DNS_FLAGS_AA                (0x0400)
#define DNS_FLAGS_TC                (0x0200)
#define DNS_FLAGS_RD                (0x0100)
#define DNS_FLAGS_RA                (0x0080)
#define DNS_FLAGS_GET_Z(v)          (((v) >> 4) & 0x7)
#define DNS_FLAGS_SET_Z(v)          ((v) << 4)
#define DNS_FLAGS_GET_RCODE(v)      ((v) & 0xF)
#define DNS_FLAGS_SET_RCODE(v)      (v)

// ------------
// DNS opcodes
// ------------
#define DNS_OPCODE_QUERY    (0)
#define DNS_OPCODE_IQUERY   (1)
#define DNS_OPCODE_STATUS   (2)

// -----------
// DNS rcodes
// -----------
#define DNS_RCODE_NO_ERROR          (0)
#define DNS_RCODE_FORMAT_ERROR      (1)
#define DNS_RCODE_SERVER_FAILURE    (2)
#define DNS_RCODE_NAME_ERROR        (3)
#define DNS_RCODE_NOT_IMPLEMENTED   (4)
#define DNS_RCODE_REFUSED           (5)

// ---------------
// Recursive data
// ---------------
struct TRecursiveData
{
    const BYTE* p;
    size_t      n;

    LPSTR   m_Texts     [MAX_UDP_DNS_DATAGRAM_LENGTH];
    size_t  m_Lengths   [MAX_UDP_DNS_DATAGRAM_LENGTH];


    TRecursiveData(const BYTE* sp, size_t sn);

    ~TRecursiveData();
};

TRecursiveData::TRecursiveData(const BYTE* sp, size_t sn) : p(sp), n(sn)
{
    memset(m_Texts, 0, sizeof(m_Texts));

    memset(m_Lengths, 0, sizeof(m_Lengths));
}

TRecursiveData::~TRecursiveData()
{
    size_t i;

    for(i = n - 1 ; i != UINT_MAX ; i--)
        delete[] m_Texts[i];
}

// ----------------
// Helper routines
// ----------------
template <class t>
static inline bool ReadVar(const BYTE*& p, size_t& n, t& v)
{
    if(n < sizeof(v))
        return false;

    memcpy(&v, p, sizeof(v));

#pragma warning( push )
#pragma warning( disable : 4244)

    if(sizeof(v) == sizeof(WORD))
        v = ntohs(v);
    else if(sizeof(v) == sizeof(DWORD))
        v = ntohl(v);

#pragma warning( pop )

    p += sizeof(v), n -= sizeof(v);

    return true;
}

static bool ReadDomainNameRecursive(size_t          szPos,
                                    TRecursiveData& RecData)
{
    if(szPos >= RecData.n) // invalid offset
        return false;

    if(RecData.m_Lengths[szPos] == UINT_MAX) // loop detected
        return false;

    if(RecData.m_Texts[szPos]) // known text
        return true;

    RecData.m_Lengths[szPos] = UINT_MAX; // marking node as visited

    size_t szLength = 0;

    size_t szPos2;

    const BYTE* p = RecData.p + szPos;

    size_t n = RecData.n - szPos;

    KString Text;

    BYTE l;

    if(!ReadVar(p, n, l))
        return false;

    szLength++;

    if(l)
    {
        switch(l >> 6)
        {
        case 0: // label
            {
                if(n < l)
                    return false;

                Text.Extend((LPCSTR)p, l);

                Text += '.';

                p += l, n -= l, szLength += l;

                szPos2 = szPos + szLength;

                if(!ReadDomainNameRecursive(szPos2, RecData))
                    return false;

                Text += RecData.m_Texts[szPos2];

                szLength += RecData.m_Lengths[szPos2];
            }

            break;

        case 3: // pointer
            {
                BYTE l2;

                if(!ReadVar(p, n, l2))
                    return false;

                szLength++;

                szPos2 = ((l & 63) << 8) | l2;

                if(!ReadDomainNameRecursive(szPos2, RecData))
                    return false;

                Text = RecData.m_Texts[szPos2];

                break;
            }

        default: // reserved
            return false;
        }
    }

    if(!Text.IsEmpty() && Text[Text.GetLength() - 1] == TEXT('.'))
        Text.SetLeft(Text.GetLength() - 1);

    memcpy( RecData.m_Texts[szPos] = new char[Text.GetLength() + 1],
            Text,
            Text.GetLength() + 1);

    RecData.m_Lengths[szPos] = szLength;

    return true;
}

static bool ReadDomainName( const BYTE*&    p,
                            size_t&         n,
                            TRecursiveData& RecData,
                            KString*        pRText = NULL)
{
    const size_t szPos = p - RecData.p;

    if(!ReadDomainNameRecursive(szPos, RecData))
        return false;

    const LPCSTR pText = RecData.m_Texts[szPos];

    const size_t szLength = RecData.m_Lengths[szPos];

    p += szLength, n -= szLength;

    if(!CheckDomainName(pText))
        return false;

    if(pRText)
        *pRText = pText;

    return true;
}

static bool ReadSection(const BYTE*&            p,
                        size_t&                 n,
                        size_t                  nrec,
                        TRecursiveData&         RecData,
                        TArray<DWORD, true>*    pRIPs = NULL)
{
    size_t i;

    for(i = nrec ; i ; i--)
    {
        if(!ReadDomainName(p, n, RecData))
            return false;

        WORD    wType;
        WORD    wClass;
        DWORD   dwTTL;
        WORD    wRDLength;

        if( !ReadVar(p, n, wType)   ||
            !ReadVar(p, n, wClass)  ||
            !ReadVar(p, n, dwTTL)   ||
            !ReadVar(p, n, wRDLength))
        {
            return false;
        }

        if((int)dwTTL < 0)
            return false;

        if(n < wRDLength)
            return false;

        if(pRIPs)
        {
            if( wType   == DNS_TYPE_A   &&
                wClass  == DNS_CLASS_IN &&
                dwTTL > 0)
            {
                DWORD dwIP;

                if(wRDLength == sizeof(dwIP))
                {
                    memcpy(&dwIP, p, sizeof(dwIP)), dwIP = ntohl(dwIP);

                    if(dwIP != DWORD_MAX)
                        pRIPs->Add() = dwIP;
                }
            }
        }

        p += wRDLength, n -= wRDLength;
    }

    return true;
}

// ----------------
// Global routines
// ----------------
bool CheckDomainNameLabel(LPCSTR s, size_t l)
{
    if(l == UINT_MAX)
        l = strlen(s);

    if(l == 0 || l > 63) // length
        return false;

    if(!isalpha(s[0])) // 1st must be alpha
        return false;

    size_t i;

    for(i = 0 ; i < l ; i++)
    {
        if(!isalnum(s[i]) && s[i] != '-') // non-1st must be alpha-digit-hypen
            return false;
    }

    if(s[l-1] == '-') // last can't be hypen
        return false;

    return true;
}

bool CheckDomainName(LPCSTR s, size_t l)
{
    if(l == UINT_MAX)
        l = strlen(s);

    if(l == 0 || l > 255) // length
        return false;

    size_t i, j = 0;

    for(i = 0 ; i <= l ; i++)
    {
        if(i ==l || s[i] == '.')
        {
            if(!CheckDomainNameLabel(s + j, i - j))
                return false;

            j = i + 1;
        }
    }

    return true;
}

bool ParseStandardDNS_UDPPacket(TDNSPacketType          dptExpectedType,
                                const void*             pData,
                                size_t                  szLength,
                                KStrings&               RRequests,
                                TArray<DWORD, true>&    RResponses,
                                bool                    bClearFirst,
                                KStrings::TIterator*    pRRequestsRet,
                                size_t*                 pRResponsesRet)
{
    DEBUG_VERIFY(szLength != UINT_MAX);

    if(szLength > MAX_UDP_DNS_DATAGRAM_LENGTH)
        return false;

    if(bClearFirst)
    {
        RRequests.Clear();

        RResponses.Clear();
    }

    const KStrings::TIterator RequestsRet = RRequests.GetLast();

    if(pRResponsesRet)
        *pRResponsesRet = RResponses.GetN();

    size_t i;

    KString Text;

    const BYTE* p = (const BYTE*)pData;

    size_t n = szLength;

    // Header
#pragma pack (1)

    struct THeader
    {
        WORD m_wID;         // ID
        WORD m_wFlags;      // Flags
        WORD m_wQDCount;    // Number of RRs in question   section
        WORD m_wANCount;    // Number of RRs in answer     section
        WORD m_wNSCount;    // Number of RRs in authority  section
        WORD m_wARCount;    // Number of RRs in additional section


        void ntoh()
        {
            m_wID       = ntohs(m_wID);
            m_wFlags    = ntohs(m_wFlags);
            m_wQDCount  = ntohs(m_wQDCount);
            m_wANCount  = ntohs(m_wANCount);
            m_wNSCount  = ntohs(m_wNSCount);
            m_wARCount  = ntohs(m_wARCount);
        }

    }Header;

#pragma pack()

    // Getting header
    {
        if(n < sizeof(Header))
            return false;

        memcpy(&Header, p, sizeof(Header)), Header.ntoh();

        p += sizeof(Header), n -= sizeof(Header);
    }

    // Checking flags
    {
        // Checking packet type (request/response)
        if( dptExpectedType != DPT_ANY &&
            dptExpectedType != ((Header.m_wFlags & DNS_FLAGS_QR) ? DPT_RESPONSE : DPT_REQUEST))
        {
            return false;
        }

        // Checking opcode (must be standard query)
        if(DNS_FLAGS_GET_OPCODE(Header.m_wFlags) != DNS_OPCODE_QUERY)
            return false;

        // Checking for no truncation
        if(Header.m_wFlags & DNS_FLAGS_TC)
            return false;

        // Checking reserved Z field (must be zero)
        if(DNS_FLAGS_GET_Z(Header.m_wFlags))
            return false;

        // Checking rcode (must be 'no error')
        if(DNS_FLAGS_GET_RCODE(Header.m_wFlags) != DNS_RCODE_NO_ERROR)
            return false;
    }

    TRecursiveData RecData((const BYTE*)pData, szLength);

    // Reading questions section
    for(i = Header.m_wQDCount ; i ; i--)
    {
        if(!ReadDomainName(p, n, RecData, &Text))
            break;

        WORD wType;
        WORD wClass;

        if( !ReadVar(p, n, wType) ||
            !ReadVar(p, n, wClass))
        {
            return false;
        }

        if( (wType  == DNS_QTYPE_ANY  || wType  == DNS_TYPE_A) &&
            (wClass == DNS_QCLASS_ANY || wClass == DNS_CLASS_IN))
        {
            *RRequests.AddLast() = Text;
        }
    }

    // Reading answers section
    if(!ReadSection(p, n, Header.m_wANCount, RecData, &RResponses))
        return false;

    // Reading authority section
    if(!ReadSection(p, n, Header.m_wNSCount, RecData))
        return false;

    // Reading additional section
    if(!ReadSection(p, n, Header.m_wARCount, RecData))
        return false;

    if(pRRequestsRet)
        *pRRequestsRet = RequestsRet.IsValid() ? RequestsRet.GetNext() : RRequests.GetFirst();

    return true;
}

#endif // _MSC_VER
