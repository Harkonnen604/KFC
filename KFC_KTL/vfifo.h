#ifndef vfifo_h
#define vfifo_h

#include "basic_types.h"
#include "placement_allocation.h"

// -----------
// Value FIFO
// -----------
template <class ObjectType, bool bPOD_Type = false>
class TValueFIFO
{
private:
	bool m_bAllocated;

	ObjectType*	m_pData;
	size_t		m_szMaxItems;
	size_t		m_szNItems;
	
	size_t m_szHeadIndex;
	size_t m_szTailIndex;

	bool m_bAutoRemoveHead;

public:
	TValueFIFO();

	TValueFIFO(size_t szMaxItems, bool bAutoRemoveHead);
	
	~TValueFIFO()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release();

	void Allocate(size_t szMaxItems, bool bAutoRemoveHead);

	void Clear();

	ObjectType&	Put();
	ObjectType&	Put(const ObjectType& SData);
	ObjectType	Get();

	ObjectType&			GetHead();
	const ObjectType&	GetHead() const;

	ObjectType&			GetTail();
	const ObjectType&	GetTail() const;

	void RemoveHead();
	void RemoveTail();

	ObjectType&			operator [] (size_t szIndex);
	const ObjectType&	operator [] (size_t szIndex) const;

	size_t GetN() const
		{ DEBUG_VERIFY_ALLOCATION; return m_szNItems; }

	bool IsEmpty() const
		{ DEBUG_VERIFY_ALLOCATION; return !m_szNItems; }

	bool IsFull() const
		{ DEBUG_VERIFY_ALLOCATION; return m_szNItems == m_szMaxItems; }
};

template <class ObjectType, bool bPOD_Type>
TValueFIFO<ObjectType, bPOD_Type>::TValueFIFO()
{
	m_bAllocated = false;

	m_pData = NULL;
}

template <class ObjectType, bool bPOD_Type>
TValueFIFO<ObjectType, bPOD_Type>::TValueFIFO(size_t szMaxItems, bool bAutoRemoveHead)
{
	m_bAllocated = false;

	m_pData = NULL;

	Allocate(szMaxItems, bAutoRemoveHead);
}

template <class ObjectType, bool bPOD_Type>
void TValueFIFO<ObjectType, bPOD_Type>::Release()
{
	if(IsAllocated())
		Clear();
		
	m_bAllocated = false;

	if(m_pData)
		kfc_free(m_pData), m_pData = NULL;
}

template <class ObjectType, bool bPOD_Type>
void TValueFIFO<ObjectType, bPOD_Type>::Allocate(size_t szMaxItems, bool bAutoRemoveHead)
{
	Release();

	try
	{
		DEBUG_VERIFY(szMaxItems > 0);

		m_szMaxItems = szMaxItems;

		m_bAutoRemoveHead = bAutoRemoveHead;
		
		m_pData = (ObjectType*)kfc_malloc(m_szMaxItems * sizeof(ObjectType));

		m_szNItems = 0;

		m_szHeadIndex = m_szTailIndex = 0;

		m_bAllocated = true;
	}

	catch(...)
	{
		Release();
		throw;
	}
}

template <class ObjectType, bool bPOD_Type>
void TValueFIFO<ObjectType, bPOD_Type>::Clear()
{
	DEBUG_VERIFY_ALLOCATION;

	if(!bPOD_Type && !IsEmpty())
	{
		size_t i;
		
		if(m_szHeadIndex < m_szTailIndex)
		{
			for(i = m_szHeadIndex ; i < m_szTailIndex ; ++i)
				m_pData[i].~ObjectType();
		}
		else
		{
			for(i = m_szHeadIndex ; i < m_szMaxItems ; ++i)
				m_pData[i].~ObjectType();

			for(i = 0 ; i < m_szTailIndex ; ++i)
				m_pData[i].~ObjectType();
		}
	}

	m_szNItems = 0;
	
	m_szHeadIndex = m_szTailIndex = 0;
}

