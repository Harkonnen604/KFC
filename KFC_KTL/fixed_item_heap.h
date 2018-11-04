#ifndef fixed_item_heap_h
#define fixed_item_heap_h

#include "growable_allocator.h"

// ----------------
// Fixed item heap
// ----------------
template <class ObjectType, class AllocatorType>
class TFixedItemHeap
{
public:
	typedef AllocatorType TAllocator;

private:
	// Header
	struct THeader
	{
	public:
		size_t m_szFirstEmpty;

	public:
		THeader() : m_szFirstEmpty(0) {}
	};

private:
	THeader& GetHeader()
		{ return *(THeader*)m_Allocator.GetDataPtr(); }

	const THeader& GetHeader() const
		{ return (const_cast<TFixedItemHeap*>(this))->GetHeader(); }

	size_t& GetFirstEmpty()
		{ return GetHeader().m_szFirstEmpty; }

	const size_t GetFirstEmpty() const
		{ return (const_cast<TFixedItemHeap*>(this))->GetFirstEmpty(); }

	size_t& GetNextEmpty(size_t szOffset)
		{ return *(size_t*)(m_Allocator.GetDataPtr() + szOffset); }

	size_t GetNextEmpty(size_t szOffset) const
		{ return (const_cast<TFixedItemHeap*>(this))->GetNextEmpty(szOffset); }

	size_t InternalReserve();

	void InternalFree(size_t szOffset);

public:
	TAllocator m_Allocator;

	TFixedItemHeap(const TFixedItemHeap&);

	TFixedItemHeap& operator = (const TFixedItemHeap&);

public:
	TFixedItemHeap();

	~TFixedItemHeap()
		{ Release(); }

	bool IsAllocated() const
		{ return m_Allocator.IsAllocated(); }

	void Release();

	void Allocate() {}

	void Clear();

	// Means allocator is minimized
	bool IsTotallyClean() const;

	size_t GetFirstCleanOffset() const
	{
		DEBUG_VERIFY_ALLOCATION;

		return sizeof(THeader);
	}

	size_t Reserve()
	{
		DEBUG_VERIFY_ALLOCATION;

		const size_t szOffset = InternalReserve();

		new(GetDataPtr(szOffset)) ObjectType();

		return szOffset;
	}

	template <class CreatorType>
	size_t Reserve(const CreatorType& Data)
	{
		DEBUG_VERIFY_ALLOCATION;

		const size_t szOffset = InternalReserve();

		new(GetDataPtr(szOffset)) ObjectType(Data);

		return szOffset;
	}

	template <class CreatorType1, class CreatorType2>
	size_t Reserve(const CreatorType1& Data1, const CreatorType2& Data2)
	{
		DEBUG_VERIFY_ALLOCATION;

		const size_t szOffset = InternalReserve();

		new(GetDataPtr(szOffset)) ObjectType(Data1, Data2);

		return szOffset;
	}

	template <class CreatorType1, class CreatorType2, class CreatorType3>
	size_t Reserve(const CreatorType1& Data1, const CreatorType2& Data2, const CreatorType3& Data3)
	{
		DEBUG_VERIFY_ALLOCATION;

		const size_t szOffset = InternalReserve();

		new(GetDataPtr(szOffset)) ObjectType(Data1, Data2, Data3);

		return szOffset;
	}

	bool IsValidOffset(size_t szOffset) const
	{
		DEBUG_VERIFY_ALLOCATION;

		return	szOffset >= GetFirstCleanOffset() &&
				szOffset + sizeof(ObjectType) <= m_Allocator.GetN() &&
				!(	(szOffset - GetFirstCleanOffset()) %
						ALIGNED_DATA_SIZE(sizeof(ObjectType), size_t));
	}

	void Free(size_t szOffset)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(IsValidOffset(szOffset));

