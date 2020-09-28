#ifndef fixed_item_heap_avl_storage_h
#define fixed_item_heap_avl_storage_h

#include "fixed_item_heap.h"
#include "fixed_item_heap_accessors.h"

// ---------------------------------
// Fixed item heap AVL storage base
// ---------------------------------

// Iterator
template <class ObjectType>
struct TFixedItemHeapAVL_StorageIterator
{
public:
    size_t x;

public:
    TFixedItemHeapAVL_StorageIterator()
        { Invalidate(); }

    TFixedItemHeapAVL_StorageIterator(size_t sx) : x(sx) {}

    bool IsValid() const
        { return x; }

    void Invalidate()
        { x = 0; }
};

// Const iterator
template <class ObjectType>
struct TFixedItemHeapAVL_StorageConstIterator
{
public:
    size_t x;

public:
    TFixedItemHeapAVL_StorageConstIterator()
        { Invalidate(); }

    TFixedItemHeapAVL_StorageConstIterator(size_t sx) : x(sx) {}

    TFixedItemHeapAVL_StorageConstIterator(TFixedItemHeapAVL_StorageIterator<ObjectType> i) : x(i.x) {}

    bool IsValid() const
        { return x; }

    void Invalidate()
        { x = 0; }
};

template <class ObjectType>
inline int Compare( TFixedItemHeapAVL_StorageIterator<ObjectType> Iter1,
                    TFixedItemHeapAVL_StorageIterator<ObjectType> Iter2)
{
    return Compare(Iter1.x, Iter2.x);
}

template <class ObjectType>
inline int Compare( TFixedItemHeapAVL_StorageIterator       <ObjectType> Iter1,
                    TFixedItemHeapAVL_StorageConstIterator  <ObjectType> Iter2)
{
    return Compare(Iter1.x, Iter2.x);
}

template <class ObjectType>
inline int Compare( TFixedItemHeapAVL_StorageConstIterator  <ObjectType> Iter1,
                    TFixedItemHeapAVL_StorageIterator       <ObjectType> Iter2)
{
    return Compare(Iter1.x, Iter2.x);
}

template <class ObjectType>
inline int Compare( TFixedItemHeapAVL_StorageConstIterator<ObjectType> Iter1,
                    TFixedItemHeapAVL_StorageConstIterator<ObjectType> Iter2)
{
    return Compare(Iter1.x, Iter2.x);
}

DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TFixedItemHeapAVL_StorageIterator       <ObjectType>, TFixedItemHeapAVL_StorageIterator     <ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TFixedItemHeapAVL_StorageIterator       <ObjectType>, TFixedItemHeapAVL_StorageConstIterator<ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TFixedItemHeapAVL_StorageConstIterator  <ObjectType>, TFixedItemHeapAVL_StorageIterator     <ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TFixedItemHeapAVL_StorageConstIterator  <ObjectType>, TFixedItemHeapAVL_StorageConstIterator<ObjectType>)

// Header
template <class ObjectType>
struct TFixedItemHeapAVL_StorageHeader
{
public:
    TFixedItemHeapAVL_StorageIterator<ObjectType> m_Root;

    size_t m_szN;

public:
    TFixedItemHeapAVL_StorageHeader()
        { m_Root.Invalidate(), m_szN = 0; }

        // (for testing purposes)
    bool IsClean() const
        { return !m_Root.IsValid() && !m_szN; }

        // (for testing purposes)
    bool operator == (const TFixedItemHeapAVL_StorageHeader& Header) const
        { return m_Root == Header.m_Root && m_szN == Header.m_szN; }
};

// (base required to avoid template tempalte argument with iterator comparisons)

template <class ObjectType>
class TFixedItemHeapAVL_StorageBase
{
public:
    // Iterator
    typedef TFixedItemHeapAVL_StorageIterator<ObjectType> TIterator;

    // Const iterator
    typedef TFixedItemHeapAVL_StorageConstIterator<ObjectType> TConstIterator;

    // Header
    typedef TFixedItemHeapAVL_StorageHeader<ObjectType> THeader;

    // Item
    struct TItem
    {
    public:
        ObjectType data;

        size_t p, l, r;

        size_t h;

    public:
        template <class KeyType>
            TItem(const KeyType& key) : data(key), p(0), l(0), r(0) {}

