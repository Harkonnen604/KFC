#include "kfc_graphics_pch.h"
#include "graphics_consts.h"

TGraphicsConsts g_GraphicsConsts;

// ----------------
// Graphics consts
// ----------------
TGraphicsConsts::TGraphicsConsts()
{
    // Registry
    m_RegistryKeyName = TEXT("Graphics\\");

    // Filenames
    m_SpritesFolderName         = TEXT("Sprites\\");
    m_FontsFolderName           = TEXT("Fonts\\");
    m_ScreenShotsFolderName     = TEXT("Screenshots\\");
    m_ScreenShotFileNamePrefix  = TEXT("Screenshot");
    m_ScreenShotFileNamePostfix = TEXT(".bmp");

    // Graphics
    m_szMaxGraphicsStripRects = 2048;
    m_DefaultCameraZClipPlanes.Set(0.05f, 100000.0f); // 5cm - 100km
    m_fDefaultCameraFOV = (float)(g_PI * 0.5);

    // Graphics effects
    m_szMaxGraphicsEffectVertices = 4096;

    // Storages
    m_szNFontsRegistrationManagerFixedEntries   = 128;
    m_szNSpritesRegistrationManagerFixedEntries = 1024;
}
