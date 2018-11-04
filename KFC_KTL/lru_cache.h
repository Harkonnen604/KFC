#ifndef lru_cache_h
#define lru_cache_h

#include "avl_storage.h"
#include "critical_section.h"

// ----------
// LRU cache
// ----------
template<class ObjectType>
class T_LRU_Cache
{
public:
	// Item
	struct TItem
	{
		ObjectType m_Data;

		void* m_OrderIter;

		size_t m_szTimesLocked;


		template <class KeyType>
		TItem(const KeyType& Key) :
			m_Data(Key), m_OrderIter(NULL), m_szTimesLocked(0) {}

		bool IsLocked() const
			{ return m_szTimesLocked > 0; }
	};

	// Storage
	typedef T_AVL_Storage<TItem> TStorage;

	// Order
	typedef TList<typename TStorage::TIterator> TOrder;

	// Accessor	
	class TAccessor
	{
	private:
		T_LRU_Cache* m_pCache;

		typename TStorage::TIterator m_Iter;


		void Lock()
		{
			if(IsValid())
				m_pCache->LockItem(m_Iter);
		}

		void Unlock()
		{
			if(IsValid())
				m_pCache->UnlockItem(m_Iter);
		}

	public:
		TAccessor() : m_pCache(NULL) {}

		TAccessor(T_LRU_Cache& Cache, typename TStorage::TIterator Iter) :
			m_pCache(&Cache), m_Iter(Iter)
		{
			Lock();
		}

		TAccessor(const TAccessor& Accessor) : m_pCache(NULL)
			{ *this = Accessor; }
		
		~TAccessor()
			{ Invalidate(); }

		bool IsValid() const
			{ return m_pCache; }

		void Invalidate()
		{
			Unlock();

			m_pCache = NULL;
		}

		TAccessor& operator = (const TAccessor& Accessor)
		{
			if(Accessor == *this)
				return *this;

			Unlock();

			m_pCache = Accessor.m_pCache, m_Iter = Accessor.m_Iter;

			Lock();

			return *this;
		}

		ObjectType& operator * () const
			{ DEBUG_VERIFY(IsValid()); return m_Iter->m_Data; }

		ObjectType* operator -> () const
			{ DEBUG_VERIFY(IsValid()); return &m_Iter->m_Data; }

		bool operator == (const TAccessor& Accessor) const
			{ return m_pCache == Accessor.m_pCache && m_Iter == Accessor.m_Iter; }

		bool operator != (const TAccessor& Accessor) const
			{ return m_pCache != Accessor.m_pCache || m_Iter != Accessor.m_Iter; }
	};

private:
	mutable TCriticalSection m_AccessCS;

	size_t m_szMaxSize;

	size_t m_szTotalSize;

	TStorage m_Storage;

	TOrder m_Order;
	

	void Purge();

public:
	T_LRU_Cache();

	T_LRU_Cache(size_t szMaxSize);

	~T_LRU_Cache()
		{ Release(); }

	bool IsAllocated() const
		{ return m_szMaxSize; }

	void Release();

	void Allocate(size_t szMaxSize);

	void LockItem(typename TStorage::TIterator Iter);

	void UnlockItem(typename TStorage::TIterator Iter);

	template <class KeyType>
	TAccessor GetObject(const KeyType& Key);

	template <class KeyType>
	TAccessor operator [] (const KeyType& Key)
		{ return GetObject(Key); }
		
	template <class KeyType>
	void DelObject(const KeyType& Key);

	size_t GetTotalSize() const
	{
		DEBUG_VERIFY_ALLOCATION;

		TCriticalSectionLocker Locker0(m_AccessCS);

		return m_szTotalSize;
	}
};

template <class ObjectType>
T_LRU_Cache<ObjectType>::T_LRU_Cache()
{
	m_szMaxSize = 0;
}

template <class ObjectType>
T_LRU_Cache<ObjectType>::T_LRU_Cache(size_t szMaxSize)
{
	m_szMaxSize = 0;

	Allocate(szMaxSize);
}

