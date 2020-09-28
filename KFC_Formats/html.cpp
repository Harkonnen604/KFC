#include "kfc_formats_pch.h"
#include "html.h"

#include "encoding.h"

// --------------
// HTML Document
// --------------

// Tag::Parameters
KString T_HTML_Document::TTag::TParameters::Write() const
{
    KString Text;

    for(TConstIterator Iter = GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        Text += TEXT(' ');
        Text += Iter->m_Name;
        Text += TEXT("=\"");
        Text += Iter->m_Value;
        Text += TEXT('"');
    }

    return Text;
}

// Tag
T_HTML_Document::TTag::TTag()
{
    m_bSelfClose = false;

    m_bModified = false;
}

void T_HTML_Document::TTag::Modify()
{
    m_bModified = true;
}

KString T_HTML_Document::TTag::Write() const
{
    if(!m_bModified)
        return m_OriginalText;

    KString Text;

    if(!m_Name.IsEmpty())
    {
        Text += TEXT("<");
        Text += m_Name;
        Text += m_Parameters.Write();
        Text += TEXT(">");
    }

    Text += m_TextAfter;

    return Text;
}

static inline bool IsSpaceSkippableChar(TCHAR c)
{
    return _istalnum(c) || c == TEXT('_');
}

static inline bool IsTermText(LPCTSTR s, LPCTSTR pText)
{
    if(*s != *pText)
        return false;

    s++, pText++;

    while(*s && *pText)
    {
        if(_istspace(*s))
        {
            if(!IsSpaceSkippableChar(*(pText-1)) && !IsSpaceSkippableChar(*pText))
                return false;

            s++;
        }
        else
        {
            if(_totlower(*s) != _totlower(*pText))
                return false;

            s++, pText++;
        }
    }

    return !*pText;
}

static void ReadTextAfter(LPCTSTR& s, LPCTSTR pTermText, KString& RText)
{
    RText.Empty();

    while(*s)
    {
        if(!_tcsncmp(s, TEXT("<!--"), 4))
        {
            RText += TEXT("<!--"), s += 4;

            while(*s && _tcsncmp(s, TEXT("-->"), 3))
                RText += *s++;

            RText += TEXT("-->"), s += 3;
        }
        else
        {
            if(IsTermText(s, pTermText))
                break;

            RText += *s++;
        }
    }
}

// HTML document
void T_HTML_Document::Clear()
{
    m_StartText.Empty();

    m_Tags.Clear();
}

