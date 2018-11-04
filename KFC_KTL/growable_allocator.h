#ifndef growable_allocator_h
#define growable_allocator_h

// -------------------
// Growable allocator
// -------------------
class TGrowableAllocator
{
public:
/*
	virtual bool IsAllocated() const = 0;

	virtual void Release();

	virtual BYTE* GetDataPtr() = 0;
	
	virtual const BYTE* GetDataPtr() = 0;

	virtual void SetN(size_t szN) = 0;

	virtual size_t Add(size_t szAmt) = 0;

	virtual size_t GetN() const = 0;
*/
};

// -------------------------
// Array growable allocator
// -------------------------
class TArrayGrowableAllocator : public TGrowableAllocator
{
private:
	TArray<BYTE, true> m_Array;

public:
	bool IsAllocated() const
		{ return true; }

	void Release()
		{ m_Array.Clear(); }

	BYTE* GetDataPtr()
		{ return m_Array.GetDataPtr(); }

	const BYTE* GetDataPtr() const
		{ return m_Array.GetDataPtr(); }

	void SetN(size_t szN)
		{ m_Array.SetN(szN); }

	size_t Add(size_t szN)
	{
		m_Array.Add(szN);

		return m_Array.GetN() - szN;
	}

	size_t GetN() const
		{ return m_Array.GetN(); }
};

// ------------------------------
// Zero array growable allocator
// ------------------------------
class TZeroArrayGrowableAllocator : public TGrowableAllocator
{
private:
	TArray<BYTE, true> m_Array;

public:
	bool IsAllocated() const
		{ return true; }

	void Release()
		{ m_Array.Clear(); }

	BYTE* GetDataPtr()
		{ return m_Array.GetDataPtr(); }

	const BYTE* GetDataPtr() const
		{ return m_Array.GetDataPtr(); }

	void SetN(size_t szN)
		{ m_Array.SetNAndZeroNewData(szN); }

	size_t Add(size_t szN)
	{
		memset(&m_Array.Add(szN), 0, szN);

		return m_Array.GetN() - szN;
	}

	size_t GetN() const
		{ return m_Array.GetN(); }
};

// ------------------------------
// Fixed size growable allocator
// ------------------------------
class TFixedSizeGrowableAllocator : public TGrowableAllocator
{
private:
	TArray<BYTE, true> m_Array;

	size_t m_szMaxN;

	size_t m_szN;

public:
	TFixedSizeGrowableAllocator(size_t szMaxN = UINT_MAX)
	{
		m_szMaxN = UINT_MAX;
		
		if(szMaxN != UINT_MAX)
			Allocate(szMaxN);
	}

	~TFixedSizeGrowableAllocator()
		{ Release(); }

	bool IsAllocated() const
		{ return m_szMaxN != UINT_MAX; }

	void Allocate(size_t szMaxN)
	{
		DEBUG_VERIFY(szMaxN != UINT_MAX);

		m_Array.SetN(m_szMaxN = szMaxN);

		m_szN = 0;
	}

	void Release()
	{
		m_szMaxN = UINT_MAX;

		m_Array.Clear();
	}

	BYTE* GetDataPtr()
		{ DEBUG_VERIFY_ALLOCATION; return m_Array.GetDataPtr(); }

	const BYTE* GetDataPtr() const
		{ DEBUG_VERIFY_ALLOCATION; return m_Array.GetDataPtr(); }

	void SetN(size_t szN)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(szN <= m_szMaxN);

		m_szN = szN;
	}

	size_t Add(size_t szN)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(m_szN + szN <= m_szMaxN);

		m_szN += szN;

		return m_Array.GetN() + (m_szN - szN);
	}

	size_t GetN() const
		{ DEBUG_VERIFY_ALLOCATION; return m_szN; }
};

// ---------------
// Direct pointer
// ---------------
template <class t>
class TDirectPtr
{
private:
	t* m_pData;

public:
	TDirectPtr() : m_pData(NULL) {}

	TDirectPtr(t& Data) : m_pData(&Data) {}

	t& operator * () const
		{ return *m_pData; }

	t* operator -> () const
		{ return m_pData; }

	operator bool () const
		{ return m_pData; }
};

// ---------------------
// Direct const pointer
// ---------------------
template <class t>
class TDirectConstPtr
{
private:
	const t* m_pData;

public:
	TDirectConstPtr() : m_pData(NULL) {}

	TDirectConstPtr(const t& Data) : m_pData(&Data) {}

	TDirectConstPtr(TDirectPtr<t> Ptr)
		{ memcpy(this, &Ptr, sizeof(*this)); }

	const t& operator * () const
		{ return *m_pData; }

	const t* operator -> () const
		{ return m_pData; }

	operator bool () const
		{ return m_pData; }
};

// ---------------------------
// Growable allocator pointer
// ---------------------------
#define DECLARE_SPECIFIC_GROWABLE_ALLOCATOR_PTR(AllocatorType, Name) \
	template <class t> \
	class Name \
	{ \
	public: \
		typedef AllocatorType TAllocator; \
\
	private: \
		TAllocator* m_pAllocator; \
\
		size_t m_szOffset; \
\
	public: \
		Name() : m_szOffset(0) {} \
\
		Name(TAllocator& Allocator, size_t szOffset) : \
			m_pAllocator(&Allocator), m_szOffset(szOffset) {} \
\
		Name(TAllocator& Allocator, t* v) : \
			m_pAllocator(&Allocator), \
			m_szOffset(reinterpret_cast<BYTE*>(v) - m_pAllocator->GetDataPtr()) {} \
\
		t& operator * () const \
			{ return *reinterpret_cast<t*>(m_pAllocator->GetDataPtr() + m_szOffset); } \
\
		t* operator -> () const \
			{ return reinterpret_cast<t*>(m_pAllocator->GetDataPtr() + m_szOffset); } \
\
		operator bool () const \
			{ return m_szOffset; } \
	}; \

template <class AllocatorType>
class TGrowableAllocatorPtr
{
public:
	DECLARE_SPECIFIC_GROWABLE_ALLOCATOR_PTR(AllocatorType, _)
};

// ---------------------------------
// Growable allocator const pointer
// ---------------------------------
#define DECLARE_SPECIFIC_GROWABLE_ALLOCATOR_CONST_PTR(AllocatorType, Name) \
	template <class t> \
	class Name \
	{ \
	public: \
		typedef AllocatorType TAllocator; \
\
	private: \
		const TAllocator* m_pAllocator; \
\
		size_t m_szOffset; \
\
	public: \
		Name() : m_szOffset(0) {} \
\
		Name(const TAllocator& Allocator, size_t szOffset) : \
			m_pAllocator(&Allocator), m_szOffset(szOffset) {} \
\
		Name(TAllocator& Allocator, const t* v) : \
			m_pAllocator(&Allocator), \
			m_szOffset(reinterpret_cast<const BYTE*>(v) - m_pAllocator->GetDataPtr()) {} \
\
		const t& operator * () const \
			{ return *reinterpret_cast<t*>(m_pAllocator->GetDataPtr() + m_szOffset); } \
\
		const t* operator -> () const \
			{ return reinterpret_cast<t*>(m_pAllocator->GetDataPtr() + m_szOffset); } \
\
		operator bool () const \
			{ return m_szOffset; } \
	}; \

template <class AllocatorType>
class TGrowableAllocatorConstPtr
{
public:
	DECLARE_SPECIFIC_GROWABLE_ALLOCATOR_CONST_PTR(AllocatorType, _)
};

#endif // growable_allocator_h
