#ifndef gui_consts_h
#define gui_consts_h

#include <KFC_KTL\consts.h>
#include <KFC_KTL\kstring.h>

// -----------
// GUI consts
// -----------
class T_GUI_Consts : public TConsts
{
public:
	T_GUI_Consts();

	// Storages
	size_t m_szNGUIFontsRegistrationManagerFixedEntries;

	// Initialization flags
	bool m_bInitializeGUIFontStorage;
	
	bool m_bWithCheckIcons;
};

extern T_GUI_Consts g_GUI_Consts;

#endif // gui_consts_h
