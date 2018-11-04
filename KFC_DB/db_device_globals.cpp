#include "kfc_db_pch.h"
#include "db_device_globals.h"

T_DB_DeviceGlobals g_DB_DeviceGlobals;

// ------------------
// DB device globals
// ------------------
T_DB_DeviceGlobals::T_DB_DeviceGlobals() : TGlobals(TEXT("DB device globals"))
{
}

void T_DB_DeviceGlobals::OnUninitialize()
{
	m_Environment.Release();
}

void T_DB_DeviceGlobals::OnInitialize()
{
	m_Environment.Allocate();

	m_Environment.SetIntAttr(SQL_ATTR_ODBC_VERSION, SQL_OV_ODBC3);

	m_Environment.SetIntAttr(SQL_ATTR_OUTPUT_NTS, SQL_TRUE);
}

// ----------------
// Global routines
// ----------------
KString GetSQLErrorText(SQLHENV hEnv, SQLHDBC hCon, SQLHSTMT hStmt)
{
	char buf[512] = "";

	SQLINTEGER err;
	SQLSMALLINT sz = 0;

	SQLError(hEnv, hCon, hStmt, NULL, &err, (SQLCHAR*)buf, sizeof(buf), &sz);

	buf[sz] = 0;

	return buf;
}