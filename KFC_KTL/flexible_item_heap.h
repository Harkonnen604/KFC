#ifndef flexible_item_heap_h
#define flexible_item_heap_h

#include "fixed_item_heap_avl_storage.h"
#include "growable_allocator.h"

// -------------------
// Flexible item heap
// -------------------

// Global header
struct TFlexibleItemHeapGlobalHeader
{
public:
    size_t m_szFirst;
    size_t m_szLast;

    size_t m_szFirstFree;
    size_t m_szLastFree;

public:
    TFlexibleItemHeapGlobalHeader() :
        m_szFirst(0), m_szLast(0),
        m_szFirstFree(0), m_szLastFree(0) {}
};

// Header
struct TFlexibleItemHeapHeader
{
public:
    size_t m_szDataOffset;

private:
    size_t m_szSizeAndBit;

public:
    size_t m_szMaxFreeSize;

    size_t m_szPrev;
    size_t m_szNext;

public:
    static size_t GetFreeSize(size_t szSizeAndBit)
        { return (szSizeAndBit & ~INT_MAX) ? 0 : szSizeAndBit; }

    size_t GetFreeSize() const
        { return GetFreeSize(m_szSizeAndBit); }

    size_t GetSize() const
        { return m_szSizeAndBit & INT_MAX; }

    void SetSize(size_t szSize)
        { (m_szSizeAndBit &= ~INT_MAX) |= szSize; }

    void IncSize(size_t szSize)
        { m_szSizeAndBit += szSize; }

    bool GetBit() const
        { return m_szSizeAndBit & ~INT_MAX; }

    void SetBit()
        { m_szSizeAndBit |= ~INT_MAX; }

    void ClearBit()
        { m_szSizeAndBit &= INT_MAX; }
};

inline int Compare(const TFlexibleItemHeapHeader& Header1, const TFlexibleItemHeapHeader& Header2)
{
    return Compare(Header1.GetFreeSize(), Header2.GetFreeSize());
}

DECLARE_COMPARISON_OPERATORS(const TFlexibleItemHeapHeader&, const TFlexibleItemHeapHeader)

// Flexible item heap
template <class HeadersAllocatorType, class DataAllocatorType>
class TFlexibleItemHeap
{
public:
    // Headers storage base
/*  typedef TFixedItemHeapAVL_Storage
        <   TFlexibleItemHeapHeader,
            HeadersAllocatorType,
            TPrivateFixedItemHeapAccessorWithAux<TFlexibleItemHeapGlobalHeader>::_ >*/

    typedef FIXED_ITEM_HEAP_AVL_STORAGE(TFlexibleItemHeapHeader,
                                        HeadersAllocatorType,
                                        TPrivateFixedItemHeapAccessorWithAux<TFlexibleItemHeapGlobalHeader>::_) THeadersStorageBase;

    // Headers storage
    class THeadersStorage : public THeadersStorageBase
    {
    public:
        // Iterator
        typedef typename THeadersStorageBase::TIterator TIterator;

        // Const iterator
        typedef typename THeadersStorageBase::TConstIterator TConstIterator;

    private:
        static void UpdateProc(THeadersStorageBase& Storage, TIterator Iter)
        {
            TIterator Left  = Storage.GetLeft (Iter);
            TIterator Right = Storage.GetRight(Iter);

            Storage[Iter].m_szMaxFreeSize =
                Max(Storage[Iter].GetFreeSize(),
                    Left. IsValid() ? Storage[Left ].m_szMaxFreeSize : 0,
                    Right.IsValid() ? Storage[Right].m_szMaxFreeSize : 0);
        }

    public:
        THeadersStorage() : THeadersStorageBase(UpdateProc) {}
    };

    // Iterator
    typedef typename THeadersStorage::TIterator TIterator;

    // Const itearator
    typedef typename THeadersStorage::TConstIterator TConstIterator;

private:
    bool m_bAllocated;

    THeadersStorage m_HeadersStorage;

    DataAllocatorType m_DataAllocator;

private:
    TFlexibleItemHeapGlobalHeader& GetGlobalHeader()
        { return m_HeadersStorage.m_Accessor.m_AuxData; }

