#include "kfc_db_pch.h"
#include "db_con.h"

#include "db_device_globals.h"

// --------------
// DB connection
// --------------
T_DB_Connection::T_DB_Connection(bool bAllocate)
{
    m_bAllocated = false;

    m_hCon = SQL_NULL_HANDLE;

    m_bConnected = false;

    if(bAllocate)
        Allocate();
}

T_DB_Connection::T_DB_Connection(   LPCTSTR pAliasName,
                                    LPCTSTR pLogin,
                                    LPCTSTR pPassword)
{
    m_bAllocated = false;

    m_hCon = SQL_NULL_HANDLE;

    m_bConnected = false;

    Allocate(pAliasName, pLogin, pPassword);
}

void T_DB_Connection::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        if(IsAllocated())
            Disconnect();

        m_bAllocated = false;

        if(m_hCon != SQL_NULL_HANDLE)
            SQLFreeHandle(SQL_HANDLE_DBC, m_hCon), m_hCon = SQL_NULL_HANDLE;

        m_bConnected = false;
    }
}

void T_DB_Connection::Allocate()
{
    Release();

    try
    {
        SQLRETURN ret = SQLAllocHandle( SQL_HANDLE_DBC,
                                        g_DB_DeviceGlobals.m_Environment,
                                        &m_hCon);

        if(!SQL_IS_SUCCESS(ret))
        {
            INITIATE_DEFINED_FAILURE(TEXT("Error allocating DB connection."));
        }

        m_bConnected = false;

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

void T_DB_Connection::Allocate( LPCTSTR pAliasName,
                                LPCTSTR pLogin,
                                LPCTSTR pPassword)
{
    Allocate();

    Connect(pAliasName, pLogin, pPassword);
}

void T_DB_Connection::Disconnect()
{
    DEBUG_VERIFY_ALLOCATION;

    if(!IsConnected())
        return;

    SQLDisconnect(m_hCon);

    m_bConnected = false;
}

void T_DB_Connection::Connect(  LPCTSTR pAliasName,
                                LPCTSTR pLogin,
                                LPCTSTR pPassword)
{
    DEBUG_VERIFY_ALLOCATION;

    SQLRETURN ret = SQLConnect( m_hCon,
                                (SQLCHAR*)pAliasName,
                                _tcslen(pAliasName),
                                (SQLCHAR*)pLogin,
                                _tcslen(pLogin),
                                (SQLCHAR*)pPassword,
                                _tcslen(pPassword));

    if(!SQL_IS_SUCCESS(ret))
    {
        INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error connecting ODBC alias \"") +
                                            pAliasName                                  +
                                            TEXT("\""),
                                        ret);
    }

    m_bConnected = true;
}
