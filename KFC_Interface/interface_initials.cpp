#include "kfc_interface_pch.h"
#include "interface_initials.h"

#include <KFC_Common\common_consts.h>
#include "interface_consts.h"

TInterfaceInitials g_InterfaceInitials;

// -------------------
// Interface initials
// -------------------
TInterfaceInitials::TInterfaceInitials() : TGlobals(TEXT("Interface initials"))
{
}

void TInterfaceInitials::OnUninitialize()
{
}

void TInterfaceInitials::OnInitialize()
{
    if(!g_CommonConsts.m_bSkipBasicCfgInitials)
        Load(), Save();
}

void TInterfaceInitials::LoadItems(KRegistryKey& Key)
{
}

void TInterfaceInitials::SaveItems(KRegistryKey& Key) const
{
}

void TInterfaceInitials::Load()
{
    TAssignmentsList::Load( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_InterfaceConsts.m_RegistryKeyName +
                            g_CommonConsts.m_InitialsRegistryKeyName);
}

void TInterfaceInitials::Save() const
{
    TAssignmentsList::Save( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_InterfaceConsts.m_RegistryKeyName +
                            g_CommonConsts.m_InitialsRegistryKeyName);
}
