#ifndef file_chars_tree_h
#define file_chars_tree_h

#include <KFC_KTL/fixed_item_heap.h>
#include <KFC_Common/file_mapping.h>

// ----------------
// File chars tree
// ----------------
template <class t>
class TFileCharsTree
{
public:
    // Iterator
    struct TIterator
    {
    public:
        size_t x;

    public:
        TIterator()
            { Invalidate(); }

        TIterator(size_t sx) : x(sx) {}

        bool IsValid() const
            { return x; }

        void Invalidate()
            { x = 0; }

        bool IsRoot() const
            { return x == -1; }
    };

    // Const iterator
    struct TConstIterator
    {
    public:
        size_t x;

    public:
        TConstIterator()
            { Invalidate(); }

        TConstIterator(size_t sx) : x(sx) {}

        TConstIterator(const TIterator &SIter) : x(SIter.x) {}

        bool IsValid() const
            { return x; }

        void Invalidate()
            { x = 0; }

        bool IsRoot() const
            { return x == -1; }
    };

private:
    // Node
    struct TNode
    {
        size_t m_szChar;

        TIterator m_FirstChild;
        TIterator m_NextSibling;

        t m_Data;
    };

    // Heap
    typedef TFixedItemHeapWithAux<TNode, TFileMappingGrowableAllocator, TIterator> THeap;

private:
    THeap m_Heap;

    bool m_bAllocated;

private:
    TNode& GetNode(TIterator Iter)
        { return m_Heap[Iter.x]; }

    const TNode& GetNode(TConstIterator Iter) const
        { return m_Heap[Iter.x]; }

    TIterator& InternalGetFirstChild(TIterator Iter)
        { return Iter.IsRoot() ? m_Heap.GetAuxData() : GetNode(Iter).m_FirstChild; }

    TConstIterator InternalGetFirstChild(TConstIterator Iter) const
        { return Iter.IsRoot() ? m_Heap.GetAuxData() : GetNode(Iter).m_FirstChild; }

    TIterator& InternalGetNextSibling(TIterator Iter)
        { return GetNode(Iter).m_NextSibling; }

    TConstIterator InternalGetNextSibling(TConstIterator Iter) const
        { return GetNode(Iter).m_NextSibling; }

    TIterator& InternalToFirstChild(TIterator& Iter)
        { return Iter = InternalGetFirstChild(Iter); }

    TConstIterator& InternalToFirstChild(TConstIterator& Iter) const
        { return Iter = InternalGetFirstChild(Iter); }

    TIterator& InternalToNextSibling(TIterator& Iter)
        { return Iter = InternalGetNextSibling(Iter); }

    TConstIterator& InternalToNextSibling(TConstIterator& Iter) const
        { return Iter = InternalGetNextSibling(Iter); }

    size_t InternalGetChar(TConstIterator Iter) const
        { return GetNode(Iter).m_szChar; }

    void DelSubTree(TIterator Iter);

public:
    TFileCharsTree();

    ~TFileCharsTree()
        { Release(); }

    bool IsAllocated() const
        { return m_bAllocated; }

    void Release();

    void Allocate(LPCTSTR pFileName);

    void Create(LPCTSTR pFileName);

    void Open(LPCTSTR pFileName, bool bReadOnly);

    void Clear();

    static TIterator GetRoot()
        { return -1; }

    static TIterator& ToRoot(TIterator& Iter)
        { return Iter = GetRoot(); }

    static TConstIterator& ToRoot(TConstIterator& Iter)
        { return Iter = GetRoot(); }

    TIterator GetFirstChild(TIterator Iter)
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(Iter.IsValid());

