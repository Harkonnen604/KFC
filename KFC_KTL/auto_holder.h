#ifndef auto_holder_h
#define auto_holder_h

#include "globals.h"
#include "ptr_holder.h"
#include "persistent_fixed_item_heap.h"
#include "critical_section.h"

// --------------------
// Auto holder globals
// --------------------
class TAutoHolderGlobals : public TGlobals
{
private:
    void OnUninitialize ();
    void OnInitialize   ();

public:
    struct TPtr
    {
        void*           m_pItem;
        volatile LONG   m_lRefCount;
    };

    typedef TPersistentFixedItemHeap<TPtr> TPtrHeap;

    struct TIter
    {
        void*               m_pIter;
        void*               m_pStorage;
        TCriticalSection*   m_pCS;
        volatile LONG       m_lRefCount;
    };

    typedef TPersistentFixedItemHeap<TIter> TIterHeap;

public:
    TPtrHeap            m_PtrHeap;
    TCriticalSection    m_PtrHeapCS;

    TIterHeap           m_IterHeap;
    TCriticalSection    m_IterHeapCS;

public:
    TAutoHolderGlobals();
};

extern TAutoHolderGlobals g_AutoHolderGlobals;

// ------------
// Auto holder
// ------------
template <class HolderItemType, class HolderItemTraits>
class TAutoHolder
{
public:
    // Holder item
    typedef HolderItemType THolderItem;

private:
    HolderItemType* m_pHolder;

public:
    TAutoHolder()
        { m_pHolder = NULL; }

    TAutoHolder(const typename HolderItemType::TItem& Item)
    {
        m_pHolder = NULL;

        Set(Item);
    }

    template <class StorageType>
    TAutoHolder(StorageType&                    Storage,
                typename StorageType::TIterator Iter,
                TCriticalSection*               pCS)
    {
        m_pHolder = NULL;

        Set(Storage, Iter, pCS);
    }

    TAutoHolder(const TAutoHolder& Holder)
    {
        if(m_pHolder = Holder.m_pHolder)
            m_pHolder->IncRef();
    }

    ~TAutoHolder()
        { Release(); }

    void Release()
    {
        if(m_pHolder)
        {
            if(!m_pHolder->DecRef())
            {
                m_pHolder->~HolderItemType();

                {
                    TCriticalSectionLocker Locker0(HolderItemTraits::GetHeapCS());

                    HolderItemTraits::GetHeap().FreeWithoutDestruction(m_pHolder);
                }
            }

            m_pHolder = NULL;
        }
    }

    TAutoHolder& Set(const typename HolderItemType::TItem& Item)
    {
        DEBUG_VERIFY(!(m_pHolder && m_pHolder->GetItem() == Item));

        Release();

        if(Item)
        {
            TCriticalSectionLocker Locker0(HolderItemTraits::GetHeapCS());

            m_pHolder = HolderItemTraits::GetHeap().Reserve(Item);
        }

        return *this;
    }

    template <class StorageType>
    TAutoHolder& Set(   StorageType&                    Storage,
                        typename StorageType::TIterator Iter,
                        TCriticalSection*               pCS)
    {
        return Set(typename HolderItemType::TItem(Storage, Iter, pCS));
    }

    TAutoHolder& operator = (const typename HolderItemType::TItem& Item)
        { return Set(Item); }

    TAutoHolder& operator = (const TAutoHolder& Holder)
    {
        if(*this == Holder)
            return *this;

        DEBUG_VERIFY
            (!( m_pHolder && Holder.m_pHolder &&
                m_pHolder->GetItem() == Holder.m_pHolder->GetItem()));

        Release();

        if(m_pHolder = Holder.m_pHolder)
            m_pHolder->IncRef();

        return *this;
    }

    typename HolderItemType::TObject* operator -> () const
        { DEBUG_VERIFY(m_pHolder); return &*m_pHolder->GetItem(); }

    typename HolderItemType::TObject& operator * () const
        { DEBUG_VERIFY(m_pHolder); return *m_pHolder->GetItem(); }

    operator bool () const
        { return m_pHolder; }

    template <class HolderItemType2, class HolderItemTraits2>
    friend inline int Compare(  const TAutoHolder<HolderItemType2, HolderItemTraits2>&                              Holder,
                                const typename TAutoHolder<HolderItemType2, HolderItemTraits2>::THolderItem::TItem& Item);

    template <class HolderItemType2, class HolderItemTraits2>
    friend inline int Compare(  const TAutoHolder<HolderItemType2, HolderItemTraits2>& Holder1,
                                const TAutoHolder<HolderItemType2, HolderItemTraits2>& Holder2);
};

template <class HolderItemType, class HolderItemTraits>
inline int Compare( const TAutoHolder<HolderItemType, HolderItemTraits>&                                Holder,
                    const typename TAutoHolder<HolderItemType, HolderItemTraits>::THolderItem::TItem&   Item)
{
    int d;

    if(d = Compare((bool)Holder, (bool)Item))
        return d;

    if(d = Compare(Holder.m_pHolder->GetItem(), Item))
        return d;

    return 0;
}

#define DECL_ARGS   class HolderItemType, class HolderItemTraits
#define USE_ARGS    HolderItemType, HolderItemTraits

DECLARE_TEMPLATE_COMPARISON_OPERATORS(  DECL_ARGS,                      \
                                        const TAutoHolder<USE_ARGS>&,   \
                                        const typename TAutoHolder<USE_ARGS>::THolderItem::TItem&)

#undef USE_ARGS
#undef DECL_ARGS

