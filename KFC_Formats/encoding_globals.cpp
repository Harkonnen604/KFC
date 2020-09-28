#include "kfc_formats_pch.h"
#include "encoding_globals.h"

TEncodingGlobals g_EncodingGlobals;

// -----------------
// Encoding globals
// -----------------
TEncodingGlobals::TEncodingGlobals() : TGlobals(TEXT("Encoding globals"))
{
}

void TEncodingGlobals::OnUninitialize()
{
    // m_Encodings.Clear();
}

void TEncodingGlobals::OnInitialize()
{
    // OEM encoding
    // m_Encodings.AddLast()->AllocateOEM();
}

/*
const TEncoding* TEncodingGlobals::FindEncoding(LPCTSTR pName) const
{
    DEBUG_VERIFY_INITIALIZATION;

    DEBUG_VERIFY(pName);

    if(!*pName)
        return NULL;

    for(TEncodings::TConstIterator Iter = m_Encodings.GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        if(!Iter->GetName().CollateNoCase(pName))
            return &*Iter;
    }

    return NULL;
}

const TEncoding& TEncodingGlobals::GetEncoding(LPCTSTR pName) const
{
    DEBUG_VERIFY_INITIALIZATION;

    DEBUG_VERIFY(pName && pName[0]);

    const TEncoding* const pEncoding = FindEncoding(pName);

    if(pEncoding == NULL)
        INITIATE_DEFINED_FAILURE((KString)TEXT("Encoding \"") + pName + TEXT("\" not found."));

    return *pEncoding;
}
*/
