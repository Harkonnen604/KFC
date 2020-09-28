#include "kfc_common_pch.h"
#include "rpc_message_registerer.h"

#ifdef _MSC_VERQ

// -----------------------
// RPC message registerer
// -----------------------
TRPCMessageRegisterer::TRPCMessageRegisterer()
{
    m_szMessage = -1;
}

void TRPCMessageRegisterer::Allocate(LPCTSTR pName)
{
    DEBUG_VERIFY(pName);

    if(IsAllocated() && m_Name == pName)
        return;

    m_Name = pName;

    m_szMessage = RegisterWindowMessage(pName);

    if(m_szMessage == 0)
    {
        m_szMessage = -1;

        INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error registering window message \"") +
                                            pName +
                                            TEXT("\""),
                                        GetLastError());
    }
}

size_t TRPCMessageRegisterer::GetMessage() const
{
    DEBUG_VERIFY_ALLOCATION;

    return m_szMessage;
}

#endif // _MSC_VER
