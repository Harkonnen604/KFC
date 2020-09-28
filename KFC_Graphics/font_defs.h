#ifndef font_defs_h
#define font_defs_h

#include <KFC_KTL\object_defs.h>
#include "graphics_consts.h"

// Font types
#define FONT_TYPE_FLAT          (1)
#define FONT_TYPE_FLAT_SHADOW   (2)

// Fixed font indices
#define SYSTEM_FONTS_START_INDEX        (0)
#define INTERFACE_FONTS_START_INDEX     (g_GraphicsConsts.m_szNFontsRegistrationManagerFixedEntries >> 3)
#define CUSTOM_FONTS_START_INDEX        (g_GraphicsConsts.m_szNFontsRegistrationManagerFixedEntries >> 1)

// System font indices
#define DEFAULT_FONT_INDEX  (SYSTEM_FONTS_START_INDEX + 0)

#endif // font_defs_h
