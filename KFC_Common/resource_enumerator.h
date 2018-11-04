#ifndef resource_enumerator_h
#define resource_enumerator_h

#ifdef _MSC_VER

// --------------------
// Resource enumerator
// --------------------
class TResourceEnumerator
{
private:
	TArray<size_t, true> m_IDs;

	size_t m_szPos;

	static BOOL CALLBACK StaticEnumProc(HMODULE	hModule,
										LPCTSTR	pType,
										LPTSTR	pName,
										LPARAM	lParam);

public:
	TResourceEnumerator(HMODULE hModule,
						LPCTSTR pType);

	const TArray<size_t, true>& GetIDs() const
		{ return m_IDs; }

	operator bool () const
		{ return m_szPos < m_IDs.GetN(); }

	TResourceEnumerator& operator ++ ()
		{ DEBUG_VERIFY(*this); m_szPos++; return *this; }

	size_t GetID() const
		{ DEBUG_VERIFY(*this); return m_IDs[m_szPos]; }
};

#endif // _MSC_VER

#endif // resource_enumerator_h
