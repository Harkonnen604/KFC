#include "kfc_networking_pch.h"
#include "socket.h"

#include <KFC_KTL/process.h>
#include <KFC_KTL/event.h>
#include <KFC_Common/timer.h>
#include "networking_device_globals.h"

// ------------
// Socket base
// ------------
bool TSocketBase::Receive(void* pRData, size_t szLength, bool bAllowSD)
{
    DEBUG_VERIFY(szLength == 0 || pRData);

    DEBUG_VERIFY(!(bAllowSD && !szLength));

    for(bool bFirst = true ; szLength > 0 ; bFirst = false)
    {
        const size_t szN = ReceiveAvailable(pRData, szLength);

        if(!szN)
        {
            if(bAllowSD && bFirst)
                return false;

            INITIATE_DEFINED_FAILURE(TEXT("Premature connection shutdown."));
        }

        DEBUG_VERIFY(szN <= szLength);

        pRData = (BYTE*)pRData + szN, szLength -= szN;
    }

    return true;
}

void TSocketBase::Send(const void* pData, size_t szLength)
{
    DEBUG_VERIFY(szLength == 0 || pData);

    while(szLength > 0)
    {
        const size_t szN = SendAvailable(pData, szLength);

        DEBUG_VERIFY(szN <= szLength);

        pData = (const BYTE*)pData + szN, szLength -= szN;
    }
}

// -------
// Socket
// -------
TSocket::TSocket()
{
    m_Socket = INVALID_SOCKET;
}

TSocket::TSocket(SOCKET Socket)
{
    m_Socket = INVALID_SOCKET;

    Allocate(Socket);
}

TSocket::TSocket(DWORD dwIP, WORD wPort)
{
    m_Socket = INVALID_SOCKET;

    Allocate(dwIP, wPort);
}

void TSocket::Release()
{
    if(VALID_SOCKET(m_Socket))
        closesocket(m_Socket), m_Socket = INVALID_SOCKET;
}

void TSocket::Invalidate()
{
    m_Socket = INVALID_SOCKET;
}

TSocket& TSocket::ReOwn(TSocket& Socket)
{
    if(&Socket == this)
        return *this;

    Release();

    if(!Socket.IsAllocated())
        return *this;

    m_Socket = Socket.m_Socket;

    m_bBound        = Socket.m_bBound;
    m_bListening    = Socket.m_bListening;
    m_bConnected    = Socket.m_bConnected;
    m_bUDP          = Socket.m_bUDP;

    m_hTerminator   = Socket.m_hTerminator;
    m_szTimeout     = Socket.m_szTimeout;
    m_szPollDelay   = Socket.m_szPollDelay;

    Socket.Invalidate();

    return *this;
}

