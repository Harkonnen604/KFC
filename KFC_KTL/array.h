#ifndef array_h
#define array_h

#include "kfc_mem.h"
#include "placement_allocation.h"
#include "qsort.h"
#include "stream.h"

// #define _DEBUG_ARRAY_MODE

// C array size
#define ARRAY_SIZE(arr)		(sizeof(arr) / sizeof(*(arr)))

// ------
// Array
// ------
template <class t, bool bPOD_Type = false>
class TArray
{
public:
	typedef t TObject;

	enum { POD_Type = bPOD_Type};

private:
	t* m_pData;

	size_t m_szNAllocated;

	size_t m_szN;

public:
	TArray();

	TArray(const TArray& SArray);

	TArray(size_t szSN);

	TArray(const t* pData, size_t szN);

	~TArray()
		{ Clear(); }

	void Invalidate();

	void Clear(bool bFixAllocation = true)
		{ SetN(0, bFixAllocation); }

	void FixAllocation();

	bool CanStaticallyGrow(size_t szAmt) const
		{ return m_szN + szAmt <= m_szNAllocated; }

	t& SetN(size_t szSN, bool bFixAllocation = true);

	t& SetNAndReinit(size_t szSN, bool bFixAllocation = true);

	t& SetNAndZeroNewData(size_t szSN, bool bFixAllocation = true);

	t& EnsureN(size_t szSN);

	t& EnsureNAndZeroNewData(size_t szSN);

	t& Add(size_t szAmt = 1);

	t& Insert(size_t szIndex, size_t szAmt = 1);

	void DelNoFix(size_t szFirst, size_t szLast);

	void DelExceptNoFix(size_t szFirst, size_t szLast);

	void DelNoFix(size_t szIndex)
		{ DelNoFix(szIndex, szIndex + 1); }

	void DelExceptNoFix(size_t szIndex)
		{ DelExceptNoFix(szIndex, szIndex + 1); }

	void Del(size_t szFirst, size_t szLast)
		{ DelNoFix(szFirst, szLast), FixAllocation(); }

	void Del(size_t szIndex)
		{ DelNoFix(szIndex), FixAllocation(); }

	void DelExcept(size_t szFirst, size_t szLast)
		{ DelExceptNoFix(szFirst, szLast), FixAllocation(); }

	void DelExcept(size_t szIndex)
		{ DelExceptNoFix(szIndex), FixAllocation(); }

	TArray& operator = (const TArray& SArray);

	TArray& Reown(TArray& Array)
	{
		m_pData			= Array.m_pData,		Array.m_pData			= NULL;
		m_szNAllocated	= Array.m_szNAllocated,	Array.m_szNAllocated	= 0;
		m_szN			= Array.m_szN,			Array.m_szN				= 0;

		return *this;
	}

	t GetData(size_t szIndex) const
		{ assert(szIndex < m_szN); return m_pData[szIndex]; }

	t& GetDataRef(size_t szIndex)
		{ return m_pData[szIndex]; }

	const t& GetDataRef(size_t szIndex) const
		{ return m_pData[szIndex]; }

	t* GetDataPtr()
		{ return m_pData; }

	const t* GetDataPtr() const
		{ return m_pData; }

	void ZeroData();
	void ZeroData(size_t szIndex);
	void ZeroData(size_t szFirst, size_t szLast);

	void Sort(TQSortFunction* pQSortFunction = CompareQSortFunction<t>);

	void KillSortedDupes(TQSortFunction* pQSortFunction = CompareQSortFunction<t>, bool bFixAllocation = true);

	void SortAndKillDupes(TQSortFunction* pQSortFunction = CompareQSortFunction<t>, bool bFixAllocation = true);

	template <class k>
	size_t FindInSorted(const k& v) const;

	template <class k>
	size_t FindFirstInSorted(const k& v) const;

	template <class k>
	size_t FindFirstGreaterInSorted(const k& v) const;

	template <class k>
	size_t FindFirstGreaterEqInSorted(const k& v) const;

	template <class k>
	size_t FindLastInSorted(const k& v) const;

	template <class k>
	size_t FindLastLessInSorted(const k& v) const;

	template <class k>
	size_t FindLastLessEqInSorted(const k& v) const;

	template <class k>
	size_t AddInSorted(const k& v);

	template <class k>
	size_t FindOrAddInSorted(const k& v);

	static bool HasCommonSorted(const TArray&	a1,
								const TArray&	a2,
								TQSortFunction*	pQSortFunction = CompareQSortFunction<t>,
								size_t*			ri1 = NULL,
								size_t*			ri2 = NULL);

