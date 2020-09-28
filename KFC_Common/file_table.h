#ifndef table_h
#define table_h

#include <KFC_KTL/tls_storage.h>
#include <KFC_KTL/fixed_item_heap_avl_storage.h>
#include "file.h"
#include "file_mapping.h"

#ifdef _MSC_VER
	#define FINDER_FILE_TABLE_SETTER \
		TThreadFileTableSetter Setter0(&m_Table)
#else // _MSC_VER
	#define FINDER_FILE_TABLE_SETTER \
		TThreadFileTableSetter Setter0(&TIndexBase::m_Table)
#endif // _MSC_VER

// ---------------------
// File table TLS item
// ---------------------
struct TFileTableTLS_Item : public T_TLS_Storage::TItem
{
private:
	static size_t ms_szIndex;

private:
	static T_TLS_Storage::TItem* Creator()
		{ return new TFileTableTLS_Item(); }

public:
	void* m_pTable;

public:
	static void Reserve()
		{ ms_szIndex = g_TLS_Storage.ReserveItemType(Creator); }	

	static void Free()
		{ g_TLS_Storage.FreeItemType(ms_szIndex); }	

	static TFileTableTLS_Item& Get()
		{ return (TFileTableTLS_Item&)g_TLS_Storage[ms_szIndex]; }

	TFileTableTLS_Item()
	{
		m_pTable = NULL;
	}
};

// -------------------------
// Thread file table setter
// -------------------------
class TThreadFileTableSetter
{
private:
	void* m_pOldTable;

public:
	TThreadFileTableSetter(void* pTable)
	{
		TFileTableTLS_Item& Item = TFileTableTLS_Item::Get();

		m_pOldTable = Item.m_pTable, Item.m_pTable = pTable;
	}

	~TThreadFileTableSetter()
	{
		TFileTableTLS_Item& Item = TFileTableTLS_Item::Get();
		
		Item.m_pTable = m_pOldTable;
	}
};

// -----------
// File table
// -----------
template <class t, size_t szNIndices>
class TFileTable
{
public:
	// Iterator
	struct TIterator
	{
	public:
		size_t x;

	public:
		TIterator()  : x(0) {}

		TIterator(size_t sx) : x(sx) {}

		bool IsValid() const
			{ return x; }

		void Invalidate()
			{ x = 0; }
	};

	// Iterators
	typedef TArray<TIterator> TIterators;

	// Index base
	class TIndexBase
	{
	public:
		// Item
		struct TItem
		{
		public:
			TIterator m_Iter;

			size_t m_szN;

		public:
			template <class k>
			TItem(const k&, TIterator Iter) : m_Iter(Iter) {}
		};

		// Storage base
/*		typedef TFixedItemHeapAVL_Storage
			<	TItem,
				TFileMappingGrowableAllocator,
				TSharedFixedItemHeapAccessor<TDirectPtr>::_>
					TStorageBase;*/

		typedef FIXED_ITEM_HEAP_AVL_STORAGE_WITH_TYPENAME(	TItem,
															TFileMappingGrowableAllocator,
															TSharedFixedItemHeapAccessor<TDirectPtr>::_) TStorageBase;

		// Storage
		class TStorage : public TStorageBase
		{
		private:
			static void UpdateProc(TStorageBase& Storage, typename TStorageBase::TIterator Iter)
			{
				typename TIndexBase::TItem& Item = Storage[Iter];

				typename TStorageBase::TIterator Left  = Storage.GetLeft (Iter);
				typename TStorageBase::TIterator Right = Storage.GetRight(Iter);

				Item.m_szN = 0;

				if(Left.IsValid())
					Item.m_szN += Storage[Left].m_szN;

				if(Right.IsValid())
					Item.m_szN += Storage[Right].m_szN;
			}

		public:
			TStorage() : TStorageBase(UpdateProc) {}
		};

	protected:
		TFileTable& m_Table;

		const size_t m_szOwnIndex;

		TStorage m_Storage;

	protected:
		TIndexBase(TFileTable& Table, size_t szOwnIndex) :
			 m_Table(Table), m_szOwnIndex(szOwnIndex) {}

