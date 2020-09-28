#ifndef proxy_h
#define proxy_h

#include <KFC_Networking/socket.h>

// -----------
// Proxy type
// -----------
enum TProxyType
{
    PT_NONE         = 0,
    PT_SOCKS4       = 4,
    PT_SOCKS5       = 5,
    PT_HTTP         = 10,
    PT_HTTPS        = 11,
    PT_FORCE_UINT   = UINT_MAX
};

bool FromString(const KString& Text, TProxyType& RType);

KString ToString(TProxyType Type);

// -------------
// Proxy setter
// -------------
class TProxySetter
{
private:
    TProxyType  m_OldType;
    DWORD       m_dwOldIP;
    WORD        m_wOldPort;

public:
    TProxySetter(TProxyType Type, DWORD dwIP, WORD wPort);

    ~TProxySetter();
};

// ----------------
// Global routines
// ----------------
TProxyType PerformProxyBypass(  TSocket&    Socket,
                                DWORD       dwIP,
                                WORD        wPort,
                                TProxyType  ProxyType,
                                DWORD       dwProxyIP,
                                WORD        wProxyPort);

TProxyType PerformProxyBypass(  TSocket&    Socket,
                                DWORD       dwIP,
                                WORD        wPort);

#endif // proxy_h
