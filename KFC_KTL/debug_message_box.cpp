#include "kfc_ktl_pch.h"
#include "debug_message_box.h"

#include "ktl_consts.h"

// ----------------
// Global routines
// ----------------
void DebugMessageBox(LPCTSTR pCaption, LPCTSTR pMessage)
{
	#ifdef _MSC_VER
		if(g_KTL_Consts.m_bConsole)
	#endif // _MSC_VER
	{
		_ftprintf(	g_KTL_Consts.m_bStdoutConsoleErrors ? stdout : stderr,
					TEXT("\"%s\": \"%s\"\n"),
					#ifdef _MSC_VER
						(LPCTSTR)KString(pCaption ? pCaption : TEXT("")).GetCharToOEM(),
						(LPCTSTR)KString(pMessage ? pMessage : TEXT("")).GetCharToOEM()
					#else // _MSC_VER
						pCaption ? pCaption : TEXT(""),
						pMessage ? pMessage : TEXT("")
					#endif // _MSC_VER
					);
	}
	#ifdef _MSC_VER
		else
		{
			MessageBox(	NULL,
						pMessage,
						pCaption,
						MB_OK | MB_ICONSTOP | (g_KTL_Consts.m_bTopmost ? MB_TOPMOST : 0));
		}
	#endif // _MSC_VER
}
