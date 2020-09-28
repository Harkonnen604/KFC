#include "kfc_cgi_pch.h"
#include "cgi_parameters.h"

#include <KFC_Formats/encoding.h>

// ---------------
// CGI parameters
// ---------------
T_CGI_Parameters::TIterator T_CGI_Parameters::Parse(LPCTSTR s, bool bClearFirst)
{
    if(bClearFirst)
        Clear();

    const TIterator Ret = GetLast();

    KStrings Items;
    Items.SplitString(s, TEXT("&"), SSEM_ALL);

    for(KStrings::TConstIterator Iter = Items.GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        size_t i;

        if((i = Iter->Find(TEXT('='))) != UINT_MAX)
        {
            Add(DecodeCGI_Parameter(Iter->Left(i).  Trim()),
                DecodeCGI_Parameter(Iter->Mid (i+1).Trim()));
        }
        else
        {
            Add(DecodeCGI_Parameter(*Iter), TEXT(""));
        }
    }

    return Ret.IsValid() ? Ret.GetNext() : GetFirst();
}

KString T_CGI_Parameters::Write() const
{
    KString Text;

    for(TConstIterator Iter = GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        if(!Text.IsEmpty())
            Text += TEXT('&');

        Text += EncodeCGI_Parameter(Iter->m_Name);
        Text += TEXT('=');
        Text += EncodeCGI_Parameter(Iter->m_Value);
    }

    return Text;
}

T_CGI_Parameters::TIterator T_CGI_Parameters::Read(kflags_t flReadOptions, bool bClearFirst)
{
    if(bClearFirst)
        Clear();

    const TIterator Ret = GetLast();

    if(flReadOptions & CGI_RO_GET)
        Parse(getenv_safe(TEXT("QUERY_STRING")));

    if(flReadOptions & CGI_RO_POST)
    {
        KString Query;

        for(;;)
        {
            const char c = getc(stdin);

            if(!c || c == (char)-1 || c == (char)0xFF || c == TEXT('\n'))
                break;

            Query += c;
        }

        Parse(Query, false);
    }

    return Ret.IsValid() ? Ret.GetNext() : GetFirst();
}

bool T_CGI_Parameters::HasIndexed(LPCTSTR pName, size_t& szRIndex) const
{
    const size_t l = _tcslen(pName);

    for(TConstIterator Iter = GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        if( (IsCaseSensitive() ?
                !_tcsncmp (Iter->m_Name, pName, l) :
                !_tcsnicmp(Iter->m_Name, pName, l)) &&

            FromString(Iter->m_Name.Mid(l), szRIndex))
        {
            return true;
        }
    }

    return false;
}

bool T_CGI_Parameters::HasIndexed(LPCTSTR pName, size_t* pRIndices, size_t szN) const
{
    const size_t l = _tcslen(pName);

    for(TConstIterator Iter = GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        if( IsCaseSensitive() ?
                !_tcsncmp (Iter->m_Name, pName, l) :
                !_tcsnicmp(Iter->m_Name, pName, l))
        {
            KStrings Tokens;

            Tokens.SplitString(Iter->m_Name.Mid(l), "_");

            if(Tokens.GetN() == szN)
            {
                KStrings::TConstIterator Iter;

                size_t i;

                for(Iter = Tokens.GetFirst(), i = 0 ; Iter.IsValid() ; ++Iter, ++i)
                {
                    if(!FromString(*Iter, pRIndices[i]))
                        break;
                }

                if(!Iter.IsValid())
                {
                    DEBUG_VERIFY(i == szN);

                    return true;
                }
            }
        }
    }

    return false;
}
