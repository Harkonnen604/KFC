#include "kfc_graphics_pch.h"
#include "graphics_cfg.h"

#include <KFC_Common\common_consts.h>
#include "graphics_consts.h"

TGraphicsCfg g_GraphicsCfg;

// -------------
// Graphics cfg
// -------------
TGraphicsCfg::TGraphicsCfg() : TGlobals(TEXT("Graphics cfg"))
{
}

void TGraphicsCfg::OnUninitialize()
{
}

void TGraphicsCfg::OnInitialize()
{
    Load(), Save();
}

void TGraphicsCfg::LoadItems(KRegistryKey& Key)
{
    Key.ReadUINT(TEXT("Graphics adapter"),          m_szGraphicsAdapter,        D3DADAPTER_DEFAULT);
    Key.ReadUINT(TEXT("Resolution X"),              m_Resolution.cx,            1024);
    Key.ReadUINT(TEXT("Resolution Y"),              m_Resolution.cy,            768);
    Key.ReadUINT(TEXT("Color depth"),               m_szColorDepth,             32);
    Key.ReadBool(TEXT("Use 32 bpp textures"),       m_bUse32BPPTextures,        true);
    Key.ReadBool(TEXT("Use 32 bpp alpha textures"), m_bUse32BPPAlphaTextures,   true);
    Key.ReadUINT(TEXT("Z buffer depth"),            m_szZBufferDepth,           24);
    Key.ReadUINT(TEXT("Stencil buffer depth"),      m_szStencilBufferDepth,     8);
    Key.ReadBool(TEXT("VSync"),                     m_bVSync,                   false);
    Key.ReadBool(TEXT("FullScreen"),                m_bFullScreen,              true);
    Key.ReadBool(TEXT("TrilinearFiltering"),        m_bTrilinearFiltering,      true);
    Key.ReadUINT(TEXT("MaxActiveLights"),           m_szMaxActiveLights,        8);
}

void TGraphicsCfg::SaveItems(KRegistryKey& Key) const
{
    Key.WriteUINT(TEXT("Graphics adapter"),             m_szGraphicsAdapter);
    Key.WriteUINT(TEXT("Resolution X"),                 m_Resolution.cx);
    Key.WriteUINT(TEXT("Resolution Y"),                 m_Resolution.cy);
    Key.WriteUINT(TEXT("Color depth"),                  m_szColorDepth);
    Key.WriteBool(TEXT("Use 32 bpp textures"),          m_bUse32BPPTextures);
    Key.WriteBool(TEXT("Use 32 bpp alpha textures"),    m_bUse32BPPAlphaTextures);
    Key.WriteUINT(TEXT("Z buffer depth"),               m_szZBufferDepth);
    Key.WriteUINT(TEXT("Stencil buffer depth"),         m_szStencilBufferDepth);
    Key.WriteBool(TEXT("VSync"),                        m_bVSync);
    Key.WriteBool(TEXT("FullScreen"),                   m_bFullScreen);
    Key.WriteBool(TEXT("TrilinearFiltering"),           m_bTrilinearFiltering);
    Key.WriteUINT(TEXT("MaxActiveLights"),              m_szMaxActiveLights);
}

void TGraphicsCfg::Load()
{
    TAssignmentsList::Load( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_GraphicsConsts.m_RegistryKeyName +
                            g_CommonConsts.m_CfgRegistryKeyName);
}

void TGraphicsCfg::Save() const
{
    TAssignmentsList::Save( g_CommonConsts.m_ApplicationRegistryKeyName +
                            g_GraphicsConsts.m_RegistryKeyName +
                            g_CommonConsts.m_CfgRegistryKeyName);
}
