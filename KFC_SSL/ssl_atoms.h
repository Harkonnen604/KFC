#ifndef ssl_atoms_h
#define ssl_atoms_h

// --------
// RSA key
// --------
class T_RSA_Key
{
private:
	RSA* m_pRSA;

	bool m_bHasPrivate;

public:
	T_RSA_Key();

	T_RSA_Key(LPCTSTR pFileName);

	~T_RSA_Key()
		{ Release(); }

	bool IsAllocated() const
		{ return m_pRSA; }

	void Release();

	// Generation
	void Generate(size_t szLength);

	// Storage
	void LoadFrom_PEM_File(LPCTSTR pFileName);

	void LoadAll(TStream& Stream);

	void LoadPublic(TStream& Stream);	

	void SaveAll(TStream& Stream) const;

	void SavePublic(TStream& Stream) const;

	// Cypher
	void EncryptPublic	(const TArray<BYTE, true>& Data, TStream& Stream) const;
	void EncryptPrivate	(const TArray<BYTE, true>& Data, TStream& Stream) const;

	void DecryptPublic	(TStream& Stream, TArray<BYTE, true>& RData) const;	
	void DecryptPrivate	(TStream& Stream, TArray<BYTE, true>& RData) const;

	// Other
	bool IsPrivateKey() const
		{ DEBUG_VERIFY_ALLOCATION; return m_bHasPrivate; }

	RSA* GetRSA() const
		{ DEBUG_VERIFY_ALLOCATION; return m_pRSA; }

	operator RSA* () const
		{ return GetRSA(); }
};

// -----------------
// X509 certificate
// -----------------
class T_X509_Certificate
{
private:
	X509* m_pX509;

public:
	T_X509_Certificate();

	T_X509_Certificate(LPCTSTR pFileName);

	~T_X509_Certificate()
		{ Release(); }

	bool IsAllocated() const
		{ return m_pX509; }

	void Release();

	void LoadFrom_PEM_File(LPCTSTR pFileName);

	X509* Get_X509() const
		{ DEBUG_VERIFY_ALLOCATION; return m_pX509; }

	operator X509* () const
		{ return Get_X509(); }
};

#endif // ssl_atoms_h