#ifndef sql_inc_h
#define sql_inc_h

#include <KFC_KTL\win_inc.h>
#include <sql.h>
#include <sqlext.h>

#define SQL_IS_SUCCESS(v)   ((v) == SQL_SUCCESS || (v) == SQL_SUCCESS_WITH_INFO)

#endif // sql_inc_h
