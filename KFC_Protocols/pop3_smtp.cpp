#include "kfc_protocols_pch.h"
#include "pop3_smtp.h"

// Limits
#define RECV_BUF_SIZE   (8192)

// -----------------
// POP3 SMTP socket
// -----------------
T_POP3_SMTP_Socket::T_POP3_SMTP_Socket(bool bAllocate)
{
    if(bAllocate)
        Allocate();
}

void T_POP3_SMTP_Socket::Release()
{
    m_Buffer.Clear();

    TSocket::Release();
}

void T_POP3_SMTP_Socket::Allocate()
{
    Release();

    try
    {
        TSocket::Allocate();

        m_Buffer.SetN(RECV_BUF_SIZE);

        m_szHead = m_szTail = 0;

        m_Buffer[m_szTail] = 0;
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void T_POP3_SMTP_Socket::SendCmd(LPCTSTR pCmd)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsConnected());

    KString s;

    s += pCmd;

    s += TEXT("\r\n");

    Send(s, s.GetLength());
}

void T_POP3_SMTP_Socket::LoadBuffer(size_t szWrapLength)
{
    DEBUG_VERIFY(szWrapLength * 2 + 1 <= m_Buffer.GetN()); // must not overlap

    // Checking if buffer has crossed wrap-position
    if(m_szHead + szWrapLength >= m_Buffer.GetN())
    {
        memcpy( m_Buffer.GetDataPtr(),
                m_Buffer.GetDataPtr() + m_szHead,
                m_szTail - m_szHead);

        m_szTail -= m_szHead, m_szHead = 0;
    }

    // Receiving more incoming data
    m_szTail += ReceiveAvailable(   m_Buffer.GetDataPtr() + m_szTail,
                                    m_Buffer.GetN() - m_szTail - 1);

    m_Buffer[m_szTail] = 0;
}

KString T_POP3_SMTP_Socket::ReceiveUntilSubString(LPCTSTR pText, size_t szCutAmt)
{
    KString s;

    const size_t szTextLength = strlen(pText);

    DEBUG_VERIFY(szCutAmt <= szTextLength);

    for(;;)
    {
        char* p = strstr(m_Buffer.GetDataPtr() + m_szHead, pText);

        if(p == NULL) // terminator sub-string not found
        {           // Adding non-terminated part (if any)
            if(m_szTail - m_szHead >= szTextLength)
            {
                const size_t szNewHead = m_szTail - szTextLength + 1;

                DEBUG_VERIFY(szNewHead > m_szHead);

                s.Extend(m_Buffer.GetDataPtr() + m_szHead, szNewHead - m_szHead);

                m_szHead = szNewHead;
            }

            // Loading more
            LoadBuffer(szTextLength);
        }
        else // terminator sub-string found at 'p'
        {
            // Adding remainder including terminator sub-string and breaking the loop
            const size_t szNewHead = (p - m_Buffer.GetDataPtr()) + szTextLength;

            DEBUG_VERIFY(szNewHead > m_szHead);

            s.Extend(m_Buffer.GetDataPtr() + m_szHead, szNewHead - m_szHead);

            m_szHead = szNewHead;

            break;
        }
    }

    return s.Left(s.GetLength() - szCutAmt);
}

KString T_POP3_SMTP_Socket::ReceiveSingle()
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsConnected());

    return ReceiveUntilSubString(TEXT("\r\n"), 2);
}

KString T_POP3_SMTP_Socket::ReceiveMulti()
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsConnected());

    // Getting at least 3 characters into current buffer
    while(m_szTail - m_szHead < 3)
        LoadBuffer(0);

    if(!memcmp(m_Buffer.GetDataPtr() + m_szHead, TEXT(".\r\n"), 3 * sizeof(TCHAR)))
    {
        m_szHead += 3;

        return TEXT('.');
    }

    return ReceiveUntilSubString(TEXT("\r\n.\r\n"), 2);
}

void T_POP3_SMTP_Socket::Verify_POP3_Response(KString* pRPostfix)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsConnected());

    const KString s = ReceiveSingle();

    if(s.GetLength() < 3 || s.Left(3).CollateNoCase(TEXT("+ok")) || s[3] && !_istspace(s[3]))
        INITIATE_DEFINED_FAILURE((KString)TEXT("Errorneous POP3 response: \"") + s + TEXT("\"."));

    if(pRPostfix)
        *pRPostfix = 4 <= s.GetLength() ? (LPCTSTR)s.Mid(4) : TEXT("");
}

void T_POP3_SMTP_Socket::Verify_SMTP_Response(KString* pRPostfix)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsConnected());

    const KString s = ReceiveSingle();

    if(s.GetLength() < 3 || !_istdigit(s[0]) || s[0] != TEXT('2') && s[0] != TEXT('3'))
        INITIATE_DEFINED_FAILURE((KString)TEXT("Errorneous SMTP response: \"") + s + TEXT("\"."));

    if(pRPostfix)
        *pRPostfix = 4 <= s.GetLength() ? (LPCTSTR)s.Mid(4) : TEXT("");
}