void TSocket::Allocate()
{
    Release();

    try
    {
        if(!VALID_SOCKET(m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
        {
            INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error creating TCP network socket"),
                                            WSAGetLastError());
        }

        #ifndef _MSC_VER
        #ifndef __linux__
        {
            int v = 1;
            setsockopt(m_Socket, SOL_SOCKET, SO_NOSIGPIPE, &v, sizeof(v));
        }
        #endif // __linux__
        #endif // _MSC_VER

        m_bUDP          = false;
        m_bBound        = false;
        m_bListening    = false;
        m_bConnected    = false;

        m_hTerminator   = NULL;
        m_szTimeout     = UINT_MAX;
        m_szPollDelay   = DEFAULT_POLL_DELAY;
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void TSocket::Allocate(SOCKET SSocket)
{
    Release();

    try
    {
        DEBUG_VERIFY(VALID_SOCKET(SSocket));

        m_Socket = SSocket;

        #ifndef _MSC_VER
        #ifndef __linux__
        {
            int v = 1;
            setsockopt(m_Socket, SOL_SOCKET, SO_NOSIGPIPE, &v, sizeof(v));
        }
        #endif // __linux__
        #endif // _MSC_VER

        m_bUDP          = false;
        m_bBound        = false;
        m_bListening    = false;
        m_bConnected    = true;

        m_hTerminator   = NULL;
        m_szTimeout     = UINT_MAX;
        m_szPollDelay   = DEFAULT_POLL_DELAY;
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void TSocket::Allocate(DWORD dwIP, WORD wPort)
{
    Release();

    try
    {
        Allocate();

        Connect(dwIP, wPort);
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void TSocket::AllocateUDP()
{
    Release();

    try
    {
        if(!VALID_SOCKET(m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)))
        {
            INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error creating UDP network socket"),
                                            WSAGetLastError());
        }

        #ifndef _MSC_VER
        #ifndef __linux__
        {
            int v = 1;
            setsockopt(m_Socket, SOL_SOCKET, SO_NOSIGPIPE, &v, sizeof(v));
        }
        #endif // __linux__
        #endif // _MSC_VER

        m_bUDP          = true;
        m_bBound        = false;
        m_bListening    = false;

        m_hTerminator   = NULL;
        m_szTimeout     = UINT_MAX;
        m_szPollDelay   = DEFAULT_POLL_DELAY;
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void TSocket::SetNonBlocking(bool bNonBlocking)
{
    DEBUG_VERIFY(m_Socket != INVALID_SOCKET);

    #ifdef _MSC_VER
    {
        u_long v = bNonBlocking;

        if(ioctlsocket(m_Socket, FIONBIO, &v))
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error switching blocking mode of network socket"), WSAGetLastError());
    }
    #else // _MSC_VER
    {
        int flags = fcntl(m_Socket, F_GETFL);

        if(bNonBlocking)
            flags |= O_NONBLOCK;
        else
            flags &= ~O_NONBLOCK;

        if(fcntl(m_Socket, F_SETFL, flags) < 0)
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error switching blocking mode of network socket"), WSAGetLastError());
    }
    #endif // _MSC_VER
}

void TSocket::WaitReading()
{
    DEBUG_VERIFY_ALLOCATION;

    if(!m_hTerminator && m_szTimeout == UINT_MAX)
        return;

    SOCKET rs = m_Socket;
    SOCKET es = m_Socket;

    if( !WaitForSockets(&rs, 1, NULL, 0, &es, 1, m_szTimeout, m_hTerminator, m_szPollDelay) ||
        !VALID_SOCKET(rs) || VALID_SOCKET(es))
    {
        CHECK_TERMINATION(m_hTerminator);

        INITIATE_DEFINED_FAILURE(TEXT("Error waiting for network socket to become readable."));
    }
}

void TSocket::WaitWriting()
{
    DEBUG_VERIFY_ALLOCATION;

    if(!m_hTerminator && m_szTimeout == UINT_MAX)
        return;

    SOCKET ws = m_Socket;
    SOCKET es = m_Socket;

    if( !WaitForSockets(NULL, 0, &ws, 1, &es, 1, m_szTimeout, m_hTerminator, m_szPollDelay) ||
        !VALID_SOCKET(ws) || VALID_SOCKET(es))
    {
        CHECK_TERMINATION(m_hTerminator);

        INITIATE_DEFINED_FAILURE(TEXT("Error waiting for network socket to become writable."));
    }
}

void TSocket::AllowBroadcast(bool bAllow)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(m_bUDP);

    const BOOL bValue = bAllow;

    if(setsockopt(m_Socket, SOL_SOCKET, SO_BROADCAST, (const char*)&bValue, sizeof(bValue)))
    {
        INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error setting socket broadcast parameter"),
                                        WSAGetLastError());
    }
}

void TSocket::Bind(DWORD dwIP, WORD wPort)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsBound());

    {
        sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));

        addr.sin_family         = AF_INET;
        addr.sin_addr.s_addr    = htonl(dwIP);
        addr.sin_port           = htons(wPort);

        if(bind(m_Socket, (const sockaddr*)&addr, sizeof(addr)))
        {
            INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error binding network socket"),
                                            WSAGetLastError());
        }
    }

    m_bBound = true;
}

void TSocket::Listen(size_t szMaxConnections)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsUDP() && IsBound() && !IsListening());

    if(listen(m_Socket, szMaxConnections))
    {
        INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error listening on network socket"),
                                        WSAGetLastError());
    }

    m_bListening = true;
}

