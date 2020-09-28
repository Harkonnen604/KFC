#include "kfc_ktl_pch.h"
#include "kstrings.h"

// --------
// Strings
// --------
KStrings::KStrings(const TArray<KString>& Strings)
{
    FOR_EACH_ARRAY(Strings, i)
        AddLast(Strings[i]);
}

KStrings& KStrings::operator = (const TArray<KString>& Strings)
{
    Clear();

    FOR_EACH_ARRAY(Strings, i)
        AddLast(Strings[i]);

    return *this;
}

KStrings::TIterator KStrings::SplitString(  const KString&  String,
                                            LPCTSTR         pSplitter,
                                            kflags_t        flEmptyMode,
                                            bool            bClearFirst)
{
    if(bClearFirst)
        Clear();

    const TIterator Ret = GetLast();

    if(!String.IsEmpty())
    {
        const size_t szSplitterLength = _tcslen(pSplitter);

        size_t szStart = 0;

        for(;;)
        {
            const size_t szPos = String.Find(pSplitter, szStart);

            if(szPos == -1)
            {
                if( szStart < String.GetLength() ||
                        (flEmptyMode & SSEM_TRAILING))
                {
                    *AddLast() = String.Mid(szStart);
                }

                break;
            }

            if( szStart < szPos ||
                    (flEmptyMode & (szStart ? SSEM_INNER : SSEM_LEADING)))
            {
                *AddLast() = String.Mid(szStart, szPos - szStart);
            }

            szStart = szPos + szSplitterLength;
        }
    }

    return Ret.IsValid() ? Ret.GetNext() : GetFirst();
}

KStrings::TIterator KStrings::SplitToTokens(LPCTSTR s,
                                            LPCTSTR pDelimiters,
                                            bool    bClearFirst)
{
    if(bClearFirst)
        Clear();

    DEBUG_VERIFY(pDelimiters && *pDelimiters);

    const TIterator Ret = GetLast();

    KString TempString = s;

    TCHAR *pToken;

    #ifdef _MSC_VER
    {
        for(pToken = _tcstok(TempString.GetDataPtr(), pDelimiters) ;
            pToken ;
            pToken = _tcstok(NULL, pDelimiters))
        {
            *AddLast() = pToken;
        }

        return Ret.IsValid() ? Ret.GetNext() : GetFirst();
    }
    #else // _MSC_VER
    {
        char* temp;

        for(pToken = _tcstok(TempString.GetDataPtr(), pDelimiters, &temp) ;
            pToken ;
            pToken = _tcstok(NULL, pDelimiters, &temp))
        {
            *AddLast() = pToken;
        }

        return Ret.IsValid() ? Ret.GetNext() : GetFirst();
    }
    #endif // _MSC_VER
}

void KStrings::KillDupes()
{
    for(TIterator Iter1 = GetFirst() ; Iter1.IsValid() ; ++Iter1)
    {
        TIterator Iter2, NIter2;

        for(Iter2 = Iter1.GetNext() ; Iter2.IsValid() ; Iter2 = NIter2)
        {
            NIter2 = Iter2.GetNext();

            if(!Iter1->Collate(*Iter2))
                Del(Iter2);
        }
    }
}

void KStrings::KillDupesNoCase()
{
    for(TIterator Iter1 = GetFirst() ; Iter1.IsValid() ; ++Iter1)
    {
        TIterator Iter2, NIter2;

        for(Iter2 = Iter1.GetNext() ; Iter2.IsValid() ; Iter2 = NIter2)
        {
            NIter2 = Iter2.GetNext();

            if(!Iter1->CollateNoCase(*Iter2))
                Del(Iter2);
        }
    }
}

bool KStrings::HasString(const KString& String) const
{
    for(TConstIterator Iter = GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        if(!Iter->Compare(String))
            return true;
    }

    return false;
}

bool KStrings::HasStringNoCase(const KString& String) const
{
    for(TConstIterator Iter = GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        if(!Iter->CompareNoCase(String))
            return true;
    }

    return false;
}

bool KStrings::HasValueString(const KString& String, KString& RValue) const
{
    const size_t l = String.GetLength();

    for(TConstIterator Iter = GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        if( Iter->GetLength() >= l + 1  &&
            (*Iter)[l] == TEXT('=')     &&
            Iter->DoesStart(String))
        {
            RValue = Iter->Mid(l + 1);

            return true;
        }
    }

    return false;
}

bool KStrings::HasValueStringNoCase(const KString& String, KString& RValue) const
{
    const size_t l = String.GetLength();

    for(TConstIterator Iter = GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        if( Iter->GetLength() >= l + 1  &&
            (*Iter)[l] == TEXT('=')     &&
            Iter->DoesStartNoCase(String))
        {
            RValue = Iter->Mid(l + 1);

            return true;
        }
    }

    return false;
}

KString KStrings::GenerateString(   LPCTSTR pDelimiter,
                                    bool    bLastDelimiter) const
{
    KString String;

    for(TConstIterator Iter = GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        String += *Iter;

        if(Iter != GetLast() || bLastDelimiter)
            String += pDelimiter;
    }

    return String;
}

KStrings::TIterator KStrings::SplitQuotedString(LPCTSTR pString, bool bClearFirst)
{
    size_t i;

    if(bClearFirst)
        Clear();

    const KStrings::TIterator Ret = GetFirst();

    KString CurToken;
    bool    bInToken    = false;
    bool    bQuoted     = false;

    for(i = 0 ; pString[i] ; i++)
    {
        if(pString[i] == TEXT('"'))
        {
            if(bQuoted)
            {
                *AddLast() = CurToken, CurToken.Empty();

                bInToken = false;

                bQuoted = false;
            }
            else
            {
                if(bInToken)
                    *AddLast() = CurToken, CurToken.Empty();

                bInToken = true;

                bQuoted = true;
            }
        }
        else
        {
            if(_istspace(pString[i]) && !bQuoted) // delimiter
            {
                if(bInToken)
                    *AddLast() = CurToken, CurToken.Empty();

                bInToken = false;
            }
            else // plain character
            {
                CurToken += pString[i];

                bInToken = true;
            }
        }
    }

    if(bInToken)
        *AddLast() = CurToken;

    return Ret.IsValid() ? Ret.GetNext() : GetFirst();
}

void KStrings::TrimAll()
{
    FOR_EACH_LIST(*this, KStrings::TIterator, Iter)
        Iter->Trim();
}
