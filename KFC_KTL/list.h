#ifndef list_h
#define list_h

#include "basic_types.h"
#include "stream.h"

// -----
// List
// -----

// Item
template <class ObjectType>
struct TListItem
{
private:
	TListItem();

	TListItem(const TListItem&);

	TListItem& operator = (const TListItem&);

public:
	ObjectType m_Data;

	TListItem* m_pPrev;
	TListItem* m_pNext;

public:
	TListItem(TListItem* pPrev, TListItem* pNext) :
		m_pPrev(pPrev), m_pNext(pNext) {}

	template <class ct>
	TListItem(TListItem* pPrev, TListItem* pNext, const ct& Data) :
		m_Data(Data), m_pPrev(pPrev), m_pNext(pNext) {}

	template <class ct1, class ct2>
	TListItem(TListItem* pPrev, TListItem* pNext, const ct1& Data1, const ct2& Data2) :
		m_Data(Data1, Data2), m_pPrev(pPrev), m_pNext(pNext) {}
};

// Iterator
template <class ObjectType>
struct TListIterator
{
public:
	typedef ObjectType TObject;
	
	typedef TListItem<ObjectType> TItem;

public:
	TItem* m_pItem;

public:
	TListIterator() : m_pItem(NULL) {}

	TListIterator(TItem* pSItem) : m_pItem(pSItem) {}

	TListIterator& FromPVoid(void* pData)
		{ m_pItem = (TItem*)pData; return *this; }

	void* AsPVoid() const
		{ return (void*)m_pItem; }

	bool IsValid() const
		{ return m_pItem != NULL; }

	void Invalidate()
		{ m_pItem = NULL; }
	
	bool IsFirst() const
		{ assert(IsValid()); return m_pItem->m_pPrev == NULL; }

	bool IsLast() const
		{ assert(IsValid()); return m_pItem->m_pNext == NULL; }

	TListIterator GetPrev() const
		{ assert(IsValid()); return m_pItem->m_pPrev; }

	TListIterator GetNext() const
		{ assert(IsValid()); return m_pItem->m_pNext; }

	TListIterator& ToPrev()
		{ return *this = GetPrev(); }

	TListIterator& ToNext()
		{ return *this = GetNext(); }

	TListIterator& operator -- ()
		{ return ToPrev(); }

	TListIterator& operator ++ ()
		{ return ToNext(); }

	TListIterator operator -- (int)
		{ const TListIterator Iter = *this; ToPrev(); return Iter; }

	TListIterator operator ++ (int)
		{ const TListIterator Iter = *this; ToNext(); return Iter; }

	TListIterator& operator += (int v)
	{
		while(v < 0)
			ToPrev(), v++;

		while(v > 0)
			ToNext(), v--;
		
		return *this;
	}

	TListIterator& operator -= (int v)
	{
		while(v < 0)
			ToNext(), v++;

		while(v > 0)
			ToPrev(), v--;
		
		return *this;
	}

	TListIterator operator + (int v) const
		{ return TListIterator(*this) += v; }

	TListIterator operator - (int v) const
		{ return TListIterator(*this) -= v; }

	ObjectType& operator [] (int v) const
		{ return *(*this + v); }

	ObjectType* operator -> () const
		{ assert(IsValid()); return &m_pItem->m_Data; }

	ObjectType& operator * () const
		{ assert(IsValid()); return m_pItem->m_Data; }
};

// Const iterator
template <class ObjectType>
struct TListConstIterator
{
public:
	typedef ObjectType TObject;

	typedef TListItem<ObjectType> TItem;

public:
	const TItem* m_pItem;

public:
	TListConstIterator() : m_pItem(NULL) {}

	TListConstIterator(const TItem* pSItem) : m_pItem(pSItem) {}

	TListConstIterator(TListIterator<ObjectType> Iter) : m_pItem(Iter.m_pItem) {}

	TListConstIterator& operator = (TListIterator<ObjectType> Iter)
		{ m_pItem = Iter.m_pItem; return *this; }

	TListConstIterator& FromPVoid(void* pData)
		{ m_pItem = (const TItem*)pData; return *this; }

	void* AsPVoid() const
		{ return (void*)m_pItem; }

	bool IsValid() const
		{ return m_pItem; }

	void Invalidate()
		{ m_pItem = NULL; }

	bool IsFirst() const
		{ assert(IsValid()); return m_pItem->m_pPrev == NULL; }