void TSocket::Listen(DWORD dwIP, WORD wPort, size_t szMaxConnections)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsUDP() && !IsBound() && !IsListening());

    Bind(dwIP, wPort);

    Listen(szMaxConnections);
}

SOCKET TSocket::Accept()
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsUDP() && IsListening());

    WaitReading();

    SOCKET Socket = accept(*this, NULL, NULL);

    if(!VALID_SOCKET(Socket))
    {
        INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error accepting network socket connection"),
                                        WSAGetLastError());
    }

    return Socket;
}

void TSocket::Connect(DWORD dwIP, WORD wPort)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsUDP() && !IsListening());

    DEBUG_VERIFY(!IsConnected());

    DEBUG_VERIFY(dwIP != INADDR_NONE && wPort);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family         = AF_INET;
    addr.sin_addr.s_addr    = htonl(dwIP);
    addr.sin_port           = htons(wPort);

    if(!m_hTerminator && m_szTimeout == UINT_MAX)
    {
        if(connect(m_Socket, (const sockaddr*)&addr, sizeof(addr)))
        {
            INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error connecting network socket"),
                                            WSAGetLastError());
        }
    }
    else
    {
        TNonBlocker NonBlocker0(*this);

        if( connect(m_Socket, (const sockaddr*)&addr, sizeof(addr)) &&
                WSAGetLastError() != WSAEWOULDBLOCK)
        {
            INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error connecting network socket"),
                                            WSAGetLastError());
        }

        WaitWriting();

        {
            int v = 0;
            int l = sizeof(v);

            if(getsockopt(m_Socket, SOL_SOCKET, SO_ERROR, (char*)&v, &l) || v)
                INITIATE_DEFINED_CODE_FAILURE(TEXT("Error connecting network socket"), v);
        }
    }

    m_bConnected = true;
}

bool TSocket::ConnectNonBlocking(DWORD dwIP, WORD wPort)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsUDP() && !IsListening());

    DEBUG_VERIFY(!IsConnected());

    DEBUG_VERIFY(dwIP != INADDR_NONE && wPort);

    SetNonBlocking(true);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family         = AF_INET;
    addr.sin_addr.s_addr    = htonl(dwIP);
    addr.sin_port           = htons(wPort);

    if(!connect(m_Socket, (const sockaddr*)&addr, sizeof(addr)))
    {
        m_bConnected = true;

        return true;
    }

    if(WSAGetLastError() == WSAEWOULDBLOCK)
    {
        m_bConnected = true;

        return false;
    }

    INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error connecting network socket"),
                                    WSAGetLastError());
}

void TSocket::InitiateShutdown(bool bSafe)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsUDP() && IsConnected());

    if(shutdown(m_Socket, SD_SEND) && !bSafe)
    {
        INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error shutting down network socket"),
                                        WSAGetLastError());
    }
}

void TSocket::ReceiveShutdown()
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsUDP() && IsConnected());

    BYTE c;

    if(ReceiveAvailable(&c, sizeof(c)) > 0)
        INITIATE_DEFINED_FAILURE(TEXT("Data arrived instead of shutdown request on network socket."));
}

size_t TSocket::ReceiveAvailable(void* pRData, size_t szLength)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsUDP() && IsConnected());

    DEBUG_VERIFY(szLength == 0 || pRData);

    if(szLength == 0)
        return 0;

    WaitReading();

    const int l = recv(m_Socket, (char*)pRData, szLength, 0);

    if(l < 0)
    {
        INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error receiving data from network socket"),
                                        WSAGetLastError());
    }

    DEBUG_VERIFY((size_t)l <= szLength);

    return (size_t)l;
}

size_t TSocket::SendAvailable(const void* pData, size_t szLength)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsUDP() && IsConnected());

    DEBUG_VERIFY(szLength == 0 || pData);

    if(szLength == 0)
        return 0;

    WaitWriting();

    const int l = send(m_Socket, (const char*)pData, szLength, 0);

    if(l <= 0)
    {
        INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error sending data to network socket"),
                                        WSAGetLastError());
    }

    DEBUG_VERIFY((size_t)l <= szLength);

    return (size_t)l;
}