        template <class KeyType, class CreatorType>
            TItem(const KeyType& key, const CreatorType& sdata) : data(key, sdata), p(0), l(0), r(0) {}
    };
};

// ----------------------------
// Fixed item heap AVL storage
// ----------------------------
#define FIXED_ITEM_HEAP_AVL_STORAGE(ObjectType, HeapAllocatorType, AccessorTemplate) \
    TFixedItemHeapAVL_Storage \
    <   ObjectType, \
        HeapAllocatorType, \
        AccessorTemplate \
        <   TFixedItemHeapWithAux \
            <   TFixedItemHeapAVL_StorageBase< ObjectType >::TItem, \
                HeapAllocatorType, \
                TFixedItemHeapAVL_StorageBase< ObjectType >::THeader>, \
            TFixedItemHeapAVL_StorageBase< ObjectType >::THeader> > \

#define FIXED_ITEM_HEAP_AVL_STORAGE_WITH_TYPENAME(ObjectType, HeapAllocatorType, AccessorTemplate) \
    TFixedItemHeapAVL_Storage \
    <   ObjectType, \
        HeapAllocatorType, \
        AccessorTemplate \
        <   TFixedItemHeapWithAux \
            <   typename TFixedItemHeapAVL_StorageBase< ObjectType >::TItem, \
                HeapAllocatorType, \
                typename TFixedItemHeapAVL_StorageBase< ObjectType >::THeader>, \
            typename TFixedItemHeapAVL_StorageBase< ObjectType >::THeader> > \

template <class ObjectType, class HeapAllocatorType, class AccessorType>
class TFixedItemHeapAVL_Storage : public TFixedItemHeapAVL_StorageBase<ObjectType>
{
public:
    // Item
    typedef typename TFixedItemHeapAVL_StorageBase<ObjectType>::TItem TItem;

    // Header
    typedef typename TFixedItemHeapAVL_StorageBase<ObjectType>::THeader THeader;

    // Iterator
    typedef typename TFixedItemHeapAVL_StorageBase<ObjectType>::TIterator TIterator;

    // Const iterator
    typedef typename TFixedItemHeapAVL_StorageBase<ObjectType>::TConstIterator TConstIterator;

    // Heap
    typedef TFixedItemHeapWithAux<TItem, HeapAllocatorType, THeader> THeap;

    // Accessor
    typedef AccessorType TAccessor;

    // Update proc
    typedef void TUpdateProc(TFixedItemHeapAVL_Storage& Storage, TIterator Iter);

private:
    template <class KeyType>
    size_t CreateItem(const KeyType& key)
        { return m_Accessor.GetHeap().Reserve(key); }

    void DestroyItem(size_t x)
        { m_Accessor.GetHeap().Free(x); }

    template <class KeyType, class CreatorType>
    size_t CreateItem(const KeyType& key, const CreatorType& data)
        { return m_Accessor.GetHeap().Reserve(key, data); }

    TItem& GetItem(size_t x)
        { return m_Accessor.GetHeap()[x]; }

    const TItem& GetItem(size_t x) const
        { return (const_cast<TFixedItemHeapAVL_Storage*>(this))->GetItem(x); }

    void Clean(size_t x);

    void Fix(size_t x);

    template <class KeyType>
    void Attach(size_t i, const KeyType& key);

    void Detach(size_t x);

private:
    TUpdateProc* const m_pUpdateProc;

public:
    TAccessor m_Accessor;

public:
    TFixedItemHeapAVL_Storage(TUpdateProc* pUpdateProc = NULL);

    bool IsAllocated() const
        { return m_Accessor.IsAllocated(); }

    void Release()
        { m_Accessor.Release(); }

    size_t GetN() const
        { return m_Accessor.GetHeader().m_szN; }

    size_t GetH() const
        { return IsEmpty() ? 0 : GetH(GetRoot()); }

    bool IsEmpty() const
        { return !GetRoot().IsValid(); }

    void Clear();

    template <class KeyType>
    TIterator Find(const KeyType& key);

    template <class KeyType>
    TConstIterator Find(const KeyType& key) const
        { return (const_cast<TFixedItemHeapAVL_Storage*>(this))->Find(key); }

    template <class KeyType>
    TIterator Get(const KeyType& key)
    {
        TIterator Iter = Find(key);

        DEBUG_VERIFY(Iter.IsValid());

        return Iter;
    }