	bool IsLast() const
		{ assert(IsValid()); return m_pItem->m_pNext == NULL; }

	TListConstIterator GetPrev() const
		{ assert(IsValid()); return m_pItem->m_pPrev; }

	TListConstIterator GetNext() const
		{ assert(IsValid()); return m_pItem->m_pNext; }

	TListConstIterator& ToPrev()
		{ return *this = GetPrev(); }

	TListConstIterator& ToNext()
		{ return *this = GetNext(); }

	TListConstIterator& operator -- ()
		{ return ToPrev(); }

	TListConstIterator& operator ++ ()
		{ return ToNext(); }

	TListConstIterator operator -- (int)
		{ const TListConstIterator Iter = *this; ToPrev(); return Iter; }

	TListConstIterator operator ++ (int)
		{ const TListConstIterator Iter = *this; ToNext(); return Iter; }		

	TListConstIterator& operator -= (int v)
	{
		while(v < 0)
			ToNext(), v++;

		while(v > 0)
			ToPrev(), v--;
		
		return *this;
	}

	TListConstIterator& operator += (int v)
	{
		while(v < 0)
			ToPrev(), v++;

		while(v > 0)
			ToNext(), v--;
		
		return *this;
	}		

	TListConstIterator operator - (int v) const
		{ return TListConstIterator(*this) -= v; }

	TListConstIterator operator + (int v) const
		{ return TListConstIterator(*this) += v; }

	const ObjectType& operator [] (int v) const
		{ return *(*this + v); }

	const ObjectType* operator -> () const
		{ assert(IsValid()); return &m_pItem->m_Data; }

	const ObjectType& operator * () const
		{ assert(IsValid()); return m_pItem->m_Data; }
};

template <class ObjectType>
inline int Compare(TListIterator<ObjectType> v1, TListIterator<ObjectType> v2)
	{ return Compare(v1.AsPVoid(), v2.AsPVoid()); }

template <class ObjectType>
inline int Compare(TListIterator<ObjectType> v1, TListConstIterator<ObjectType> v2)
	{ return Compare(v1.AsPVoid(), v2.AsPVoid()); }

template <class ObjectType>
inline int Compare(TListConstIterator<ObjectType> v1, TListIterator<ObjectType> v2)
	{ return Compare(v1.AsPVoid(), v2.AsPVoid()); }

template <class ObjectType>
inline int Compare(TListConstIterator<ObjectType> v1, TListConstIterator<ObjectType> v2)
	{ return Compare(v1.AsPVoid(), v2.AsPVoid()); }

DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TListIterator<ObjectType>,		TListIterator<ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TListIterator<ObjectType>,		TListConstIterator<ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TListConstIterator<ObjectType>,	TListIterator<ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TListConstIterator<ObjectType>,	TListConstIterator<ObjectType>)

// List
template <class ObjectType>
class TList
{
public:
	typedef ObjectType TObject;
	
	typedef TListItem<ObjectType> TItem;
	
	typedef TListIterator<ObjectType> TIterator;
	
	typedef TListConstIterator<ObjectType> TConstIterator;

private:
	TItem* m_pFirst;
	TItem* m_pLast;

	size_t m_szN;

private:
	TIterator Add(TItem* pItem);

public:
	TList();

	TList(const TList& SList);

	~TList()
		{ Clear(); }

	size_t GetN() const
		{ return m_szN; }

	bool IsEmpty() const
		{ return m_szN == 0; }

	void Clear()
		{ SetN(0); }

	void SetN(size_t szSN);

	TIterator Add(TIterator Prev, TIterator Next)
		{ return Add(new TItem(Prev.m_pItem, Next.m_pItem)); }

	template <class ct>
	TIterator Add(TIterator Prev, TIterator Next, const ct& Data)
		{ return Add(new TItem(Prev.m_pItem, Next.m_pItem, Data)); }

	template <class ct1, class ct2>
	TIterator Add(TIterator Prev, TIterator Next, const ct1& Data1, const ct2& Data2)
		{ return Add(new TItem(Prev.m_pItem, Next.m_pItem, Data1, Data2)); }

	TIterator AddBefore(TIterator Iter)
		{ return Add(Iter.IsValid() ? Iter.GetPrev() : m_pLast, Iter); }

	template <class ct>
	TIterator AddBefore(TIterator Iter, const ct& Data)
		{ return Add(Iter.IsValid() ? Iter.GetPrev() : m_pLast, Iter, Data); }