size_t TSocket::ReceiveFrom(void*   pRData,
                            size_t  szLength,
                            DWORD&  dwR_IP,
                            WORD&   wRPort)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsUDP());

    WaitReading();

    sockaddr_in addr;
    int sz = sizeof(addr);

    const int l = recvfrom( m_Socket,
                            (char*)pRData,
                            szLength,
                            0,
                            (sockaddr*)&addr,
                            &sz);

    if(l < 0 || sz != sizeof(addr) || addr.sin_family != AF_INET)
        INITIATE_DEFINED_FAILURE(TEXT("Error receiving from UDP network socket."));

    dwR_IP = ntohl(addr.sin_addr.s_addr);

    wRPort = ntohs(addr.sin_port);

    return l;
}

size_t TSocket::SendTo(const void* pData, size_t szLength, DWORD dwIP, WORD wPort)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsUDP());

    DEBUG_VERIFY(dwIP != INADDR_NONE && wPort);

    WaitWriting();

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family         = AF_INET;
    addr.sin_addr.s_addr    = htonl(dwIP);
    addr.sin_port           = htons(wPort);

    const int l = sendto(   m_Socket,
                            (const char*)pData,
                            szLength,
                            0,
                            (const sockaddr*)&addr,
                            sizeof(addr));

    if(l < 0)
        INITIATE_DEFINED_FAILURE(TEXT("Error sending through UDP network socket."));

    return l;
}

void TSocket::GetLocalIP_Port(DWORD& dwR_IP, WORD& wRPort) const
{
    DEBUG_VERIFY_ALLOCATION;

    sockaddr_in addr;

    int sz = sizeof(addr);

    if( getsockname(m_Socket, (sockaddr*)&addr, &sz) ||
        sz != sizeof(addr) ||
        addr.sin_family != AF_INET)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error getting local network socket IP/port."));
    }

    dwR_IP = ntohl(addr.sin_addr.s_addr);
    wRPort = ntohs(addr.sin_port);
}

void TSocket::GetRemoteIP_Port(DWORD& dwR_IP, WORD& wRPort) const
{
    DEBUG_VERIFY_ALLOCATION;

    sockaddr_in addr;

    int sz = sizeof(addr);

    if( getpeername(m_Socket, (sockaddr*)&addr, &sz) ||
        sz != sizeof(addr) ||
        addr.sin_family != AF_INET)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error getting local network socket IP/port."));
    }

    dwR_IP = ntohl(addr.sin_addr.s_addr);
    wRPort = ntohs(addr.sin_port);
}

#ifdef _MSC_VER

void TSocket::BindEvent(HANDLE hEvent, int iTypes)
{
    DEBUG_VERIFY_ALLOCATION;

    if(WSAEventSelect(m_Socket, hEvent, iTypes) == SOCKET_ERROR)
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error binding network event"), WSAGetLastError());
}

#endif // _MSC_VER

// ----------------
// Global routines
// ----------------
DWORD ReadIP(LPCTSTR s)
{
    UINT v1, v2, v3, v4;

    if( _stscanf(s, TEXT("%u.%u.%u.%u"), &v1, &v2, &v3, &v4) == 4 &&
            v1 < 0x100 && v2 < 0x100 && v3 < 0x100 && v4 < 0x100)
    {
        return (v1 << 24) | (v2 << 16) | (v3 << 8) | v4;
    }

    return INADDR_NONE;
}

KString WriteIP(DWORD dwIP)
{
    return KString::Formatted(  TEXT("%u.%u.%u.%u"),
                                    (dwIP >> 24),
                                    (dwIP >> 16) & 0xFF,
                                    (dwIP >> 8)  & 0xFF,
                                    (dwIP)       & 0xFF);
}

bool ReadIP_Port(LPCTSTR s, DWORD& dwR_IP, WORD& wRPort)
{
    UINT v1, v2, v3, v4, v5;

    if( _stscanf(s, TEXT("%u.%u.%u.%u:%u"), &v1, &v2, &v3, &v4, &v5) == 5 &&
        v1 < 0x100 && v2 < 0x100 && v3 < 0x100 && v4 < 0x100 && v5 <= 0x10000)
    {
        dwR_IP = (v1 << 24) | (v2 << 16) | (v3 << 8) | v4;
        wRPort = (WORD)v5;

        return true;
    }

    return false;
}

