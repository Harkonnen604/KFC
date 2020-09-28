#include "kfc_protocols_pch.h"
#include "http.h"

#include <KFC_KTL/hex.h>
#include <KFC_Formats/url.h>
#include <KFC_Networking/socket.h>
#include "protocols_consts.h"
#include "proxy.h"

// -------------
// HTTP headers
// -------------
void T_HTTP_Headers::Clear()
{
    m_Prefix.Empty();

    TVariableList::Clear();
}

void T_HTTP_Headers::Parse(LPCTSTR s)
{
    Clear();

    KStrings Tokens(s, TEXT("\r\n"));

    if(Tokens.IsEmpty())
        return;

    m_Prefix = *Tokens.GetFirst();

    for(KStrings::TConstIterator Iter = Tokens + 1 ; Iter.IsValid() ; ++Iter)
    {
        TIterator Iter2 = AddLast();

        {
            LPCTSTR s = *Iter;

            LPCTSTR p;

            if(p = _tcschr(s, TEXT(':')))
                Iter2->m_Name = KString(s, p - s).Trim(), (Iter2->m_Value = p+1).Trim();
            else
                (Iter2->m_Name = s).Trim(), Iter2->m_Value.Empty();
        }
    }
}

KString T_HTTP_Headers::Write() const
{
    KString Text;

    Text += m_Prefix, Text += TEXT("\r\n");

    for(TConstIterator Iter = GetFirst() ; Iter.IsValid() ; ++Iter)
        Text += Iter->m_Name, Text += TEXT(": "), Text += Iter->m_Value, Text += TEXT("\r\n");

    return Text;
}

void T_HTTP_Headers::ParseRequestPrefix(KString* pRCommand,
                                        KString* pRResource,
                                        KString* pRVersion) const
{
    KStrings Tokens(m_Prefix, TEXT(" \t\r\n"));

    if(0 < Tokens.GetN() && pRCommand)
        *pRCommand = Tokens[0];

    if(1 < Tokens.GetN() && pRResource)
        *pRResource = Tokens[1];

    if(2 < Tokens.GetN() && pRVersion)
        *pRVersion = Tokens[2];
}

void T_HTTP_Headers::ParseResponsePrefix(   int*        pRCode,
                                            KString*    pRCodeText,
                                            KString*    pRVersion) const
{
    KStrings Tokens(m_Prefix, TEXT(" \t\r\n"));

    if(0 < Tokens.GetN() && pRVersion)
        *pRVersion = Tokens[0];

    if(1 < Tokens.GetN() && pRCode)
        *pRCode = _ttoi(Tokens[1]);

    if(pRCodeText)
    {
        pRCodeText->Empty();

        for(KStrings::TConstIterator Iter = Tokens + 2 ; Iter.IsValid() ; ++Iter)
            *pRCodeText += *Iter, *pRCodeText += TEXT(' ');

        pRCodeText->TrimRight();
    }
}

void T_HTTP_Headers::SetRequestPrefix(  LPCTSTR pCommand,
                                        LPCTSTR pResource,
                                        LPCTSTR pVersion)
{
    m_Prefix =
        KString(pCommand).Trim().ToUpper() + TEXT(' ') +
        KString(pResource).Trim() + TEXT(' ') +
        KString(pVersion).Trim().ToUpper();
}

void T_HTTP_Headers::SetResponsePrefix( int     iCode,
                                        LPCTSTR pCodeText,
                                        LPCTSTR pVersion)
{
    m_Prefix =
        KString(pVersion).Trim().ToUpper() + TEXT(' ') +
        KString(iCode) + TEXT(' ') +
        KString(pCodeText).Trim();
}