	template <class ct1, class ct2>
	TIterator AddBefore(TIterator Iter, const ct1& Data1, const ct2& Data2)
		{ return Add(Iter.IsValid() ? Iter.GetPrev() : m_pLast, Iter, Data1, Data2); }

	TIterator AddAfter(TIterator Iter)
		{ return Add(Iter, Iter.IsValid() ? Iter.GetNext() : m_pFirst); }

	template <class ct>
	TIterator AddAfter(TIterator Iter, const ct& Data)
		{ return Add(Iter, Iter.IsValid() ? Iter.GetNext() : m_pFirst, Data); }

	template <class ct1, class ct2>
	TIterator AddAfter(TIterator Iter, const ct1& Data1, const ct2& Data2)
		{ return Add(Iter, Iter.IsValid() ? Iter.GetNext() : m_pFirst, Data1, Data2); }

	TIterator AddFirst()
		{ return AddAfter(NULL); }

	template <class ct>
	TIterator AddFirst(const ct& Data)
		{ return AddAfter(NULL, Data); }

	template <class ct1, class ct2>
	TIterator AddFirst(const ct1& Data1, const ct2& Data2)
		{ return AddAfter(NULL, Data1, Data2); }

	TIterator AddLast()
		{ return AddBefore(NULL); }

	template <class ct>
	TIterator AddLast(const ct& Data)
		{ return AddBefore(NULL, Data); }

	template <class ct1, class ct2>
	TIterator AddLast(const ct1& Data1, const ct2& Data2)
		{ return AddBefore(NULL, Data1, Data2); }

	void Del(TIterator Iter);

	void DelFirst()
		{ Del(GetFirst()); }

	void DelLast()
		{ Del(GetLast()); }

	TIterator GetFirst()
		{ return m_pFirst; }

	TConstIterator GetFirst() const
		{ return m_pFirst; }

	TIterator GetLast()
		{ return m_pLast; }

	TConstIterator GetLast() const
		{ return m_pLast; }

	TIterator GetIndexed(size_t szIndex);

	TConstIterator GetIndexed(size_t szIndex) const;

	TIterator GetIndexedRev(size_t szIndex);

	TConstIterator GetIndexedRev(size_t szIndex) const;

	TIterator operator + (int iIndex)
		{ return GetIndexed(iIndex); }	

	TConstIterator operator + (int iIndex) const
		{ return GetIndexed(iIndex); }

	TIterator operator - (int iIndex)
		{ return GetIndexed(-iIndex); }

	TConstIterator operator - (int iIndex) const
		{ return GetIndexed(-iIndex); }

	ObjectType& operator [] (int iIndex)
		{ return *GetIndexed(iIndex); }

	const ObjectType& operator [] (int iIndex) const
		{ return *GetIndexed(iIndex); }

	void Swap(TIterator Iter1, TIterator Iter2);

	TList& operator = (const TList& SList);

	TList& Load(TStream& Stream);

	const TList& Save(TStream& Stream) const;

	TList& MergeLeft	(TList& List);
	TList& MergeRight	(TList& List);

private:
	void SubMerge(	TIterator&				CIter,
					TIterator				LIter,
					TListItem<ObjectType>*&	pMFirst,
					TListItem<ObjectType>*&	pMLast);

	void SubSort(	TIterator&		FIter,
					TIterator&		LIter,
					size_t			szN,
					TQSortFunction*	pSF);

public:
	void Sort(TQSortFunction* pSF = CompareQSortFunction<ObjectType>)
		{ SubSort(make_temp(GetFirst()), make_temp(GetLast()), GetN(), pSF); }

	template <class k>
	TList& operator << (const k& Data)
		{ AddLast(Data); return *this; }
};

// List
template <class ObjectType>
TList<ObjectType>::TList()
{
	m_pFirst = m_pLast = NULL;

	m_szN = 0;
}

template <class ObjectType>
TList<ObjectType>::TList(const TList& SList)
{
	m_pFirst = m_pLast = NULL;

	m_szN = 0;

	*this = SList;
}

template <class ObjectType>
void TList<ObjectType>::SetN(size_t szSN)
{
	while(m_szN < szSN)
		AddLast();

	while(m_szN > szSN)
		DelLast();
}

