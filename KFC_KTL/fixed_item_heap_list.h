#ifndef fixed_item_heap_list_h
#define fixed_item_heap_list_h

#include "fixed_item_heap.h"
#include "fixed_item_heap_accessors.h"

// --------------------------
// Fixed item heap list base
// --------------------------

// Iterator
template <class ObjectType>
struct TFixedItemHeapListIterator
{
public:
    size_t x;

public:
    TFixedItemHeapListIterator()
        { Invalidate(); }

    TFixedItemHeapListIterator(size_t sx) : x(sx) {}

    bool IsValid() const
        { return x; }

    void Invalidate()
        { x = 0; }
};

// Const iterator
template <class ObjectType>
struct TFixedItemHeapListConstIterator
{
public:
    size_t x;

public:
    TFixedItemHeapListConstIterator()
        { Invalidate(); }

    TFixedItemHeapListConstIterator(size_t sx) : x(sx) {}

    TFixedItemHeapListConstIterator(TFixedItemHeapListIterator<ObjectType> i) : x(i.x) {}

    bool IsValid() const
        { return x; }

    void Invalidate()
        { x = 0; }
};

template <class ObjectType>
inline int Compare( TFixedItemHeapListIterator<ObjectType> Iter1,
                    TFixedItemHeapListIterator<ObjectType> Iter2)
{
    return Compare(Iter1.x, Iter2.x);
}

template <class ObjectType>
inline int Compare( TFixedItemHeapListIterator      <ObjectType> Iter1,
                    TFixedItemHeapListConstIterator <ObjectType> Iter2)
{
    return Compare(Iter1.x, Iter2.x);
}

template <class ObjectType>
inline int Compare( TFixedItemHeapListConstIterator <ObjectType> Iter1,
                    TFixedItemHeapListIterator      <ObjectType> Iter2)
{
    return Compare(Iter1.x, Iter2.x);
}

template <class ObjectType>
inline int Compare( TFixedItemHeapListConstIterator<ObjectType> Iter1,
                    TFixedItemHeapListConstIterator<ObjectType> Iter2)
{
    return Compare(Iter1.x, Iter2.x);
}

DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TFixedItemHeapListIterator      <ObjectType>, TFixedItemHeapListIterator        <ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TFixedItemHeapListIterator      <ObjectType>, TFixedItemHeapListConstIterator   <ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TFixedItemHeapListConstIterator <ObjectType>, TFixedItemHeapListIterator        <ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TFixedItemHeapListConstIterator <ObjectType>, TFixedItemHeapListConstIterator   <ObjectType>)

// Header
template <class ObjectType>
struct TFixedItemHeapListHeader
{
public:
    TFixedItemHeapListIterator<ObjectType> m_First, m_Last;

    size_t m_szN;

public:
    TFixedItemHeapListHeader()
        { m_First.Invalidate(), m_Last.Invalidate(), m_szN = 0; }

    // (for testing purposes)
    bool IsClean() const
        { return !m_First.IsValid() && !m_Last.IsValid() && !m_szN; }

    // (for testing purposes)
    bool operator == (const TFixedItemHeapListHeader& Header) const
        { return m_First == Header.m_First && m_Last == Header.m_Last && m_szN == Header.m_szN; }
};

// (base required to avoid template tempalte argument with iterator comparisons)

template <class ObjectType>
class TFixedItemHeapListBase
{
public:
    // Iterator
    typedef TFixedItemHeapListIterator<ObjectType> TIterator;

    // Const iterator
    typedef TFixedItemHeapListConstIterator<ObjectType> TConstIterator;

    // Header
    typedef TFixedItemHeapListHeader<ObjectType> THeader;

    // Item
    struct TItem
    {
    public:
        ObjectType data;

        size_t pr, nx;

    public:
        TItem() : pr(0), nx(0) {}
    };
};

// ---------------------
// Fixed item heap list
// ---------------------
#define FIXED_ITEM_HEAP_LIST(ObjectType, HeapAllocatorType, AccessorTemplate) \
    TFixedItemHeapList \
    <   ObjectType, \
        HeapAllocatorType, \
        AccessorTemplate \
        <   TFixedItemHeapWithAux \
            <   TFixedItemHeapListBase< ObjectType >::TItem, \
                HeapAllocatorType, \
                TFixedItemHeapListBase< ObjectType >::THeader>, \
            TFixedItemHeapListBase< ObjectType >::THeader> > \

#define FIXED_ITEM_HEAP_LIST_WITH_TYPENAME(ObjectType, HeapAllocatorType, AccessorTemplate) \
    TFixedItemHeapList \
    <   ObjectType, \
        HeapAllocatorType, \
        AccessorTemplate \
        <   TFixedItemHeapWithAux \
            <   typename TFixedItemHeapListBase< ObjectType >::TItem, \
                HeapAllocatorType, \
                typename TFixedItemHeapListBase< ObjectType >::THeader>, \
            typename TFixedItemHeapListBase< ObjectType >::THeader> > \

template <class ObjectType, class HeapAllocatorType, class AccessorType>
class TFixedItemHeapList : public TFixedItemHeapListBase<ObjectType>
{
public:
    // Item
    typedef typename TFixedItemHeapListBase<ObjectType>::TItem TItem;

    // Header
    typedef typename TFixedItemHeapListBase<ObjectType>::THeader THeader;

    // Iterator
    typedef typename TFixedItemHeapListBase<ObjectType>::TIterator TIterator;

    // Const iterator
    typedef typename TFixedItemHeapListBase<ObjectType>::TConstIterator TConstIterator;

