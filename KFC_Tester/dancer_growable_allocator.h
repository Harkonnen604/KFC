#ifndef dancer_growable_allocator_h
#define dancer_growable_allocator_h

#include <KFC_KTL/growable_allocator.h>
#include <KFC_KTL/pow2.h>

// --------------------------
// Dancer growable allocator
// --------------------------
class TDancerGrowableAllocator : public TGrowableAllocator
{
private:
	BYTE* m_pData;

	size_t m_szN;

	size_t m_szNAllocated;

public:
	TDancerGrowableAllocator() : m_pData(NULL), m_szN(0), m_szNAllocated(0) {}

	~TDancerGrowableAllocator()
		{ Release(); }

	bool IsAllocated() const
		{ return true; }

	void Release()
		{ delete[] m_pData, m_pData = NULL, m_szN = 0, m_szNAllocated = 0; }

	BYTE* GetDataPtr()
		{ return m_pData; }

	const BYTE* GetDataPtr() const
		{ return m_pData; }

	void SetN(size_t szN, bool bForceReallocation = false)
	{
		if(!szN)
		{
			delete[] m_pData, m_pData = NULL, m_szN = 0, m_szNAllocated = 0;
			return;
		}

		size_t szNewNAllocated = MinPow2GreaterEq(szN);

		if(szNewNAllocated != m_szNAllocated || bForceReallocation)
		{
			m_szNAllocated = szNewNAllocated;

			BYTE* pOldData = m_pData;

			m_pData = new BYTE[m_szNAllocated];

			memcpy(m_pData, pOldData, Min(m_szN, szN));

			delete[] pOldData;
		}

		m_szN = szN;
	}

	size_t Add(size_t szN)
		{ SetN(m_szN + szN); return m_szN - szN; }

	size_t GetN() const
		{ return m_szN; }
};

#endif // dancer_growable_allocator_h