template <class ObjectType>
typename TList<ObjectType>::TIterator TList<ObjectType>::Add(TItem* pItem)
{
	assert((pItem->m_pPrev ? pItem->m_pPrev->m_pNext : m_pFirst) == pItem->m_pNext);
	assert((pItem->m_pNext ? pItem->m_pNext->m_pPrev : m_pLast)  == pItem->m_pPrev);

	(pItem->m_pPrev ? pItem->m_pPrev->m_pNext : m_pFirst) = pItem;
	(pItem->m_pNext ? pItem->m_pNext->m_pPrev : m_pLast)  = pItem;

	m_szN++;

	return pItem;
}

template <class ObjectType>
void TList<ObjectType>::Del(TIterator Iter)
{
	assert(Iter.IsValid());

	m_szN--;

	(Iter.m_pItem->m_pPrev ? Iter.m_pItem->m_pPrev->m_pNext : m_pFirst) = Iter.m_pItem->m_pNext;
	(Iter.m_pItem->m_pNext ? Iter.m_pItem->m_pNext->m_pPrev : m_pLast)  = Iter.m_pItem->m_pPrev;	

	delete Iter.m_pItem;
}

template <class ObjectType>
typename TList<ObjectType>::TIterator TList<ObjectType>::GetIndexed(size_t szIndex)
{
	assert(szIndex <= m_szN);

	TIterator Iter;

	for(Iter = GetFirst() ; szIndex ; --szIndex, ++Iter);

	return Iter;
}

template <class ObjectType>
typename TList<ObjectType>::TConstIterator TList<ObjectType>::GetIndexed(size_t szIndex) const
{
	assert(szIndex <= m_szN);

	TConstIterator Iter;

	for(Iter = GetFirst() ; szIndex ; --szIndex, ++Iter);

	return Iter;
}

template <class ObjectType>
typename TList<ObjectType>::TIterator TList<ObjectType>::GetIndexedRev(size_t szIndex)
{
	assert(szIndex <= m_szN);

	TIterator Iter;

	for(Iter = GetLast() ; szIndex ; --szIndex, --Iter);

	return Iter;
}

template <class ObjectType>
typename TList<ObjectType>::TConstIterator TList<ObjectType>::GetIndexedRev(size_t szIndex) const
{
	assert(szIndex <= m_szN);

	TConstIterator Iter;

	for(Iter = GetLast() ; szIndex ; --szIndex, --Iter);

	return Iter;
}

template <class ObjectType>
void TList<ObjectType>::Swap(TIterator Iter1, TIterator Iter2)
{
	assert(Iter1.IsValid() && Iter2.IsValid());

	if(Iter1 == Iter2)
		return;

	if(Iter1.m_pItem->m_pNext == Iter2.m_pItem) // close case 1->2
	{
		(Iter1.m_pItem->m_pPrev ? Iter1.m_pItem->m_pPrev->m_pNext : m_pFirst) = Iter2.m_pItem;
		(Iter2.m_pItem->m_pNext ? Iter2.m_pItem->m_pNext->m_pPrev : m_pLast)  = Iter1.m_pItem;

		Iter1.m_pItem->m_pNext = Iter2.m_pItem->m_pNext;
		Iter2.m_pItem->m_pPrev = Iter1.m_pItem->m_pPrev;

		Iter2.m_pItem->m_pNext = Iter1.m_pItem;
		Iter1.m_pItem->m_pPrev = Iter2.m_pItem;
	}
	else if(Iter2.m_pItem->m_pNext == Iter1.m_pItem) // close case 2->1
	{
		(Iter2.m_pItem->m_pPrev ? Iter2.m_pItem->m_pPrev->m_pNext : m_pFirst) = Iter1.m_pItem;
		(Iter1.m_pItem->m_pNext ? Iter1.m_pItem->m_pNext->m_pPrev : m_pLast)  = Iter2.m_pItem;

		Iter2.m_pItem->m_pNext = Iter1.m_pItem->m_pNext;
		Iter1.m_pItem->m_pPrev = Iter2.m_pItem->m_pPrev;

		Iter1.m_pItem->m_pNext = Iter2.m_pItem;
		Iter2.m_pItem->m_pPrev = Iter1.m_pItem;
	}
	else // general case
	{
		(Iter1.m_pItem->m_pPrev ? Iter1.m_pItem->m_pPrev->m_pNext : m_pFirst) = Iter2.m_pItem;
		(Iter1.m_pItem->m_pNext ? Iter1.m_pItem->m_pNext->m_pPrev : m_pLast)  = Iter2.m_pItem;

		(Iter2.m_pItem->m_pPrev ? Iter2.m_pItem->m_pPrev->m_pNext : m_pFirst) = Iter1.m_pItem;
		(Iter2.m_pItem->m_pNext ? Iter2.m_pItem->m_pNext->m_pPrev : m_pLast)  = Iter1.m_pItem;

		::Swap(Iter1.m_pItem->m_pPrev, Iter2.m_pItem->m_pPrev);
		::Swap(Iter1.m_pItem->m_pNext, Iter2.m_pItem->m_pNext);
	}	
}