void T_HTML_Document::Parse(LPCTSTR s)
{
    ReadTextAfter(s, TEXT("<"), m_StartText);

    m_Tags.Clear();

    while(*s)
    {
        TTag& Tag = *m_Tags.AddLast();

        // Tag name and parameters
        if(*s == TEXT('<'))
        {
            // Skipping '<'
            Tag.m_OriginalText += *s++;

            // Pre-name spaces
            while(*s && *s != TEXT('>') && !(s[0] == TEXT('/') && s[1] == TEXT('>')) && _istspace(*s))
                Tag.m_OriginalText += *s++;

            // Name
            while(*s && *s != TEXT('>') && !(s[0] == TEXT('/') && s[1] == TEXT('>')) && !_istspace(*s))
                Tag.m_Name += *s, Tag.m_OriginalText += *s++;

            // Parameters
            for(;;)
            {
                // Pre-parameter spaces
                while(*s && *s != TEXT('/') && *s != TEXT('>') && !(s[0] == TEXT('/') && s[1] == TEXT('>')) && _istspace(*s))
                    Tag.m_OriginalText += *s++;

                if(!*s || *s == TEXT('>') || s[0] == TEXT('/') && s[1] == TEXT('>')) // no more parameters
                    break;

                TTag::TParameter& Parameter = *Tag.m_Parameters.AddLast();

                if(*s == TEXT('\'') || *s == TEXT('"')) // quoted name
                {
                    const TCHAR qc = *s;

                    // Skipping opening quote
                    Tag.m_OriginalText += *s++;

                    // Parameter name
                    while(*s && *s != qc)
                        Parameter.m_Name += *s, Tag.m_OriginalText += *s++;

                    // Skipping closing quote
                    if(*s == qc)
                        Tag.m_OriginalText += *s++;
                }
                else // non-quoted names
                {
                    // Parameter name
                    while(*s && *s != TEXT('>') && !(s[0] == TEXT('/') && s[1] == TEXT('>')) && *s != TEXT('=') && !_istspace(*s))
                        Parameter.m_Name += *s, Tag.m_OriginalText += *s++;
                }

                // Pre-'=' spaces
                while(*s && *s != TEXT('>') && !(s[0] == TEXT('/') && s[1] == TEXT('>')) && _istspace(*s))
                    Tag.m_OriginalText += *s++;

                if(*s != TEXT('=')) // no '='
                    continue;

                // Skipping '='
                Tag.m_OriginalText += *s++;

                // Pre-value spaces
                while(*s && *s != TEXT('>') && !(s[0] == TEXT('/') && s[1] == TEXT('>')) && _istspace(*s))
                    Tag.m_OriginalText += *s++;

                if(*s == TEXT('\'') || *s == TEXT('"')) // quoted value
                {
                    const TCHAR qc = *s;

                    // Skipping opening quote
                    Tag.m_OriginalText += *s++;

                    // Parameter value
                    while(*s && *s != qc)
                        Parameter.m_Value += *s, Tag.m_OriginalText += *s++;

                    // Skipping closing quote
                    if(*s == qc)
                        Tag.m_OriginalText += *s++;
                }
                else // non-quoted value
                {
                    // Parameter value
                    while(*s && *s != TEXT('>') && !(s[0] == TEXT('/') && s[1] == TEXT('>')) && !_istspace(*s))
                        Parameter.m_Value += *s, Tag.m_OriginalText += *s++;
                }
            }

            // Skipping '>' or '/>
            if(*s == TEXT('>'))
                Tag.m_OriginalText += *s++;
            else if(s[0] == TEXT('/') && s[1] == TEXT('>'))
                Tag.m_OriginalText += *s++, Tag.m_OriginalText += *s++, Tag.m_bSelfClose = true;
        }

        // Text after
        {
            static const LPCTSTR ppTextTags[] =
                {   TEXT("script"),
                    TEXT("textarea"),
                    TEXT("option"),
                    TEXT("pre"),
                    TEXT("title"),
                    TEXT("style")};

            KString TermText;

            size_t j;

            for(j = 0 ; j < ARRAY_SIZE(ppTextTags) ; j++)
            {
                if(!Tag.m_Name.CompareNoCase(ppTextTags[j]))
                    break;
            }

            if(j < ARRAY_SIZE(ppTextTags)) // text tag
                TermText = (KString)TEXT("</") + ppTextTags[j] + TEXT(">");
            else
                TermText = TEXT("<");

            ReadTextAfter(s, TermText, Tag.m_TextAfter);

            Tag.m_OriginalText += Tag.m_TextAfter;
        }
    }
}

KString T_HTML_Document::Write() const
{
    KString Text;

    Text += m_StartText;

    for(TTags::TConstIterator Iter = m_Tags.GetFirst() ; Iter.IsValid() ; ++Iter)
        Text += Iter->Write();

    return Text;
}

void T_HTML_Document::ClearTags(TTags::TIterator& Iter, LPCTSTR pTerminatorTagNames)
{
    KStrings Tokens;

    Tokens.SplitToTokens(pTerminatorTagNames, TEXT(" \t\r\n"));

    TTags::TIterator NIter;

    for( ; Iter.IsValid() ; Iter = NIter)
    {
        if(Tokens.HasStringNoCase(Iter->m_Name))
            break;

        NIter = Iter.GetNext(), m_Tags.Del(Iter);
    }
}

