#ifndef ftp_h
#define ftp_h

#include <KFC_Networking/socket.h>

// Ports
#define FTP_PORT    (21)

// ---------------
// FTP connection
// ---------------
class T_FTP_Connection
{
private:
    TSocket m_Socket;

    DWORD   m_dwIP;
    KString m_Login;
    KString m_Password;

    bool m_bPassiveMode;

    HANDLE m_hTerminator;
    size_t m_szTimeout;

    size_t m_szNAttempts;
    size_t m_szRetryDelay;

private:
    void Reconnect(bool bIsRetry);

    void SendRequest(LPCTSTR pRequest);

    KString ReceiveReply(int iDesiredCodeLevel, int* pRCode = NULL);

    void PrepareTransfer(LPCTSTR pRequest, TSocket& RDataSocket);

public:
    T_FTP_Connection();

    T_FTP_Connection(   DWORD   dwIP,
                        LPCTSTR pLogin,
                        LPCTSTR pPassword,
                        bool    bPassiveMode,
                        HANDLE  hTerminator     = NULL,
                        size_t  szTimeout       = UINT_MAX,
                        size_t  szNAttempts     = 1,
                        size_t  szRetryDelay    = 500);

    ~T_FTP_Connection()
        { Release(); }

    bool IsAllocated() const
        { return m_Socket.IsAllocated(); }

    void Release(bool bFromException = false);

    void Allocate(  DWORD   dwIP,
                    LPCTSTR pLogin,
                    LPCTSTR pPassword,
                    bool    bPassiveMode,
                    HANDLE  hTerminator = NULL,
                    size_t  szTimeout   = UINT_MAX,
                    size_t  szNAttempts     = 1,
                    size_t  szRetryDelay    = 500);

    HANDLE GetTerminator() const
        { DEBUG_VERIFY_ALLOCATION; return m_hTerminator; }

    size_t GetTimeout() const
        { DEBUG_VERIFY_ALLOCATION; return m_szTimeout; }

    size_t GetNAttempts() const
        { DEBUG_VERIFY_ALLOCATION; return m_szNAttempts; }

    size_t GetRetryDelay() const
        { DEBUG_VERIFY_ALLOCATION; return m_szRetryDelay; }

    void SetTerminator(HANDLE hTerminator);

    void SetTimeout(size_t szTimeout);

    void SetNAttempts(size_t szNAttempts);

    void SetRetryDelay(size_t szRetryDelay);

    void List(  LPCTSTR             pMask,
                KStrings&           RItems,
                TArray<bool, true>& RAreFolders,
                bool                bFullPaths = true);

    void ListFolders(LPCTSTR pMask, KStrings& RFolders, bool bFullPaths = true);

    void ListFiles(LPCTSTR pMask, KStrings& RFiles, bool bFullPaths = true);

    void GetFile(LPCTSTR pRemoteFileName, LPCTSTR pLocalFileName);

    void PutFile(LPCTSTR pLocalFileName, LPCTSTR pRemoteFileName);

    void LoadFile(LPCTSTR pRemoteFileName, TArray<BYTE, true>& RData);

    void SaveFile(LPCTSTR pRemoteFileName, const char* pData, size_t szLength);

    void DeleteFile(LPCTSTR pRemoteFileName, bool bSafe, bool bOnce = true);

    void CreateFolder(LPCTSTR pRemoteFolderName, bool bSafe, bool bOnce = true);

    void DeleteFolder(LPCTSTR pRemoteFolderName, bool bSafe, bool bOnce = true);

    bool IsUsingPassiveMode() const
        { DEBUG_VERIFY_ALLOCATION; return m_bPassiveMode; }
};

#endif // ftp_h
