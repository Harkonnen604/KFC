#include "kfc_gui_pch.h"
#include "gui_consts.h"

T_GUI_Consts g_GUI_Consts;

// -----------
// GUI consts
// -----------
T_GUI_Consts::T_GUI_Consts()
{
    // Registry
    m_RegistryKeyName = TEXT("GUI\\");

    // Storages
    m_szNGUIFontsRegistrationManagerFixedEntries = 128;

    // Initialization flags
    m_bInitializeGUIFontStorage = false;

    m_bWithCheckIcons = false;
}