	t& operator [] (size_t szIndex)
		{ return GetDataRef(szIndex); }

	const t& operator [] (size_t szIndex) const
		{ return GetDataRef(szIndex); }

	bool IsLast(const t& Data) const
		{ return !IsEmpty() && GetDataRef(GetN() - 1) == Data; }

	size_t GetFirst		() const { assert(m_szN >= 1); return 0; }
	size_t GetPostFirst	() const { assert(m_szN >= 2); return 1; }

	size_t GetLast		() const { assert(m_szN >= 1); return GetN() - 1; }
	size_t GetPreLast	() const { assert(m_szN >= 2); return GetN() - 2; }

	t&			GetFirstItem()			{ return GetDataRef(GetFirst()); }
	const t&	GetFirstItem() const	{ return GetDataRef(GetFirst()); }

	t&			GetPostFirstItem()			{ return GetDataRef(GetPostFirst()); }
	const t&	GetPostFirstItem() const	{ return GetDataRef(GetPostFirst()); }

	t&			GetLastItem()		{ return GetDataRef(GetLast()); }
	const t&	GetLastItem() const	{ return GetDataRef(GetLast()); }

	t&			GetPreLastItem()		{ return GetDataRef(GetPreLast()); }
	const t&	GetPreLastItem() const	{ return GetDataRef(GetPreLast()); }

	void DelLast()
		{ Del(GetLast()); }

	void DelLastNoFix()
		{ DelNoFix(GetLast()); }

	TArray& ReadItems(TStream& Stream);

	const TArray& WriteItems(TStream& Stream) const;

	TArray& Load(TStream& Stream);

	const TArray& Save(TStream& Stream) const;

	size_t GetN() const
		{ return m_szN; }

	bool IsEmpty() const
		{ return m_szN == 0; }

	t* operator + (int i)
		{ return m_pData + i; }

	const t* operator + (int i) const
		{ return m_pData + i; }

	TArray& operator += (const TArray& Arr);

	TArray& Reverse()
	{
		if(IsEmpty())
			return *this;

		t* p1 = m_pData;
		t* p2 = m_pData + GetLast();

		for( ; p1 < p2 ; p1++, p2--)
			Swap(*p1, *p2);

		return *this;
	}

	template <class k>
	TArray& operator << (const k& Data)
		{ Add() = Data; return *this; }
};

template <class t, bool bPOD_Type>
TArray<t, bPOD_Type>::TArray()
{
	m_pData = NULL;

	m_szNAllocated = 0;

	m_szN = 0;
}

template <class t, bool bPOD_Type>
TArray<t, bPOD_Type>::TArray(const TArray& SArray)
{
	m_pData = NULL;

	m_szNAllocated = 0;

	m_szN = 0;

	*this = SArray;
}

template <class t, bool bPOD_Type>
TArray<t, bPOD_Type>::TArray(size_t szSN)
{
	m_pData = NULL;

	m_szNAllocated = 0;

	m_szN = 0;

	SetN(szSN);
}

template <class t, bool bPOD_Type>
TArray<t, bPOD_Type>::TArray(const t* pData, size_t szN)
{
	assert(pData || !szN);

	if(!szN)
	{
		m_pData = NULL;

		m_szNAllocated = 0;
	}
	else
	{
		#ifdef _DEBUG_ARRAY_MODE
			m_szNAllocated = szN;
		#else // _DEBUG_ARRAY_MODE
			for(m_szNAllocated = 1; m_szNAllocated < szN ; m_szNAllocated <<= 1);
		#endif // _DEBUG_ARRAY_MODE

		m_pData = (t*)kfc_malloc(m_szNAllocated * sizeof(t));

		if(bPOD_Type)
		{
			memcpy(m_pData, pData, szN * sizeof(t));
		}
		else
		{
			for(size_t i = 0 ; i < szN ; i++)
				new(m_pData + i) t(pData[i]);
		}
	}

	m_szN = szN;
}

template <class t, bool bPOD_Type>
void TArray<t, bPOD_Type>::Invalidate()
{
	m_pData = NULL;

	m_szNAllocated = 0;

	m_szN = 0;
}