		InternalFree(szOffset);
	}

	void FreeWithDestruction(size_t szOffset)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(IsValidOffset(szOffset));

		GetDataPtr(szOffset)->~ObjectType();

		InternalFree(szOffset);
	}

	ObjectType* GetDataPtr(size_t szOffset)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(szOffset);

		return (ObjectType*)(m_Allocator.GetDataPtr() + szOffset);
	}

	const ObjectType* GetDataPtr(size_t szOffset) const
		{ return (const_cast<TFixedItemHeap*>(this))->GetDataPtr(szOffset); }

	size_t GetOffset(const ObjectType* pData) const
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(pData);

		return (BYTE*)pData - m_Allocator.GetDataPtr();
	}

	ObjectType& operator [] (size_t szOffset)
	{
		DEBUG_VERIFY(IsValidOffset(szOffset));

		return *GetDataPtr(szOffset);
	}

	const ObjectType& operator [] (size_t szOffset) const
	{
		DEBUG_VERIFY(IsValidOffset(szOffset));

		return *GetDataPtr(szOffset);
	}

	ObjectType* operator + (size_t szOffset)
	{
		DEBUG_VERIFY(IsValidOffset(szOffset));

		return GetDataPtr(szOffset);
	}

	const ObjectType* operator + (size_t szOffset) const
	{
		DEBUG_VERIFY(IsValidOffset(szOffset));

		return GetDataPtr(szOffset);
	}
};

template <class ObjectType, class AllocatorType>
TFixedItemHeap<ObjectType, AllocatorType>::TFixedItemHeap()
{
}

template <class ObjectType, class AllocatorType>
void TFixedItemHeap<ObjectType, AllocatorType>::Release()
{
	m_Allocator.Release();
}

template <class ObjectType, class AllocatorType>
void TFixedItemHeap<ObjectType, AllocatorType>::Clear()
{
	DEBUG_VERIFY_ALLOCATION;

	m_Allocator.SetN(GetFirstCleanOffset());

	new(&GetHeader()) THeader();
}

template <class ObjectType, class AllocatorType>
bool TFixedItemHeap<ObjectType, AllocatorType>::IsTotallyClean() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_Allocator.GetN() == GetFirstCleanOffset();
}

template <class ObjectType, class AllocatorType>
size_t TFixedItemHeap<ObjectType, AllocatorType>::InternalReserve()
{
	DEBUG_VERIFY_ALLOCATION;

	size_t szOffset;

	size_t& szFirstEmpty = GetFirstEmpty();

	if(szFirstEmpty)
		szOffset = szFirstEmpty, szFirstEmpty = GetNextEmpty(szFirstEmpty);
	else
		szOffset = m_Allocator.Add(Max(sizeof(size_t), ALIGNED_DATA_SIZE(sizeof(ObjectType), size_t)));

	return szOffset;
}

template <class ObjectType, class AllocatorType>
void TFixedItemHeap<ObjectType, AllocatorType>::InternalFree(size_t szOffset)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(szOffset);

	size_t& szFirstEmpty = GetFirstEmpty();

	GetNextEmpty(szOffset) = szFirstEmpty, szFirstEmpty = szOffset;
}

// -------------------------
// Fixed item heap with aux
// -------------------------
template <class ObjectType, class AllocatorType, class AuxType>
class TFixedItemHeapWithAux
{
public:
	typedef AllocatorType TAllocator;

private:
	// Header
	struct THeader
	{
	public:
		size_t m_szFirstEmpty;

	public:
		THeader() : m_szFirstEmpty(0) {}
	};

private:
	THeader& GetHeader()
		{ return *(THeader*)m_Allocator.GetDataPtr(); }

	const THeader& GetHeader() const
		{ return (const_cast<TFixedItemHeapWithAux*>(this))->GetHeader(); }

	size_t& GetFirstEmpty()
		{ return GetHeader().m_szFirstEmpty; }

	const size_t GetFirstEmpty() const
		{ return (const_cast<TFixedItemHeapWithAux*>(this))->GetFirstEmpty(); }