T_HTML_Document::TTags::TConstIterator T_HTML_Document::FindTag(LPCTSTR                 pTagName,
                                                                TTags::TConstIterator   After,
                                                                TTags::TConstIterator   Till) const
{
    for(TTags::TConstIterator Iter = After.IsValid() ? After.GetNext() : m_Tags.GetFirst() ;
        Iter != Till ;
        ++Iter)
    {
        if(!Iter->m_Name.CompareNoCase(pTagName))
            return Iter;
    }

    return NULL;
}

T_HTML_Document::TTags::TIterator
    T_HTML_Document::GetNextSame(TTags::TIterator Iter,
                                 TTags::TIterator Till)
{
    const KString& Name = Iter->m_Name;

    for(++Iter ; Iter != Till ; ++Iter)
    {
         if(!CompareNoCase(Iter->m_Name, Name))
             return Iter;
    }

    return NULL;
}

T_HTML_Document::TTags::TConstIterator
    T_HTML_Document::GetNextSame(   TTags::TConstIterator Iter,
                                    TTags::TConstIterator Till)
{
    const KString& Name = Iter->m_Name;

    for(++Iter ; Iter != Till ; ++Iter)
    {
         if(!CompareNoCase(Iter->m_Name, Name))
             return Iter;
    }

    return NULL;
}

size_t T_HTML_Document::GetNTags(   LPCTSTR                 pTagName,
                                    TTags::TConstIterator   After,
                                    TTags::TConstIterator   Till) const
{
    size_t szN = 0;

    for(TTags::TConstIterator Iter = After.IsValid() ? After.GetNext() : m_Tags.GetFirst() ;
        Iter != Till ;
        ++Iter)
    {
        if(!Iter->m_Name.CompareNoCase(pTagName))
            szN++;
    }

    return szN;
}

void T_HTML_Document::ModifyTags(TTags::TIterator After, TTags::TIterator Till)
{
    for(TTags::TIterator Iter = After.IsValid() ? After.GetNext() : m_Tags.GetFirst() ;
        Iter != Till ;
        ++Iter)
    {
        Iter->Modify();
    }
}

/*
void T_HTML_Document::Encode(const TEncoding* pEncoding)
{
    if(!pEncoding)
        return;

    pEncoding->EncodeSelf(m_StartText.GetDataPtr());

    for(TTags::TIterator Iter = m_Tags.GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        for(TTag::TParameters::TIterator PIter = Iter->m_Parameters.GetFirst() ;
            PIter.IsValid() ;
            ++PIter)
        {
            pEncoding->EncodeSelf(PIter->m_Value.GetDataPtr());
        }

        pEncoding->EncodeSelf(Iter->m_TextAfter.GetDataPtr());
    }
}

T_HTML_Document& T_HTML_Document::Decode(const TEncoding* pEncoding)
{
    if(!pEncoding)
        return *this;

    pEncoding->DecodeSelf(m_StartText.GetDataPtr());

    for(TTags::TIterator Iter = m_Tags.GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        for(TTag::TParameters::TIterator PIter = Iter->m_Parameters.GetFirst() ;
            PIter.IsValid() ;
            ++PIter)
        {
            pEncoding->DecodeSelf(PIter->m_Value.GetDataPtr());
        }

        pEncoding->DecodeSelf(Iter->m_TextAfter.GetDataPtr());
    }

    return *this;
}
*/

KString T_HTML_Document::GetMetaContent(LPCTSTR pMetaName,
                                        LPCTSTR pMetaValue,
                                        LPCTSTR pDefaultContent) const
{
    DEBUG_VERIFY(*pMetaValue);

    for(TTags::TConstIterator Iter = m_Tags.GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        if( !Iter->m_Name.CompareNoCase(TEXT("meta")) &&
            !CompareNoCase(Iter->m_Parameters[pMetaName], pMetaValue))
        {
            return Iter->m_Parameters.Get(TEXT("content"), pDefaultContent);
        }
    }

    return pDefaultContent;
}

KString T_HTML_Document::GetTitle() const
{
    TTags::TConstIterator Iter = FindTag(TEXT("title"));

    return Iter.IsValid() ? (LPCTSTR)Iter->m_TextAfter : TEXT("");
}
