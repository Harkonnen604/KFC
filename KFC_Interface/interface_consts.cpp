#include "kfc_interface_pch.h"
#include "interface_consts.h"

#include <KFC_Input\keyboard_key_defs.h>
#include <KFC_Graphics\color_defs.h>

TInterfaceConsts g_InterfaceConsts;

// -----------------
// Interface consts
// -----------------
TInterfaceConsts::TInterfaceConsts()
{
	// Registry
	m_RegistryKeyName = TEXT("Interface\\");

	// Filenames
	m_InterfacesFolderName = TEXT("Interfaces\\");

	// Hotkeys
	m_szPerformanceHotKey	= DIK_F12;
	m_szScreenShotHotKey	= DIK_SYSRQ;	

	// Hot message
	m_fHotMessageAppearanceDelay	= 200.0f;
	m_fHotMessageShowSpeedCoef		= 1.0f;
	m_fHotMessageHideSpeedCoef		= 2.5f;
	m_HotPointerSpacing.Set(4.0f, 4.0f);

	// Colors
	m_PerformanceColor = WhiteColor(192);

	// Controls
	m_DefaultInteractiveControlTransitionDelays[0] = 100.0f;
	m_DefaultInteractiveControlTransitionDelays[1] = 100.0f;
	m_DefaultInteractiveControlTransitionDelays[2] = 50.0f;	

	m_fDefaultCheckBoxControlCheckTransitionDelay = 50.0f;
}