	size_t& GetNextEmpty(size_t szOffset)
		{ return *(size_t*)(m_Allocator.GetDataPtr() + szOffset); }

	size_t GetNextEmpty(size_t szOffset) const
		{ return (const_cast<TFixedItemHeapWithAux*>(this))->GetNextEmpty(szOffset); }

	size_t InternalReserve();

	void InternalFree(size_t szOffset);

private:
	size_t m_szAuxDataSize;

public:
	TAllocator m_Allocator;

public:
	TFixedItemHeapWithAux();

	~TFixedItemHeapWithAux()
		{ Release(); }

	bool IsAllocated() const
		{ return m_Allocator.IsAllocated(); }

	void Release();

	void Allocate() {}

	void AllocateWithAux() {}

	void Clear();

	void ClearWithAux();

	// Means allocator is minimized
	bool IsTotallyClean() const;

	// Means allocator is minimized, and 'bool t::IsClean() const' returns 'true'
	bool IsTotallyCleanWithAux() const;

	// Means allocator is minimized and aux data equals 't()'
	bool IsTotallyCleanWithAuxDefaultCmp() const;

	AuxType& GetAuxData()
	{
		DEBUG_VERIFY_ALLOCATION;

		return *reinterpret_cast<AuxType*>(&GetHeader() + 1);
	}

	const AuxType& GetAuxData() const
		{ return (const_cast<TFixedItemHeapWithAux*>(this))->GetAuxData(); }

	size_t GetFirstCleanOffset() const
	{
		DEBUG_VERIFY_ALLOCATION;

		return sizeof(THeader) + ALIGNED_DATA_SIZE(sizeof(AuxType), size_t);
	}

	size_t Reserve()
	{
		DEBUG_VERIFY_ALLOCATION;

		const size_t szOffset = InternalReserve();

		new(GetDataPtr(szOffset)) ObjectType();

		return szOffset;
	}

	template <class CreatorType>
	size_t Reserve(const CreatorType& Data)
	{
		DEBUG_VERIFY_ALLOCATION;

		const size_t szOffset = InternalReserve();

		new(GetDataPtr(szOffset)) ObjectType(Data);

		return szOffset;
	}

	template <class CreatorType1, class CreatorType2>
	size_t Reserve(const CreatorType1& Data1, const CreatorType2& Data2)
	{
		DEBUG_VERIFY_ALLOCATION;

		const size_t szOffset = InternalReserve();

		new(GetDataPtr(szOffset)) ObjectType(Data1, Data2);

		return szOffset;
	}

	template <class CreatorType1, class CreatorType2, class CreatorType3>
	size_t Reserve(const CreatorType1& Data1, const CreatorType2& Data2, const CreatorType3& Data3)
	{
		DEBUG_VERIFY_ALLOCATION;

		const size_t szOffset = InternalReserve();

		new(GetDataPtr(szOffset)) ObjectType(Data1, Data2, Data3);

		return szOffset;
	}

	bool IsValidOffset(size_t szOffset) const
	{
		DEBUG_VERIFY_ALLOCATION;

		return	szOffset >= GetFirstCleanOffset() &&
				szOffset + sizeof(ObjectType) <= m_Allocator.GetN() &&
				!(	(szOffset - GetFirstCleanOffset()) %
						ALIGNED_DATA_SIZE(sizeof(ObjectType), size_t));
	}

	void Free(size_t szOffset)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(IsValidOffset(szOffset));

