#ifndef common_consts_h
#define common_consts_h

#include <KFC_KTL/consts.h>

// Main proc args reporter
#ifdef _MSC_VER
	#define REPORT_MAIN_PROC_ARGS	KFC_NOP
#else // _MSC_VR
	#define REPORT_MAIN_PROC_ARGS \
		do { for(int i = 0 ; i < argc ; i++) g_CommonConsts.m_MainProcArgs << argv[i]; } while(false)
#endif // _MSC_VER

// --------------
// Common consts
// --------------
class TCommonConsts : public TConsts
{
public:
	// Behavior
	bool m_bStartFolderNeeded;
	bool m_bTimeGlobalsNeeded;

	// Strings
	size_t m_szTabSize;

	// Files
	size_t m_szFileCopyBufferSize;

	// Filenames
	KString	m_TempFileNamePrefix;
	KString m_DefinitionsFolderName;
	
	// Global registry
	size_t	m_szRegistryKeyTimeout;
	KString	m_ApplicationRegistryKeyName; // change for the real applications
	KString m_CfgRegistryKeyName;
	KString m_InitialsRegistryKeyName;

	// Shareware timeout settings
	KString m_SharewareInstallTimeRegistryKeyName;		// required to set for sharewares
	KString m_SharewareLastStartTimeRegistryKeyName;	// required to set for sharewares
	KString m_SharewareJustInstalledRegistyKeyName;		// required to set for sharewares
	KString m_SharewareExpiredRegistryKeyName;			// required to set for sharewares
	UINT	m_uiSharewareNonExpiredValue;				// required to set for sharewares
	size_t	m_szSharewareTimeoutHours;					// required to set for sharewares
	
	// Min portion sizes
	size_t m_szDefaultCompoundInterpolatorMinPortionSize;
	size_t m_szDefaultMessageProcessorMinPortionSize;
	size_t m_szDefaultInterpolatorProcessorMinPortionSize;

	// Timelimit
	KString m_FirstStartValueName;
	KString m_LastStartValueName;
	KString m_ExpiredValueName;
	KString m_HackedValueName;

	// Redundancy
	bool m_bSkipBasicCfgInitials;

	// DDE
	#ifdef _MSC_VER
		bool m_bInitDDE;
	#endif // _MSC_VER

	// COM
	#ifdef _MSC_VER
		bool		m_bInitOLE;
		bool		m_bInitCOM;
		bool		m_bInitCOM_GIT;
		kflags_t	m_flCOM_Init;
	#endif // _MSC_VER
	
	// Args
	#ifndef _MSC_VER
		KStrings m_MainProcArgs;
	#endif // _MSC_VER

	// Structured info
	size_t m_szMaxStructuredInfoInclusionDepth;

public:
	TCommonConsts();
};

extern TCommonConsts g_CommonConsts;

#endif // common_consts_h
