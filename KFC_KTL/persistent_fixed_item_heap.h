#ifndef persistent_fixed_item_heap_h
#define persistent_fixed_item_heap_h

#include "pow2.h"

// ---------------------------
// Persistent fixed item heap
// ---------------------------
template <class ObjectType>
class TPersistentFixedItemHeap
{
private:
	enum { item_size = union_size_t<void*, ObjectType>::res };

public:
	template <class t>
	struct fittable_t
	{
		enum { res = item_size >= sizeof(t) && !(item_size % alignment_t<t>::res) };
	};

private:
	size_t m_szBlockSize;

	TArray<void*, true> m_Blocks;

	void* m_pFirstEmpty;

	size_t m_szN;

private:
	void* AddBlock();

	void InternalFree(const void* p)
		{ *(void**)p = m_pFirstEmpty, m_pFirstEmpty = (void*)p, m_szN--; }

	void* InternalReserve()
	{
		if(!m_pFirstEmpty)
			m_pFirstEmpty = AddBlock();

		void* p = m_pFirstEmpty;

		m_pFirstEmpty = *(void**)p;

		m_szN++;

		return p;
	}

public:
	TPersistentFixedItemHeap()
		{ m_szBlockSize = 0; }

	TPersistentFixedItemHeap(size_t szBlockSize)
		{ m_szBlockSize = 0; Allocate(szBlockSize); }

	~TPersistentFixedItemHeap()
		{ Release(); }

	bool IsAllocated() const
		{ return m_szBlockSize; }

	void Release();

	void Allocate(size_t szBlockSize);

	void Free(const ObjectType* pObject)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(pObject);

		pObject->~ObjectType();

		return InternalFree(pObject);
	}

	void FreeWithoutDestruction(const ObjectType* pObject)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(pObject);

		return InternalFree(pObject);
	}
	
	ObjectType* ReserveWithoutConstruction()
	{
		DEBUG_VERIFY_ALLOCATION;

		ObjectType* pObject = (ObjectType*)InternalReserve();

		return pObject;
	}

	ObjectType* Reserve()
	{
		DEBUG_VERIFY_ALLOCATION;

		ObjectType* pObject = (ObjectType*)InternalReserve();

		new(pObject) ObjectType();

		return pObject;
	}

	template <class CreatorType>
	ObjectType* Reserve(const CreatorType& Data)
	{
		DEBUG_VERIFY_ALLOCATION;

		ObjectType* pObject = (ObjectType*)InternalReserve();

		new(pObject) ObjectType(Data);

		return pObject;
	}

	template <class CreatorType1, class CreatorType2>
	ObjectType* Reserve(const CreatorType1& Data1, const CreatorType2& Data2)
	{
		DEBUG_VERIFY_ALLOCATION;

		ObjectType* pObject = (ObjectType*)InternalReserve();

		new(pObject) ObjectType(Data1, Data2);

		return pObject;
	}

	size_t GetN() const
		{ DEBUG_VERIFY_ALLOCATION; return m_szN; }

	bool IsEmpty() const
		{ DEBUG_VERIFY_ALLOCATION; return !m_szN; }
		
	size_t GetBlockSize() const
		{ DEBUG_VERIFY_ALLOCATION; return m_szBlockSize; }
};

template <class ObjectType>
void TPersistentFixedItemHeap<ObjectType>::Release()
{
	if(m_szBlockSize)
	{
		assert(!m_szN);

		for(size_t i = m_Blocks.GetN() - 1 ; i != -1 ; i--)
			kfc_free(m_Blocks[i]);

		m_Blocks.Clear();

		m_szBlockSize = 0;
	}
}

template <class ObjectType>
void TPersistentFixedItemHeap<ObjectType>::Allocate(size_t szBlockSize)
{
	Release();

	DEBUG_VERIFY(szBlockSize);

	DEBUG_VERIFY(IsPow2(szBlockSize));

	m_szBlockSize = szBlockSize;

	m_pFirstEmpty = NULL;

	m_szN = 0;
}

template <class ObjectType>
void* TPersistentFixedItemHeap<ObjectType>::AddBlock()
{
	m_Blocks.Add() = kfc_malloc(m_szBlockSize * item_size);

	BYTE* pBlock = (BYTE*)m_Blocks.GetLastItem();

	size_t pi, i;

	for(pi = 0, i = item_size ; i < m_szBlockSize * item_size ; pi = i, i += item_size)
		*(void**)(pBlock+pi) = pBlock + i;

	*(void**)(pBlock+pi) = NULL;

	m_szBlockSize <<= 1;

	return pBlock;
}

#endif // persistent_fixed_item_heap_h