bool ReadIP_Ports(LPCTSTR s, DWORD& dwR_IP, WORD& wRPort1, WORD& wRPort2)
{
    UINT v1, v2, v3, v4, v5, v6;

    if( _stscanf(s, TEXT("%u.%u.%u.%u:%u:%u"), &v1, &v2, &v3, &v4, &v5, &v6) == 6 &&
        v1 < 0x100 && v2 < 0x100 && v3 < 0x100 && v4 < 0x100 &&
        v5 < 0x10000 && v6 < 0x10000)
    {
        dwR_IP = (v1 << 24) | (v2 << 16) | (v3 << 8) | v4;
        wRPort1 = (WORD)v5, wRPort2 = (WORD)v6;

        return true;
    }

    return false;
}

bool ReadIP_PortLazily(LPCTSTR s, DWORD& dwR_IP, WORD& wRPort)
{
    KString str = s;

    LPTSTR p = str.GetDataPtr();

    for( ; *p ; p++)
    {
        if(!_istdigit(*p))
            *p = TEXT(' ');
    }

    UINT v1, v2, v3, v4, v5;

    if( _stscanf(str, TEXT("%u %u %u %u %u"), &v1, &v2, &v3, &v4, &v5) == 5 &&
        v1 < 0x100 && v2 < 0x100 && v3 < 0x100 && v4 < 0x100 && v5 < 0x10000)
    {
        dwR_IP = (v1 << 24) | (v2 << 16) | (v3 << 8) | v4;
        wRPort = (WORD)v5;

        return true;
    }

    return false;
}

bool ReadIP_PortsLazily(LPCTSTR s, DWORD& dwR_IP, WORD& wRPort1, WORD& wRPort2)
{
    KString str = s;

    LPTSTR p = str.GetDataPtr();

    for( ; *p ; p++)
    {
        if(!_istdigit(*p))
            *p = TEXT(' ');
    }

    UINT v1, v2, v3, v4, v5, v6;

    if( _stscanf(str, TEXT("%u %u %u %u %u %u"), &v1, &v2, &v3, &v4, &v5, &v6) == 6 &&
        v1 < 0x100 && v2 < 0x100 && v3 < 0x100 && v4 < 0x100 &&
        v5 < 0x10000 && v6 < 0x10000)
    {
        dwR_IP = (v1 << 24) | (v2 << 16) | (v3 << 8) | v4;
        wRPort1 = (WORD)v5, wRPort2 = (WORD)v6;

        return true;
    }

    return false;
}

bool ReadAddressPort(KString s, KString& RAddress, WORD& wRPort)
{
    s.Trim();

    size_t i;

    for(i = s.GetLength() - 1 ; i != UINT_MAX && _istdigit(s[i]) ; i--);

    if(i == UINT_MAX || !i || s[i] != TEXT(':'))
        return false;

    RAddress.Allocate(s, i);

    UINT p;

    if(_stscanf((LPCTSTR)s+i+1, TEXT("%u"), &p) != 1 || p >= 0x10000)
        return false;

    wRPort = (WORD)p;

    return true;
}

bool ReadAddressPorts(KString s, KString& RAddress, WORD& wRPort1, WORD& wRPort2)
{
    s.Trim();

    size_t i, j;

    for(i = s.GetLength() - 1 ; i != UINT_MAX && _istdigit(s[i]) ; i--);

    if(i == UINT_MAX || !i || s[i] != TEXT(':'))
        return false;

    for(j = i - 1 ; j != UINT_MAX && _istdigit(s[j]) ; j--);

    if(j == UINT_MAX || !j || s[j] != TEXT(':') || j == i -  1)
        return false;

    RAddress.Allocate(s, j);

    UINT p1, p2;

    if(_stscanf((LPCTSTR)s+i+1, TEXT("%u:%u"), &p1, &p2) != 2 || p1 >= 0x10000 || p2 >= 0x10000)
        return false;

    wRPort1 = (WORD)p1, wRPort2 = (WORD)p2;

    return true;
}