        return InternalGetFirstChild(Iter);
    }

    TConstIterator GetFirstChild(TConstIterator Iter) const
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(Iter.IsValid());

        return InternalGetFirstChild(Iter);
    }

    TIterator GetNextSibling(TIterator Iter)
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(Iter.IsValid());

        return Iter.IsRoot() ? 0 : InternalGetNextSibling(Iter);
    }

    TConstIterator GetNextSibling(TConstIterator Iter) const
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(Iter.IsValid());

        DEBUG_VERIFY(!Iter.IsRoot());

        return Iter.IsRoot() ? 0 : InternalGetNextSibling(Iter);
    }

    TIterator& ToFirstChild(TIterator& Iter)
        { return Iter = GetFirstChild(Iter); }

    TConstIterator& ToFirstChild(TConstIterator& Iter) const
        { return Iter = GetFirstChild(Iter); }

    TIterator& ToNextSibling(TIterator& Iter)
        { return Iter = GetNextSibling(Iter); }

    TConstIterator& ToNextSibling(TConstIterator& Iter) const
        { return Iter = GetNextSibling(Iter); }

    bool IsLeaf(TConstIterator Iter) const
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(Iter.IsValid());

        return !GetFirstChild(Iter).IsValid();
    }

    bool IsLastChild(TConstIterator Iter) const
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(Iter.IsValid());

        return Iter.IsRoot() || !InternalGetNextSibling(Iter).IsValid();
    }

    TIterator FindChild(TIterator Iter, size_t szChar);

    TConstIterator FindChild(TConstIterator Iter, size_t szChar) const
        { return const_cast<TFileCharsTree*>(this)->FindChild(TIterator(Iter.x), szChar); }

    TIterator GetChild(TIterator Iter, size_t szChar)
    {
        Iter = FindChild(Iter, szChar);

        DEBUG_VERIFY(Iter.IsValid());

        return Iter;
    }

    TConstIterator GetChild(TConstIterator Iter, size_t szChar) const
        { return const_cast<TFileCharsTree*>(this)->GetChild(TIterator(Iter.x), szChar); }

    bool HasChild(TConstIterator Iter, size_t szChar) const
        { return FindChild(Iter, szChar).IsValid(); }

    TIterator InsertFirstChild(TIterator Iter, size_t szChar);

    TIterator InsertNextSibling(TIterator Iter, size_t szChar);

    TIterator InsertChild(TIterator Iter, size_t szChar);

    void DelFirstChild(TIterator Iter);

    void DelNextSibling(TIterator Iter);

    size_t GetChar(TConstIterator Iter) const
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(Iter.IsValid());

        DEBUG_VERIFY(!Iter.IsRoot());

        return InternalGetChar(Iter);
    }

    t* GetDataPtr(TIterator Iter)
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(Iter.IsValid());

        DEBUG_VERIFY(!Iter.IsRoot());

        return &GetNode(Iter).m_Data;
    }

    const t* GetDataPtr(TConstIterator Iter) const
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(Iter.IsValid());

        DEBUG_VERIFY(!Iter.IsRoot());

        return &GetNode(Iter).m_Data;
    }

    t& GetDataRef (TIterator Iter)
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(Iter.IsValid());

        DEBUG_VERIFY(!Iter.IsRoot());

        return GetNode(Iter).m_Data;
    }

    const t& GetDataRef(TConstIterator Iter) const
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(Iter.IsValid());

        DEBUG_VERIFY(!Iter.IsRoot());

        return GetNode(Iter).m_Data;
    }

    t& operator [] (TIterator Iter)
        { return GetDataRef(Iter); }

    const t& operator [] (TConstIterator Iter) const
        { return GetDataRef(Iter); }

    bool IsReadOnly() const
        { DEBUG_VERIFY_ALLOCATION; return m_Heap.m_Allocator.IsReadOnly(); }

    THeap& GetHeap()
        { DEBUG_VERIFY_ALLOCATION; return m_Heap; }

    const THeap& GetHeap() const
        { DEBUG_VERIFY_ALLOCATION; return m_Heap; }
};

template <class t>
TFileCharsTree<t>::TFileCharsTree()
{
    m_bAllocated = false;
}

template <class t>
void TFileCharsTree<t>::Release()
{
    m_Heap.m_Allocator.Release();
}

template <class t>
void TFileCharsTree<t>::Allocate(LPCTSTR pFileName)
{
    Release();

    if(FileExists(pFileName))
        Open(pFileName, false);
    else
        Create(pFileName);
}

template <class t>
void TFileCharsTree<t>::Create(LPCTSTR pFileName)
{
    Release();

    try
    {
        m_Heap.m_Allocator.Allocate(pFileName, false, 0);

        m_Heap.AllocateWithAux();

        m_bAllocated = true;

        Clear();
    }

    catch(...)
    {
        Release();
        throw;
    }
}

template <class t>
void TFileCharsTree<t>::Open(LPCTSTR pFileName, bool bReadOnly)
{
    Release();

    try
    {
        m_Heap.m_Allocator.Allocate(pFileName, bReadOnly);

        m_Heap.AllocateWithAux();

        m_bAllocated = true;
    }

    catch(...)
    {
        Release();
        throw;
    }
}