		InternalFree(szOffset);
	}

	void FreeWithDestruction(size_t szOffset)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(IsValidOffset(szOffset));

		GetDataPtr(szOffset)->~ObjectType();

		InternalFree(szOffset);
	}

	ObjectType* GetDataPtr(size_t szOffset)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(szOffset);

		return (ObjectType*)(m_Allocator.GetDataPtr() + szOffset);
	}

	const ObjectType* GetDataPtr(size_t szOffset) const
		{ return (const_cast<TFixedItemHeapWithAux*>(this))->GetDataPtr(szOffset); }

	size_t GetOffset(const ObjectType* pData) const
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(pData);

		return (BYTE*)pData - m_Allocator.GetDataPtr();
	}

	ObjectType& operator [] (size_t szOffset)
	{
		DEBUG_VERIFY(IsValidOffset(szOffset));

		return *GetDataPtr(szOffset);
	}

	const ObjectType& operator [] (size_t szOffset) const
	{
		DEBUG_VERIFY(IsValidOffset(szOffset));

		return *GetDataPtr(szOffset);
	}

	ObjectType* operator + (size_t szOffset)
	{
		DEBUG_VERIFY(IsValidOffset(szOffset));

		return GetDataPtr(szOffset);
	}

	const ObjectType* operator + (size_t szOffset) const
	{
		DEBUG_VERIFY(IsValidOffset(szOffset));

		return GetDataPtr(szOffset);
	}
};

template <class ObjectType, class AllocatorType, class AuxType>
TFixedItemHeapWithAux<ObjectType, AllocatorType, AuxType>::TFixedItemHeapWithAux()
{
	m_szAuxDataSize = UINT_MAX;
}

template <class ObjectType, class AllocatorType, class AuxType>
void TFixedItemHeapWithAux<ObjectType, AllocatorType, AuxType>::Release()
{
	m_szAuxDataSize = UINT_MAX;

	m_Allocator.Release();
}

template <class ObjectType, class AllocatorType, class AuxType>
void TFixedItemHeapWithAux<ObjectType, AllocatorType, AuxType>::Clear()
{
	DEBUG_VERIFY_ALLOCATION;

	m_Allocator.SetN(GetFirstCleanOffset());

	new(&GetHeader()) THeader();
}

template <class ObjectType, class AllocatorType, class AuxType>
void TFixedItemHeapWithAux<ObjectType, AllocatorType, AuxType>::ClearWithAux()
{
	DEBUG_VERIFY_ALLOCATION;

	Clear();

	new(&GetAuxData()) AuxType();
}

template <class ObjectType, class AllocatorType, class AuxType>
bool TFixedItemHeapWithAux<ObjectType, AllocatorType, AuxType>::IsTotallyClean() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_Allocator.GetN() == GetFirstCleanOffset();
}

template <class ObjectType, class AllocatorType, class AuxType>
bool TFixedItemHeapWithAux<ObjectType, AllocatorType, AuxType>::IsTotallyCleanWithAux() const
{
	DEBUG_VERIFY_ALLOCATION;

	return IsTotallyClean() && GetAuxData().IsClean();
}

template <class ObjectType, class AllocatorType, class AuxType>
bool TFixedItemHeapWithAux<ObjectType, AllocatorType, AuxType>::IsTotallyCleanWithAuxDefaultCmp() const
{
	DEBUG_VERIFY_ALLOCATION;

	return IsTotallyClean() && GetAuxData() == AuxType();
}

template <class ObjectType, class AllocatorType, class AuxType>
size_t TFixedItemHeapWithAux<ObjectType, AllocatorType, AuxType>::InternalReserve()
{
	DEBUG_VERIFY_ALLOCATION;

	size_t szOffset;

	size_t& szFirstEmpty = GetFirstEmpty();

	if(szFirstEmpty)
		szOffset = szFirstEmpty, szFirstEmpty = GetNextEmpty(szFirstEmpty);
	else
		szOffset = m_Allocator.Add(Max(sizeof(size_t), ALIGNED_DATA_SIZE(sizeof(ObjectType), size_t)));

	return szOffset;
}

template <class ObjectType, class AllocatorType, class AuxType>
void TFixedItemHeapWithAux<ObjectType, AllocatorType, AuxType>::InternalFree(size_t szOffset)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(szOffset);

	size_t& szFirstEmpty = GetFirstEmpty();

	GetNextEmpty(szOffset) = szFirstEmpty, szFirstEmpty = szOffset;
}

#endif // fixed_item_heap_h