KString WriteIP_Port(DWORD dwIP, WORD wPort, LPCTSTR pSeparator)
{
    return KString::Formatted(  TEXT("%u.%u.%u.%u%s%u"),
                                    (dwIP >> 24),
                                    (dwIP >> 16) & 0xFF,
                                    (dwIP >> 8)  & 0xFF,
                                    (dwIP)       & 0xFF,
                                    pSeparator,
                                    (UINT)wPort);
}

KString WriteIP_Ports(DWORD dwIP, WORD wPort1, WORD wPort2, LPCTSTR pSeparator)
{
    return KString::Formatted(  TEXT("%u.%u.%u.%u%s%u%s%u"),
                                    (dwIP >> 24),
                                    (dwIP >> 16) & 0xFF,
                                    (dwIP >> 8)  & 0xFF,
                                    (dwIP)       & 0xFF,
                                    pSeparator,
                                    (UINT)wPort1,
                                    pSeparator,
                                    (UINT)wPort2);
}

KString WriteAddressPort(LPCTSTR pAddress, WORD wPort, LPCTSTR pSeparator)
{
    return (KString)pAddress + pSeparator + wPort;
}

KString WriteAddressPorts(LPCTSTR pAddress, WORD wPort1, WORD wPort2, LPCTSTR pSeparator)
{
    return (KString)pAddress + pSeparator + wPort1 + pSeparator + wPort2;
}

static inline int PrepareWaitSets(  const SOCKET* pReceive, size_t szNReceive,
                                    const SOCKET* pSend,    size_t szNSend,
                                    const SOCKET* pError,   size_t szNError,
                                    fd_set& rset,
                                    fd_set& wset,
                                    fd_set& eset)
{
    size_t i;

    FD_ZERO(&rset);
    FD_ZERO(&wset);
    FD_ZERO(&eset);

    int nfds = 0;

    for(i = 0 ; i < szNReceive ; i++)
    {
        DEBUG_VERIFY(VALID_SOCKET(pReceive[i]));

        FD_SET(pReceive[i], &rset);

        #ifndef _MSC_VER
        {
            if((int)pReceive[i] >= nfds)
                nfds = (int)pReceive[i] + 1;
        }
        #endif // _MSC_VER
    }

    for(i = 0 ; i < szNSend ; i++)
    {
        DEBUG_VERIFY(VALID_SOCKET(pSend[i]));

        FD_SET(pSend[i], &wset);

        #ifndef _MSC_VER
        {
            if((int)pSend[i] >= nfds)
                nfds = (int)pSend[i] + 1;
        }
        #endif // _MSC_VER
    }

    for(i = 0 ; i < szNError ; i++)
    {
        DEBUG_VERIFY(VALID_SOCKET(pError[i]));

        FD_SET(pError[i], &eset);

        #ifndef _MSC_VER
        {
            if((int)pError[i] >= nfds)
                nfds = (int)pError[i] + 1;
        }
        #endif // _MSC_VER
    }

    return nfds;
}

static inline size_t ReadWaitSets(  const fd_set& rset,
                                    const fd_set& wset,
                                    const fd_set& eset,
                                    SOCKET* pReceive,   size_t szNReceive,
                                    SOCKET* pSend,      size_t szNSend,
                                    SOCKET* pError,     size_t szNError)
{
    size_t ret = 0;

    size_t i;

    {
        for(i = 0 ; i < szNReceive ; i++)
        {
            DEBUG_VERIFY(VALID_SOCKET(pReceive[i]));

            if(FD_ISSET(pReceive[i], &rset))
                ret++;
            else
                pReceive[i] = INVALID_SOCKET;
        }
    }

    {
        for(i = 0 ; i < szNSend ; i++)
        {
            DEBUG_VERIFY(VALID_SOCKET(pSend[i]));

            if(FD_ISSET(pSend[i], &wset))
                ret++;
            else
                pSend[i] = INVALID_SOCKET;
        }
    }

    {
        for(i = 0 ; i < szNError ; i++)
        {
            DEBUG_VERIFY(VALID_SOCKET(pError[i]));

            if(FD_ISSET(pError[i], &eset))
                ret++;
            else
                pError[i] = INVALID_SOCKET;
        }
    }

    return ret;
}

