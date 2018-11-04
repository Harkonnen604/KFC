#include "kfc_common_pch.h"
#include "common_tokens.h"

#include "common_consts.h"
#include "common_cfg.h"
#include "common_initials.h"
#include "startup_globals.h"

TCommonTokens g_CommonTokens;

// --------------
// Common tokens
// --------------
TCommonTokens::TCommonTokens() : TGlobals(TEXT("Common tokens"))
{
	#ifdef _MSC_VER
		AddSubGlobals(g_CommonCfg);
		AddSubGlobals(g_CommonInitials);
	#endif // MSC_VER

	AddSubGlobals(g_StartupGlobals);
}

void TCommonTokens::OnUninitialize()
{
	m_FileNameTokensRegisterer.Release();

	m_FileNameTokens.Clear();
}

void TCommonTokens::OnInitialize()
{
	// --- Filename tokens ---
	m_FileNameTokensRegisterer.Allocate(FILENAME_TOKENS);

	m_FileNameTokensRegisterer.Add(TEXT("[TempFileNamePrefix]"),	g_CommonConsts.m_TempFileNamePrefix);
	m_FileNameTokensRegisterer.Add(TEXT("[DefinitionsFolder]"),		g_CommonConsts.m_DefinitionsFolderName);
	m_FileNameTokensRegisterer.Add(TEXT("[StartFolder]"),			g_StartupGlobals.m_StartFolder);

	#ifdef _MSC_VER
	{
		m_FileNameTokensRegisterer.Add(TEXT("[WindowsFolder]"),			g_StartupGlobals.m_WindowsFolder);
		m_FileNameTokensRegisterer.Add(TEXT("[SystemFolder]"),			g_StartupGlobals.m_SystemFolder);
		m_FileNameTokensRegisterer.Add(TEXT("[TempFolder]"),			g_StartupGlobals.m_TempFolder);
	}
	#endif // _MSC_VER
}
