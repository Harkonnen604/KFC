#ifndef db_device_globals_h
#define db_device_globals_h

#include <KFC_KTL\globals.h>

#include "db_env.h"

// ------------------
// DB device globals
// ------------------
class T_DB_DeviceGlobals : public TGlobals
{
private:
	void OnUninitialize	();
	void OnInitialize	();

public:
	T_DB_Environment m_Environment;

public:
	T_DB_DeviceGlobals();
};

extern T_DB_DeviceGlobals g_DB_DeviceGlobals;

// ----------------
// Global routines
// ----------------
KString GetSQLErrorText(SQLHENV hEnv, SQLHDBC hCon, SQLHSTMT hStmt);

#endif // db_device_globals_h