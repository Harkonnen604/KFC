#ifndef interface_consts_h
#define interface_consts_h

#include <KFC_KTL\consts.h>
#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\basic_bitypes.h>
#include <KFC_KTL\kstring.h>
#include <KFC_KTL\array.h>
#include <KFC_Graphics\d3d_color.h>

// -----------------
// Interface consts
// -----------------
class TInterfaceConsts : public TConsts
{
public:
	// Filenames
	KString m_InterfacesFolderName;

	// Hotkeys
	size_t m_szPerformanceHotKey;
	size_t m_szScreenShotHotKey;

	// Hot message
	float m_fHotMessageAppearanceDelay;
	float m_fHotMessageShowSpeedCoef;
	float m_fHotMessageHideSpeedCoef;
	FSIZE m_HotPointerSpacing;

	// Colors
	TD3DColor m_PerformanceColor;

	// Controls
	float m_DefaultInteractiveControlTransitionDelays[3];

	float m_fDefaultCheckBoxControlCheckTransitionDelay;


	TInterfaceConsts();
};

extern TInterfaceConsts g_InterfaceConsts;

#endif // interface_consts_h
