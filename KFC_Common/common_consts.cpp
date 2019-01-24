#include "kfc_common_pch.h"
#include "common_consts.h"

TCommonConsts g_CommonConsts;

// --------------
// Common consts
// --------------
TCommonConsts::TCommonConsts()
{
	// Behavior
	m_bStartFolderNeeded = true;
	m_bTimeGlobalsNeeded = true;

	// Registry
	m_RegistryKeyName = TEXT("Common\\");

	// Strings
	m_szTabSize = 8;

	// Files
	m_szFileCopyBufferSize = 65536;

	// Filenames
	m_TempFileNamePrefix	= TEXT("Temp");
	m_DefinitionsFolderName	= TEXT("Definitions\\");

	// Global registry
	m_ApplicationRegistryKeyName	= TEXT("Software\\ECCM Corp.\\KFC Defaults\\");
	m_CfgRegistryKeyName			= TEXT("Configuration\\");
	m_InitialsRegistryKeyName		= TEXT("Initials\\");

	// Timelimit
	#ifdef _DEBUG
	{
		m_FirstStartValueName	= TEXT("FirstStart");
		m_LastStartValueName	= TEXT("LastStart");
		m_ExpiredValueName		= TEXT("Expired");
		m_HackedValueName		= TEXT("Hacked");
	}
	#else // _DEBUG
	{
		m_FirstStartValueName	= TEXT("{BDA8A2BD-9BCD-4234-B1DE-CD488255DF3B}");
		m_LastStartValueName	= TEXT("{D04C7236-F9B0-4f17-9A45-458A95E87530}");
		m_ExpiredValueName		= TEXT("{52D2C0E7-21EC-4b20-89A0-E1F0E670A63D}");
		m_HackedValueName		= TEXT("{4B93F146-2482-403a-9C22-7E5B7EB8A647}");
	}
	#endif // _DEBUG

	// Redundancy
	m_bSkipBasicCfgInitials = true;

	// DDE
	#ifdef _MSC_VER
	{
		m_bInitDDE = true;
	}
	#endif // _MSC_VER

	// COM
	#ifdef _MSC_VER
	{
		m_bInitOLE		= false;
		m_bInitCOM		= true;
		m_bInitCOM_GIT	= false;
		m_flCOM_Init	= COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY;
	}
	#endif // _MSC_VER

	// Structured info
	m_szMaxStructuredInfoInclusionDepth = 64;
}
