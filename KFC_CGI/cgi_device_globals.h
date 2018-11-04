#ifndef cgi_device_globals_h
#define cgi_device_globals_h

#include <KFC_KTL/globals.h>
#include <KFC_KTL/tokens.h>

// -------------------
// CGI device globals
// -------------------
class T_CGI_DeviceGlobals : public TGlobals
{
private:
	void OnUninitialize	();
	void OnInitialize	();

public:
	TTokens m_HTML_Tokens;
	TTokens m_HTML_SpaceTokens;
	TTokens m_HTML_NoSpaceTokens;
	TTokens m_HTML_EOL_Tokens;
	TTokens m_HTML_TextAreaTokens;


	T_CGI_DeviceGlobals();
};

extern T_CGI_DeviceGlobals g_CGI_DeviceGlobals;

#endif // cgi_device_globals_h