		void Clear()
			{ m_Storage.Clear(); }

		virtual typename TStorage::TIterator
			Add(typename TFileTable::TIterator TableIter, const t& Data) = 0;

		virtual void Modify(typename TStorage::TIterator Iter, const t& Data) = 0;

		void Del(typename TStorage::TIterator Iter)
			{ m_Storage.Del(Iter); }

		friend class TFileTable;
	};

	// Index
	template <class k>
	class TIndex : public TIndexBase
	{
	public:
		// Storage
		typedef typename TIndexBase::TStorage TStorage;

	private:
		TIndex(TFileTable& Table, size_t szOwnIndex) :
		   TIndexBase(Table, szOwnIndex) {}

		typename TStorage::TIterator Add(typename TFileTable::TIterator TableIter, const t& Data)
			{ return TIndexBase::m_Storage.Add(k(Data), TableIter); }

		void Modify(typename TStorage::TIterator Iter, const t& Data)
			{ TIndexBase::m_Storage.ModifyArbitraryWithHint(Iter, k(Data)); }

	public:
		void ListAll(TIterators& RIters) const;

		TIterator GetFirst() const;
		TIterator GetLast () const;

		TIterator& ToFirst(TIterator& Iter) const
			{ return Iter = GetFirst(); }

		TIterator& ToLast(TIterator& Iter) const
			{ return Iter = GetLast(); }

		template <class kk>
		TIterator Find(const kk& Key) const;

		template <class kk>
		bool Has(const kk& Key) const
			{ return Find(Key).IsValid(); }

		TIterator GetPrev(TIterator Iter) const
		{
			DEBUG_VERIFY(Iter.IsValid());

			typename TStorage::TConstIterator Iter2 =
				TIndexBase::m_Table.m_DataStorage[Iter.x].m_IndexIters[TIndexBase::m_szOwnIndex];

			return TIndexBase::m_Storage.ToPrev(Iter2).IsValid() ? TIndexBase::m_Storage[Iter2].m_Iter : TIterator();
		}

		TIterator GetNext(TIterator Iter) const
		{
			DEBUG_VERIFY(Iter.IsValid());

			typename TStorage::TConstIterator Iter2 =
				TIndexBase::m_Table.m_DataStorage[Iter.x].m_IndexIters[TIndexBase::m_szOwnIndex];

			return TIndexBase::m_Storage.ToNext(Iter2).IsValid() ? TIndexBase::m_Storage[Iter2].m_Iter : TIterator();
		}

		TIterator& ToPrev(TIterator& Iter) const
			{ return Iter = GetPrev(Iter); }

		TIterator& ToNext(TIterator& Iter) const
			{ return Iter = GetNext(Iter); }

		template <class kk>
		void FindAll(const kk& Key, TIterators& RIters) const;

		template <class kk>
		void FindAllLess(const kk& Key, TIterators& RIters) const;

		template <class kk>
		void FindAllLessEq(const kk& Key, TIterators& RIters) const;

		template <class kk>
		void FindAllGreater(const kk& Key, TIterators& RIters) const;

		template <class kk>
		void FindAllGreaterEq(const kk& Key, TIterators& RIters) const;

		template <class kk>
		TIterator FindFirstGreater(const kk& Key) const;

		template <class kk>
		TIterator FindFirstGreaterEq(const kk& Key) const;

		template <class kk>
		TIterator FindLastLess(const kk& Key) const;

		template <class kk>
		TIterator FindLastLessEq(const kk& Key) const;

		size_t GetN() const
			{ return TIndexBase::m_Storage.GetN(); }

		friend class TFileTable;
	};

	// Data item
	struct TDataItem : public t
	{
	public:
		typename TIndexBase::TStorage::TIterator m_IndexIters[szNIndices];

	public:
		TDataItem() {}

		TDataItem(const t& Data) : t(Data) {}
	};

	// Data storage
	typedef TFixedItemHeap<TDataItem, TFileMappingGrowableAllocator> TDataStorage;	

private:
	TDataStorage m_DataStorage;

	TFileMapping m_IndexHeadersMapping;

	typename TIndexBase::TStorage::THeap m_IndicesHeap;

