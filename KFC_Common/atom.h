#ifndef atom_h
#define atom_h

#ifdef _MSC_VER

// ----------------
// Global routines
// ----------------
bool KGetAtomName(ATOM aAtom, KString& RName);

inline KString KGetAtomName(ATOM aAtom)
{
	KString Name;

	KFC_VERIFY(KGetAtomName(aAtom, Name));

	return Name;
}

bool KGlobalGetAtomName(ATOM aAtom, KString& RName);

inline KString KGlobalGetAtomName(ATOM aAtom)
{
	KString Name;

	KFC_VERIFY(KGetAtomName(aAtom, Name));

	return Name;
}

// -----
// Atom
// -----
class TAtom
{
private:
	ATOM m_aAtom;

private:
	TAtom(const TAtom&);

	TAtom& operator = (const TAtom&);

public:
	TAtom()
	{
		m_aAtom = 0;
	}

	TAtom(ATOM aAtom)
	{
		m_aAtom = 0;

		Allocate(aAtom);		
	}

	TAtom(LPCTSTR pString)
	{
		m_aAtom = 0;

		Allocate(pString);
	}

	~TAtom()
		{ Release(); }

	bool IsAllocated() const
		{ return m_aAtom; }

	void Release()
	{
		if(m_aAtom)
			DeleteAtom(m_aAtom), m_aAtom = 0;
	}

	void Allocate(LPCTSTR pString)
	{
		Release();

		DEBUG_VERIFY(pString);

		m_aAtom = AddAtom(pString);
		KFC_VERIFY(m_aAtom);
	}

	void Allocate(ATOM aAtom)
	{
		Release();

		DEBUG_VERIFY(aAtom);

		m_aAtom = aAtom;
	}

	ATOM GetAtom() const
	{
		DEBUG_VERIFY_ALLOCATION;

		return m_aAtom;
	}

	operator ATOM () const
		{ return GetAtom(); }

	operator KString () const
	{
		DEBUG_VERIFY_ALLOCATION;

		return KGetAtomName(m_aAtom);		
	}
};

// ------------
// Global atom
// ------------
class TGlobalAtom
{
private:
	ATOM m_aAtom;

private:
	TGlobalAtom(const TGlobalAtom&);

	TGlobalAtom& operator = (const TGlobalAtom&);

public:
	TGlobalAtom()
	{
		m_aAtom = 0;
	}

	TGlobalAtom(ATOM aAtom)
	{
		m_aAtom = 0;

		Allocate(aAtom);		
	}

	TGlobalAtom(LPCTSTR pString)
	{
		m_aAtom = 0;

		Allocate(pString);
	}

	~TGlobalAtom()
		{ Release(); }

	bool IsAllocated() const
		{ return m_aAtom; }

	void Release()
	{
		if(m_aAtom)
			GlobalDeleteAtom(m_aAtom), m_aAtom = 0;
	}

	void Allocate(LPCTSTR pString)
	{
		Release();

		DEBUG_VERIFY(pString);

		m_aAtom = GlobalAddAtom(pString);
		KFC_VERIFY(m_aAtom);
	}

	void Allocate(ATOM aAtom)
	{
		Release();

		DEBUG_VERIFY(aAtom);

		m_aAtom = aAtom;
	}

	ATOM GetAtom() const
	{
		DEBUG_VERIFY_ALLOCATION;

		return m_aAtom;
	}

	operator ATOM () const
		{ return GetAtom(); }

	operator KString () const
	{
		DEBUG_VERIFY_ALLOCATION;

		return KGlobalGetAtomName(m_aAtom);
	}
};

#endif // _MSC_VER

#endif // atom_h