// ----------------
// Global routines
// ----------------
void OpenURL(   const T_URL&            URL,
                const T_HTTP_Buffer&    Request,
                T_HTTP_Buffer&          RResponse,
                T_HTTP_Headers*         pRHeaders,
                bool                    bHeadersOnly,
                HANDLE                  hTerminator,
                size_t                  szTimeout)
{
    T_HTTP_Headers TempHeaders;

    if(pRHeaders == NULL)
        pRHeaders = &TempHeaders;

    KString Text;

    if(URL.m_Protocol.CollateNoCase(TEXT("http")))
        INITIATE_DEFINED_FAILURE((KString)TEXT("Unsupported protocol \"") + URL.m_Protocol + TEXT("\"."));

    TArray<DWORD, true> IPs;
    ResolveIPs(URL.m_Address, IPs, true, hTerminator, szTimeout);

    for(size_t i = 0 ; ; i++)
    {
        TEST_BLOCK_BEGIN
        {
            RResponse.Clear();

            TSocket Socket;
            Socket.Allocate();

            Socket.SetTerminator(hTerminator);
            Socket.SetTimeout   (szTimeout);

            const TProxyType ProxyType =
                PerformProxyBypass( Socket,
                                    IPs[i],
                                    URL.m_wPort);

            // Sending request headers
            {
                T_HTTP_Headers Headers;

                if(ProxyType == PT_HTTP)
                {
                    Headers.SetRequestPrefix(   Request.IsEmpty() ?
                                                    TEXT("GET") :
                                                    TEXT("POST"),
                                                (KString)URL,
                                                TEXT("HTTP/1.0"));
                }
                else
                {
                    Headers.SetRequestPrefix(   Request.IsEmpty() ?
                                                    TEXT("GET") :
                                                    TEXT("POST"),
                                                (KString)URL.m_Resource);
                }

                Headers.Add(TEXT("User-Agent"), TEXT("MSIE"));

                Headers.Add(TEXT("Host"), URL.m_Address);

                Headers.Add(TEXT("Accept"), TEXT("*/*"));

                Headers.Add(TEXT("Accept-Language"), TEXT("en-us"));

        //      Headers.Add(TEXT("Connection"), TEXT("close"));

                if(!Request.IsEmpty())
                {
                    Headers.Add(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));

                    Headers.Add(TEXT("Content-Length"), (KString)Request.GetN());
                }

                Text = Headers.Write(), Text += TEXT("\r\n"), Text += (KString)Request;

                Socket.Send(Text.GetDataPtr(), Text.GetLength());
            }

            // Receiving response headers
            {
                Text.Empty();

                char lc[2] = {0, 0};

                for(;;)
                {
                    char c;

                    Socket >> c;

                    Text += c;

                    if( lc[1] == '\n' && c == '\n' ||
                        lc[0] == '\n' && lc[1] == '\r' && c == '\n')
                    {
                        break;
                    }

                    lc[0] = lc[1], lc[1] = c;
                }

                pRHeaders->Parse(Text);
            }

            if(bHeadersOnly)
                return;

            // Estimating response data length
            size_t  szDataLength;
            bool    bChunked = false;

            {
                size_t  szContentLength = UINT_MAX;
                bool    bClose          = false;

                for(T_HTTP_Headers::TConstIterator Iter = pRHeaders->GetFirst() ;
                    Iter.IsValid() ;
                    ++Iter)
                {
                    if(!Iter->m_Name.CompareNoCase(TEXT("Content-Length")))
                    {
                        szContentLength = _ttoi(Iter->m_Value);
                    }
                    else if(!Iter->m_Name .CompareNoCase(TEXT("Connection")) &&
                            !Iter->m_Value.CompareNoCase(TEXT("close")))
                    {
                        bClose = true;
                    }
                    else if(!Iter->m_Name .CompareNoCase(TEXT("Transfer-Encoding")) &&
                            !Iter->m_Value.CompareNoCase(TEXT("chunked")))
                    {
                        bChunked = true;
                    }
                }

                if(szContentLength != UINT_MAX)
                    szDataLength = szContentLength;
                else if(bClose)
                    szDataLength = UINT_MAX;
                else
                    szDataLength = 0;
            }

            // Receiving response data
            if(bChunked)
            {
                char c;

                for(;;)
                {
                    size_t cl = 0;

                    for(;;)
                    {
                        Socket >> c;

                        if(IsHexChar(c))
                            cl <<= 4, cl |= CharToHex(c);
                        else if(c == '\n')
                            break;
                    }

                    Socket.Receive(&RResponse.Add(cl), cl);

                    while((Socket >> c, c) != '\n');

                    if(cl == 0)
                        break;
                }
            }
            else
            {
                if(szDataLength == UINT_MAX)
                {
                    for(;;)
                    {
                        char buf[2048];

                        const size_t l = Socket.ReceiveAvailable(buf, sizeof(buf));

                        if(l == 0)
                            break;

                        memcpy(&RResponse.Add(l), buf, l);
                    }
                }
                else
                {
                    Socket.Receive(&RResponse.Add(szDataLength), szDataLength);
                }
            }
        }
        TEST_BLOCK_KFC_EXCEPTION_HANDLER
        {
            if( !g_ProtocolsConsts.m_bTryAllHTTP_IPs ||
                i == IPs.GetN() - 1)
            {
                throw;
            }

            continue;
        }
        TEST_BLOCK_END

        break;
    }
}

void ParseContentType(LPCTSTR s, KString& RType, KString* pRCharset)
{
    LPCTSTR p;

    RType.Empty();

    if(pRCharset)
        pRCharset->Empty();

    bool bFirst = true;

    for(;;)
    {
        if(!(p = _tcschr(s, TEXT(';'))))
            break;

        if(bFirst)
        {
            bFirst = false;

            RType.Allocate(s, p - s);

            if(!pRCharset)
                break;

            continue;
        }

        p++;

        for( ; _istspace(*p) ; p++);

        if(!_tcsnicmp(p, TEXT("charset"), 7))
        {
            pRCharset->Empty();

            p += 7;

            for( ; _istspace(*p) ; p++);

            if(*p == TEXT('='))
            {
                p++;

                for( ; _istspace(*p) ; p++);

                for( ; *p && !_istspace(*p) && *p != TEXT(';') ; p++)
                    (*pRCharset) += *p;
            }
        }

        s = p;
    }

    if(bFirst)
        RType = s;

    RType.Trim();

    if(pRCharset)
        pRCharset->Trim();
}