template <class HolderItemType, class HolderItemTraits>
inline int Compare( const TAutoHolder<HolderItemType, HolderItemTraits>& Holder1,
                    const TAutoHolder<HolderItemType, HolderItemTraits>& Holder2)
{
    return Compare(Holder1.m_pHolder, Holder2.m_pHolder);
}

#define DECL_ARGS   class HolderItemType, class HolderItemTraits
#define USE_ARGS    HolderItemType, HolderItemTraits

DECLARE_TEMPLATE_COMPARISON_OPERATORS(  DECL_ARGS,                      \
                                        const TAutoHolder<USE_ARGS>&,   \
                                        const TAutoHolder<USE_ARGS>&)

#undef USE_ARGS
#undef DECL_ARGS

// ----------------
// Ptr holder item
// ----------------
template <class ObjectType, template <class> class ptr_deleter = cpp_ptr_deleter>
class TPtrHolderItem
{
public:
    // Object
    typedef ObjectType TObject;

    // Item
    typedef ObjectType* TItem;

private:
    const TItem m_Item;

    volatile LONG m_lRefCount;

public:
    TPtrHolderItem(TItem Item) :
        m_Item(Item), m_lRefCount(1)
    {
        DEBUG_VERIFY(m_Item);
    }

    ~TPtrHolderItem()
        { ptr_deleter<ObjectType>::Delete(m_Item); }

    const TItem& GetItem() const
        { return m_Item; }

    void IncRef()
        { InterlockedIncrement(&m_lRefCount); }

    bool DecRef()
        { return InterlockedDecrement(&m_lRefCount) > 0; }
};

template <class t>
class TPtrHolderItemTraits
{
public:
    typedef TPersistentFixedItemHeap<TPtrHolderItem<t> > THeap;

public:
    static THeap& GetHeap()
    {
        kfc_static_assert(TAutoHolderGlobals::TPtrHeap::fittable_t<TPtrHolderItem<t> >::res);

        return (THeap&)g_AutoHolderGlobals.m_PtrHeap;
    }

    static TCriticalSection& GetHeapCS()
        { return g_AutoHolderGlobals.m_PtrHeapCS; }
};

// ----------------
// Ptr auto holder
// ----------------
#define PTR_AH(type) \
    TAutoHolder<TPtrHolderItem<type>, TPtrHolderItemTraits<type> >

#define CPTR_AH(type, deleter) \
    TAutoHolder<TPtrHolderItem<type, deleter>, TPtrHolderItemTraits<type> >

// -----------------
// Iter holder item
// -----------------
template <class StorageType>
class TIterHolderItem
{
public:
    // Object
    typedef typename StorageType::TObject TObject;

    // Item
    struct TItem
    {
    public:
        typename StorageType::TIterator m_Iter;
        StorageType*                    m_pStorage;
        TCriticalSection*               m_pCS;

    public:
        TItem() : m_pStorage(NULL) {}

        TItem(  StorageType&                    Storage,
                typename StorageType::TIterator Iter,
                TCriticalSection*               pCS) :

            m_Iter(Iter), m_pStorage(&Storage), m_pCS(pCS)
        {
            DEBUG_VERIFY(Iter.IsValid());
        }

        operator bool () const
            { return m_pStorage; }

        typename StorageType::TObject* operator -> () const
            { DEBUG_VERIFY(*this); return &*m_Iter; }

        typename StorageType::TObject& operator * () const
            { DEBUG_VERIFY(*this); return *m_Iter; }
    };

private:
    TItem m_Item;

    volatile LONG m_lRefCount;

public:
    TIterHolderItem(const TItem& Item) :
        m_Item(Item), m_lRefCount(1)
    {
        DEBUG_VERIFY(m_Item);
    }

    ~TIterHolderItem()
    {
        if(m_Item)
        {
            TSafeCriticalSectionLocker Locker0(m_Item.m_pCS);

            m_Item.m_pStorage->Del(m_Item.m_Iter);
        }
    }

    const TItem& GetItem() const
        { return m_Item; }

    void IncRef()
        { InterlockedIncrement(&m_lRefCount); }

    bool DecRef()
        { return InterlockedDecrement(&m_lRefCount) > 0; }
};

template <class StorageType>
inline int Compare( const struct TIterHolderItem<StorageType>::TItem& Item1,
                    const struct TIterHolderItem<StorageType>::TItem& Item2)
{
    int d;

    if(d = Compare(Item1.m_pStorage, Item2.m_pStorage))
        return d;

    if(d = Compare(Item1.m_Iter, Item2.m_Iter))
        return d;

    return 0;
}

DECLARE_TEMPLATE_COMPARISON_OPERATORS(  class StorageType,                                      \
                                        const struct TIterHolderItem<StorageType>::TItem&,  \
                                        const struct TIterHolderItem<StorageType>::TItem&)

template <class t>
class TIterHolderItemTraits
{
public:
    typedef TPersistentFixedItemHeap<TIterHolderItem<t> > THeap;

public:
    static THeap& GetHeap()
    {
        kfc_static_assert(TAutoHolderGlobals::TIterHeap::fittable_t<TIterHolderItem<t> >::res);

        return (THeap&)g_AutoHolderGlobals.m_IterHeap;
    }

    static TCriticalSection& GetHeapCS()
        { return g_AutoHolderGlobals.m_IterHeapCS; }
};

// -----------------
// Iter auto holder
// -----------------
#define ITER_AH(type) TAutoHolder<TIterHolderItem<type>, TIterHolderItemTraits<type> >

#endif // auto_holder_h