template <class t>
void TFileCharsTree<t>::Clear()
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsReadOnly());

    m_Heap.ClearWithAux();
}

template <class t>
typename TFileCharsTree<t>::TIterator TFileCharsTree<t>::FindChild(TIterator Iter, size_t szChar)
{
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(Iter.IsValid());

        int d;

        for(ToFirstChild(Iter) ;
            Iter.IsValid() && (d = GetChar(Iter) - szChar) <= 0 ;
            ToNextSibling(Iter))
        {
            if(!d)
                return Iter;
        }

        return 0;
    }

template <class t>
typename TFileCharsTree<t>::TIterator TFileCharsTree<t>::InsertFirstChild(TIterator Iter, size_t szChar)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsReadOnly());

    DEBUG_VERIFY(Iter.IsValid());

    const size_t szOffset = m_Heap.Reserve();

    TNode& Node = m_Heap[szOffset];

    TIterator& FirstChild = InternalGetFirstChild(Iter);

    DEBUG_VERIFY(!FirstChild.IsValid() || szChar < InternalGetChar(FirstChild));

    Node.m_szChar = szChar;

    Node.m_NextSibling = FirstChild, FirstChild = szOffset;

    return szOffset;
}

template <class t>
typename TFileCharsTree<t>::TIterator TFileCharsTree<t>::InsertNextSibling(TIterator Iter, size_t szChar)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsReadOnly());

    DEBUG_VERIFY(Iter.IsValid());

    DEBUG_VERIFY(!Iter.IsRoot());

    DEBUG_VERIFY(InternalGetChar(Iter) < szChar);

    const size_t szOffset = m_Heap.Reserve();

    TNode& Node = GetNode(szOffset);

    TIterator& NextSibling = InternalGetNextSibling(Iter);

    DEBUG_VERIFY(!NextSibling.IsValid() || szChar < InternalGetChar(NextSibling));

    Node.m_szChar = szChar;

    Node.m_NextSibling = NextSibling, NextSibling = szOffset;

    return szOffset;
}

template <class t>
typename TFileCharsTree<t>::TIterator
    TFileCharsTree<t>::InsertChild(TIterator Iter, size_t szChar)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsReadOnly());

    DEBUG_VERIFY(Iter.IsValid());

    TIterator CIter = Iter;
    TIterator PIter;

    for(CIter = GetFirstChild(PIter = CIter) ;
        CIter.IsValid() ;
        CIter = GetNextSibling(PIter = CIter))
    {
        const int d = Compare(GetChar(CIter), szChar);

        if(!d)
            return CIter;

        if(d > 0)
            break;
    }

    return  PIter == Iter ?
                InsertFirstChild (PIter, szChar) :
                InsertNextSibling(PIter, szChar);
}

template <class t>
void TFileCharsTree<t>::DelSubTree(TIterator Iter)
{
    for(TIterator Iter2 = InternalGetFirstChild(Iter) ;
        Iter2.IsValid() ;
        InternalToNextSibling(Iter2))
    {
        DelSubTree(Iter2);
    }

    m_Heap.Free(Iter.x);
}

template <class t>
void TFileCharsTree<t>::DelFirstChild(TIterator Iter)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsReadOnly());

    DEBUG_VERIFY(Iter.IsValid());

    TIterator& FirstChild = InternalGetFirstChild(Iter);

    const TIterator DelIter = FirstChild;

    DEBUG_VERIFY(DelIter.IsValid());

    FirstChild = InternalGetNextSibling(DelIter);

    DelSubTree(DelIter);
}

template <class t>
void TFileCharsTree<t>::DelNextSibling(TIterator Iter)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsReadOnly());

    DEBUG_VERIFY(Iter.IsValid());

    DEBUG_VERIFY(!Iter.IsRoot());

    TIterator& NextSibling = InternalGetNextSibling(Iter);

    const TIterator DelIter = NextSibling;

    DEBUG_VERIFY(DelIter.IsValid());

    NextSibling = InternalGetNextSibling(DelIter);

    DelSubTree(DelIter);
}

template <class t>
inline int Compare(struct TFileCharsTree<t>::TIterator Iter1, struct TFileCharsTree<t>::TIterator Iter2)
    { return Compare(Iter1.x, Iter2.x); }

DECLARE_TEMPLATE_COMPARISON_OPERATORS(class t, struct TFileCharsTree<t>::TIterator, struct TFileCharsTree<t>::TIterator);

#endif // file_chars_tree_h
