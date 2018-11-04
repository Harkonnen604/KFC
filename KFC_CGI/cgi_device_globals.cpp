#include "kfc_cgi_pch.h"
#include "cgi_device_globals.h"

T_CGI_DeviceGlobals g_CGI_DeviceGlobals;

// -------------------
// CGI device globals
// -------------------
T_CGI_DeviceGlobals::T_CGI_DeviceGlobals() : TGlobals(TEXT("CGI device globals"))
{
}

void T_CGI_DeviceGlobals::OnUninitialize()
{
	m_HTML_TextAreaTokens.	Clear();
	m_HTML_EOL_Tokens.		Clear();
	m_HTML_NoSpaceTokens.	Clear();
	m_HTML_SpaceTokens.		Clear();
	m_HTML_NoSpaceTokens.	Clear();
	m_HTML_Tokens.			Clear();
}

void T_CGI_DeviceGlobals::OnInitialize()
{
	m_HTML_Tokens
		(TEXT("<"),  TEXT("&lt;"))
		(TEXT(">"),  TEXT("&gt;"))
		(TEXT("&"),  TEXT("&amp;"))
		(TEXT("\""), TEXT("&quot;"));

	m_HTML_SpaceTokens
		(TEXT(" "),    TEXT("&nbsp;"))
		(TEXT("\240"), TEXT("&nbsp;")); // 0xA0

	m_HTML_NoSpaceTokens
		(TEXT("\240"), TEXT(" "));

	m_HTML_EOL_Tokens
		(TEXT("\r"), TEXT(""))
		(TEXT("\n"), TEXT("<BR>"));

	m_HTML_TextAreaTokens
		(TEXT("\r"), TEXT(""));

	m_HTML_TextAreaTokens
		(TEXT("\n"), TEXT("&#10;"));
}