    const TFlexibleItemHeapGlobalHeader& GetGlobalHeader() const
        { return m_HeadersStorage.m_Accessor.m_AuxData; }

    void Attach(TIterator Iter)
    {
        DEBUG_VERIFY(Iter.IsValid());

        TFlexibleItemHeapHeader& Header = m_HeadersStorage[Iter];

        if(Header.m_szPrev)
            m_HeadersStorage[Header.m_szPrev].m_szNext = Iter.x;
        else
            GetGlobalHeader().m_szFirst = Iter.x;

        if(Header.m_szNext)
            m_HeadersStorage[Header.m_szNext].m_szPrev = Iter.x;
        else
            GetGlobalHeader().m_szLast = Iter.x;
    }

    void Detach(TIterator Iter)
    {
        DEBUG_VERIFY(Iter.IsValid());

        TFlexibleItemHeapHeader& Header = m_HeadersStorage[Iter];

        if(Header.m_szPrev)
            m_HeadersStorage[Header.m_szPrev].m_szNext = Header.m_szNext;
        else
            GetGlobalHeader().m_szFirst = Header.m_szNext;

        if(Header.m_szNext)
            m_HeadersStorage[Header.m_szNext].m_szPrev = Header.m_szPrev;
        else
            GetGlobalHeader().m_szLast = Header.m_szPrev;
    }

private:
    TFlexibleItemHeap(const TFlexibleItemHeap&);

    TFlexibleItemHeap& operator = (const TFlexibleItemHeap&);

public:
    TFlexibleItemHeap();

    ~TFlexibleItemHeap()
        { Release(); }

    bool IsAllocated() const
    {
        return  m_bAllocated &&
                    m_HeadersStorage.IsAllocated() &&
                    m_DataAllocator. IsAllocated();
    }

    void Release();

    void Allocate();

    HeadersAllocatorType& GetHeadersAllocator()
        { return m_HeadersStorage.m_Accessor.GetHeapAllocator(); }

    DataAllocatorType& GetDataAllocator()
        { return m_DataAllocator; }

    void Clear();

    TIterator Reserve(size_t szSize);

    void Free(TIterator Iter);

    void* GetDataPtr(TIterator Iter)
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(Iter.IsValid());

        return m_DataAllocator.GetDataPtr() + m_HeadersStorage[Iter].m_szDataOffset;
    }

    const void* GetDataPtr(TConstIterator Iter) const
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(Iter.IsValid());

        return m_DataAllocator.GetDataPtr() + m_HeadersStorage[Iter].m_szDataOffset;
    }

    void* operator [] (TIterator Iter)
        { return GetDataPtr(Iter); }

    const void* operator [] (TConstIterator Iter) const
        { return GetDataPtr(Iter); }
};

template <class HeadersAllocatorType, class DataAllocatorType>
TFlexibleItemHeap<HeadersAllocatorType, DataAllocatorType>::TFlexibleItemHeap()
{
    m_bAllocated = false;
}

template <class HeadersAllocatorType, class DataAllocatorType>
void TFlexibleItemHeap<HeadersAllocatorType, DataAllocatorType>::Release()
{
    m_bAllocated = false;

    m_DataAllocator.Release();

    m_HeadersStorage.Release();
}

template <class HeadersAllocatorType, class DataAllocatorType>
void TFlexibleItemHeap<HeadersAllocatorType, DataAllocatorType>::Allocate()
{
    DEBUG_VERIFY(m_DataAllocator.IsAllocated());

    m_HeadersStorage.m_Accessor.AllocateHeap();

    m_bAllocated = true;
}

template <class HeadersAllocatorType, class DataAllocatorType>
void TFlexibleItemHeap<HeadersAllocatorType, DataAllocatorType>::Clear()
{
    DEBUG_VERIFY_ALLOCATION;

    m_HeadersStorage.Clear();

    m_DataAllocator.SetN(0);
}