	TPtrHolder<TIndexBase> m_Indices[szNIndices];

	size_t m_szCurIndex;

public:
	TFileTable();

	~TFileTable()
		{ Release(); }

	template <class k>
	TIndex<k>& RegisterIndex();

	bool IsAllocated() const
		{ return m_DataStorage.IsAllocated(); }

	void Release();

	void Allocate(LPCTSTR pFileNamePrefix);

	void Clear();

	TIterator Add(const t& Data);

	void Modify(TIterator Iter, size_t szIndex = -1);

	void Del(TIterator Iter);

	void RebuildIndex(const TIterators& Iters, size_t szIndex);

	t& GetItem(TIterator Iter)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(Iter.IsValid());

		return m_DataStorage[Iter.x];
	}

	const t& GetItem(TIterator Iter) const
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(Iter.IsValid());

		return m_DataStorage[Iter.x];
	}

	t& operator [] (TIterator Iter)
		{ return GetItem(Iter); }

	const t& operator [] (TIterator Iter) const
		{ return GetItem(Iter); }
};

template <class t, size_t szNIndices>
inline TFileTable<t, szNIndices>& GetThreadFileTable()
{
	TFileTableTLS_Item& Item = TFileTableTLS_Item::Get();

	DEBUG_VERIFY(Item.m_pTable);

	return *(TFileTable<t, szNIndices>*)Item.m_pTable;
}

template <class t, size_t szNIndices>
inline int Compare(	struct TFileTable<t, szNIndices>::TIterator Iter1,
					struct TFileTable<t, szNIndices>::TIterator Iter2)
{
	return Compare(Iter1.x, Iter2.x);
}

#define ARGS_DEF	class t, size_t szNIndices
#define ARGS_USE	t, szNIndices

DECLARE_TEMPLATE_COMPARISON_OPERATORS(ARGS_DEF, struct TFileTable<ARGS_USE>::TIterator, struct TFileTable<ARGS_USE>::TIterator)

#undef ARGS_USE
#undef ARGS_DEF

template <class t, size_t szNIndices, class k>
inline int Compare(	const struct TFileTable<t, szNIndices>::TIndexBase::TItem&	Item,
					const k&													Key)
{
	TFileTable<t, szNIndices>& Table = GetThreadFileTable<t, szNIndices>();

	return Compare(Table[Item.m_Iter], Key);
}

// Index
template <class t, size_t szNIndices>
template <class k>
void TFileTable<t, szNIndices>::TIndex<k>::ListAll(TIterators& RIters) const
{
	DEBUG_VERIFY(TIndexBase::m_Table.IsAllocated());

	RIters.Clear();

//	FINDER_FILE_TABLE_SETTER; // no comparisons needed for entire listing

	for(typename TStorage::TConstIterator Iter = TIndexBase::m_Storage.GetFirst() ;
		Iter.IsValid() ;
		TIndexBase::m_Storage.ToNext(Iter))
	{
		RIters << TIndexBase::m_Storage[Iter].m_Iter;
	}
}

template <class t, size_t szNIndices>
template <class k>
typename TFileTable<t, szNIndices>::TIterator
	TFileTable<t, szNIndices>::TIndex<k>::GetFirst() const
{
	DEBUG_VERIFY(TIndexBase::m_Table.IsAllocated());

	typename TStorage::TConstIterator Iter = TIndexBase::m_Storage.GetFirst();

	return Iter.IsValid() ? TIndexBase::m_Storage[Iter].m_Iter : 0;
}

template <class t, size_t szNIndices>
template <class k>
typename TFileTable<t, szNIndices>::TIterator
	TFileTable<t, szNIndices>::TIndex<k>::GetLast() const
{
	DEBUG_VERIFY(TIndexBase::m_Table.IsAllocated());

	typename TStorage::TConstIterator Iter = TIndexBase::m_Storage.GetLast();

	return Iter.IsValid() ? TIndexBase::m_Storage[Iter].m_Iter : 0;
}