template <class ObjectType>
TList<ObjectType>& TList<ObjectType>::operator = (const TList<ObjectType>& SList)
{
	if(this == &SList)
		return *this;

	Clear();

	for(TConstIterator Iter = SList.GetFirst() ; Iter.IsValid() ; ++Iter)
		*this << *Iter;

	return *this;
}

template <class ObjectType>
TList<ObjectType>& TList<ObjectType>::Load(TStream& Stream)
{
	size_t szN;

	Stream >> szN;

	SetN(szN);

	TIterator Iter;

	for(Iter = GetFirst() ; Iter.IsValid() ; ++Iter)
		Stream >> *Iter;

	return *this;
}

template <class ObjectType>
const TList<ObjectType>& TList<ObjectType>::Save(TStream& Stream) const
{
	Stream << GetN();

	TConstIterator Iter;

	for(Iter = GetFirst() ; Iter.IsValid() ; ++Iter)
		Stream << *Iter;

	return *this;
}

template <class ObjectType>
inline TStream& operator >> (TStream& Stream, TList<ObjectType>& RList)
	{ return RList.Load(Stream), Stream; }

template <class ObjectType>
inline TStream& operator << (TStream& Stream, const TList<ObjectType>& List)
	{ return List.Save(Stream), Stream; }

template <class ObjectType>
inline int operator - (typename TList<ObjectType>::TConstIterator Iter1, typename TList<ObjectType>::TConstIterator Iter2)
{
	assert(Iter1.IsValid() && Iter2.IsValid());

	if(Iter1 == Iter2)
		return 0;

	int v1 = 0;

	while(!Iter1.IsFirst())
		Iter1.ToPrev(), v1++;

	int v2 = 0;

	while(!Iter2.IsFirst())
		Iter2.ToPrev(), v2++;

	assert(Iter1 == Iter2); // both should hit same target

	return v1 - v2;
}

template <class ObjectType>
TList<ObjectType>& TList<ObjectType>::MergeLeft(TList<ObjectType>& List)
{
	if(List.IsEmpty())
		return *this;

	if(IsEmpty())
	{
		m_pFirst	= List.m_pFirst;
		m_pLast		= List.m_pLast;
		m_szN		= List.m_szN;
	}
	else
	{
		List.m_pLast->m_pNext = m_pFirst, m_pFirst->m_pPrev = List.m_pLast;

		m_pFirst = List.m_pFirst;

		m_szN += List.m_szN;
	}

	List.m_pFirst	= NULL;
	List.m_pLast	= NULL;
	List.m_szN		= 0;

	return *this;
}

template <class ObjectType>
TList<ObjectType>& TList<ObjectType>::MergeRight(TList<ObjectType>& List)
{
	if(List.IsEmpty())
		return *this;

	if(IsEmpty())
	{
		m_pFirst	= List.m_pFirst;
		m_pLast		= List.m_pLast;
		m_szN		= List.m_szN;
	}
	else
	{
		m_pLast->m_pNext = List.m_pFirst, List.m_pFirst->m_pPrev = m_pLast;

		m_pLast = List.m_pLast;

		m_szN += List.m_szN;
	}

	List.m_pFirst	= NULL;
	List.m_pLast	= NULL;
	List.m_szN		= 0;

	return *this;
}

