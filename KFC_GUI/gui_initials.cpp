#include "kfc_gui_pch.h"
#include "gui_initials.h"

#include <KFC_Common\common_consts.h>
#include "gui_consts.h"

T_GUI_Initials g_GUI_Initials;

// -------------
// GUI initials
// -------------
T_GUI_Initials::T_GUI_Initials() : TGlobals(TEXT("GUI initials"))
{
}

void T_GUI_Initials::OnUninitialize()
{
}

void T_GUI_Initials::OnInitialize()
{
    if(!g_CommonConsts.m_bSkipBasicCfgInitials)
        Load(), Save();
}

void T_GUI_Initials::LoadItems(KRegistryKey& Key)
{
}

void T_GUI_Initials::SaveItems(KRegistryKey& Key) const
{
}

void T_GUI_Initials::Load()
{
    TAssignmentsList::Load( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_GUI_Consts.m_RegistryKeyName +
                            g_CommonConsts.m_InitialsRegistryKeyName);
}

void T_GUI_Initials::Save() const
{
    TAssignmentsList::Save( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_GUI_Consts.m_RegistryKeyName +
                            g_CommonConsts.m_InitialsRegistryKeyName);
}