template <class t, size_t szNIndices>
template <class k>
template <class kk>
typename TFileTable<t, szNIndices>::TIterator
	TFileTable<t, szNIndices>::TIndex<k>::Find(const kk& Key) const
{
	DEBUG_VERIFY(TIndexBase::m_Table.IsAllocated());

	FINDER_FILE_TABLE_SETTER;

	typename TStorage::TConstIterator Iter = TIndexBase::m_Storage.Find(Key);

	return Iter.IsValid() ? TIndexBase::m_Storage[Iter].m_Iter : 0;
}

template <class t, size_t szNIndices>
template <class k>
template <class kk>
void TFileTable<t, szNIndices>::TIndex<k>::FindAll(const kk& Key, TIterators& RIters) const
{
	DEBUG_VERIFY(TIndexBase::m_Table.IsAllocated());

	RIters.Clear();

	FINDER_FILE_TABLE_SETTER;

	typename TStorage::TConstIterator FIter = TIndexBase::m_Storage.FindFirst(Key);

	if(!FIter.IsValid())
		return;

	typename TStorage::TConstIterator LIter = TIndexBase::m_Storage.FindFirstGreater(Key);

	for(typename TStorage::TConstIterator Iter = FIter ; Iter != LIter ; TIndexBase::m_Storage.ToNext(Iter))
		RIters << TIndexBase::m_Storage[Iter].m_Iter;
}

template <class t, size_t szNIndices>
template <class k>
template <class kk>
void TFileTable<t, szNIndices>::TIndex<k>::FindAllLess(const kk& Key, TIterators& RIters) const
{
	RIters.Clear();

	FINDER_FILE_TABLE_SETTER;

	typename TStorage::TConstIterator FIter = TIndexBase::m_Storage.GetFirst();

	if(!FIter.IsValid())
		return;

	typename TStorage::TConstIterator LIter = TIndexBase::m_Storage.FindFirst(Key);

	for(typename TStorage::TConstIterator Iter = FIter ; Iter != LIter ; TIndexBase::m_Storage.ToNext(Iter))
		RIters << TIndexBase::m_Storage[Iter].m_Iter;
}

template <class t, size_t szNIndices>
template <class k>
template <class kk>
void TFileTable<t, szNIndices>::TIndex<k>::FindAllLessEq(const kk& Key, TIterators& RIters) const
{
	RIters.Clear();

	FINDER_FILE_TABLE_SETTER;

	typename TStorage::TConstIterator FIter = TIndexBase::m_Storage.GetFirst();

	if(!FIter.IsValid())
		return;

	typename TStorage::TConstIterator LIter = TIndexBase::m_Storage.FindFirstGreater(Key);

	for(typename TStorage::TConstIterator Iter = FIter ; Iter != LIter ; TIndexBase::m_Storage.ToNext(Iter))
		RIters << TIndexBase::m_Storage[Iter].m_Iter;
}

template <class t, size_t szNIndices>
template <class k>
template <class kk>
void TFileTable<t, szNIndices>::TIndex<k>::FindAllGreater(const kk& Key, TIterators& RIters) const
{
	RIters.Clear();

	FINDER_FILE_TABLE_SETTER;

	typename TStorage::TConstIterator FIter = TIndexBase::m_Storage.FindFirstGreater(Key);

	if(!FIter.IsValid())
		return;

	typename TStorage::TConstIterator LIter = 0;

	for(typename TStorage::TConstIterator Iter = FIter ; Iter != LIter ; TIndexBase::m_Storage.ToNext(Iter))
		RIters << TIndexBase::m_Storage[Iter].m_Iter;
}

template <class t, size_t szNIndices>
template <class k>
template <class kk>
void TFileTable<t, szNIndices>::TIndex<k>::FindAllGreaterEq(const kk& Key, TIterators& RIters) const
{
	RIters.Clear();

	FINDER_FILE_TABLE_SETTER;

	typename TStorage::TConstIterator FIter = TIndexBase::m_Storage.FindFirst(Key);

	if(!FIter.IsValid())
		return;

	typename TStorage::TConstIterator LIter = 0;

	for(typename TStorage::TConstIterator Iter = FIter ; Iter != LIter ; TIndexBase::m_Storage.ToNext(Iter))
		RIters << TIndexBase::m_Storage[Iter].m_Iter;
}

