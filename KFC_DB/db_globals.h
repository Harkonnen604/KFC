#ifndef db_globals_h
#define db_globals_h

#include <KFC_KTL\module_globals.h>

// -----------
// DB globals
// -----------
class T_DB_Globals : public TModuleGlobals
{
public:
	T_DB_Globals();
};

extern T_DB_Globals g_DB_Globals;

#endif // db_globals_h