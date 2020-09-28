#ifndef fixed_item_heap_accessors_h
#define fixed_item_heap_accessors_h

#include "fixed_item_heap.h"

// ---------------------------------
// Private fixed item heap accessor
// ---------------------------------
template <class HeapType, class HeaderType>
class TPrivateFixedItemHeapAccessor
{
public:
    typedef HeapType    THeap;
    typedef HeaderType  THeader;

private:
    THeap m_Heap;

public:
    bool IsAllocated() const
        { return m_Heap.IsAllocated(); }

    void Release()
        { m_Heap.Release(); }

    void AllocateHeap()
        { m_Heap.AllocateWithAux(); }

    THeap& GetHeap()
        { return m_Heap; }

    const THeap& GetHeap() const
        { return m_Heap; }

    THeader& GetHeader()
        { return m_Heap.GetAuxData(); }

    const THeader& GetHeader() const
        { return m_Heap.GetAuxData(); }

    bool FastClear()
    {
        m_Heap.ClearWithAux();

        return true;
    }

    typename THeap::TAllocator& GetHeapAllocator()
        { return m_Heap.m_Allocator; }

    const typename THeap::TAllocator& GetHeapAllocator() const
        { return m_Heap.m_Allocator; }
};

// ------------------------------------------
// Private fixed item heap accessor with aux
// ------------------------------------------
#define DECLARE_SPECIFIC_PRIVATE_FIXED_ITME_HEAP_ACCESSOR_WITH_AUX(AuxDataType, Name) \
    template <class HeapType, class HeaderType> \
    class Name \
    { \
    public: \
        typedef HeapType    THeap; \
        typedef HeaderType  THeader; \
\
    private: \
        THeap m_Heap; \
\
    public: \
        AuxDataType m_AuxData; \
\
    public: \
        bool IsAllocated() const \
            { return m_Heap.IsAllocated(); } \
\
        void Release() \
            { m_Heap.Release(); } \
\
        void AllocateHeap() \
            { m_Heap.AllocateWithAux(); } \
\
        THeap& GetHeap() \
            { return m_Heap; } \
\
        const THeap& GetHeap() const \
            { return m_Heap; } \
\
        THeader& GetHeader() \
            { return m_Heap.GetAuxData(); } \
\
        const THeader& GetHeader() const \
            { return m_Heap.GetAuxData(); } \
\
        bool FastClear() \
        { \
            new(&m_AuxData) AuxDataType(); \
\
            m_Heap.ClearWithAux(); \
\
            return true; \
        } \
\
        typename THeap::TAllocator& GetHeapAllocator() \
            { return m_Heap.m_Allocator; } \
\
        const typename THeap::TAllocator& GetHeapAllocator() const \
            { return m_Heap.m_Allocator; } \
    }; \

template <class AuxDataType>
class TPrivateFixedItemHeapAccessorWithAux
{
public:
    DECLARE_SPECIFIC_PRIVATE_FIXED_ITME_HEAP_ACCESSOR_WITH_AUX(AuxDataType, _)
};

// --------------------------------
// Shared fixed item heap accessor
// --------------------------------
#define DECLARE_SPECIFIC_SHARED_FIXED_ITEM_HEAP_ACCESSOR(HeaderPtrTemplate, Name) \
    template <class HeapType, class HeaderType> \
    class Name \
    { \
    public: \
        typedef HeapType                    THeap; \
        typedef HeaderType                  THeader; \
        typedef HeaderPtrTemplate<THeader>  THeaderPtr; \
\
    private: \
        THeap* m_pHeap; \
\
        THeaderPtr m_pHeader; \
\
    public: \
        Name() : m_pHeap(NULL) {} \
\
        bool IsAllocated() const \
            { return m_pHeap; } \
\
        void Release() \
            { m_pHeap = NULL; } \
\
        void Allocate(THeap& Heap, THeaderPtr pHeader) \
        { \
            Release(); \
\
            DEBUG_VERIFY(Heap.IsAllocated()); \
\
            m_pHeap = &Heap; \
\
            m_pHeader = pHeader; \
        }; \
\
        THeap& GetHeap() \
            { DEBUG_VERIFY_ALLOCATION; return *m_pHeap; } \
\
        const THeap& GetHeap() const \
            { DEBUG_VERIFY_ALLOCATION; return *m_pHeap; } \
\
        THeader& GetHeader() \
            { DEBUG_VERIFY_ALLOCATION; return *m_pHeader; } \
\
        const THeader& GetHeader() const \
            { DEBUG_VERIFY_ALLOCATION; return *m_pHeader; } \
\
        bool FastClear() \
            { return false; } \
    }; \

template <template <class> class HeaderPtrTemplate>
class TSharedFixedItemHeapAccessor
{
public:
    DECLARE_SPECIFIC_SHARED_FIXED_ITEM_HEAP_ACCESSOR(HeaderPtrTemplate, _)
};

#endif // fixed_item_heap_accessors_h
