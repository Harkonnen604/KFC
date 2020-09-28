#ifndef kfc_networking_pch_h
#define kfc_networking_pch_h

#include <KFC_KTL/kfc_ktl_pch.h>
#include <KFC_Common/kfc_common_pch.h>

#ifdef _MSC_VER

    #define VALID_SOCKET(s)     ((s) != INVALID_SOCKET)

#else // _MSC_VER

    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <fcntl.h>

    typedef int SOCKET;

    #define VALID_SOCKET(s)     ((s) >= 0)

    #define INVALID_SOCKET      (-1)

    #define closesocket(s)  close(s)

    #define SD_RECV     (SHUT_RD)
    #define SD_SEND     (SHUT_WR)
    #define SD_BOTH     (SHUT_RDWR)

    inline int WSAGetLastError()
        { return errno; }

    #define WSAEWOULDBLOCK  (EINPROGRESS) // for 'connect'

    typedef hostent HOSTENT;

#endif // _MSC_VER

#endif // kfc_networking_pch_h