bool WaitForSockets(SOCKET* pReceive,   size_t szNReceive,
                    SOCKET* pSend,      size_t szNSend,
                    SOCKET* pError,     size_t szNError,
                    size_t  szTimeout,
                    HANDLE  hTerminator,
                    size_t  szPollDelay)
{
    if(szNReceive == 0 && szNSend == 0 && szNError == 0)
    {
        if(hTerminator)
        {
            if(WaitForSingleObject(hTerminator, szTimeout) == WAIT_OBJECT_0)
                CHECK_TERMINATION(hTerminator);
        }
        else
        {
            DEBUG_VERIFY(szTimeout != UINT_MAX);

            Sleep(szTimeout);
        }

        return false;
    }

    DEBUG_VERIFY(   (pReceive   || szNReceive   == 0) &&
                    (pSend      || szNSend      == 0) &&
                    (pError     || szNError     == 0));

    fd_set rset;
    fd_set wset;
    fd_set eset;

    int r;

    if(hTerminator) // polled wait
    {
        TTimer Timer(true);

        for(;;)
        {
            CHECK_TERMINATION(hTerminator);

            const int nfds  =
                PrepareWaitSets(pReceive,   szNReceive,
                                pSend,      szNSend,
                                pError,     szNError,
                                rset, wset, eset);

            size_t szElapsed = (size_t)Timer.GetElapsedTime();

            if(szTimeout != UINT_MAX && szElapsed >= szTimeout)
                return false;

            const size_t szTickTime = Min(szTimeout - szElapsed, szPollDelay);

            timeval tm = {(long)(szTickTime / 1000), (long)(szTickTime % 1000) * 1000};

            if(r = select(nfds, &rset, &wset, &eset, &tm))
                break;
        }
    }
    else // pure wait
    {
        const int nfds  =
            PrepareWaitSets(pReceive,   szNReceive,
                            pSend,      szNSend,
                            pError,     szNError,
                            rset, wset, eset);

        timeval* ptm = NULL;

        timeval tm;

        if(szTimeout != UINT_MAX)
        {
            tm.tv_sec  = szTimeout / 1000;
            tm.tv_usec = szTimeout % 1000 * 1000;

            ptm = &tm;
        }

        if(!(r = select(nfds, &rset, &wset, &eset, ptm)))
            return false;
    }

    if(r < 0)
    {
        INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error waiting on network socket(s)"),
                                        WSAGetLastError());
    }
    else // r > 0
    {
        DEBUG_EVALUATE_VERIFY(ReadWaitSets( rset, wset, eset,
                                            pReceive,   szNReceive,
                                            pSend,      szNSend,
                                            pError,     szNError) > 0);
    }

    return true;
}

struct TResolveIPs_ThreadContext
{
public:
    KString m_Address;

    TArray<DWORD, true> m_IPs;

    TEvent m_FinishedEvent;
    TEvent m_DoneEvent;

public:
    TResolveIPs_ThreadContext(LPCTSTR pAddress) : m_Address(pAddress)
    {
        m_FinishedEvent.Allocate(false, true);
        m_DoneEvent.    Allocate(false, true);
    }
};

DECLARE_THREAD_PROC(ResolveIPs_ThreadProc, pParam)
{
    TThreadGuard Guard0(g_NetworkingDeviceGlobals.m_lNResolveThreads);

    KFC_OUTER_BLOCK_BEGIN
    {
        TPtrHolder<TResolveIPs_ThreadContext> pContext
            ((TResolveIPs_ThreadContext*)pParam);

        const HOSTENT* pHostent = gethostbyname(TAnsiString(pContext->m_Address));

        if( pHostent &&
            pHostent->h_addrtype == AF_INET &&
            pHostent->h_length == sizeof(DWORD))
        {
            for(const DWORD* const* p = (const DWORD* const*)pHostent->h_addr_list ; *p ; p++)
                pContext->m_IPs.Add() = ntohl(**p);
        }

        pContext->m_FinishedEvent.Set();

        pContext->m_DoneEvent.Wait();
    }
    KFC_OUTER_BLOCK_END

    return 0;
}

