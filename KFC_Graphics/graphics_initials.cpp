#include "kfc_graphics_pch.h"
#include "graphics_initials.h"

#include <KFC_Common\common_consts.h>
#include "graphics_consts.h"

TGraphicsInitials g_GraphicsInitials;

// ------------------
// Graphics initials
// ------------------
TGraphicsInitials::TGraphicsInitials() : TGlobals(TEXT("Graphics initials"))
{
}

void TGraphicsInitials::OnUninitialize()
{
}

void TGraphicsInitials::OnInitialize()
{
    Load(), Save();
}

void TGraphicsInitials::LoadItems(KRegistryKey& Key)
{
}

void TGraphicsInitials::SaveItems(KRegistryKey& Key) const
{
}

void TGraphicsInitials::Load()
{
    TAssignmentsList::Load( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_GraphicsConsts.m_RegistryKeyName +
                            g_CommonConsts.m_InitialsRegistryKeyName);
}

void TGraphicsInitials::Save() const
{
    TAssignmentsList::Save( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_GraphicsConsts.m_RegistryKeyName +
                            g_CommonConsts.m_InitialsRegistryKeyName);
}