    template <class KeyType>
    TConstIterator Get(const KeyType& key) const
        { return (const_cast<TFixedItemHeapAVL_Storage*>(this))->Get(key); }

    template <class KeyType>
    TIterator FindFirst(const KeyType& key);

    template <class KeyType>
    TConstIterator FindFirst(const KeyType& key) const
        { return (const_cast<TFixedItemHeapAVL_Storage*>(this))->FindFirst(key); }

    template <class KeyType>
    TIterator FindFirstGreater(const KeyType& key);

    template <class KeyType>
    TConstIterator FindFirstGreater(const KeyType& key) const
        { return (const_cast<TFixedItemHeapAVL_Storage*>(this))->FindFirstGreater(key); }

    template <class KeyType>
    TIterator FindFirstGreaterEq(const KeyType& key);

    template <class KeyType>
    TConstIterator FindFirstGreaterEq(const KeyType& key) const
        { return (const_cast<TFixedItemHeapAVL_Storage*>(this))->FindFirstGreaterEq(key); }

    template <class KeyType>
    TIterator FindLast(const KeyType& key);

    template <class KeyType>
    TConstIterator FindLast(const KeyType& key) const
        { return (const_cast<TFixedItemHeapAVL_Storage*>(this))->FindLast(key); }

    template <class KeyType>
    TIterator FindLastLess(const KeyType& key);

    template <class KeyType>
    TConstIterator FindLastLess(const KeyType& key) const
        { return (const_cast<TFixedItemHeapAVL_Storage*>(this))->FindLastLess(key); }

    template <class KeyType>
    TIterator FindLastLessEq(const KeyType& key);

    template <class KeyType>
    TConstIterator FindLastLessEq(const KeyType& key) const
        { return (const_cast<TFixedItemHeapAVL_Storage*>(this))->FindLastLessEq(key); }

    template <class KeyType>
    bool Has(const KeyType& key) const
        { return Find(key).IsValid(); }

    template <class KeyType>
    TIterator Add(const KeyType& key)
    {
        size_t x = CreateItem(key);

        m_Accessor.GetHeader().m_szN++;

        Attach(x, key);

        return x;
    }

    template <class KeyType, class CreatorType>
    TIterator Add(const KeyType& key, const CreatorType& data)
    {
        size_t x = CreateItem(key, data);

        m_Accessor.GetHeader().m_szN++;

        Attach(x, key);

        return x;
    }

    template <class KeyType>
    void ModifyArbitraryWithHint(TIterator i, const KeyType& Key)
    {
        DEBUG_VERIFY(i.IsValid());

        Detach(i.x);

        Attach(i.x, Key);
    }

    template <class KeyType>
    void ModifyArbitrary(TIterator i)
        { ModifyArbitraryWithHint(i, KeyType(GetDataRef(i))); }

    void Modify(TIterator i)
        { ModifyArbitrary<ObjectType>(i); }

    void Del(TIterator i)
    {
        DEBUG_VERIFY(i.IsValid());

        m_Accessor.GetHeader().m_szN--;

        Detach(i.x);

        DestroyItem(i.x);
    }

    void Upd(TIterator i);

    TIterator GetRoot()
        { return m_Accessor.GetHeader().m_Root; }

    TConstIterator GetRoot() const
        { return (const_cast<TFixedItemHeapAVL_Storage*>(this))->GetRoot(); }

    TIterator GetFirst()
    {
        if(IsEmpty())
            return 0;

        size_t x = m_Accessor.GetHeader().m_Root.x;

        for(;;)
        {
            TItem& xi = GetItem(x);

            if(!xi.l)
                break;

            x = xi.l;
        }

        return x;
    }

    TConstIterator GetFirst() const
        { return (const_cast<TFixedItemHeapAVL_Storage*>(this))->GetFirst(); }

    TIterator GetLast()
    {
        if(IsEmpty())
            return 0;

        size_t x = m_Accessor.GetHeader().m_Root.x;

        for(;;)
        {
            TItem& xi = GetItem(x);

            if(!xi.r)
                break;

            x = xi.r;
        }

        return x;
    }

    TConstIterator GetLast() const
        { return (const_cast<TFixedItemHeapAVL_Storage*>(this))->GetLast(); }

