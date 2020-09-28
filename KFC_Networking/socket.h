#ifndef socket_h
#define socket_h

#include <KFC_Common/timer.h>

// ------------
// Socket base
// ------------
class TSocketBase : public TStream
{
public:
    virtual ~TSocketBase() {}

    virtual void InitiateShutdown(bool bSafe) = 0;

    virtual void ReceiveShutdown() = 0;

    virtual size_t ReceiveAvailable(void* pRData, size_t szLength) = 0;

    virtual size_t SendAvailable(const void* pRData, size_t szLength) = 0;

    bool Receive(void* pRData, size_t szLength, bool bAllowSD = false);

    void Send(const void* pData, size_t szLength);

    void StreamRead(void* pRData, size_t szLength)
        { Receive(pRData, szLength); }

    void StreamWrite(const void* pData, size_t szLength)
        { Send(pData, szLength); }

    virtual bool IsConnected() const = 0;
};

// -------
// Socket
// -------
class TSocket : public TSocketBase
{
public:
    // Terminator setter
    class TTerminatorSetter
    {
    private:
        TSocket& m_Socket;

        HANDLE m_hOldTerminator;

    public:
        TTerminatorSetter(TSocket& Socket, HANDLE hTermianator) :
            m_Socket(Socket)
        {
            m_hOldTerminator = m_Socket.GetTerminator();

            m_Socket.SetTerminator(hTermianator);
        }

        ~TTerminatorSetter()
        {
            m_Socket.SetTerminator(m_hOldTerminator);
        }
    };

    // Timeout setter
    class TTimeoutSetter
    {
    private:
        TSocket& m_Socket;

        size_t m_szOldTimeout;

    public:
        TTimeoutSetter(TSocket& Socket, size_t szTimeout) :
            m_Socket(Socket)
        {
            m_szOldTimeout = m_Socket.GetTimeout();

            m_Socket.SetTimeout(szTimeout);
        }

        ~TTimeoutSetter()
        {
            m_Socket.SetTimeout(m_szOldTimeout);
        }
    };

    // Poll delay setter
    class TPollDelaySetter
    {
    private:
        TSocket& m_Socket;

        size_t m_szOldPollDelay;

    public:
        TPollDelaySetter(TSocket& Socket, size_t szPollDelay) :
            m_Socket(Socket)
        {
            m_szOldPollDelay = m_Socket.GetPollDelay();

            m_Socket.SetPollDelay(szPollDelay);
        }

        ~TPollDelaySetter()
        {
            m_Socket.SetPollDelay(m_szOldPollDelay);
        }
    };

private:
    SOCKET m_Socket;

    bool m_bBound;
    bool m_bListening;
    bool m_bConnected;
    bool m_bUDP;
    HANDLE m_hTerminator;
    size_t m_szTimeout;
    size_t m_szPollDelay;

private:
    friend class TNonBlocker;

    class TNonBlocker
    {
    private:
        TSocket& m_Socket;

    public:
        TNonBlocker(TSocket& Socket) : m_Socket(Socket)
            { m_Socket.SetNonBlocking(true); }

        ~TNonBlocker()
            { m_Socket.SetNonBlocking(false); }
    };

    void SetNonBlocking(bool bNonBlocking);

public:
    TSocket();

    TSocket(SOCKET Socket);

    TSocket(DWORD dwIP, WORD wPort);

    ~TSocket()
        { Release(); }

    bool IsAllocated() const
        { return VALID_SOCKET(m_Socket); }

    void Release();

    void Invalidate();

    TSocket& ReOwn(TSocket& Socket);

    void Allocate();

    void Allocate(SOCKET Socket);

    void Allocate(DWORD dwIP, WORD wPort);

    void AllocateUDP();

    HANDLE GetTerminator() const
        { DEBUG_VERIFY_ALLOCATION; return m_hTerminator; }

    size_t GetTimeout() const
        { DEBUG_VERIFY_ALLOCATION; return m_szTimeout; }

    size_t GetPollDelay() const
        { DEBUG_VERIFY_ALLOCATION; return m_szPollDelay; }

    void SetTerminator(HANDLE hTerminator)
        { DEBUG_VERIFY_ALLOCATION; m_hTerminator = hTerminator; }

    void SetTimeout(size_t szTimeout)
        { DEBUG_VERIFY_ALLOCATION; m_szTimeout = szTimeout; }

    void SetPollDelay(size_t szPollDelay)
        { DEBUG_VERIFY_ALLOCATION; m_szPollDelay = szPollDelay; }

    void WaitReading();

    void WaitWriting();

    void AllowBroadcast(bool bAllow = true);

    void Bind(DWORD dwIP, WORD wPort);

    void Listen(size_t szMaxConnections = SOMAXCONN);

    void Listen(DWORD dwIP, WORD wPort, size_t szMaxConnections = SOMAXCONN);

    SOCKET Accept();

    void Connect(DWORD dwIP, WORD wPort);

    bool ConnectNonBlocking(DWORD dwIP, WORD wPort);

    void InitiateShutdown(bool bSafe = false);

    void ReceiveShutdown();

    size_t ReceiveAvailable(void* pRData, size_t szLength);

    size_t SendAvailable(const void* pData, size_t szLength);

    size_t ReceiveFrom( void*   pRData,
                        size_t  szLength,
                        DWORD&  dwR_IP  = temp<DWORD>   (),
                        WORD&   wRPort  = temp<WORD>    ());