    // Heap
    typedef TFixedItemHeapWithAux<TItem, HeapAllocatorType, THeader> THeap;

    // Accessor
    typedef AccessorType TAccessor;

private:
    size_t CreateItem()
        { return m_Accessor.GetHeap().Reserve(); }

    void DestroyItem(size_t x)
        { m_Accessor.GetHeap().Free(x); }

    TItem& GetItem(size_t x)
        { return m_Accessor.GetHeap()[x]; }

    const TItem& GetItem(size_t x) const
        { return (const_cast<TFixedItemHeapList*>(this))->GetItem(x); }

    void Attach(size_t x, size_t pr, size_t nx);

    void Detach(size_t x);

public:
    TAccessor m_Accessor;

public:
    bool IsAllocated() const
        { return m_Accessor.IsAllocated(); }

    void Release()
        { m_Accessor.Release(); }

    size_t GetN() const
        { return m_Accessor.GetHeader().m_szN; }

    bool IsEmpty() const
        { return !GetN(); }

    void Clear();

    TIterator Add(TIterator Prev, TIterator Next)
    {
        DEBUG_VERIFY((Prev.IsValid() ? GetNext(Prev) : GetFirst()) == Next);
        DEBUG_VERIFY((Next.IsValid() ? GetPrev(Next) : GetLast ()) == Prev);

        size_t x = CreateItem();

        m_Accessor.GetHeader().m_szN++;

        Attach(x, Prev.x, Next.x);

        return x;
    }

    TIterator AddBefore(TIterator Iter)
        { return Add(Iter.IsValid() ? GetPrev(Iter) : 0, Iter); }

    TIterator AddAfter(TIterator Iter)
        { return Add(Iter, GetNext(Iter)); }

    TIterator AddFirst()
        { return Add(0, GetFirst()); }

    TIterator AddLast()
        { return Add(GetLast(), 0); }

    void Del(TIterator i)
    {
        DEBUG_VERIFY(i.IsValid());

        m_Accessor.GetHeader().m_szN--;

        Detach(i.x);

        DestroyItem(i.x);
    }

    void DelFirst()
    {
        DEBUG_VERIFY(!IsEmpty());

        Del(GetFirst());
    }

    void DelLast()
    {
        DEBUG_VERIFY(!IsEmpty());

        Del(GetLast());
    }

    // Iterator ops
    ObjectType* GetDataPtr(TIterator i)
        { DEBUG_VERIFY(i.IsValid()); return &GetItem(i.x).data; }

    ObjectType& GetDataRef(TIterator i)
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).data; }

    ObjectType& operator [] (TIterator i)
        { return GetDataRef(i); }

    TIterator GetFirst()
        { return m_Accessor.GetHeader().m_First; }

    TIterator GetLast()
        { return m_Accessor.GetHeader().m_Last; }

    TIterator GetPrev(TIterator i)
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).pr; }

    TIterator GetNext(TIterator i)
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).nx; }

    TIterator& ToFirst(TIterator& i)
        { return i = GetFirst(); }

    TIterator& ToLast(TIterator& i)
        { return i = GetLast(); }

    TIterator& ToPrev(TIterator& i)
        { return i = GetPrev(i); }

    TIterator& ToNext(TIterator& i)
        { return i = GetNext(i); }

    // Const iterator ops
    const ObjectType* GetDataPtr(TConstIterator i) const
        { DEBUG_VERIFY(i.IsValid()); return &GetItem(i.x).data; }

    const ObjectType& GetDataRef(TConstIterator i) const
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).data; }

    const ObjectType& operator [] (TConstIterator i) const
        { return GetDataRef(i); }

    TConstIterator GetFirst() const
        { return m_Accessor.GetHeader().m_First; }

    TConstIterator GetLast() const
        { return m_Accessor.GetHeader().m_Last; }

    TConstIterator GetPrev(TConstIterator i) const
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).pr; }

    TConstIterator GetNext(TConstIterator i) const
        { DEBUG_VERIFY(i.IsValid()); return GetItem(i.x).nx; }

    TConstIterator& ToFirst(TConstIterator& i) const
        { return i = GetFirst(); }

    TConstIterator& ToLast(TConstIterator& i) const
        { return i = GetLast(); }

    TConstIterator& ToPrev(TConstIterator& i) const
        { return i = GetPrev(i); }

    TConstIterator& ToNext(TConstIterator& i) const
        { return i = GetNext(i); }
};

template <class ObjectType, class HeapAllocatorType, class AccessorType>
void TFixedItemHeapList<ObjectType, HeapAllocatorType, AccessorType>::Clear()
{
    if(!m_Accessor.FastClear())
    {
        while(!IsEmpty())
            DelLast();
    }
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
void TFixedItemHeapList<ObjectType, HeapAllocatorType, AccessorType>::Attach(size_t x, size_t pr, size_t nx)
{
    TItem& xi = GetItem(x);

    xi.pr = pr, xi.nx = nx;

    (pr ? GetItem(pr).nx : m_Accessor.GetHeader().m_First.x) = x;
    (nx ? GetItem(nx).pr : m_Accessor.GetHeader().m_Last. x) = x;
}

template <class ObjectType, class HeapAllocatorType, class AccessorType>
void TFixedItemHeapList<ObjectType, HeapAllocatorType, AccessorType>::Detach(size_t x)
{
    TItem& xi = GetItem(x);

    (xi.pr ? GetItem(xi.pr).nx : m_Accessor.GetHeader().m_First.x) = xi.nx;
    (xi.nx ? GetItem(xi.nx).pr : m_Accessor.GetHeader().m_Last. x) = xi.pr;
}

#endif // fixed_item_heap_list_h