template <class t, size_t szNIndices>
template <class k>
template <class kk>
typename TFileTable<t, szNIndices>::TIterator
	TFileTable<t, szNIndices>::TIndex<k>::FindFirstGreater(const kk& Key) const
{
	DEBUG_VERIFY(TIndexBase::m_Table.IsAllocated());

	FINDER_FILE_TABLE_SETTER;

	typename TStorage::TConstIterator Iter = TIndexBase::m_Storage.FindFirstGreater(Key);

	return Iter.IsValid() ? TIndexBase::m_Storage[Iter].m_Iter : 0;
}

template <class t, size_t szNIndices>
template <class k>
template <class kk>
typename TFileTable<t, szNIndices>::TIterator
	TFileTable<t, szNIndices>::TIndex<k>::FindFirstGreaterEq(const kk& Key) const
{
	DEBUG_VERIFY(TIndexBase::m_Table.IsAllocated());

	FINDER_FILE_TABLE_SETTER;

	typename TStorage::TConstIterator Iter = TIndexBase::m_Storage.FindFirstGreaterEq(Key);

	return Iter.IsValid() ? TIndexBase::m_Storage[Iter].m_Iter : 0;
}

template <class t, size_t szNIndices>
template <class k>
template <class kk>
typename TFileTable<t, szNIndices>::TIterator
	TFileTable<t, szNIndices>::TIndex<k>::FindLastLess(const kk& Key) const
{
	DEBUG_VERIFY(TIndexBase::m_Table.IsAllocated());

	FINDER_FILE_TABLE_SETTER;

	typename TStorage::TConstIterator Iter = TIndexBase::m_Storage.FindLastLess(Key);

	return Iter.IsValid() ? TIndexBase::m_Storage[Iter].m_Iter : 0;
}

template <class t, size_t szNIndices>
template <class k>
template <class kk>
typename TFileTable<t, szNIndices>::TIterator
	TFileTable<t, szNIndices>::TIndex<k>::FindLastLessEq(const kk& Key) const
{
	DEBUG_VERIFY(TIndexBase::m_Table.IsAllocated());

	FINDER_FILE_TABLE_SETTER;

	typename TStorage::TConstIterator Iter = TIndexBase::m_Storage.FindLastLessEq(Key);

	return Iter.IsValid() ? TIndexBase::m_Storage[Iter].m_Iter : 0;
}

// Table
template <class t, size_t szNIndices>
TFileTable<t, szNIndices>::TFileTable()
{
	m_szCurIndex = 0;
}

#ifdef _MSC_VER
	template <class t, size_t szNIndices>
	template <class k>
	typename TFileTable<t, szNIndices>::TIndex<k>& TFileTable<t, szNIndices>::RegisterIndex()
#else // _MSC_VER
	template <class t, size_t szNIndices>
	template <class k>
	typename TFileTable<t, szNIndices>::template TIndex<k>& TFileTable<t, szNIndices>::RegisterIndex()
#endif // _MSC_VER
{
	DEBUG_VERIFY(!IsAllocated());

	DEBUG_VERIFY(m_szCurIndex < szNIndices);

	TIndex<k>* pIndex = new TIndex<k>(*this, m_szCurIndex);

	m_Indices[m_szCurIndex] = pIndex;

	m_szCurIndex++;

	return *pIndex;
}

template <class t, size_t szNIndices>
void TFileTable<t, szNIndices>::Release()
{
	for(size_t i = m_szCurIndex - 1 ; i != -1 ; i--)
		m_Indices[i]->m_Storage.Release();

	m_IndicesHeap.Release();

	m_IndexHeadersMapping.Release();

	m_DataStorage.Release();
}