template <class t, bool bPOD_Type>
void TArray<t, bPOD_Type>::FixAllocation()
{
	assert(m_szN <= m_szNAllocated);

	if(m_szN == 0)
	{
		kfc_free(m_pData), m_szNAllocated = 0;
		return;
	}

	const size_t szOldNAllocated = m_szNAllocated;

	#ifdef _DEBUG_ARRAY_MODE
	{
		m_szNAllocated = m_szN;
	}
	#else // _DEBUG_ARRAY_MODE
	{
		size_t x;

		while((x = (m_szNAllocated >> 1)) >= m_szN)
			m_szNAllocated = x;
	}
	#endif // _DEBUG_ARRAY_MODE

	if(m_szNAllocated != szOldNAllocated)
		kfc_realloc(m_pData, m_szNAllocated * sizeof(t));
}

template <class t, bool bPOD_Type>
t& TArray<t, bPOD_Type>::SetN(size_t szSN, bool bFixAllocation)
{
	assert(m_szN <= m_szNAllocated);

	if(szSN < m_szN) // shrinking
	{
		if(bFixAllocation)
			Del(szSN, m_szN);
		else
			DelNoFix(szSN, m_szN);
	}
	else if(szSN > m_szN) // expanding
	{
		const size_t szOldNAllocated = m_szNAllocated;

		#ifdef _DEBUG_ARRAY_MODE
		{
			m_szNAllocated = szSN;
		}
		#else // _DEBUG_ARRAY_MODE
		{
			if(!m_szNAllocated)
				m_szNAllocated = 1;

			while(m_szNAllocated < szSN)
				m_szNAllocated <<= 1;
		}
		#endif // _DEBUG_ARRAY_MODE

		if(m_szNAllocated != szOldNAllocated)
			kfc_realloc(m_pData, m_szNAllocated * sizeof(t));

		if(!bPOD_Type)
		{
			while(m_szN < szSN)
				new(m_pData + m_szN++) t();
		}
		else
		{
			m_szN = szSN;
		}
	}

	if(bFixAllocation)
		FixAllocation();

	return *m_pData;
}

template <class t, bool bPOD_Type>
t& TArray<t, bPOD_Type>::SetNAndReinit(size_t szSN, bool bFixAllocation)
{
	const size_t szNRecreate = Min(m_szN, szSN);

	SetN(szSN, bFixAllocation);

	if(bPOD_Type)
	{
		memset(m_pData, 0, szNRecreate * sizeof(t));
	}
	else
	{
		size_t i;

		for(i = szNRecreate - 1 ; i != UINT_MAX ; i--)
			m_pData[i].~t();

		for(i = 0 ; i < szNRecreate ; i++)
			new(m_pData + i) t();
	}

	return *m_pData;
}

template <class t, bool bPOD_Type>
t& TArray<t, bPOD_Type>::SetNAndZeroNewData(size_t szSN, bool bFixAllocation)
{
	assert(bPOD_Type);

	const size_t szOldN = m_szN;

	SetN(szSN, bFixAllocation);

	if(szOldN < m_szN)
		ZeroData(szOldN, m_szN);

	return *m_pData;
}

template <class t, bool bPOD_Type>
t& TArray<t, bPOD_Type>::EnsureN(size_t szSN)
{
	if(m_szN < szSN)
		SetN(szSN);

	return *m_pData;
}

template <class t, bool bPOD_Type>
t& TArray<t, bPOD_Type>::EnsureNAndZeroNewData(size_t szSN)
{
	if(m_szN < szSN)
		SetNAndZeroNewData(szSN);

	return *m_pData;
}

template <class t, bool bPOD_Type>
t& TArray<t, bPOD_Type>::Add(size_t szAmt)
{
	SetN(m_szN + szAmt, false);

	return m_pData[m_szN - szAmt];
}

template <class t, bool bPOD_Type>
t& TArray<t, bPOD_Type>::Insert(size_t szIndex, size_t szAmt)
{
	assert(szIndex <= m_szN);

	size_t szSN = m_szN + szAmt;

	const size_t szOldNAllocated = m_szNAllocated;

	#ifdef _DEBUG_ARRAY_MODE
	{
		m_szNAllocated = szSN;
	}
	#else // _DEBUG_ARRAY_MODE
	{
		if(!m_szNAllocated)
			m_szNAllocated = 1;

		while(m_szNAllocated < szSN)
			m_szNAllocated <<= 1;
	}
	#endif // _DEBUG_ARRAY_MODE

	if(m_szNAllocated != szOldNAllocated)
		kfc_realloc(m_pData, m_szNAllocated * sizeof(t));

	memmove(m_pData + szIndex + szAmt,
			m_pData + szIndex,
			(m_szN - szIndex) * sizeof(t));

	if(!bPOD_Type)
	{
		for(size_t i = 0 ; i < szAmt ; i++)
			new(m_pData + szIndex + i) t();
	}

	m_szN = szSN;

	return m_pData[szIndex];
}