DWORD ResolveIP(LPCTSTR pAddress,
                HANDLE  hTerminator,
                DWORD   dwTimeout)
{
    DEBUG_VERIFY(pAddress);

    TArray<DWORD, true> IPs;

    ResolveIPs(pAddress, IPs, true, hTerminator, dwTimeout);

    return IPs[0];
}

size_t ResolveIPs(  LPCTSTR                 pAddress,
                    TArray<DWORD, true>&    R_IPs,
                    bool                    bClearFirst,
                    HANDLE                  hTerminator,
                    DWORD                   dwTimeout)
{
    if(bClearFirst)
        R_IPs.Clear();

    const size_t szRet = R_IPs.GetN();

    if(!hTerminator && dwTimeout == INFINITE)
    {
        const HOSTENT* pHostent = gethostbyname(TAnsiString(pAddress));

        if(pHostent == NULL)
        {
            INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error resolving network address \"") +
                                                pAddress +
                                                TEXT("\""),
                                            WSAGetLastError());
        }

        if( pHostent->h_addrtype    != AF_INET ||
            pHostent->h_length      != sizeof(DWORD))
        {
            INITIATE_DEFINED_FAILURE(   (KString)TEXT("Invalid address type upon resolving network address \"") +
                                            pAddress +
                                            TEXT("\""));
        }

        if(!pHostent->h_addr_list[0])
        {
            INITIATE_DEFINED_FAILURE(   (KString)TEXT("No addresses returned upon resolving network address \"") +
                                            pAddress +
                                            TEXT("\""));
        }

        for(const DWORD* const* p = (const DWORD* const*)pHostent->h_addr_list ; *p ; p++)
            R_IPs.Add() = ntohl(**p);

        return szRet;
    }
    else
    {
        TResolveIPs_ThreadContext* pContext = new TResolveIPs_ThreadContext(pAddress);

        StartGuardedThread(g_NetworkingDeviceGlobals.m_lNResolveThreads, ResolveIPs_ThreadProc, pContext);

        HANDLE Handles[2] = {pContext->m_FinishedEvent, hTerminator};

        DWORD r = WaitForMultipleObjects(hTerminator ? 2 : 1, Handles, FALSE, dwTimeout);

        if(r == WAIT_TIMEOUT) // timed out
        {
            pContext->m_DoneEvent.Set();
            INITIATE_DEFINED_FAILURE((KString)TEXT("Timed out resolving network address \"") + pAddress + TEXT("\"."));
        }

        if(r == WAIT_OBJECT_0 + 0) // finished
        {
            memcpy( &R_IPs.Add(pContext->m_IPs.GetN()),
                    pContext->m_IPs.GetDataPtr(),
                    pContext->m_IPs.GetN() * sizeof(DWORD));

            pContext->m_DoneEvent.Set();

            if(R_IPs.GetN() == szRet)
                INITIATE_DEFINED_FAILURE((KString)TEXT("Error resolving network address \"") + pAddress + TEXT("\""));

            return szRet;
        }

        if(r == WAIT_OBJECT_0 + 1) // terminated
        {
            pContext->m_DoneEvent.Set();
            CHECK_TERMINATION(hTerminator);
        }

        INITIATE_FAILURE;
    }
}

// -------
// Subnet
// -------
bool FromString(KString Text, TSubnet& RSubnet)
{
    Text.Trim();

    if(Text == '*')
    {
        RSubnet.Set(INADDR_ANY, 0);
        return true;
    }

    size_t szPos = Text.Find('/');

    if(szPos == UINT_MAX)
    {
        RSubnet.Set(ReadIP(Text));
    }
    else
    {
        DWORD dwIP = ReadIP(Text.Left(szPos));

        size_t szNet;

        if(!FromString(Text.Mid(szPos + 1), szNet))
            return false;

        RSubnet.Set(dwIP, szNet);
    }

    return RSubnet.IsValid();
}
