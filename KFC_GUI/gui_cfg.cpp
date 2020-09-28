#include "kfc_gui_pch.h"
#include "gui_cfg.h"

#include <KFC_Common\common_consts.h>
#include "gui_consts.h"

T_GUI_Cfg g_GUI_Cfg;

// --------
// GUI cfg
// --------
T_GUI_Cfg::T_GUI_Cfg() : TGlobals(TEXT("GUI cfg"))
{
}

void T_GUI_Cfg::OnUninitialize()
{
}

void T_GUI_Cfg::OnInitialize()
{
    if(!g_CommonConsts.m_bSkipBasicCfgInitials)
        Load(), Save();
}

void T_GUI_Cfg::LoadItems(KRegistryKey& Key)
{
}

void T_GUI_Cfg::SaveItems(KRegistryKey& Key) const
{
}

void T_GUI_Cfg::Load()
{
    TAssignmentsList::Load( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_GUI_Consts.m_RegistryKeyName +
                            g_CommonConsts.m_CfgRegistryKeyName);
}

void T_GUI_Cfg::Save() const
{
    TAssignmentsList::Save( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_GUI_Consts.m_RegistryKeyName +
                            g_CommonConsts.m_CfgRegistryKeyName);
}