template <class ObjectType>
void TList<ObjectType>::SubMerge(	TIterator&				CIter,
									TIterator				LIter,
									TListItem<ObjectType>*&	pMFirst,
									TListItem<ObjectType>*&	pMLast)
{
	// Pre-advancing CIter
	TIterator NCIter = CIter == LIter ? TIterator() : CIter.GetNext();

	// Detaching CIter from main list
	(CIter.m_pItem->m_pPrev ? CIter.m_pItem->m_pPrev->m_pNext : m_pFirst) = CIter.m_pItem->m_pNext;
	(CIter.m_pItem->m_pNext ? CIter.m_pItem->m_pNext->m_pPrev : m_pLast ) = CIter.m_pItem->m_pPrev;

	// Attaching CIter to merge-list
	CIter.m_pItem->m_pPrev = pMLast;
	CIter.m_pItem->m_pNext = NULL;

	(pMLast ? pMLast->m_pNext : pMFirst) = CIter.m_pItem;
	pMLast = CIter.m_pItem;

	// Advancing CIter
	CIter = NCIter;
}

template <class ObjectType>
void TList<ObjectType>::SubSort(TIterator&		FIter,
								TIterator&		LIter,
								size_t			szN,
								TQSortFunction*	pSF)
{
	if(szN <= 1)
		return;

	size_t szHalfN = szN >> 1;

	TListItem<ObjectType>* pPFItem = FIter.GetPrev().m_pItem;
	TListItem<ObjectType>* pNLItem = LIter.GetNext().m_pItem;

	TIterator FIter1 = FIter;
	TIterator FIter2 = FIter + szHalfN;
	TIterator LIter1 = FIter2.GetPrev();
	TIterator LIter2 = LIter;

	SubSort(FIter1, LIter1, szHalfN, pSF);
	SubSort(FIter2, LIter2, szN - szHalfN, pSF);

	TListItem<ObjectType>* pMFirst = NULL;
	TListItem<ObjectType>* pMLast  = NULL;

	TIterator CIter1 = FIter1;
	TIterator CIter2 = FIter2;

	for(;;)
	{
		if(CIter1.IsValid() && CIter2.IsValid())
		{
			if(pSF(&*CIter1, &*CIter2) <= 0)
				SubMerge(CIter1, LIter1, pMFirst, pMLast);
			else
				SubMerge(CIter2, LIter2, pMFirst, pMLast);
		}
		else if(CIter1.IsValid())
		{
			SubMerge(CIter1, LIter1, pMFirst, pMLast);
		}
		else if(CIter2.IsValid())
		{
			SubMerge(CIter2, LIter2, pMFirst, pMLast);
		}
		else
		{
			break;
		}
	}

	assert(pMFirst && pMLast);

	pMFirst->m_pPrev = pPFItem;
	pMLast ->m_pNext = pNLItem;

	(pPFItem ? pPFItem->m_pNext : m_pFirst) = pMFirst;
	(pNLItem ? pNLItem->m_pPrev : m_pLast ) = pMLast;

	FIter.m_pItem = pMFirst;
	LIter.m_pItem = pMLast;
}

// List/iterator subtraction routines
template <class ObjectType>
inline int operator - (typename TList<ObjectType>::TConstIterator Iter1, typename TList<ObjectType>::TIterator Iter2)
	{ return Iter1 - TList<ObjectType>::TConstIterator(Iter2); }

template <class ObjectType>
inline int operator - (typename TList<ObjectType>::TConstIterator Iter, TList<ObjectType>& List)
	{ return Iter - List.GetFirst(); }

template <class ObjectType>
inline int operator - (typename TList<ObjectType>::TIterator Iter1, typename TList<ObjectType>::TConstIterator Iter2)
	{ return TList<ObjectType>::TConstIterator(Iter1) - Iter2; }

template <class ObjectType>
inline int operator - (typename TList<ObjectType>::TIterator Iter1, typename TList<ObjectType>::TIterator Iter2)
	{ return TList<ObjectType>::TConstIterator(Iter1) - TList<ObjectType>::TConstIterator(Iter2); }

template <class ObjectType>
inline int operator - (typename TList<ObjectType>::TIterator Iter, const TList<ObjectType>& List)
	{ return TList<ObjectType>::TConstIterator(Iter) - List.GetFirst(); }

template <class ObjectType>
inline int operator - (const TList<ObjectType>& List, typename TList<ObjectType>::TConstIterator Iter)
	{ return List.GetFirst() - Iter; }

template <class ObjectType>
inline int operator - (const TList<ObjectType>& List, typename TList<ObjectType>::TIterator Iter)
	{ return List.GetFirst() - TList<ObjectType>::TConstIterator(Iter); }

/*template <class ObjectType>
inline int operator - (const TList<ObjectType>& List1, const TList<ObjectType>& List2)
	{ return List1.GetFirst() - List2.GetFirst(); }*/

#endif // list_h
