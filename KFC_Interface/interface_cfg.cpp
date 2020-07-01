#include "kfc_interface_pch.h"
#include "interface_cfg.h"

#include <KFC_Common\common_consts.h>
#include "interface_consts.h"

TInterfaceCfg g_InterfaceCfg;

// --------------
// Interface cfg
// --------------
TInterfaceCfg::TInterfaceCfg() : TGlobals(TEXT("Interface cfg"))
{
}

void TInterfaceCfg::OnUninitialize()
{
}

void TInterfaceCfg::OnInitialize()
{
	if(!g_CommonConsts.m_bSkipBasicCfgInitials)
		Load(), Save();
}

void TInterfaceCfg::LoadItems(KRegistryKey& Key)
{
}

void TInterfaceCfg::SaveItems(KRegistryKey& Key) const
{
}

void TInterfaceCfg::Load()
{
	TAssignmentsList::Load(	g_CommonConsts.m_ApplicationRegistryKeyName +
							g_InterfaceConsts.m_RegistryKeyName +
							g_CommonConsts.m_CfgRegistryKeyName);
}

void TInterfaceCfg::Save() const
{
	TAssignmentsList::Save(	g_CommonConsts.m_ApplicationRegistryKeyName +
							g_InterfaceConsts.m_RegistryKeyName +
							g_CommonConsts.m_CfgRegistryKeyName);
}