template <class ObjectType>
void T_LRU_Cache<ObjectType>::Release()
{
	m_szMaxSize = 0;

	DEBUG_VERIFY(m_Order.GetN() == m_Storage.GetN()); // no accessors left

	m_Order.Clear();

	m_Storage.Clear();
}
template <class ObjectType>
void T_LRU_Cache<ObjectType>::Allocate(size_t szMaxSize)
{
	Release();

	DEBUG_VERIFY(szMaxSize > 0);

	m_szMaxSize = szMaxSize;

	m_szTotalSize = 0;
}

template <class ObjectType>
void T_LRU_Cache<ObjectType>::LockItem(typename TStorage::TIterator Iter)
{
	DEBUG_VERIFY_ALLOCATION;

	TCriticalSectionLocker Locker0(m_AccessCS);

	TItem& Item = *Iter;

	if(Item.m_szTimesLocked++ == 0 && Item.m_OrderIter) // '&&' is important for lock-on-add
	{
		typename TOrder::TIterator Iter;
		Iter.FromPVoid(Item.m_OrderIter);
		m_Order.Del(Iter);
	}
}

template <class ObjectType>
void T_LRU_Cache<ObjectType>::UnlockItem(typename TStorage::TIterator Iter)
{
	DEBUG_VERIFY_ALLOCATION;

	TCriticalSectionLocker Locker0(m_AccessCS);

	TItem& Item = *Iter;

	DEBUG_VERIFY(Item.m_szTimesLocked > 0);

	if(--Item.m_szTimesLocked == 0)
	{
		const typename TOrder::TIterator OrderIter = m_Order.AddFirst();

		*OrderIter = Iter, Item.m_OrderIter = OrderIter.AsPVoid();

		Purge();
	}
}

template <class ObjectType>
void T_LRU_Cache<ObjectType>::Purge()
{
	while(m_szTotalSize > m_szMaxSize && !m_Order.IsEmpty())
	{
		typename TStorage::TIterator StorageIter = *m_Order.GetLast();

		if(StorageIter.IsValid())
		{
			m_szTotalSize -= StorageIter->m_Data.GetCachedSize();

			m_Storage.Del(StorageIter);
		}

		m_Order.DelLast();
	}
}

template <class ObjectType>
template <class KeyType>
typename T_LRU_Cache<ObjectType>::TAccessor
	T_LRU_Cache<ObjectType>::GetObject(const KeyType& Key)
{
	DEBUG_VERIFY_ALLOCATION;

	TCriticalSectionLocker Locker0(m_AccessCS);

	typename TStorage::TIterator StorageIter = m_Storage.Find(Key);

	if(!StorageIter.IsValid())
	{
		StorageIter = m_Storage.Add(Key);

		m_szTotalSize += StorageIter->m_Data.GetCachedSize();

		Purge();
	}

	return TAccessor(*this, StorageIter);
}

template <class ObjectType>
template <class KeyType>
void T_LRU_Cache<ObjectType>::DelObject(const KeyType& Key)
{
	DEBUG_VERIFY_ALLOCATION;

	TCriticalSectionLocker Locker0(m_AccessCS);

	typename TStorage::TIterator StorageIter = m_Storage.Find(Key);

	if(!StorageIter.IsValid())
		return;

	DEBUG_VERIFY(!StorageIter->IsLocked());

	{
		typename TOrder::TIterator Iter;
		Iter.FromPVoid(StorageIter->m_OrderIter);
		m_Order.Del(Iter);
	}

	m_szTotalSize -= StorageIter->m_Data.GetCachedSize();

	m_Storage.Del(StorageIter);
}

#ifdef _MSC_VER

	template <class ObjectType, class KeyType>
	inline int Compare(const struct T_LRU_Cache<ObjectType>::TItem& Item, const KeyType& Key)
		{ return Compare(Item.m_Data, Key); }

	#define DECLARE_LRU_CACHE_COMPARE(ObjectType, KeyType)

#else // _MSC_VER

	#define DECLARE_LRU_CACHE_COMPARE(ObjectType, KeyType) \
		inline int Compare(const struct T_LRU_Cache<ObjectType>::TItem& Item, const KeyType& Key) \
			{ return Compare(Item.m_Data, Key); } \

#endif // _MSC_VER

#endif // lru_cache_h