template <class HeadersAllocatorType, class DataAllocatorType>
typename TFlexibleItemHeap<HeadersAllocatorType, DataAllocatorType>::TIterator
    TFlexibleItemHeap<HeadersAllocatorType, DataAllocatorType>::Reserve(size_t szSize)
{
    DEBUG_VERIFY_ALLOCATION;

    if(!szSize)
        szSize = 1;

    TIterator Iter;

    // Searching
    {
        typename THeadersStorage::TIterator CIter = m_HeadersStorage.GetRoot();

        if(CIter.IsValid() && m_HeadersStorage[CIter].m_szMaxFreeSize >= szSize)
        {
            for(;;)
            {
                DEBUG_VERIFY(CIter.IsValid());

                typename THeadersStorage::TIterator Left = m_HeadersStorage.GetLeft(CIter);

                if(Left.IsValid() && m_HeadersStorage[Left].m_szMaxFreeSize >= szSize)
                {
                    CIter = Left;
                    continue;
                }

                if(m_HeadersStorage[CIter].GetFreeSize() >= szSize)
                {
                    Iter = CIter;
                    break;
                }

                m_HeadersStorage.ToRight(CIter);
            }
        }
    }

    TFlexibleItemHeapHeader NewHeader;

    size_t szFreeSize;

    if(Iter.IsValid()) // deleting free block
    {
        NewHeader = m_HeadersStorage[Iter];

        szFreeSize = NewHeader.GetFreeSize();

        Detach(Iter);

        m_HeadersStorage.Del(Iter);
    }
    else // preparing new busy block
    {
        NewHeader.m_szDataOffset    = m_DataAllocator.Add(szSize);
        NewHeader.m_szPrev          = GetGlobalHeader().m_szLast;
        NewHeader.m_szNext          = 0;

        szFreeSize = 0;
    }

    NewHeader.SetSize(szSize), NewHeader.SetBit();

    Iter = m_HeadersStorage.Add(NewHeader);

    Attach(Iter);

    // Creating free block afterwards
    if(szSize < szFreeSize)
    {
        TFlexibleItemHeapHeader& Header = m_HeadersStorage[Iter];

        NewHeader.SetSize(szFreeSize - szSize), NewHeader.ClearBit();

        NewHeader.m_szDataOffset    = Header.m_szDataOffset + szSize;
        NewHeader.m_szPrev          = Iter.x;
        NewHeader.m_szNext          = Header.m_szNext;

        Attach(m_HeadersStorage.Add(NewHeader));
    }

    return Iter;
}

template <class HeadersAllocatorType, class DataAllocatorType>
void TFlexibleItemHeap<HeadersAllocatorType, DataAllocatorType>::Free(TIterator Iter)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(Iter.IsValid());

    TFlexibleItemHeapHeader NewHeader = m_HeadersStorage[Iter];

    Detach(Iter);

    m_HeadersStorage.Del(Iter);

    NewHeader.ClearBit();

    size_t szPrev, szNext;

    if(szPrev = NewHeader.m_szPrev)
    {
        TFlexibleItemHeapHeader& Prev = m_HeadersStorage[szPrev];

        if(!Prev.GetBit())
        {
            NewHeader.IncSize(Prev.GetSize());

            NewHeader.m_szDataOffset = Prev.m_szDataOffset;

            NewHeader.m_szPrev = Prev.m_szPrev;

            Detach(TIterator(szPrev));

            m_HeadersStorage.Del(TIterator(szPrev));
        }
    }

    if(szNext = NewHeader.m_szNext)
    {
        TFlexibleItemHeapHeader& Next = m_HeadersStorage[szNext];

        if(!Next.GetBit())
        {
            NewHeader.IncSize(Next.GetSize());

            NewHeader.m_szNext = Next.m_szNext;

            Detach(TIterator(szNext));

            m_HeadersStorage.Del(TIterator(szNext));
        }
    }

    if(NewHeader.m_szNext) // not last
    {
        Attach(m_HeadersStorage.Add(NewHeader));
    }
    else // last
    {
        m_DataAllocator.SetN(NewHeader.m_szDataOffset);

        size_t szLast;

        while((szLast = GetGlobalHeader().m_szLast) != NewHeader.m_szPrev)
        {
            Detach(szLast);

            m_HeadersStorage.Del(TIterator(szLast));
        }
    }
}

#endif // flexible_item_heap_h