template <class t, bool bPOD_Type>
void TArray<t, bPOD_Type>::DelNoFix(size_t szFirst, size_t szLast)
{
	assert(szFirst <= szLast && szLast <= m_szN);

	if(!bPOD_Type)
	{
		size_t i;

		for(i = szLast - 1 ; i != szFirst - 1 ; i--)
			m_pData[i].~t();
	}

	memmove(m_pData	+ szFirst,
			m_pData	+ szLast,
			(m_szN - szLast) * sizeof(t));

	m_szN -= szLast - szFirst;
}

template <class t, bool bPOD_Type>
void TArray<t, bPOD_Type>::DelExceptNoFix(size_t szFirst, size_t szLast)
{
	assert(szFirst <= szLast && szLast <= m_szN);

	if(!bPOD_Type)
	{
		size_t i;

		for(i = m_szN - 1 ; i != szLast - 1 ; i--)
			m_pData[i].~t();

		for(i = szFirst - 1 ; i != UINT_MAX ; i--)
			m_pData[i].~t();
	}

	memmove(m_pData,
			m_pData + szFirst,
			(szLast - szFirst) * sizeof(t));

	m_szN = szLast - szFirst;
}

template <class t, bool bPOD_Type>
TArray<t, bPOD_Type>& TArray<t, bPOD_Type>::operator = (const TArray& SArray)
{
	if(this == &SArray)
		return *this;

	SetN(SArray.GetN());

	if(bPOD_Type)
	{
		memcpy(GetDataPtr(), SArray.GetDataPtr(), m_szN * sizeof(t));
	}
	else
	{
		size_t i;

		for(i = 0 ; i < m_szN ; i++)
			m_pData[i] = SArray[i];
	}

	return *this;
}

template <class t, bool bPOD_Type>
void TArray<t, bPOD_Type>::ZeroData()
{
	assert(bPOD_Type);

	memset(m_pData, 0, m_szN * sizeof(t));
}

template <class t, bool bPOD_Type>
void TArray<t, bPOD_Type>::ZeroData(size_t szIndex)
{
	assert(bPOD_Type);

	assert(szIndex < m_szN);

	memset(m_pData + szIndex, 0, sizeof(t));
}

template <class t, bool bPOD_Type>
void TArray<t, bPOD_Type>::ZeroData(size_t szFirst, size_t szLast)
{
	assert(bPOD_Type);

	assert(szFirst <= szLast && szLast <= m_szN);

	memset(m_pData + szFirst, 0, (szLast - szFirst) * sizeof(t));
}

template <class t, bool bPOD_Type>
void TArray<t, bPOD_Type>::Sort(TQSortFunction* pQSortFunction)
{
	qsort(m_pData, m_szN, sizeof(t), pQSortFunction);
}

template <class t, bool bPOD_Type>
void TArray<t, bPOD_Type>::KillSortedDupes(TQSortFunction* pQSortFunction, bool bFixAllocation)
{
	if(IsEmpty())
		return;

	size_t i;

	t* pCmp = m_pData;

	t* pCur;

	if(bPOD_Type)
	{
		for(pCur = m_pData + 1, i = m_szN - 1 ;
			i && pQSortFunction(pCmp, pCur) ;
			pCmp++, i--, pCur++);

		for(;;)
		{
			for( ; i && !pQSortFunction(pCmp, pCur) ; i--, pCur++);

			if(!i)
				break;

			*++pCmp = *pCur++, i--;
		}
	}
	else
	{
		for(pCur = m_pData + 1, i = m_szN - 1 ;
			i && pQSortFunction(pCmp, pCur) ;
			pCmp++, i--, pCur++);

		for(;;)
		{
			for( ; i && !pQSortFunction(pCmp, pCur) ; i--, pCur++)
				pCur->~t();

			if(!i)
				break;

			memcpy(++pCmp, pCur++, sizeof(t)), i--;
		}
	}

	m_szN = (pCmp - m_pData) + 1;

	if(bFixAllocation)
		FixAllocation();
}

template <class t, bool bPOD_Type>
void TArray<t, bPOD_Type>::SortAndKillDupes(TQSortFunction* pQSortFunction, bool bFixAllocation)
{
	Sort(pQSortFunction);

	KillSortedDupes(pQSortFunction, bFixAllocation);
}

