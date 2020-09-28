#include "kfc_db_pch.h"
#include "db_env.h"

// ---------------
// DB environment
// ---------------
T_DB_Environment::T_DB_Environment()
{
    m_bAllocated = false;

    m_hEnv = SQL_NULL_HANDLE;
}

void T_DB_Environment::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        if(m_hEnv != SQL_NULL_HANDLE)
            SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv), m_hEnv = SQL_NULL_HANDLE;
    }
}

void T_DB_Environment::Allocate()
{
    Release();

    try
    {
        SQLRETURN ret = SQLAllocHandle( SQL_HANDLE_ENV,
                                        SQL_NULL_HANDLE,
                                        &m_hEnv);

        if(!SQL_IS_SUCCESS(ret))
        {
            m_hEnv = SQL_NULL_HANDLE;

            INITIATE_DEFINED_FAILURE(TEXT("Error allocating DB environment."));
        }

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

void T_DB_Environment::SetIntAttr(size_t szAttr, size_t szValue)
{
    DEBUG_VERIFY_ALLOCATION;

    SQLRETURN ret = SQLSetEnvAttr(m_hEnv, szAttr, (SQLPOINTER)szValue, 0);

    if(!SQL_IS_SUCCESS(ret))
        INITIATE_DEFINED_FAILURE(TEXT("Unable to set DB environment integer attribute."));
}

void T_DB_Environment::SetStringAttr(size_t szAttr, LPCTSTR pValue)
{
    DEBUG_VERIFY_ALLOCATION;

    SQLRETURN ret = SQLSetEnvAttr(m_hEnv, szAttr, (void*)pValue, _tcslen(pValue));

    if(!SQL_IS_SUCCESS(ret))
        INITIATE_DEFINED_FAILURE(TEXT("Unable to set DB environment string attribute."));
}
