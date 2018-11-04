#ifndef resource_updater_h
#define resource_updater_h

#ifdef _MSC_VER

// -----------------
// Resource updater
// -----------------
class TResourceUpdater
{
private:
	HANDLE m_hHandle;
	
public:
	TResourceUpdater();

	TResourceUpdater(	LPCTSTR	pFileName,
						bool	bDeleteExisting = false);

	~TResourceUpdater() { Release(); }

	bool IsAllocated() const
		{ return m_hHandle; }

	void Release(bool bCommit = true);

	void Allocate(	LPCTSTR	pFileName,
					bool	bDeleteExisting = false);

	void Update(LPCTSTR	pType,
				size_t	szID,
				void*	pData,
				size_t	szLength,
				WORD	wLanguage = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));

	void UpdateFile(LPCTSTR	pType,
					size_t	szID,
					LPCTSTR	pFileName,
					WORD	wLanguage			= MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
					bool	bPrependWithLength	= false,
					BYTE	bXORValue			= 0x00);

	HANDLE GetHandle() const;

	operator HANDLE () const { return GetHandle(); }
};

#endif // _MSC_VER

#endif // resource_updater_h
