#ifndef document_h
#define document_h

#include <KFC_KTL/variable_list.h>
#include "encoding.h"

// --------------
// HTML document
// --------------
class T_HTML_Document
{
public:
    // Tag
    class TTag
    {
    public:
        // Parameter
        typedef TVariable TParameter;

        // Parameters
        class TParameters : public TVariableList
        {
        public:
            TParameters() : TVariableList(false) {}

            KString Write() const;
        };

    private:
        bool m_bModified;

    public:
        KString     m_OriginalText;
        KString     m_Name;
        TParameters m_Parameters;
        KString     m_TextAfter;
        bool        m_bSelfClose;

    public:
        TTag();

        void Modify();

        KString Write() const;
    };

    // Tags
    typedef TList<TTag> TTags;


    KString m_StartText;

    TTags m_Tags;


    T_HTML_Document() {}

    T_HTML_Document(LPCTSTR s)
        { Parse(s); }

    void Clear();

    void Parse(LPCTSTR s);

    KString Write() const;

    void ClearTags(TTags::TIterator& Iter, LPCTSTR pTerminatorTagNames);

    TTags::TIterator FindTag(   LPCTSTR             pTagName,
                                TTags::TIterator    After = NULL,
                                TTags::TIterator    Till  = NULL)
    {
        return  TTags::TIterator().
                    FromPVoid(  static_cast<const T_HTML_Document*>(this)->
                                    FindTag(pTagName, After, Till).AsPVoid());
    }

    TTags::TConstIterator FindTag(  LPCTSTR                 pTagName,
                                    TTags::TConstIterator   After = NULL,
                                    TTags::TConstIterator   Till  = NULL) const;

    static TTags::TIterator GetNextSame(TTags::TIterator Iter,
                                        TTags::TIterator Till = NULL);

    static TTags::TConstIterator GetNextSame(   TTags::TConstIterator Iter,
                                                TTags::TConstIterator Till = NULL);

    static TTags::TIterator& ToNextSame(TTags::TIterator&   Iter,
                                        TTags::TIterator    Till = NULL)
    {
        return Iter = GetNextSame(Iter, Till);
    }

    static const TTags::TConstIterator& ToNextSame( TTags::TConstIterator&  Iter,
                                                    TTags::TConstIterator   Till = NULL)
    {
        return Iter = GetNextSame(Iter, Till);
    }

    size_t GetNTags(LPCTSTR                 pTagName,
                    TTags::TConstIterator   After = NULL,
                    TTags::TConstIterator   Till  = NULL) const;

    void ModifyTags(TTags::TIterator After = NULL, TTags::TIterator Till = NULL);

    // void Encode(const TEncoding* pEncoding);

    // T_HTML_Document& Decode(const TEncoding* pEncoding);

    KString GetMetaContent( LPCTSTR pMetaName,
                            LPCTSTR pMetaValue,
                            LPCTSTR pDefaultContent = TEXT("")) const;

    KString GetTitle() const;
};

#endif // document_h
