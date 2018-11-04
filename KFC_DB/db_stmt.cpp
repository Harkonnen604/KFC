#include "kfc_db_pch.h"
#include "db_stmt.h"

#include "db_device_globals.h"

// -------------
// DB statement
// -------------
T_DB_Statement::T_DB_Statement()
{
	m_bAllocated = false;

	m_hStmt = SQL_NULL_HANDLE;
}

T_DB_Statement::T_DB_Statement(SQLHDBC hCon)
{
	m_bAllocated = false;

	m_hStmt = SQL_NULL_HANDLE;

	Allocate(hCon);
}

T_DB_Statement::T_DB_Statement(SQLHDBC hCon, LPCTSTR pQuery, bool bJustPrepare)
{
	m_bAllocated = false;

	m_hStmt = SQL_NULL_HANDLE;

	Allocate(hCon);

	if(bJustPrepare)
		Prepare(pQuery);
	else
		ExecuteDirect(pQuery);
}

void T_DB_Statement::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		if(m_hStmt != SQL_NULL_HANDLE)
			SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt), m_hStmt = SQL_NULL_HANDLE;
	}
}

void T_DB_Statement::Allocate(SQLHDBC hCon)
{
	Release();

	try
	{
		DEBUG_VERIFY(hCon != SQL_NULL_HANDLE);

		SQLRETURN ret = SQLAllocHandle(	SQL_HANDLE_STMT,
										hCon,
										&m_hStmt);

		if(!SQL_IS_SUCCESS(ret))
		{
			m_hStmt = SQL_NULL_HANDLE;

			INITIATE_DEFINED_FAILURE(TEXT("Error allocating DB statement."));
		}
	
		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

T_DB_Statement& T_DB_Statement::Prepare(LPCSTR pQuery)
{
	DEBUG_VERIFY_ALLOCATION;

	SQLRETURN ret = SQLPrepare(m_hStmt, (SQLCHAR*)pQuery, SQL_NTS);

	if(!SQL_IS_SUCCESS(ret))
	{
		INITIATE_DEFINED_FAILURE(	(KString)TEXT("Error preparing DB statement \"") +
										pQuery +
										TEXT("\"\r\n") +
										GetSQLErrorText(NULL, NULL, m_hStmt));
	}

	return *this;
}

T_DB_Statement& T_DB_Statement::Execute()
{
	DEBUG_VERIFY_ALLOCATION;

	SQLRETURN ret = SQLExecute(m_hStmt);

	if(!SQL_IS_SUCCESS(ret))
		INITIATE_DEFINED_FAILURE(TEXT("Error executing DB statement."));

	return *this;
}

T_DB_Statement& T_DB_Statement::ExecuteDirect(LPCTSTR pQuery)
{
	DEBUG_VERIFY_ALLOCATION;

	SQLRETURN ret = SQLExecDirect(m_hStmt, (SQLCHAR*)pQuery, SQL_NTS);

	if(!SQL_IS_SUCCESS(ret))
	{
		INITIATE_DEFINED_FAILURE(	(KString)TEXT("Error directly executing DB statement \"") +
										pQuery +
										TEXT("\"\r\n") +
										GetSQLErrorText(NULL, NULL, m_hStmt));
	}

	return *this;
}

size_t T_DB_Statement::GetNumCols() const
{
	DEBUG_VERIFY_ALLOCATION;

	SQLSMALLINT Count;

	SQLRETURN ret = SQLNumResultCols(m_hStmt, &Count);
	if(!SQL_IS_SUCCESS(ret))
		INITIATE_DEFINED_FAILURE(TEXT("Error getting number of DB result columns."));

	return (size_t)Count;
}

size_t T_DB_Statement::GetNumRows() const
{
	DEBUG_VERIFY_ALLOCATION;

	SQLINTEGER Count;
	
	SQLRETURN ret = SQLRowCount(m_hStmt, &Count);
	if(!SQL_IS_SUCCESS(ret))
		INITIATE_DEFINED_FAILURE(TEXT("Error getting number of DB result rows."));

	return (size_t)Count;
}

void T_DB_Statement::GetColNames(TArray<KString>& RNames)
{
	DEBUG_VERIFY_ALLOCATION;

	RNames.Clear();

	const size_t szN = GetNumCols();

	size_t i;

	for(i = 1 ; i <= szN ; i++)
	{
		char buf[1024] = "";

		short sz;

		SQLRETURN ret = SQLColAttribute(m_hStmt,
										i,
										SQL_DESC_BASE_COLUMN_NAME,
										buf,
										sizeof(buf),
										&sz,
										NULL);

		if(!SQL_IS_SUCCESS(ret))
			INITIATE_DEFINED_FAILURE(TEXT("Error getting DB result column name."));

		buf[sz] = 0;

		RNames.Add() = buf;
	}
}