template <class t, size_t szNIndices>
void TFileTable<t, szNIndices>::Allocate(LPCTSTR pFileNamePrefix)
{
	Release();

	try
	{
		DEBUG_VERIFY(m_szCurIndex == szNIndices);

		const KString DataFileName			= (KString)pFileNamePrefix + TEXT(".dat");
		const KString IndexHeadersFileName	= (KString)pFileNamePrefix + TEXT(".idx.hdr");
		const KString IndexDataFileName		= (KString)pFileNamePrefix + TEXT(".idx.dat");

		bool bClear = false;

		if(	!FileExists(DataFileName)			||
			!FileExists(IndexHeadersFileName)	||
			!FileExists(IndexDataFileName)		||
			GetFileLength(IndexHeadersFileName) <
				sizeof(TIndexBase::TStorage::THeader) * szNIndices)
		{
			CreateEmptyFile(DataFileName);
			CreateEmptyFile(IndexHeadersFileName);
			CreateEmptyFile(IndexDataFileName);

			bClear = true;
		}

		{
			m_DataStorage.m_Allocator.Allocate(DataFileName, false);

			m_DataStorage.Allocate();
		}

		{
			m_IndexHeadersMapping.Allocate
				(	IndexHeadersFileName,
					false,
					sizeof(TIndexBase::TStorage::THeader) * szNIndices);
		}

		{
			m_IndicesHeap.m_Allocator.Allocate
				(IndexDataFileName, false);

			m_IndicesHeap.Allocate();
		}

		if(bClear)
			Clear();

		{
			typename TIndexBase::TStorage::THeader* pHeader =
				(typename TIndexBase::TStorage::THeader*)m_IndexHeadersMapping.GetDataPtr();

			for(size_t i = 0 ; i < szNIndices ; i++, pHeader++)
				m_Indices[i]->m_Storage.m_Accessor.Allocate(m_IndicesHeap, *pHeader);
		}
	}

	catch(...)
	{
		Release();
		throw;
	}
}

template <class t, size_t szNIndices>
void TFileTable<t, szNIndices>::Clear()
{
	DEBUG_VERIFY_ALLOCATION;

	m_DataStorage.Clear();

	m_IndicesHeap.Clear();

	typename TIndexBase::TStorage::THeader* pHeader = 
		(typename TIndexBase::TStorage::THeader*)m_IndexHeadersMapping.GetDataPtr();

	for(size_t i = 0 ; i < szNIndices ; i++, pHeader++)
		new(pHeader) typename TIndexBase::TStorage::THeader();
}

template <class t, size_t szNIndices>
typename TFileTable<t, szNIndices>::TIterator TFileTable<t, szNIndices>::Add(const t& Data)
{
	DEBUG_VERIFY_ALLOCATION;

	TThreadFileTableSetter Setter0(this);

	TIterator Iter = m_DataStorage.Reserve(Data);

	TDataItem& Item = m_DataStorage[Iter.x];

	for(size_t i = 0 ; i < szNIndices ; i++)
		Item.m_IndexIters[i] = m_Indices[i]->Add(Iter, Data);

	return Iter;
}

template <class t, size_t szNIndices>
void TFileTable<t, szNIndices>::Modify(TIterator Iter, size_t szIndex)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(Iter.IsValid());

	TThreadFileTableSetter Setter0(this);

	const TDataItem& Item = m_DataStorage[Iter.x];

	if(szIndex == -1)
	{
		for(size_t i = 0 ; i < szNIndices ; i++)
			m_Indices[i]->Modify(Item.m_IndexIters[i], Item);
	}
	else
	{
		DEBUG_VERIFY(szIndex < szNIndices);

		m_Indices[szIndex]->Modify(Item.m_IndexIters[szIndex], Item);
	}
}

template <class t, size_t szNIndices>
void TFileTable<t, szNIndices>::Del(TIterator Iter)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(Iter.IsValid());

	const TDataItem& Item = m_DataStorage[Iter.x];

	for(size_t i = 0 ; i < szNIndices ; i++)
		m_Indices[i]->Del(Item.m_IndexIters[i]);

	m_DataStorage.Free(Iter.x);
}

template <class t, size_t szNIndices>
void TFileTable<t, szNIndices>::RebuildIndex(const TIterators& Iters, size_t szIndex)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(szIndex < szNIndices);

	TThreadFileTableSetter Setter0(this);

	m_Indices[szIndex]->Clear();

	for(size_t i = 0 ; i < Iters.GetN() ; i++)
	{
		TDataItem& Item = m_DataStorage[Iters[i].x];

		Item.m_IndexIters[szIndex] = m_Indices[szIndex]->Add(Iters[i], Item);
	}
}

#endif // table_h
