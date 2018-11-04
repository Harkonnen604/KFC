#ifndef db_stmt_h
#define db_stmt_h

// -------------
// DB statement
// -------------
class T_DB_Statement
{
private:
	bool m_bAllocated;

	SQLHSTMT m_hStmt;
	
public:
	T_DB_Statement();

	T_DB_Statement(SQLHDBC hCon);

	T_DB_Statement(SQLHDBC hCon, LPCTSTR pQuery, bool bJustPrepare = false);

	~T_DB_Statement()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	void Allocate(SQLHDBC hCon);

	T_DB_Statement& Prepare(LPCSTR pQuery);

	T_DB_Statement& Execute();

	T_DB_Statement& ExecuteDirect(LPCTSTR pQuery);

	size_t GetNumCols() const;

	size_t GetNumRows() const;

	void GetColNames(TArray<KString>& RNames);

	SQLHSTMT GetStmt() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hStmt; }

	operator SQLHSTMT () const
		{ return GetStmt(); }	
};

#endif // db_stmt_h