template <class ObjectType, bool bPOD_Type>
ObjectType& TValueFIFO<ObjectType, bPOD_Type>::Put()
{
	DEBUG_VERIFY_ALLOCATION;

	if(IsFull())
	{
		DEBUG_VERIFY(m_bAutoRemoveHead);

		RemoveHead();
	}

	if(!bPOD_Type)
		new(m_pData + m_szTailIndex) ObjectType();

	if(++m_szTailIndex == m_szMaxItems)
		m_szTailIndex = 0;

	m_szNItems++;

	return GetTail();
}

template <class ObjectType, bool bPOD_Type>
ObjectType& TValueFIFO<ObjectType, bPOD_Type>::Put(const ObjectType& SData)
{
	DEBUG_VERIFY_ALLOCATION;

	if(IsFull())
	{
		DEBUG_VERIFY(m_bAutoRemoveHead);

		RemoveHead();
	}

	if(bPOD_Type)
		m_pData[m_szTailIndex] = SData;
	else
		new(m_pData + m_szTailIndex) ObjectType(SData);

	if(++m_szTailIndex == m_szMaxItems)
		m_szTailIndex = 0;

	m_szNItems++;

	return GetTail();
}

template <class ObjectType, bool bPOD_Type>
ObjectType TValueFIFO<ObjectType, bPOD_Type>::Get()
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(!IsEmpty());

	ObjectType RetVal = GetHead();

	RemoveHead();

	return RetVal;
}

template <class ObjectType, bool bPOD_Type>
ObjectType& TValueFIFO<ObjectType, bPOD_Type>::GetHead()
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(!IsEmpty());

	return m_pData[m_szHeadIndex];
}

template <class ObjectType, bool bPOD_Type>
const ObjectType& TValueFIFO<ObjectType, bPOD_Type>::GetHead() const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(!IsEmpty());

	return m_pData[m_szHeadIndex];
}

template <class ObjectType, bool bPOD_Type>
ObjectType& TValueFIFO<ObjectType, bPOD_Type>::GetTail()
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(!IsEmpty());

	return m_pData[m_szTailIndex ? (m_szTailIndex - 1) : (m_szMaxItems - 1)];
}

template <class ObjectType, bool bPOD_Type>
const ObjectType& TValueFIFO<ObjectType, bPOD_Type>::GetTail() const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(!IsEmpty());

	return m_pData[m_szTailIndex ? (m_szTailIndex - 1) : (m_szMaxItems - 1)];
}

template <class ObjectType, bool bPOD_Type>
void TValueFIFO<ObjectType, bPOD_Type>::RemoveHead()
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(!IsEmpty());

	if(!bPOD_Type)
		m_pData[m_szHeadIndex].~ObjectType();

	if(++m_szHeadIndex == m_szMaxItems)
		m_szHeadIndex = 0;
	
	m_szNItems--;
}

template <class ObjectType, bool bPOD_Type>
void TValueFIFO<ObjectType, bPOD_Type>::RemoveTail()
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(!IsEmpty());

	if(--m_szTailIndex == UINT_MAX)
		m_szTailIndex = m_szMaxItems - 1;

	m_szNItems--;

	if(!bPOD_Type)
		m_pData[m_szTailIndex].~ObjectType();
}

template <class ObjectType, bool bPOD_Type>
ObjectType& TValueFIFO<ObjectType, bPOD_Type>::operator [] (size_t szIndex)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(szIndex < m_szNItems);

	const size_t szDstIndex = m_szHeadIndex + szIndex;

	return m_pData[szDstIndex < m_szMaxItems ? szDstIndex : szDstIndex - m_szMaxItems];
}

template <class ObjectType, bool bPOD_Type>
const ObjectType& TValueFIFO<ObjectType, bPOD_Type>::operator [] (size_t szIndex) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(szIndex < m_szNItems);

	const size_t szDstIndex = m_szHeadIndex + szIndex;

	return m_pData[szDstIndex < m_szMaxItems ? szDstIndex : szDstIndex - m_szMaxItems];
}

#endif // vfifo_h
