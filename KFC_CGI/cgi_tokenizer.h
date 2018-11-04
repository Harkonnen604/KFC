#ifndef cgi_tokenizer_h
#define cgi_tokenizer_h

#include <KFC_KTL/tokens.h>
#include <KFC_KTL/file_names.h>
#include <KFC_Common/file.h>
#include "cgi_device_globals.h"

// ----------------
// Global routines
// ----------------
inline KString EncodeForHTML(const KString& Text)
{
	return	g_CGI_DeviceGlobals.m_HTML_SpaceTokens
				(g_CGI_DeviceGlobals.m_HTML_Tokens(Text));
}

inline KString EncodeForHTMLWithoutSpaces(const KString& Text)
{
	if(Text.IsSpaces())
		return g_CGI_DeviceGlobals.m_HTML_SpaceTokens(TEXT(" "));

	return	g_CGI_DeviceGlobals.m_HTML_NoSpaceTokens
				(g_CGI_DeviceGlobals.m_HTML_Tokens(Text));
}

inline KString EncodeForHTMLWithEOL(const KString& Text)
{
	return	g_CGI_DeviceGlobals.m_HTML_EOL_Tokens
				(g_CGI_DeviceGlobals.m_HTML_SpaceTokens
					(g_CGI_DeviceGlobals.m_HTML_Tokens(Text)));
}

inline KString EncodeForHTMLWithEOL_AndWithoutSpaces(const KString& Text)
{
	return	g_CGI_DeviceGlobals.m_HTML_EOL_Tokens
				(EncodeForHTMLWithoutSpaces(Text));
}

inline KString EncodeForHTML_TextArea(const KString& Text)
{
	return	g_CGI_DeviceGlobals.m_HTML_TextAreaTokens
				(g_CGI_DeviceGlobals.m_HTML_Tokens(Text));
}

KString ProcessCGI_TokensPreloaded(const KString& Text, const TTokens* pTokens = NULL, LPCTSTR pBasePath = "");

inline void ShowCGI_TokensPreloaded(const KString& Text, const TTokens *pTokens = NULL, LPCTSTR pBasePath = "")
	{ _tprintf(TEXT("%s"), (LPCTSTR)ProcessCGI_TokensPreloaded(Text, pTokens, pBasePath)); }

inline KString PreloadCGI_Template(LPCTSTR pFileName)
	{ return TFile(pFileName, FOF_BINARYREAD).ReadToString(); }

inline KString ProcessCGI_Tokens(LPCTSTR pFileName, const TTokens* pTokens = NULL)
	{ return ProcessCGI_TokensPreloaded(PreloadCGI_Template(pFileName), pTokens, GetFilePath(pFileName)); }

inline void ShowCGI_Tokens(LPCTSTR pFileName, const TTokens *pTokens = NULL)
	{ return ShowCGI_TokensPreloaded(PreloadCGI_Template(pFileName), pTokens, GetFilePath(pFileName)); }

#endif // cgi_tokenizer_h
