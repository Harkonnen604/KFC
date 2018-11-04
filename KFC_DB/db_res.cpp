#include "kfc_db_pch.h"
#include "db_res.h"

// DB plain result getter def
#define IMPLEMENT_DB_PLAIN_RESULT_GETTER(Type, SQLType, Name)		\
	bool T_DB_Result::Get##Name(size_t szCol, Type& RValue) const	\
	{																\
		DEBUG_VERIFY_ALLOCATION;									\
																	\
		return GetData(szCol, (SQLType), &RValue, sizeof(RValue));	\
	}																\

// DB plain result default getter def
#define IMPLEMENT_DB_RESULT_DEFAULT_GETTER(Type, DefType, Name)							\
	void T_DB_Result::Get##Name(size_t szCol, Type& RValue, DefType DefaultValue) const	\
	{																					\
		DEBUG_VERIFY_ALLOCATION;														\
																						\
		if(!Get##Name(szCol, RValue))													\
			RValue = DefaultValue;														\
	}																					\

// ----------
// DB result
// ----------
T_DB_Result::T_DB_Result()
{
	m_bAllocated = false;

	m_hStmt = SQL_NULL_HANDLE;

	m_bHasData = false;
}

T_DB_Result::T_DB_Result(SQLHSTMT hSStmt)
{
	m_bAllocated = false;

	m_hStmt = SQL_NULL_HANDLE;

	m_bHasData = false;

	Allocate(hSStmt);
}

void T_DB_Result::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		if(m_hStmt != SQL_NULL_HANDLE)
			SQLCloseCursor(m_hStmt), m_hStmt = SQL_NULL_HANDLE;
	}
}

void T_DB_Result::Allocate(SQLHSTMT hSStmt)
{
	Release();

	try
	{
		DEBUG_VERIFY(hSStmt != SQL_NULL_HANDLE);

		m_hStmt = hSStmt;

		m_bHasData = true;		

		Fetch();

		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

void T_DB_Result::Fetch()
{
	if(!m_bHasData)
		return;

	SQLRETURN ret = SQLFetch(m_hStmt);

	if(!SQL_IS_SUCCESS(ret))
	{
		if(ret == SQL_NO_DATA)
		{
			m_bHasData = false;
			return;
		}

		INITIATE_DEFINED_FAILURE(TEXT("Error fetching DB result."));
	}
}

bool T_DB_Result::GetData(	size_t	szCol,
							type_t	tpType,
							void*	pRData,
							size_t	szSize) const
{
	DEBUG_VERIFY_ALLOCATION;

	SQLINTEGER Len;

	SQLRETURN ret = SQLGetData(	m_hStmt,
								(SQLUSMALLINT)	szCol,
								(SQLSMALLINT)	tpType,
								(SQLPOINTER)	pRData,
								(SQLINTEGER)	szSize,
								&Len);

	if(!SQL_IS_SUCCESS(ret))
	{
		INITIATE_DEFINED_FAILURE(	(KString)TEXT("Error getting DB result data from column ") +
										szCol +
										TEXT("."));
	}

	return Len != SQL_NULL_DATA && Len != SQL_NO_TOTAL;
}

// Getters
IMPLEMENT_DB_PLAIN_RESULT_GETTER(__int64,	SQL_C_SBIGINT,	Int64);
IMPLEMENT_DB_PLAIN_RESULT_GETTER(int,		SQL_C_SLONG,	Int);
IMPLEMENT_DB_PLAIN_RESULT_GETTER(long,		SQL_C_SLONG,	Long);
IMPLEMENT_DB_PLAIN_RESULT_GETTER(short,		SQL_C_SSHORT,	Short);
IMPLEMENT_DB_PLAIN_RESULT_GETTER(char,		SQL_C_STINYINT,	Char);

IMPLEMENT_DB_PLAIN_RESULT_GETTER(QWORD,	SQL_C_UBIGINT,	QWORD);
IMPLEMENT_DB_PLAIN_RESULT_GETTER(UINT,	SQL_C_ULONG,	UINT);
IMPLEMENT_DB_PLAIN_RESULT_GETTER(DWORD,	SQL_C_ULONG,	DWORD);
IMPLEMENT_DB_PLAIN_RESULT_GETTER(WORD,	SQL_C_USHORT,	WORD);
IMPLEMENT_DB_PLAIN_RESULT_GETTER(BYTE,	SQL_C_UTINYINT,	BYTE);

IMPLEMENT_DB_PLAIN_RESULT_GETTER(double,	SQL_C_DOUBLE,	Double);
IMPLEMENT_DB_PLAIN_RESULT_GETTER(float,		SQL_C_FLOAT,	Float);

bool T_DB_Result::GetString(size_t szCol, KString& RValue) const
{
	char Data[16384] = "";

	const bool bRet = GetData(szCol, SQL_C_CHAR, Data, sizeof(Data) - 1);

	if(bRet)
		RValue = Data;

	return bRet;
}

// Default getters
IMPLEMENT_DB_RESULT_DEFAULT_GETTER(__int64,	__int64,	Int64);
IMPLEMENT_DB_RESULT_DEFAULT_GETTER(int,		int,		Int);
IMPLEMENT_DB_RESULT_DEFAULT_GETTER(long,	long,		Long);
IMPLEMENT_DB_RESULT_DEFAULT_GETTER(short,	short,		Short);
IMPLEMENT_DB_RESULT_DEFAULT_GETTER(char,	char,		Char);

IMPLEMENT_DB_RESULT_DEFAULT_GETTER(QWORD,	QWORD,	QWORD);
IMPLEMENT_DB_RESULT_DEFAULT_GETTER(UINT,	UINT,	UINT);
IMPLEMENT_DB_RESULT_DEFAULT_GETTER(DWORD,	DWORD,	DWORD);
IMPLEMENT_DB_RESULT_DEFAULT_GETTER(WORD,	WORD,	WORD);
IMPLEMENT_DB_RESULT_DEFAULT_GETTER(BYTE,	BYTE,	BYTE);

IMPLEMENT_DB_RESULT_DEFAULT_GETTER(double,	double,	Double);
IMPLEMENT_DB_RESULT_DEFAULT_GETTER(float,	float,	Float);

IMPLEMENT_DB_RESULT_DEFAULT_GETTER(KString, LPCTSTR, String);

KString T_DB_Result::GetString(size_t szCol)
{
	DEBUG_VERIFY_ALLOCATION;

	KString String;

	GetString(szCol, String);

	return String;
}

void T_DB_Result::GetRow(TArray<KString>& RValues)
{
	DEBUG_VERIFY_ALLOCATION;

	RValues.Clear();

	SQLSMALLINT Count;

	SQLRETURN ret = SQLNumResultCols(m_hStmt, &Count);

	if(!SQL_IS_SUCCESS(ret))
		INITIATE_DEFINED_FAILURE(TEXT("Error getting number of DB result columns."));

	size_t i;

	for(i = 0 ; i < (size_t)Count ; i++)
		RValues.Add() = GetString(i + 1);
}

// Other methods
T_DB_Result& T_DB_Result::ToNext()
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(HasData());

	Fetch();

	return *this;
}

bool T_DB_Result::HasData() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_bHasData;
}

#undef IMPLEMENT_DB_RESULT_DEFAULT_GETTER
#undef IMPLEMENT_DB_PLAIN_RESULT_GETTER
