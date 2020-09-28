#ifndef cgi_globals_h
#define cgi_globals_h

#include <KFC_KTL/module_globals.h>

// ------------
// CGI globals
// ------------
class T_CGI_Globals : public TModuleGlobals
{
public:
    T_CGI_Globals();
};

extern T_CGI_Globals g_CGI_Globals;

#endif // cgi_globals_h