    // Iterator ops
    ObjectType* GetDataPtr(TIterator i)
        { DEBUG_VERIFY(i.IsValid()); return &GetItem(i.x).data; }

    ObjectType& GetDataRef(TIterator i)
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).data; }

    ObjectType& operator [] (TIterator i)
        { return GetDataRef(i); }

    TIterator GetLeft(TIterator i)
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).l; }

    TIterator GetRight(TIterator i)
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).r; }

    TIterator GetParent(TIterator i)
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).p; }

    TIterator GetPrev(TIterator i);
    TIterator GetNext(TIterator i);

    TIterator& ToLeft(TIterator& i)
        { i.x = GetItem(i.x).l; return i; }

    TIterator& ToRight(TIterator& i)
        { i.x = GetItem(i.x).r; return i; }

    TIterator& ToParent(TIterator& i)
        { i.x = GetItem(i.x).p; return i; }

    TIterator& ToPrev(TIterator& i)
        { return i = GetPrev(i); }

    TIterator& ToNext(TIterator& i)
        { return i = GetNext(i); }

    size_t GetH(TIterator i) const
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).h; }

    // Const iterator ops
    const ObjectType* GetDataPtr(TConstIterator i) const
        { DEBUG_VERIFY(i.IsValid()); return &GetItem(i.x).data; }

    const ObjectType& GetDataRef(TConstIterator i) const
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).data; }

    const ObjectType& operator [] (TConstIterator i) const
        { return GetDataRef(i); }

    TConstIterator GetLeft(TConstIterator i) const
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).l; }

    TConstIterator GetRight(TConstIterator i) const
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).r; }

    TConstIterator GetParent(TConstIterator i) const
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).p; }

    TConstIterator GetPrev(TConstIterator i) const;
    TConstIterator GetNext(TConstIterator i) const;

    TConstIterator& ToLeft(TConstIterator& i) const
        { i.x = GetItem(i.x).l; return i; }

    TConstIterator& ToRight(TConstIterator& i) const
        { i.x = GetItem(i.x).r; return i; }

    TConstIterator& ToParent(TConstIterator& i) const
        { i.x = GetItem(i.x).p; return i; }

    TConstIterator& ToPrev(TConstIterator& i) const
        { return i = GetPrev(i); }

    TConstIterator& ToNext(TConstIterator& i) const
        { return i = GetNext(i); }

    size_t GetH(TConstIterator i) const
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).h; }
};

