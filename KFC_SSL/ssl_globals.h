#ifndef kfc_ssl_globals_h
#define kfc_ssl_globals_h

#include <KFC_KTL\module_globals.h>

// ------------
// SSL globals
// ------------
class T_SSL_Globals : public TModuleGlobals
{
public:
	T_SSL_Globals();
};

extern T_SSL_Globals g_SSL_Globals;

#endif // kfc_ssl_globals_h