template <class t, bool bPOD_Type>
template <class k>
size_t TArray<t, bPOD_Type>::FindInSorted(const k& v) const
{
	if(IsEmpty() || Compare(GetFirstItem(), v) > 0 || Compare(GetLastItem(), v) < 0)
		return UINT_MAX;

	int l = 0;
	int r = m_szN - 1;

	for(;;)
	{
		int m = (l + r) >> 1;

		int d = Compare(m_pData[m], v);

		if(!d)
			return m;

		if(d < 0)
			l = m + 1;
		else
			r = m - 1;

		if(l > r)
			return UINT_MAX;
	}
}

template <class t, bool bPOD_Type>
template <class k>
size_t TArray<t, bPOD_Type>::FindFirstInSorted(const k& v) const
{
	if(IsEmpty() || Compare(GetFirstItem(), v) > 0 || Compare(GetLastItem(), v) < 0)
		return UINT_MAX;

	int l = 0;
	int r = m_szN - 1;

	size_t x = UINT_MAX;

	for(;;)
	{
		const int m = (l + r) >> 1;

		const int d = Compare(m_pData[m], v);

		if(!d)
			x = m;

		if(d < 0)
			l = m + 1;
		else
			r = m - 1;

		if(l > r)
			return x;
	}
}

template <class t, bool bPOD_Type>
template <class k>
size_t TArray<t, bPOD_Type>::FindFirstGreaterInSorted(const k& v) const
{
	if(IsEmpty() || Compare(GetLastItem(), v) <= 0)
		return UINT_MAX;

	int l = 0;
	int r = m_szN - 1;

	size_t x = UINT_MAX;

	for(;;)
	{
		const int m = (l + r) >> 1;

		const int d = Compare(m_pData[m], v);

		if(d <= 0)
		{
			l = m + 1;
		}
		else
		{
			x = m;
			r = m - 1;
		}

		if(l > r)
			return x;
	}
}

template <class t, bool bPOD_Type>
template <class k>
size_t TArray<t, bPOD_Type>::FindFirstGreaterEqInSorted(const k& v) const
{
	if(IsEmpty() || Compare(GetLastItem(), v) < 0)
		return UINT_MAX;

	int l = 0;
	int r = m_szN - 1;

	size_t x = UINT_MAX;

	for(;;)
	{
		const int m = (l + r) >> 1;

		const int d = Compare(m_pData[m], v);

		if(d < 0)
		{
			l = m + 1;
		}
		else
		{
			x = m;
			r = m - 1;
		}

		if(l > r)
			return x;
	}
}

template <class t, bool bPOD_Type>
template <class k>
size_t TArray<t, bPOD_Type>::FindLastInSorted(const k& v) const
{
	if(IsEmpty() || Compare(GetFirstItem(), v) > 0 || Compare(GetLastItem(), v) < 0)
		return UINT_MAX;

	int l = 0;
	int r = m_szN - 1;

	size_t x = UINT_MAX;

	for(;;)
	{
		const int m = (l + r) >> 1;

		const int d = Compare(m_pData[m], v);

		if(!d)
			x = m;

		if(d <= 0)
			l = m + 1;
		else
			r = m - 1;

		if(l > r)
			return x;
	}
}

template <class t, bool bPOD_Type>
template <class k>
size_t TArray<t, bPOD_Type>::FindLastLessInSorted(const k& v) const
{
	if(IsEmpty() || Compare(GetFirstItem(), v) >= 0)
		return UINT_MAX;

	int l = 0;
	int r = m_szN - 1;

	size_t x = UINT_MAX;

	for(;;)
	{
		const int m = (l + r) >> 1;

		const int d = Compare(m_pData[m], v);

		if(d < 0)
		{
			x = m;
			l = m + 1;
		}
		else
		{
			r = m - 1;
		}

		if(l > r)
			return x;
	}
}

template <class t, bool bPOD_Type>
template <class k>
size_t TArray<t, bPOD_Type>::FindLastLessEqInSorted(const k& v) const
{
	if(IsEmpty() || Compare(GetFirstItem(), v) > 0)
		return UINT_MAX;

	int l = 0;
	int r = m_szN - 1;

	size_t x = UINT_MAX;

	for(;;)
	{
		const int m = (l + r) >> 1;

		const int d = Compare(m_pData[m], v);

		if(d <= 0)
		{
			x = m;
			l = m + 1;
		}
		else
		{
			r = m - 1;
		}

		if(l > r)
			return x;
	}
}