    size_t SendTo(const void* pData, size_t szLength, DWORD dwIP, WORD wPort);

    SOCKET GetSocket() const
        { DEBUG_VERIFY_ALLOCATION; return m_Socket; }

    operator SOCKET () const
        { return GetSocket(); }

    void GetLocalIP_Port(DWORD& dwR_IP, WORD& dwR_Port = temp<WORD>()) const;

    DWORD GetLocalIP() const
    {
        DEBUG_VERIFY_ALLOCATION;

        DWORD dwIP;
        GetLocalIP_Port(dwIP);

        return dwIP;
    }

    WORD GetLocalPort() const
    {
        DEBUG_VERIFY_ALLOCATION;

        WORD wPort;
        GetLocalIP_Port(temp<DWORD>(), wPort);

        return wPort;
    }

    void GetRemoteIP_Port(DWORD& dwR_IP, WORD& wRPort = temp<WORD>()) const;

    DWORD GetRemoteIP() const
    {
        DEBUG_VERIFY_ALLOCATION;

        DWORD dwIP;
        GetRemoteIP_Port(dwIP);

        return dwIP;
    }

    WORD GetRemotePort() const
    {
        DEBUG_VERIFY_ALLOCATION;

        WORD wPort;
        GetRemoteIP_Port(temp<DWORD>(), wPort);

        return wPort;
    }

    #ifdef _MSC_VER
        void BindEvent(HANDLE hEvent, int iTypes);
    #endif // _MSC_VER

    // ---------------- TRIVIALS ----------------
    bool IsUDP() const
        { DEBUG_VERIFY_ALLOCATION; return m_bUDP; }

    bool IsBound() const
        { DEBUG_VERIFY_ALLOCATION; return m_bBound; }

    bool IsListening() const
        { DEBUG_VERIFY_ALLOCATION; DEBUG_VERIFY(!IsUDP()); return m_bListening; }

    bool IsConnected() const
        { DEBUG_VERIFY_ALLOCATION; DEBUG_VERIFY(!IsUDP()); return m_bConnected; }
};

// ----------------
// Global routines
// ----------------
DWORD ReadIP(LPCTSTR s);

KString WriteIP(DWORD dwIP);

bool ReadIP_Port(LPCTSTR s, DWORD& dwR_IP, WORD& wRPort);

bool ReadIP_Ports(LPCTSTR s, DWORD& dwR_IP, WORD& wRPort1, WORD& wRPort2);

bool ReadIP_PortLazily(LPCTSTR s, DWORD& dwR_IP, WORD& wRPort);

bool ReadIP_PortsLazily(LPCTSTR s, DWORD& dwR_IP, WORD& wRPort1, WORD& wRPort2);

bool ReadAddressPort(KString s, KString& RAddress, WORD& wRPort);

bool ReadAddressPorts(KString s, KString& RAddress, WORD& wRPort1, WORD& wRPort2);

KString WriteIP_Port(DWORD dwIP, WORD wPort, LPCTSTR pSeparator = TEXT(":"));

KString WriteIP_Ports(DWORD dwIP, WORD wPort1, WORD wPort2, LPCTSTR pSeparator = TEXT(":"));

KString WriteAddressPort(LPCTSTR pAddress, WORD wPort, LPCTSTR pSeparator = TEXT(":"));

KString WriteAddressPorts(LPCTSTR pAddress, WORD wPort1, WORD wPort2, LPCTSTR pSeparator = TEXT(":"));

bool WaitForSockets(SOCKET* pReceive,   size_t szNReceive,
                    SOCKET* pSend,      size_t szNSend,
                    SOCKET* pError,     size_t szNError,
                    size_t  szTimeout   = UINT_MAX,
                    HANDLE  hTerminator = NULL,
                    size_t  szPollDelay = DEFAULT_POLL_DELAY);

DWORD ResolveIP(LPCTSTR pAddress,
                HANDLE  hTerminator = NULL,
                DWORD   dwTimeout   = INFINITE);

size_t ResolveIPs(  LPCTSTR                 pAddress,
                    TArray<DWORD, true>&    R_IPs,
                    bool                    bClearFirst = true,
                    HANDLE                  hTerminator = NULL,
                    DWORD                   dwTimeout   = INFINITE);

// -------
// Subnet
// -------
struct TSubnet
{
public:
    DWORD   m_dwIP;
    size_t  m_szNet;

public:
    TSubnet()
        { m_dwIP = INADDR_NONE, m_szNet = 0; }

    TSubnet(DWORD dwIP, size_t szNet = 32)
        { m_dwIP = dwIP, m_szNet = szNet; }

    TSubnet& Set(DWORD dwIP, size_t szNet = 32)
    {
        m_dwIP = dwIP, m_szNet = szNet;

        return *this;
    }

    bool IsValid() const
        { return m_dwIP != INADDR_NONE && m_szNet <= 32; }

    bool TestIP(DWORD dwIP) const
        { return IsValid() && (!m_szNet || !((dwIP ^ m_dwIP) & (DWORD_MAX << (32u - m_szNet)))); }

    operator KString () const
    {
        if(!IsValid())
            return WriteIP(INADDR_NONE);

        switch(m_szNet)
        {
        case 0:
            return "*";

        case 32:
            return WriteIP(m_dwIP);

        default:
            return WriteIP(m_dwIP) + '/' + m_szNet;
        }
    }
};

DECLARE_BASIC_STREAMING(TSubnet)

bool FromString(KString Text, TSubnet& RSubnet);

#endif // socket_h
