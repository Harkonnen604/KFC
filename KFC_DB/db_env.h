#ifndef db_env_h
#define db_env_h

// ---------------
// DB environment
// ---------------
class T_DB_Environment
{
private:
	bool m_bAllocated;
	
	SQLHENV m_hEnv;

public:
	T_DB_Environment();

	~T_DB_Environment()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	void Allocate();	

	void SetIntAttr(size_t szAttr, size_t szValue);

	void SetStringAttr(size_t szAttr, LPCTSTR pValue);

	SQLHENV GetEnv() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hEnv; }

	operator SQLHENV () const
		{ return GetEnv(); }
};

#endif // db_env_h