template <class t, bool bPOD_Type>
template <class k>
size_t TArray<t, bPOD_Type>::AddInSorted(const k& v)
{
	if(IsEmpty() || Compare(GetLastItem(), v) <= 0)
		return Add() = v, GetLast();

	size_t szIndex = FindFirstGreaterInSorted(v);
	assert(szIndex != UINT_MAX);

	Insert(szIndex) = v;

	return szIndex;
}

template <class t, bool bPOD_Type>
template <class k>
size_t TArray<t, bPOD_Type>::FindOrAddInSorted(const k& v)
{
	size_t szIndex = FindInSorted(v);

	if(szIndex == UINT_MAX)
		szIndex = AddInSorted(v);

	return szIndex;
}

template <class t, bool bPOD_Type>
bool TArray<t, bPOD_Type>::HasCommonSorted(const TArray&	a1,
											const TArray&	a2,
											TQSortFunction*	pQSortFunction,
											size_t*			ri1,
											size_t*			ri2)
{
	if(a1.IsEmpty() || a2.IsEmpty())
		return false;

	size_t i1 = 0, i2 = 0;

	for(;;)
	{
		int d;

		while((d = pQSortFunction(a1.m_pData + i1, a2.m_pData + i2)) < 0)
		{
			if(++i1 == a1.GetN())
				return false;
		}

		if(!d)
			break;

		while((d = pQSortFunction(a2.m_pData + i2, a1.m_pData + i1)) < 0)
		{
			if(++i2 == a2.GetN())
				return false;
		}

		if(!d)
			break;
	}

	if(ri1)
		*ri1 = i1;

	if(ri2)
		*ri2 = i2;

	return true;
}

template <class t, bool bPOD_Type>
TArray<t, bPOD_Type>& TArray<t, bPOD_Type>::ReadItems(TStream& Stream)
{
	if(bPOD_Type)
	{
		STREAM_READ_ARR(Stream, m_pData, m_szN, t);
	}
	else
	{
		size_t i;

		for(i = 0 ; i < m_szN ; i++)
			Stream >> m_pData[i];
	}

	return *this;
}

template <class t, bool bPOD_Type>
const TArray<t, bPOD_Type>& TArray<t, bPOD_Type>::WriteItems(TStream& Stream) const
{
	if(bPOD_Type)
	{
		STREAM_WRITE_ARR(Stream, m_pData, m_szN, t);
	}
	else
	{
		size_t i;

		for(i = 0 ; i < m_szN ; i++)
			Stream << m_pData[i];
	}

	return *this;
}

template <class t, bool bPOD_Type>
TArray<t, bPOD_Type>& TArray<t, bPOD_Type>::operator += (const TArray<t, bPOD_Type>& Arr)
{
	size_t szN = Arr.GetN();

	t* pDst = &Add(szN);

	const t* pSrc = Arr.GetDataPtr();

	if(bPOD_Type)
	{
		memcpy(pDst, pSrc, szN * sizeof(t));
	}
	else
	{
		assert(false); // default constructors are already called, should use POD-style adder

		for( ; szN ; szN--, pSrc++, pDst++)
			new(pDst) t(*pSrc);
	}

	return *this;
}

template <class t, bool bPOD_Type>
TArray<t, bPOD_Type>& TArray<t, bPOD_Type>::Load(TStream& Stream)
	{ return SetN(ReadVar<size_t>(Stream)), ReadItems(Stream); }

template <class t, bool bPOD_Type>
const TArray<t, bPOD_Type>& TArray<t, bPOD_Type>::Save(TStream& Stream) const
	{ return Stream << m_szN, WriteItems(Stream); }

template <class t, bool bPOD_Type>
TStream& operator >> (TStream& Stream, TArray<t, bPOD_Type>& RArray)
	{ return RArray.Load(Stream), Stream; }

template <class t, bool bPOD_Type>
TStream& operator << (TStream& Stream, const TArray<t, bPOD_Type>& Array)
	{ return Array.Save(Stream), Stream; }

// --------------
// Array clearer
// --------------
template <class t, bool bPOD_Type = false>
class TArrayClearer
{
private:
	TArray<t, bPOD_Type>& m_Array;

public:
	TArrayClearer(TArray<t, bPOD_Type>& Array) : m_Array(Array) {}

	~TArrayClearer()
		{ m_Array.Clear(); }
};

#endif // array_h
