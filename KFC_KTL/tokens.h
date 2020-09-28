#ifndef tokens_h
#define tokens_h

// -------
// Tokens
// -------
class TTokens
{
private:
    // Storage
    typedef TTrie<KString> TStorage;

public:
    typedef TTrieWalker<KString> TWalker;

    typedef TConstTrieWalker<KString> TConstWalker;

private:
    TStorage m_Storage;

public:
    TTokens() {}

    TTokens(LPCTSTR pSrcString, const KString& DstString)
        { (*this)(pSrcString, DstString); }

    bool IsEmpty() const
        { return m_Storage.IsEmpty(); }

    void Clear()
        { m_Storage.Clear(); }

    TTokens& Add(LPCTSTR pSrcString, const KString& DstString)
        { DEBUG_VERIFY(pSrcString); DEBUG_VERIFY(*pSrcString); *m_Storage.Add(pSrcString) = DstString; return *this; }

    void Del(LPCTSTR pSrcString)
        { DEBUG_VERIFY(pSrcString); DEBUG_VERIFY(*pSrcString); m_Storage.Del(pSrcString); }

    TTokens& operator () (LPCTSTR pSrcString, const KString& DstString)
        { return Add(pSrcString, DstString); }

    KString Process(LPCTSTR pString) const;

    TWalker GetWalker()
        { return TWalker(m_Storage); }

    TConstWalker GetWalker() const
        { return TConstWalker(m_Storage); }

    KString operator () (LPCTSTR pString) const
        { return Process(pString); }

    KString& operator [] (LPCTSTR pSrcString)
        { return m_Storage[pSrcString]; }

    const KString& operator [] (LPCTSTR pSrcString) const
        { return m_Storage[pSrcString]; }

    const KString& ProcessEnumValue(LPCTSTR pString, LPCTSTR pValueName) const;
};

#define FOR_EACH_TOKENS(_tokens, _walker) \
    for(TTokens::TWalker _walker = (_tokens).GetWalker() ; _walker ; ++_walker)

#define FOR_EACH_TOKENS_CONST(_tokens, _walker) \
    for(TTokens::TConstWalker _walker = (_tokens).GetWalker() ; _walker ; ++_walker)

// ------------------
// Tokens registerer
// ------------------
class TTokensRegisterer
{
private:
    TTokens* m_pTokens;

    KStrings m_SrcStrings;

public:
    TTokensRegisterer();

    TTokensRegisterer(TTokens& STokens);

    ~TTokensRegisterer() { Release(); }

    bool IsAllocated() const
        { return m_pTokens; }

    void Release();

    void Allocate(TTokens& STokens);

    void Add(LPCTSTR pSrcString, const KString& DstString);
};

#endif // tokens_h