template <class ObjectType, class HeapAllocatorType, class AccessorType>
TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::TFixedItemHeapAVL_Storage(TUpdateProc* pUpdateProc) :
    m_pUpdateProc(pUpdateProc)
{
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
void TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::Clean(size_t x)
{
    if(!x)
        return;

    TItem& xi = GetItem(x);

    Clean(xi.l), Clean(xi.r);

    m_Accessor.GetHeap().Free(x);
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
void TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::Clear()
{
    if(!m_Accessor.FastClear())
    {
        Clean(m_Accessor.GetHeader().m_Root.x);

        new(&m_Accessor.GetHeader()) THeader();
    }
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
void TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::Fix(size_t x)
{
    while(x)
    {
        TItem& xi = GetItem(x);

        const size_t p = xi.p;

        const size_t h1 = xi.l ? GetItem(xi.l).h : 0;
        const size_t h2 = xi.r ? GetItem(xi.r).h : 0;

        if(h1 > h2 + 1 || h1 == h2 + 1 && p && GetItem(p).r == x) // right rotation
        {
            const size_t a = xi.l;

            TItem& ai = GetItem(a);

            const size_t b = ai.r;

            ai.r = x, xi.p = a;

            if(p)
            {
                TItem& pi = GetItem(p);

                if(pi.l == x)
                {
                    pi.l = a;
                }
                else
                {
                    DEBUG_VERIFY(pi.r == x);

                    pi.r = a;
                }
            }
            else
            {
                m_Accessor.GetHeader().m_Root.x = a;
            }

            ai.p = p;

            if(xi.l = b)
                GetItem(b).p = x, x = b;
        }
        else if(h2 > h1 + 1 || h2 == h1 + 1 && p && GetItem(p).l == x) // left rotation
        {
            const size_t a = xi.r;

            TItem& ai = GetItem(a);

            const size_t b = ai.l;

            ai.l = x, xi.p = a;

            if(p)
            {
                TItem& pi = GetItem(p);

                if(pi.l == x)
                {
                    pi.l = a;
                }
                else
                {
                    DEBUG_VERIFY(pi.r == x);

                    pi.r = a;
                }
            }
            else
            {
                m_Accessor.GetHeader().m_Root.x = a;
            }

            ai.p = p;

            if(xi.r = b)
                GetItem(b).p = x, x = b;
        }
        else
        {
            xi.h = Max(h1, h2) + 1;

            if(m_pUpdateProc)
                m_pUpdateProc(*this, x);

            x = p;
        }
    }
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
template <class KeyType>
typename TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::TIterator
    TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::Find(const KeyType& key)
{
    size_t x = m_Accessor.GetHeader().m_Root.x;

    while(x)
    {
        TItem& xi = GetItem(x);

        const int d = Compare(xi.data, key);

        if(d > 0)
            x = xi.l;
        else if(d < 0)
            x = xi.r;
        else
            return x;
    }

    return 0;
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
template <class KeyType>
typename TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::TIterator
    TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::FindFirst(const KeyType& key)
{
    size_t x = m_Accessor.GetHeader().m_Root.x, y = 0;

    while(x)
    {
        TItem& xi = GetItem(x);

        const int d = Compare(xi.data, key);

        if(d > 0)
            x = xi.l;
        else if(d < 0)
            x = xi.r;
        else
            y = x, x = xi.l;
    }

    return y;
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
template <class KeyType>
typename TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::TIterator
    TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::FindFirstGreater(const KeyType& key)
{
    size_t x = m_Accessor.GetHeader().m_Root.x, y = 0;

    while(x)
    {
        TItem& xi = GetItem(x);

        const int d = Compare(xi.data, key);

        if(d <= 0)
            x = xi.r;
        else if(d > 0)
            y = x, x = xi.l;
    }

    return y;
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
template <class KeyType>
typename TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::TIterator
    TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::FindFirstGreaterEq(const KeyType& key)
{
    size_t x = m_Accessor.GetHeader().m_Root.x, y = 0;

    while(x)
    {
        TItem& xi = GetItem(x);

        const int d = Compare(xi.data, key);

        if(d >= 0)
            y = x, x = xi.l;
        else
            x = xi.r;
    }

    return y;
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
template <class KeyType>
typename TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::TIterator
    TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::FindLast(const KeyType& key)
{
    size_t x = m_Accessor.GetHeader().m_Root.x, y = 0;

    while(x)
    {
        TItem& xi = GetItem(x);

        const int d = Compare(xi.data, key);

        if(d > 0)
            x = xi.l;
        else if(d < 0)
            x = xi.r;
        else
            y = x, x = xi.r;
    }

    return y;
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
template <class KeyType>
typename TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::TIterator
    TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::FindLastLess(const KeyType& key)
{
    size_t x = m_Accessor.GetHeader().m_Root.x, y = 0;

    while(x)
    {
        TItem& xi = GetItem(x);

        const int d = Compare(xi.data, key);

        if(d >= 0)
            x = xi.l;
        else
            y = x, x = xi.r;
    }

    return y;
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
template <class KeyType>
typename TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::TIterator
    TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::FindLastLessEq(const KeyType& key)
{
    size_t x = m_Accessor.GetHeader().m_Root.x, y = 0;

    while(x)
    {
        TItem& xi = GetItem(x);

        const int d = Compare(xi.data, key);

        if(d <= 0)
            y = x, x = xi.r;
        else
            x = xi.l;
    }

    return y;
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
template <class KeyType>
void TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::Attach(size_t i, const KeyType& key)
{
    TItem& ii = GetItem(i);

    ii.l = ii.r = 0;

    size_t& rx = m_Accessor.GetHeader().m_Root.x;

    if(!rx)
    {
        ii.p = 0, rx = i;
    }
    else
    {
        size_t x = rx;

        for(;;)
        {
            TItem& xi = GetItem(x);

            const int d = Compare(xi.data, key);

            if(d >= 0)
            {
                if(!xi.l)
                {
                    ii.p = x, xi.l = i;
                    break;
                }

                x = xi.l;
            }
            else
            {
                if(!xi.r)
                {
                    ii.p = x, xi.r = i;
                    break;
                }

                x = xi.r;
            }
        }
    }

    Fix(i);
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
void TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::Detach(size_t x)
{
    TItem& xi = GetItem(x);

    size_t p = xi.p;

    size_t y;

    if(y = xi.l)
    {
        if(p)
        {
            TItem& pi = GetItem(p);

            if(pi.l == x)
            {
                pi.l = y;
            }
            else
            {
                DEBUG_VERIFY(pi.r == x);

                pi.r = y;
            }
        }
        else
        {
            m_Accessor.GetHeader().m_Root.x = y;
        }

        TItem* yi = &GetItem(y);

        yi->p = p;

        while(yi->r)
            yi = &GetItem(y = yi->r);

        if(yi->r = xi.r)
            GetItem(xi.r).p = y;

        Fix(y);
    }
    else if(y = xi.r)
    {
        if(p)
        {
            TItem& pi = GetItem(p);

            if(pi.l == x)
            {
                pi.l = y;
            }
            else
            {
                DEBUG_VERIFY(pi.r == x);

                pi.r = y;
            }
        }
        else
        {
            m_Accessor.GetHeader().m_Root.x = y;
        }

        TItem* yi = &GetItem(y);

        yi->p = p;

        while(yi->l)
            yi = &GetItem(y = yi->l);

        if(yi->l = xi.l)
            GetItem(xi.l).p = y;

        Fix(y);
    }
    else
    {
        if(p)
        {
            TItem& pi = GetItem(p);

            if(pi.l == x)
            {
                pi.l = 0;
            }
            else
            {
                DEBUG_VERIFY(pi.r == x);

                pi.r = 0;
            }

            Fix(p);
        }
        else
        {
            m_Accessor.GetHeader().m_Root.x = 0;
        }
    }
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
void TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::Upd(TIterator i)
{
    DEBUG_VERIFY(i.IsValid());

    if(!m_pUpdateProc)
        return;

    for( ; i.IsValid() ; ToParent(i))
        m_pUpdateProc(*this, i);
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
typename TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::TIterator
    TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::GetPrev(TIterator i)
{
    DEBUG_VERIFY(i.IsValid());

    size_t x = i.x;

    const TItem* xi = &GetItem(x);

    if(xi->l)
    {
        for(xi = &GetItem(x = xi->l) ; xi->r ; xi = &GetItem(x = xi->r));
    }
    else
    {
        size_t y;

        do
        {
            y = x;

            if(!(x = xi->p))
                break;

            xi = &GetItem(x);

        }while(x && xi->r != y);
    }

    return x;
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
typename TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::TIterator
    TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::GetNext(TIterator i)
{
    DEBUG_VERIFY(i.IsValid());

    size_t x = i.x;

    const TItem* xi = &GetItem(x);

    if(xi->r)
    {
        for(xi = &GetItem(x = xi->r) ; xi->l ; xi = &GetItem(x = xi->l));
    }
    else
    {
        size_t y;

        do
        {
            y = x;

            if(!(x = xi->p))
                break;

            xi = &GetItem(x);

        }while(x && xi->l != y);
    }

    return x;
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
typename TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::TConstIterator
    TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::GetPrev(TConstIterator i) const
{
    DEBUG_VERIFY(i.IsValid());

    size_t x = i.x;

    const TItem* xi = &GetItem(x);

    if(xi->l)
    {
        for(xi = &GetItem(x = xi->l) ; xi->r ; xi = &GetItem(x = xi->r));
    }
    else
    {
        size_t y;

        do
        {
            y = x;

            if(!(x = xi->p))
                break;

            xi = &GetItem(x);

        }while(x && xi->r != y);
    }

    return x;
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
typename TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::TConstIterator
    TFixedItemHeapAVL_Storage<ObjectType, HeapAllocatorType, AccessorType>::GetNext(TConstIterator i) const
{
    DEBUG_VERIFY(i.IsValid());

    size_t x = i.x;

    const TItem* xi = &GetItem(x);

    if(xi->r)
    {
        for(xi = &GetItem(x = xi->r) ; xi->l ; xi = &GetItem(x = xi->l));
    }
    else
    {
        size_t y;

        do
        {
            y = x;

            if(!(x = xi->p))
                break;

            xi = &GetItem(x);

        }while(x && xi->l != y);
    }

    return x;
}

#endif // fixed_item_heap_avl_storage_h
